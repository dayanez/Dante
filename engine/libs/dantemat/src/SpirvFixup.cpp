
#include "SpirvFixup.h"

namespace dantemat {

bool fixupClipDistance(std::string& spirvDisassembly) {
    size_t p = spirvDisassembly.find("OpDecorate %dante_gl_ClipDistance Location");
    if (p == std::string::npos) {
        return false;
    }
    size_t lineEnd = spirvDisassembly.find('\n', p);
    if (lineEnd == std::string::npos) {
        lineEnd = spirvDisassembly.size();
    }
    spirvDisassembly.replace(p, lineEnd - p,
            "OpDecorate %dante_gl_ClipDistance BuiltIn ClipDistance");
    return true;
}

} // namespace dantemat
