#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/sysfs.h>
#include <linux/module.h>

static int foo;
static struct kobject *my_kobj;

static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	dump_stack();

	return sprintf(buf, "%d \n", foo);
}

static ssize_t foo_store(struct kobject *kobj, struct kobj_sttribute *attr, const char *buf, size_t count)
{
	dump_stack();

	sscanf(buf, "%d", &foo);
	return count;
}

static struct kobj_attribute foo_attribute = __ATTR(foo, 0600, foo_show, foo_store);

static struct attribure *attrs[] = {
	&foo_attribute.attr, 
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static void __exit task_exit(void)
{
	kobject_put(my_kobj);
}

static int __init task_init(void)
{
	int ret;
	
	dump_stack();

	my_kobj = kobject_create_and_add("Paul_kobject", kernel_kobj);
	if(!my_kobj)
	{
		printk(KERN_ALERT"init failure");
		return -ENOMEM;
	}

	ret = sysfs_create_group(my_kobj, &attr_group);
	if(ret)
	{
		kobject_put(my_kobj);
	}
	return ret;
}

module_init(task_init);
module_exit(task_exit);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR ("Paul Prince <paulprince_r@live.com>");

