#include <linux/init.h>
#include <linux/module.h>
#include <linux/of.h>

MODULE_LICENSE("Dual BSD/GPL");
static struct device_node* dtNode;
static __init int Hi_module(void)
{   int x1=10;   char* propStr;   int lenp;
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
module_init(Hi_module);
module_exit(Hi_cleanup_module);
/*
/ {
	interrupt-parent = <0x1>;
	#address-cells = <0x2>;
	#size-cells = <0x2>;
	model = "Orange Pi Zero 2";
	compatible = "allwinner,h616", "arm,sun50iw9p1";
	...
  };
*/
/*
extern struct device_node *of_find_node_by_name(struct device_node *from,
	const char *name);
extern struct device_node *of_find_node_by_type(struct device_node *from,
	const char *type);
extern struct device_node *of_find_compatible_node(struct device_node *from,
	const char *type, const char *compat);

extern const void *of_get_property(const struct device_node *node,
				const char *name,
				int *lenp);
				
				

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


