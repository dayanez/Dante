
#ifndef TNT_DANTE_BACKEND_PRIVATE_DISPATCHER_H
#define TNT_DANTE_BACKEND_PRIVATE_DISPATCHER_H

#include <stdint.h>

namespace dante::backend {

class Driver;
class CommandBase;

/*
 * Dispatcher is a data structure containing only function pointers.
 * Each function pointer targets code that unpacks the arguments to the driver's method and
 * calls it.
 *
 * Dispatcher's function pointers are populated during initialization and no CommandStream calls
 * can be made before that.
 *
 * When a command is inserted into the stream, the corresponding function pointer is copied
 * directly into CommandBase from Dispatcher.
 */
class Dispatcher {
public:
    using Execute = void (*)(Driver& driver, CommandBase* self, intptr_t* next);
#define DECL_DRIVER_API_SYNCHRONOUS(RetType, methodName, paramsDecl, params)
#define DECL_DRIVER_API(methodName, paramsDecl, params)                     Execute methodName##_;
#define DECL_DRIVER_API_RETURN(RetType, methodName, paramsDecl, params)     Execute methodName##_;

#include "DriverAPI.inc"
};

} // namespace dante::backend

#endif //TNT_DANTE_BACKEND_PRIVATE_DISPATCHER_H
