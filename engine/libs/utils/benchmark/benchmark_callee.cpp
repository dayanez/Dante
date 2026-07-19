
#if __has_attribute(visibility)
#    define UTILS_PUBLIC __attribute__((visibility("default")))
#else
#    define UTILS_PUBLIC
#endif

UTILS_PUBLIC
void bar() noexcept {
}
