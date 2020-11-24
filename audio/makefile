CFLAGS = `pkg-config --cflags --libs gstreamer-1.0`

all : audio.c
	@echo "audio.c -> ../obj/audio.o"
	@gcc -c audio.c -o ../obj/audio.o $(CFLAGS) 
