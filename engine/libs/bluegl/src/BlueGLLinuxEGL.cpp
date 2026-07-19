
#include <dlfcn.h>
#include <string.h>

namespace bluegl {

struct Driver {
    void* (*eglGetProcAddress)(const char*);
    void* library;
} g_driver = {nullptr, nullptr};

bool initBinder() {
    const char* library_name = "libEGL.so.1";
    g_driver.library = dlopen(library_name, RTLD_GLOBAL | RTLD_NOW);

    if (!g_driver.library) {
        return false;
    }

    g_driver.eglGetProcAddress = (void *(*)(const char *))
            dlsym(g_driver.library, "eglGetProcAddress");

    return g_driver.eglGetProcAddress;
}

void* loadFunction(const char* name) {
  return (void*) g_driver.eglGetProcAddress((const char*) name);
}

void shutdownBinder() {
    dlclose(g_driver.library);
    memset(&g_driver, 0, sizeof(g_driver));
}

} // namespace bluegl
