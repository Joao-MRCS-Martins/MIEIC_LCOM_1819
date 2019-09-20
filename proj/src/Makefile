PROG=proj

SRCS = proj.c kbc.c keyboard.c mouse.c timer.c vbe.c videog.c game.c tetromino.c main_menu.c rtc.c bitmap.c credits.c update_mov.c

CPPFLAGS += -pedantic -D __LCOM_OPTIMIZED__

DPADD += ${LIBLCF}
LDADD += -llcf

.include <minix.lcom.mk>
