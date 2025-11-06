#include <linux/init.h>
#include <linux/timer.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

MODULE_LICENSE("Dual BSD/GPL");
static struct tasklet_struct my_tasklet;

void my_tasklet_func(unsigned long arg)
{
	for(int n=0; n<20; n++)
	{
		printk(KERN_ALERT "n=%d\r\n", n);
		mdelay(1000);
	}
}

irqreturn_t my_ISR(int irq, void *dev_id)
{
    printk(KERN_ALERT "ISR triggered\n");
    tasklet_schedule(&my_tasklet); // 延後處理
    return IRQ_HANDLED;
}

static int __init Init_module(void)
{
	int x1=10;  
	printk(KERN_ALERT "Hello, start init module\n");
	
	tasklet_init(&my_tasklet, my_tasklet_func, 123);
	// 這裡通常會 request_irq()
	return 0;
}

static void __exit Exit_module(void)
{
	printk(KERN_ALERT "GoodBye, Joey\n");
	tasklet_kill(&my_tasklet);
}

// insmod
module_init(Init_module);
// rmmod
module_exit(Exit_module);