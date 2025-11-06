#include <linux/init.h>
#include <linux/module.h>
#include <linux/of.h>

MODULE_LICENSE("Dual BSD/GPL");
static struct device_node* dtNode;

static __init int Hi_init_module(void)
{   
	int x1 = 10;   
	int lenp; 
	char* propStr; 
	
	printk(KERN_ALERT "Happy, world, x1=%d, &x1=%p \r\n", x1, &x1);
	
	dtNode = of_find_compatible_node( NULL , NULL , "allwinner,h616" );
	propStr = (char*)of_get_property(dtNode,"model", &lenp );
	printk(KERN_ALERT "propStr=%s, lenp=%d\r\n", propStr, lenp );
	
	return 0;
}
static __exit void Hi_cleanup_module(void)
{
	printk(KERN_ALERT "GoodBye, world\r\n");
}

module_init(Hi_init_module);
module_exit(Hi_cleanup_module);
