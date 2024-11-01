/*
 * Copyright 2024, Dornerworks
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

/**
 * @brief USB HID Touchscreen driver
 * @see USB HID spec Appendix B
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <usb/drivers/touchscreen.h>
#include "../services.h"
#include "hid.h"

struct usb_touchscreen_dev {
	struct usb_dev *udev;
	struct usb_hid_device *hid;
	struct endpoint *ep_int;
	struct xact int_xact;
	struct touchscreen_event event;
};

static struct usb_touchscreen_dev* touchscreen;

int touchscreen_irq_handler(void* token, enum usb_xact_status stat, int bytes_remaining)
{
    ZF_LOGE("WE in the touch screen irq handler fr fr ");
}



static void dump_touchscreen_data(void* buffer, size_t max_size){
    return;
    size_t cnt = 0;
    char* the_buffer = (char*) buffer;
    printf("--------- Touchscreen Packet ---------\n");
    while(cnt < max_size){
        printf("Byte %02d 0x%02x\n", cnt, (uint8_t) the_buffer[cnt++]);
    }

}

int usb_touchscreen_bind(usb_dev_t *usb_dev){

    int err = 0;

    touchscreen = (struct usb_touchscreen_dev*) usb_malloc(sizeof(struct usb_touchscreen_dev));
    if(!touchscreen){
        ZF_LOGF("Out of memory for touchscreen");
    }
    usb_dev->dev_data = (struct udev_priv*)touchscreen;
    touchscreen->udev = usb_dev;

    touchscreen->hid = usb_hid_alloc(usb_dev);
    assert(touchscreen->hid);
    touchscreen->hid->iface = 0;
    touchscreen->ep_int = usb_dev->ep[touchscreen->hid->iface];

    if(touchscreen->ep_int == NULL || touchscreen->ep_int->type != EP_INTERRUPT){
        ZF_LOGE("Touchscreen ep int is %p", touchscreen->ep_int);
        ZF_LOGE("Touchscreen ep type is %d", touchscreen->ep_int->type);
        ZF_LOGF("Interrupt EP for touchscreen not found...");
    }

    // for(int i = 0; i < 32; i++){
    //     struct endpoint* ep = usb_dev->ep[i];
    //     if(ep &&  ep->type == EP_INTERRUPT){
    //         ZF_LOGE("------- EP %d ----------", i);
    //         ZF_LOGE("Ep dir is %d", ep->dir);
    //         ZF_LOGE("Ep interval is %d", ep->interval);
    //         ZF_LOGE("Ep maxpacket is %d", ep->max_pkt);
    //         ZF_LOGE("Ep num is %d",ep->num);
    //         ZF_LOGE("Ep type is %d", ep->type);
    //     }
    // }
    // ZF_LOGE("IN A WHILE 1");
    // while(1);

    if (touchscreen->ep_int->dir == EP_DIR_IN) {
        ZF_LOGE("EP is in");
		touchscreen->int_xact.type = PID_IN;
	} else {
        ZF_LOGE("EP is out");
		touchscreen->int_xact.type = PID_OUT;
	}

	touchscreen->int_xact.len = 8;


	err = usb_alloc_xact(usb_dev->dman, &touchscreen->int_xact, 1);
	if (err) {
		ZF_LOGF("Out of DMA memory\n");
	}

    char* zero_buffer = touchscreen->int_xact.vaddr;
    for(int i =0; i < touchscreen->ep_int->max_pkt; i++){
        zero_buffer[i] = 0xff;
    }
    // dump_touchscreen_data(touchscreen->int_xact.vaddr,touchscreen->ep_int->max_pkt);
    int count = 0;
    while(count < 4) {
	    err = usbdev_schedule_xact(usb_dev, touchscreen->ep_int, &touchscreen->int_xact, 1,
	    		&touchscreen_irq_handler, touchscreen);

        if(err){
            ZF_LOGF("Interrupt endpoint failed with count %d", count);
        } else{
            count++;
        }

        dump_touchscreen_data(touchscreen->int_xact.vaddr,touchscreen->ep_int->max_pkt);

    }
    ZF_LOGE("FIN");
    while(1);
    return 0;
}

int usb_touchscreen_configure(usb_dev_t *udev){
    ZF_LOGE("Inside configure");
    return 0;
}

int usb_touchscreen_write(usb_dev_t *udev, void *buf, int len){
    ZF_LOGE("Inside write");

    return 0;
}



int usb_touchscreen_read(usb_dev_t *udev) {
    ZF_LOGE("Inside read");
    int err = 0;

    err = usbdev_schedule_xact(udev, touchscreen->ep_int, &touchscreen->int_xact, 1,
		&touchscreen_irq_handler, touchscreen);
    if(!err){
        dump_touchscreen_data(touchscreen->int_xact.vaddr, (size_t)touchscreen->ep_int->max_pkt);
    }

    return 0;
}