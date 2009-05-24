#ifndef _LTT_FACILITY_KERNEL_ARCH_H_
#define _LTT_FACILITY_KERNEL_ARCH_H_

#include <linux/types.h>
#include <asm/ltt/ltt-facility-id-kernel_arch_mips.h>
#include <linux/ltt-core.h>

/* Named types */

enum lttng_syscall_name {
	LTTNG_o32_base = 3999,
	LTTNG_o32_syscall = 4000,
	LTTNG_o32_exit = 4001,
	LTTNG_o32_fork = 4002,
	LTTNG_o32_read = 4003,
	LTTNG_o32_write = 4004,
	LTTNG_o32_open = 4005,
	LTTNG_o32_close = 4006,
	LTTNG_o32_waitpid = 4007,
	LTTNG_o32_creat = 4008,
	LTTNG_o32_link = 4009,
	LTTNG_o32_unlink = 4010,
	LTTNG_o32_execve = 4011,
	LTTNG_o32_chdir = 4012,
	LTTNG_o32_time = 4013,
	LTTNG_o32_mknod = 4014,
	LTTNG_o32_chmod = 4015,
	LTTNG_o32_lchown = 4016,
	LTTNG_o32_break = 4017,
	LTTNG_o32_unused18 = 4018,
	LTTNG_o32_lseek = 4019,
	LTTNG_o32_getpid = 4020,
	LTTNG_o32_mount = 4021,
	LTTNG_o32_umount = 4022,
	LTTNG_o32_setuid = 4023,
	LTTNG_o32_getuid = 4024,
	LTTNG_o32_stime = 4025,
	LTTNG_o32_ptrace = 4026,
	LTTNG_o32_alarm = 4027,
	LTTNG_o32_unused28 = 4028,
	LTTNG_o32_pause = 4029,
	LTTNG_o32_utime = 4030,
	LTTNG_o32_stty = 4031,
	LTTNG_o32_gtty = 4032,
	LTTNG_o32_access = 4033,
	LTTNG_o32_nice = 4034,
	LTTNG_o32_ftime = 4035,
	LTTNG_o32_sync = 4036,
	LTTNG_o32_kill = 4037,
	LTTNG_o32_rename = 4038,
	LTTNG_o32_mkdir = 4039,
	LTTNG_o32_rmdir = 4040,
	LTTNG_o32_dup = 4041,
	LTTNG_o32_pipe = 4042,
	LTTNG_o32_times = 4043,
	LTTNG_o32_prof = 4044,
	LTTNG_o32_brk = 4045,
	LTTNG_o32_setgid = 4046,
	LTTNG_o32_getgid = 4047,
	LTTNG_o32_signal = 4048,
	LTTNG_o32_geteuid = 4049,
	LTTNG_o32_getegid = 4050,
	LTTNG_o32_acct = 4051,
	LTTNG_o32_umount2 = 4052,
	LTTNG_o32_lock = 4053,
	LTTNG_o32_ioctl = 4054,
	LTTNG_o32_fcntl = 4055,
	LTTNG_o32_mpx = 4056,
	LTTNG_o32_setpgid = 4057,
	LTTNG_o32_ulimit = 4058,
	LTTNG_o32_unused59 = 4059,
	LTTNG_o32_umask = 4060,
	LTTNG_o32_chroot = 4061,
	LTTNG_o32_ustat = 4062,
	LTTNG_o32_dup2 = 4063,
	LTTNG_o32_getppid = 4064,
	LTTNG_o32_getpgrp = 4065,
	LTTNG_o32_setsid = 4066,
	LTTNG_o32_sigaction = 4067,
	LTTNG_o32_sgetmask = 4068,
	LTTNG_o32_ssetmask = 4069,
	LTTNG_o32_setreuid = 4070,
	LTTNG_o32_setregid = 4071,
	LTTNG_o32_sigsuspend = 4072,
	LTTNG_o32_sigpending = 4073,
	LTTNG_o32_sethostname = 4074,
	LTTNG_o32_setrlimit = 4075,
	LTTNG_o32_getrlimit = 4076,
	LTTNG_o32_getrusage = 4077,
	LTTNG_o32_gettimeofday = 4078,
	LTTNG_o32_settimeofday = 4079,
	LTTNG_o32_getgroups = 4080,
	LTTNG_o32_setgroups = 4081,
	LTTNG_o32_reserved82 = 4082,
	LTTNG_o32_symlink = 4083,
	LTTNG_o32_unused84 = 4084,
	LTTNG_o32_readlink = 4085,
	LTTNG_o32_uselib = 4086,
	LTTNG_o32_swapon = 4087,
	LTTNG_o32_reboot = 4088,
	LTTNG_o32_readdir = 4089,
	LTTNG_o32_mmap = 4090,
	LTTNG_o32_munmap = 4091,
	LTTNG_o32_truncate = 4092,
	LTTNG_o32_ftruncate = 4093,
	LTTNG_o32_fchmod = 4094,
	LTTNG_o32_fchown = 4095,
	LTTNG_o32_getpriority = 4096,
	LTTNG_o32_setpriority = 4097,
	LTTNG_o32_profil = 4098,
	LTTNG_o32_statfs = 4099,
	LTTNG_o32_fstatfs = 4100,
	LTTNG_o32_ioperm = 4101,
	LTTNG_o32_socketcall = 4102,
	LTTNG_o32_syslog = 4103,
	LTTNG_o32_setitimer = 4104,
	LTTNG_o32_getitimer = 4105,
	LTTNG_o32_stat = 4106,
	LTTNG_o32_lstat = 4107,
	LTTNG_o32_fstat = 4108,
	LTTNG_o32_unused109 = 4109,
	LTTNG_o32_iopl = 4110,
	LTTNG_o32_vhangup = 4111,
	LTTNG_o32_idle = 4112,
	LTTNG_o32_vm86 = 4113,
	LTTNG_o32_wait4 = 4114,
	LTTNG_o32_swapoff = 4115,
	LTTNG_o32_sysinfo = 4116,
	LTTNG_o32_ipc = 4117,
	LTTNG_o32_fsync = 4118,
	LTTNG_o32_sigreturn = 4119,
	LTTNG_o32_clone = 4120,
	LTTNG_o32_setdomainname = 4121,
	LTTNG_o32_uname = 4122,
	LTTNG_o32_modify_ldt = 4123,
	LTTNG_o32_adjtimex = 4124,
	LTTNG_o32_mprotect = 4125,
	LTTNG_o32_sigprocmask = 4126,
	LTTNG_o32_create_module = 4127,
	LTTNG_o32_init_module = 4128,
	LTTNG_o32_delete_module = 4129,
	LTTNG_o32_get_kernel_syms = 4130,
	LTTNG_o32_quotactl = 4131,
	LTTNG_o32_getpgid = 4132,
	LTTNG_o32_fchdir = 4133,
	LTTNG_o32_bdflush = 4134,
	LTTNG_o32_sysfs = 4135,
	LTTNG_o32_personality = 4136,
	LTTNG_o32_afs_syscall = 4137,
	LTTNG_o32_setfsuid = 4138,
	LTTNG_o32_setfsgid = 4139,
	LTTNG_o32__llseek = 4140,
	LTTNG_o32_getdents = 4141,
	LTTNG_o32__newselect = 4142,
	LTTNG_o32_flock = 4143,
	LTTNG_o32_msync = 4144,
	LTTNG_o32_readv = 4145,
	LTTNG_o32_writev = 4146,
	LTTNG_o32_cacheflush = 4147,
	LTTNG_o32_cachectl = 4148,
	LTTNG_o32_sysmips = 4149,
	LTTNG_o32_unused150 = 4150,
	LTTNG_o32_getsid = 4151,
	LTTNG_o32_fdatasync = 4152,
	LTTNG_o32__sysctl = 4153,
	LTTNG_o32_mlock = 4154,
	LTTNG_o32_munlock = 4155,
	LTTNG_o32_mlockall = 4156,
	LTTNG_o32_munlockall = 4157,
	LTTNG_o32_sched_setparam = 4158,
	LTTNG_o32_sched_getparam = 4159,
	LTTNG_o32_sched_setscheduler = 4160,
	LTTNG_o32_sched_getscheduler = 4161,
	LTTNG_o32_sched_yield = 4162,
	LTTNG_o32_sched_get_priority_max = 4163,
	LTTNG_o32_sched_get_priority_min = 4164,
	LTTNG_o32_sched_rr_get_interval = 4165,
	LTTNG_o32_nanosleep = 4166,
	LTTNG_o32_mremap = 4167,
	LTTNG_o32_accept = 4168,
	LTTNG_o32_bind = 4169,
	LTTNG_o32_connect = 4170,
	LTTNG_o32_getpeername = 4171,
	LTTNG_o32_getsockname = 4172,
	LTTNG_o32_getsockopt = 4173,
	LTTNG_o32_listen = 4174,
	LTTNG_o32_recv = 4175,
	LTTNG_o32_recvfrom = 4176,
	LTTNG_o32_recvmsg = 4177,
	LTTNG_o32_send = 4178,
	LTTNG_o32_sendmsg = 4179,
	LTTNG_o32_sendto = 4180,
	LTTNG_o32_setsockopt = 4181,
	LTTNG_o32_shutdown = 4182,
	LTTNG_o32_socket = 4183,
	LTTNG_o32_socketpair = 4184,
	LTTNG_o32_setresuid = 4185,
	LTTNG_o32_getresuid = 4186,
	LTTNG_o32_query_module = 4187,
	LTTNG_o32_poll = 4188,
	LTTNG_o32_nfsservctl = 4189,
	LTTNG_o32_setresgid = 4190,
	LTTNG_o32_getresgid = 4191,
	LTTNG_o32_prctl = 4192,
	LTTNG_o32_rt_sigreturn = 4193,
	LTTNG_o32_rt_sigaction = 4194,
	LTTNG_o32_rt_sigprocmask = 4195,
	LTTNG_o32_rt_sigpending = 4196,
	LTTNG_o32_rt_sigtimedwait = 4197,
	LTTNG_o32_rt_sigqueueinfo = 4198,
	LTTNG_o32_rt_sigsuspend = 4199,
	LTTNG_o32_pread64 = 4200,
	LTTNG_o32_pwrite64 = 4201,
	LTTNG_o32_chown = 4202,
	LTTNG_o32_getcwd = 4203,
	LTTNG_o32_capget = 4204,
	LTTNG_o32_capset = 4205,
	LTTNG_o32_sigaltstack = 4206,
	LTTNG_o32_sendfile = 4207,
	LTTNG_o32_getpmsg = 4208,
	LTTNG_o32_putpmsg = 4209,
	LTTNG_o32_mmap2 = 4210,
	LTTNG_o32_truncate64 = 4211,
	LTTNG_o32_ftruncate64 = 4212,
	LTTNG_o32_stat64 = 4213,
	LTTNG_o32_lstat64 = 4214,
	LTTNG_o32_fstat64 = 4215,
	LTTNG_o32_pivot_root = 4216,
	LTTNG_o32_mincore = 4217,
	LTTNG_o32_madvise = 4218,
	LTTNG_o32_getdents64 = 4219,
	LTTNG_o32_fcntl64 = 4220,
	LTTNG_o32_reserved221 = 4221,
	LTTNG_o32_gettid = 4222,
	LTTNG_o32_readahead = 4223,
	LTTNG_o32_setxattr = 4224,
	LTTNG_o32_lsetxattr = 4225,
	LTTNG_o32_fsetxattr = 4226,
	LTTNG_o32_getxattr = 4227,
	LTTNG_o32_lgetxattr = 4228,
	LTTNG_o32_fgetxattr = 4229,
	LTTNG_o32_listxattr = 4230,
	LTTNG_o32_llistxattr = 4231,
	LTTNG_o32_flistxattr = 4232,
	LTTNG_o32_removexattr = 4233,
	LTTNG_o32_lremovexattr = 4234,
	LTTNG_o32_fremovexattr = 4235,
	LTTNG_o32_tkill = 4236,
	LTTNG_o32_sendfile64 = 4237,
	LTTNG_o32_futex = 4238,
	LTTNG_o32_sched_setaffinity = 4239,
	LTTNG_o32_sched_getaffinity = 4240,
	LTTNG_o32_io_setup = 4241,
	LTTNG_o32_io_destroy = 4242,
	LTTNG_o32_io_getevents = 4243,
	LTTNG_o32_io_submit = 4244,
	LTTNG_o32_io_cancel = 4245,
	LTTNG_o32_exit_group = 4246,
	LTTNG_o32_lookup_dcookie = 4247,
	LTTNG_o32_epoll_create = 4248,
	LTTNG_o32_epoll_ctl = 4249,
	LTTNG_o32_epoll_wait = 4250,
	LTTNG_o32_remap_file_pages = 4251,
	LTTNG_o32_set_tid_address = 4252,
	LTTNG_o32_restart_syscall = 4253,
	LTTNG_o32_fadvise64 = 4254,
	LTTNG_o32_statfs64 = 4255,
	LTTNG_o32_fstatfs64 = 4256,
	LTTNG_o32_timer_create = 4257,
	LTTNG_o32_timer_settime = 4258,
	LTTNG_o32_timer_gettime = 4259,
	LTTNG_o32_timer_getoverrun = 4260,
	LTTNG_o32_timer_delete = 4261,
	LTTNG_o32_clock_settime = 4262,
	LTTNG_o32_clock_gettime = 4263,
	LTTNG_o32_clock_getres = 4264,
	LTTNG_o32_clock_nanosleep = 4265,
	LTTNG_o32_tgkill = 4266,
	LTTNG_o32_utimes = 4267,
	LTTNG_o32_mbind = 4268,
	LTTNG_o32_get_mempolicy = 4269,
	LTTNG_o32_set_mempolicy = 4270,
	LTTNG_o32_mq_open = 4271,
	LTTNG_o32_mq_unlink = 4272,
	LTTNG_o32_mq_timedsend = 4273,
	LTTNG_o32_mq_timedreceive = 4274,
	LTTNG_o32_mq_notify = 4275,
	LTTNG_o32_mq_getsetattr = 4276,
	LTTNG_o32_vserver = 4277,
	LTTNG_o32_waitid = 4278,
	LTTNG_o32_add_key = 4280,
	LTTNG_o32_request_key = 4281,
	LTTNG_o32_keyctl = 4282,
	LTTNG_o32_set_thread_area = 4283,
	LTTNG_o32_inotify_init = 4284,
	LTTNG_o32_inotify_add_watch = 4285,
	LTTNG_o32_inotify_rm_watch = 4286,
	LTTNG_o32_migrate_pages = 4287,
	LTTNG_o32_openat = 4288,
	LTTNG_o32_mkdirat = 4289,
	LTTNG_o32_mknodat = 4290,
	LTTNG_o32_fchownat = 4291,
	LTTNG_o32_futimesat = 4292,
	LTTNG_o32_fstatat64 = 4293,
	LTTNG_o32_unlinkat = 4294,
	LTTNG_o32_renameat = 4295,
	LTTNG_o32_linkat = 4296,
	LTTNG_o32_symlinkat = 4297,
	LTTNG_o32_readlinkat = 4298,
	LTTNG_o32_fchmodat = 4299,
	LTTNG_o32_faccessat = 4300,
	LTTNG_o32_pselect6 = 4301,
	LTTNG_o32_ppoll = 4302,
	LTTNG_o32_unshare = 4303,
	LTTNG_o32_set_robust_list = 4304,
	LTTNG_o32_get_robust_list = 4305,
	LTTNG_o32_splice = 4306,
	LTTNG_o32_sync_file_range = 4307,
	LTTNG_o32_tee = 4308,
	LTTNG_o32_vmsplice = 4309,
	LTTNG_l64_base = 4999,
	LTTNG_l64_read = 5000,
	LTTNG_l64_write = 5001,
	LTTNG_l64_open = 5002,
	LTTNG_l64_close = 5003,
	LTTNG_l64_stat = 5004,
	LTTNG_l64_fstat = 5005,
	LTTNG_l64_lstat = 5006,
	LTTNG_l64_poll = 5007,
	LTTNG_l64_lseek = 5008,
	LTTNG_l64_mmap = 5009,
	LTTNG_l64_mprotect = 5010,
	LTTNG_l64_munmap = 5011,
	LTTNG_l64_brk = 5012,
	LTTNG_l64_rt_sigaction = 5013,
	LTTNG_l64_rt_sigprocmask = 5014,
	LTTNG_l64_ioctl = 5015,
	LTTNG_l64_pread64 = 5016,
	LTTNG_l64_pwrite64 = 5017,
	LTTNG_l64_readv = 5018,
	LTTNG_l64_writev = 5019,
	LTTNG_l64_access = 5020,
	LTTNG_l64_pipe = 5021,
	LTTNG_l64__newselect = 5022,
	LTTNG_l64_sched_yield = 5023,
	LTTNG_l64_mremap = 5024,
	LTTNG_l64_msync = 5025,
	LTTNG_l64_mincore = 5026,
	LTTNG_l64_madvise = 5027,
	LTTNG_l64_shmget = 5028,
	LTTNG_l64_shmat = 5029,
	LTTNG_l64_shmctl = 5030,
	LTTNG_l64_dup = 5031,
	LTTNG_l64_dup2 = 5032,
	LTTNG_l64_pause = 5033,
	LTTNG_l64_nanosleep = 5034,
	LTTNG_l64_getitimer = 5035,
	LTTNG_l64_setitimer = 5036,
	LTTNG_l64_alarm = 5037,
	LTTNG_l64_getpid = 5038,
	LTTNG_l64_sendfile = 5039,
	LTTNG_l64_socket = 5040,
	LTTNG_l64_connect = 5041,
	LTTNG_l64_accept = 5042,
	LTTNG_l64_sendto = 5043,
	LTTNG_l64_recvfrom = 5044,
	LTTNG_l64_sendmsg = 5045,
	LTTNG_l64_recvmsg = 5046,
	LTTNG_l64_shutdown = 5047,
	LTTNG_l64_bind = 5048,
	LTTNG_l64_listen = 5049,
	LTTNG_l64_getsockname = 5050,
	LTTNG_l64_getpeername = 5051,
	LTTNG_l64_socketpair = 5052,
	LTTNG_l64_setsockopt = 5053,
	LTTNG_l64_getsockopt = 5054,
	LTTNG_l64_clone = 5055,
	LTTNG_l64_fork = 5056,
	LTTNG_l64_execve = 5057,
	LTTNG_l64_exit = 5058,
	LTTNG_l64_wait4 = 5059,
	LTTNG_l64_kill = 5060,
	LTTNG_l64_uname = 5061,
	LTTNG_l64_semget = 5062,
	LTTNG_l64_semop = 5063,
	LTTNG_l64_semctl = 5064,
	LTTNG_l64_shmdt = 5065,
	LTTNG_l64_msgget = 5066,
	LTTNG_l64_msgsnd = 5067,
	LTTNG_l64_msgrcv = 5068,
	LTTNG_l64_msgctl = 5069,
	LTTNG_l64_fcntl = 5070,
	LTTNG_l64_flock = 5071,
	LTTNG_l64_fsync = 5072,
	LTTNG_l64_fdatasync = 5073,
	LTTNG_l64_truncate = 5074,
	LTTNG_l64_ftruncate = 5075,
	LTTNG_l64_getdents = 5076,
	LTTNG_l64_getcwd = 5077,
	LTTNG_l64_chdir = 5078,
	LTTNG_l64_fchdir = 5079,
	LTTNG_l64_rename = 5080,
	LTTNG_l64_mkdir = 5081,
	LTTNG_l64_rmdir = 5082,
	LTTNG_l64_creat = 5083,
	LTTNG_l64_link = 5084,
	LTTNG_l64_unlink = 5085,
	LTTNG_l64_symlink = 5086,
	LTTNG_l64_readlink = 5087,
	LTTNG_l64_chmod = 5088,
	LTTNG_l64_fchmod = 5089,
	LTTNG_l64_chown = 5090,
	LTTNG_l64_fchown = 5091,
	LTTNG_l64_lchown = 5092,
	LTTNG_l64_umask = 5093,
	LTTNG_l64_gettimeofday = 5094,
	LTTNG_l64_getrlimit = 5095,
	LTTNG_l64_getrusage = 5096,
	LTTNG_l64_sysinfo = 5097,
	LTTNG_l64_times = 5098,
	LTTNG_l64_ptrace = 5099,
	LTTNG_l64_getuid = 5100,
	LTTNG_l64_syslog = 5101,
	LTTNG_l64_getgid = 5102,
	LTTNG_l64_setuid = 5103,
	LTTNG_l64_setgid = 5104,
	LTTNG_l64_geteuid = 5105,
	LTTNG_l64_getegid = 5106,
	LTTNG_l64_setpgid = 5107,
	LTTNG_l64_getppid = 5108,
	LTTNG_l64_getpgrp = 5109,
	LTTNG_l64_setsid = 5110,
	LTTNG_l64_setreuid = 5111,
	LTTNG_l64_setregid = 5112,
	LTTNG_l64_getgroups = 5113,
	LTTNG_l64_setgroups = 5114,
	LTTNG_l64_setresuid = 5115,
	LTTNG_l64_getresuid = 5116,
	LTTNG_l64_setresgid = 5117,
	LTTNG_l64_getresgid = 5118,
	LTTNG_l64_getpgid = 5119,
	LTTNG_l64_setfsuid = 5120,
	LTTNG_l64_setfsgid = 5121,
	LTTNG_l64_getsid = 5122,
	LTTNG_l64_capget = 5123,
	LTTNG_l64_capset = 5124,
	LTTNG_l64_rt_sigpending = 5125,
	LTTNG_l64_rt_sigtimedwait = 5126,
	LTTNG_l64_rt_sigqueueinfo = 5127,
	LTTNG_l64_rt_sigsuspend = 5128,
	LTTNG_l64_sigaltstack = 5129,
	LTTNG_l64_utime = 5130,
	LTTNG_l64_mknod = 5131,
	LTTNG_l64_personality = 5132,
	LTTNG_l64_ustat = 5133,
	LTTNG_l64_statfs = 5134,
	LTTNG_l64_fstatfs = 5135,
	LTTNG_l64_sysfs = 5136,
	LTTNG_l64_getpriority = 5137,
	LTTNG_l64_setpriority = 5138,
	LTTNG_l64_sched_setparam = 5139,
	LTTNG_l64_sched_getparam = 5140,
	LTTNG_l64_sched_setscheduler = 5141,
	LTTNG_l64_sched_getscheduler = 5142,
	LTTNG_l64_sched_get_priority_max = 5143,
	LTTNG_l64_sched_get_priority_min = 5144,
	LTTNG_l64_sched_rr_get_interval = 5145,
	LTTNG_l64_mlock = 5146,
	LTTNG_l64_munlock = 5147,
	LTTNG_l64_mlockall = 5148,
	LTTNG_l64_munlockall = 5149,
	LTTNG_l64_vhangup = 5150,
	LTTNG_l64_pivot_root = 5151,
	LTTNG_l64__sysctl = 5152,
	LTTNG_l64_prctl = 5153,
	LTTNG_l64_adjtimex = 5154,
	LTTNG_l64_setrlimit = 5155,
	LTTNG_l64_chroot = 5156,
	LTTNG_l64_sync = 5157,
	LTTNG_l64_acct = 5158,
	LTTNG_l64_settimeofday = 5159,
	LTTNG_l64_mount = 5160,
	LTTNG_l64_umount2 = 5161,
	LTTNG_l64_swapon = 5162,
	LTTNG_l64_swapoff = 5163,
	LTTNG_l64_reboot = 5164,
	LTTNG_l64_sethostname = 5165,
	LTTNG_l64_setdomainname = 5166,
	LTTNG_l64_create_module = 5167,
	LTTNG_l64_init_module = 5168,
	LTTNG_l64_delete_module = 5169,
	LTTNG_l64_get_kernel_syms = 5170,
	LTTNG_l64_query_module = 5171,
	LTTNG_l64_quotactl = 5172,
	LTTNG_l64_nfsservctl = 5173,
	LTTNG_l64_getpmsg = 5174,
	LTTNG_l64_putpmsg = 5175,
	LTTNG_l64_afs_syscall = 5176,
	LTTNG_l64_reserved177 = 5177,
	LTTNG_l64_gettid = 5178,
	LTTNG_l64_readahead = 5179,
	LTTNG_l64_setxattr = 5180,
	LTTNG_l64_lsetxattr = 5181,
	LTTNG_l64_fsetxattr = 5182,
	LTTNG_l64_getxattr = 5183,
	LTTNG_l64_lgetxattr = 5184,
	LTTNG_l64_fgetxattr = 5185,
	LTTNG_l64_listxattr = 5186,
	LTTNG_l64_llistxattr = 5187,
	LTTNG_l64_flistxattr = 5188,
	LTTNG_l64_removexattr = 5189,
	LTTNG_l64_lremovexattr = 5190,
	LTTNG_l64_fremovexattr = 5191,
	LTTNG_l64_tkill = 5192,
	LTTNG_l64_reserved193 = 5193,
	LTTNG_l64_futex = 5194,
	LTTNG_l64_sched_setaffinity = 5195,
	LTTNG_l64_sched_getaffinity = 5196,
	LTTNG_l64_cacheflush = 5197,
	LTTNG_l64_cachectl = 5198,
	LTTNG_l64_sysmips = 5199,
	LTTNG_l64_io_setup = 5200,
	LTTNG_l64_io_destroy = 5201,
	LTTNG_l64_io_getevents = 5202,
	LTTNG_l64_io_submit = 5203,
	LTTNG_l64_io_cancel = 5204,
	LTTNG_l64_exit_group = 5205,
	LTTNG_l64_lookup_dcookie = 5206,
	LTTNG_l64_epoll_create = 5207,
	LTTNG_l64_epoll_ctl = 5208,
	LTTNG_l64_epoll_wait = 5209,
	LTTNG_l64_remap_file_pages = 5210,
	LTTNG_l64_rt_sigreturn = 5211,
	LTTNG_l64_set_tid_address = 5212,
	LTTNG_l64_restart_syscall = 5213,
	LTTNG_l64_semtimedop = 5214,
	LTTNG_l64_fadvise64 = 5215,
	LTTNG_l64_timer_create = 5216,
	LTTNG_l64_timer_settime = 5217,
	LTTNG_l64_timer_gettime = 5218,
	LTTNG_l64_timer_getoverrun = 5219,
	LTTNG_l64_timer_delete = 5220,
	LTTNG_l64_clock_settime = 5221,
	LTTNG_l64_clock_gettime = 5222,
	LTTNG_l64_clock_getres = 5223,
	LTTNG_l64_clock_nanosleep = 5224,
	LTTNG_l64_tgkill = 5225,
	LTTNG_l64_utimes = 5226,
	LTTNG_l64_mbind = 5227,
	LTTNG_l64_get_mempolicy = 5228,
	LTTNG_l64_set_mempolicy = 5229,
	LTTNG_l64_mq_open = 5230,
	LTTNG_l64_mq_unlink = 5231,
	LTTNG_l64_mq_timedsend = 5232,
	LTTNG_l64_mq_timedreceive = 5233,
	LTTNG_l64_mq_notify = 5234,
	LTTNG_l64_mq_getsetattr = 5235,
	LTTNG_l64_vserver = 5236,
	LTTNG_l64_waitid = 5237,
	LTTNG_l64_add_key = 5239,
	LTTNG_l64_request_key = 5240,
	LTTNG_l64_keyctl = 5241,
	LTTNG_l64_set_thread_area = 5242,
	LTTNG_l64_inotify_init = 5243,
	LTTNG_l64_inotify_add_watch = 5244,
	LTTNG_l64_inotify_rm_watch = 5245,
	LTTNG_l64_migrate_pages = 5246,
	LTTNG_l64_openat = 5247,
	LTTNG_l64_mkdirat = 5248,
	LTTNG_l64_mknodat = 5249,
	LTTNG_l64_fchownat = 5250,
	LTTNG_l64_futimesat = 5251,
	LTTNG_l64_fstatat64 = 5252,
	LTTNG_l64_unlinkat = 5253,
	LTTNG_l64_renameat = 5254,
	LTTNG_l64_linkat = 5255,
	LTTNG_l64_symlinkat = 5256,
	LTTNG_l64_readlinkat = 5257,
	LTTNG_l64_fchmodat = 5258,
	LTTNG_l64_faccessat = 5259,
	LTTNG_l64_pselect6 = 5260,
	LTTNG_l64_ppoll = 5261,
	LTTNG_l64_unshare = 5262,
	LTTNG_l64_set_robust_list = 5263,
	LTTNG_l64_get_robust_list = 5264,
	LTTNG_l64_splice = 5265,
	LTTNG_l64_sync_file_range = 5266,
	LTTNG_l64_tee = 5267,
	LTTNG_l64_vmsplice = 5268,
	LTTNG_N32_base = 5999,
	LTTNG_N32_read = 6000,
	LTTNG_N32_write = 6001,
	LTTNG_N32_open = 6002,
	LTTNG_N32_close = 6003,
	LTTNG_N32_stat = 6004,
	LTTNG_N32_fstat = 6005,
	LTTNG_N32_lstat = 6006,
	LTTNG_N32_poll = 6007,
	LTTNG_N32_lseek = 6008,
	LTTNG_N32_mmap = 6009,
	LTTNG_N32_mprotect = 6010,
	LTTNG_N32_munmap = 6011,
	LTTNG_N32_brk = 6012,
	LTTNG_N32_rt_sigaction = 6013,
	LTTNG_N32_rt_sigprocmask = 6014,
	LTTNG_N32_ioctl = 6015,
	LTTNG_N32_pread64 = 6016,
	LTTNG_N32_pwrite64 = 6017,
	LTTNG_N32_readv = 6018,
	LTTNG_N32_writev = 6019,
	LTTNG_N32_access = 6020,
	LTTNG_N32_pipe = 6021,
	LTTNG_N32__newselect = 6022,
	LTTNG_N32_sched_yield = 6023,
	LTTNG_N32_mremap = 6024,
	LTTNG_N32_msync = 6025,
	LTTNG_N32_mincore = 6026,
	LTTNG_N32_madvise = 6027,
	LTTNG_N32_shmget = 6028,
	LTTNG_N32_shmat = 6029,
	LTTNG_N32_shmctl = 6030,
	LTTNG_N32_dup = 6031,
	LTTNG_N32_dup2 = 6032,
	LTTNG_N32_pause = 6033,
	LTTNG_N32_nanosleep = 6034,
	LTTNG_N32_getitimer = 6035,
	LTTNG_N32_setitimer = 6036,
	LTTNG_N32_alarm = 6037,
	LTTNG_N32_getpid = 6038,
	LTTNG_N32_sendfile = 6039,
	LTTNG_N32_socket = 6040,
	LTTNG_N32_connect = 6041,
	LTTNG_N32_accept = 6042,
	LTTNG_N32_sendto = 6043,
	LTTNG_N32_recvfrom = 6044,
	LTTNG_N32_sendmsg = 6045,
	LTTNG_N32_recvmsg = 6046,
	LTTNG_N32_shutdown = 6047,
	LTTNG_N32_bind = 6048,
	LTTNG_N32_listen = 6049,
	LTTNG_N32_getsockname = 6050,
	LTTNG_N32_getpeername = 6051,
	LTTNG_N32_socketpair = 6052,
	LTTNG_N32_setsockopt = 6053,
	LTTNG_N32_getsockopt = 6054,
	LTTNG_N32_clone = 6055,
	LTTNG_N32_fork = 6056,
	LTTNG_N32_execve = 6057,
	LTTNG_N32_exit = 6058,
	LTTNG_N32_wait4 = 6059,
	LTTNG_N32_kill = 6060,
	LTTNG_N32_uname = 6061,
	LTTNG_N32_semget = 6062,
	LTTNG_N32_semop = 6063,
	LTTNG_N32_semctl = 6064,
	LTTNG_N32_shmdt = 6065,
	LTTNG_N32_msgget = 6066,
	LTTNG_N32_msgsnd = 6067,
	LTTNG_N32_msgrcv = 6068,
	LTTNG_N32_msgctl = 6069,
	LTTNG_N32_fcntl = 6070,
	LTTNG_N32_flock = 6071,
	LTTNG_N32_fsync = 6072,
	LTTNG_N32_fdatasync = 6073,
	LTTNG_N32_truncate = 6074,
	LTTNG_N32_ftruncate = 6075,
	LTTNG_N32_getdents = 6076,
	LTTNG_N32_getcwd = 6077,
	LTTNG_N32_chdir = 6078,
	LTTNG_N32_fchdir = 6079,
	LTTNG_N32_rename = 6080,
	LTTNG_N32_mkdir = 6081,
	LTTNG_N32_rmdir = 6082,
	LTTNG_N32_creat = 6083,
	LTTNG_N32_link = 6084,
	LTTNG_N32_unlink = 6085,
	LTTNG_N32_symlink = 6086,
	LTTNG_N32_readlink = 6087,
	LTTNG_N32_chmod = 6088,
	LTTNG_N32_fchmod = 6089,
	LTTNG_N32_chown = 6090,
	LTTNG_N32_fchown = 6091,
	LTTNG_N32_lchown = 6092,
	LTTNG_N32_umask = 6093,
	LTTNG_N32_gettimeofday = 6094,
	LTTNG_N32_getrlimit = 6095,
	LTTNG_N32_getrusage = 6096,
	LTTNG_N32_sysinfo = 6097,
	LTTNG_N32_times = 6098,
	LTTNG_N32_ptrace = 6099,
	LTTNG_N32_getuid = 6100,
	LTTNG_N32_syslog = 6101,
	LTTNG_N32_getgid = 6102,
	LTTNG_N32_setuid = 6103,
	LTTNG_N32_setgid = 6104,
	LTTNG_N32_geteuid = 6105,
	LTTNG_N32_getegid = 6106,
	LTTNG_N32_setpgid = 6107,
	LTTNG_N32_getppid = 6108,
	LTTNG_N32_getpgrp = 6109,
	LTTNG_N32_setsid = 6110,
	LTTNG_N32_setreuid = 6111,
	LTTNG_N32_setregid = 6112,
	LTTNG_N32_getgroups = 6113,
	LTTNG_N32_setgroups = 6114,
	LTTNG_N32_setresuid = 6115,
	LTTNG_N32_getresuid = 6116,
	LTTNG_N32_setresgid = 6117,
	LTTNG_N32_getresgid = 6118,
	LTTNG_N32_getpgid = 6119,
	LTTNG_N32_setfsuid = 6120,
	LTTNG_N32_setfsgid = 6121,
	LTTNG_N32_getsid = 6122,
	LTTNG_N32_capget = 6123,
	LTTNG_N32_capset = 6124,
	LTTNG_N32_rt_sigpending = 6125,
	LTTNG_N32_rt_sigtimedwait = 6126,
	LTTNG_N32_rt_sigqueueinfo = 6127,
	LTTNG_N32_rt_sigsuspend = 6128,
	LTTNG_N32_sigaltstack = 6129,
	LTTNG_N32_utime = 6130,
	LTTNG_N32_mknod = 6131,
	LTTNG_N32_personality = 6132,
	LTTNG_N32_ustat = 6133,
	LTTNG_N32_statfs = 6134,
	LTTNG_N32_fstatfs = 6135,
	LTTNG_N32_sysfs = 6136,
	LTTNG_N32_getpriority = 6137,
	LTTNG_N32_setpriority = 6138,
	LTTNG_N32_sched_setparam = 6139,
	LTTNG_N32_sched_getparam = 6140,
	LTTNG_N32_sched_setscheduler = 6141,
	LTTNG_N32_sched_getscheduler = 6142,
	LTTNG_N32_sched_get_priority_max = 6143,
	LTTNG_N32_sched_get_priority_min = 6144,
	LTTNG_N32_sched_rr_get_interval = 6145,
	LTTNG_N32_mlock = 6146,
	LTTNG_N32_munlock = 6147,
	LTTNG_N32_mlockall = 6148,
	LTTNG_N32_munlockall = 6149,
	LTTNG_N32_vhangup = 6150,
	LTTNG_N32_pivot_root = 6151,
	LTTNG_N32__sysctl = 6152,
	LTTNG_N32_prctl = 6153,
	LTTNG_N32_adjtimex = 6154,
	LTTNG_N32_setrlimit = 6155,
	LTTNG_N32_chroot = 6156,
	LTTNG_N32_sync = 6157,
	LTTNG_N32_acct = 6158,
	LTTNG_N32_settimeofday = 6159,
	LTTNG_N32_mount = 6160,
	LTTNG_N32_umount2 = 6161,
	LTTNG_N32_swapon = 6162,
	LTTNG_N32_swapoff = 6163,
	LTTNG_N32_reboot = 6164,
	LTTNG_N32_sethostname = 6165,
	LTTNG_N32_setdomainname = 6166,
	LTTNG_N32_create_module = 6167,
	LTTNG_N32_init_module = 6168,
	LTTNG_N32_delete_module = 6169,
	LTTNG_N32_get_kernel_syms = 6170,
	LTTNG_N32_query_module = 6171,
	LTTNG_N32_quotactl = 6172,
	LTTNG_N32_nfsservctl = 6173,
	LTTNG_N32_getpmsg = 6174,
	LTTNG_N32_putpmsg = 6175,
	LTTNG_N32_afs_syscall = 6176,
	LTTNG_N32_reserved177 = 6177,
	LTTNG_N32_gettid = 6178,
	LTTNG_N32_readahead = 6179,
	LTTNG_N32_setxattr = 6180,
	LTTNG_N32_lsetxattr = 6181,
	LTTNG_N32_fsetxattr = 6182,
	LTTNG_N32_getxattr = 6183,
	LTTNG_N32_lgetxattr = 6184,
	LTTNG_N32_fgetxattr = 6185,
	LTTNG_N32_listxattr = 6186,
	LTTNG_N32_llistxattr = 6187,
	LTTNG_N32_flistxattr = 6188,
	LTTNG_N32_removexattr = 6189,
	LTTNG_N32_lremovexattr = 6190,
	LTTNG_N32_fremovexattr = 6191,
	LTTNG_N32_tkill = 6192,
	LTTNG_N32_reserved193 = 6193,
	LTTNG_N32_futex = 6194,
	LTTNG_N32_sched_setaffinity = 6195,
	LTTNG_N32_sched_getaffinity = 6196,
	LTTNG_N32_cacheflush = 6197,
	LTTNG_N32_cachectl = 6198,
	LTTNG_N32_sysmips = 6199,
	LTTNG_N32_io_setup = 6200,
	LTTNG_N32_io_destroy = 6201,
	LTTNG_N32_io_getevents = 6202,
	LTTNG_N32_io_submit = 6203,
	LTTNG_N32_io_cancel = 6204,
	LTTNG_N32_exit_group = 6205,
	LTTNG_N32_lookup_dcookie = 6206,
	LTTNG_N32_epoll_create = 6207,
	LTTNG_N32_epoll_ctl = 6208,
	LTTNG_N32_epoll_wait = 6209,
	LTTNG_N32_remap_file_pages = 6210,
	LTTNG_N32_rt_sigreturn = 6211,
	LTTNG_N32_fcntl64 = 6212,
	LTTNG_N32_set_tid_address = 6213,
	LTTNG_N32_restart_syscall = 6214,
	LTTNG_N32_semtimedop = 6215,
	LTTNG_N32_fadvise64 = 6216,
	LTTNG_N32_statfs64 = 6217,
	LTTNG_N32_fstatfs64 = 6218,
	LTTNG_N32_sendfile64 = 6219,
	LTTNG_N32_timer_create = 6220,
	LTTNG_N32_timer_settime = 6221,
	LTTNG_N32_timer_gettime = 6222,
	LTTNG_N32_timer_getoverrun = 6223,
	LTTNG_N32_timer_delete = 6224,
	LTTNG_N32_clock_settime = 6225,
	LTTNG_N32_clock_gettime = 6226,
	LTTNG_N32_clock_getres = 6227,
	LTTNG_N32_clock_nanosleep = 6228,
	LTTNG_N32_tgkill = 6229,
	LTTNG_N32_utimes = 6230,
	LTTNG_N32_mbind = 6231,
	LTTNG_N32_get_mempolicy = 6232,
	LTTNG_N32_set_mempolicy = 6233,
	LTTNG_N32_mq_open = 6234,
	LTTNG_N32_mq_unlink = 6235,
	LTTNG_N32_mq_timedsend = 6236,
	LTTNG_N32_mq_timedreceive = 6237,
	LTTNG_N32_mq_notify = 6238,
	LTTNG_N32_mq_getsetattr = 6239,
	LTTNG_N32_vserver = 6240,
	LTTNG_N32_waitid = 6241,
	LTTNG_N32_add_key = 6243,
	LTTNG_N32_request_key = 6244,
	LTTNG_N32_keyctl = 6245,
	LTTNG_N32_set_thread_area = 6246,
	LTTNG_N32_inotify_init = 6247,
	LTTNG_N32_inotify_add_watch = 6248,
	LTTNG_N32_inotify_rm_watch = 6249,
	LTTNG_N32_migrate_pages = 6250,
	LTTNG_N32_openat = 6251,
	LTTNG_N32_mkdirat = 6252,
	LTTNG_N32_mknodat = 6253,
	LTTNG_N32_fchownat = 6254,
	LTTNG_N32_futimesat = 6255,
	LTTNG_N32_fstatat64 = 6256,
	LTTNG_N32_unlinkat = 6257,
	LTTNG_N32_renameat = 6258,
	LTTNG_N32_linkat = 6259,
	LTTNG_N32_symlinkat = 6260,
	LTTNG_N32_readlinkat = 6261,
	LTTNG_N32_fchmodat = 6262,
	LTTNG_N32_faccessat = 6263,
	LTTNG_N32_pselect6 = 6264,
	LTTNG_N32_ppoll = 6265,
	LTTNG_N32_unshare = 6266,
	LTTNG_N32_set_robust_list = 6267,
	LTTNG_N32_get_robust_list = 6268,
	LTTNG_N32_splice = 6269,
	LTTNG_N32_sync_file_range = 6270,
	LTTNG_N32_tee = 6271,
	LTTNG_N32_vmsplice = 6272,
};

/* Event syscall_entry structures */

/* Event syscall_entry logging function */
static inline void trace_kernel_arch_syscall_entry(
		enum lttng_syscall_name lttng_param_syscall_id,
		const void * lttng_param_address)
#if (!defined(CONFIG_LTT) || !defined(CONFIG_LTT_FACILITY_KERNEL_ARCH))
{
}
#else
{
	unsigned int index;
	struct ltt_channel_struct *channel;
	struct ltt_trace_struct *trace;
	struct rchan_buf *relayfs_buf;
	void *buffer = NULL;
	size_t real_to_base = 0; /* The buffer is allocated on arch_size alignment */
	size_t *to_base = &real_to_base;
	size_t real_to = 0;
	size_t *to = &real_to;
	size_t real_len = 0;
	size_t *len = &real_len;
	size_t reserve_size;
	size_t slot_size;
	size_t align;
	const void *real_from;
	const void **from = &real_from;
	u64 tsc;
	size_t before_hdr_pad, after_hdr_pad, header_size;

	if(ltt_traces.num_active_traces == 0) return;

	/* For each field, calculate the field size. */
	/* size = *to_base + *to + *len */
	/* Assume that the padding for alignment starts at a
	 * sizeof(void *) address. */

	*from = &lttng_param_syscall_id;
	align = sizeof(enum lttng_syscall_name);

	if(*len == 0) {
		*to += ltt_align(*to, align); /* align output */
	} else {
		*len += ltt_align(*to+*len, align); /* alignment, ok to do a memcpy of it */
	}

	*len += sizeof(enum lttng_syscall_name);

	*from = &lttng_param_address;
	align = sizeof(const void *);

	if(*len == 0) {
		*to += ltt_align(*to, align); /* align output */
	} else {
		*len += ltt_align(*to+*len, align); /* alignment, ok to do a memcpy of it */
	}

	*len += sizeof(const void *);

	reserve_size = *to_base + *to + *len;
	preempt_disable();
	ltt_nesting[smp_processor_id()]++;
	index = ltt_get_index_from_facility(ltt_facility_kernel_arch_8597B65C,
						event_kernel_arch_syscall_entry);

	list_for_each_entry_rcu(trace, &ltt_traces.head, list) {
		if(!trace->active) continue;

		channel = ltt_get_channel_from_index(trace, index);
		relayfs_buf = channel->rchan->buf[smp_processor_id()];

		slot_size = 0;
		buffer = ltt_reserve_slot(trace, relayfs_buf,
			reserve_size, &slot_size, &tsc,
			&before_hdr_pad, &after_hdr_pad, &header_size);
		if(!buffer) continue; /* buffer full */

		*to_base = *to = *len = 0;

		ltt_write_event_header(trace, channel, buffer,
			ltt_facility_kernel_arch_8597B65C, event_kernel_arch_syscall_entry,
			reserve_size, before_hdr_pad, tsc);
		*to_base += before_hdr_pad + after_hdr_pad + header_size;

		*from = &lttng_param_syscall_id;
		align = sizeof(enum lttng_syscall_name);

		if(*len == 0) {
			*to += ltt_align(*to, align); /* align output */
		} else {
			*len += ltt_align(*to+*len, align); /* alignment, ok to do a memcpy of it */
		}

		*len += sizeof(enum lttng_syscall_name);

		/* Flush pending memcpy */
		if(*len != 0) {
			memcpy(buffer+*to_base+*to, *from, *len);
			*to += *len;
			*len = 0;
		}

		*from = &lttng_param_address;
		align = sizeof(const void *);

		if(*len == 0) {
			*to += ltt_align(*to, align); /* align output */
		} else {
			*len += ltt_align(*to+*len, align); /* alignment, ok to do a memcpy of it */
		}

		*len += sizeof(const void *);

		/* Flush pending memcpy */
		if(*len != 0) {
			memcpy(buffer+*to_base+*to, *from, *len);
			*to += *len;
			*len = 0;
		}

		ltt_commit_slot(relayfs_buf, buffer, slot_size);

	}

	ltt_nesting[smp_processor_id()]--;
	preempt_enable_no_resched();
}
#endif //(!defined(CONFIG_LTT) || !defined(CONFIG_LTT_FACILITY_KERNEL_ARCH))


/* Event syscall_exit structures */

/* Event syscall_exit logging function */
static inline void trace_kernel_arch_syscall_exit(
		void)
#if (!defined(CONFIG_LTT) || !defined(CONFIG_LTT_FACILITY_KERNEL_ARCH))
{
}
#else
{
	unsigned int index;
	struct ltt_channel_struct *channel;
	struct ltt_trace_struct *trace;
	struct rchan_buf *relayfs_buf;
	void *buffer = NULL;
	size_t real_to_base = 0; /* The buffer is allocated on arch_size alignment */
	size_t *to_base = &real_to_base;
	size_t real_to = 0;
	size_t *to = &real_to;
	size_t real_len = 0;
	size_t *len = &real_len;
	size_t reserve_size;
	size_t slot_size;
	u64 tsc;
	size_t before_hdr_pad, after_hdr_pad, header_size;

	if(ltt_traces.num_active_traces == 0) return;

	/* For each field, calculate the field size. */
	/* size = *to_base + *to + *len */
	/* Assume that the padding for alignment starts at a
	 * sizeof(void *) address. */

	reserve_size = *to_base + *to + *len;
	preempt_disable();
	ltt_nesting[smp_processor_id()]++;
	index = ltt_get_index_from_facility(ltt_facility_kernel_arch_8597B65C,
						event_kernel_arch_syscall_exit);

	list_for_each_entry_rcu(trace, &ltt_traces.head, list) {
		if(!trace->active) continue;

		channel = ltt_get_channel_from_index(trace, index);
		relayfs_buf = channel->rchan->buf[smp_processor_id()];

		slot_size = 0;
		buffer = ltt_reserve_slot(trace, relayfs_buf,
			reserve_size, &slot_size, &tsc,
			&before_hdr_pad, &after_hdr_pad, &header_size);
		if(!buffer) continue; /* buffer full */

		*to_base = *to = *len = 0;

		ltt_write_event_header(trace, channel, buffer,
			ltt_facility_kernel_arch_8597B65C, event_kernel_arch_syscall_exit,
			reserve_size, before_hdr_pad, tsc);
		*to_base += before_hdr_pad + after_hdr_pad + header_size;

		ltt_commit_slot(relayfs_buf, buffer, slot_size);

	}

	ltt_nesting[smp_processor_id()]--;
	preempt_enable_no_resched();
}
#endif //(!defined(CONFIG_LTT) || !defined(CONFIG_LTT_FACILITY_KERNEL_ARCH))


#endif //_LTT_FACILITY_KERNEL_ARCH_H_
