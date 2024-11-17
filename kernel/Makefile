subdirs	= ./bootsector

all: $(subdirs)

$(subdirs): building
	@$(MAKE) -C $@


building:
	@echo "Building" $(subdirs)

debug:
	@echo $(subdirs)

.PHONY: all debug
