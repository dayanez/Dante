

#ifndef TNT_MATERIALPARSER_H
#define TNT_MATERIALPARSER_H

#include <unordered_map>

#include "Config.h"

#include <utils/Path.h>
#include <utils/Status.h>

namespace dantemat {
class MaterialBuilder;
}
class TestMaterialParser;

namespace matp {

class JsonishValue;
class MaterialLexeme;

class MaterialParser {
public:
    MaterialParser();
    // Resolves #include directives in the material by looking at the given file structure.
    // Returns the string of resolved material with the state that indicates whether the include
    // resolution was successful.
    std::pair<utils::Status, utils::CString> resolveIncludes(
            const std::unique_ptr<const char[]>& buffer, ssize_t size,
            const utils::Path& materialFilePath,
            bool insertLineDirectives, bool insertLineDirectiveCheck);
    // Parses a string material so that it can be used in MaterialBuilder.
    // Call MaterialBuilder::init before passing in the builder; call MaterialBuilder::build to
    // create dantemat::Package after.
    // When the input shader has #includes, it has to be resolved before calling into parse.
    utils::Status parse(
            dantemat::MaterialBuilder& builder,
            const Config& config, ssize_t& size, std::unique_ptr<const char[]>& buffer);
    // Replaces macro keywords with user specified ones. Must be called before parse.
    utils::Status processTemplateSubstitutions(
            const Config& config, ssize_t& size, std::unique_ptr<const char[]>& buffer);
private:
    friend class ::TestMaterialParser;

    utils::Status parseMaterial(const char* buffer, size_t size,
            dantemat::MaterialBuilder& builder) const noexcept;
    utils::Status processMaterial(const MaterialLexeme&,
            dantemat::MaterialBuilder& builder) const noexcept;
    utils::Status processVertexShader(const MaterialLexeme&,
            dantemat::MaterialBuilder& builder) const noexcept;
    utils::Status processFragmentShader(const MaterialLexeme&,
            dantemat::MaterialBuilder& builder) const noexcept;
    utils::Status processComputeShader(const MaterialLexeme&,
            dantemat::MaterialBuilder& builder) const noexcept;
    utils::Status ignoreLexeme(
            const MaterialLexeme&, dantemat::MaterialBuilder& builder) const noexcept;

    utils::Status parseMaterialAsJSON(const char* buffer, size_t size,
            dantemat::MaterialBuilder& builder) const noexcept;
    utils::Status processMaterialJSON(const JsonishValue*,
            dantemat::MaterialBuilder& builder) const noexcept;
    utils::Status processVertexShaderJSON(const JsonishValue*,
            dantemat::MaterialBuilder& builder) const noexcept;
    utils::Status processFragmentShaderJSON(const JsonishValue*,
            dantemat::MaterialBuilder& builder) const noexcept;
    utils::Status processComputeShaderJSON(const JsonishValue*,
            dantemat::MaterialBuilder& builder) const noexcept;
    utils::Status ignoreLexemeJSON(
            const JsonishValue*, dantemat::MaterialBuilder& builder) const noexcept;
    utils::Status isValidJsonStart(const char* buffer, size_t size) const noexcept;

    utils::Status processMaterialParameters(
            dantemat::MaterialBuilder& builder, const Config& config) const;

    // Member function pointer type, this is used to implement a Command design
    // pattern.
    using MaterialConfigProcessor = utils::Status (MaterialParser::*)
            (const MaterialLexeme&, dantemat::MaterialBuilder& builder) const;
    // Map used to store Command pattern function pointers.
    // Using string_view is generally not recommended in a map, but the string keys are program constants,
    // which guarantees that the strings will outlive lifetime of the map. See MaterialParser.cpp
    std::unordered_map<std::string_view, MaterialConfigProcessor> mConfigProcessor;

    // The same, but for pure JSON syntax
    using MaterialConfigProcessorJSON = utils::Status (MaterialParser::*)
            (const JsonishValue*, dantemat::MaterialBuilder& builder) const;
    std::unordered_map<std::string_view, MaterialConfigProcessorJSON> mConfigProcessorJSON;
};

} // namespace matp


#endif // TNT_MATERIALPARSER_H
