TARGETS = gst

.PHONY: help
help :
		@echo "In home directory!"
		@echo "Make Targets:"
		@echo " tutorials				- only compile the tutorials"
		@echo " audio					- only compile audio part "
		@echo " video					- only compile video part "
		@echo " all  					- compile both audio and video "
		@echo " clean					- clean bin "

.PHONY: tutorials
tutorials: 

.PHONY: audio
audio:
	@make clean
	@make -C audio
	@make -C ./utils $(addsuffix -audio, $(TARGETS))
	@make -C main
	@make -C obj
	@echo "done"

.PHONY: video
video: 
	@make clean
	@make -C video
	@make -C ./utils $(addsuffix -video, $(TARGETS))
	@make -C main
	@make -C obj
	@echo "done"

.PHONY: all
all:
	@make clean
	@make -C audio
	@make -C video
	@make -C ./utils $(addsuffix -all, $(TARGETS))
	@make -C main
	@make -C obj
	@echo "done"

.PHONY: clean
clean: 
	@make -C obj $(addsuffix -clean, $(TARGETS))
	@-rm -r bin/app
	@echo "done"
