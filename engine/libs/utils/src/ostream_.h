
#ifndef TNT_UTILS_OSTREAM__H
#define TNT_UTILS_OSTREAM__H

#include <utils/Mutex.h>
#include <utils/ostream.h>

#include <mutex>
#include <utility>

namespace utils::io {

struct ostream_ {
    Mutex mLock;
    ostream::Buffer mData;
    std::pair<ostream::ConsumerCallback, void*> mConsumer{};
    bool mShowHex = false;
};

} // utils::io

#endif // TNT_UTILS_OSTREAM__H
