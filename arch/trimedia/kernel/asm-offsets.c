/*
 * This program is used to generate definitions needed by
 * assembly language modules.
 *
 * We use the technique used in the OSF Mach kernel code:
 * generate asm statements containing #defines,
 * compile this file to assembler, and then extract the
 * #defines from the assembly-language output.
 */

#define _ASM_TRIMEDIA_SYSTEM_H 
#include <linux/stddef.h>
#include <linux/thread_info.h>
#include <linux/types.h>

#include <asm/unistd.h>
#include <asm/ptrace.h>

#define DEFINE(sym, val) \
        asm volatile("\n->" #sym " %0 " #val : : "i" (val))

#define BLANK() asm volatile("\n->" : : )

int main(void)
{
	DEFINE(SIZEOF_STRUCT_PT_REGS,sizeof(struct pt_regs));
	DEFINE(SIZEOF_STRUCT_SWITCH_STACK,sizeof(struct switch_stack));
	DEFINE(OFFSET_KSP_THREAD_INFO,offsetof(struct thread_info,ksp));
	DEFINE(OFFSETOF_TASK_THREAD_INFO,offsetof(struct thread_info,task));
	DEFINE(SIZEOF_SYSCALL_ARGS,6*sizeof(unsigned long));
	DEFINE(PAGE_SIZE_ORDER_PLUS_THREAD_SIZE_ORDER,PAGE_SHIFT+THREAD_SIZE_ORDER);	
	DEFINE(__NR_sigreturn,__NR_sigreturn);
	DEFINE(__NR_rt_sigreturn,__NR_rt_sigreturn);
	DEFINE(__NR_clone,__NR_clone);
	DEFINE(__NR_vfork,__NR_vfork);
	DEFINE(NR_syscalls,NR_syscalls);
	
	return 0;
}
