/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/config.h>
#include <asm/types.h>
#include <asm/linkage.h>

#if BITS_PER_LONG == 32
#define _sys_truncate64 	_trimedia_truncate64
#define _sys_ftruncate64 	_trimedia_ftruncate64
#define _sys_fadvise64 		_trimedia_fadvise64
#define _sys_fadvise64_64 	_trimedia_fadvise64_64
#define _sys_pread64 		_trimedia_pread64
#define _sys_pwrite64 		_trimedia_pwrite64
#ifdef CONFIG_MMU
#define _sys_readahead 		_trimedia_readahead
#endif 
#endif 

#ifndef CONFIG_MMU
#define _sys_madvise  		_sys_ni_syscall
#define _sys_readahead  	_sys_ni_syscall
#define _sys_mprotect 		_sys_ni_syscall
#define _sys_msync 		_sys_ni_syscall
#define _sys_mlock 		_sys_ni_syscall
#define _sys_munlock 		_sys_ni_syscall
#define _sys_mlockall 		_sys_ni_syscall
#define _sys_munlockall 	_sys_ni_syscall
#define _sys_mremap 		_sys_ni_syscall
#define _sys_mincore 		_sys_ni_syscall
#define _sys_remap_file_pages   _sys_ni_syscall
#define _sys_swapon 		_sys_ni_syscall
#define _sys_swapoff  		_sys_ni_syscall
#endif 

GLOBAL_DATA(sys_call_table)
	.word SYMBOL_NAME(sys_restart_syscall    ) /*0*/
	.word SYMBOL_NAME(sys_exit		 ) /*1*/
	.word SYMBOL_NAME(sys_fork		 ) /*2*/
	.word SYMBOL_NAME(sys_read		 ) /*3*/
	.word SYMBOL_NAME(sys_write		 ) /*4*/
	.word SYMBOL_NAME(sys_open		 ) /*5*/
	.word SYMBOL_NAME(sys_close		 ) /*6*/
	.word SYMBOL_NAME(sys_waitpid		 ) /*7*/
	.word SYMBOL_NAME(sys_creat		 ) /*8*/
	.word SYMBOL_NAME(sys_link		 ) /*9*/
	.word SYMBOL_NAME(sys_unlink		 ) /*10*/
	.word SYMBOL_NAME(sys_execve		 ) /*11*/
	.word SYMBOL_NAME(sys_chdir		 ) /*12*/
	.word SYMBOL_NAME(sys_time		 ) /*13*/
	.word SYMBOL_NAME(sys_mknod		 ) /*14*/
	.word SYMBOL_NAME(sys_chmod		 ) /*15*/
	.word SYMBOL_NAME(sys_chown		 ) /*16*/
	.word SYMBOL_NAME(sys_ni_syscall         ) /*17: old break syscall holder*/
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /*18: old sys_stat*/
	.word SYMBOL_NAME(sys_lseek		 ) /*19*/
	.word SYMBOL_NAME(sys_getpid		 ) /*20*/
	.word SYMBOL_NAME(sys_mount		 ) /*21*/
	.word SYMBOL_NAME(sys_oldumount		 ) /*22*/
	.word SYMBOL_NAME(sys_setuid		 ) /*23*/
	.word SYMBOL_NAME(sys_getuid		 ) /*24*/
	.word SYMBOL_NAME(sys_stime		 ) /*25*/
	.word SYMBOL_NAME(sys_ptrace		 ) /*26*/
	.word SYMBOL_NAME(sys_alarm		 ) /*27*/
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /*28: old sys_fstat*/
	.word SYMBOL_NAME(sys_pause		 ) /*29*/
	.word SYMBOL_NAME(sys_utime		 ) /*30*/
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /*31: old stty syscall holder*/
	.word SYMBOL_NAME(sys_ni_syscall	 ) /*32: old gtty syscall holder*/
	.word SYMBOL_NAME(sys_access		 ) /*33*/
	.word SYMBOL_NAME(sys_nice		 ) /*34*/
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /*35: old ftime syscall holder*/
	.word SYMBOL_NAME(sys_sync		 ) /*36*/
	.word SYMBOL_NAME(sys_kill		 ) /*37*/
	.word SYMBOL_NAME(sys_rename		 ) /*38*/
	.word SYMBOL_NAME(sys_mkdir		 ) /*39*/
	.word SYMBOL_NAME(sys_rmdir		 ) /*40*/
	.word SYMBOL_NAME(sys_dup		 ) /*41*/
	.word SYMBOL_NAME(sys_pipe		 ) /*42*/
	.word SYMBOL_NAME(sys_times		 ) /*43*/
	.word SYMBOL_NAME(sys_ni_syscall	 ) /*44: old prof syscall holder*/
	.word SYMBOL_NAME(sys_brk		 ) /*45*/
	.word SYMBOL_NAME(sys_setgid		 ) /*46*/
	.word SYMBOL_NAME(sys_getgid		 ) /*47*/
	.word SYMBOL_NAME(sys_signal		 ) /*48*/
	.word SYMBOL_NAME(sys_geteuid		 ) /*49*/
	.word SYMBOL_NAME(sys_getegid		 ) /*50*/
	.word SYMBOL_NAME(sys_acct		 ) /*51*/
	.word SYMBOL_NAME(sys_umount		 ) /*52*/
	.word SYMBOL_NAME(sys_ni_syscall	 ) /*53: old lock syscall holder*/
	.word SYMBOL_NAME(sys_ioctl		 ) /*54*/
	.word SYMBOL_NAME(sys_fcntl		 ) /*55*/
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /*56: old mpx syscall holder*/
	.word SYMBOL_NAME(sys_setpgid		 ) /*57*/
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /*58: old ulimit syscall holder*/
	.word SYMBOL_NAME(sys_ni_syscall  	 ) /*59: old old uname syscall holder*/
	.word SYMBOL_NAME(sys_umask		 ) /*60*/
	.word SYMBOL_NAME(sys_chroot		 ) /*61*/
	.word SYMBOL_NAME(sys_ustat		 ) /*62*/
	.word SYMBOL_NAME(sys_dup2		 ) /*63*/
	.word SYMBOL_NAME(sys_getppid		 ) /*64*/
	.word SYMBOL_NAME(sys_getpgrp		 ) /*65*/
	.word SYMBOL_NAME(sys_setsid		 ) /*66*/
	.word SYMBOL_NAME(sys_sigaction		 ) /*67*/
	.word SYMBOL_NAME(sys_sgetmask		 ) /*68*/
	.word SYMBOL_NAME(sys_ssetmask		 ) /*69*/
	.word SYMBOL_NAME(sys_setreuid		 ) /*70*/
	.word SYMBOL_NAME(sys_setregid		 ) /*71*/
	.word SYMBOL_NAME(sys_sigsuspend 	 ) /*72*/
	.word SYMBOL_NAME(sys_sigpending	 ) /*73*/
	.word SYMBOL_NAME(sys_sethostname	 ) /*74*/
	.word SYMBOL_NAME(sys_setrlimit		 ) /*75*/
	.word SYMBOL_NAME(sys_getrlimit		 ) /*76*/
	.word SYMBOL_NAME(sys_getrusage		 ) /*77*/
	.word SYMBOL_NAME(sys_gettimeofday	 ) /*78*/
	.word SYMBOL_NAME(sys_settimeofday	 ) /*79*/
	.word SYMBOL_NAME(sys_getgroups		 ) /*80*/
	.word SYMBOL_NAME(sys_setgroups		 ) /*81*/
	.word SYMBOL_NAME(old_select		 ) /*82*/
	.word SYMBOL_NAME(sys_symlink		 ) /*83*/
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /*84: old lstat syscall holder*/
	.word SYMBOL_NAME(sys_readlink		 ) /*85*/
	.word SYMBOL_NAME(sys_uselib		 ) /*86*/
	.word SYMBOL_NAME(sys_swapon		 ) /*87*/
	.word SYMBOL_NAME(sys_reboot		 ) /*88*/
	.word SYMBOL_NAME(old_readdir		 ) /*89*/
	.word SYMBOL_NAME(sys_mmap2 		 ) /*90*/
	.word SYMBOL_NAME(sys_munmap		 ) /*91*/
	.word SYMBOL_NAME(sys_truncate		 ) /*92*/
	.word SYMBOL_NAME(sys_ftruncate		 ) /*93*/
	.word SYMBOL_NAME(sys_fchmod		 ) /*94*/
	.word SYMBOL_NAME(sys_fchown		 ) /*95*/
	.word SYMBOL_NAME(sys_getpriority	 ) /*96*/
	.word SYMBOL_NAME(sys_setpriority	 ) /*97*/
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /*98: old profil syscall holder*/
	.word SYMBOL_NAME(sys_statfs		 ) /*99*/
	.word SYMBOL_NAME(sys_fstatfs		 ) /*100*/
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /*101: i386 ioperm syscall holder*/
	.word SYMBOL_NAME(sys_socketcall	 ) /*102*/
	.word SYMBOL_NAME(sys_syslog		 ) /*103*/
	.word SYMBOL_NAME(sys_setitimer		 ) /*104*/
	.word SYMBOL_NAME(sys_getitimer		 ) /*105*/
	.word SYMBOL_NAME(sys_newstat		 ) /*106*/
	.word SYMBOL_NAME(sys_newlstat		 ) /*107*/
	.word SYMBOL_NAME(sys_newfstat		 ) /*108*/
	.word SYMBOL_NAME(sys_ni_syscall  	 ) /*109: old uname syscall holder*/
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /* sys_iopl not supported 110 */
	.word SYMBOL_NAME(sys_vhangup		 ) /*111*/
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /* sys_idle Obsolete 112 */
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /* sys_vm86 not supported 113 */
	.word SYMBOL_NAME(sys_wait4		 ) /*114*/
	.word SYMBOL_NAME(sys_swapoff		 ) /*115*/
	.word SYMBOL_NAME(sys_sysinfo		 ) /*116*/
	.word SYMBOL_NAME(sys_ipc		 ) /*117*/
	.word SYMBOL_NAME(sys_fsync		 ) /*118*/
	.word SYMBOL_NAME(sys_sigreturn		 ) /*119*/
	.word SYMBOL_NAME(sys_clone		 ) /*120*/
	.word SYMBOL_NAME(sys_setdomainname	 ) /*121*/
	.word SYMBOL_NAME(sys_newuname 		 ) /*122*/
	.word SYMBOL_NAME(sys_cacheflush	 ) /*123*/
	.word SYMBOL_NAME(sys_adjtimex		 ) /*124*/
	.word SYMBOL_NAME(sys_mprotect		 ) /*125*/
	.word SYMBOL_NAME(sys_sigprocmask	 ) /*126*/
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /*127: old create_module syscall holder*/
	.word SYMBOL_NAME(sys_init_module	 ) /*128*/
	.word SYMBOL_NAME(sys_delete_module	 ) /*129*/
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /*130: old get_kernel_syms syscall holder*/
	.word SYMBOL_NAME(sys_quotactl		 ) /*131*/
	.word SYMBOL_NAME(sys_getpgid		 ) /*132*/
	.word SYMBOL_NAME(sys_fchdir		 ) /*133*/
	.word SYMBOL_NAME(sys_bdflush		 ) /*134*/
	.word SYMBOL_NAME(sys_sysfs		 ) /*135*/
	.word SYMBOL_NAME(sys_personality	 ) /*136*/
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /*137 Syscall for Andrew File System, not supported */
	.word SYMBOL_NAME(sys_setfsuid		 ) /*138*/
	.word SYMBOL_NAME(sys_setfsgid		 ) /*139*/
	.word SYMBOL_NAME(sys_llseek		 ) /*140*/
	.word SYMBOL_NAME(sys_getdents		 ) /*141*/
	.word SYMBOL_NAME(sys_select 		 ) /*142*/
	.word SYMBOL_NAME(sys_flock		 ) /*143*/
	.word SYMBOL_NAME(sys_msync		 ) /*144*/
	.word SYMBOL_NAME(sys_readv		 ) /*145*/
	.word SYMBOL_NAME(sys_writev		 ) /*146*/
	.word SYMBOL_NAME(sys_getsid		 ) /*147*/
	.word SYMBOL_NAME(sys_fdatasync		 ) /*148*/
	.word SYMBOL_NAME(sys_sysctl		 ) /*149*/
	.word SYMBOL_NAME(sys_mlock		 ) /*150*/
	.word SYMBOL_NAME(sys_munlock		 ) /*151*/
	.word SYMBOL_NAME(sys_mlockall		 ) /*152*/
	.word SYMBOL_NAME(sys_munlockall	 ) /*153*/
	.word SYMBOL_NAME(sys_sched_setparam	 ) /*154*/
	.word SYMBOL_NAME(sys_sched_getparam	 ) /*155*/
	.word SYMBOL_NAME(sys_sched_setscheduler ) /*156*/
	.word SYMBOL_NAME(sys_sched_getscheduler ) /*157*/
	.word SYMBOL_NAME(sys_sched_yield	 ) /*158*/
	.word SYMBOL_NAME(sys_sched_get_priority_max	) /*159*/
	.word SYMBOL_NAME(sys_sched_get_priority_min	) /*160*/
	.word SYMBOL_NAME(sys_sched_rr_get_interval	) /*161*/
	.word SYMBOL_NAME(sys_nanosleep		 ) /*162*/
	.word SYMBOL_NAME(sys_mremap		 ) /*163*/
	.word SYMBOL_NAME(sys_setresuid		 ) /*164*/
	.word SYMBOL_NAME(sys_getresuid		 ) /*165*/
	.word SYMBOL_NAME(sys_getpagesize	 ) /*166*/
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /*167: old query_module syscall holder*/
	.word SYMBOL_NAME(sys_poll		 ) /*168*/
	.word SYMBOL_NAME(sys_nfsservctl	 ) /*169*/
	.word SYMBOL_NAME(sys_setresgid		 ) /*170*/
	.word SYMBOL_NAME(sys_getresgid		 ) /*171*/
	.word SYMBOL_NAME(sys_prctl		 ) /*172*/
	.word SYMBOL_NAME(sys_rt_sigreturn	 ) /*173*/
	.word SYMBOL_NAME(sys_rt_sigaction	 ) /*174*/
	.word SYMBOL_NAME(sys_rt_sigprocmask	 ) /*175*/
	.word SYMBOL_NAME(sys_rt_sigpending	 ) /*176*/
	.word SYMBOL_NAME(sys_rt_sigtimedwait	 ) /*177*/
	.word SYMBOL_NAME(sys_rt_sigqueueinfo	 ) /*178*/
	.word SYMBOL_NAME(sys_rt_sigsuspend	 ) /*179*/
	.word SYMBOL_NAME(sys_pread64		 ) /*180*/
	.word SYMBOL_NAME(sys_pwrite64		 ) /*181*/
	.word SYMBOL_NAME(sys_lchown		 ) /*182*/
	.word SYMBOL_NAME(sys_getcwd		 ) /*183*/
	.word SYMBOL_NAME(sys_capget		 ) /*184*/
	.word SYMBOL_NAME(sys_capset		 ) /*185*/
	.word SYMBOL_NAME(sys_sigaltstack	 ) /*186*/
	.word SYMBOL_NAME(sys_sendfile		 ) /*187*/
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /*188 sys_getpmsg not supported */
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /*189 sys_putpmsg not supported */
	.word SYMBOL_NAME(sys_vfork		 ) /*190*/
	.word SYMBOL_NAME(sys_getrlimit	 	 ) /*191*/
	.word SYMBOL_NAME(sys_mmap2		 ) /*192*/
	.word SYMBOL_NAME(sys_truncate64	 ) /*193*/
	.word SYMBOL_NAME(sys_ftruncate64	 ) /*194*/
	.word SYMBOL_NAME(sys_stat64		 ) /*195*/
	.word SYMBOL_NAME(sys_lstat64		 ) /*196*/
	.word SYMBOL_NAME(sys_fstat64		 ) /*197*/
	.word SYMBOL_NAME(sys_chown		 ) /*198*/
	.word SYMBOL_NAME(sys_getuid		 ) /*199*/
	.word SYMBOL_NAME(sys_getgid		 ) /*200*/
	.word SYMBOL_NAME(sys_geteuid		 ) /*201*/
	.word SYMBOL_NAME(sys_getegid		 ) /*202*/
	.word SYMBOL_NAME(sys_setreuid	   	 ) /*203*/
	.word SYMBOL_NAME(sys_setregid	 	 ) /*204*/
	.word SYMBOL_NAME(sys_getgroups  	 ) /*205*/
	.word SYMBOL_NAME(sys_setgroups 	 ) /*206*/
	.word SYMBOL_NAME(sys_fchown		 ) /*207*/
	.word SYMBOL_NAME(sys_setresuid 	 ) /*208*/
	.word SYMBOL_NAME(sys_getresuid	 	 ) /*209*/
	.word SYMBOL_NAME(sys_setresgid	 	 ) /*210*/
	.word SYMBOL_NAME(sys_getresgid	 	 ) /*211*/
	.word SYMBOL_NAME(sys_lchown		 ) /*212*/
	.word SYMBOL_NAME(sys_setuid		 ) /*213*/
	.word SYMBOL_NAME(sys_setgid		 ) /*214*/
	.word SYMBOL_NAME(sys_setfsuid	 	 ) /*215*/
	.word SYMBOL_NAME(sys_setfsgid	 	 ) /*216*/
	.word SYMBOL_NAME(sys_pivot_root	 ) /*217*/
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /*218*/ 
        .word SYMBOL_NAME(sys_ni_syscall 	 ) /*219*/
	.word SYMBOL_NAME(sys_getdents64	 ) /*220*/
	.word SYMBOL_NAME(sys_fcntl64		 ) /*221*/
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /*222: reserved for TUX*/
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /*223*/
	.word SYMBOL_NAME(sys_gettid		 ) /*224*/
	.word SYMBOL_NAME(sys_readahead		 ) /*225*/
	.word SYMBOL_NAME(sys_setxattr		 ) /*226*/
	.word SYMBOL_NAME(sys_lsetxattr		 ) /*227*/
	.word SYMBOL_NAME(sys_fsetxattr		 ) /*228*/
	.word SYMBOL_NAME(sys_getxattr		 ) /*229*/
	.word SYMBOL_NAME(sys_lgetxattr		 ) /*230*/
	.word SYMBOL_NAME(sys_fgetxattr		 ) /*231*/
	.word SYMBOL_NAME(sys_listxattr		 ) /*232*/
	.word SYMBOL_NAME(sys_llistxattr	 ) /*233*/
	.word SYMBOL_NAME(sys_flistxattr	 ) /*234*/
	.word SYMBOL_NAME(sys_removexattr	 ) /*235*/
	.word SYMBOL_NAME(sys_lremovexattr	 ) /*236*/
	.word SYMBOL_NAME(sys_fremovexattr	 ) /*237*/
	.word SYMBOL_NAME(sys_tkill		 ) /*238*/
	.word SYMBOL_NAME(sys_sendfile64	 ) /*239*/
	.word SYMBOL_NAME(sys_futex		 ) /*240*/
	.word SYMBOL_NAME(sys_sched_setaffinity	 ) /*241*/
	.word SYMBOL_NAME(sys_sched_getaffinity	 ) /*242*/
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /*243: sys_set_thread_area */
	.word SYMBOL_NAME(sys_ni_syscall	 ) /*244: sys_get_thread_area*/
	.word SYMBOL_NAME(sys_io_setup		 ) /*245*/
	.word SYMBOL_NAME(sys_io_destroy	 ) /*246*/
	.word SYMBOL_NAME(sys_io_getevents	 ) /*247*/
	.word SYMBOL_NAME(sys_io_submit		 ) /*248*/
	.word SYMBOL_NAME(sys_io_cancel		 ) /*249*/
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /*250: sys_alloc_hugepages*/
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /*251: sys_free_hugepages*/
	.word SYMBOL_NAME(sys_exit_group	 ) /*252*/
	.word SYMBOL_NAME(sys_lookup_dcookie	 ) /*253*/
	.word SYMBOL_NAME(sys_epoll_create	 ) /*254*/
	.word SYMBOL_NAME(sys_epoll_ctl	 	 ) /*255*/
	.word SYMBOL_NAME(sys_epoll_wait	 ) /*256*/
	.word SYMBOL_NAME(sys_remap_file_pages	 ) /*257*/
	.word SYMBOL_NAME(sys_set_tid_address	 ) /*258*/
	.word SYMBOL_NAME(sys_timer_create	 ) /*259*/
	.word SYMBOL_NAME(sys_timer_settime	 ) /*(__NR_timer_create+1)*/
	.word SYMBOL_NAME(sys_timer_gettime	 ) /*(__NR_timer_create+2)*/
	.word SYMBOL_NAME(sys_timer_getoverrun	 ) /*(__NR_timer_create+3)*/
	.word SYMBOL_NAME(sys_timer_delete	 ) /*(__NR_timer_create+4)*/
	.word SYMBOL_NAME(sys_clock_settime	 ) /*(__NR_timer_create+5)*/
	.word SYMBOL_NAME(sys_clock_gettime	 ) /*(__NR_timer_create+6)*/
	.word SYMBOL_NAME(sys_clock_getres	 ) /*(__NR_timer_create+7)*/
	.word SYMBOL_NAME(sys_clock_nanosleep	 ) /*(__NR_timer_create+8)*/
	.word SYMBOL_NAME(sys_statfs64		 ) /*268*/
	.word SYMBOL_NAME(sys_fstatfs64		 ) /*269*/
	.word SYMBOL_NAME(sys_tgkill		 ) /*270*/
	.word SYMBOL_NAME(sys_utimes		 ) /*271*/
	.word SYMBOL_NAME(sys_fadvise64_64	 ) /*272*/
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /*273: sys_vserver*/
	.word SYMBOL_NAME(sys_mbind		 ) /*274*/
	.word SYMBOL_NAME(sys_get_mempolicy	 ) /*275*/
	.word SYMBOL_NAME(sys_set_mempolicy	 ) /*276*/
	.word SYMBOL_NAME(sys_mq_open 		 ) /*277*/
	.word SYMBOL_NAME(sys_mq_unlink		 ) /*(__NR_mq_open+1)*/
	.word SYMBOL_NAME(sys_mq_timedsend	 ) /*(__NR_mq_open+2)*/
	.word SYMBOL_NAME(sys_mq_timedreceive	 ) /*(__NR_mq_open+3)*/
	.word SYMBOL_NAME(sys_mq_notify		 ) /*(__NR_mq_open+4)*/
	.word SYMBOL_NAME(sys_mq_getsetattr	 ) /*(__NR_mq_open+5)*/
	.word SYMBOL_NAME(sys_ni_syscall 	 ) /*283: reserved for kexec*/
	.word SYMBOL_NAME(sys_waitid		 ) /*284*/
	.word SYMBOL_NAME(sys_ni_syscall         ) /* sys_altroot not support 285*/
	.word SYMBOL_NAME(sys_add_key		 ) /*286*/
	.word SYMBOL_NAME(sys_request_key	 ) /*287*/
	.word SYMBOL_NAME(sys_keyctl		 ) /*288*/

