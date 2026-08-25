
#ifndef TNT_DANTE_DETAILS_SHADERGENERATOR_H
#define TNT_DANTE_DETAILS_SHADERGENERATOR_H


#include "MaterialInfo.h"

#include <private/dante/EngineEnums.h>
#include <private/dante/Variant.h>

#include <dante/MaterialEnums.h>

#include <dantemat/MaterialBuilder.h>

#include <backend/DriverEnums.h>

#include <utils/CString.h>
#include <utils/sstream.h>

#include <string>

namespace dantemat {

class CodeGenerator;

class ShaderGenerator {
public:
    ShaderGenerator(
            MaterialBuilder::PropertyList const& properties,
            MaterialBuilder::VariableList const& variables,
            MaterialBuilder::OutputList const& outputs,
            MaterialBuilder::PreprocessorDefineList const& defines,
            MaterialBuilder::ConstantList const& constants,
            MaterialBuilder::PushConstantList const& pushConstants,
            utils::CString const& materialCode,
            size_t lineOffset,
            utils::CString const& materialVertexCode,
            size_t vertexLineOffset,
            MaterialBuilder::MaterialDomain materialDomain) noexcept;

    std::string createSurfaceVertexProgram(dante::backend::ShaderModel shaderModel,
            MaterialBuilder::TargetApi targetApi, MaterialBuilder::TargetLanguage targetLanguage,
            MaterialBuilder::FeatureLevel featureLevel,
            MaterialInfo const& material, dante::Variant variant,
            dante::Interpolation interpolation,
            dante::VertexDomain vertexDomain, uint32_t apiLevel) const noexcept;

    std::string createSurfaceFragmentProgram(dante::backend::ShaderModel shaderModel,
            MaterialBuilder::TargetApi targetApi, MaterialBuilder::TargetLanguage targetLanguage,
            MaterialBuilder::FeatureLevel featureLevel,
            MaterialInfo const& material, dante::Variant variant,
            dante::Interpolation interpolation,
            dante::UserVariantFilterMask variantFilter, uint32_t apiLevel) const noexcept;

    std::string createSurfaceComputeProgram(dante::backend::ShaderModel shaderModel,
            MaterialBuilder::TargetApi targetApi, MaterialBuilder::TargetLanguage targetLanguage,
            MaterialBuilder::FeatureLevel featureLevel,
            MaterialInfo const& material, uint32_t apiLevel) const noexcept;

    /**
     * When a GLSL shader is optimized we run it through an intermediate SPIR-V
     * representation. Unfortunately external samplers cannot be used with SPIR-V
     * at this time, so we must transform them into regular 2D samplers. This
     * fixup step can be used to turn the samplers back into external samplers after
     * the optimizations have been applied.
     */
    static void fixupExternalSamplers(dante::backend::ShaderModel sm, std::string& shader,
            MaterialBuilder::FeatureLevel featureLevel,
            MaterialInfo const& material) noexcept;

private:
    static void generateVertexDomainDefines(utils::io::sstream& out,
            dante::VertexDomain domain) noexcept;

    static void generateSurfaceMaterialVariantProperties(utils::io::sstream& out,
            MaterialBuilder::PropertyList const properties,
            const MaterialBuilder::PreprocessorDefineList& defines) noexcept;

    static void generateSurfaceMaterialVariantDefines(utils::io::sstream& out,
            dante::backend::ShaderStage stage,
            MaterialBuilder::FeatureLevel featureLevel,
            MaterialInfo const& material, dante::Variant variant) noexcept;

    static void generatePostProcessMaterialVariantDefines(utils::io::sstream& out,
            dante::backend::ShaderStage stage,
            MaterialBuilder::FeatureLevel featureLevel,
            MaterialInfo const& material, dante::PostProcessVariant variant) noexcept;

    static void generateUserSpecConstants(
            const CodeGenerator& cg, utils::io::sstream& fs,
            MaterialBuilder::ConstantList const& constants);

    std::string createPostProcessVertexProgram(dante::backend::ShaderModel sm,
            MaterialBuilder::TargetApi targetApi, MaterialBuilder::TargetLanguage targetLanguage,
            MaterialBuilder::FeatureLevel featureLevel,
            MaterialInfo const& material, dante::Variant::type_t variantKey,
            uint32_t apiLevel) const noexcept;

    std::string createPostProcessFragmentProgram(dante::backend::ShaderModel sm,
            MaterialBuilder::TargetApi targetApi, MaterialBuilder::TargetLanguage targetLanguage,
            MaterialBuilder::FeatureLevel featureLevel,
            MaterialInfo const& material, dante::Variant::type_t variantKey,
            uint32_t apiLevel) const noexcept;

    static void appendShader(utils::io::sstream& ss,
            const utils::CString& shader, size_t lineOffset) noexcept;

    static bool hasSkinningOrMorphing(
            dante::Variant variant,
            MaterialBuilder::FeatureLevel featureLevel) noexcept;

    static bool hasStereo(
            dante::Variant variant,
            MaterialBuilder::FeatureLevel featureLevel) noexcept;

    static bool hasLighting(MaterialInfo const& material, dante::Variant variant) noexcept;

    MaterialBuilder::PropertyList mProperties;
    MaterialBuilder::VariableList mVariables;
    MaterialBuilder::OutputList mOutputs;
    MaterialBuilder::MaterialDomain mMaterialDomain;
    MaterialBuilder::PreprocessorDefineList mDefines;
    MaterialBuilder::ConstantList mConstants;
    MaterialBuilder::PushConstantList mPushConstants;
    utils::CString mMaterialFragmentCode;   // fragment or compute code
    utils::CString mMaterialVertexCode;
    size_t mMaterialLineOffset;
    size_t mMaterialVertexLineOffset;
    bool mIsMaterialVertexShaderEmpty;
};

} // namespace dante

#endif // TNT_DANTE_DETAILS_SHADERGENERATOR_H
