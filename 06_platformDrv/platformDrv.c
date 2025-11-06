#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>

#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/pm.h>

MODULE_LICENSE("Dual BSD/GPL");

/*
#define PLATFORM_DEVID_NONE	(-1)
#define PLATFORM_DEVID_AUTO	(-2)

struct device { // ...
	struct bus_type	*bus;		//type of bus device is on/
	struct device_driver *driver;	// which driver has allocated this   device /
	void *platform_data;	//Platform specific data, device core doesn't touch it/
	void *driver_data;	// Driver data, set and get with dev_set/get_drvdata /
...
};
*/
struct platformDrv_user_data
{
  char 	cUser_Name[ 16 ];
  int 	iUser_Age;
};

int pfd_probe(struct platform_device *pd)
{
	printk(KERN_ALERT "platformDrv_probe\n");
	return 0;
}
int pfd_remove(struct platform_device *pd)
{
	printk(KERN_ALERT "platformDrv_remove\n");
	return 0;	
}
static int myDrv_dev_pm_ops_suspend(struct device *dev)
{
	printk(KERN_ALERT "myDrv_dev_pm_ops_suspend\n");
	return 0;		
}
int myDrv_dev_pm_ops_resume(struct device *dev)
{
	printk(KERN_ALERT "myDrv_dev_pm_ops_resume\n");
	return 0;			
}

//新增電源管理功能，系統進入睡眠或喚醒時，kernel 會呼叫這些 callback。
struct dev_pm_ops   myDrv_dev_pm_ops = {
	.suspend= myDrv_dev_pm_ops_suspend, //int (*suspend)(struct device *dev);
	.resume = myDrv_dev_pm_ops_resume 	//int (*resume)(struct device *dev);
};

struct platformDrv_user_data p_userdata = { "Joey", 27 };

struct platform_device Joey_platform_device =  {
	.name="Joey platform device",
	.id=PLATFORM_DEVID_NONE,  //,
	.dev.platform_data = &p_userdata   //struct device dev;
};

struct platform_driver Joey_platform_driver = {
	.probe = pfd_probe, 
	.remove = pfd_remove, 
	.driver = 
	{
		.name = "Joey platform device",
		.owner = THIS_MODULE,
		.pm = &myDrv_dev_pm_ops
	}
};

static int My_platformDrv_init(void)
{ 
	int retV;
	printk(KERN_ALERT "Hello, this is Joey's platform driver example\n");

	retV = platform_device_register( &Joey_platform_device );
	retV = platform_driver_probe( &Joey_platform_driver , pfd_probe );
    
	return 0;
}
static void My_platformDrv_exit(void)
{
	printk(KERN_ALERT "Goodbye, Have a good day\n");
	platform_device_unregister( &my_device );
}

module_init(My_platformDrv_init);
module_exit(My_platformDrv_exit);
/*
platform_device.h
extern int platform_device_register(struct platform_device *);
extern void platform_device_unregister(struct platform_device *);

#define platform_driver_probe(drv, probe) \
	__platform_driver_probe(drv, probe, THIS_MODULE)
extern int __platform_driver_probe(struct platform_driver *driver,
		int (*probe)(struct platform_device *), struct module *module);

struct platform_driver {
	int (*probe)(struct platform_device *);
	int (*remove)(struct platform_device *);
	void (*shutdown)(struct platform_device *);
	int (*suspend)(struct platform_device *, pm_message_t state);
	int (*resume)(struct platform_device *);
	struct device_driver driver;
	const struct platform_device_id *id_table;
	bool prevent_deferred_probe;
};
struct platform_device {
	const char	*name;
	int		id;
	bool		id_auto;
	struct device	dev;
	u32		num_resources;
	struct resource	*resource;

	const struct platform_device_id	*id_entry;
	char *driver_override; // Driver name to force a match/

	// MFD cell pointer/
	struct mfd_cell *mfd_cell;

	//arch specific additions
	struct pdev_archdata	archdata;
};
struct device_driver {
	const char		*name;
	struct bus_type		*bus;

	struct module		*owner;
	const char		*mod_name;	// used for built-in modules /

	bool suppress_bind_attrs;	// disables bind/unbind via sysfs /
	enum probe_type probe_type;

	const struct of_device_id	*of_match_table;
	const struct acpi_device_id	*acpi_match_table;

	int (*probe) (struct device *dev);
	int (*remove) (struct device *dev);
	void (*shutdown) (struct device *dev);
	int (*suspend) (struct device *dev, pm_message_t state);
	int (*resume) (struct device *dev);
	const struct attribute_group **groups;

	const struct dev_pm_ops *pm;

	struct driver_private *p;
};
struct dev_pm_ops {
	int (*prepare)(struct device *dev);
	void (*complete)(struct device *dev);
	int (*suspend)(struct device *dev);
	int (*resume)(struct device *dev);
	int (*freeze)(struct device *dev);
	int (*thaw)(struct device *dev);
	int (*poweroff)(struct device *dev);
	int (*restore)(struct device *dev);
	int (*suspend_late)(struct device *dev);
	int (*resume_early)(struct device *dev);
	int (*freeze_late)(struct device *dev);
	int (*thaw_early)(struct device *dev);
	int (*poweroff_late)(struct device *dev);
	int (*restore_early)(struct device *dev);
	int (*suspend_noirq)(struct device *dev);
	int (*resume_noirq)(struct device *dev);
	int (*freeze_noirq)(struct device *dev);
	int (*thaw_noirq)(struct device *dev);
	int (*poweroff_noirq)(struct device *dev);
	int (*restore_noirq)(struct device *dev);
	int (*runtime_suspend)(struct device *dev);
	int (*runtime_resume)(struct device *dev);
	int (*runtime_idle)(struct device *dev);
};
struct device { // ...
	struct bus_type	*bus;		//type of bus device is on/
	struct device_driver *driver;	// which driver has allocated this   device /
	void *platform_data;	//Platform specific data, device core doesn't touch it/
	void *driver_data;	// Driver data, set and get with dev_set/get_drvdata /
...
};



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
{
	unsigned long res = n;
	might_fault();
	if (likely(access_ok(VERIFY_READ, from, n)))
		res = __copy_from_user(to, from, n);
	if (unlikely(res))
		memset(to + (n - res), 0, res);
	return res;
}

static inline long copy_to_user(void __user *to,
		const void *from, unsigned long n)
{
	might_fault();
	if (access_ok(VERIFY_WRITE, to, n))
		return __copy_to_user(to, from, n);
	else
		return n;
}



#define MAJOR(dev)	((unsigned int) ((dev) >> MINORBITS))
#define MINOR(dev)	((unsigned int) ((dev) & MINORMASK))
#define MKDEV(ma,mi)	(((ma) << MINORBITS) | (mi))

extern int register_chrdev_region(dev_t, unsigned, const char *);
extern void unregister_chrdev_region(dev_t, unsigned);

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

*/

