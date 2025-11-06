#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/miscdevice.h>

MODULE_LICENSE("Dual BSD/GPL");

//static dev_t devNo;
//static struct cdev pCDEV;
static char kBUF[ 64 ];
static int charDRV_open(struct inode *inode, struct file *file)
{
  printk(KERN_ALERT "charDRV_open\r\n" );
  return 0;
}
static int charDRV_release(struct inode *inode, struct file *file)
{
  printk(KERN_ALERT "charDRV_release\r\n" );
  return 0;
}
// retV = read ( fd, (void *)BUF, 64 );
static ssize_t charDRV_read(struct file *file, char __user *BUF, size_t size, loff_t *off)
{ long retV;
  static char retKBUF[64];
  printk(KERN_ALERT "charDRV_read\r\n" );
  
  memset( (void*)retKBUF,0,64 );    
  strcpy( retKBUF, "Hi ");
  strcat( retKBUF, kBUF );
  
  retV = copy_to_user( (void*)BUF, (void *)retKBUF, strlen(retKBUF) );

  return strlen(retKBUF);	// ??   read ?  the number of bytes actually read
}// release  retV   file   BUF   size  off
//                     write ( fd , (void *)showMess, strlen(showMess) ); 
static ssize_t charDRV_write(struct file *file, const char __user *BUF, size_t size, loff_t *off)
{ long retV;
  printk(KERN_ALERT "charDRV_write\r\n" );
  
  retV = copy_from_user( (void*)kBUF, (void*)BUF, size );
  
  return size;		
}
static struct file_operations fops = {
  read:charDRV_read, //	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
  write:charDRV_write, //	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
  open:charDRV_open, //	int (*open) (struct inode *, struct file *);
  release:charDRV_release //	int (*release) (struct inode *, struct file *);
};
static struct miscdevice cyhMISC=
{
  .minor=88,
  .name="cyh_MISC",
  .fops=&fops
};
static __init int Hi_module(void)
{  int retV;
	printk(KERN_ALERT "Happy, world \r\n" );
	//devNo = MKDEV( 88 , 0 );
	//retV = register_chrdev_region(devNo, 1, "cyhDRV1" );
/*
M1:void cdev_init(struct cdev *, const struct file_operations *);
M2:struct cdev *cdev_alloc(void);
*/
    //cdev_init( &pCDEV , &fops );
    //retV = cdev_add( &pCDEV, devNo, 1 );

    retV = misc_register( &cyhMISC );


	return 0;
}
static __exit void Hi_cleanup_module(void)
{
	printk(KERN_ALERT "GoodBye, world\r\n");
    //cdev_del( &pCDEV );
	//unregister_chrdev_region(devNo, 1);
	misc_deregister(  &cyhMISC  );
}
module_init(Hi_module);
module_exit(Hi_cleanup_module);
/*
struct miscdevice  {
	int minor;
	const char *name;
	const struct file_operations *fops;
	struct list_head list;
	struct device *parent;
	struct device *this_device;
	const struct attribute_group **groups;
	const char *nodename;
	umode_t mode;
};
extern int misc_register(struct miscdevice *misc);
extern void misc_deregister(struct miscdevice *misc);


static inline long copy_from_user(void *to,
		const void __user * from, unsigned long n)
static inline long copy_to_user(void __user *to,
		const void *from, unsigned long n)

extern int register_chrdev_region(dev_t, unsigned, const char *);
extern void unregister_chrdev_region(dev_t, unsigned);
#define MAJOR(dev)	((unsigned int) ((dev) >> MINORBITS))
#define MINOR(dev)	((unsigned int) ((dev) & MINORMASK))
#define MKDEV(ma,mi)	(((ma) << MINORBITS) | (mi))

struct cdev {
	struct kobject kobj;
	struct module *owner;
	const struct file_operations *ops;
	struct list_head list;
	dev_t dev;
	unsigned int count;
};

void cdev_init(struct cdev *, const struct file_operations *);

struct cdev *cdev_alloc(void);

int cdev_add(struct cdev *, dev_t, unsigned);

void cdev_del(struct cdev *);


struct file_operations {
	struct module *owner;
	loff_t (*llseek) (struct file *, loff_t, int);
	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	ssize_t (*read_iter) (struct kiocb *, struct iov_iter *);
	ssize_t (*write_iter) (struct kiocb *, struct iov_iter *);
	int (*iterate) (struct file *, struct dir_context *);
	int (*iterate_shared) (struct file *, struct dir_context *);
	unsigned int (*poll) (struct file *, struct poll_table_struct *);
	long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
	long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
	int (*mmap) (struct file *, struct vm_area_struct *);
	int (*open) (struct inode *, struct file *);
	int (*flush) (struct file *, fl_owner_t id);
	int (*release) (struct inode *, struct file *);
	int (*fsync) (struct file *, loff_t, loff_t, int datasync);
	int (*fasync) (int, struct file *, int);
	int (*lock) (struct file *, int, struct file_lock *);
	ssize_t (*sendpage) (struct file *, struct page *, int, size_t, loff_t *, int);
	unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
	int (*check_flags)(int);
	int (*flock) (struct file *, int, struct file_lock *);
	ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
	ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
	int (*setlease)(struct file *, long, struct file_lock **, void **);
	long (*fallocate)(struct file *file, int mode, loff_t offset,
			  loff_t len);
	void (*show_fdinfo)(struct seq_file *m, struct file *f);
#ifndef CONFIG_MMU
	unsigned (*mmap_capabilities)(struct file *);
#endif
	ssize_t (*copy_file_range)(struct file *, loff_t, struct file *,
			loff_t, size_t, unsigned int);
	int (*clone_file_range)(struct file *, loff_t, struct file *, loff_t,
			u64);
	ssize_t (*dedupe_file_range)(struct file *, u64, u64, struct file *,
			u64);
};



struct file {
	union {
		struct llist_node	fu_llist;
		struct rcu_head 	fu_rcuhead;
	} f_u;
	struct path		f_path;
	struct inode		*f_inode;	 
	const struct file_operations	*f_op;
    //Protects f_ep_links, f_flags.
    //Must not be taken from IRQ context.
    //
	spinlock_t		f_lock;
	atomic_long_t		f_count;
	unsigned int 		f_flags;
	fmode_t			f_mode;
	struct mutex		f_pos_lock;
	loff_t			f_pos;
	struct fown_struct	f_owner;
	const struct cred	*f_cred;
	struct file_ra_state	f_ra;

	u64			f_version;
#ifdef CONFIG_SECURITY
	void			*f_security;
#endif
	//needed for tty driver, and maybe others  
	void			*private_data;

#ifdef CONFIG_EPOLL
	//Used by fs/eventpoll.c to link all the hooks to this file //
	struct list_head	f_ep_links;
	struct list_head	f_tfile_llink;
#endif // #ifdef CONFIG_EPOLL  
	struct address_space	*f_mapping;
} __attribute__((aligned(4)));












#define module_init(initfn)					\
	static inline initcall_t __maybe_unused __inittest(void)		\
	{ return initfn; }					\
	int init_module(void) __attribute__((alias(#initfn)));

//This is only required if you want to be unloadable.
#define module_exit(exitfn)					\
	static inline exitcall_t __maybe_unused __exittest(void)		\
	{ return exitfn; }					\
	void cleanup_module(void) __attribute__((alias(#exitfn)));
*/
/*
	static inline initcall_t __maybe_unused __inittest(void)		\
	{ return Hi_module; }					\
	int init_module(void) __attribute__((alias(#Hi_module)));   // alias( "Hi_module" )
*/


