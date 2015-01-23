#include <sak.h>
#include <string.h>
#include <stdio.h>

#define MAX_PLUGIN_INFO (64*1024)
static SakPluginInfo g_plugin_infos[MAX_PLUGIN_INFO];

static int sak_plugin_dump(const SakPlugin *plugin)
{
    printf("SakPlugin:\n");
    printf("  magic: 0x%lx\n  core: 0x%x\n  type: 0x%x\n  subtype: 0x%x\n  version: 0x%x  name: %s\n  desc: %s\n  data size: %d\n",
	plugin->magic, plugin->core_version, plugin->type, plugin->subtype, plugin->version, plugin->name, plugin->desc, plugin->data_size);
    return 0;
}

static int sak_plugin_register(const char *file, const SakPlugin *plugin, const SakDl *dl)
{
    int i;
    int ret = -1;
    SakPluginInfo *info = NULL;
    for (i = 0; i < MAX_PLUGIN_INFO; i++){
        info = &g_plugin_infos[i];
        if (info->dl == NULL){
            info->dl = dl;
            info->plugin = plugin;
            info->file = strdup(file);
            ret = 0;
            break;
        }
    }
    sak_plugin_dump(plugin);
    return ret;
}

int sak_plugin_load(const char *file, SakPluginOpt opt)
{
    SakDl *dl = sak_dl_open(file);
    if (dl == NULL){
        return -1;
    }
    SakPlugin *plugin = (SakPlugin *)sak_dl_getsymbol(dl, SAK_PLUGIN_NAME);
    if (plugin == NULL || plugin->magic != SAK_PLUGIN_MAGIC){
        sak_dl_close(dl);
        return -1;
    }
    sak_plugin_register(file, plugin, dl);
    return 0;
}

int sak_plugin_loadall(const char *dir, SakPluginOpt opt)
{
    SakDirItem item;
    SakDir *sak_dir = sak_dir_open(dir);
    if (sak_dir == NULL){
        return -1;
    }
    while(sak_dir_read(sak_dir, &item) == 0){
        char file[1024] = {0};
        sprintf(file, "%s/%s", dir, item.name);
        if (item.type == SAK_FILE_NORMAL){
            sak_plugin_load(file, 0);
        }else if(item.type == SAK_FILE_DIR){
            sak_plugin_loadall(file, 0);
        }
    }
    sak_dir_close(sak_dir);
    return 0;
}

int sak_plugin_get_byfile(const char *file, SakPlugin **plugin)
{
    return 0;
}

int sak_plugin_get_byname(const char *name, SakPlugin **plugin)
{
    return 0;
}

int sak_plugin_get_bytype(uint16_t type, uint16_t subtype, SakPlugin **plugin)
{
    return 0;
}

int sak_plugin_update(SakPlugin *plugin, const char *newfile)
{
    return 0;
}

