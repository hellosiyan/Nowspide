bin_PROGRAMS = nowspide

QUIET_GEN = $(Q:@=@echo '  GEN   '$@;)

nowspide_SOURCES =      \
	nsp-net.c				\
	nsp-net.h				\
    main.c              

nowspide_CFLAGS =       \
    $(NSP_CFLAGS)       \
    $(NSP_WARNINGS)     \
    -DPACKAGE_DATA_DIR=\""$(datadir)"\" \
    -DPACKAGE_LOCALE_DIR=\""$(datadir)/locale"\"

nowspide_LDADD = $(NSP_LIBS)

