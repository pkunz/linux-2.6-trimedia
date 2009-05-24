#ifndef __ASM_OFFSETS_H__
#define __ASM_OFFSETS_H__
/*
 * DO NOT MODIFY.
 *
 * This file was generated by /home/guohr/WB2.6-testwork/arm_small_prj/build/linux-2.6.14-small/Kbuild
 *
 */

#define TSK_ACTIVE_MM 128 /* offsetof(struct task_struct, active_mm) */

#define TI_FLAGS 0 /* offsetof(struct thread_info, flags) */
#define TI_PREEMPT 4 /* offsetof(struct thread_info, preempt_count) */
#define TI_ADDR_LIMIT 8 /* offsetof(struct thread_info, addr_limit) */
#define TI_TASK 12 /* offsetof(struct thread_info, task) */
#define TI_EXEC_DOMAIN 16 /* offsetof(struct thread_info, exec_domain) */
#define TI_CPU 20 /* offsetof(struct thread_info, cpu) */
#define TI_CPU_DOMAIN 24 /* offsetof(struct thread_info, cpu_domain) */
#define TI_CPU_SAVE 28 /* offsetof(struct thread_info, cpu_context) */
#define TI_USED_CP 76 /* offsetof(struct thread_info, used_cp) */
#define TI_TP_VALUE 92 /* offsetof(struct thread_info, tp_value) */
#define TI_FPSTATE 96 /* offsetof(struct thread_info, fpstate) */
#define TI_VFPSTATE 240 /* offsetof(struct thread_info, vfpstate) */
#define TI_IWMMXT_STATE 96 /* (offsetof(struct thread_info, fpstate)+4)&~7 */

#define S_R0 0 /* offsetof(struct pt_regs, ARM_r0) */
#define S_R1 4 /* offsetof(struct pt_regs, ARM_r1) */
#define S_R2 8 /* offsetof(struct pt_regs, ARM_r2) */
#define S_R3 12 /* offsetof(struct pt_regs, ARM_r3) */
#define S_R4 16 /* offsetof(struct pt_regs, ARM_r4) */
#define S_R5 20 /* offsetof(struct pt_regs, ARM_r5) */
#define S_R6 24 /* offsetof(struct pt_regs, ARM_r6) */
#define S_R7 28 /* offsetof(struct pt_regs, ARM_r7) */
#define S_R8 32 /* offsetof(struct pt_regs, ARM_r8) */
#define S_R9 36 /* offsetof(struct pt_regs, ARM_r9) */
#define S_R10 40 /* offsetof(struct pt_regs, ARM_r10) */
#define S_FP 44 /* offsetof(struct pt_regs, ARM_fp) */
#define S_IP 48 /* offsetof(struct pt_regs, ARM_ip) */
#define S_SP 52 /* offsetof(struct pt_regs, ARM_sp) */
#define S_LR 56 /* offsetof(struct pt_regs, ARM_lr) */
#define S_PC 60 /* offsetof(struct pt_regs, ARM_pc) */
#define S_PSR 64 /* offsetof(struct pt_regs, ARM_cpsr) */
#define S_OLD_R0 68 /* offsetof(struct pt_regs, ARM_ORIG_r0) */
#define S_FRAME_SIZE 72 /* sizeof(struct pt_regs) */

#define VMA_VM_MM 0 /* offsetof(struct vm_area_struct, vm_mm) */
#define VMA_VM_FLAGS 20 /* offsetof(struct vm_area_struct, vm_flags) */

#define VM_EXEC 4 /* VM_EXEC */

#define PAGE_SZ 4096 /* PAGE_SIZE */
#define VIRT_OFFSET -1073741824 /* PAGE_OFFSET */

#define SYS_ERROR0 10420224 /* 0x9f0000 */

#define SIZEOF_MACHINE_DESC 56 /* sizeof(struct machine_desc) */

#endif
