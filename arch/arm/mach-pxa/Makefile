#
# Makefile for the linux kernel.
#

# Common support (must be linked before board specific support)
obj-y += generic.o irq.o dma.o time.o
obj-$(CONFIG_PXA25x) += pxa25x.o
obj-$(CONFIG_PXA27x) += pxa27x.o

# Specific board support
obj-$(CONFIG_ARCH_LUBBOCK) += lubbock.o
obj-$(CONFIG_MACH_MAINSTONE) += mainstone.o
obj-$(CONFIG_ARCH_PXA_IDP) += idp.o
obj-$(CONFIG_PXA_SHARP_C7xx)	+= corgi.o corgi_ssp.o corgi_lcd.o ssp.o
obj-$(CONFIG_PXA_SHARP_Cxx00)	+= spitz.o corgi_ssp.o corgi_lcd.o ssp.o
obj-$(CONFIG_MACH_POODLE)	+= poodle.o

# Support for blinky lights
led-y := leds.o
led-$(CONFIG_ARCH_LUBBOCK) += leds-lubbock.o
led-$(CONFIG_MACH_MAINSTONE) += leds-mainstone.o
led-$(CONFIG_ARCH_PXA_IDP) += leds-idp.o

obj-$(CONFIG_LEDS) += $(led-y)

# Misc features
obj-$(CONFIG_PM) += pm.o sleep.o
obj-$(CONFIG_KGDB_PXA_SERIAL) += kgdb-serial.o

ifeq ($(CONFIG_PXA27x),y)
obj-$(CONFIG_PM) += standby.o
endif
