/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef __ASM_TRIMEDIA_IO_H__
#define __ASM_TRIMEDIA_IO_H__

#include <linux/types.h>

/*don't include it when generating asm-offset.h*/
#ifndef __ASM_OFFSETS
#include <linux/kernel.h>
#endif 

#ifndef CONFIG_SANE_CHECK
#define IO_ERROR(x)
#else
#define IO_ERROR   BUG_ON
#endif 

#define IO_SPACE_LIMIT 0xFFFFFFFF

#define non_pcimem_readb(addr) \
  ({ unsigned char __v = (*(__force volatile unsigned char *) (addr)); __v; })
#define non_pcimem_readw(addr) \
  ({ unsigned short __v = (*(__force volatile unsigned short *) (addr)); __v; })
#define non_pcimem_readl(addr) \
  ({ unsigned long __v = (*(__force volatile unsigned long *) (addr)); __v; })

#define non_pcimem_writeb(val, addr) \
  (void)((*(volatile unsigned char *) (addr)) = (val))
#define non_pcimem_writew(val, addr) \
  (void)((*(volatile unsigned short *) (addr)) = (val))
#define non_pcimem_writel(val, addr) \
  (void)((*(volatile unsigned int *) (addr)) = (val))

/* Some places try to pass in an loff_t for PHYSADDR (?!), so we cast it to
   long before casting it to a pointer to avoid compiler warnings.  */
#define ioremap(physaddr, size)	((void __iomem *)(unsigned long)(physaddr))
#define iounmap(addr)		((void)0)

#define ioremap_nocache(physaddr, size)		ioremap (physaddr, size)
#define ioremap_writethrough(physaddr, size)	ioremap (physaddr, size)
#define ioremap_fullcache(physaddr, size)	ioremap (physaddr, size)

#define readb_relaxed(a) readb(a)
#define readw_relaxed(a) readw(a)
#define readl_relaxed(a) readl(a)

extern void memcpy_fromio(void* dst, const volatile void __iomem * src,unsigned long count);
extern void memcpy_toio(volatile void __iomem * dst,void* src,unsigned long count);
extern void memset_io(volatile void __iomem * dst,__u8 c,unsigned long count);

#ifndef CONFIG_PCI

#define __raw_readb 	non_pcimem_readb
#define __raw_readw 	non_pcimem_readw
#define __raw_readl 	non_pcimem_readl

#define __raw_writeb 	non_pcimem_writeb
#define __raw_writew 	non_pcimem_writew
#define __raw_writel 	non_pcimem_writel

#define readb 		__raw_readb
#define readw 		__raw_readw
#define readl  		__raw_readl

#define writeb 		__raw_writeb
#define writew 		__raw_writew
#define writel 		__raw_writel

#else /*CONFIG_PCI*/

#include <asm/byteorder.h>
#include <asm/pci.h>

#if PCI_LITTLE_ENDIAN 
#define pci16_to_cpu   le16_to_cpu
#define pci32_to_cpu   le32_to_cpu
#define cpu_to_pci16   cpu_to_le16
#define cpu_to_pci32   cpu_to_le32
#elif PCI_BIG_ENDIAN
#define pci16_to_cpu   be16_to_cpu
#define pci32_to_cpu   be32_to_cpu
#define cpu_to_pci16   cpu_to_be16
#define cpu_to_pci32   cpu_to_be32
#elif 
#error "Please define PCI order first!!"
#endif 

#ifndef USE_TMHWPCI_H
extern int tmhwPciReadMemSpace(int unitid,unsigned long addr,int size,void* val);
extern int tmhwPciWriteMemSpace(int unitid,unsigned long addr,int size,unsigned long val);
#endif 

/*
 * read/write byte, word and long word from pci memory space.
 * PCI byte order, DO NOT switch to byte order.
 */
static inline __u8 pcimem_readb(const volatile void __iomem* addr)
{
	int err=0;
	__u8 val=0;
	err=tmhwPciReadMemSpace(0,(unsigned long)addr,sizeof(val),&val);
	IO_ERROR(err!=0);
	return val;
}

static inline __u16 pcimem_readw(const volatile void __iomem* addr)
{
	int err=0;
	__u16 val=0;
	err=tmhwPciReadMemSpace(0,(unsigned long)addr,sizeof(val),&val);
	IO_ERROR(err!=0);
	return val;
}

static inline __u32 pcimem_readl(const volatile void __iomem* addr)
{
	int err=0;
	__u32 val=0;
	err=tmhwPciReadMemSpace(0,(unsigned long)addr,sizeof(val),&val);
	IO_ERROR(err!=0);
	return val;
}

static inline void pcimem_writeb(__u8 val,const volatile void __iomem* addr)
{
	int err=0;
	err=tmhwPciWriteMemSpace(0,(unsigned long)addr,sizeof(val),val);
	IO_ERROR(err!=0);
	return;	
}

static inline void pcimem_writew(__u16 val, const volatile void __iomem* addr)
{
	int err=0;
	err=tmhwPciWriteMemSpace(0,(unsigned long)addr,sizeof(val),val);
	IO_ERROR(err!=0);
	return;
}

static inline void pcimem_writel(__u32 val, const volatile void __iomem* addr)
{
	int err=0;
	err=tmhwPciWriteMemSpace(0,(unsigned long)addr,sizeof(val),val);
	IO_ERROR(err!=0);
	return;
}


static inline __u8 __raw_readb(const volatile void __iomem* addr)
{
	if(is_pci_mem(addr)) return pcimem_readb(addr);
	BUG();
	return non_pcimem_readb(addr);
}

static inline __u16 __raw_readw(const volatile void __iomem* addr)
{
	if(is_pci_mem(addr)) return pcimem_readw(addr);
	BUG();
	return non_pcimem_readb(addr);
}

static inline __u32 __raw_readl(const volatile void __iomem* addr)
{
	if(is_pci_mem(addr)) return pcimem_readl(addr);
	BUG();
	return non_pcimem_readl(addr);
}

static inline void __raw_writeb(__u8 val,const volatile void __iomem* addr)
{
	if(is_pci_mem(addr)){
		pcimem_writeb(val,addr);
		return;
	}
	non_pcimem_writeb(val,addr);
	return;
}

static inline void __raw_writew(__u16 val, const volatile void __iomem* addr)
{
	if(is_pci_mem(addr)){
		pcimem_writew(val,addr);
		return;
	}
	non_pcimem_writew(val,addr);
	return;
}

static inline void __raw_writel(__u32 val, const volatile void __iomem* addr)
{
	if(is_pci_mem(addr)){
		pcimem_writel(val,addr);
		return;
	}
	non_pcimem_writel(val,addr);
	return;
}

static inline __u8 readb(const volatile void __iomem* addr)
{
	return __raw_readb(addr);
}

static inline __u16 readw(const volatile void __iomem* addr)
{
	if(is_pci_mem(addr)) 
		return pci16_to_cpu(pcimem_readw(addr));
	return non_pcimem_readw(addr);
}

static inline __u32 readl(const volatile void __iomem* addr)
{
	if(is_pci_mem(addr))
		return pci32_to_cpu(pcimem_readl(addr));
	return non_pcimem_readl(addr);
}

static inline void writeb(__u8 val, const volatile void __iomem* addr)
{
	__raw_writeb(val,addr);
	return;
}

static inline void writew(__u16 val, const volatile void __iomem* addr)
{
	if(is_pci_mem(addr)){
		pcimem_writew(cpu_to_pci16(val),addr);
		return;
	}
	non_pcimem_writew(val,addr);
	return;
}

static inline void writel(__u32 val, const volatile void __iomem* addr)
{
	if(is_pci_mem(addr)){
		pcimem_writel(cpu_to_pci32(val),addr);
		return;
	}
	non_pcimem_writel(val,addr);
	return;
}

#endif /*CONFIG_PCI*/

#ifndef CONFIG_PCI

static inline __u8 inb(unsigned long ioport)
{
	BUG();
	return 0;
}

static inline __u16 inw(unsigned long ioport)
{
	BUG();
	return 0;
}

static inline __u32 inl(unsigned long ioport)
{
	BUG();
	return 0;
}

static inline void outb(__u8 val, unsigned long ioport)
{
	BUG();
	return;
}

static inline void outw(__u16 val, unsigned long ioport)
{
	BUG();
	return;
}

static inline void outl(__u32 val, unsigned long ioport)
{
	BUG();
	return;
}

#else /*CONFIG_PCI*/

#ifndef USE_TMHWPCI_H
extern int tmhwPciReadIo(int unitid,unsigned long addr,int size,void* val);
extern int tmhwPciWriteIo(int unitid,unsigned long addr,int size,unsigned long val);
#endif 

static inline __u8 pciio_readb(unsigned long ioport)
{
	int err=0;
	__u8 val=0;
	err=tmhwPciReadIo(0,(unsigned long)ioport,sizeof(val),&val);
	IO_ERROR(err!=0);
	return val;
}

static inline __u16 pciio_readw(unsigned long ioport)
{
	int err=0;
	__u16 val=0;
	err=tmhwPciReadIo(0,(unsigned long)ioport,sizeof(val),&val);
	IO_ERROR(err!=0);
	return val;
}

static inline __u32 pciio_readl(unsigned long ioport)
{
	int err=0;
	__u32 val=0;
	err=tmhwPciReadIo(0,(unsigned long)ioport,sizeof(val),&val);
	IO_ERROR(err!=0);
	return val;
}

static inline void pciio_writeb(__u8 val,unsigned long ioport)
{
	int err=0;
	err=tmhwPciWriteIo(0,(unsigned long)ioport,sizeof(val),val);
	IO_ERROR(err!=0);
	return;
}

static inline void pciio_writew(__u16 val, unsigned long ioport)
{
	int err=0;
	err=tmhwPciWriteIo(0,(unsigned long)ioport,sizeof(val),val);
	IO_ERROR(err!=0);
	return;
}

static inline void pciio_writel(__u32 val,unsigned long ioport)
{
	int err=0;
	err=tmhwPciWriteIo(0,(unsigned long)ioport,sizeof(val),val);
	IO_ERROR(err!=0);
	return;
}

static inline __u8 inb(unsigned long ioport)
{
	IO_ERROR(!is_pci_ioport(ioport));
	return pciio_readb(ioport);
}

static inline __u16 inw(unsigned long ioport)
{
	IO_ERROR(!is_pci_ioport(ioport));
	return pci16_to_cpu(pciio_readw(ioport));
}

static inline __u32 inl(unsigned long ioport)
{
	IO_ERROR(!is_pci_ioport(ioport));
	return pci32_to_cpu(pciio_readl(ioport));
}

static inline void outb(__u8 val, unsigned long ioport)
{
	IO_ERROR(!is_pci_ioport(ioport));
	pciio_writeb(val,ioport);
}

static inline void outw(__u16 val, unsigned long ioport)
{
	IO_ERROR(!is_pci_ioport(ioport));
	pciio_writew(cpu_to_pci16(val),ioport);
}

static inline void outl(__u32 val, unsigned long ioport)
{
	IO_ERROR(!is_pci_ioport(ioport));
	pciio_writel(cpu_to_pci32(val),ioport);
}

#endif /*CONFIG_PCI*/

/*
 * FIXME: I don't know how to implement it for trimedia
 */
#define inb_p(port)		inb((port))
#define outb_p(val, port)	outb((val), (port))
#define inw_p(port)		inw((port))
#define outw_p(val, port)	outw((val), (port))
#define inl_p(port)		inl((port))
#define outl_p(val, port)	outl((val), (port))

static inline void insb (unsigned long port, void *dst, unsigned long count)
{
	unsigned char *p = dst;
	while (count--)
		*p++ = inb (port);
}
static inline void insw (unsigned long port, void *dst, unsigned long count)
{
	unsigned short *p = dst;
	while (count--)
		*p++ = inw (port);
}
static inline void insl (unsigned long port, void *dst, unsigned long count)
{
	unsigned long *p = dst;
	while (count--)
		*p++ = inl (port);
}

static inline void
outsb (unsigned long port, const void *src, unsigned long count)
{
	const unsigned char *p = src;
	while (count--)
		outb (*p++, port);
}
static inline void
outsw (unsigned long port, const void *src, unsigned long count)
{
	const unsigned short *p = src;
	while (count--)
		outw (*p++, port);
}
static inline void
outsl (unsigned long port, const void *src, unsigned long count)
{
	const unsigned long *p = src;
	while (count--)
		outl (*p++, port);
}

static inline void* ioport_map(unsigned long port, unsigned int len)
{
	return (void*)port;
}

static inline void ioport_unmap(void* addr)
{
	/*nothing to do*/
}

#define IO_COND(addr,is_pio,is_mmio)   do { 		\
	unsigned long port=(unsigned long)(addr); 	\
	if(is_pci_ioport(port)){ 			\
		is_pio; 				\
	}else{ 						\
		is_mmio; 				\
	}  						\
}while(0)

static inline u8 ioread8(void* addr)
{
	IO_COND(addr,return inb(port),return readb(addr));	
	return 0;
}

static inline u16 ioread16(void* addr)
{
	IO_COND(addr, return inw(port),return readw(addr));
	return 0;
}

static inline u32 ioread32(void* addr)
{
	IO_COND(addr, return inl(port),return readl(addr));
	return 0;
}

static inline void iowrite8(u8 val, void* addr)
{
	IO_COND(addr,outb(val,port),writeb(val,addr));
}

static inline void iowrite16(u16 val, void* addr)
{
	IO_COND(addr,outw(val,port),writew(val,addr));
}

static inline void iowrite32(u32 val, void* addr)
{
	IO_COND(addr,outl(val,port),writel(val,addr));
}

/*TODO: implement other iomap routines.*/

#define mmiowb()

#define page_to_phys(page)      ((page - mem_map) << PAGE_SHIFT)
#if 0
/* This is really stupid; don't define it.  */
#define page_to_bus(page)       page_to_phys (page)
#endif

/* Conversion between virtual and physical mappings.  */
#define mm_ptov(addr)		((void*)(addr))	
#define mm_vtop(addr)		((unsigned long)(addr))
#define phys_to_virt(addr)	((void*)(addr))
#define virt_to_phys(addr) 	((unsigned long)(addr))	

/*
 * Convert a physical pointer to a virtual kernel pointer for /dev/mem
 * access
 */
#define xlate_dev_mem_ptr(p)	__va(p)

/*
 * Convert a virtual cached pointer to an uncached pointer
 */
#define xlate_dev_kmem_ptr(p)	p

#endif /* __ASM_TRIMEDIA_IO_H__ */

