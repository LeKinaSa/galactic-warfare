PROG=proj

SRCS = proj.c game_logic.c video.c mouse.c timer.c kbc.c utils.c keyboard.c dispatcher.c rtc.c

# __LCOM_OPTIMIZED__ disables most of the LCF's overhead (and functionality!)
CPPFLAGS += -pedantic -D __LCOM_OPTIMIZED__

DPADD += ${LIBLCF}
LDADD += -llcf

.include <minix.lcom.mk>
