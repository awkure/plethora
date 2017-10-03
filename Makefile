ARCH ?= $(if $(filter $(shell uname -m),x86_64), amd64, i386)

OBJS := $(ARCH)/platform.o 
DEPS := -I plethora.h 

LDEPS :=
RDEPS := 

NOSTD ?= 0

ifeq ($(NOSTD),1)
	NFLAGS := -fno-asynchronous-unwind-tables -fno-unwind-tables -Wl,--gc-sections -Wa,--noexecstack -fdata-sections -fno-builtin $(COMPILER_FLAGS) -nostdlib -D_NOSTD
	OBJS += $(ARCH)/start.S
endif

CFLAGS += -s -g -O3 -Os -Wall -Wextra -Wshadow -Wno-missing-field-initializers $(NFLAGS) 
LFLAGS += $(foreach lib,$(LDEPS),-l$(lib))
RFLAGS += $(foreach lib,$(RDEPS),-l$(lib))


.DEFAULT_GOAL := all 

all: plethora clean

help:
	@- echo "make [ all | plethora | clean ] NOSTD=[ 1 | 0 ] ?ARCH=[ i386 | amd64 ]"

clean:
	$(RM) *.o $(ARCH)/*.o *~ -f 

distclean: clean

%.o: %.c $(DEPS)

plethora: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)
	#if test -n "$$(NOSTD)" then strip -R .comment $@ && strip -R .note.gnu.build-id $@; fi;



.PHONY: plethora all clean distclean 
