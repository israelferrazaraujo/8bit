# ___________________________________________________________
#/               __           _                              \
#|              / _|         (_)                             |
#|             | |_ _   _ ___ _  ___  _ __                   |
#|             |  _| | | / __| |/ _ \| '_ \                  |
#|             | | | |_| \__ \ | (_) | | | |                 |
#|             |_|  \__,_|___/_|\___/|_| |_| *               |
#|                                                           |
#|               The MSX C Library for SDCC                  |
#|                     V1.2 - August 2019                    |
#|                                                           |
#|                                                           |
#|               Compilation Script for MacOS                |
#|                 (may be for linux too ! ?)                |
#|                                                           |
#\___________________________________________________________/
#
#  What does this file ?
#  It generate an MSX DOS executable file ready to be launch on a MSX Computer with MSX-DOS 
#  It also start the openMSX emuilator if it isn't already started
#
#
.PHONY: all, clean, fclean, emulator, re
#HEX2BINDIR = ../../tools/							# where is the hex2bin folder : ./ if installed in system
#HEX2BIN = ../../tools/hex2bin       				# name of hex2bin command name
ASM = /usr/local/bin/sdasz80  						# ASM compilator command name
CC = /usr/local/bin/sdcc							# SDCC commpilator command name
C_FILES :=  $(wildcard *.c) 	# which files to compil. File name must be sent by Sublime Text Build Systeme into the $(CompFile) variable
DEST := dsk/ 										# destination of executable files
ASM_FILES := $(wildcard *.s)  						# definition of sources files 
REL_FILES := $(subst .s,.rel,$(ASM_FILES))          # definition of .rel files

IHX_FILES := $(subst .c,.ihx,$(C_FILES))  			# definition of .ihx files 
COM_FILES := $(subst .ihx,.com,$(IHX_FILES)) 		# definition of .com files 

# Folder where to find other compiled files to include
INCLUDEDIR = ../../fusion-c/include/
# Folder where to find Fusion.lib 		
LIBDIR = ../../fusion-c/lib/

# standrd crt0 
crt0 = $(INCLUDEDIR)crt0_msxdos.rel

# use this crt0 if you want to pass parameters to your program
#crt0 = $(INCLUDEDIR)crt0_msxdos_advanced.rel

INC1 = ../lib/trig/trig.rel
INC2 = ../lib/fixed/fixed.rel
#INC3 = $(INCLUDEDIR)
#INC4 = $(INCLUDEDIR)
#INC5 = $(INCLUDEDIR)
#INC6 = $(INCLUDEDIR)
#INC7 = $(INCLUDEDIR)
#INC8 = $(INCLUDEDIR)
#INC9 = $(INCLUDEDIR)
#INCA = $(INCLUDEDIR)
#INCB = $(INCLUDEDIR)
#INCC = $(INCLUDEDIR)
#INCD = $(INCLUDEDIR)
#INCE = $(INCLUDEDIR)
#INCF = $(INCLUDEDIR)

# Standard Code-loc adress
ADDR_CODE = 0x106
# use this parameter if you are using crt0_msxdos_advanced
#ADDR_CODE = 0x180 
ADDR_DATA = 0x0

# --opt-code-size --allow-unsafe-read
# The above line is the full command line used by SDD to compil and link your to source file to an MSX-DOS executable
CCFLAGS = --code-loc $(ADDR_CODE) --data-loc $(ADDR_DATA) --allow-unsafe-read --disable-warning 196 -mz80 --no-std-crt0 --opt-code-speed --max-allocs-per-node 3000 fusion.lib -L $(LIBDIR) $(crt0) $(INC1) $(INC2) $(INC3) $(INC4) $(INC5) $(INC6) $(INC7) $(INC8) $(INC9) $(INCA) $(INCB) $(INCC) $(INCD) $(INCE) $(INCF)


########## END OF CONFIGURATION ######################


all:  $(REL_FILES) $(COM_FILES) clean

%.ihx: %.c
	@sdcc $(CCFLAGS) $^
	@echo "$(CC) is Processing ... !"

%.com: %.ihx
	../../tools/hex2bin -e com $^
	@cp $@ $(DEST)
	@echo "Updating files in  $(DEST)... !"
%.rel: %.s
#	$(AS) -o $^

clean:
	@rm -f *.com *.asm *.lst *.sym *.bin *.ihx *.lk *.map *.noi *.rel
	@echo "Temp files removed!"

hotbit:
	# Starting emulator script 
	../../openMSX/emul_start_hotbit.sh

philips:
	# Starting emulator script 
	../../openMSX/emul_start_philips.sh

turbo:
	# Starting emulator script 
	../../openMSX/emul_start_turbo.sh
