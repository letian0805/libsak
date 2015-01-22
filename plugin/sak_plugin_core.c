#include <sak.h>

int sak_plugin_load(const char *file, SakPluginOpt opt)
{
    return 0;
}

int sak_plugin_loadall(const char *dir, SakPluginOpt opt)
{
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

