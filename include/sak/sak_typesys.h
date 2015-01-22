#ifndef __SAK_TYPESYS_H
#define __SAK_TYPESYS_H

#define SAK_FALSE 0
#define SAK_TRUE 1

typedef uint8_t SakBool;
typedef uint64_t SakInt;
typedef double SakFloat;
typedef char * SakString;

typedef enum{
    SAK_BASE_TYPE_UNKNOWN = 0,
    SAK_BASE_TYPE_BOOL,
    SAK_BASE_TYPE_INT,
    SAK_BASE_TYPE_FLOAT,
    SAK_BASE_TYPE_STRING,
}SakBaseType;

typedef struct SakTypeObject SakTypeObject;
typedef int (*SakTypeInit)(void **data);
typedef int (*SakTypeDeinit)(void **data);

struct SakTypeObject{
    const char *name;
    uint32_t size;
    SakTypeInit init;
    SakTypeDeinit deinit;
};

int sak_type_install(const char *name, uint32_t size, SakTypeInit init, SakTypeDeinit deinit);
int sak_type_findid_byname(const char *name);
int sak_type_uninstall(int id);

#endif //__SAK_TYPESYS_H

