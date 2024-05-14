// SPDX-License-Identifier: GPL-2.0
/*
 * Simple pei driver to support PEI error injection
 *
 * Copyright (C) 2024 ByteDance, Inc.
 * Author:
 *      guojinhui.liam@bytedance.com
 *
 * Usage is simple, allocate a new id to the pei driver and bind the
 * device to it.  For example:
 *
 * # echo "8086 10f5" > /sys/bus/pci/drivers/pei-drv/new_id
 * # echo -n 0000:00:19.0 > /sys/bus/pci/drivers/e1000e/unbind
 * # echo -n 0000:00:19.0 > /sys/bus/pci/drivers/pei-drv/bind
 * # ls -l /sys/bus/pci/devices/0000:00:19.0/driver
 * .../0000:00:19.0/driver -> ../../../bus/pci/drivers/pei-drv
 */

#include <linux/module.h>
#include <linux/pci.h>

static char ids[1024] __initdata;

module_param_string(ids, ids, sizeof(ids), 0);
MODULE_PARM_DESC(ids, "Initial PCI IDs to add to the pei driver, format is "
                 "\"vendor:device[:subvendor[:subdevice[:class[:class_mask]]]]\""
                 " and multiple comma separated entries can be specified");

static int pei_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
        pci_info(pdev, "claimed by PEI drv\n");
        return 0;
}

static void pei_remove(struct pci_dev *pdev)
{
        pci_info(pdev, "remove PEI drv\n");
}

static pci_ers_result_t pei_error_detected(struct pci_dev *pdev,
                        pci_channel_state_t state)
{
        return PCI_ERS_RESULT_NEED_RESET;
}

static pci_ers_result_t pei_slot_reset(struct pci_dev *pdev)
{
        return PCI_ERS_RESULT_RECOVERED;
}

static void pei_error_resume(struct pci_dev *pdev)
{
        return;
}

static const struct pci_error_handlers pei_err_handler = {
    .error_detected = pei_error_detected,
    .slot_reset = pei_slot_reset,
    .resume     = pei_error_resume,
};

static struct pci_driver pei_driver = {
        .name           = "pei-drv",
        .id_table       = NULL, /* only dynamic id's */
        .probe          = pei_probe,
        .remove         = pei_remove,
        .err_handler    = &pei_err_handler,
};

static int __init pei_init(void)
{
        char *p, *id;
        int rc;

        rc = pci_register_driver(&pei_driver);
        if (rc)
                return rc;

        /* no ids passed actually */
        if (ids[0] == '\0')
                return 0;

        /* add ids specified in the module parameter */
        p = ids;
        while ((id = strsep(&p, ","))) {
                unsigned int vendor, device, subvendor = PCI_ANY_ID,
                        subdevice = PCI_ANY_ID, class = 0, class_mask = 0;
                int fields;

                if (!strlen(id))
                        continue;

                fields = sscanf(id, "%x:%x:%x:%x:%x:%x",
                                &vendor, &device, &subvendor, &subdevice,
                                &class, &class_mask);

                if (fields < 2) {
                        pr_warn("pei-drv: invalid ID string \"%s\"\n", id);
                        continue;
                }

                pr_info("pei-drv: add %04X:%04X sub=%04X:%04X cls=%08X/%08X\n",
                       vendor, device, subvendor, subdevice, class, class_mask);

                rc = pci_add_dynid(&pei_driver, vendor, device,
                                   subvendor, subdevice, class, class_mask, 0);
                if (rc)
                        pr_warn("pei-drv: failed to add dynamic ID (%d)\n",
                                rc);
        }

        return 0;
}

static void __exit pei_exit(void)
{
        pci_unregister_driver(&pei_driver);
}

module_init(pei_init);
module_exit(pei_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jinhui Guo <guojinhui.liam@bytedance.com>");
