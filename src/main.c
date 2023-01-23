/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <logging/log.h>
#include <dk_buttons_and_leds.h>
#include "remote.h"

//Flash Storage
#include <zephyr/device.h>
#include <string.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/fs/nvs.h>

static struct nvs_fs fs;

#define STORAGE_NODE_LABEL storage

#define NAME_ID 1
#define EMAIL_ID 2
#define ADDRESS_ID 3
#define PHONE_ID 4
//Flash Storage

//NFC
#include <nfc_t2t_lib.h>
#include <nfc/ndef/msg.h>
#include <nfc/ndef/text_rec.h>

#include <dk_buttons_and_leds.h>

#define MAX_REC_COUNT		4
#define NDEF_MSG_BUF_SIZE	128

#define NFC_FIELD_LED		DK_LED1
//NFC

#define LOG_MODULE_NAME app
LOG_MODULE_REGISTER(LOG_MODULE_NAME);
#define RUN_STATUS_LED DK_LED1
#define CONN_STATUS_LED DK_LED2
#define RUN_LED_BLINK_INTERVAL 1000

static struct bt_conn *current_conn;

static const uint8_t en_code[] = {'e', 'n'};

/* Declarations */
void on_connected(struct bt_conn *conn, uint8_t err);
void on_disconnected(struct bt_conn *conn, uint8_t reason);
void on_notif_changed(enum bt_button_notifications_enabled status);
void on_name_received(struct bt_conn *conn, const uint8_t *const data, uint16_t len);
void on_email_received(struct bt_conn *conn, const uint8_t *const data, uint16_t len);
void on_address_received(struct bt_conn *conn, const uint8_t *const data, uint16_t len);
void on_phone_received(struct bt_conn *conn, const uint8_t *const data, uint16_t len);

/* BT Functions */
struct bt_conn_cb bluetooth_callbacks = {
	.connected 		= on_connected,
	.disconnected 	= on_disconnected,
};
struct bt_remote_service_cb remote_callbacks = {
    .notif_changed = on_notif_changed,
    .name_received = on_name_received,
    .email_received = on_email_received,
	.address_received = on_address_received,
	.phone_received = on_phone_received,
};

/* Buffer used to hold an NFC NDEF message. */
static uint8_t ndef_msg_buf[NDEF_MSG_BUF_SIZE];


/* Callbacks */
static void nfc_callback(void *context,
			 nfc_t2t_event_t event,
			 const uint8_t *data,
			 size_t data_length)
{
	ARG_UNUSED(context);
	ARG_UNUSED(data);
	ARG_UNUSED(data_length);

	switch (event) {
	case NFC_T2T_EVENT_FIELD_ON:
		dk_set_led_off(NFC_FIELD_LED);
		break;
	case NFC_T2T_EVENT_FIELD_OFF:
		dk_set_led_on(NFC_FIELD_LED);
		break;
	default:
		break;
	}
}

void on_connected(struct bt_conn *conn, uint8_t err)
{
	if(err) {
		LOG_ERR("connection err: %d", err);
		return;
	}
	LOG_INF("Connected.");
	current_conn = bt_conn_ref(conn);
	dk_set_led_on(CONN_STATUS_LED);
}

void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
	LOG_INF("Disconnected (reason: %d)", reason);
	dk_set_led_off(CONN_STATUS_LED);
	if(current_conn) {
		bt_conn_unref(current_conn);
		current_conn = NULL;
	}
}

void on_notif_changed(enum bt_button_notifications_enabled status)
{
    if (status == BT_BUTTON_NOTIFICATIONS_ENABLED) {
        LOG_INF("Notifications enabled");
    } else {
        LOG_INF("Notifications disabled");
    }
}


void on_name_received(struct bt_conn *conn, const uint8_t *const data, uint16_t len)
{
	char buf[16];
	uint32_t sz = sizeof(ndef_msg_buf);

    char temp_str[len+1];
    memcpy(temp_str, data, len);
    temp_str[len] = 0x00;

    LOG_INF("Received data on conn %p. Len: %d", (void *)conn, len);
	
	strcpy(buf, temp_str);
	(void)nvs_write(&fs, NAME_ID, &buf, strlen(buf)+1);
	LOG_INF("Adding name %s at id %d\n", buf, NAME_ID);

//	msg_encode(ndef_msg_buf, &sz);
}

void on_email_received(struct bt_conn *conn, const uint8_t *const data, uint16_t len)
{
	char buf[16];
	uint32_t sz = sizeof(ndef_msg_buf);

    char temp_str[len+1];
    memcpy(temp_str, data, len);
    temp_str[len] = 0x00;

    LOG_INF("Received data on conn %p. Len: %d", (void *)conn, len);
	
	strcpy(buf, temp_str);
	(void)nvs_write(&fs, EMAIL_ID, &buf, strlen(buf)+1);
	LOG_INF("Adding email %s at id %d\n", buf, EMAIL_ID);
	
//	msg_encode(ndef_msg_buf, &sz);
}

void on_address_received(struct bt_conn *conn, const uint8_t *const data, uint16_t len)
{
	char buf[16];
	uint32_t sz = sizeof(ndef_msg_buf);

    char temp_str[len+1];
    memcpy(temp_str, data, len);
    temp_str[len] = 0x00;

    LOG_INF("Received data on conn %p. Len: %d", (void *)conn, len);
	
	strcpy(buf, temp_str);
	(void)nvs_write(&fs, ADDRESS_ID, &buf, strlen(buf)+1);
	LOG_INF("Adding address %s at id %d\n", buf, ADDRESS_ID);
	
// msg_encode(ndef_msg_buf, &sz);
}

void on_phone_received(struct bt_conn *conn, const uint8_t *const data, uint16_t len)
{
	char buf[16];
	uint32_t sz = sizeof(ndef_msg_buf);

    char temp_str[len+1];
    memcpy(temp_str, data, len);
    temp_str[len] = 0x00;

    LOG_INF("Received data on conn %p. Len: %d", (void *)conn, len);
	
	strcpy(buf, temp_str);
	(void)nvs_write(&fs, PHONE_ID, &buf, strlen(buf)+1);
	LOG_INF("Adding phone %s at id %d\n", buf, PHONE_ID);
	
	//msg_encode(ndef_msg_buf, &sz);
}

// void button_handler(uint32_t button_state, uint32_t has_changed)
// {
//     int err;
// 	int button_pressed = 0;
// 	if (has_changed & button_state)
// 	{
// 		switch (has_changed)
// 		{
// 			case DK_BTN1_MSK:
// 				button_pressed = 1;
// 				break;
// 			case DK_BTN2_MSK:
// 				button_pressed = 2;
// 				break;
// 			case DK_BTN3_MSK:
// 				button_pressed = 3;
// 				break;
// 			case DK_BTN4_MSK:
// 				button_pressed = 4;
// 				break;
// 			default:
// 				break;
// 		}
//         LOG_INF("Button %d pressed.", button_pressed);
//         set_button_value(button_pressed);
//         err = send_button_notification(current_conn, button_pressed);
//         if (err) {
//             LOG_ERR("Couldn't send notificaton. (err: %d)", err);
//         }
//     }
// }

/* Configurations */
static void configure_dk_buttons_leds(void)
{
    int err;
    err = dk_leds_init();
    if (err) {
        LOG_ERR("Couldn't init LEDS (err %d)", err);
    }
    // err = dk_buttons_init(button_handler);
    // if (err) {
    //     LOG_ERR("Couldn't init buttons (err %d)", err);
    // }
}

void nvsinit(void)
{
	int rc = 0;
	struct flash_pages_info info;

	fs.flash_device = FLASH_AREA_DEVICE(STORAGE_NODE_LABEL);
	if (!device_is_ready(fs.flash_device)) {
		LOG_INF("Flash device %s is not ready\n", fs.flash_device->name);
		return;
	}
	fs.offset = FLASH_AREA_OFFSET(STORAGE_NODE_LABEL);
	rc = flash_get_page_info_by_offs(fs.flash_device, fs.offset, &info);
	if (rc) {
		LOG_INF("Unable to get page info\n");
		return;
	}
	fs.sector_size = info.size;
	fs.sector_count = 3U;

	rc = nvs_mount(&fs);
	if (rc) {
		LOG_INF("Flash Init failed\n");
		return;
	}
}

//writes to nfc 

int msg_encode(uint8_t *buffer, uint32_t *sz)
{
	int err;
	char nmbuf[16];
	char embuf[16];
	char adbuf[16];
	char phbuf[16];

	NFC_NDEF_MSG_DEF(nfc_text_msg, 6);

	nfc_ndef_msg_clear(&NFC_NDEF_MSG(nfc_text_msg));

//Only showing one input (name) for the sake of simplicity

	uint8_t name_size = nvs_read(&fs, NAME_ID, &nmbuf, sizeof(nmbuf));

	uint8_t email_size = nvs_read(&fs, EMAIL_ID, &embuf, sizeof(embuf));

	uint8_t add_size = nvs_read(&fs, ADDRESS_ID, &adbuf, sizeof(adbuf));

	uint8_t ph_size = nvs_read(&fs, PHONE_ID, &phbuf, sizeof(phbuf));

	LOG_INF("Data Read!\n");

	NFC_NDEF_TEXT_RECORD_DESC_DEF(nfc_name,
				      UTF_8,
				      en_code,
				      sizeof(en_code),
				      nmbuf,
				      name_size-1);

	NFC_NDEF_TEXT_RECORD_DESC_DEF(nfc_email,
				      UTF_8,
				      en_code,
				      sizeof(en_code),
				      embuf,
				      email_size-1);

	NFC_NDEF_TEXT_RECORD_DESC_DEF(nfc_address,
				      UTF_8,
				      en_code,
				      sizeof(en_code),
				      adbuf,
				      add_size-1);

	NFC_NDEF_TEXT_RECORD_DESC_DEF(nfc_phone,
				      UTF_8,
				      en_code,
				      sizeof(en_code),
				      phbuf,
				      ph_size-1);

	LOG_INF("Records Created!\n");

	/* Create NFC NDEF message description, capacity - MAX_REC_COUNT
	 * records
	 */

	/* Add text records to NDEF text message */
	err = nfc_ndef_msg_record_add(&NFC_NDEF_MSG(nfc_text_msg),
				   &NFC_NDEF_TEXT_RECORD_DESC(nfc_name));
	if (err < 0) {
		LOG_INF("Cannot add name!\n");
		return err;
	}

	err = nfc_ndef_msg_record_add(&NFC_NDEF_MSG(nfc_text_msg),
				   &NFC_NDEF_TEXT_RECORD_DESC(nfc_email));
	if (err < 0) {
		LOG_INF("Cannot add email!\n");
		return err;
	}

	err = nfc_ndef_msg_record_add(&NFC_NDEF_MSG(nfc_text_msg),
				   &NFC_NDEF_TEXT_RECORD_DESC(nfc_address));
	if (err < 0) {
		LOG_INF("Cannot add address!\n");
		return err;
	}

	err = nfc_ndef_msg_record_add(&NFC_NDEF_MSG(nfc_text_msg),
				   &NFC_NDEF_TEXT_RECORD_DESC(nfc_phone));
	if (err < 0) {
		LOG_INF("Cannot add phone!\n");
		return err;
	}

	LOG_INF("Records added to message!\n");

	err = nfc_ndef_msg_encode(&NFC_NDEF_MSG(nfc_text_msg),
				      buffer,
				      sz);
	if (err < 0) {
		LOG_INF("Cannot encode data!\n");
	}

	LOG_INF("Records Encoded!\n");
	
	return err;
}

/* Main */
void main(void)
{
    int err;
    int blink_status = 0;
	int rc = 0;
	char buf[16];

	// LOG_INF("Hello World! %s\n", CONFIG_BOARD);

    configure_dk_buttons_leds();

	nvsinit();

	rc = nvs_read(&fs, NAME_ID, &buf, sizeof(buf));
	if (rc > 0) { /* item was found, show it */
	LOG_INF("Id: %d, Name: %s\n", NAME_ID, buf);
	} else   {/* item was not found, add it */
	strcpy(buf, "Bob Belcher");
	LOG_INF("No name found, adding %s at id %d\n", buf, NAME_ID);
	(void)nvs_write(&fs, NAME_ID, &buf, strlen(buf)+1);
	}

	rc = nvs_read(&fs, EMAIL_ID, &buf, sizeof(buf));
	if (rc > 0) { /* item was found, show it */
	LOG_INF("Id: %d, Email: %s\n", EMAIL_ID, buf);
	} else   {/* item was not found, add it */
	strcpy(buf, "bob@burgers.com");
	LOG_INF("No email found, adding %s at id %d\n", buf, EMAIL_ID);
	(void)nvs_write(&fs, EMAIL_ID, &buf, strlen(buf)+1);
	}

	rc = nvs_read(&fs, ADDRESS_ID, &buf, sizeof(buf));
	if (rc > 0) { /* item was found, show it */
	LOG_INF("Id: %d, Address: %s\n", ADDRESS_ID, buf);
	} else   {/* item was not found, add it */
	strcpy(buf, "Omaha, NE");
	LOG_INF("No address found, adding %s at id %d\n", buf, ADDRESS_ID);
	(void)nvs_write(&fs, ADDRESS_ID, &buf, strlen(buf)+1);
	}

	rc = nvs_read(&fs, PHONE_ID, &buf, sizeof(buf));
	if (rc > 0) { /* item was found, show it */
	LOG_INF("Id: %d, Phone: %s\n", PHONE_ID, buf);
	} else   {/* item was not found, add it */
	strcpy(buf, "12345678");
	LOG_INF("No phone found, adding %s at id %d\n", buf, PHONE_ID);
	(void)nvs_write(&fs, PHONE_ID, &buf, strlen(buf)+1);
	}
	
	uint32_t sz = sizeof(ndef_msg_buf);

	LOG_INF("Starting NFC Text Record example\n");

	/* Set up NFC */
	if (nfc_t2t_setup(nfc_callback, NULL) < 0) {
		LOG_INF("Cannot setup NFC T2T library!\n");
	}

	/* Encode welcome message */
	if (msg_encode(ndef_msg_buf, &sz) < 0) {
		LOG_INF("Cannot encode message!\n");
	}

	/* Set created message as the NFC payload */
	if (nfc_t2t_payload_set(ndef_msg_buf, sz) < 0) {
		LOG_INF("Cannot set payload!\n");
	}

	/* Start sensing NFC field */
	if (nfc_t2t_emulation_start() < 0) {
		LOG_INF("Cannot start emulation!\n");
	}
	LOG_INF("NFC configuration done\n");

    err = bluetooth_init(&bluetooth_callbacks, &remote_callbacks);
    if (err) {
        LOG_INF("Couldn't initialize Bluetooth. err: %d", err);
    }

    LOG_INF("Running...");
    for (;;) {
        dk_set_led(RUN_STATUS_LED, (blink_status++)%2);
        k_sleep(K_MSEC(RUN_LED_BLINK_INTERVAL));
    }
}
