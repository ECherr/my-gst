# some simple audio&video functions by using GStreamer



- [**GStreamer**](https://gstreamer.freedesktop.org/)



## now 

- [x] audio collection
- [x] video collection
- [x] send audio stream
- [x] send video stream
- [x] receive audio stream
- [x] receive video stream
  - [x] other's
  - [x] own



## later

- [ ] tutorial
- [ ] ... ...



## Install GStreamer on your Linux

- @@@my Linux Distribution: Arch

  > ```bash
  > $: sudo pacman -S gstreamer
  > ```

- ubuntu or debian

  > ```bash
  > $: sudo apt-get install libgstreamer1.0-0 gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-doc gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio
  > ```

- other distribution's information see [Installing GStreamer on Linux](https://gstreamer.freedesktop.org/documentation/installing/on-linux.html?gi-language=c)



## build APP

1. change to the top of work directory
2. enter`make`
3. choose your `make target`



## run app

details see `./app --help`

