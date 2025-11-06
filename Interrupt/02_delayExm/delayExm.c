#include <linux/init.h>
#include <linux/module.h>
#include <linux/param.h>   //HZ 100 10ms
#include <linux/jiffies.h>
#include <linux/time.h>
#include <linux/wait.h>
#include <linux/delay.h>
#include <linux/sched.h>

MODULE_LICENSE("Dual BSD/GPL");

DECLARE_WAIT_QUEUE_HEAD(joey_wait_queue_head);

static __init int delay_init_module(void)
{   int x1=10;
    struct timeval timevalVar;
	printk(KERN_ALERT "Happy, world, x1=%d, &x1=%p \r\n", x1, &x1);
	
	printk(KERN_ALERT "jiffies=%ld \r\n", jiffies ); 
	//do_gettimeofday( &timevalVar );
	//printk(KERN_ALERT "timevalVar.tv_sec=%d, timevalVar.tv_usec=%d \r\n", timevalVar.tv_sec, timevalVar.tv_usec ); 
	
	// Method 1: BusyWait
	{ 
		int n1 = 0; unsigned long j;
		for( n1 = 0; n1 < 10; n1++ )
		{
			printk(KERN_ALERT "n1=%d \r\n", n1 );  // 1Sec

			j = jiffies + 1 * HZ;
			while (jiffzies < j) ;   // BusyWait
		}
    }
	
	// Method 2: schedule();
	{ 
		int n1=0; unsigned long j;
		for( n1=0; n1<10; n1++ )
		{
			printk(KERN_ALERT "n1=%d \r\n", n1 );

			j = jiffies + 1 * HZ;
			while (jiffies < j)
				schedule();  // OS Ready/Running/Wait&Block
		}
	}
	
	// Method 3: schedule_timeout
    {  
		int n1=0; 
		for( n1=0; n1<10; n1++ )
		{
			printk(KERN_ALERT "n1=%d \r\n", n1 );

			set_current_state(TASK_INTERRUPTIBLE);
			schedule_timeout( 1*HZ );
		}
    }	
	
	//Methed 4: wait_event_timeout
	{ 
		int n1=0; unsigned long j;
		for( n1=0; n1<10; n1++ )
		{
			printk(KERN_ALERT "n1=%d \r\n", n1 );
			wait_event_timeout( joey_wait_queue_head , 0, 1 * HZ );
			// --> cyh_wait_queue_head => Block/WaitQueue HZ(100)==> 99 98=> 1 0
			// 0 -> Ready
		}
    }
	// ...  tasklet()   work_queue()   {{{irq+kmalloc( size, GFP_KERNEL )}}XX
	
	//Method 5: mdelay
	{  
		int n1=0; 
		for( n1=0; n1<10; n1++ )
		{
			printk(KERN_ALERT "n1=%d \r\n", n1 );
			mdelay( 1000 );
		}
	}	

	return 0;
}
static __exit void delay_cleanup_module(void)
{
	printk(KERN_ALERT "GoodBye, world\r\n");
}

module_init(delay_init_module);
module_exit(delay_cleanup_module);
