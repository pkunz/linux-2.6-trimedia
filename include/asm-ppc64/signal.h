#ifndef _ASMPPC64_SIGNAL_H
#define _ASMPPC64_SIGNAL_H

#include <linux/types.h>
#include <linux/compiler.h>
#include <asm/siginfo.h>

/* Avoid too many header ordering problems.  */
struct siginfo;

#define _NSIG		64
#define _NSIG_BPW	64
#define _NSIG_WORDS	(_NSIG / _NSIG_BPW)

typedef unsigned long old_sigset_t;		/* at least 32 bits */

typedef struct {
	unsigned long sig[_NSIG_WORDS];
} sigset_t;

#define SIGHUP		 1
#define SIGINT		 2
#define SIGQUIT		 3
#define SIGILL		 4
#define SIGTRAP		 5
#define SIGABRT		 6
#define SIGIOT		 6
#define SIGBUS		 7
#define SIGFPE		 8
#define SIGKILL		 9
#define SIGUSR1		10
#define SIGSEGV		11
#define SIGUSR2		12
#define SIGPIPE		13
#define SIGALRM		14
#define SIGTERM		15
#define SIGSTKFLT	16
#define SIGCHLD		17
#define SIGCONT		18
#define SIGSTOP		19
#define SIGTSTP		20
#define SIGTTIN		21
#define SIGTTOU		22
#define SIGURG		23
#define SIGXCPU		24
#define SIGXFSZ		25
#define SIGVTALRM	26
#define SIGPROF		27
#define SIGWINCH	28
#define SIGIO		29
#define SIGPOLL		SIGIO
/*
#define SIGLOST		29
*/
#define SIGPWR		30
#define SIGSYS		31
#define	SIGUNUSED	31

/* These should not be considered constants from userland.  */
#define SIGRTMIN	32
#define SIGRTMAX	_NSIG

/*
 * SA_FLAGS values:
 *
 * SA_ONSTACK is not currently supported, but will allow sigaltstack(2).
 * SA_INTERRUPT is a no-op, but left due to historical reasons. Use the
 * SA_RESTART flag to get restarting signals (which were the default long ago)
 * SA_NOCLDSTOP flag to turn off SIGCHLD when children stop.
 * SA_RESETHAND clears the handler when the signal is delivered.
 * SA_NOCLDWAIT flag on SIGCHLD to inhibit zombies.
 * SA_NODEFER prevents the current signal from being masked in the handler.
 *
 * SA_ONESHOT and SA_NOMASK are the historical Linux names for the Single
 * Unix names RESETHAND and NODEFER respectively.
 */
#define SA_NOCLDSTOP	0x00000001u
#define SA_NOCLDWAIT	0x00000002u
#define SA_SIGINFO	0x00000004u
#define SA_ONSTACK	0x08000000u
#define SA_RESTART	0x10000000u
#define SA_NODEFER	0x40000000u
#define SA_RESETHAND	0x80000000u

#define SA_NOMASK	SA_NODEFER
#define SA_ONESHOT	SA_RESETHAND
#define SA_INTERRUPT	0x20000000u /* dummy -- ignored */

#define SA_RESTORER	0x04000000u

/* 
 * sigaltstack controls
 */
#define SS_ONSTACK	1
#define SS_DISABLE	2
#define SS_ALLOW_NESTED 0x4000

#define MINSIGSTKSZ	2048
#define SIGSTKSZ	8192

#include <asm-generic/signal.h>

struct old_sigaction {
	__sighandler_t sa_handler;
	old_sigset_t sa_mask;
	unsigned long sa_flags;
	__sigrestore_t sa_restorer;
};

struct sigaction {
	__sighandler_t sa_handler;
	unsigned long sa_flags;
	__sigrestore_t sa_restorer;
	sigset_t sa_mask;		/* mask last for extensibility */
};

struct k_sigaction {
	struct sigaction sa;
};

typedef struct sigaltstack {
	void __user *ss_sp;
	int ss_flags;
	size_t ss_size;
} stack_t;

struct pt_regs;
struct timespec;
extern int do_signal(sigset_t *oldset, struct pt_regs *regs);
extern int do_signal32(sigset_t *oldset, struct pt_regs *regs);
#define ptrace_signal_deliver(regs, cookie) do { } while (0)

#endif /* _ASMPPC64_SIGNAL_H */
