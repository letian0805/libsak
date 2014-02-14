incdir = ["include", "debug", "data", "pool"]

sources = ["debug/log.c", "data/stack.c", "data/queue.c", "pool/mempool.c"]

libs = ["pthread"]

#SharedLibrary(target = "sak", source = sources, CPPPATH = incdir)
libs += Library(target = "sak", source = sources, CPPPATH = incdir)

Program(target = "logdump", source = "debug/logdump.c", CPPPATH = incdir)
Program(target = "data_test", source = "test/data_test.c", CPPPATH = incdir, LIBS = libs)
