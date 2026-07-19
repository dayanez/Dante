
//! \file

#ifndef TNT_DANTE_DEBUGREGISTRY_H
#define TNT_DANTE_DEBUGREGISTRY_H

#include <dante/DanteAPI.h>

#include <utils/compiler.h>

#include <math/mathfwd.h>

#include <stddef.h>

namespace dante {

/**
 * A registry of runtime properties used exclusively for debugging
 *
 * Dante exposes a few properties that can be queried and set, which control certain debugging
 * features of the engine. These properties can be set at runtime at anytime.
 *
 */
class UTILS_PUBLIC DebugRegistry : public DanteAPI {
public:

    /**
     * Queries whether a property exists
     * @param name The name of the property to query
     * @return true if the property exists, false otherwise
     */
    bool hasProperty(const char* UTILS_NONNULL name) const noexcept;

    /**
     * Queries the address of a property's data from its name
     * @param name Name of the property we want the data address of
     * @return Address of the data of the \p name property
     * @{
     */
    void* UTILS_NULLABLE getPropertyAddress(const char* UTILS_NONNULL name);

    void const* UTILS_NULLABLE getPropertyAddress(const char* UTILS_NONNULL name) const noexcept;

    template<typename T>
    inline T* UTILS_NULLABLE getPropertyAddress(const char* UTILS_NONNULL name) {
        return static_cast<T*>(getPropertyAddress(name));
    }

    template<typename T>
    inline T const* UTILS_NULLABLE getPropertyAddress(const char* UTILS_NONNULL name) const noexcept {
        return static_cast<T*>(getPropertyAddress(name));
    }

    template<typename T>
    inline bool getPropertyAddress(const char* UTILS_NONNULL name,
            T* UTILS_NULLABLE* UTILS_NONNULL p) {
        *p = getPropertyAddress<T>(name);
        return *p != nullptr;
    }

    template<typename T>
    inline bool getPropertyAddress(const char* UTILS_NONNULL name,
            T* const UTILS_NULLABLE* UTILS_NONNULL p) const noexcept {
        *p = getPropertyAddress<T>(name);
        return *p != nullptr;
    }
    /** @}*/

    /**
     * Set the value of a property
     * @param name Name of the property to set the value of
     * @param v Value to set
     * @return true if the operation was successful, false otherwise.
     * @{
     */
    bool setProperty(const char* UTILS_NONNULL name, bool v) noexcept;
    bool setProperty(const char* UTILS_NONNULL name, int v) noexcept;
    bool setProperty(const char* UTILS_NONNULL name, float v) noexcept;
    bool setProperty(const char* UTILS_NONNULL name, math::float2 v) noexcept;
    bool setProperty(const char* UTILS_NONNULL name, math::float3 v) noexcept;
    bool setProperty(const char* UTILS_NONNULL name, math::float4 v) noexcept;
    /** @}*/

    /**
     * Get the value of a property
     * @param name Name of the property to get the value of
     * @param v A pointer to a variable which will hold the result
     * @return true if the call was successful and \p v was updated
     * @{
     */
    bool getProperty(const char* UTILS_NONNULL name, bool* UTILS_NONNULL v) const noexcept;
    bool getProperty(const char* UTILS_NONNULL name, int* UTILS_NONNULL v) const noexcept;
    bool getProperty(const char* UTILS_NONNULL name, float* UTILS_NONNULL v) const noexcept;
    bool getProperty(const char* UTILS_NONNULL name, math::float2* UTILS_NONNULL v) const noexcept;
    bool getProperty(const char* UTILS_NONNULL name, math::float3* UTILS_NONNULL v) const noexcept;
    bool getProperty(const char* UTILS_NONNULL name, math::float4* UTILS_NONNULL v) const noexcept;
    /** @}*/

    struct DataSource {
        void const* UTILS_NULLABLE data;
        size_t count;
    };

    DataSource getDataSource(const char* UTILS_NONNULL name) const noexcept;

    struct FrameHistory {
        using duration_ms = float;
        duration_ms target{};
        duration_ms targetWithHeadroom{};
        duration_ms frameTime{};
        duration_ms frameTimeDenoised{};
        float scale = 1.0f;
        float pid_e = 0.0f;
        float pid_i = 0.0f;
        float pid_d = 0.0f;
    };

protected:
    // prevent heap allocation
    ~DebugRegistry() = default;
};


} // namespace dante

#endif /* TNT_DANTE_DEBUGREGISTRY_H */
