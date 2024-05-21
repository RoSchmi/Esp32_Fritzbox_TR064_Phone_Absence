# Esp32_Fritzbox_TR064_Phone_Absence

This App shows some example functions how a Fritzbox can be controlled from an Esp32 Mcu via TR-064 protocol.
This App checks if mobile phones are present or absent
in the Fritzbox WLAN range and switches a DECT!200 power
socket to reflect the present/absent state.
Additionally some more outcommened functions are show, e.g. reading the power consumption on the DECT!200 power socket.

Fritzbox is a Router/DECT phone combination, which can be controlled via TR-064 protocol (http and https)

The application uses a modification of the following TR-064 library:

https://github.com/RoSchmi/ESP32_TR064_SOAP_Library

The modification allows the use of http and https transmission to access the Fritzbox




