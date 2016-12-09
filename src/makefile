# les executables
CC=`which gcc`
LD=`which gcc`

# les options des executables
CFLAGS = -c -fPIC -I../src/include -I./tools/include -g -Wall
LFLAGS = -ldl -L../lib -rdynamic -ltraitsignal -lm 


TARGET = WavAutoSegmentor
OBJ = main.o detection.o split.o

$(TARGET) : $(OBJ)
	$(CC) $^ $(LFLAGS) -o $@ 
	mv $(TARGET) ..
.o:.c
	#echo $(CIRDOX_ROOT)
	$(CC) $(CFLAGS) $< -c 
	mv *.o ./lib
clean:
	rm -f *.o *.orig ../WavAutoSegmentor




