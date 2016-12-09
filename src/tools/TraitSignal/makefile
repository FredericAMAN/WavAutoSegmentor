# les executables
CC=`which gcc`
LD=`which gcc`

# les options des executables
CFLAGS = -I../include -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include/ -I../../../src/include -I/usr/lib/glib-2.0/include -g -Wall -fPIC
AR=ar
ARFLAGS=rv

TARGET = libtraitsignal.a
OBJ = trait_signal.o


$(TARGET) : $(OBJ)
	$(AR) $(ARFLAGS) ../../../lib/$@ $^
.o:.c
	$(CC) $(CFLAGS) $< -c 
clean:
	rm -f *.o *.orig *~ ../../../lib/$(TARGET)

