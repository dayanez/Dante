
#ifndef TNT_DANTE_FG_BLACKBOARD_H
#define TNT_DANTE_FG_BLACKBOARD_H

#include "fg/FrameGraphId.h"

#include <string_view>
#include <unordered_map>

namespace dante {

class Blackboard {
    using Container = std::unordered_map<
            std::string_view,
            FrameGraphHandle>;

public:
    Blackboard() noexcept;
    ~Blackboard() noexcept;

    FrameGraphHandle& operator [](std::string_view name) noexcept;

    void put(std::string_view name, FrameGraphHandle handle) noexcept;

    template<typename T>
    FrameGraphId<T> get(std::string_view&& name) const noexcept {
        return static_cast<FrameGraphId<T>>(getHandle(std::forward<std::string_view>(name)));
    }

    void remove(std::string_view name) noexcept;

private:
    FrameGraphHandle getHandle(std::string_view name) const noexcept;
    Container mMap;
};

} // namespace dante


#endif //TNT_DANTE_FG_BLACKBOARD_H
