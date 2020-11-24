CFLAGS = `pkg-config --cflags --libs gstreamer-1.0`

all : video.c
	@echo "video.c -> ../obj/video.o"
	@gcc -c video.c -o ../obj/video.o $(CFLAGS)
