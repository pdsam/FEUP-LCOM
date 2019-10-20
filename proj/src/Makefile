PROG=proj

.PATH: ${.CURDIR}

SRCS += game.c keyboard.c mouse.c v_gr.c timer.c loop.c player.c menu.c logic.c rtc.c sprite.c serialPort.c highscore.c aSprite.c proj.c

CPPFLAGS += -pedantic #-D __LCOM_OPTIMIZED__

DPADD += ${LIBLCF}
LDADD += -llcf

.include <minix.lcom.mk>
