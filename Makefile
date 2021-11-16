include config.mk

PHONY := all
all:
	@for dir in $(BUILD_DIR); do \
		$(MAKE) -C $$dir; \
	done

PHONY += clean
clean:
	rm -f $(shell find -name "*.o" -o -name "*.dis") $(IMG) $(ELF)

PHONY += distclean
distclean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	rm -f $(shell find -name "*.dis")
	rm -f $(IMG) $(ELF)

PHONY += install
install:
	@if [ ! -f $(IMG) ]; then \
		echo "$(IMG) not found, please try to make!" ; \
	else \
		sudo mount /dev/sdc1 /mnt ; \
		sudo cp -r $(IMG) /mnt ; \
		sudo umount /mnt ;\
		echo "succeed to install $(IMG)" ; \
	fi

.PHONY: $(PHONY)
