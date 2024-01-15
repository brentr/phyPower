// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * phyPower.c
 *   Brent Roman <brent@mbari.org> revised 1/15/24
 */

#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/regulator/consumer.h>
#include <linux/platform_device.h>
#include <linux/phy/phy.h>

MODULE_AUTHOR("Brent Roman <brent@mbari.org>");
MODULE_DESCRIPTION("Gate power to platform phys");
MODULE_LICENSE("GPL");


static int disablePhy(struct phy *phy)
{
	int try = 100;
	if (!phy->pwr)
		return -ENODEV;
	while(--try) {
    	int err;
		if (!regulator_is_enabled(phy->pwr))
			return 0;
		err = phy_power_off(phy);
		if (err)
			return err;
	}
	return -E2BIG;
}

static int enablePhy(struct phy *phy)
{
	if (!phy->pwr)
	  return -ENODEV;
	if (regulator_is_enabled(phy->pwr))
	  return 0;
	return phy_power_on(phy);
}


static int disableNamedPhy(struct device *dev, void *data)
{
	char *phyName = data;
	struct phy *phy = phy_get(dev, phyName);
	int err;
	if (IS_ERR(phy))
		return 0;
	if ((err = disablePhy(phy)))
		dev_err(dev,"Failed to disable %s power (err=%d)\n", phyName, err);
	else
		dev_info(dev, "disabled %s power\n", phyName);
	phy_put(dev, phy);
	return 1;  //stop iterating
}

static int enableNamedPhy(struct device *dev, void *data)
{
	char *phyName = data;
	struct phy *phy = phy_get(dev, phyName);
	int err;
	if (IS_ERR(phy))
		return 0;
	if ((err = enablePhy(phy)))
		dev_err(dev,"Failed to enable %s power (err=%d)\n", phyName, err);
	else
		dev_info(dev, "enabled %s power\n", phyName);
	phy_put(dev, phy);
	return 1;  //stop iterating
}

static ssize_t command_store(struct kobject *kobj, struct kobj_attribute *attr,
			   const char *buf, size_t count)
{
	int err;
	enum {cmdSizeLimit = 256};
	char stripped[cmdSizeLimit];
	char *phyName;

	if (count > cmdSizeLimit)
		printk(KERN_ERR "phyPower: command >%u bytes long?!\n", cmdSizeLimit);
	else{
	  bool enabled;
	  switch(*buf) {
		  case '+':
		    enabled=true;
		    break;
		  case '-':
		    enabled=false;
		    break;
		  default:
		    printk(KERN_ERR "phyPower: %s did not start with + or -\n", buf);
		    return count;
	  }

	  //use phy name in place, unless it's terminated with a control char
		phyName = (char *)buf+1;
		if (count > 1 && buf[count-1] < ' ') {
		  strncpy(stripped, phyName, count-2);
		  stripped[count-2] = 0;
		  phyName = stripped;
		}
		err = bus_for_each_dev(&platform_bus_type, NULL, phyName,
								enabled ? enableNamedPhy : disableNamedPhy);
	  	if (err <= 0)
		  printk(KERN_ERR "phyPower: no phy named \"%s\" (err=%d)\n", phyName, err);
	}
	return count;
}

static const struct kobj_attribute command_attribute = __ATTR_WO(command);

static struct kobject *phyPower_kobj;

static int __init phyPower_init(void)
{
	int err;
	phyPower_kobj = kobject_create_and_add("phyPower", NULL);
	if (!phyPower_kobj)
		return -ENOMEM;
	err = sysfs_create_file(phyPower_kobj, &command_attribute.attr);
	if (err)
		kobject_put(phyPower_kobj);
	return err;
}

static void __exit phyPower_exit(void)
{
	kobject_put(phyPower_kobj);
}

module_init(phyPower_init);
module_exit(phyPower_exit);
