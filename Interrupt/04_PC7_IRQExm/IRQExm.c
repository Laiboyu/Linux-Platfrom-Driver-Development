#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/gpio.h>
#include <linux/jiffies.h>
#include <linux/interrupt.h>
MODULE_LICENSE("Dual BSD/GPL");

// static 這個變數只能在這個檔案中存取（file scope）。
static dev_t devNo;
static struct cdev pCdevObj;
static char kBUF[64];
static unsigned int* pPC7_DATAreg; //Phisical Address: 0x300B058

static irqreturn_t PC7_irq_handler_t(int irq, void* devID)
{
	int* recDEVID;
	recDEVID = (int*)devID;
	printk(KERN_ALERT "PC7_irq_handler_t, *recDEVID=%d\r\n", *recDEVID);
	
	return 0;
}

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
	static char lkBUF[64];  
	
    memset( lkBUF, 0, 64);
    printk(KERN_ALERT "charDrv_read\n");
    
    strcpy( lkBUF, "Happy " );
    strcat( lkBUF, kBUF);
	
    //copy_to_user(void __user *to, const void *from, unsigned long n)
    retV = copy_to_user((void*)BUF, (void*)lkBUF, 64);
    
	return strlen(lkBUF); //? The return value is the number of bytes actually read.
}

static ssize_t charDrv_write(struct file *file, const char __user *BUF, size_t size, loff_t *off)
{    
	int retV;
	char CData;
    printk(KERN_ALERT "charDrv_write\n");
	memset(kBUF, 0, 64);
	
	//copy_from_user(void *to, const void __user *from, unsigned long n)
    retV = copy_from_user((void *)&kBUF, (void*)BUF, size);
	
    return strlen(kBUF); 
}

static struct file_operations fops = {
	read:charDrv_read,//ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	write:charDrv_write,//ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	open:charDrv_open, //int (*open) (struct inode *, struct file *);
	release:charDrv_release,//int (*release) (struct inode *, struct file *);
};

static int pass2PC7IRQ = 1;
static int __init IRQ_init_module(void)
{
	int retV;	unsigned long j;	int x1;
	
	printk(KERN_ALERT "Hello, Joey\n"); 
	devNo = MKDEV(88, 0); // mknod /dev/CharDrv1  c  88 0  /dev/CharDrv1
	retV = register_chrdev_region(devNo, 1, "CharDrv_Joey"); //向kernel註冊設備號 devNo
	
	cdev_init(&pCdevObj, &fops);
	// cdev_add 將字元裝置加入內核，讓它可以處理 open, read, write, ioctl 等系統呼叫。
	retV = cdev_add(&pCdevObj, devNo, 1); 
	
	retV = gpio_request(70, "PC115_GPIO" );  // 75 ->70
	printk(KERN_ALERT "gpio_request retV=%d\r\n", retV );
	gpio_direction_output( 70 , 1);
    retV = gpio_request(69, "PC115_GPIO" );
	printk(KERN_ALERT "gpio_request retV=%d\r\n", retV );
	gpio_direction_output( 69 , 1);
	
    j = jiffies + 1 * HZ;
    while (jiffies < j)
     ;
	gpio_direction_output( 70 , 0);
	gpio_direction_output( 69 , 0);	

	for(x1=0; x1<10; x1++ )
	{
		gpio_direction_output( 70 , 1);
		gpio_direction_output( 69 , 1);	
		j = jiffies + 1 * HZ;
		while (jiffies < j);

		gpio_direction_output( 70 , 0);
		gpio_direction_output( 69 , 0);	
		j = jiffies + 1 * HZ;
		while (jiffies < j);
    }
	
	retV = request_irq( gpio_to_irq( 71 ) , PC7_irq_handler_t , IRQF_TRIGGER_FALLING,"cyh71irq", (void*)&pass2PC7IRQ);

	
	return 0;
}

static void __exit IRQ_exit_module(void)
{
	printk(KERN_ALERT "GoodBye, Joey\n");
	
	free_irq( gpio_to_irq( 71 ) , (void*)&pass2PC7IRQ );
	gpio_free(70);
	gpio_free(69);
	
	cdev_del( &pCdevObj );
	unregister_chrdev_region(devNo, 1);
}

// insmod
module_init(IRQ_init_module);
// rmmod
module_exit(IRQ_exit_module);

/*
static inline void __iomem *ioremap(phys_addr_t offset, size_t size)
static inline void iounmap(void __iomem *addr)
*/