CFLAGS = -I ./include
##LIB    = ./libggfonts.so
LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm

all: fromspace fromspaceSound

fromspaceSound: fromspace.cpp log.cpp rodrigoO.cpp skylerE.cpp gameFoundation.h timers.cpp
	g++ $(CFLAGS) fromspace.cpp rodrigoO.cpp skylerE.cpp averyR.cpp loayS.cpp log.cpp timers.cpp libggfonts.a -Wall -Wextra -lssl -lcrypto $(LFLAGS) \
	    -ofromspaceSound \
	-DOPENAL_ENABLE \
	/usr/lib/x86_64-linux-gnu/libopenal.so \
	/usr/lib/x86_64-linux-gnu/libalut.so

fromspace: fromspace.cpp log.cpp rodrigoO.cpp skylerE.cpp gameFoundation.h timers.cpp
	g++ $(CFLAGS) fromspace.cpp rodrigoO.cpp skylerE.cpp averyR.cpp loayS.cpp log.cpp timers.cpp libggfonts.a -Wall -Wextra -lssl -lcrypto $(LFLAGS) \
	-ofromspace

clean:
	rm -f fromspace
	rm -f fromspaceSound
	rm -f *.o

