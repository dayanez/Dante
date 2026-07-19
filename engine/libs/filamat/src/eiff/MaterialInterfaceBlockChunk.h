
#ifndef TNT_FILAMAT_MAT_INTEFFACE_BLOCK_CHUNK_H
#define TNT_FILAMAT_MAT_INTEFFACE_BLOCK_CHUNK_H

#include "Chunk.h"

#include <backend/Program.h>

#include <utils/CString.h>
#include <utils/FixedCapacityVector.h>

#include <tuple>

#include <stdint.h>

namespace dante {
class SamplerInterfaceBlock;
class BufferInterfaceBlock;
struct SubpassInfo;
struct MaterialConstant;
struct MaterialPushConstant;
} // namespace dante

namespace filamat {

class MaterialUniformInterfaceBlockChunk final : public Chunk {
public:
    explicit MaterialUniformInterfaceBlockChunk(dante::BufferInterfaceBlock const& uib);
    ~MaterialUniformInterfaceBlockChunk() override = default;

private:
    void flatten(Flattener&) override;

    dante::BufferInterfaceBlock const& mUib;
};

// ------------------------------------------------------------------------------------------------

class MaterialSamplerInterfaceBlockChunk final : public Chunk {
public:
    explicit MaterialSamplerInterfaceBlockChunk(dante::SamplerInterfaceBlock const& sib);
    ~MaterialSamplerInterfaceBlockChunk() override = default;

private:
    void flatten(Flattener&) override;

    dante::SamplerInterfaceBlock const& mSib;
};

// ------------------------------------------------------------------------------------------------

class MaterialSubpassInterfaceBlockChunk final : public Chunk {
public:
    explicit MaterialSubpassInterfaceBlockChunk(dante::SubpassInfo const& subpass);
    ~MaterialSubpassInterfaceBlockChunk() override = default;

private:
    void flatten(Flattener&) override;

    dante::SubpassInfo const& mSubpass;
};

// ------------------------------------------------------------------------------------------------

class MaterialConstantParametersChunk final : public Chunk {
public:
    explicit MaterialConstantParametersChunk(
            FixedCapacityVector<dante::MaterialConstant> constants);
    ~MaterialConstantParametersChunk() override = default;

private:
    void flatten(Flattener&) override;

    FixedCapacityVector<dante::MaterialConstant> mConstants;
};

// ------------------------------------------------------------------------------------------------

class MaterialPushConstantParametersChunk final : public Chunk {
public:
    explicit MaterialPushConstantParametersChunk(CString const& structVarName,
            FixedCapacityVector<dante::MaterialPushConstant> constants);
    ~MaterialPushConstantParametersChunk() override = default;

private:
    void flatten(Flattener&) override;

    CString mStructVarName;
    FixedCapacityVector<dante::MaterialPushConstant> mConstants;
};

// ------------------------------------------------------------------------------------------------

class MaterialBindingUniformInfoChunk final : public Chunk {
    using Container = FixedCapacityVector<std::tuple<
            uint8_t, CString, dante::backend::Program::UniformInfo>>;
public:
    explicit MaterialBindingUniformInfoChunk(Container list) noexcept;
    ~MaterialBindingUniformInfoChunk() override = default;

private:
    void flatten(Flattener &) override;

    Container mBindingUniformInfo;
};

// ------------------------------------------------------------------------------------------------

class MaterialAttributesInfoChunk final : public Chunk {
    using Container = FixedCapacityVector<std::pair<CString, uint8_t>>;
public:
    explicit MaterialAttributesInfoChunk(Container list) noexcept;
    ~MaterialAttributesInfoChunk() override = default;

private:
    void flatten(Flattener &) override;

    Container mAttributeInfo;
};

// ------------------------------------------------------------------------------------------------

class MaterialDescriptorBindingsChuck final : public Chunk {
    using Container = dante::SamplerInterfaceBlock;
public:
    explicit MaterialDescriptorBindingsChuck(Container const& sib) noexcept;
    ~MaterialDescriptorBindingsChuck() override = default;

private:
    void flatten(Flattener&) override;

    Container const& mSamplerInterfaceBlock;
};

// ------------------------------------------------------------------------------------------------

class MaterialDescriptorSetLayoutChunk final : public Chunk {
    using Container = dante::SamplerInterfaceBlock;
public:
    explicit MaterialDescriptorSetLayoutChunk(Container const& sib) noexcept;
    ~MaterialDescriptorSetLayoutChunk() override = default;

private:
    void flatten(Flattener&) override;

    Container const& mSamplerInterfaceBlock;
};

} // namespace filamat

#endif // TNT_FILAMAT_MAT_INTEFFACE_BLOCK_CHUNK_H
