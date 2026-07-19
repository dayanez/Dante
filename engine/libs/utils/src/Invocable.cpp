
#include <utils/Invocable.h>
#include <utils/ostream.h>

namespace utils {
io::ostream& InvocableBase::printInvocable(io::ostream& out, const char* name) {
    return out << name;
}
} // namespace utils
