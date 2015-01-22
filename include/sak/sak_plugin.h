#ifndef __SAK_PLUGIN_H
#ifndef __SAK_DEV
#error Do not include this file, please include sak.h instead!!!
#endif

#define __SAK_PLUGIN_H

#include <stdint.h>

#define SAK_PLUGIN_CORE_VERSION 0x00000001

typedef enum{
    SAK_PLUGIN_TYPE_UNKNOWN = 0,
    SAK_PLUGIN_TYPE_MANAGER,
    SAK_PLUGIN_TYPE_NORMAL,
}SakPluginType;

#define SAK_PLUGIN_MAX_PATH 1024

typedef struct SakPluginCore SakPluginCore;
struct SakPluginCore{
    uint32_t version;
};

typedef struct SakPlugin SakPlugin;

struct SakPlugin{
    uint32_t core_version;
    uint16_t type;
    uint16_t subtype;
    uint32_t version;
    uint64_t magic;
    const char *name;
    const char *desc;
    uint32_t data_size;
    int (*Init)(const SakPluginCore *core, uint32_t pm_version, void *data);
    int (*Deinit)(const SakPluginCore *core, void *data);
};

typedef struct SakPluginInfo SakPluginInfo;
struct SakPluginInfo{
    char *file;
    SakPlugin *plugin;
    SakDl *dl;
};

typedef struct SakPluginManager SakPluginManager;
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
