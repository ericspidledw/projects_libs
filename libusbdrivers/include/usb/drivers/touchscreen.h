/*
 * Copyright 20924, Dornerworks
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _USB_TOUCHSCREEN_H
#define _USB_TOUCHSCREEN_H

#include <platsupport/serial.h>
#include <usb/usb.h>

int usb_touchscreen_bind(usb_dev_t *usb_dev);
int usb_touchscreen_configure(usb_dev_t *udev);


int usb_touchscreen_write(usb_dev_t *udev, void *buf, int len);
int usb_touchscreen_read(usb_dev_t *udev);

#endif /* _USB_PL2303_H_ */


struct touchscreen_event {
    uint8_t tmp;
};
