CFLAGS = `pkg-config --cflags --libs gstreamer-1.0`

all: app
app:
	@echo "link *.o to ../bin/app"
	@gcc *.o -o ../bin/app $(CFLAGS)

.PHONY: %-clean
%-clean:
	@echo "cleaning obj ... ..."
	@-rm -r *.o
