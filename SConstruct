import sys, os
import string
env = Environment()

env['SAK_TOP'] = os.getcwd()
env['SAK_INCLUDE'] = 'include'
env['SAK_TEST'] = 'test'
env['SAK_SRC'] = 'src'

incdir = ["include", "debug", "data", "pool", "platform", "mem", "io"]
sources = [ "io/epool.c", "io/esignal.c", "io/token_bucket.c", 
            "debug/log.c", "debug/trace.c",
            "data/stack.c", "data/queue.c", 
            "mem/mempool.c", "mem/mem.c"]
cflags = ["-g", "-Wall"]
if sys.platform == "win32":
    cflags += ["-D__windows__"]
    sources += ["platform/windows.c"]
else:
    cflags += ["-D__linux__"]
    sources += ["platform/linux.c"]

env['CFLAGS'] = cflags
env['CPPPATH'] = incdir

libs = ["dl", "pthread"]
env['LIBS'] = libs
env.SharedLibrary(target = "sak", source = sources)
#env.Library(target = "sak", source = sources)
Export('env')
env.Program(target = "logdump", source = "debug/logdump.c")
SConscript('test/SConscript')
