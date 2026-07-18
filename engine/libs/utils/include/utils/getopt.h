/*
 * Copyright (C) 2026 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef UTILS_GETOPT_H
#define UTILS_GETOPT_H

// If system has getopt, we just include it
#if defined(HAS_SYSTEM_GETOPT)
#include <getopt.h>
#else
#include <getopt/getopt.h>
#endif

#ifdef __cplusplus

#undef no_argument
#undef required_argument
#undef optional_argument

// This is an aliasing of getopt to prevent compilation conflicts with the system getop (if
// it exists). Please refer to third_party/getopt for API details.
namespace utils {
namespace getopt {
    using ::getopt;
    using ::optarg;
    using ::optind;
    using ::opterr;
    using ::optopt;
    using ::option;
    using ::getopt_long;
    using ::getopt_long_only;

    constexpr int no_argument = 0;
    constexpr int required_argument = 1;
    constexpr int optional_argument = 2;
}
}
#endif // __cplusplus

#endif // UTILS_GETOPT_H
