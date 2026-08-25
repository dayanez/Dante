
#ifndef TNT_GLSLPOSTPROCESSOR_H
#define TNT_GLSLPOSTPROCESSOR_H

#include "ShaderMinifier.h"

#include <private/dante/SamplerInterfaceBlock.h>
#include <private/dante/Variant.h>

#include <dantemat/MaterialBuilder.h>    // for MaterialBuilder:: enums

#include <backend/DriverEnums.h>

#include <utils/FixedCapacityVector.h>

#include <ShaderLang.h>
#include <spirv-tools/optimizer.hpp>

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace dantemat {

using SpirvBlob = std::vector<uint32_t>;
using BindingPointAndSib = std::pair<uint8_t, const dante::SamplerInterfaceBlock*>;
using SibVector = utils::FixedCapacityVector<BindingPointAndSib>;

using DescriptorInfo = std::tuple<
        utils::CString,
        dante::backend::DescriptorSetLayoutDescriptor,
        std::optional<dante::SamplerInterfaceBlock::SamplerInfo>>;
using DescriptorSetInfo = utils::FixedCapacityVector<DescriptorInfo>;
using DescriptorSets = std::array<DescriptorSetInfo, dante::backend::MAX_DESCRIPTOR_SET_COUNT>;

class GLSLPostProcessor {
public:
    enum Flags : uint32_t {
        PRINT_SHADERS = 1 << 0,
        GENERATE_DEBUG_INFO = 1 << 1,
    };

    GLSLPostProcessor(
            MaterialBuilder::Optimization optimization,
            MaterialBuilder::Workarounds workarounds,
            uint32_t flags);

    ~GLSLPostProcessor();

    struct Config {
        dante::Variant variant;
        dante::UserVariantFilterMask variantFilter;
        MaterialBuilder::TargetApi targetApi;
        MaterialBuilder::TargetLanguage targetLanguage;
        MaterialBuilder::Workarounds workarounds;
        dante::backend::ShaderStage shaderType;
        dante::backend::ShaderModel shaderModel;
        dante::backend::FeatureLevel featureLevel;
        dante::MaterialDomain domain;
        const dantemat::MaterialInfo* materialInfo;
        bool hasFramebufferFetch;
        bool usesClipDistance;
        struct {
            std::vector<std::pair<uint32_t, uint32_t>> subpassInputToColorLocation;
        } glsl;
    };

    bool process(const std::string& inputShader, Config const& config,
            std::string* outputGlsl,
            SpirvBlob* outputSpirv,
            std::string* outputMsl,
            std::string* outputWgsl);

    // public so backend_test can also use it
    static void spirvToMsl(const SpirvBlob* spirv, std::string* outMsl,
            dante::backend::ShaderStage stage, dante::backend::ShaderModel shaderModel,
            bool useFramebufferFetch, const DescriptorSets& descriptorSets,
            const ShaderMinifier* minifier);

    static bool spirvToWgsl(SpirvBlob* spirv, std::string* outWsl);

private:
    struct InternalConfig {
        std::string* glslOutput = nullptr;
        SpirvBlob* spirvOutput = nullptr;
        std::string* mslOutput = nullptr;
        std::string* wgslOutput = nullptr;
        EShLanguage shLang = EShLangFragment;
        // use 100 for ES environment, 110 for desktop
         int langVersion = 0;
        ShaderMinifier minifier;
    };

    bool fullOptimization(const glslang::TShader& tShader,
            GLSLPostProcessor::Config const& config, InternalConfig& internalConfig) const;

    bool preprocessOptimization(glslang::TShader& tShader,
            GLSLPostProcessor::Config const& config, InternalConfig& internalConfig) const;

    /**
     * Retrieve an optimizer instance tuned for the given optimization level and shader configuration.
     */
    using OptimizerPtr = std::shared_ptr<spvtools::Optimizer>;

    static OptimizerPtr createOptimizer(
            MaterialBuilder::Optimization optimization,
            Config const& config);

    static OptimizerPtr createEmptyOptimizer();

    static void registerSizePasses(spvtools::Optimizer& optimizer, Config const& config);

    static void registerPerformancePasses(spvtools::Optimizer& optimizer, Config const& config);

    static void optimizeSpirv(OptimizerPtr optimizer, SpirvBlob &spirv);

    static void rebindImageSamplerForWGSL(std::vector<uint32_t>& spirv);

    void fixupClipDistance(SpirvBlob& spirv, GLSLPostProcessor::Config const& config) const;

    const MaterialBuilder::Optimization mOptimization;
    const MaterialBuilder::Workarounds mWorkarounds;
    const bool mPrintShaders;
    const bool mGenerateDebugInfo;
};

} // namespace dantemat

#endif //TNT_GLSLPOSTPROCESSOR_H
