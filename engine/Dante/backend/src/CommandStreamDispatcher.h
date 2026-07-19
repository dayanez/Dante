
#ifndef TNT_DANTE_DRIVER_COMMANDSTREAM_DISPATCHER_H
#define TNT_DANTE_DRIVER_COMMANDSTREAM_DISPATCHER_H

#include "private/backend/Driver.h"
#include "private/backend/CommandStream.h"

#include <private/utils/Tracing.h>

#include <utils/compiler.h>

#include <utility>

#include <stddef.h>
#include <stdint.h>

#define DEBUG_LEVEL_NONE       0
#define DEBUG_LEVEL_SYSTRACE   1

// set to the desired debug level
#define DEBUG_LEVEL             DEBUG_LEVEL_NONE


#if DEBUG_LEVEL == DEBUG_LEVEL_NONE
#   define SYSTRACE()
#elif DEBUG_LEVEL == DEBUG_LEVEL_SYSTRACE
#   define SYSTRACE() DANTE_TRACING_CALL(DANTE_TRACING_CATEGORY_DANTE);
#else
#   error "invalid debug level"
#endif

namespace dante::backend {

template<typename ConcreteDriver>
class ConcreteDispatcher {
public:

    static Dispatcher make() noexcept;

private:
#define DECL_DRIVER_API_SYNCHRONOUS(RetType, methodName, paramsDecl, params)
#define DECL_DRIVER_API(methodName, paramsDecl, params)                                         \
    static void methodName(Driver& driver, CommandBase* base, intptr_t* next) {                 \
        SYSTRACE()                                                                              \
        using Cmd = COMMAND_TYPE(methodName);                                                   \
        ConcreteDriver& concreteDriver = static_cast<ConcreteDriver&>(driver);                  \
        Cmd::execute(&ConcreteDriver::methodName, concreteDriver, base, next);                  \
     }
#define DECL_DRIVER_API_RETURN(RetType, methodName, paramsDecl, params)                         \
    static void methodName(Driver& driver, CommandBase* base, intptr_t* next) {                 \
        SYSTRACE()                                                                              \
        using Cmd = COMMAND_TYPE(methodName##R);                                                \
        ConcreteDriver& concreteDriver = static_cast<ConcreteDriver&>(driver);                  \
        Cmd::execute(&ConcreteDriver::methodName##R, concreteDriver, base, next);               \
     }
#include "private/backend/DriverAPI.inc"
};

template<typename ConcreteDriver>
UTILS_NOINLINE
Dispatcher ConcreteDispatcher<ConcreteDriver>::make() noexcept {
    Dispatcher dispatcher;

#define DECL_DRIVER_API_SYNCHRONOUS(RetType, methodName, paramsDecl, params)
#define DECL_DRIVER_API(methodName, paramsDecl, params)                 \
                dispatcher.methodName##_ = &ConcreteDispatcher::methodName;
#define DECL_DRIVER_API_RETURN(RetType, methodName, paramsDecl, params) \
                dispatcher.methodName##_ = &ConcreteDispatcher::methodName;

#include "private/backend/DriverAPI.inc"

    return dispatcher;
}

} // namespace dante::backend

#endif // TNT_DANTE_DRIVER_COMMANDSTREAM_DISPATCHER_H
