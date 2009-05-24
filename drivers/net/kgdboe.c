/*
 * drivers/net/kgdboe.c
 *
 * A network interface for GDB.
 * Based upon 'gdbserial' by David Grothe <dave@gcom.com>
 * and Scott Foehner <sfoehner@engr.sgi.com>
 *
 * Maintainers: Amit S. Kale <amitkale@linsyssoft.com> and
 * 		Tom Rini <trini@kernel.crashing.org>
 *
 * 2004 (c) Amit S. Kale <amitkale@linsyssoft.com>
 * 2004-2005 (c) MontaVista Software, Inc.
 * 2005 (c) Wind River Systems, Inc.
 *
 * Contributors at various stages not listed above:
 * San Mehat <nettwerk@biodome.org>, Robert Walsh <rjwalsh@durables.org>,
 * wangdi <wangdi@clusterfs.com>, Matt Mackall <mpm@selenic.com>,
 * Pavel Machek <pavel@suse.cz>, Jason Wessel <jason.wessel@windriver.com>
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/kgdb.h>
#include <linux/netpoll.h>
#include <linux/init.h>
#ifdef CONFIG_PROC_FS
#include <linux/proc_fs.h>
#endif				/* CONFIG_PROC_FS */

#include <asm/atomic.h>

#define IN_BUF_SIZE 512		/* power of 2, please */
#if 0
#define NOT_CONFIGURED_STRING "not_configured"
#else
#define NOT_CONFIGURED_STRING "@127.0.0.1/,@127.0.0.1/"
#endif
#define OUT_BUF_SIZE 30		/* We don't want to send too big of a packet. */
#define MAX_KGDBOE_CONFIG_STR 256

static char in_buf[IN_BUF_SIZE], out_buf[OUT_BUF_SIZE];
static int in_head, in_tail, out_count;
static atomic_t in_count;
/* 0 = unconfigured, 1 = netpoll options parsed, 2 = fully configured. */
static int configured;
static struct kgdb_io local_kgdb_io_ops;
static int use_dynamic_mac;

MODULE_DESCRIPTION("KGDB driver for network interfaces");
MODULE_LICENSE("GPL");
static char config[MAX_KGDBOE_CONFIG_STR] = NOT_CONFIGURED_STRING;
static struct kparam_string kps = {
	.string = config,
	.maxlen = MAX_KGDBOE_CONFIG_STR,
};

static void rx_hook(struct netpoll *np, int port, char *msg, int len,
		    struct sk_buff *skb)
{
	int i;

	np->remote_port = port;

	/* Copy the MAC address if we need to. */
	if (use_dynamic_mac) {
		memcpy(np->remote_mac, eth_hdr(skb)->h_source,
				sizeof(np->remote_mac));
		use_dynamic_mac = 0;
	}

	/*
	 * This could be GDB trying to attach.  But it could also be GDB
	 * finishing up a session, with kgdb_connected=0 but GDB sending
	 * an ACK for the final packet.  To make sure we don't try and
	 * make a breakpoint when GDB is leaving, make sure that if
	 * !kgdb_connected the only len == 1 packet we allow is ^C.
	 */
	if (!kgdb_connected && (len != 1 || msg[0] == 3) &&
	    !atomic_read(&kgdb_setting_breakpoint)) {
		tasklet_schedule(&kgdb_tasklet_breakpoint);
	}

	for (i = 0; i < len; i++) {
		if (msg[i] == 3)
			tasklet_schedule(&kgdb_tasklet_breakpoint);

		if (atomic_read(&in_count) >= IN_BUF_SIZE) {
			/* buffer overflow, clear it */
			in_head = in_tail = 0;
			atomic_set(&in_count, 0);
			break;
		}
		in_buf[in_head++] = msg[i];
		in_head &= (IN_BUF_SIZE - 1);
		atomic_inc(&in_count);
	}
}

#define LOCAL_PORT 6443
#define REMOTE_PORT 6442
#define LOCAL_DEV_NAME "eth0"

static struct netpoll np = {
	.dev_name = LOCAL_DEV_NAME,
	.name = "kgdboe",
	.rx_hook = rx_hook,
	.local_port = LOCAL_PORT,
	.remote_port = REMOTE_PORT,
	.remote_mac = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
};

#ifdef CONFIG_PROC_FS
static int eth_get_char(void);
static void eth_put_char(u8 chr);
static int init_kgdboe(void);
static void eth_flush_buf(void);
static void eth_pre_exception_handler(void);
static void eth_post_exception_handler(void);

/* Prototypes for proc filesystem access */
static int module_permission(struct inode *inode, int op,
			     struct nameidata *foo);
static int module_open(struct inode *inode, struct file *file);
static int module_close(struct inode *inode, struct file *file);
static ssize_t procfs_output(struct file *filp,
			     char *buffer, size_t length, loff_t * offset);
static ssize_t procfs_input(struct file *filp,
			    const char *buff, size_t len, loff_t * off);
/* Account for: 
 * kgdboe=999999@255.255.255.255/eth10:1,999999@255.255.255.255/00:00:00:00:00:00 
 */
#define MAX_KGDBOE_CONFIG_STRING 80
#define PROC_ENTRY_FILENAME "sys/kgdboe"
static char kgdboe_config_string[MAX_KGDBOE_CONFIG_STRING] = NOT_CONFIGURED_STRING;
static struct proc_dir_entry *KGDB_Proc_File;
static struct inode_operations KGDB_Proc_Inode_Ops = {
	.permission = module_permission,	/* check for permissions */
};

static struct file_operations KGDB_Proc_File_Ops = {
	.read = procfs_output,
	.write = procfs_input,
	.open = module_open,
	.release = module_close,
};

/* 
 * This function decides whether to allow an operation
 * (return zero) or not allow it (return a non-zero
 * which indicates why it is not allowed).
 *
 * The operation can be one of the following values:
 * 0 - Execute (run the "file" - meaningless in our case)
 * 2 - Write (input to the kernel module)
 * 4 - Read (output from the kernel module)
 *
 * This is the real function that checks file
 * permissions. The permissions returned by ls -l are
 * for referece only, and can be overridden here.
 */

static int module_permission(struct inode *inode, int op, struct nameidata *foo)
{
	/* 
	 * We allow everybody to read from our module, but
	 * only root (uid 0) may write to it 
	 */
	if (op == 4 || (op == 2 && current->euid == 0))
		return 0;

	/* 
	 * If it's anything else, access is denied 
	 */
	return -EACCES;
}

/* 
 * Increment the module reference count when ever a procfs operation is pending.
 */
static int module_open(struct inode *inode, struct file *file)
{
	try_module_get(THIS_MODULE);
	return 0;
}

/* 
 * Decrement the module reference count when ever a procfs operation is complete.
 */
static int module_close(struct inode *inode, struct file *file)
{
	module_put(THIS_MODULE);
	return 0;
}

static ssize_t procfs_output(struct file *filp,	/* see include/linux/fs.h   */
			     char *buffer,	/* buffer to fill with data */
			     size_t length,	/* length of the buffer     */
			     loff_t * offset)
{
	static int finished = 0;
	int i;
	char message[MAX_KGDBOE_CONFIG_STRING + 30];

	/* 
	 * We return 0 to indicate end of file, that we have
	 * no more information. Otherwise, processes will
	 * continue to read from us in an endless loop. 
	 */
	if (finished) {
		finished = 0;
		return 0;
	}

	sprintf(message, "%s\n", kgdboe_config_string);
	for (i = 0; i < length && message[i]; i++) {
		/* This macro writes to the user space buffer */
		put_user(message[i], buffer + i);
	}

	/* 
	 * Notice, we assume here that the size of the message
	 * is below len, or it will be received cut. In a real
	 * life situation, if the size of the message is less
	 * than len then we'd return len and on the second call
	 * start filling the buffer with the len+1'th byte of
	 * the message. 
	 */
	finished = 1;

	return i;		/* Return the number of bytes "read" */
}

static ssize_t procfs_input(struct file *filp,
			    const char *buff, size_t len, loff_t * off)
{
	int i;
	char kmessage[MAX_KGDBOE_CONFIG_STRING];
	char kmessage_save[MAX_KGDBOE_CONFIG_STRING];
	/* 
	 * Put the input into kmessage, where module_output
	 * will later be able to use it 
	 */
	for (i = 0; i < MAX_KGDBOE_CONFIG_STRING - 1 && i < len; i++) {
		get_user(kmessage[i], buff + i);
		/* Stop if there is a newline character */
		if (kmessage[i] == '\n')
			break;
	}

	kmessage[i] = '\0';	/* terminate the string */

	/* Setup the orignal values to the np structure */
	strcpy(np.dev_name, LOCAL_DEV_NAME);
	np.local_port = LOCAL_PORT;
	np.remote_port = REMOTE_PORT;
	np.remote_mac[0] = 0xff;
	np.remote_mac[1] = 0xff;
	np.remote_mac[2] = 0xff;
	np.remote_mac[3] = 0xff;
	np.remote_mac[4] = 0xff;
	np.remote_mac[5] = 0xff;

	/* Try out the new net poll settings.  We have to use a saved
	 * buffer because the netpoll_parse_options can destroy parts of
	 * the buffer.
	 */
	strncpy(kmessage_save, kmessage, sizeof(kmessage_save));
	i = netpoll_parse_options(&np, kmessage_save);
	if (i == 0) {
		if (np.remote_mac[0] == 0xff &&
		    np.remote_mac[1] == 0xff &&
		    np.remote_mac[2] == 0xff &&
		    np.remote_mac[3] == 0xff &&
		    np.remote_mac[4] == 0xff && np.remote_mac[5] == 0xff) {
			use_dynamic_mac = 1;
		} else {
			use_dynamic_mac = 0;
		}
		/* Save the config string in the internal static variable */
		strncpy(kgdboe_config_string, kmessage,
			sizeof(kgdboe_config_string));
#ifndef CONFIG_KGDBOE_MODULE
		/* If this is not a kernel module we have to check the
		 * initialization routines and use the module initializer to
		 * register all the kernel hooks.  This is designed
		 * specifically to allow KGDBOE to be enabled if you did not
		 * initially pass in the kernel parameters to initalize it and
		 * you your kernel doesn't have any modules, but you still
		 * want to debug it. 
		 */
		if (kgdb_io_ops.init == 0) {
			/* Do the net poll setup at this time */
			if (netpoll_setup(&np)) {
				printk
				    (KERN_ERR
				     "kgdboe: netpoll_setup failed kgdboe failed\n");
			} else {
				kgdb_io_ops.read_char = eth_get_char;
				kgdb_io_ops.write_char = eth_put_char;
				kgdb_io_ops.init = init_kgdboe;
				kgdb_io_ops.flush = eth_flush_buf;
				kgdb_io_ops.late_init = NULL;
				kgdb_io_ops.pre_exception =
				    eth_pre_exception_handler;
				kgdb_io_ops.post_exception =
				    eth_post_exception_handler;

				if (kgdb_register_io_module(&kgdb_io_ops))
					memset(&kgdb_io_ops, 0,
					       sizeof(struct kgdb_io));
				else
					printk(KERN_INFO
					       "kgdboe: debugging over ethernet enabled\n");
			}
		}
#endif				/* ! CONFIG_KGDBOE_MODULE */
	} else {
		printk(KERN_ERR
		       "kgdboe: Usage: [src-port]@[src-ip]/[dev],[tgt-port]@<tgt-ip>/[tgt-macaddr]\n");
		if (strncmp
		    (kgdboe_config_string, "not_configured",
		     sizeof(kgdboe_config_string)) != 0) {
			printk(KERN_ERR
			       "kgdboe: reverting to prior configuration\n");
			/* revert back to the original config */
			strncpy(kmessage_save, kgdboe_config_string,
				sizeof(kmessage_save));
			netpoll_parse_options(&np, kmessage_save);
		}
	}

	/* Tell the kernel we read all the data it provided through the hook */
	return len;
}

#endif				/* CONFIG_PROC_FS */

static void eth_pre_exception_handler(void)
{
#ifndef CONFIG_KGDBOE_NOMODULE
	/* Increment the module count when the debugger is active */
	if (!module_refcount(THIS_MODULE))
		try_module_get(THIS_MODULE);
#endif
	netpoll_set_trap(1);
}

static void eth_post_exception_handler(void)
{
	/* decrement the module count when the debugger detaches */
#ifndef CONFIG_KGDBOE_NOMODULE
	if (!kgdb_connected && module_refcount(THIS_MODULE))
		module_put(THIS_MODULE);
#endif
	netpoll_set_trap(0);
}

static int eth_get_char(void)
{
	int chr;

	while (atomic_read(&in_count) == 0)
		netpoll_poll(&np);

	chr = in_buf[in_tail++];
	in_tail &= (IN_BUF_SIZE - 1);
	atomic_dec(&in_count);
	return chr;
}

static void eth_flush_buf(void)
{
	if (out_count && np.dev) {
		netpoll_send_udp(&np, out_buf, out_count);
		memset(out_buf, 0, sizeof(out_buf));
		out_count = 0;
	}
}

static void eth_put_char(u8 chr)
{
	out_buf[out_count++] = chr;
	if (out_count == OUT_BUF_SIZE)
		eth_flush_buf();
}

static int option_setup(char *opt)
{
	char opt_scratch[MAX_KGDBOE_CONFIG_STR];

#ifdef CONFIG_PROC_FS
        /* Save the option setup information */
	strncpy(kgdboe_config_string, opt, sizeof(kgdboe_config_string));
#endif

	/* If we're being given a new configuration, copy it in. */
	if (opt != config)
		strcpy(config, opt);
	/* But work on a copy as netpoll_parse_options will eat it. */
	strcpy(opt_scratch, opt);
	configured = !netpoll_parse_options(&np, opt_scratch);

	use_dynamic_mac = 1;

	return 0;
}
__setup("kgdboe=", option_setup);

/* With our config string set by some means, configure kgdboe. */
static int configure_kgdboe(void)
{
	/* Try out the string. */
	option_setup(config);

	if (!configured) {
		printk(KERN_ERR "kgdboe: configuration incorrect - kgdboe not "
		       "loaded.\n");
		printk(KERN_ERR "  Usage: kgdboe=[src-port]@[src-ip]/[dev],"
				"[tgt-port]@<tgt-ip>/<tgt-macaddr>\n");
		return -EINVAL;
	}

	/* Bring it up. */
	if (netpoll_setup(&np)) {
		printk(KERN_ERR "kgdboe: netpoll_setup failed kgdboe failed\n");
		return -EINVAL;
	}

	if (kgdb_register_io_module(&local_kgdb_io_ops)) {
		netpoll_cleanup(&np);
		return -EINVAL;
	}

	configured = 2;

	return 0;
}

static int init_kgdboe(void)
{
	int ret;

	/* Already done? */
	if (configured == 2)
		return 0;

	/* OK, go ahead and do it. */
	ret = configure_kgdboe();
#ifdef CONFIG_PROC_FS
	if (!ret) {
		KGDB_Proc_File = create_proc_entry(PROC_ENTRY_FILENAME, 0644, NULL);
	
		if (KGDB_Proc_File == NULL) {
			printk(KERN_INFO "Error: Could not initialize /proc/%s,\n",
			       PROC_ENTRY_FILENAME);
			return -ENOMEM;
		}
	
		KGDB_Proc_File->owner = THIS_MODULE;
		KGDB_Proc_File->proc_iops = &KGDB_Proc_Inode_Ops;
		KGDB_Proc_File->proc_fops = &KGDB_Proc_File_Ops;
		KGDB_Proc_File->mode = S_IFREG | S_IRUGO | S_IWUSR;
		KGDB_Proc_File->uid = 0;
		KGDB_Proc_File->gid = 0;
		KGDB_Proc_File->size = 80;
	}
#endif

	if (configured == 2)
		printk(KERN_INFO "kgdboe: debugging over ethernet enabled\n");

	return ret;
}

static void cleanup_kgdboe(void)
{
	netpoll_cleanup(&np);
	configured = 0;
	kgdb_unregister_io_module(&local_kgdb_io_ops);

#ifdef CONFIG_PROC_FS
	remove_proc_entry(PROC_ENTRY_FILENAME, &proc_root);
#endif                         /* CONFIG_PROC_FS */
}

static int param_set_kgdboe_var(const char *kmessage, struct kernel_param *kp)
{
	char kmessage_save[MAX_KGDBOE_CONFIG_STR];
	int msg_len = strlen(kmessage);

	if (msg_len + 1 > MAX_KGDBOE_CONFIG_STR) {
		printk(KERN_ERR "%s: string doesn't fit in %u chars.\n",
		       kp->name, MAX_KGDBOE_CONFIG_STR - 1);
		return -ENOSPC;
	}

	if (kgdb_connected) {
		printk(KERN_ERR "kgdboe: Cannot reconfigure while KGDB is "
				"connected.\n");
		return 0;
	}

	/* Start the reconfiguration process by saving the old string */
	strncpy(kmessage_save, config, sizeof(kmessage_save));


	/* Copy in the new param and strip out invalid characters so we
	 * can optionally specify the MAC.
	 */
	strncpy(config, kmessage, sizeof(config));
	msg_len--;
	while (msg_len > 0 &&
			(config[msg_len] < ',' || config[msg_len] > 'f')) {
		config[msg_len] = '\0';
		msg_len--;
	}

	/* Check to see if we are unconfiguring the io module and that it
	 * was in a fully configured state, as this is the only time that
	 * netpoll_cleanup should get called
	 */
	if (configured == 2 && strcmp(config, NOT_CONFIGURED_STRING) == 0) {
		printk(KERN_INFO "kgdboe: reverting to unconfigured state\n");
		cleanup_kgdboe();
		return 0;
	} else
		/* Go and configure with the new params. */
		configure_kgdboe();

	if (configured == 2)
		return 0;

	/* If the new string was invalid, revert to the previous state, which
	 * is at a minimum not_configured. */
	strncpy(config, kmessage_save, sizeof(config));
	if (strcmp(kmessage_save, NOT_CONFIGURED_STRING) != 0) {
		printk(KERN_INFO "kgdboe: reverting to prior configuration\n");
		/* revert back to the original config */
		strncpy(config, kmessage_save, sizeof(config));
		configure_kgdboe();
	}
	return 0;
}

static struct kgdb_io local_kgdb_io_ops = {
	.read_char = eth_get_char,
	.write_char = eth_put_char,
	.init = init_kgdboe,
	.flush = eth_flush_buf,
	.pre_exception = eth_pre_exception_handler,
	.post_exception = eth_post_exception_handler
};

module_init(init_kgdboe);
module_exit(cleanup_kgdboe);
module_param_call(kgdboe, param_set_kgdboe_var, param_get_string, &kps, 0644);
MODULE_PARM_DESC(kgdboe, " kgdboe=[src-port]@[src-ip]/[dev],"
		 "[tgt-port]@<tgt-ip>/<tgt-macaddr>\n");
