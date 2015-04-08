#ifndef __SAK_PLUGIN_H
#ifndef __SAK_DEV
#error Do not include this file, please include sak.h instead!!!
#endif

#define __SAK_PLUGIN_H

#include <stdint.h>
#include "sak/sak_macros.h"

#define SAK_PLUGIN_CORE_VERSION SAK_MAKE_INT32(0, 0, 0, 1)
#define SAK_PLUGIN_MAGIC SAK_MAKE_INT64('S','A','K','P','L','G','I','N')

typedef enum{
    SAK_PLUGIN_TYPE_UNKNOWN = 0,
    SAK_PLUGIN_TYPE_MANAGER,
    SAK_PLUGIN_TYPE_NORMAL,
}SakPluginType;

typedef struct SakPlugin SakPlugin;
typedef struct SakPluginCore SakPluginCore;
typedef struct SakPluginInfo SakPluginInfo;
typedef struct SakPluginManager SakPluginManager;

typedef int (*SakPluginEntryFunc)(const SakPluginCore *core);

#define SAK_PLUGIN_NAME "ExportedSakPlugin"

#define SAK_EXPORT_PLUGIN(_type, _subtype, _version, _name, _desc, _data_size, _init, _deinit) \
const SakPlugin ExportedSakPlugin = { \
    .magic = SAK_PLUGIN_MAGIC, \
    .core_version = SAK_PLUGIN_CORE_VERSION, \
    .type = _type, \
    .subtype = _subtype, \
    .version = _version, \
    .name = _name, \
    .desc = _desc, \
    .data_size = _data_size, \
    .init = _init, \
    .deinit = _deinit \
};

struct SakPluginCore{
    uint32_t version;
};

struct SakPlugin{
    uint64_t magic;
    uint32_t core_version;
    uint16_t type;
    uint16_t subtype;
    uint32_t version;
    const char *name;
    const char *desc;
    uint32_t data_size;
    int (*init)(const SakPluginCore *core, void *data);
    int (*deinit)(const SakPluginCore *core, void *data);
};

struct SakPluginInfo{
    const char *file;
    const SakDl *dl;
    const SakPlugin *plugin;
    void *data;
};

struct SakPluginManager{
    int (*add_plugin)(SakPluginManager *man, SakPluginInfo *plugin_info);
    int (*update_plugin)(SakPluginManager *man, const char *newfile, uint16_t subtype);
    int (*update_self)(SakPluginManager *man, const char *newfile);
    int (*get_byfile)(SakPluginManager *man, const char *file, SakPlugin **plugin);
    int (*get_byname)(SakPluginManager *man, const char *name, SakPlugin **plugin);
    int (*get_bytype)(SakPluginManager *man, uint16_t subtype, SakPlugin **plugin);
    int (*remove)(SakPluginManager *man, uint16_t type, uint16_t subtype);
    void *priv;
};

typedef enum{
    SAK_PLUGIN_OPT_NONE = 0,
    SAK_PLUGIN_OPT_LOAD,
    SAK_PLUGIN_OPT_UNLOAD,
    SAK_PLUGIN_OPT_RELOAD,
}SakPluginOpt;

int sak_plugin_load(const char *file, SakPluginOpt opt);

int sak_plugin_loadall(const char *dir, SakPluginOpt opt);

int sak_plugin_get_byfile(const char *file, SakPlugin **plugin);

int sak_plugin_get_byname(const char *name, SakPlugin **plugin);

int sak_plugin_get_bytype(uint16_t type, uint16_t subtype, SakPlugin **plugin);

int sak_plugin_update(SakPlugin *plugin, const char *newfile);

#endif //__SAK_PLUGIN_H
