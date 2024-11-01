#ifndef __XHCI_DEBUG__
#define __XHCI_DEBUG__
#include "./xhci.h"


// static void print_endpoint_ctx(struct xhci_ep_ctx* the_ctx)
// {

// 	ZF_LOGE("----- dumping endpoint context ----- ");
// 	ZF_LOGE("Endpoint state %d", the_ctx->ep_info & 0x7);
// 	ZF_LOGE("Endpoint mult %d", (the_ctx->ep_info >> 8) & 0x3);
// 	ZF_LOGE("Max primary stream %d", (the_ctx->ep_info >> 10) & 0xf);
// 	ZF_LOGE("LSA %d", (the_ctx->ep_info >> 15) & 0x1);
// 	ZF_LOGE("Interval %d", (the_ctx->ep_info >> 16) & 0xff);
// 	ZF_LOGE("MAX ESIT Hi %d", (the_ctx->ep_info >> 24) & 0xff);

// 	ZF_LOGE("Err cnt is %d", (the_ctx->ep_info2 >> 1) & 0x3);
// 	ZF_LOGE("Ep type is %d", (the_ctx->ep_info2 >> 3) & 0x7);
// 	ZF_LOGE("Hid is %d", (the_ctx->ep_info2 >> 7) & 0x1);
// 	ZF_LOGE("Max burst size is %d", (the_ctx->ep_info2 >> 8) & 0xff);
// 	ZF_LOGE("Max packet size is %d", (the_ctx->ep_info2 >> 16) & 0xffff);

// 	ZF_LOGE("Deq cycle state is %d", (the_ctx->deq) & (0x1));
// 	ZF_LOGE("Dequeue pointer is %lx", (the_ctx->deq) & ~(0xf));

// 	ZF_LOGE("avg trb len is %d", the_ctx->tx_info & 0xffff);
// 	ZF_LOGE("Max ESIT payload is %d", (the_ctx->tx_info >> 16) & 0xffff);
// 	ZF_LOGE("------------------- ");

// }

// static void print_slot_ctx(struct xhci_slot_ctx* ctx)
// {
// 	ZF_LOGE("----- dumping slot context -----");
// 	ZF_LOGE("Route string is %lx", ctx->dev_info & 0x7FFFF);
// 	ZF_LOGE("Slot speed is 0x%lx", (ctx->dev_info >> 20) & 0xf);
// 	ZF_LOGE("Slot MTT is 0x%lx", (ctx->dev_info >> 25) & 0x1);
// 	ZF_LOGE("Slot Hub is 0x%lx", (ctx->dev_info >> 26) & 0x1);
// 	ZF_LOGE("Context entries is 0x%lx", (ctx->dev_info >> 27) & 0x1f);

// 	ZF_LOGE("Max exit latency is 0x%x", (ctx->dev_info2) & 0xffff);
// 	ZF_LOGE("Root Hub port number is %d", (ctx->dev_info2 >> 16) & 0xff);
// 	ZF_LOGE("Number of ports %d", (ctx->dev_info2 >> 24) & 0xff);

// 	ZF_LOGE("Parent hub slot id is %d", (ctx->tt_info) & 0xff);
// 	ZF_LOGE("Parent port number is %d", (ctx->tt_info >> 8) & 0xff);
// 	ZF_LOGE("TT Think Time is %d", (ctx->tt_info >> 16) & 0x3);
// 	ZF_LOGE("Interrupter target is %d", (ctx->tt_info >> 22) & 0x3ff);

// 	ZF_LOGE("USB device address is %d", (ctx->dev_state) & 0xff);
// 	ZF_LOGE("Slot state is %d", (ctx->dev_state >> 27) & 0x1f);

// 	ZF_LOGE("------------------------------");

// }

static void print_input_ctrl_context(struct xhci_input_control_ctx* input) {
	// ZF_LOGE("------- Dumping input context -------");
	ZF_LOGE("Drop flags is 0x%x", input->drop_flags & ~(0x3));
	ZF_LOGE("Add flags is 0x%x", input->add_flags);
	// ZF_LOGE("------------------------");
}

#define PFIELD(d, x) printf("\t0x%-4x : %s\n", (d)->x, #x)
#define PFIELD2(d, x) printf("\t0x%-4x : %-20s | ", (d)->x, #x)
static void xhci_usb_print_descriptor(struct usb_descriptor_header *desc, int index)
{
	return;
	int type = desc->bDescriptorType;
	switch (desc->bDescriptorType) {
	case DEVICE:{
		struct device_desc d;
		memcpy(&d, desc, desc->bLength);
		printf("Device descriptor:\n");
		PFIELD(&d, bLength);
		PFIELD(&d, bDescriptorType);
		PFIELD(&d, bcdUSB);
		PFIELD2(&d, bDeviceClass);
		printf("%s\n", usb_class_get_description(d.bDeviceClass));
		PFIELD2(&d, bDeviceSubClass);
		printf("%s\n", usb_class_get_description(d.bDeviceSubClass));
		PFIELD(&d, bDeviceProtocol);
		PFIELD(&d, bMaxPacketSize0);
		PFIELD(&d, idVendor);
		PFIELD(&d, idProduct);
		PFIELD(&d, bcdDevice);
		PFIELD(&d, iManufacturer);
		PFIELD(&d, iProduct);
		PFIELD(&d, iSerialNumber);
		PFIELD(&d, bNumConfigurations);
		break;}
	case CONFIGURATION:{
		struct config_desc d;
		memcpy(&d, desc, desc->bLength);
		if (index >= 0) {
			printf("Config descriptor %d\n", index);
		} else {
			printf("Config descriptor\n");
		}
		PFIELD(&d, bLength);
		PFIELD(&d, bDescriptorType);
		PFIELD(&d, wTotalLength);
		PFIELD(&d, bNumInterfaces);
		PFIELD(&d, bConfigurationValue);
		PFIELD(&d, iConfigurationIndex);
		PFIELD2(&d, bmAttributes);
		printf("%s ", (d.bmAttributes & (1 << 6)) ?
		       "Self powered" : "");
		printf("%s ", (d.bmAttributes & (1 << 5)) ?
		       "Remote wakeup" : "");
		printf("%s ", (d.bmAttributes & (1 << 7)) ?
		       "" : "Warning: bit 7 should be set");
		printf("%s ", (d.bmAttributes & (1 << 4)) ?
		       "Warning: bit 5 should not be set" : "");
		printf("\n");
		PFIELD2(&d, bMaxPower);
		printf("%dmA\n", d.bMaxPower * 2);
		break;}
	case INTERFACE:{
		struct iface_desc d;
		memcpy(&d, desc, desc->bLength);
		if (index >= 0) {
			printf("Interface descriptor %d\n", index);
		} else {
			printf("Interface descriptor\n");
		}
		PFIELD(&d, bLength);
		PFIELD(&d, bDescriptorType);
		PFIELD(&d, bInterfaceNumber);
		PFIELD(&d, bAlternateSetting);
		PFIELD(&d, bNumEndpoints);
		PFIELD2(&d, bInterfaceClass);
		printf("%s\n", usb_class_get_description(d.bInterfaceClass));
		PFIELD2(&d, bInterfaceSubClass);
		printf("%s\n", usb_class_get_description(d.bInterfaceSubClass));
		PFIELD(&d, bInterfaceProtocol);
		PFIELD(&d, iInterface);
		break;}
	case ENDPOINT:{
		struct endpoint_desc d;
		memcpy(&d, desc, desc->bLength);
		if (index >= 0) {
			printf("Endpoint descriptor %d\n", index);
		} else {
			printf("Endpoint descriptor\n");
		}
		PFIELD(&d, bLength);
		PFIELD(&d, bDescriptorType);
		PFIELD2(&d, bEndpointAddress);
		printf("%d-", d.bEndpointAddress & 0xf);
		if (d.bmAttributes & 0x3) {
			if (d.bEndpointAddress & (1 << 7)) {
				printf("IN");
			} else {
				printf("OUT");
			}
		}
		printf("\n");
		PFIELD2(&d, bmAttributes);
		switch (d.bmAttributes & 0x3) {
		case 0:
			printf("CONTROL");
			break;
		case 1:
			printf("ISOCH");
			break;
		case 2:
			printf("BULK");
			break;
		case 3:
			printf("INT");
			break;
		}
		printf(",");
		switch ((d.bmAttributes >> 2) & 0x3) {
		case 0:
			printf("No synch");
			break;
		case 1:
			printf("Asynchronous");
			break;
		case 2:
			printf("Adaptive");
			break;
		case 3:
			printf("Synchronous");
			break;
		}
		printf(",");
		switch ((d.bmAttributes >> 4) & 0x3) {
		case 0:
			printf("DATA");
			break;
		case 1:
			printf("Feedback");
			break;
		case 2:
			printf("Implicit feedback data");
			break;
		case 3:
			printf("<Reserved>");
			break;
		}
		printf("\n");
		PFIELD2(&d, wMaxPacketSize);
		printf("%d bytes, %d xacts per uFrame\n",
		       d.wMaxPacketSize & 0x7ff,
		       ((d.wMaxPacketSize >> 11) & 0x3) + 1);
		PFIELD(&d, bInterval);
		break;}
	case DEVICE_QUALIFIER:
	case OTHER_SPEED_CONFIGURATION:
	case INTERFACE_POWER:
	case STRING:
		printf("Descriptor type %d not implemented\n", type);
		break;
		/* Class specific types */
	case HID:{
		struct hid_desc d;
		memcpy(&d, desc, desc->bLength);
		printf("HID descriptor\n");
		PFIELD(&d, bLength);
		PFIELD(&d, bDescriptorType);
		PFIELD(&d, bcdHID);
		PFIELD(&d, bCountryCode);
		PFIELD(&d, bNumDescriptors);
		PFIELD(&d, bReportDescriptorType);
		PFIELD(&d, wReportDescriptorLength);
		break;}
	default:
		printf("Unknown descriptor type %d\n", type);
	}
}


#endif