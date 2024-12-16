CC = gcc
CFLAGS = -Wall -O2 -I.
LDFLAGS = -lpigpio -lwiringPi -lpthread -lrt

TARGET = game
OBJS = main.o lcd.o mcp3008.o game.o music.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

main.o: main.c pins.h lcd.h mcp3008.h game.h music.h
	$(CC) $(CFLAGS) -c main.c

lcd.o: lcd.c lcd.h pins.h
	$(CC) $(CFLAGS) -c lcd.c

mcp3008.o: mcp3008.c mcp3008.h
	$(CC) $(CFLAGS) -c mcp3008.c

game.o: game.c game.h lcd.h font5x7.h pins.h mcp3008.h
	$(CC) $(CFLAGS) -c game.c

music.o: music.c music.h pins.h
	$(CC) $(CFLAGS) -c music.c

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
