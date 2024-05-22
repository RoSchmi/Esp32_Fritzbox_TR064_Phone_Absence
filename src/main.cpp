// Program 'Esp32_Fritzbox_TR064_Phone_Absence'
// by RoSchmi
//
// This App shows examples how to check through TR-064 protocol 
// if mobile phones are absent from the Fritzbox WLAN range and 
// to switch a DECT!200 power socket to reflect the present/absent state.
// Additionally some other functions are shown.
//
// Uses a modification of the following library:
// https://github.com/Aypac/Arduino-TR-064-SOAP-Library
// https://github.com/RoSchmi/Esp32_TR064_SOAP_Library

// Some settings which have to be made on the Fritzbox can be found here:
// https://www.schlaue-huette.de/apis-co/fritz-tr064/
//
// Standardmäßig ist die TR-064 Schnittstelle nicht aktiviert.
// -->FritzBox Weboberfläche-->Expertenansicht
// Heimnetz » Heimnetzübersicht » Netzwerkeinstellungen
// --> Zugriff für Anwendungen zulassen --> neu starten
// --> Usernamen/Passwort angeben (System » FritzBox Benutzer)
// Benutzereinstellungen: „FRITZBox Einstellungen“ und „Sprachnachrichten, Faxnachrichten, FRITZApp Fon und Anrufliste“ aktivieren.
// wenn alles richtig, öffne zum Test: http://fritz.box:49000/tr64desc.xml

#include <Arduino.h>
#include "config_secret.h"
#include "config.h"

#if defined(ESP8266)
  	//Imports for ESP8266
  	#include <ESP8266WiFi.h>
  	#include <ESP8266WiFiMulti.h>
 	 #include <ESP8266HTTPClient.h>
 	 ESP8266WiFiMulti wiFiMulti;
#elif defined(ESP32)
  	//Imports for ESP32
 	 #include <WiFi.h>
  	#include <WiFiMulti.h>
  	#include <HTTPClient.h>
  	WiFiMulti wiFiMulti;
#endif

#include <tr064.h>

// Default Esp32 stack size of 8192 byte is not enough for some applications.
// --> configure stack size dynamically from code to 16384
// https://community.platformio.org/t/esp32-stack-configuration-reloaded/20994/4
// Patch: Replace C:\Users\thisUser\.platformio\packages\framework-arduinoespressif32\cores\esp32\main.cpp
// with the file 'main.cpp' from folder 'patches' of this repository, then use the following code to configure stack size
#if !(USING_DEFAULT_ARDUINO_LOOP_STACK_SIZE)
  uint16_t USER_CONFIG_ARDUINO_LOOP_STACK_SIZE = 8192;
  //uint16_t USER_CONFIG_ARDUINO_LOOP_STACK_SIZE = 16384;
  
#endif

typedef struct HostsResponse
{
    bool isValid;
    char hostName[50];
    char index[10];
    char active[3];
    char ipAddress[50];
    char macAddress[20];
} HostsResponse;

//-------------------------------------------------------------------------------------
// Put your router settings here
//-------------------------------------------------------------------------------------
// Wifi network name (SSID)
const char* wifi_ssid = IOT_CONFIG_WIFI_SSID; 
// Wifi network password
const char* wifi_password = IOT_CONFIG_WIFI_PASSWORD;
// The username if you created an account, "admin" otherwise
const char* fuser = FRITZ_USER;
// The password for the aforementioned account.
const char* fpass = FRITZ_PASSWORD;

// IP address or URL of your router. For http transmission this can be "192.168.179.1" for most FRITZ!Boxes
// For https transmission you MUST use the URL (e.g. fritz.box)
//const char* IP = "192.168.179.1";
const char* IP = FRITZ_IP_ADDRESS;

// Setting up port for http/https transmission and 
// X509Certificate when using https
#if TRANSPORT_PROTOCOL == 0
	const int PORT = 49000;
	Protocol protocol = Protocol::useHttp;
#else
    const int PORT = 49443;
	X509Certificate myX509Certificate = myfritzbox_root_ca;
	#if TRANSPORT_PROTOCOL == 1     // ssl but no rootCa validation
		Protocol protocol = Protocol::useHttpsInsec;
	#else
		Protocol protocol = Protocol::useHttps;
	#endif
#endif

// TR-064 connection
#if TRANSPORT_PROTOCOL == 0
    TR064 connection(PORT, IP, fuser, fpass);
#else
	TR064 connection(PORT, IP, fuser, fpass, protocol, myX509Certificate);
#endif

// Set flag if switching of powersocket shall be performed
#define USE_POWERSOCKETS 0
//#define USE_POWERSOCKETS 1

// The AIN of the DECT!200 powersocket can be found in the FritzBox Webinterface
// or on the device itself.
// const String Steckdose1 = "12345 0123456"; (exactly this format - 5 digits, then space, then 7 digits )
const String Steckdose1 = FRITZ_DEVICE_AIN_01;

// The number of different people/user you want to be able to detect
// If changed, other parts of the code have to be changed also
const int numUser = MAX_NUMBER_OF_USERS;

// The maximum amount of devices per user
// If changed, other parts of the code have to be changed also
const int maxDevices = MAX_DEVICES;
/*
 * The array of macs. Structure:
 * = {{"mac1:user1", "mac2:user1", ..., "macX:user1"}, {"mac1:user2", "mac2:user2", ..., "macX:user2"}, ..., {"mac1:userY", "mac2:userY", ..., "macX:userY"}};
 * Doesn't matter if upper or lowercase :)
 */
const char macsPerUser[numUser][maxDevices][18] =
    { {USER_1_MAC_1,USER_1_MAC_2, USER_1_MAC_3}, //User1
      {USER_2_MAC_1,USER_2_MAC_2, USER_2_MAC_3}, //User2
      {USER_3_MAC_1,USER_3_MAC_2, USER_3_MAC_3}, //User3
      {USER_4_MAC_1,USER_4_MAC_2, USER_4_MAC_3}};//User4

// Status arrays. 
bool onlineUsers[numUser];
bool powerSocketStates[numUser];

// Array-settings.
const String STATUS_MAC = "MAC";
const String STATUS_IP = "IP";
const String STATUS_ACTIVE = "ACTIVE";
const String STATUS_HOSTNAME = "HOSTNAME";
const int STATUS_MAC_INDEX = 0;
const int STATUS_IP_INDEX = 1;
const int STATUS_ACTIVE_INDEX = 3;
const int STATUS_HOSTNAME_INDEX = 2;

// forward declarations
void ensureWIFIConnection();
void SetSwitch(String AIN, String state);
void GetDeviceInfo(String AIN);
int getDeviceNumber();
void verboseStatus(String r[4][2]);
void getStatusOfMAC(String mac, String (&r)[4][2]);
HostsResponse getHostStatusByIndex(int index);
HostsResponse getHostStatusByMACAddress(const String mac);
void WahlRundruf();
int getWifiNumber();
void getStatusOfAllWifi(int numDev);
bool GetHostNameByIndex(int index, char * outHostName, int maxHostNameLength);
void serialEvent();

// RoSchmi
void * StackPtrAtStart;
void * StackPtrEnd;
UBaseType_t watermarkStart;
// RoSchmi

void setup() {
    Serial.begin(115200);
    while(!Serial);
    Serial.println(F("boot..."));
// RoSchmi  
void* SpStart = NULL;
  StackPtrAtStart = (void *)&SpStart;
  watermarkStart =  uxTaskGetStackHighWaterMark(NULL);
  StackPtrEnd = StackPtrAtStart - watermarkStart;

  Serial.begin(115200);
  delay(2000);

  Serial.printf("\r\n\r\nAddress of Stackpointer near start is:  %p \r\n",  (void *)StackPtrAtStart);
  Serial.printf("End of Stack is near: %p \r\n",  (void *)StackPtrEnd);
  Serial.printf("Free Stack near start is:  %d \r\n",  (uint32_t)StackPtrAtStart - (uint32_t)StackPtrEnd);

// RoSchmi


    // Connect to wifi
    ensureWIFIConnection();
    Serial.println(F("WIFI connected..."));
    
    // delay is optional
    delay(5000);

    // For debugging activate proper Debug level 
    // The wanted debug level has to be set in file config.h
    connection.debug_level = SELECTED_DEBUG_LEVEL;

    if(Serial) Serial.setDebugOutput(true);

    if(Serial) Serial.println(F("Initialize TR-064 connection\n\n"));
    connection.init();

	// Request the number of (connected) Wifi-Devices
	int numDev = getWifiNumber();
	if(Serial) Serial.printf("WIFI has %d (connected) devices.\n", numDev);

	// Check the status of all the devices connected to wifi
	getStatusOfAllWifi(numDev);

	// Get the number of all devices, that are known to this router
	numDev = getDeviceNumber();
	if (Serial) Serial.printf("Router has %d known devices.\n", numDev);
}
 
void loop() {
    ensureWIFIConnection();
    //RoSchmi
    void* SpActual = NULL;
  Serial.printf("Free Stack at actual position is: %d \r\n", (uint32_t)&SpActual - (uint32_t)StackPtrEnd);
  delay(60000);

    /*
    delay(1000);
    GetDeviceInfo(Steckdose1);
    delay(1000);
    
    SetSwitch(Steckdose1, "ON");
    Serial.println("Switched on");
    GetDeviceInfo(Steckdose1);

    delay(5000);
    SetSwitch(Steckdose1, "OFF");
    Serial.println("Switched off");
    GetDeviceInfo(Steckdose1);

    delay(5000);
    */

    //Serial.println("Wahlrundruf");
    //WahlRundruf();
     
    // For the next round, assume all users are offline
	for (int i=0;i<numUser;++i) {
		onlineUsers[i] = false;
	}

    for (int i = 0; i < numUser; ++i) 
    {
		if (Serial) Serial.printf("> USER %d -------------------------------\n",i);
        // Check for each device-mac-address if an entry exists and if the state is active
        for (int j = 0; j < maxDevices; j++ )
        {
            String curMac = macsPerUser[i][j];
            bool User_Device_Absent = (curMac == "");
            if (!User_Device_Absent)
            {
                HostsResponse hostsResponse = getHostStatusByMACAddress(curMac);
                {
                    if (hostsResponse.isValid)
                    {
                        onlineUsers[i] = onlineUsers[i] ? true : hostsResponse.active[i] == '1';
                    }
                }
            }
        }

        if (onlineUsers[i] == true)    // state of at least one device is active
        {   
            if (Serial) Serial.printf("At least one Device of User_%d is present.\n\n", i);
            
            #if USE_POWERSOCKETS == 1    // Only when powersockets shall be used
            if (i == 0)  // example: for powerSocket and device with ID = 0
            {
                if (powerSocketStates[i] != onlineUsers[i])
                {
                    powerSocketStates[i] = onlineUsers[i];
                    SetSwitch(Steckdose1, "ON");
                    Serial.println("Switched on\n");
                }
            }
            #endif
        }
        else        // state of all devices of this user is inactive
        {
            if (Serial) Serial.printf("All Devices of User_%d are absent.\n\n", i);
            
            #if USE_POWERSOCKETS == 1  // Only when powersockets shall be used
            if (i == 0)  // example: for powerSocket and device with ID = 0
            {
                if (powerSocketStates[i] != onlineUsers[i])
                {
                    powerSocketStates[i] = onlineUsers[i];
                    SetSwitch(Steckdose1, "OFF");
                    Serial.println("Switched off\n");
                }
            }
            #endif
        }
    }

    /*
    HostsResponse hostsResponse = getHostStatusByIndex(10);
    if (hostsResponse.isValid)
    {
        Serial.print("Main: Hostname of Index No. 11: ");
        Serial.println(hostsResponse.hostName);
        volatile int dummy54376 = 1;   
    }
    else
    {
        Serial.println("Error reading HostName of specified index");
        volatile int dummy54377 = 1; 
    }
    */
    
    /*
    char hostName[100];
    if (GetHostNameByIndex(2, hostName, sizeof(hostName)))
    {
        //verboseStatus()
        Serial.print("Main: Hostname of Index No. 1: ");
        Serial.println(hostName);    
    }
    else
    {
        Serial.print("Error reading HostName of specified index");
    }
    */
  
	delay(5000);
}

// not used in this example
void serialEvent(){
  String inData;
  char inChar;
    while(Serial.available() > 0) {
        inChar = Serial.read();
        if(inChar != '\n' && inChar != '\r') {
            inData += inChar;
        }
    }
    if(inData == "on") {
        SetSwitch(Steckdose1, "ON");
    }else if(inData == "off") {
        SetSwitch(Steckdose1, "OFF");
    }else if(inData == "toggle") {
        SetSwitch(Steckdose1, "TOGGLE");
    }else if(inData == "info") {
      GetDeviceInfo(Steckdose1);
    }else if(inData == "ring") {
     // WahlRundruf();  
    }
    Serial.println(inData);
}
 
void SetSwitch(String AIN, String state) {
    ensureWIFIConnection();
    String paramsb[][2] = {{"NewAIN", AIN},{"NewSwitchState", state}};
    connection.action("urn:dslforum-org:service:X_AVM-DE_Homeauto:1", "SetSwitch", paramsb, 2);
}
 
void GetDeviceInfo(String AIN) {
    ensureWIFIConnection();
    String paramsb[][2] = {{"NewAIN", AIN}};
    String reqb[][2] = {{"NewMultimeterPower", ""}, {"NewTemperatureCelsius", ""}};
    connection.action("urn:dslforum-org:service:X_AVM-DE_Homeauto:1", "GetSpecificDeviceInfos", paramsb, 1, reqb, 2);
    float power = reqb[0][1].toInt() / 100.0;
    float temp = reqb[1][1].toInt() / 10.0;
    Serial.print("Stromverbrauch: ");
    Serial.print(power, 1);
    Serial.println("W");
    Serial.print("Temperatur: ");
    Serial.print(temp, 1);
    Serial.println("*C");
}

/**
 * Prints the status of the device on the screen (arrays r of the getStatusOfMAC methods).
 * return nothing
 */
void verboseStatus(String r[4][2]) {
	for (int i=0;i<4;++i) {
		if(Serial) Serial.print(r[i][0]+"="+r[i][1]+", ");
	}
	if(Serial) Serial.print("\n");
}

/** 
 *  Get the number of devices that were connected to the WIFI lastly
 *  (some of them might not be online anymore, you need to check them individually!)
 *  return (int)
 */
int getWifiNumber() {
	String params[][2] = {{}};
	String req[][2] = {{"NewTotalAssociations", ""}};
	connection.action("WLANConfiguration:1", "GetTotalAssociations", params, 0, req, 1);
	int numDev = (req[0][1]).toInt();
	return numDev;
}

/** Print the status of all devices that were connected to the WIFI lastly
*  (some of them might not be online anymore, also gets you the hostnames and macs)
*  return nothing as of yet
 */
void getStatusOfAllWifi() {
  	getStatusOfAllWifi(getWifiNumber());
}


/** 
 *  Print the status of all devices that were connected to the WIFI lastly
 * (some of them might not be online anymore, also gets you the hostnames and macs)
 * return nothing as of yet
 */
void getStatusOfAllWifi(int numDev) {
	//Query the mac and status of each device
	for (int i=0;i<numDev;++i) {
		String params[][2] = {{"NewAssociatedDeviceIndex", String(i)}};
		String req[][2] = {{"NewAssociatedDeviceAuthState", ""}, {"NewAssociatedDeviceMACAddress", ""}, {"NewAssociatedDeviceIPAddress", ""}};
		if(connection.action("WLANConfiguration:1", "GetGenericAssociatedDeviceInfo", params, 1, req, 2)){
		if(Serial) {
			Serial.printf("%d:\t", i);
			Serial.println((req[1][1])+" is online "+(req[0][1]));
			Serial.flush();
		}
		}else{
			if(Serial) {
        Serial.printf("Fehler");        
				Serial.flush();
			}
		}
	}
}

/** 
 *  Get the status of one very specific device. May contain less information as the same option for WIFI.
 * return nothing, but fills the array r
 */
void getStatusOfMAC(String mac, String (&r)[4][2]) {
	//Ask for one specific device
	String params[][2] = {{"NewMACAddress", mac}};
	String req[][2] = {{"NewIPAddress", ""}, {"NewActive", ""}, {"NewHostName", ""}};
	// RoSchmi
    if(connection.action("Hosts:1", "GetSpecificHostEntry", params, 1, req, 3))
    //if(connection.action("Hosts:1", "GetSpecificHostEntry", params, 1, req, 2))
    {
		if(Serial) {
			Serial.println(mac + " is actually online " + (req[1][1]));
			Serial.flush();
		}
		r[STATUS_MAC_INDEX][0] = STATUS_MAC;
		r[STATUS_MAC_INDEX][1] = mac;
		r[STATUS_IP_INDEX][0] = STATUS_IP;
		r[STATUS_IP_INDEX][1] = req[0][1];
		r[STATUS_HOSTNAME_INDEX][0] = STATUS_HOSTNAME;
		r[STATUS_HOSTNAME_INDEX][1] = req[2][1];
        // RoSchmi
        String theHostname = r[STATUS_HOSTNAME_INDEX][1];
		r[STATUS_ACTIVE_INDEX][0] = STATUS_ACTIVE;
		r[STATUS_ACTIVE_INDEX][1] = req[1][1];
        // RoSchmi
        String theActiveState = r[STATUS_ACTIVE_INDEX][1];
        
	}else{
		if(Serial) {
		Serial.println(mac + " Fehler");
		Serial.flush();
		}
	}
}

/** 
 *  Get the number of devices that were connected to the router (ever)
 *  (some of them might not be online, you need to check them individually!)
 *  return (int)
 */
int getDeviceNumber() {
	String params[][2] = {{}};
	String req[][2] = {{"NewHostNumberOfEntries", ""}};
	connection.action("Hosts:1", "GetHostNumberOfEntries", params, 0, req, 1);
	int numDev = (req[0][1]).toInt();
	return numDev;
}

HostsResponse getHostStatusByMACAddress(const String mac)
{
    HostsResponse hostsResponse;
    const char emptyStr[2] {0};

    hostsResponse.isValid = false;
    strcpy(hostsResponse.active, emptyStr);
    strcpy(hostsResponse.hostName, emptyStr);
    strcpy(hostsResponse.index, emptyStr);
    strcpy(hostsResponse.ipAddress, emptyStr);
    strcpy(hostsResponse.macAddress, emptyStr);
    
    //Ask for one specific device
    const String service = "urn:dslforum-org:service:Hosts:1";
    
	String call_params[][2] = {{"NewMACAddress", mac}};
	String reqb[][2] = {{"NewIPAddress", ""}, {"NewActive", ""}, {"NewHostName", ""}, {"NewIndex", ""}};
	// RoSchmi
    if(connection.action(service, "GetSpecificHostEntry", call_params, 1, reqb, 4))   
    { 
        strncpy(hostsResponse.macAddress, mac.c_str(), sizeof(hostsResponse.macAddress) -1);
        strncpy(hostsResponse.ipAddress, reqb[0][1].c_str(), sizeof(hostsResponse.ipAddress) - 1);
        strncpy(hostsResponse.active, reqb[1][1].c_str(), sizeof(hostsResponse.active) - 1);
        strncpy(hostsResponse.hostName, reqb[2][1].c_str(), sizeof(hostsResponse.hostName) - 1);    
        strncpy(hostsResponse.index, reqb[3][1].c_str(), sizeof(hostsResponse.index) -1);
        hostsResponse.isValid = true;
        volatile int dummy2345 = 1;
        dummy2345 = dummy2345 + 1;
    }
    return hostsResponse;
}

HostsResponse getHostStatusByIndex(int index)
{
    HostsResponse hostsResponse;
    hostsResponse.isValid = false;
    if (index < 0 || index >= 100000)
    {
        return hostsResponse;
    }
    char idString[7] {0};
    itoa(index, idString, 10);
    String service = "urn:dslforum-org:service:Hosts:1";
    String call_params[][2] = {{"NewIndex", idString}};
    String reqb[][2] = {{"NewIPAddress", ""}, {"NewActive", ""}, {"NewHostName", ""}, {"NewMACAddress", ""}};
    if (connection.action(service, "GetGenericHostEntry", call_params, 1, reqb, 4))
    {
        strncpy(hostsResponse.index, idString, sizeof(hostsResponse.index) -1); 
        strncpy(hostsResponse.ipAddress, reqb[0][1].c_str(), sizeof(hostsResponse.ipAddress) - 1);
        strncpy(hostsResponse.active, reqb[1][1].c_str(), sizeof(hostsResponse.active) - 1);
        strncpy(hostsResponse.hostName, reqb[2][1].c_str(), sizeof(hostsResponse.hostName) - 1);  
        strncpy(hostsResponse.macAddress, reqb[3][1].c_str(), sizeof(hostsResponse.macAddress) -1);
        hostsResponse.isValid = true;
        volatile int dummy2345 = 1;
        dummy2345 = dummy2345 + 1;
    }
    return hostsResponse;
}

// index may be from 0 to 100000, outHostName is the result
bool GetHostNameByIndex(int index, char * outHostName, int maxHostNameLength)
{
    if (index < 0 || index >= 100000)
    {
        return false;
    } 
    char idString[7] {0};
    itoa(index, idString, 10);  
    String service = "urn:dslforum-org:service:Hosts:1";
    String call_params[][2] = {{"NewIndex", idString}};
    String reqb[][2] = {{"NewHostName", ""}};
    bool result = connection.action(service, "GetGenericHostEntry", call_params, 1, reqb, 1);
    strncpy(outHostName, reqb[0][1].c_str(), maxHostNameLength - 1);
    
    Serial.print("Hostname of Index No. 1: ");
    Serial.println(outHostName);
    return result;
}

  //  Rundruffunktion über TR064 an der Fritzbox auslösen
void WahlRundruf() {
      String service = "urn:dslforum-org:service:X_VoIP:1";

  // Die Telefonnummer **9 ist der Fritzbox-Rundruf.
  String call_params[][2] = {{"NewX_AVM-DE_PhoneNumber", "**9"}};
  connection.action(service, "X_AVM-DE_DialNumber", call_params, 1);

  // Warte vier Sekunden bis zum auflegen
  delay(4000);
  connection.action(service, "X_AVM-DE_DialHangup");
  
}
 
void ensureWIFIConnection() {
    if((wiFiMulti.run() != WL_CONNECTED)) {
        wiFiMulti.addAP(wifi_ssid, wifi_password);
        while ((wiFiMulti.run() != WL_CONNECTED)) {
            delay(100);
        }
    }
}