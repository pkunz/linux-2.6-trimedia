#ifndef _LINUX_INIT_H
#define _LINUX_INIT_H

#include <linux/config.h>
#include <linux/compiler.h>

/* These macros are used to mark some functions or 
 * initialized data (doesn't apply to uninitialized data)
 * as `initialization' functions. The kernel can take this
 * as hint that the function is used only during the initialization
 * phase and free up used memory resources after
 *
 * Usage:
 * For functions:
 * 
 * You should add __init immediately before the function name, like:
 *
 * static void __init initme(int x, int y)
 * {
 *    extern int z; z = x * y;
 * }
 *
 * If the function has a prototype somewhere, you can also add
 * __init between closing brace of the prototype and semicolon:
 *
 * extern int initialize_foobar_device(int, int, int) __init;
 *
 * For initialized data:
 * You should insert __initdata between the variable name and equal
 * sign followed by value, e.g.:
 *
 * static int init_variable __initdata = 0;
 * static char linux_logo[] __initdata = { 0x32, 0x36, ... };
 *
 * Don't forget to initialize data not at file scope, i.e. within a function,
 * as gcc otherwise puts the data into the bss section and not into the init
 * section.
 * 
 * Also note, that this data cannot be "const".
 */

/* These are for everybody (although not all archs will actually
   discard it in modules) */
#define __init		__attribute__ ((__section__ (".init.text")))
#define __initdata	__attribute__ ((__section__ (".init.data")))
#define __exitdata	__attribute__ ((__section__(".exit.data")))
#define __exit_call	__attribute_used__ __attribute__ ((__section__ (".exitcall.exit")))

#ifdef MODULE
#define __exit		__attribute__ ((__section__(".exit.text")))
#else
#define __exit		__attribute_used__ __attribute__ ((__section__(".exit.text")))
#endif

/* For assembly routines */
#define __INIT		.section	".init.text","ax"
#define __FINIT		.previous
#define __INITDATA	.section	".init.data","aw"

#ifndef __ASSEMBLY__
/*
 * Used for initialization calls..
 */
typedef int (*initcall_t)(void);
typedef void (*exitcall_t)(void);

#define INITCALL_DEBUG

struct initcall_info{
	struct initcall_info* next;
	initcall_t fn;
#ifdef INITCALL_DEBUG
	char* name;
#endif
};

struct exitcall_info{
	struct exitcall_info* next;
	exitcall_t fn;
};

extern struct initcall_info* initcall_1_list_ptr;
extern struct initcall_info* initcall_2_list_ptr;
extern struct initcall_info* initcall_3_list_ptr;
extern struct initcall_info* initcall_4_list_ptr;
extern struct initcall_info* initcall_5_list_ptr;
extern struct initcall_info* initcall_6_list_ptr;
extern struct initcall_info* initcall_7_list_ptr;

extern struct initcall_info* con_initcall_list_ptr;
extern struct initcall_info* security_initcall_list_ptr;

extern struct exitcall_info* exitcall_list_ptr;

/* Defined in init/main.c */
extern char saved_command_line[];
#endif
  
#ifndef MODULE

#ifndef __ASSEMBLY__

/* initcalls are now grouped by functionality into separate 
 * subsections. Ordering inside the subsections is determined
 * by link order. 
 * For backwards compatibility, initcall() puts the call in 
 * the device init subsection.
 */
#ifndef INITCALL_DEBUG
#define __define_initcall(level,fn) \
	struct initcall_info initcall_##level##_list={NULL,fn}
#else
#define __define_initcall(level,fn) \
	struct initcall_info initcall_##level##_list={NULL,fn,__FILE__ ": " #fn}
#endif 

#define core_initcall(fn)		__define_initcall(1,fn)
#define postcore_initcall(fn)		__define_initcall(2,fn)
#define arch_initcall(fn)		__define_initcall(3,fn)
#define subsys_initcall(fn)		__define_initcall(4,fn)
#define fs_initcall(fn)			__define_initcall(5,fn)
#define device_initcall(fn)		__define_initcall(6,fn)
#define late_initcall(fn)		__define_initcall(7,fn)

#define __initcall(fn) device_initcall(fn)

#define __exitcall(fn) \
	struct exitcall_info exitcall_list={NULL,fn}

#define console_initcall(fn) \
	struct initcall_info con_initcall_list={NULL,fn}

#define security_initcall(fn) \
	struct initcall_info security_initcall_list={NULL,fn}

struct obs_kernel_param {
	struct obs_kernel_param* next;
	const char *str;
	int (*setup_func)(char *);
	int early;
};

extern struct obs_kernel_param* init_setup_list_ptr;

/*
 * Only for really core code.  See moduleparam.h for the normal way.
 *
 * Force the alignment so the compiler doesn't space elements of the
 * obs_kernel_param "array" too far apart in .init.setup.
 */
/*
 * For trimedia, I realy don't know how to acheive it, so have to 
 * use link list.
 */
#define __setup_end(str,fn) 					\
	static char __setup_str_##fn[]=str; 			\
	static struct obs_kernel_param init_setup_list##fn= 	\
		{NULL,__setup_str_##fn,fn,0}

#define __setup_link(str,fn,prev_fn) 				\
	static char __setup_str_##fn[]=str; 			\
	static struct obs_kernel_param init_setup_list##fn= 	\
		{&init_setup_list##prev_fn,__setup_str_##fn,fn,0}

#define __setup_start(str,fn,prev_fn) 				\
	static char __setup_str_##fn[]=str; 			\
	struct obs_kernel_param init_setup_list= 		\
		{&init_setup_list##prev_fn,__setup_str_##fn,fn,0}

#define __setup(str,fn) 					\
	static char __setup_str_##fn[]=str; 			\
	struct obs_kernel_param init_setup_list= 		\
		{NULL,__setup_str_##fn,fn,0}

#define __obsolete_setup(str)  					\
	static char __setup_str_##fn[]=str; 			\
	struct obs_kernel_param init_setup_list= 		\
		{NULL,__setup_str_##fn,NULL,0}

#define early_param(str,fn) 					\
	static char __setup_str_##fn[]=str; 			\
	struct obs_kernel_param init_setup_list= 		\
		{NULL,__setup_str_##fn,fn,1}

/* Relies on saved_command_line being set */
void __init parse_early_param(void);
#endif /* __ASSEMBLY__ */

/**
 * module_init() - driver initialization entry point
 * @x: function to be run at kernel boot time or module insertion
 * 
 * module_init() will either be called during do_initcalls (if
 * builtin) or at module insertion time (if a module).  There can only
 * be one per module.
 */
#define module_init(x)	__initcall(x);

/**
 * module_exit() - driver exit entry point
 * @x: function to be run when driver is removed
 * 
 * module_exit() will wrap the driver clean-up code
 * with cleanup_module() when used with rmmod when
 * the driver is a module.  If the driver is statically
 * compiled into the kernel, module_exit() has no effect.
 * There can only be one per module.
 */
#define module_exit(x)	__exitcall(x);

#else /* MODULE */

/* Don't use these in modules, but some people do... */
#define core_initcall(fn)		module_init(fn)
#define postcore_initcall(fn)		module_init(fn)
#define arch_initcall(fn)		module_init(fn)
#define subsys_initcall(fn)		module_init(fn)
#define fs_initcall(fn)			module_init(fn)
#define device_initcall(fn)		module_init(fn)
#define late_initcall(fn)		module_init(fn)

#define security_initcall(fn)		module_init(fn)

/* These macros create a dummy inline: gcc 2.9x does not count alias
 as usage, hence the `unused function' warning when __init functions
 are declared static. We use the dummy __*_module_inline functions
 both to kill the warning and check the type of the init/cleanup
 function. */

/* Each module must use one module_init(), or one no_module_init */
#define module_init(initfn)					\
	static inline initcall_t __inittest(void)		\
	{ return initfn; }					\
	int init_module(void) __attribute__((alias(#initfn)));

/* This is only required if you want to be unloadable. */
#define module_exit(exitfn)					\
	static inline exitcall_t __exittest(void)		\
	{ return exitfn; }					\
	void cleanup_module(void) __attribute__((alias(#exitfn)));

#define __setup_param(str, unique_id, fn)	/* nothing */
#define __setup_null_param(str, unique_id) 	/* nothing */
#define __setup(str, func) 			/* nothing */
#define __obsolete_setup(str) 			/* nothing */
#endif

/* Data marked not to be saved by software_suspend() */
#define __nosavedata __attribute__ ((__section__ (".data.nosave")))

/* This means "can be init if no module support, otherwise module load
   may call it." */
#ifdef CONFIG_MODULES
#define __init_or_module
#define __initdata_or_module
#else
#define __init_or_module __init
#define __initdata_or_module __initdata
#endif /*CONFIG_MODULES*/

#ifdef CONFIG_HOTPLUG
#define __devinit
#define __devinitdata
#define __devexit
#define __devexitdata
#else
#define __devinit __init
#define __devinitdata __initdata
#define __devexit __exit
#define __devexitdata __exitdata
#endif

#ifdef CONFIG_HOTPLUG_CPU
#define __cpuinit
#define __cpuinitdata
#define __cpuexit
#define __cpuexitdata
#else
#define __cpuinit	__init
#define __cpuinitdata __initdata
#define __cpuexit __exit
#define __cpuexitdata	__exitdata
#endif

/* Functions marked as __devexit may be discarded at kernel link time, depending
   on config options.  Newer versions of binutils detect references from
   retained sections to discarded sections and flag an error.  Pointers to
   __devexit functions must use __devexit_p(function_name), the wrapper will
   insert either the function_name or NULL, depending on the config options.
 */
#if defined(MODULE) || defined(CONFIG_HOTPLUG)
#define __devexit_p(x) x
#else
#define __devexit_p(x) NULL
#endif

#ifdef MODULE
#define __exit_p(x) x
#else
#define __exit_p(x) NULL
#endif

#endif /* _LINUX_INIT_H */
