// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2015, Google, Inc
 * Written by Simon Glass <sjg@chromium.org>
 * All rights reserved.
 */

// #include <common.h>
// #include <dm.h>
// #include <dm/device_compat.h>
// #include <init.h>
// #include <log.h>
// #include <pci.h>
// #include <reset.h>
#include <usb.h>
#include <xhci.h>
#include <usb/usb_host.h>

#include <platsupport/io.h>
#include <pci/pci.h>
#include <pci/ioreg.h>

#include <usb/usb_host.h>
#include "../../services.h"


// struct xhci_pci_plat {
// 	struct reset_ctl reset;
// };

static int _irq_line;
uintptr_t xhci_pci_init(uint16_t vid, uint16_t did, ps_io_ops_t *io_ops)
{
	int err;
	libpci_device_t *controller_dev;
	volatile struct xhci_hccr *cap_regs;
	volatile struct xhci_hcor *op_regs;
	uint32_t val;
	uint16_t extra_caps1;
	uint16_t extra_caps2;

	/* Find the device */
	libpci_scan(io_ops->io_port_ops);
	controller_dev = libpci_find_device(vid, did);
	if (controller_dev) {
		libpci_device_iocfg_debug_print(&controller_dev->cfg, false);
		libpci_read_ioconfig(&controller_dev->cfg, controller_dev->bus,
			controller_dev->dev, controller_dev->fun);
		/* Map device memory */
		printf("Mapping in XHCI Dev at %p with size of %zu\n",
			controller_dev->cfg.base_addr[0], controller_dev->cfg.base_addr_size[0]);
		cap_regs = MAP_DEVICE(io_ops,
				controller_dev->cfg.base_addr[0],
				controller_dev->cfg.base_addr_size[0]); // here we map in the controller registers
		printf("Cap regs is %p\n", cap_regs);
		if (!cap_regs) {
			ZF_LOGF("Invalid Registers\n");
		}
		_irq_line = controller_dev->interrupt_line;
		extra_caps1 = cap_regs->cr_hccparams;
		if(extra_caps1){
			ZF_LOGE("We got extra capabilities for the USB host\n");
			printf("Extra caps is %d\n", extra_caps1);
		}
	} else {
		ZF_LOGF("XHCI: Host device not found!\n");
	}

	// hccr = (struct xhci_hccr *)dm_pci_map_bar(dev, should get this from the mapping above probably not needed?
	// 		PCI_BASE_ADDRESS_0, 0, 0, PCI_REGION_TYPE,
	// 		PCI_REGION_MEM);
	// if (!hccr) {
	// 	printf("xhci-pci init cannot map PCI mem bar\n");
	// 	return -EIO;
	// }
	/* enable busmaster */ // is this needed?
	// dm_pci_read_config32(dev, PCI_COMMAND, &cmd);
	// cmd |= PCI_COMMAND_MASTER;
	// dm_pci_write_config32(dev, PCI_COMMAND, cmd);
	return (uintptr_t) cap_regs;
}

// static int xhci_pci_probe(struct udevice *dev)
// {
// 	// struct xhci_pci_plat *plat = dev_get_plat(dev);
// 	struct xhci_hccr *hccr;
// 	struct xhci_hcor *hcor;
// 	int ret;

// 	// ret = reset_get_by_index(dev, 0, &plat->reset);
// 	// if (ret && ret != -ENOENT && ret != -ENOTSUPP) {
// 	// 	dev_err(dev, "failed to get reset\n");
// 	// 	return ret;
// 	// }

// 	// if (reset_valid(&plat->reset)) {
// 	// 	ret = reset_assert(&plat->reset);
// 	// 	if (ret)
// 	// 		goto err_reset;

// 	// 	ret = reset_deassert(&plat->reset);
// 	// 	if (ret)
// 	// 		goto err_reset;
// 	// }

// 	ret = xhci_pci_init(dev, &hccr, &hcor);
// 	if (ret)
// 		goto err_reset;

// 	ret = xhci_register(dev, hccr, hcor);
// 	if (ret)
// 		goto err_reset;

// 	return 0;

// err_reset:
// 	if (reset_valid(&plat->reset))
// 		reset_free(&plat->reset);

// 	return ret;
// }

// static int xhci_pci_remove(struct udevice *dev)
// {
// 	struct xhci_pci_plat *plat = dev_get_plat(dev);

// 	xhci_deregister(dev);
// 	if (reset_valid(&plat->reset))
// 		reset_free(&plat->reset);

// 	return 0;
// }
