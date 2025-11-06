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
