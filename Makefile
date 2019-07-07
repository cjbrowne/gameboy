CFLAGS+=-g -Iinclude -Werror -Wextra -Wall `pkg-config --cflags sdl2`
LDFLAGS+=`pkg-config --libs sdl2`
all: gbe

.PHONY: clean

clean:
	$(RM) *.o gbe

gbe: lcd_controller.o gbe.o main.o
	$(CC) $^ -o $@ $(LDFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

gbe.o: gbe.c include/gbe.h
	$(CC) $(CFLAGS) -c $< -o $@

lcd_controller.o: lcd_controller.c include/lcd_controller.h
	$(CC) $(CFLAGS) -c $< -o $@