#include <linux/init.h>
#include <linux/timer.h>
#include <linux/string.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");

void timer_function(struct timer_list*);

DEFINE_TIMER(TLVar, timer_function);

void timer_function(struct timer_list* arg)
{
	static int n = 0;
	char kBUF[32];
	strcpy(kBUF, (char*)"Hi Joey");
	printk(KERN_ALERT "n=%d, kBUF=%s\r\n", n++, kBUF);
	
	if(n<10)
		mod_timer(&TLVar, jiffies+HZ);
		
}

static int __init Init_module(void)
{
	int x1=10;  
	printk(KERN_ALERT "Happy, Joey, x1=%d, &x1=%p \r\n", x1, &x1);
	
	TLVar.expires = jiffies+HZ;
	add_timer( &TLVar );
	
	//init_timer(&TLVar);  // for 4.x
	//setup_timer(&TLVar, timer_function, (unsigned long)mess );
    //TLVar.data = (struct timer_list*)mess;  // for 4.x
	
	return 0;
}

static void __exit Exit_module(void)
{
	printk(KERN_ALERT "GoodBye, Joey\n");
	del_timer(&TLVar);
}

// insmod
module_init(Init_module);
// rmmod
module_exit(Exit_module);

/*
struct timer_list {

	struct hlist_node	entry;
	unsigned long		expires;
	void			(*function)(struct timer_list *);
	u32			.，;

#ifdef CONFIG_LOCKDEP
	struct lockdep_map	lockdep_map;
#endif

	ANDROID_KABI_RESERVE(1);
	ANDROID_KABI_RESERVE(2);
};
*/