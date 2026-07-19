
#ifndef GLTFIO_GLTFENUMS_H
#define GLTFIO_GLTFENUMS_H

#include <dante/IndexBuffer.h>
#include <dante/RenderableManager.h>
#include <dante/TextureSampler.h>
#include <dante/VertexBuffer.h>

#include <cgltf.h>

#define GL_NEAREST                        0x2600
#define GL_LINEAR                         0x2601
#define GL_NEAREST_MIPMAP_NEAREST         0x2700
#define GL_LINEAR_MIPMAP_NEAREST          0x2701
#define GL_NEAREST_MIPMAP_LINEAR          0x2702
#define GL_LINEAR_MIPMAP_LINEAR           0x2703
#define GL_REPEAT                         0x2901
#define GL_MIRRORED_REPEAT                0x8370
#define GL_CLAMP_TO_EDGE                  0x812F

inline dante::TextureSampler::WrapMode getWrapMode(cgltf_int wrap) {
    switch (wrap) {
        case GL_REPEAT:
            return dante::TextureSampler::WrapMode::REPEAT;
        case GL_MIRRORED_REPEAT:
            return dante::TextureSampler::WrapMode::MIRRORED_REPEAT;
        case GL_CLAMP_TO_EDGE:
            return dante::TextureSampler::WrapMode::CLAMP_TO_EDGE;
    }
    return dante::TextureSampler::WrapMode::REPEAT;
}

inline dante::TextureSampler::MinFilter getMinFilter(cgltf_int minFilter) {
    switch (minFilter) {
        case GL_NEAREST:
            return dante::TextureSampler::MinFilter::NEAREST;
        case GL_LINEAR:
            return dante::TextureSampler::MinFilter::LINEAR;
        case GL_NEAREST_MIPMAP_NEAREST:
            return dante::TextureSampler::MinFilter::NEAREST_MIPMAP_NEAREST;
        case GL_LINEAR_MIPMAP_NEAREST:
            return dante::TextureSampler::MinFilter::LINEAR_MIPMAP_NEAREST;
        case GL_NEAREST_MIPMAP_LINEAR:
            return dante::TextureSampler::MinFilter::NEAREST_MIPMAP_LINEAR;
        case GL_LINEAR_MIPMAP_LINEAR:
            return dante::TextureSampler::MinFilter::LINEAR_MIPMAP_LINEAR;
    }
    return dante::TextureSampler::MinFilter::LINEAR_MIPMAP_LINEAR;
}

inline dante::TextureSampler::MagFilter getMagFilter(cgltf_int magFilter) {
    switch (magFilter) {
        case GL_NEAREST:
            return dante::TextureSampler::MagFilter::NEAREST;
        case GL_LINEAR:
            return dante::TextureSampler::MagFilter::LINEAR;
    }
    return dante::TextureSampler::MagFilter::LINEAR;
}

inline bool getVertexAttrType(cgltf_attribute_type atype, dante::VertexAttribute* attrType) {
    switch (atype) {
        case cgltf_attribute_type_position:
            *attrType = dante::VertexAttribute::POSITION;
            return true;
        case cgltf_attribute_type_texcoord:
            *attrType = dante::VertexAttribute::UV0;
            return true;
        case cgltf_attribute_type_color:
            *attrType = dante::VertexAttribute::COLOR;
            return true;
        case cgltf_attribute_type_joints:
            *attrType = dante::VertexAttribute::BONE_INDICES;
            return true;
        case cgltf_attribute_type_weights:
            *attrType = dante::VertexAttribute::BONE_WEIGHTS;
            return true;
        case cgltf_attribute_type_normal:
        case cgltf_attribute_type_tangent:
        default:
            return false;
    }
}

inline bool getCustomVertexAttrType(int8_t customIndex, dante::VertexAttribute* attrType) {
    if (customIndex < 0) {
        return false;
    }
    *attrType = static_cast<dante::VertexAttribute>(
            customIndex + (uint8_t) dante::VertexAttribute::CUSTOM0);
    return true;
}

inline bool getIndexType(cgltf_component_type ctype, dante::IndexBuffer::IndexType* itype) {
    switch (ctype) {
        case cgltf_component_type_r_8u:
        case cgltf_component_type_r_16u:
            *itype = dante::IndexBuffer::IndexType::USHORT;
            return true;
        case cgltf_component_type_r_32u:
            *itype = dante::IndexBuffer::IndexType::UINT;
            return true;
        default:
            break;
    }
    return false;
}

inline bool getPrimitiveType(cgltf_primitive_type in,
        dante::RenderableManager::PrimitiveType* out) {
    switch (in) {
        case cgltf_primitive_type_points:
            *out = dante::RenderableManager::PrimitiveType::POINTS;
            return true;
        case cgltf_primitive_type_lines:
            *out = dante::RenderableManager::PrimitiveType::LINES;
            return true;
        case cgltf_primitive_type_line_strip:
            *out = dante::RenderableManager::PrimitiveType::LINE_STRIP;
            return true;
        case cgltf_primitive_type_triangles:
            *out = dante::RenderableManager::PrimitiveType::TRIANGLES;
            return true;
        case cgltf_primitive_type_triangle_strip:
            *out = dante::RenderableManager::PrimitiveType::TRIANGLE_STRIP;
            return true;
        case cgltf_primitive_type_invalid:
        case cgltf_primitive_type_line_loop:
        case cgltf_primitive_type_triangle_fan:
        case cgltf_primitive_type_max_enum:
            return false;
    }
    return false;
}

// This converts a cgltf component type into a Dante Attribute type.
//
// This function has two out parameters. One result is a safe "permitted type" which we know is
// universally accepted across GPU's and backends, but may require conversion (see Transcoder). The
// other result is the "actual type" which requires no conversion.
//
// Returns false if the given component type is invalid.
inline bool getElementType(cgltf_type type, cgltf_component_type ctype,
        dante::VertexBuffer::AttributeType* permitType,
        dante::VertexBuffer::AttributeType* actualType) {
    switch (type) {
	    case cgltf_type_scalar:
            switch (ctype) {
                case cgltf_component_type_r_8:
                    *permitType = dante::VertexBuffer::AttributeType::BYTE;
                    *actualType = dante::VertexBuffer::AttributeType::BYTE;
                    return true;
                case cgltf_component_type_r_8u:
                    *permitType = dante::VertexBuffer::AttributeType::UBYTE;
                    *actualType = dante::VertexBuffer::AttributeType::UBYTE;
                    return true;
                case cgltf_component_type_r_16:
                    *permitType = dante::VertexBuffer::AttributeType::SHORT;
                    *actualType = dante::VertexBuffer::AttributeType::SHORT;
                    return true;
                case cgltf_component_type_r_16u:
                    *permitType = dante::VertexBuffer::AttributeType::USHORT;
                    *actualType = dante::VertexBuffer::AttributeType::USHORT;
                    return true;
                case cgltf_component_type_r_32u:
                    *permitType = dante::VertexBuffer::AttributeType::UINT;
                    *actualType = dante::VertexBuffer::AttributeType::UINT;
                    return true;
                case cgltf_component_type_r_32f:
                    *permitType = dante::VertexBuffer::AttributeType::FLOAT;
                    *actualType = dante::VertexBuffer::AttributeType::FLOAT;
                    return true;
                default:
                    return false;
            }
            break;
	    case cgltf_type_vec2:
            switch (ctype) {
                case cgltf_component_type_r_8:
                    *permitType = dante::VertexBuffer::AttributeType::BYTE2;
                    *actualType = dante::VertexBuffer::AttributeType::BYTE2;
                    return true;
                case cgltf_component_type_r_8u:
                    *permitType = dante::VertexBuffer::AttributeType::UBYTE2;
                    *actualType = dante::VertexBuffer::AttributeType::UBYTE2;
                    return true;
                case cgltf_component_type_r_16:
                    *permitType = dante::VertexBuffer::AttributeType::SHORT2;
                    *actualType = dante::VertexBuffer::AttributeType::SHORT2;
                    return true;
                case cgltf_component_type_r_16u:
                    *permitType = dante::VertexBuffer::AttributeType::USHORT2;
                    *actualType = dante::VertexBuffer::AttributeType::USHORT2;
                    return true;
                case cgltf_component_type_r_32f:
                    *permitType = dante::VertexBuffer::AttributeType::FLOAT2;
                    *actualType = dante::VertexBuffer::AttributeType::FLOAT2;
                    return true;
                default:
                    return false;
            }
            break;
	    case cgltf_type_vec3:
            switch (ctype) {
                case cgltf_component_type_r_8:
                    *permitType = dante::VertexBuffer::AttributeType::FLOAT3;
                    *actualType = dante::VertexBuffer::AttributeType::BYTE3;
                    return true;
                case cgltf_component_type_r_8u:
                    *permitType = dante::VertexBuffer::AttributeType::FLOAT3;
                    *actualType = dante::VertexBuffer::AttributeType::UBYTE3;
                    return true;
                case cgltf_component_type_r_16:
                    *permitType = dante::VertexBuffer::AttributeType::SHORT3;
                    *actualType = dante::VertexBuffer::AttributeType::SHORT3;
                    return true;
                case cgltf_component_type_r_16u:
                    *permitType = dante::VertexBuffer::AttributeType::FLOAT3;
                    *actualType = dante::VertexBuffer::AttributeType::USHORT3;
                    return true;
                case cgltf_component_type_r_32f:
                    *permitType = dante::VertexBuffer::AttributeType::FLOAT3;
                    *actualType = dante::VertexBuffer::AttributeType::FLOAT3;
                    return true;
                default:
                    return false;
            }
            break;
	    case cgltf_type_vec4:
            switch (ctype) {
                case cgltf_component_type_r_8:
                    *permitType = dante::VertexBuffer::AttributeType::BYTE4;
                    *actualType = dante::VertexBuffer::AttributeType::BYTE4;
                    return true;
                case cgltf_component_type_r_8u:
                    *permitType = dante::VertexBuffer::AttributeType::UBYTE4;
                    *actualType = dante::VertexBuffer::AttributeType::UBYTE4;
                    return true;
                case cgltf_component_type_r_16:
                    *permitType = dante::VertexBuffer::AttributeType::SHORT4;
                    *actualType = dante::VertexBuffer::AttributeType::SHORT4;
                    return true;
                case cgltf_component_type_r_16u:
                    *permitType = dante::VertexBuffer::AttributeType::USHORT4;
                    *actualType = dante::VertexBuffer::AttributeType::USHORT4;
                    return true;
                case cgltf_component_type_r_32f:
                    *permitType = dante::VertexBuffer::AttributeType::FLOAT4;
                    *actualType = dante::VertexBuffer::AttributeType::FLOAT4;
                    return true;
                default:
                    return false;
            }
            break;
        default:
            return false;
    }
    return false;
}

#endif // GLTFIO_GLTFENUMS_H
