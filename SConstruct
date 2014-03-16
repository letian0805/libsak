import sys

incdir = ["include", "debug", "data", "pool", "platform", "mem", "io"]
libs = []
sources = ["io/epool.c", "io/token_bucket.c", "debug/log.c", "data/stack.c", "data/queue.c", "mem/mempool.c", "mem/mem.c"]
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
Program(target = "stack_test", source = "test/stack_test.c", CPPPATH = incdir, LIBS = libs, CFLAGS=cflags)
Program(target = "queue_test", source = "test/queue_test.c", CPPPATH = incdir, LIBS = libs, CFLAGS=cflags)
Program(target = "macro_test", source = "test/macro_test.c", CPPPATH = incdir, LIBS = libs, CFLAGS=cflags)
Program(target = "mempool_test", source = "test/mempool_test.c", CPPPATH = incdir, LIBS = libs, CFLAGS=cflags)
Program(target = "mem_test", source = "test/mem_test.c", CPPPATH = incdir, LIBS = libs, CFLAGS=cflags)
Program(target = "epool_test", source = "test/epool_test.c", CPPPATH = incdir, LIBS = libs, CFLAGS=cflags)
Program(target = "tokbkt_test", source = "test/tokbkt_test.c", CPPPATH = incdir, LIBS = libs, CFLAGS=cflags)
