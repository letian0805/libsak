#ifndef __SAK_OBJECT_H
#define __SAK_OBJECT_H

#include <stdint.h>

typedef enum{
    SAK_PROP_MODE_NONE = 0,
    SAK_PROP_MODE_WRITE = 0x1,
    SAK_PROP_MODE_READ = 0x2,
}SakPropMode;

typedef struct SakProp SakProp;
typedef struct SakObject SakObject;
typedef int (*SakFunc)(SakObject *obj);
typedef struct SakData SakData;

typedef enum{
    SAK_DATA_TYPE_UNKNOWN = 0,
    SAK_DATA_TYPE_INT,
    SAK_DATA_TYPE_FLOAT,
    SAK_DATA_TYPE_STRING,
    SAK_DATA_TYPE_OBJECT,

    SAK_DATA_TYPE_MASK = 0xff
}SakDataType;

struct SakData{
    uint64_t type:8;
    uint64_t data_size:28;
    uint64_t buffer_size:28;
    union{
        void *p;
        uint64_t i;
        double f;
    }d;
};

int sak_data_alloc(SakData **data, SakDataType type);
int sak_data_init(SakData *data, SakDataType type);
int sak_data_type(SakData *data);
int sak_data_set(SakData *data, uint32_t size, void *val);
int sak_data_get(SakData *data, uint32_t *size, void *val);
int sak_data_deinit(SakData *data);
int sak_data_free(SakData **data);

struct SakProp{
    char *name;
    uint32_t index;
    SakObject *owner;
    SakPropMode mode;
    SakData data;
};

#define MAX_EVENT_NAME 64
typedef struct SakEvent SakEvent;
struct SakEvent{
    char name[MAX_EVENT_NAME];
    SakObject *from;
    SakData data;
};

typedef int (*SakObjEventHandler)(SakObject *obj, SakEvent *event);

struct SakObject{
    char *name;
    uint32_t id;
    uint32_t refcount;
    uint32_t prop_num;
    uint32_t prop_size;
    SakProp **props;
};

int sak_prop_new(SakProp **prop, const char *name, SakPropMode mode, uint32_t size, void *value);
int sak_prop_set_owner(SakProp *prop, SakObject *obj);
int sak_prop_get_owner(SakProp *prop, SakObject **owner);
int sak_prop_set_index(SakProp *prop, uint32_t index);
int sak_prop_get_index(SakProp *prop, uint32_t *index);
int sak_prop_set_value(SakProp *prop, uint32_t size, void *value);
int sak_prop_get_value(SakProp *prop, uint32_t size, void *value);
int sak_prop_delete(SakProp **prop);

int sak_object_new(SakObject **obj, const char *name, uint32_t prop_num, uint32_t event_num);

#endif //__SAK_OBJECT_H
