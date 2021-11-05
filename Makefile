include config.mk

PHONY := all
all:
	@for dir in $(BUILD_DIR); do \
		$(MAKE) -C $$dir; \
	done

PHONY += clean
clean:
	rm -f $(shell find -name "*.o") $(IMG) $(ELF)

PHONY += distclean
distclean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	rm -f $(IMG) $(ELF)

.PHONY: $(PHONY)