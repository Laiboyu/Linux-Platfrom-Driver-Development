#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/ioctl.h>
#include <linux/slab.h>    // kmalloc/kfree 定義在這裡
MODULE_LICENSE("Dual BSD/GPL");

#define BUF_SIZE 64
#define DEVICE_NAME "joey_misc"

static dev_t devNo;
static struct cdev CdevObj;
static char wBUF[64];
static char* kBUF;

static int charDrv_open(struct inode *inode, struct file *file)
{
	printk(KERN_ALERT "charDrv_open\n");
	return 0;
}
static int charDrv_release(struct inode *inode, struct file *file)
{
	printk(KERN_ALERT "charDrv_release\n");
	return 0;  
}

static ssize_t charDrv_read(struct file *file, char __user *BUF, size_t cyh, loff_t *off)
{   
	int retV;
	static char lkBUF[BUF_SIZE];  
	
    memset(lkBUF, 0, BUF_SIZE);
    printk(KERN_ALERT "charDrv_read\n");
    
    strcpy( lkBUF, "Happy " );
    strcat( lkBUF, kBUF);
	
	// 將資料拷貝回 user space
    //copy_to_user(void __user *to, const void *from, unsigned long n)
    retV = copy_to_user((char __user *)BUF, (void*)lkBUF, BUF_SIZE);
	if(retV != 0) 
	{
		printk(KERN_ALERT "copy_to_user failed, %d bytes not copied\n", retV);
		return -EFAULT;
	}

    return strlen(lkBUF);
}

static ssize_t charDrv_write(struct file *file, const char __user *BUF, size_t size, loff_t *off)
{    
	int retV;
    printk(KERN_ALERT "charDrv_write\n");
    memset(kBUF, 0, BUF_SIZE);
	
	//copy_from_user(void *to, const void __user *from, unsigned long n)
    retV = copy_from_user((void *)kBUF, (char __user *)BUF, BUF_SIZE);
	if(retV != 0) 
	{
		printk(KERN_ALERT "copy_from_user failed, %d bytes not copied\n", retV);
		return -EFAULT;
	}

    return strlen(kBUF); 
}

static struct file_operations fops = {
	read:charDrv_read,			//ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	write:charDrv_write,		//ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	open:charDrv_open, 			//int (*open) (struct inode *, struct file *);
	release:charDrv_release,	//int (*release) (struct inode *, struct file *);
};

static struct miscdevice joey_misc_device = {
	.minor 	= 88,				// 分配 minor number 為 88
	.name 	= DEVICE_NAME,
	.fops	= &fops
};

static int __init miscDrv_init_module(void)
{
	int retVal;
	printk(KERN_ALERT "Hello, this is joey's Miscellaneous Device Driver example\n"); 
	
	retVal = misc_register(&joey_misc_device);
	if(retVal)
		printk(KERN_ERR "Failed to register misc device\n");
	else
		printk(KERN_ALERT "joey_misc device registered\n");
	
	return 0;
}

static void __exit miscDrv_exit_module(void)
{
	printk(KERN_ALERT "GoodBye, Joey\n");
	
	misc_deregister(&joey_misc_device);
}

// insmod
module_init(miscDrv_init_module);
// rmmod
module_exit(miscDrv_exit_module);