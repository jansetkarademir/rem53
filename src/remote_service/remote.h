
#include <zephyr.h>
#include <logging/log.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <bluetooth/hci.h>

/** @brief UUID of the Remote Service. **/
#define BT_UUID_REMOTE_SERV_VAL \
	BT_UUID_128_ENCODE(0xe9ea0001, 0xe19b, 0x482d, 0x9293, 0xc7907585fc48)

/** @brief UUID of the Button Characteristic. **/
#define BT_UUID_REMOTE_BUTTON_CHRC_VAL \
	BT_UUID_128_ENCODE(0xe9ea0002, 0xe19b, 0x482d, 0x9293, 0xc7907585fc48)

/** @brief UUID of the Name Characteristic. **/
#define BT_UUID_REMOTE_NAME_CHRC_VAL \
	BT_UUID_128_ENCODE(0xe9ea0003, 0xe19b, 0x482d, 0x9293, 0xc7907585fc48)

/** @brief UUID of the Email Characteristic. **/
#define BT_UUID_REMOTE_EMAIL_CHRC_VAL \
	BT_UUID_128_ENCODE(0xe9ea0004, 0xe19b, 0x482d, 0x9293, 0xc7907585fc48)

/** @brief UUID of the Address Characteristic. **/
#define BT_UUID_REMOTE_ADDRESS_CHRC_VAL \
	BT_UUID_128_ENCODE(0xe9ea0005, 0xe19b, 0x482d, 0x9293, 0xc7907585fc48)

/** @brief UUID of the Phone Characteristic. **/
#define BT_UUID_REMOTE_PHONE_CHRC_VAL \
	BT_UUID_128_ENCODE(0xe9ea0006, 0xe19b, 0x482d, 0x9293, 0xc7907585fc48)


#define BT_UUID_REMOTE_SERVICE          BT_UUID_DECLARE_128(BT_UUID_REMOTE_SERV_VAL)
#define BT_UUID_REMOTE_BUTTON_CHRC 	    BT_UUID_DECLARE_128(BT_UUID_REMOTE_BUTTON_CHRC_VAL)
#define BT_UUID_REMOTE_NAME_CHRC 		BT_UUID_DECLARE_128(BT_UUID_REMOTE_NAME_CHRC_VAL)
#define BT_UUID_REMOTE_EMAIL_CHRC 		BT_UUID_DECLARE_128(BT_UUID_REMOTE_EMAIL_CHRC_VAL)
#define BT_UUID_REMOTE_ADDRESS_CHRC 	BT_UUID_DECLARE_128(BT_UUID_REMOTE_ADDRESS_CHRC_VAL)
#define BT_UUID_REMOTE_PHONE_CHRC 		BT_UUID_DECLARE_128(BT_UUID_REMOTE_PHONE_CHRC_VAL)

enum bt_button_notifications_enabled {
	BT_BUTTON_NOTIFICATIONS_ENABLED,
	BT_BUTTON_NOTIFICATIONS_DISABLED,
};

struct bt_remote_service_cb {
	void (*notif_changed)(enum bt_button_notifications_enabled status);
    void (*name_received)(struct bt_conn *conn, const uint8_t *const data, uint16_t len);
    void (*email_received)(struct bt_conn *conn, const uint8_t *const data, uint16_t len);
    void (*address_received)(struct bt_conn *conn, const uint8_t *const data, uint16_t len);
    void (*phone_received)(struct bt_conn *conn, const uint8_t *const data, uint16_t len);
};

int send_button_notification(struct bt_conn *conn, uint8_t value);
void set_button_value(uint8_t btn_value);
int bluetooth_init(struct bt_conn_cb *bt_cb, struct bt_remote_service_cb *remote_cb);
