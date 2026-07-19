
#ifndef TNT_UTILS_GENERIC_THERMALMANAGER_H
#define TNT_UTILS_GENERIC_THERMALMANAGER_H

#include <stdint.h>

namespace utils {

class ThermalManager {
public:
    enum class ThermalStatus : int8_t {
        ERROR = -1,
        NONE,
        LIGHT,
        MODERATE,
        SEVERE,
        CRITICAL,
        EMERGENCY,
        SHUTDOWN
    };

    ThermalManager() = default;

    // Movable
    ThermalManager(ThermalManager&& rhs) noexcept = default;
    ThermalManager& operator=(ThermalManager&& rhs) noexcept = default;

    // not copiable
    ThermalManager(ThermalManager const& rhs) = delete;
    ThermalManager& operator=(ThermalManager const& rhs) = delete;

    ThermalStatus getCurrentThermalStatus() const noexcept {
        return ThermalStatus::NONE;
    }
};

} // namespace utils

#endif // TNT_UTILS_GENERIC_THERMALMANAGER_H
