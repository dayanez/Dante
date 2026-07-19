
#ifndef TNT_FILAMAT_SHADER_ENTRY_H
#define TNT_FILAMAT_SHADER_ENTRY_H

#include <private/dante/Variant.h>

#include <backend/DriverEnums.h>

#include <string>
#include <vector>

namespace filamat {

// TextEntry stores a shader in ASCII text format, like GLSL.
struct TextEntry {
    dante::backend::ShaderModel shaderModel;
    dante::Variant variant;
    dante::backend::ShaderStage stage;
    std::string shader;
};

struct BinaryEntry {
    dante::backend::ShaderModel shaderModel;
    dante::Variant variant;
    dante::backend::ShaderStage stage;
    size_t dictionaryIndex;  // maps to an index in the blob dictionary

    // temporarily holds this entry's binary data until added to the dictionary
    std::vector<uint8_t> data;
};

}  // namespace filamat

#endif // TNT_FILAMAT_SHADER_ENTRY_H
