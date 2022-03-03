#ifndef _CONFIG_SECRET_H
#define _CONFIG_SECRET_H

// Rename config_secret_template.h into config_secret.h to activate the content

// Wifi
#define IOT_CONFIG_WIFI_SSID            "MySSID"
#define IOT_CONFIG_WIFI_PASSWORD        "MyPassword"

// FritzBox
#define FRITZ_IP_ADDRESS "fritz.box"    // URL of FritzBox (needed for https)
//#define FRITZ_IP_ADDRESS "192.168.1.1"   // (example) IP Address of FritzBox (not for https transmission)
//#define FRITZ_IP_ADDRESS "192.168.179.1" // IP Address of FritzBox (not for https transmission)
                                        // Change for your needs

#define FRITZ_USER "thisUser"           // FritzBox User (may be empty)
#define FRITZ_PASSWORD "MySecretName"   // FritzBox Password

#define FRITZ_DEVICE_AIN_01 "12345 0123456" // AIN = Actor ID Numberof Fritz!Dect (12 digits separated by a space)
#define FRITZ_DEVICE_AIN_02 ""              // must be entered exactly in this format
#define FRITZ_DEVICE_AIN_03 ""
#define FRITZ_DEVICE_AIN_04 ""

// MAC-Addresses of different devices (max 3) of different users (max 4)
// Set empty string if not available

#define MAX_NUMBER_OF_USERS 4   // Don't change, otherwise code has to be changed
#define MAX_DEVICES 3           // Don't change, otherwise code has to be changed

#define USER_1_MAC_1 "17:80:1E:22:35:67"   // Special Phone
#define USER_1_MAC_2 ""
#define USER_1_MAC_3 ""

#define USER_2_MAC_1 ""
#define USER_2_MAC_2 ""
#define USER_2_MAC_3 ""

#define USER_3_MAC_1 ""
#define USER_3_MAC_2 ""
#define USER_3_MAC_3 ""

#define USER_4_MAC_1 ""
#define USER_4_MAC_2 ""
#define USER_4_MAC_3 ""

#endif // _CONFIG_SECRET_H