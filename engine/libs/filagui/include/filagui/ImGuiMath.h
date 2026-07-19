
#ifndef FILAGUI_IMGUIMATH_H_
#define FILAGUI_IMGUIMATH_H_

#include <imgui.h>

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) {
    return { lhs.x+rhs.x, lhs.y+rhs.y };
}

static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) {
    return { lhs.x-rhs.x, lhs.y-rhs.y };
}

#endif /* FILAGUI_IMGUIMATH_H_ */
