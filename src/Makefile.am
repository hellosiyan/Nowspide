bin_PROGRAMS = nowspide

QUIET_GEN = $(Q:@=@echo '  GEN   '$@;)

nowspide_SOURCES =      \
	nsp-net.c				\
	nsp-net.h				\
	nsp-window.c			\
	nsp-window.h			\
	nsp-feed.c				\
	nsp-feed.h				\
	nsp-feed-list.c			\
	nsp-feed-list.h			\
	nsp-feed-item-list.c	\
	nsp-feed-item-list.h	\
	nsp-feed-parser.c		\
	nsp-feed-parser.h		\
	nsp-db.c				\
	nsp-db.h				\
	nsp-app.c				\
	nsp-app.h				\
    main.c              

nowspide_CFLAGS =       \
    $(NSP_CFLAGS)       \
    $(NSP_WARNINGS)     \
    -DPACKAGE_DATA_DIR=\""$(datadir)"\" \
    -DPACKAGE_LOCALE_DIR=\""$(datadir)/locale"\"

nowspide_LDADD = $(NSP_LIBS)

