#ifndef _TOYOS_DATA_STRUCT
#define _TOYOS_DATA_STRUCT

#define offsetof(type, member) __builtin_offsetof(type, member)
#define container_of(object, type, member) \
     (type*)((char*)(object) - offsetof(type, member))

#endif