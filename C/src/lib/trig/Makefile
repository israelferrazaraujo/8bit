.PHONY: compile, clean

########## END OF CONFIGURATION ######################

all: compile clean

compile:
	@sdcc -mz80 -c --no-std-crt0 --max-allocs-per-node 100000 --allow-unsafe-read --opt-code-speed trig.c
	@echo "SDCC is Processing ..."

clean:
	@rm -f *.com *.asm *.lst *.sym *.bin *.ihx *.lk *.map *.noi
	@echo "Temp files removed!"

