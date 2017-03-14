CC=gcc
CFLAGS=-g -Wall `pkg-config --cflags dbus-glib-1`
LIBS=`pkg-config --libs dbus-glib-1`
RM=rm -rf

all: dbus-bindings server

clean:
	$(RM) *~ obj/*~

dbus-bindings: dbus-server dbus-client

dbus-server:
	dbus-binding-tool --mode=glib-server --prefix=object ../data/log-player-demo.xml > dbus-methods.h

dbus-client:
	dbus-binding-tool --mode=glib-client ../data/log-player-demo.xml > ../src/log-player-dbus-methods.h

server:
	$(CC) $(CFLAGS) $(LIBS) -o server main.c server.c
