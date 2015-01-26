import sys, os
import string
env = Environment()

cwd = os.getcwd()
env['SAK_TOP'] = cwd
env['SAK_INCLUDE'] = os.path.join(cwd, 'include')
env['SAK_TEST'] = os.path.join(cwd, 'test')
env['SAK_SRC'] = os.path.join(cwd, 'src')
env['SAK_LIB'] = ['sak']
env['SAK_LIBPATH'] = cwd

incdir = ["include", "debug", "data", "pool", "sys", "mem", "io"]
sources = [ "io/epool.c", "io/esignal.c", "io/token_bucket.c", 
            "debug/sak_log.c", "debug/sak_trace.c",
            "data/stack.c", "data/queue.c", 
            "mem/mempool.c", "mem/mem.c",
            "sys/sak_prog.c", "sys/sak_dir.c", "sys/sak_dl.c",
            "plugin/sak_plugin_core.c" ]
cflags = ["-g", "-Wall"]
if sys.platform == "win32":
    cflags += ["-D__windows__"]
else:
    cflags += ["-D__linux__"]

env['CFLAGS'] = cflags
env['CPPPATH'] = incdir

libs = ["dl", "pthread"]
env['LIBS'] = libs
env.SharedLibrary(target = "sak", source = sources)
#env.Library(target = "sak", source = sources)
Export('env')
SConscript('test/SConscript')
SConscript('tools/SConscript')
