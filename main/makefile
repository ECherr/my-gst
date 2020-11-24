CFLAGS = `pkg-config --cflags --libs gstreamer-1.0`

all : main.c
	@echo "main.c -> ../obj/main.o"
	@gcc -c main.c -o ../obj/main.o $(CFLAGS)
