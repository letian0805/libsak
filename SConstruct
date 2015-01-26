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

incdir = ["include", "io"]
src_io = [ "io/epool.c", "io/esignal.c", "io/token_bucket.c" ]
src_debug = ["debug/sak_log.c", "debug/sak_trace.c"]
src_data = ["data/stack.c", "data/queue.c"]
src_mem = ["mem/mempool.c", "mem/mem.c"]
src_sys = ["sys/sak_prog.c", "sys/sak_dir.c", "sys/sak_dl.c"]
src_plugin = ["plugin/sak_plugin_core.c" ]

sources = src_debug + src_data + src_sys + src_plugin
libs = []

if sys.platform == "win32":
    cflags = ["-D__windows__", "-D_CRT_SECURE_NO_WARNINGS", "-D_DEBUG", "-D_WINDOWS"]
else:
    cflags = ["-g", "-D__linux__", "-Wall"]
    sources += src_io + src_mem
    libs += ["dl", "pthread"]

env['CFLAGS'] = cflags
env['CPPPATH'] = incdir

env['LIBS'] = libs
env.Library(target = "sak", source = sources)
env.SharedLibrary(target = "sak", source = sources)
Export('env')
SConscript('test/SConscript')
SConscript('tools/SConscript')
