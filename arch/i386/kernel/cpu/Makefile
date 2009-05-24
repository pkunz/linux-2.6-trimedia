#
# Makefile for x86-compatible CPU details and quirks
#

obj-y	:=	common.o proc.o

obj-$(CONFIG_CPU_SUP_AMD)	+=	amd.o
obj-$(CONFIG_CPU_SUP_CYRIX)	+=	cyrix.o
obj-$(CONFIG_CPU_SUP_CENTAUR)	+=	centaur.o
obj-$(CONFIG_CPU_SUP_TRANSMETA)	+=	transmeta.o
obj-$(CONFIG_CPU_SUP_INTEL)	+=	intel.o intel_cacheinfo.o
obj-$(CONFIG_CPU_SUP_RISE)	+=	rise.o
obj-$(CONFIG_CPU_SUP_NEXGEN)	+=	nexgen.o
obj-$(CONFIG_CPU_SUP_UMC)	+=	umc.o

obj-$(CONFIG_X86_MCE)	+=	mcheck/

obj-$(CONFIG_MTRR)	+= 	mtrr/
obj-$(CONFIG_CPU_FREQ)	+=	cpufreq/
