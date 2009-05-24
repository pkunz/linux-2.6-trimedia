#
# Makefile for arch/trimedia/kernel
#

obj-y += time.o ptrace.o init_task.o setup.o process.o irq.o signal.o end_of_intr.o sys_trimedia.o traps.o exception.o io.o dma.o pci-dma.o

obj-$(CONFIG_PCI) += pci.o quirks.o

CFLAGS_pci.o = $(NDKCFLAGS)
CFLAGS_dma.o = $(NDKCFLAGS)

