#
# Makefile for x86_64-specific library files.
#

CFLAGS_csum-partial.o := -funroll-loops

ifeq ($(CONFIG_CROSS_COMPILER_DEFAULT_BITS_32),y)
AFLAGS_clear_page.o = -Wa,-64
AFLAGS_copy_page.o = -Wa,-64
AFLAGS_copy_user.o = -Wa,-64
AFLAGS_csum-copy.o = -Wa,-64
AFLAGS_getuser.o = -Wa,-64
AFLAGS_memcpy.o = -Wa,-64
AFLAGS_memset.o = -Wa,-64
AFLAGS_putuser.o = -Wa,-64
AFLAGS_thunk.o = -Wa,-64
endif

obj-y := io.o

lib-y := csum-partial.o csum-copy.o csum-wrappers.o delay.o \
	usercopy.o getuser.o putuser.o  \
	thunk.o clear_page.o copy_page.o bitstr.o bitops.o
lib-y += memcpy.o memmove.o memset.o copy_user.o
