SUBDIRS := 01 02 03

.PHONY: all $(SUBDIRS)
all: $(SUBDIRS)

.PHONY: clean
clean: $(SUBDIRS)

.PHONY: validate
validate: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) $(MAKECMDGOALS) -C $@