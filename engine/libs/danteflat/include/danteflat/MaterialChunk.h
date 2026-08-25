
#ifndef TNT_DANTEMAT_MATERIAL_CHUNK_H
#define TNT_DANTEMAT_MATERIAL_CHUNK_H

#include <private/dante/Variant.h>

#include <dante/MaterialChunkType.h>

#include <danteflat/ChunkContainer.h>
#include <danteflat/Unflattener.h>

#include <backend/DriverEnums.h>

#include <utils/FixedCapacityVector.h>
#include <utils/Invocable.h>

#include <tsl/robin_map.h>

namespace danteflat {

class MaterialChunk {
public:
    using ShaderModel = dante::backend::ShaderModel;
    using ShaderStage = dante::backend::ShaderStage;
    using Variant = dante::Variant;

    explicit MaterialChunk(ChunkContainer const& container);
    ~MaterialChunk() noexcept;

    // call this once after container.parse() has been called
    bool initialize(dantemat::ChunkType materialTag);

    // call this as many times as needed
    // populates "shaderContent" with the requested shader, or returns false on failure.
    bool getShader(ShaderContent& shaderContent, BlobDictionary const& dictionary,
            ShaderModel shaderModel, dante::Variant variant, ShaderStage stage) const;

    uint32_t getShaderCount() const noexcept;

    void visitShaders(utils::Invocable<void(ShaderModel, Variant, ShaderStage)>&& visitor) const;

    bool hasShader(ShaderModel model, Variant variant, ShaderStage stage) const noexcept;

    // Populates a pre-sized vector (matching dictionary size) with the frequency of each index
    // Returns the total exact byte length of the variable-length indices stream.
    size_t getDictionaryOccurrences(std::vector<uint32_t>& outOccurrences) const;

    // These methods are for debugging purposes only (matdbg)
    // @{
    static void decodeKey(uint32_t key,
            ShaderModel* outModel, Variant* outVariant, ShaderStage* outStage);
    const tsl::robin_map<uint32_t, uint32_t>& getOffsets() const { return mOffsets; }
    // @}

private:
    ChunkContainer const& mContainer;
    dantemat::ChunkType mMaterialTag = dantemat::ChunkType::Unknown;
    Unflattener mUnflattener;
    const uint8_t* mBase = nullptr;
    tsl::robin_map<uint32_t, uint32_t> mOffsets;

    uint16_t mSharedStrings = 0;
    uint16_t mVertexStrings = 0;
    uint16_t mFragmentStrings = 0;
    uint16_t mComputeStrings = 0;

    bool getTextShader(Unflattener unflattener,
            BlobDictionary const& dictionary, ShaderContent& shaderContent,
            ShaderModel shaderModel, dante::Variant variant, ShaderStage shaderStage) const;

    bool getBinaryShader(
            BlobDictionary const& dictionary, ShaderContent& shaderContent,
            ShaderModel shaderModel, dante::Variant variant, ShaderStage shaderStage) const;
};

} // namespace dantemat

#endif // TNT_DANTEMAT_MATERIAL_CHUNK_H
