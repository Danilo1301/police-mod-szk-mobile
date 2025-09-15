/* DO NOT CHANGE IT */

#ifndef __GETINTERFACE_H
#define __GETINTERFACE_H

#include <dlfcn.h>

#define DEFAULT_LIB_NAME    "AML"

#define WRAP_INTERFACE(__interface_name, __interface_var)	RegisterInterface(#__interface_name, __interface_var)

typedef void* (*GetInterfaceFn)(const char*);
typedef void* (*RegInterfaceFn)(const char*, void*);

inline void* GetInterface(const char* szInterfaceName)
{
    GetInterfaceFn _GetInterface = (GetInterfaceFn)dlsym((void*)dlopen("lib" DEFAULT_LIB_NAME ".so", RTLD_NOW), "GetInterface");
    return _GetInterface(szInterfaceName);
}

inline void RegisterInterface(const char* szInterfaceName, void* pInterface)
{
    RegInterfaceFn _RegInterface = (RegInterfaceFn)dlsym((void*)dlopen("lib" DEFAULT_LIB_NAME ".so", RTLD_NOW), "CreateInterface");
    _RegInterface(szInterfaceName, pInterface);
}

#endif // __GETINTERFACE_H