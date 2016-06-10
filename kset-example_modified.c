/*
 * Sample kset and ktype implementation
 *
 * Copyright (C) 2004-2007 Greg Kroah-Hartman <greg@kroah.com>
 * Copyright (C) 2007 Novell Inc.
 *
 * Released under the GPL version 2 only.
 *
 */

#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include<linux/slab.h>
/*
 * This module shows how to create a kset in sysfs called
 * /sys/kernel/kset-example
 * Then tree kobjects are created and assigned to this kset, "foo", "baz",
 * and "bar".  In those kobjects, attributes of the same name are also
 * created and if an integer is written to these files, it can be later
 * read out of it.
 */


/*
 * This is our "object" that we will create a few of and register them with
 * sysfs.
 */

//a very good example - kobject{} is always embedded in a container object,
//seldom used stand alone !!!
struct foo_obj {
	struct kobject kobj;
	int foo;
	int baz;
	int bar;
};

struct ctrl_leds_obj {
	struct kobject kobj;
	int gpio_no;
        
};
 
#define to_foo_obj(x) container_of(x, struct foo_obj, kobj)
#define to_ctrl_leds_obj(x) container_of(x, struct ctrl_leds_obj, kobj)

/* a custom attribute that works just for a struct foo_obj. */
//our abstract custom attribute uses abstract basic attribute !!
//
//we will create several instances of this abstract custom attribute -
//each such instance may represent an attribute of a device type !!!

struct foo_attribute {
	struct attribute attr;
	ssize_t (*show)(struct foo_obj *foo, struct foo_attribute *attr, char *buf);
	ssize_t (*store)(struct foo_obj *foo, struct foo_attribute *attr, const char *buf, size_t count);
};

struct ctrl_leds_attribute {
	struct attribute attr;
//	ssize_t (*show)(struct foo_obj *foo, struct foo_attribute *attr, char *buf);
	ssize_t (*store)(struct ctrl_leds_obj *foo, struct ctrl_leds_attribute *attr, const char *buf, size_t count);
};

#define to_foo_attr(x) container_of(x, struct foo_attribute, attr)
#define to_ctrl_leds_attr(x) container_of(x, struct ctrl_leds_attribute, attr)

/*
 * The default show function that must be passed to sysfs.  This will be
 * called by sysfs for whenever a show function is called by the user on a
 * sysfs file associated with the kobjects we have registered.  We need to
 * transpose back from a "default" kobject to our custom struct foo_obj and
 * then call the show function for that specific object.
 */
static ssize_t ctrl_leds_attr_show(struct kobject *kobj,
			     struct attribute *attr,
			     char *buf)
{
	struct ctrl_leds_attribute *attribute;
	struct ctrl_leds_obj *foo;

        dump_stack();
	attribute = to_foo_attr(attr);
	foo = to_foo_obj(kobj);

	if (!attribute->show)
		return -EIO;

	return attribute->show(foo, attribute, buf);
}

/*
 * Just like the default show function above, but this one is for when the
 * sysfs "store" is requested (when a value is written to a file.)
 */
static ssize_t ctrl_leds_attr_store(struct kobject *kobj,
			      struct attribute *attr,
			      const char *buf, size_t len)
{
	struct ctrl_leds_attribute *attribute;
	struct ctrl_leds_obj *foo;

        dump_stack();
	attribute = to_ctrl_leds_attr(attr);
	foo = to_ctrl_leds_obj(kobj);

	if (!attribute->store)
		return -EIO;

	return attribute->store(foo, attribute, buf, len);
}

/* Our custom sysfs_ops that we will associate with our ktype later on */
static struct sysfs_ops foo_sysfs_ops = {
	.show = foo_attr_show,
	.store = foo_attr_store,
};

static struct sysfs_ops ctrl_leds_sysfs_ops = {
//	.show = ctrl_leds__attr_show,
	.store = ctrl_leds_attr_store,
};
/*
 * The release function for our object.  This is REQUIRED by the kernel to
 * have.  We free the memory held in our object here.
 *
 * NEVER try to get away with just a "blank" release function to try to be
 * smarter than the kernel.  Turns out, no one ever is...
 */
static void foo_release(struct kobject *kobj)
{
	struct foo_obj *foo;

	foo = to_foo_obj(kobj);
	kfree(foo);
}

static void ctrl_leds_release(struct kobject *kobj)
{
	struct ctrl_leds_obj *foo;

	foo = to_ctrl_leds_obj(kobj);
	gpio_free(foo->gpio_no);
        kfree(foo);
        
}
/*
 * The "foo" file where the .foo variable is read from and written to.
 */
static ssize_t foo_show(struct foo_obj *foo_obj, struct foo_attribute *attr,
			char *buf)
{
        dump_stack(); 
	return sprintf(buf, "%d\n", foo_obj->foo);
}

static ssize_t foo_store(struct foo_obj *foo_obj, struct foo_attribute *attr,
			 const char *buf, size_t count)
{

        dump_stack(); 
	sscanf(buf, "%du", &foo_obj->foo);
	return count;
}

static struct foo_attribute foo_attribute =
	__ATTR(dev_param1, 0666, foo_show, foo_store);


static ssize_t ctr_leds_store(struct ctrl_leds_obj *foo_obj, struct ctrl_leds_attribute *attr,
			 const char *buf, size_t count)
{

        int value;

        dump_stack(); 
        printk("count passed is %d\n", count);

	sscanf(buf, "%du", &value);

        if(value==1 || value==0){
           gpio_set_value(foo_obj->gpio_no,value);
           return count;
        }
        else {

             printk("error in value passed\n"); 
             return -EINVAL;

       }

}

static struct ctrl_leds_attribute ctrl_leds_attribute =
	__ATTR(set_val, 0666, NULL, ctrl_leds_store);

/*
 * More complex function where we determine which varible is being accessed by
 * looking at the attribute for the "baz" and "bar" files.
 */
static ssize_t b_show(struct foo_obj *foo_obj, struct foo_attribute *attr,
		      char *buf)
{
	int var;

	if (strcmp(attr->attr.name, "dev_param2") == 0)
		var = foo_obj->baz;
	else
		var = foo_obj->bar;
	return sprintf(buf, "%d\n", var);
}

static ssize_t b_store(struct foo_obj *foo_obj, struct foo_attribute *attr,
		       const char *buf, size_t count)
{
	int var;

	sscanf(buf, "%du", &var);
	if (strcmp(attr->attr.name, "dev_param2") == 0)
		foo_obj->baz = var;
	else
		foo_obj->bar = var;
	return count;
}

static struct foo_attribute baz_attribute =
	__ATTR(dev_param2, 0666, b_show, b_store);
static struct foo_attribute bar_attribute =
	__ATTR(dev_param3, 0666, b_show, b_store);

/*
 * Create a group of attributes so that we can create and destory them all
 * at once.
 */
static struct attribute *foo_default_attrs[] = {
	&foo_attribute.attr,
	&baz_attribute.attr,
	&bar_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};


static struct attribute *ctrl_leds_default_attrs[] = {
	&ctrl_leds_attribute.attr,
//	&baz_attribute.attr,
//	&bar_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

/*
 * Our own ktype for our kobjects.  Here we specify our sysfs ops, the
 * release function, and the set of default attributes we want created
 * whenever a kobject of this type is registered with the kernel.
 */
static struct kobj_type foo_ktype = {
	.sysfs_ops = &foo_sysfs_ops,
	.release = foo_release,
	.default_attrs = foo_default_attrs,
};


static struct kobj_type ctrl_leds_ktype = {
	.sysfs_ops = &ctrl_leds_sysfs_ops,
	.release = ctrl_leds_release,
	.default_attrs = ctrl_leds_default_attrs,
};


static struct kset *example_kset;
static struct foo_obj *foo_obj;
static struct foo_obj *bar_obj;
static struct foo_obj *baz_obj;

static struct foo_obj *create_foo_obj(const char *name)
{
	struct foo_obj *foo;
	int retval;

	/* allocate the memory for the whole object */
	foo = kzalloc(sizeof(*foo), GFP_KERNEL);
	//foo = kzalloc(sizeof(struct led_ctrl_obj), GFP_KERNEL);
	if (!foo)
		return NULL;

	/*
	 * As we have a kset for this kobject, we need to set it before calling
	 * the kobject core.
	 */
	foo->kobj.kset = example_kset;

	/*
	 * Initialize and add the kobject to the kernel.  All the default files
	 * will be created here.  As we have already specified a kset for this
	 * kobject, we don't have to set a parent for the kobject, the kobject
	 * will be placed beneath that kobject of the kset automatically.
	 */
	retval = kobject_init_and_add(&foo->kobj, &foo_ktype, NULL, "%s", name);
	//retval = kobject_init_and_add(&foo->kobj, &ctrl_leds_ktype, &pdev->dev.kobj, "%s", name);
	if (retval) {
		kfree(foo);
		return NULL;
	}

	/*
	 * We are always responsible for sending the uevent that the kobject
	 * was added to the system.
	 */
	//here, the uevent may not have any impact on the system 
        //kobject_uevent(&foo->kobj, KOBJ_ADD);

	return foo;
}

static void destroy_foo_obj(struct foo_obj *foo)
{
	kobject_put(&foo->kobj);
}

static int example_init(void)
{
	/*
	 * Create a kset with the name of "kset_example",
	 * located under /sys/kernel/
	 */

        //in this case, our kset is created and registered with 
        //LDDM and sysfs - however, kobject of the kset is actually 
        //used for such registrations and other fields of kset will 
        //be used for managing kobjects grouped under this kset and 
        //other run-time activities !!!
	example_kset = kset_create_and_add("kset_devices_typeA", NULL, kernel_kobj);
	if (!example_kset)
		return -ENOMEM;

	/*
	 * Create three device objects(representing devices) and 
           register them with our kset,using respective kobjects !!!
	 */
	foo_obj = create_foo_obj("device0");
	if (!foo_obj)
		goto foo_error;

	bar_obj = create_foo_obj("device1");
	if (!bar_obj)
		goto bar_error;

	baz_obj = create_foo_obj("device2");
	if (!baz_obj)
		goto baz_error;

	return 0;

baz_error:
	destroy_foo_obj(bar_obj);
bar_error:
	destroy_foo_obj(foo_obj);
foo_error:
	return -EINVAL;
}

static void example_exit(void)
{
	destroy_foo_obj(baz_obj);
	destroy_foo_obj(bar_obj);
	destroy_foo_obj(foo_obj);
	kset_unregister(example_kset);
}

module_init(example_init);
module_exit(example_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Greg Kroah-Hartman <greg@kroah.com>");
