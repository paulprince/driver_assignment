/*
 * Sample kobject implementation
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

/*
 * This module shows how to create a simple subdirectory in sysfs called
 * /sys/kernel/kobject-example  In that directory, 3 files are created:
 * "foo", "baz", and "bar".  If an integer is written to these files, it can be
 * later read out of it.
 */

static int foo;
static int baz;
static int bar;

/*
 * The "foo" file where a static variable is read from and written to.
 */
static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
        dump_stack();
	return sprintf(buf, "%d\n", foo);
}

static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{

        dump_stack();
	sscanf(buf, "%du", &foo);
	return count;
}

//we associate our useful attribute with an abstract attribute object 
//we also associate a show method and store method with our 
//useful attribute object - together they are effective !!!
static struct kobj_attribute foo_attribute =
	__ATTR(foo, 0666, foo_show, foo_store);

/*
 * More complex function where we determine which varible is being accessed by
 * looking at the attribute for the "baz" and "bar" files.
 */
static ssize_t b_show(struct kobject *kobj, struct kobj_attribute *attr,
		      char *buf)
{
	int var;
        dump_stack();

	if (strcmp(attr->attr.name, "baz") == 0)
		var = baz;
	else
		var = bar;
	return sprintf(buf, "%d\n", var);
}

static ssize_t b_store(struct kobject *kobj, struct kobj_attribute *attr,
		       const char *buf, size_t count)
{
	int var;
        dump_stack();

	sscanf(buf, "%du", &var);
	if (strcmp(attr->attr.name, "baz") == 0)
		baz = var;
	else
		bar = var;
	return count;
}

static struct kobj_attribute baz_attribute =
	__ATTR(baz, 0666, b_show, b_store);
static struct kobj_attribute bar_attribute =
	__ATTR(bar, 0666, b_show, b_store);

//the following objects are used to manage the attributes declared above 
//this is one way of setting up attributes - we will see another 
//method, in the next example !!!
/*
 * Create a group of attributes so that we can create and destory them all
 * at once.
 */
static struct attribute *attrs[] = {
	&foo_attribute.attr,
	&baz_attribute.attr,
	&bar_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

/*
 * An unnamed attribute group will put all of the attributes directly in
 * the kobject directory.  If we specify a name, a subdirectory will be
 * created for the attributes with the directory being the name of the
 * attribute group.
 */
static struct attribute_group attr_group = {
	.attrs = attrs,
};

static struct kobject *example_kobj;

static int __init example_init(void)
{
	int retval=0;

	/*
	 * Create a simple(dummy) kobject with the name of "kobject_example",
	 * located under /sys/kernel/
	 *
	 * As this is a simple directory, no uevent will be sent to
	 * userspace.  That is why this function should not be used for
	 * any type of dynamic kobjects, where the name and number are
	 * not known ahead of time.
	
         */
        //we may use kobjects directly or we may use kobjects indirectly - 
        //we may use kobjects indirectly, using other objects of device model ??


        //here, a kobject{} is created, initialized , registered with 
        //unified device(driver) model(a subsystem) and also registered with 
        //sysfs(another subsystem) 
        //initially, understand the relationship with unified device model
        //and once clear, try to understand the relationship with 
        //sysfs pseudo fs !!!

        //in this example, we are not using a kset, but a kobject{} is
        //used as the parent - in this case, kernel_kobj is ptr to 
        //a system kobject, which may be used for experimental and 
        //testing purposes - this is one of the scenarios - in the 
        //next example(kset_example.c), 
        //we will see use of a kset{} along with our 
        //kobject !! 

        //refer to <ksrc>/lib/kobject.c 
        //in this context, parent of our kobject will be a system kobject - 
        //second parameter ??
        //creates a new kobject - initializes the kobject - registers with device model - 
        //registers with sysfs ??

	example_kobj = kobject_create_and_add("kobject_example", kernel_kobj);
	if (!example_kobj)
		return -ENOMEM;

	/* Create the files associated with this kobject */
        //with the help of sysfs_create_group(), system will
        //create one sysfs file per attribute passed via 
        //attr_group{} under sysfs directory entry of 
        //kobject {} passed as param1 !!!

        //where will the sysfs_dirent{} objects corresponding 
        //to regular files of attributes be maintained ??
        //hint: they will be maintained under a specific 
        //sysfs_dirent{} object of the sysfs !! which one ???
        	
        //in this case, regular sysfs files are created for one or 
        //more attributes - these are created under kobject's sysfs directory 
        //what is the use of these regular sysfs files ??

        //when we are registering our attributes with device model/sysfs. 
        //we are also registering our attributes' call backs - show()/store()
        //
        //when we access our sysfs regular file of our attribute, we are 
        //eventually accessing our attribute/kobject/call backs - 
        //this is what enables us to actually take actions, when a read call 
        //and a write call are invoked on an attribute's sysfs file ??

        //these call backs have strict prototypes - these call backs are 
        //passed certain objects - these objects help us do more productive 
        //work - these objects enable us to write re-entrant call backs, which 
        //can be effective for several device instances ??
        //
        //assignment requires following changes :
        //
        //you need to add 2 more attributes - in this case, you must 
        //use sysfs_create_file() API for registering attributes
        //
        //we need to create 2 more attributes with appropriate names/show/store
        //you need to use a single set of show/store between the 2 attributes 
        //
        //code, build, and test the attributes - check call trace,using dump stack ??


        retval = sysfs_create_group(example_kobj, &attr_group);
	if (retval)
		kobject_put(example_kobj);

         //sysfs_create_file(example_kobj, &new_attribute1.attr);
         //sysfs_create_file(example_kobj, &new_attribute2.attr);


	return retval;
}

static void __exit example_exit(void)
{

        //this decreases the usage count of the respective kobject {} and
        //if the count drops to 0, free the superobject and kobject,
        //using destructor  !!!
        //in this case, destructor is provided by the system !!!

	kobject_put(example_kobj);
}

module_init(example_init);
module_exit(example_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Greg Kroah-Hartman <greg@kroah.com>");
