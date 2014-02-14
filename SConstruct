import sys

incdir = ["include", "debug", "data", "pool", "platform"]
libs = []
sources = ["debug/log.c", "data/stack.c", "data/queue.c", "pool/mempool.c"]
cflags = ["-g"]
if sys.platform == "win32":
    cflags += ["-D__windows__"]
    sources += ["platform/windows.c"]
else:
    cflags += ["-D__linux__"]
    sources += ["platform/linux.c"]

#libs += SharedLibrary(target = "sak", source = sources, CPPPATH = incdir)
libs += Library(target = "sak", source = sources, CPPPATH = incdir, CFLAGS=cflags)
libs += ["pthread"]

Program(target = "logdump", source = "debug/logdump.c", CPPPATH = incdir, CFLAGS=cflags)
Program(target = "data_test", source = "test/data_test.c", CPPPATH = incdir, LIBS = libs, CFLAGS=cflags)
