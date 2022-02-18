# Esp32_Fritzbox_TR064_Phone_Absence

This App is intended to check if mobile phones are present or absent
in the Fritzbox WLAN range and to switch a DECT!200 power
socket to reflect the present/absent state

This is still work in progress, not yet intended to be useful for others

Use ESP32 to control a Fritzbox (Router/DECT phone combination) via TR-064 protocol (http and https)

The application uses a modification of the following TR-064 library:

https://github.com/Aypac/Arduino-TR-064-SOAP-Library

The modification allows the use of https transmission to access the Fritzbox




