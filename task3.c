#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <lnux/slab.h>

#define to_my_obj(x) container_of(x, struct my_obj, kobj)
#define to_my_attr(x) container_of(x, struct my_attribute, attr)

struct my_obj 
{
	struct kobject kobj;
	int foo;
};

static struct kset *my_kset;
static struct my_obj *foo_obj;






static ssize_t foo_show(struct foo)
{


}






static ssize_t foo_attribute foo_attribute = __ATTR(dev_param1, 0666, foo_show, foo_store);




static struct sysfs_ops my_sysfs_ops = {
	.show = my_attr_show,
	.store = my_attr_store,
};

static void my_release(struct kobject *kobj)
{
	struct my_obj *obj;
	obj = to_my_obj(kobj);
	kfree(obj);
}

static struct attribute *my_default_attrs[] = {
	&foo.attribute.attr,
	NULL,
};

static struct kobj_type my_ktype = {
	.sysfs_ops = &my_sys_ops,
	.release = my_release,
	.default_attrs = my_default_attrs,
};

static struct my_obj *create_my_obj(const char *name)
{
	struct my_obj *obj;
	int ret;

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

static void __exit task_exit(void)
{

}

static int __init task_init(void)
{
	my_kset = kset_create_and_add("kset_device_paul", NULL, kernel_kobj);
	if(!my_kset)
	{
		return -ENOMEM;
	}

	foo_obj = create_my_obj("dev0");
	if(!foo_obj)
		goto foo_error;


	return 0;

foo_error:
	return -EINVAL;
}

module_init(task_init);
module_exit(task_exit);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR ("Paul Prince <paulprince_r@live.com>");
