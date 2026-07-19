
#ifndef TNT_DANTE_DRIVER_ANDROID_VIRTUAL_MACHINE_ENV_H
#define TNT_DANTE_DRIVER_ANDROID_VIRTUAL_MACHINE_ENV_H

#include <utils/compiler.h>
#include <utils/Mutex.h>

#include <jni.h>

namespace dante {

class VirtualMachineEnv {
public:
    // must be called before VirtualMachineEnv::get() from a thread that is attached to the JavaVM
    static jint JNI_OnLoad(JavaVM* vm);

    // must be called on backend thread
    static VirtualMachineEnv& get() noexcept;

    // can be called from any thread that already has a JniEnv
    static JNIEnv* getThreadEnvironment();

    static bool hasVirtualMachine() noexcept;

    // must be called from the backend thread
    JNIEnv* getEnvironment() noexcept {
        JNIEnv* env = mJniEnv;
        if (UTILS_UNLIKELY(!env)) {
            return getEnvironmentSlow();
        }
        return env;
    }

    static void handleException(JNIEnv* env) noexcept;

private:
    explicit VirtualMachineEnv(JavaVM* vm) noexcept;
    ~VirtualMachineEnv() noexcept;
    JNIEnv* getEnvironmentSlow();

    static utils::Mutex sLock;
    static JavaVM* sVirtualMachine UTILS_GUARDED_BY(sLock);
    static JavaVM* getVirtualMachine();

    JNIEnv* mJniEnv = nullptr;
    JavaVM* mVirtualMachine = nullptr;
};

} // namespace dante

#endif // TNT_DANTE_DRIVER_ANDROID_VIRTUAL_MACHINE_ENV_H
