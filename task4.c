#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>

#define to_my_obj(x) container_of(x, struct my_obj, kobj)
#define to_my_attr(x) container_of(x, struct my_attribute, attr)

struct my_obj 
{
	struct kobject kobj;
	int foo;
	int bar;
	int baz;
};

static struct kset *my_kset;
static struct my_obj *foo_obj;
static struct my_obj *bar_obj;
static struct my_obj *baz_obj;

struct my_attribute 
{
	struct attribute attr;
	ssize_t (*show)(struct my_obj *foo, struct my_attribute *attr, char *buf);
	ssize_t (*store)(struct my_obj *foo, struct my_attribute *attr, const char *buf,size_t count);
};


static ssize_t my_attr_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	struct my_attribute *attribute;
	struct my_obj *foo;
	
	printk(KERN_INFO "In my_attr_show\n");
	dump_stack();
	
	attribute = to_my_attr(attr);
	foo = to_my_obj(kobj);

	if(!attribute->show)
		return -EIO;

	return attribute->show(foo, attribute, buf);
}

static ssize_t my_attr_store(struct kobject *kobj, struct attribute *attr, const char *buf, size_t len)
{
	struct my_attribute *attribute;
	struct my_obj *foo;
	
	printk(KERN_INFO "In my_attr_store\n");
	dump_stack();
		
	attribute = to_my_attr(attr);
	foo = to_my_obj(kobj);
	
	if(!attribute->store)
		return -EIO;

	return attribute->store(foo, attribute, buf, len);
}

static struct sysfs_ops my_sysfs_ops = {
	.show = my_attr_show,
	.store = my_attr_store,
};

static void my_release(struct kobject *kobj)
{
	struct my_obj *obj;
	
	printk(KERN_INFO "In my_release\n");
	dump_stack();
	
	obj = to_my_obj(kobj);
	kfree(obj);
}


static ssize_t my_show(struct my_obj *obj, struct my_attribute *attr, char *buf)
{
	printk(KERN_INFO "In my_show\n");
	dump_stack();

	return sprintf(buf, "%d\n", obj->foo);
}

static ssize_t my_store (struct my_obj *obj, struct my_attribute *attr, const char *buf, size_t count)
{
	printk(KERN_INFO "In my_store\n");
	dump_stack();
	sscanf(buf, "%du", &obj->foo);

	return count;
}

static ssize_t b_show(struct my_obj *obj, struct my_attribute *attr, char *buf)
{
	int var;
	
	printk(KERN_INFO "In my_show\n");
	dump_stack();

	if(strcmp(attr->attr.name, "dev_param2") == 0)
		var = obj->bar;
	else
		var = obj->baz;

	return sprintf(buf, "%d\n", var);
}

static ssize_t b_store (struct my_obj *obj, struct my_attribute *attr, const char *buf, size_t count)
{
	int var;

	printk(KERN_INFO "In my_store\n");
	dump_stack();
	sscanf(buf, "%du", &var);

	if(strcmp(attr->attr.name, "dev_param2") == 0)
		obj->bar = var;
	else
		obj->baz = var;

	return count;
}

static struct my_attribute foo_attribute = __ATTR(dev_param1, 0666, my_show, my_store);
static struct my_attribute bar_attribute = __ATTR(dev_param2, 0666, b_show, b_store);
static struct my_attribute baz_attribute = __ATTR(dev_param3, 0666, b_show, b_store);

static struct attribute *my_default_attrs[] = {
	&foo_attribute.attr,
	&bar_attribute.attr,
	&baz_attribute.attr,
	NULL,
};

static struct kobj_type my_ktype = {
	.sysfs_ops = &my_sysfs_ops,  
	.release = my_release,
	.default_attrs = my_default_attrs,
};

static struct my_obj *create_my_obj(const char *name)
{
	struct my_obj *obj;
	int ret;
	
	printk(KERN_INFO "In create_my_obj\n");
	dump_stack();

	obj = kzalloc(sizeof(*obj), GFP_KERNEL);
	if(!obj)
		return NULL;
	
	obj->kobj.kset = my_kset;

	ret = kobject_init_and_add(&obj->kobj, &my_ktype, NULL, "%s", name);
	if(ret)
	{
		kfree(obj);
		return NULL;
	}

	return obj;
}

static void destroy_my_obj(struct my_obj *foo)
{
	printk(KERN_INFO "In destroy_my_obj\n");
	dump_stack();

	kobject_put(&foo->kobj);
}

static void __exit task_exit(void)
{
	printk(KERN_INFO "In task_exit\n");
	dump_stack();
	
	destroy_my_obj(foo_obj);
	destroy_my_obj(bar_obj);
	destroy_my_obj(baz_obj);

	kset_unregister(my_kset);
}

static int __init task_init(void)
{
	printk(KERN_INFO "In task_init\n");
	dump_stack();
	
	my_kset = kset_create_and_add("kset_device_paul", NULL, kernel_kobj);
	if(!my_kset)
	{
		return -ENOMEM;
	}

	foo_obj = create_my_obj("dev0");
	if(!foo_obj)
		goto foo_error;
	
	bar_obj = create_my_obj("dev1");
	if(!foo_obj)
		goto bar_error;

	baz_obj = create_my_obj("dev2");
	if(!foo_obj)
		goto baz_error;

	return 0;

baz_error:
	destroy_my_obj(bar_obj);
bar_error:
	destroy_my_obj(foo_obj);
foo_error:
	return -EINVAL;
}

module_init(task_init);
module_exit(task_exit);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR ("Paul Prince <paulprince_r@live.com>");
