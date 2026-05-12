BUILD_PRX=0
DEBUG=0

PSPSDK=$(shell psp-config --pspsdk-path)
PSPDIR=$(shell psp-config --psp-prefix)

TARGET = pspalatro
OBJS = src/main.o media/pspalatro_icon.o



CFLAGS = -Wall -Wno-unused-label -G0 -Ofast
ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG_MODE=1
endif
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LIBS = -lpspgum -lpspgu -lpspdmac -lpsppower -lpspaudiolib -lpspaudio -lpsputility -lvorbisenc -lvorbisfile -lvorbis -logg -lzip -lz -lbz2 -llzma -lmbedcrypto
LDFLAGS =

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = PSPalatro
PSP_EBOOT_ICON = media/pspalatro_icon.png
PSP_EBOOT_PIC1 = media/pspalatro_pic.png

include $(PSPSDK)/lib/build.mak
debug:
	$(MAKE) DEBUG=1

media/pspalatro_icon.o: media/pspalatro_icon.png
	psp-ld -r -b binary $< -o $@

