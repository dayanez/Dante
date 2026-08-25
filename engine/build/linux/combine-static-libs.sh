#!/usr/bin/env bash
set -e

SELF_NAME=$(basename "$0")

function print_help {
    echo "$SELF_NAME. Combine multiple static libraries using an archiver tool."
    echo ""
    echo "Usage:"
    echo "    $SELF_NAME <path-to-ar> <output-archive> <archives>..."
    echo ""
    echo "Notes:"
    echo "    <archives> must be a list of absolute paths to static library archives."
    echo "    Mach-O universal binaries are supported, but all archives must be homogeneous: either"
    echo "    all universal, or none."
    echo "    This script creates temporary working directories inside the current directory."
}

if [[ "$#" -lt 3 ]]; then
    print_help
    exit 1
fi

AR_TOOL="$1"
shift
OUTPUT_PATH="$1"
shift
ARCHIVES=("$@")

function combine_static_libs {
    local output_path="$1"
    shift
    local archives=("$@")

    # Create a temporary working directory named after the destination library.
    local temp_dir="${output_path##*/}"   # remove leading path, if any
    temp_dir="_${temp_dir%.a}"            # remove '.a' extension
    mkdir -p "${temp_dir}"

    pushd "${temp_dir}" >/dev/null

    for a in "${archives[@]}"; do
        # Create a separate directory for each archive to combine. This is necessary to avoid
        # overriding object files from seprate archives that have the same name.
        dir_name="${a##*/}"         # remove leading path, if any
        dir_name="${dir_name%.a}"   # remove '.a' extension
        mkdir -p "${dir_name}"

        pushd "${dir_name}" >/dev/null

        # Extract object files from each archive.
        "${AR_TOOL}" -x "$a"

        # Prepend the library name to the object file to ensure each object file has a unique name.
        # Object files are named *.o on Linux/macOS, but *.obj on Windows (even when the
        # compiler is a non-MSVC, GNU-target toolchain like zig cc) - nullglob so a pattern
        # with no matches (e.g. no .obj files on Linux) expands to nothing instead of itself.
        shopt -s nullglob
        for o in *.o *.obj; do
            mv "$o" "${dir_name}_${o}"
        done
        shopt -u nullglob

        popd >/dev/null
    done

    popd >/dev/null

    # Combine the library files into a single static library archive. Batched (rather than one
    # "ar qc output <every object>" call) because a target that pulls in enough dependencies
    # (e.g. dantemat's glslang+spirv-tools+spirv-cross chain) can produce an object list long
    # enough to exceed Windows' ~32K CreateProcess command-line limit in a single invocation.
    rm -f "${output_path}"
    all_objects=()
    while IFS=  read -r -d $'\0'; do
        all_objects+=("$REPLY")
    done < <(find "${temp_dir}" \( -iname '*.o' -o -iname '*.obj' \) -print0)
    batch_size=150
    for ((i = 0; i < ${#all_objects[@]}; i += batch_size)); do
        "${AR_TOOL}" -qc "${output_path}" "${all_objects[@]:i:batch_size}"
    done

    # Clean up. Delete objects in the temporary working directory. In theory we could leave the
    # directory as a "cache" of extracted object files, but it complicates the logic to handle cases
    # where object files are removed from a library.
    find "${temp_dir}" \( -iname "*.o" -o -iname "*.obj" \) -delete
}

function universal_error {
    echo "Error: archives passed to $SELF_NAME must be either all universal, or all non-universal."
    echo "Archives: ${ARCHIVES[*]}"
    exit 1
}

# Determine if the archives are Mach-O universal binaries.
has_universal=""
for a in "${ARCHIVES[@]}"; do
    if file "$a" | grep -q 'Mach-O universal'; then
        if [[ "${has_universal}" == "false" ]]; then
            universal_error
        fi
        has_universal=true
    else
        if [[ "${has_universal}" == "true" ]]; then
            universal_error
        fi
        has_universal=false
    fi
done

if [[ "${has_universal}" == "true" ]]; then
    # If the archives are universal libraries, we have to do some extra work:
    # - "thin" each archive into its individual architectures
    # - for each architecture, combine the single-architecture static libraries
    # - create a final universal binary from the static libraries

    archs_temp_dir="${OUTPUT_PATH##*/}"               # remove leading path, if any
    archs_temp_dir="_${archs_temp_dir%.a}_archs"      # remove '.a' extension

    mkdir -p "${archs_temp_dir}"
    pushd "${archs_temp_dir}" >/dev/null

    # Thin each archive into its architectures.
    for a in "${ARCHIVES[@]}"; do
        # Determine which architectures are present in the archive.
        archs=$(lipo -archs "${a}")

        for arch in ${archs}; do
            mkdir -p "${arch}"
            lipo -thin "${arch}" -output "${arch}/${a##*/}" "$a"
        done
    done

    popd >/dev/null

    # Combine each set of single-architecture archives.
    arch_outputs=()
    for arch_dir in "${archs_temp_dir}"/*/ ; do
        arch=$(basename "$arch_dir")

        arch_output="${OUTPUT_PATH%.a}_${arch}.a"
        arch_outputs+=("$arch_output")

        archives=()
        while IFS=  read -r -d $'\0'; do
            archives+=("$REPLY")
        done < <(find "$(pwd)/${archs_temp_dir}/${arch}" -iname '*.a' -print0)

        combine_static_libs "$arch_output" "${archives[@]}"
    done

    # Finally, combine the single-architecture archives into a universal binary.
    lipo -create "${arch_outputs[@]}" -output "${OUTPUT_PATH}"

    # Clean up.
    rm "${arch_outputs[@]}"
    find "${archs_temp_dir}" -iname "*.a" -delete

else

    combine_static_libs "${OUTPUT_PATH}" "${ARCHIVES[@]}"

fi
