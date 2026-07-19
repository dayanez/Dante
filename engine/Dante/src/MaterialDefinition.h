#ifndef TNT_DANTE_MATERIALDEFINITION_H
#define TNT_DANTE_MATERIALDEFINITION_H

#include "ProgramSpecialization.h"

#include "ds/DescriptorSetLayout.h"

#include <private/dante/BufferInterfaceBlock.h>
#include <private/dante/ConstantInfo.h>
#include <private/dante/SamplerInterfaceBlock.h>
#include <private/dante/SubpassInfo.h>
#include <private/dante/Variant.h>

#include <backend/DriverApiForward.h>
#include <backend/DriverEnums.h>
#include <backend/Program.h>

#include <tsl/robin_set.h>

namespace dante {

class FEngine;
class MaterialParser;

/** A MaterialDefinition is a parsed, unmarshalled material file, containing no state.
 *
 * Given that this is a pure read-only class, nearly all members are public without getters.
 */
struct MaterialDefinition {
    using BlendingMode = dante::BlendingMode;
    using Shading = dante::Shading;
    using Interpolation = dante::Interpolation;
    using VertexDomain = dante::VertexDomain;
    using TransparencyMode = dante::TransparencyMode;
    using CullingMode = backend::CullingMode;

    using AttributeInfoContainer = utils::FixedCapacityVector<std::pair<utils::CString, uint8_t>>;

    using BindingUniformInfoContainer = utils::FixedCapacityVector<
        std::tuple<uint8_t, utils::CString, backend::Program::UniformInfo>>;

    // public only due to std::make_unique().
    MaterialDefinition(FEngine& engine, std::unique_ptr<MaterialParser> parser);

    // Free GPU resources owned by this MaterialDefinition.
    void terminate(FEngine& engine);

    // Creates the program for the material's given variant at the backend level. This function
    // unconditionally compiles the program, ignoring the cache.
    backend::Handle<backend::HwProgram> compileProgram(FEngine& engine,
            MaterialParser const& parser, ProgramSpecialization const& specialization,
            backend::CompilerPriorityQueue priorityQueue) const noexcept;

    // Returns a compiled program. It first consults MaterialCache, and only calls compileProgram()
    // when there is a cache miss.
    //
    // Must be called outside of backend render pass.
    // Must be called before Material::getProgram().
    backend::Handle<backend::HwProgram> prepareProgram(FEngine& engine, backend::DriverApi& driver,
            MaterialParser const& parser, ProgramSpecialization const& specialization,
            backend::CompilerPriorityQueue priorityQueue) const;

    void acquirePrograms(FEngine& engine,
            utils::Slice<backend::Handle<backend::HwProgram>> programCache,
            MaterialParser const& parser,
            utils::Slice<const backend::Program::SpecializationConstant> specializationConstants,
            bool isDefaultMaterial) const;

    void releasePrograms(FEngine& engine,
            utils::Slice<backend::Handle<backend::HwProgram>> programCache,
            MaterialParser const& parser,
            utils::Slice<const backend::Program::SpecializationConstant> specializationConstants,
            bool isDefaultMaterial) const;

    MaterialParser const& getMaterialParser() const noexcept { return *mMaterialParser; }

    utils::Slice<const Variant> getVariants() const noexcept;
    utils::Slice<const Variant> getDepthVariants() const noexcept;

    // Checks whether a given combination of Variant and DynamicSpecConstKey produces a valid and
    // compiled shader program for this material under the specified shader model and platform
    // capabilities.
    bool isValidProgram(Variant const variant, DynamicSpecConstKey const specKey,
            backend::ShaderModel const sm, bool isStereoSupported) const noexcept;

    backend::DescriptorSetLayout const& getPerViewDescriptorSetLayoutDescription(
            Variant const variant, bool useS2dDescriptorSetLayout) const noexcept;

    // Keep track of the definitions of the descriptor set layouts, as these
    // may be used by some backends in parallel compilation of programs.
    backend::DescriptorSetLayout perViewDescriptorSetLayoutPcfDescription;
    backend::DescriptorSetLayout perViewDescriptorSetLayoutS2dDescription;
    backend::DescriptorSetLayout descriptorSetLayoutDescription;

    // try to order by frequency of use
    dante::DescriptorSetLayout perViewDescriptorSetLayoutPcf;
    dante::DescriptorSetLayout perViewDescriptorSetLayoutS2d;
    dante::DescriptorSetLayout descriptorSetLayout;
    backend::Program::DescriptorSetInfo programDescriptorBindings;

    backend::RasterState rasterState;
    TransparencyMode transparencyMode = TransparencyMode::DEFAULT;
    bool isVariantLit = false;
    backend::FeatureLevel featureLevel = backend::FeatureLevel::FEATURE_LEVEL_1;
    Shading shading = Shading::UNLIT;

    BlendingMode blendingMode = BlendingMode::OPAQUE;
    std::array<backend::BlendFunction, 4> customBlendFunctions = {};
    Interpolation interpolation = Interpolation::SMOOTH;
    VertexDomain vertexDomain = VertexDomain::OBJECT;
    MaterialDomain materialDomain = MaterialDomain::SURFACE;
    CullingMode cullingMode = CullingMode::NONE;
    AttributeBitset requiredAttributes;
    UserVariantFilterMask variantFilterMask = 0;
    RefractionMode refractionMode = RefractionMode::NONE;
    RefractionType refractionType = RefractionType::SOLID;
    ReflectionMode reflectionMode = ReflectionMode::DEFAULT;
    uint64_t materialProperties = 0;
    uint8_t perViewLayoutIndex = 0;

    float maskThreshold = 0.4f;
    float specularAntiAliasingVariance = 0.0f;
    float specularAntiAliasingThreshold = 0.0f;

    bool doubleSided = false;
    bool doubleSidedCapability = false;
    bool hasShadowMultiplier = false;
    bool hasCustomDepthShader = false;
    bool specularAntiAliasing = false;

    SamplerInterfaceBlock samplerInterfaceBlock;
    BufferInterfaceBlock uniformInterfaceBlock;
    SubpassInfo subpassInfo;
    tsl::robin_set<std::string_view> parameterNames;

    BindingUniformInfoContainer bindingUniformInfo;
    AttributeInfoContainer attributeInfo;

    // Constants defined by this material. Does not include reserved constants.
    utils::FixedCapacityVector<MaterialConstant> materialConstants;
    // A map from the Constant name to the materialConstants index.
    std::unordered_map<std::string_view, uint32_t> specializationConstantsNameToIndex;
    // A list of default values for spec constants. Includes reserved constants.
    utils::FixedCapacityVector<backend::Program::SpecializationConstant> specializationConstants;

    // current push constants for the HwProgram
    std::array<utils::FixedCapacityVector<backend::Program::PushConstant>,
            backend::Program::SHADER_TYPE_COUNT>
            pushConstants;

    utils::CString name;
    uint64_t cacheId = 0;
    utils::CString source;

private:
    friend class MaterialCache;
    friend class FMaterial; // for onEditCallback

    static std::unique_ptr<MaterialParser> createParser(backend::Backend const backend,
            utils::FixedCapacityVector<backend::ShaderLanguage> languages,
            const void* UTILS_NONNULL data, size_t size);

    static std::unique_ptr<MaterialDefinition> create(FEngine& engine,
            std::unique_ptr<MaterialParser> parser);

    void processMain();
    void processParameterNames();
    void processBlendingMode();
    void processSpecializationConstants(FEngine& engine);
    void processPushConstants();
    void processDescriptorSets(FEngine& engine);

    backend::Program getSurfaceProgram(FEngine& engine, MaterialParser const& parser,
            ProgramSpecialization const& specialization) const noexcept;

    backend::Program getProgramWithVariants(FEngine const& engine, MaterialParser const& parser,
            ProgramSpecialization const& specialization, Variant vertexVariant,
            Variant fragmentVariant) const;

    std::unique_ptr<MaterialParser> mMaterialParser;
};

} // namespace dante

#endif  // TNT_DANTE_MATERIALDEFINITION_H
