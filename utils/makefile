CFLAGS = `pkg-config --cflags --libs gstreamer-1.0` 

.PHONY: %-audio
%-audio:
	@echo "compiling audio part... ..."
	@gcc -c utils.c -o ../obj/utils.o -D AUDIO=1 $(CFLAGS)

.PHONY: %-video
%-video:
	@echo "compiling video part... ..."
	@gcc -c utils.c -o ../obj/utils.o -D VIDEO=1 $(CFLAGS) 

.PHONY: %-all
%-all:
	@echo "compiling both audio and video... ..."
	@gcc -c utils.c -o ../obj/utils.o -D AUDIO=1 -D VIDEO=1 $(CFLAGS)
