#include <Arduino.h>

#ifndef _CONFIG_H
#define _CONFIG_H

// This file is for 'not secret' user specific configurations
//
// Please select the transport protocol, http or https with ot without rootCa validation
// For full https you must include the Root Certificate of your FritzBox

// Define other settings according to your needs

// The credentials of your WiFi router have to be set in the file config_secret.h

// Different debug level, don't change the following 5 lines

#define _DEBUG_NONE 0      //< Print no debug messages whatsoever
#define _DEBUG_ERROR 1       //< Only print error messages
#define _DEBUG_WARNING 2     //< Only print error and warning messages
#define _DEBUG_INFO 3        //< Print error, warning and info messages
#define _DEBUG_VERBOSE 4     //< Print all messages

// Choose used debug level here
#define SELECTED_DEBUG_LEVEL   _DEBUG_ERROR
//#define SELECTED_DEBUG_LEVEL   _DEBUG_VERBOSE

// Set transport protocol here: (httpsInsec and https only working on Esp32)
// http (0) means: normal http via port 49000
// httpsInsec (1) means: https via port 49443 without rootCa validation
// https (2) means: https via port 49443 with rootCa validation

#define TRANSPORT_PROTOCOL 0        // 0 = http, 1 = httpsInsec, 2 = https
//#define TRANSPORT_PROTOCOL 1      // 0 = http, 1 = httpsInsec, 2 = https
//#define TRANSPORT_PROTOCOL 2      // 0 = http, 1 = httpsInsec, 2 = https
                                    // should be 1 or 2 for normal operation and 0 for testing
// Not yet implemented                                  
#define WORK_WITH_WATCHDOG 0        // 1 = yes, 0 = no, Watchdog is used (1) or not used (0)
                                    // should be 1 for normal operation and 0 for testing
// not tested if it works
#define USE_STATIC_IP 0                 // 1 = use static IpAddress, 0 = use DHCP
                                        // for static IP: Ip-addresses have to be set in the code
// Zertifikat von FritzBox herunterladen

// Klicken Sie in der Benutzeroberfläche der FRITZ!Box auf "Internet".
// Klicken Sie im Menü "Internet" auf "Freigaben".
// Klicken Sie auf die Registerkarte "FRITZ!Box-Dienste".
// Klicken Sie auf "Zertifikat herunterladen" und speichern Sie die Datei mit dem Zertifikat auf Ihrem Computer.
// Open the certificate with 'Editor' and add double quotes as can be seen here
// Each FritzBox can have its specific certificate

/*
const char *myfritzbox_root_ca =
"-----BEGIN CERTIFICATE-----\n"
"MIID2DCCAsCgAwIBAgIJAOReByhZW+7gMA0GCSqGSIb3DQEBCwUAMCcxJTAjBgNV\n"
"BAMTHGhzbHk5eHh3ODd2bWt5YncubXlmcml0ei5uZXQwHhcNMTkxMDI4MTAyMzE1\n"
"WhcNMzgwMTE1MTAyMzE1WjAnMSUwIwYDVQQDExxoc2x5OXh4dzg3dm1reWJ3Lm15\n"
"ZnJpdHoubmV0MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuCbT+qAv\n"
"scEcYZco6Gl9SHzinr3VCLsTCkibcuGt/FdsLLCcdGHfLb2NX9mMBF+BwYRoFuXt\n"
"HVx6O5FrlvIHDECw+uQ+vfzFMbpm6b1lvgce/8rll/NgCTirwDW6wS9iy7CtAlSm\n"
"+nqZdoVqZvcWInrckn7n7p/ZdCM2U2hQ1cNZkJtxXvc/aKL9Lutj28J0J6XxJDFC\n"
"viPKENz6+fd+B5dwhmbfRPABgPTS/mqb2vCzwNNtvhnkvPRskqc6QSckdm3/HIop\n"
"iKQP/Ao1lnB4V/tDOf7VhTvVok6pA1D2ccJA/HNAYCvw9/fFoKtAxbnVFXI0+Bls\n"
"UifYdnCcUkqtDwIDAQABo4IBBTCCAQEwHQYDVR0OBBYEFMnzFscyTefQr+dtqMmW\n"
"vAWi/GfxMFcGA1UdIwRQME6AFMnzFscyTefQr+dtqMmWvAWi/GfxoSukKTAnMSUw\n"
"IwYDVQQDExxoc2x5OXh4dzg3dm1reWJ3Lm15ZnJpdHoubmV0ggkA5F4HKFlb7uAw\n"
"DAYDVR0TBAUwAwEB/zB5BgNVHREEcjBwghxoc2x5OXh4dzg3dm1reWJ3Lm15ZnJp\n"
"dHoubmV0gglmcml0ei5ib3iCDXd3dy5mcml0ei5ib3iCC215ZnJpdHouYm94gg93\n"
"d3cubXlmcml0ei5ib3iCCWZyaXR6Lm5hc4INd3d3LmZyaXR6Lm5hczANBgkqhkiG\n"
"9w0BAQsFAAOCAQEAk8nOxqt1SVEK+N9hT3whwt94shwepQFi0k+oBt3QUpm8Z1OV\n"
"ipQ4ERUSicVGnHTEBXzxbUaEXuyTAYmaKBnyErR6GjNmp5YNPvlIPBJVku/p8412\n"
"Y2Thn3YLhqpPG4HIkhD0E+tIh98WZbgwtQc7horRPqkaIBaBdRzi0pHJplRQeXPM\n"
"knj/XioZvpnd3eMsocHBaAOOjzAOToVjFz9yS4woGaVVYFqYnj6KeJ0JOT9aehjv\n"
"+Zr7KKh3XDhhBF43/TncYKPqm5uOLHlITivzQ8BTH0pPUujQwa0j+szGftuBjjHw\n"
"xMX1RtE24A1Pi28qtRu/DbA1nbsj4gy4ymh4vA==\n"
"-----END CERTIFICATE-----";
*/

//Certificate No 2

const char *myfritzbox_root_ca =
"-----BEGIN CERTIFICATE-----\n"
"MIID+TCCAuGgAwIBAgIUBMTzm8qN19UC3UJmmH9QJySC16wwDQYJKoZIhvcNAQEL\n"
"BQAwJzElMCMGA1UEAxMcenFudXphZWl4d203dmtzaC5teWZyaXR6Lm5ldDAeFw0y\n"
"MzEwMDkyMzM1MTFaFw0zODAxMTUyMzM1MTFaMCcxJTAjBgNVBAMTHHpxbnV6YWVp\n"
"eHdtN3Zrc2gubXlmcml0ei5uZXQwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK\n"
"AoIBAQDYFzxxDFcK8DWr3wCJBZgEtIxAYoQCQC5HyFSczwwoD9x/wWg0hSMUat1/\n"
"5yVO1RGjhJmUDHEl0AXma3Kv4f+EpRDwVgVie8r6mf2AvhSGOyAYCaY750hRA7eP\n"
"7vNBJ2D5F7w04syLfTAIb4UiDZfoKWRQcKylwvEpP8q7VZyQp4jOB9iLCG1UeUSU\n"
"kgi9+0idSYhpA+ACaKeGZZnut3RTxVTC3oeAMa2vd+Tj7gpzc4OEedF5bPIfhXI2\n"
"M2lu0/p7/qHhBe7NqEvTNaArCpPmPz4089QZ/TI+4vTkGxDDwfT5dc2vVdZqB901\n"
"sSJQ0C9wBUOdhnn9qARQ0z3cepIzAgMBAAGjggEbMIIBFzAdBgNVHQ4EFgQUAmey\n"
"fi8cXhiuVOaICrt320D1PoswYgYDVR0jBFswWYAUAmeyfi8cXhiuVOaICrt320D1\n"
"PouhK6QpMCcxJTAjBgNVBAMTHHpxbnV6YWVpeHdtN3Zrc2gubXlmcml0ei5uZXSC\n"
"FATE85vKjdfVAt1CZph/UCckgtesMAwGA1UdEwQFMAMBAf8wgYMGA1UdEQR8MHqC\n"
"HHpxbnV6YWVpeHdtN3Zrc2gubXlmcml0ei5uZXSCCWZyaXR6LmJveIINd3d3LmZy\n"
"aXR6LmJveIILbXlmcml0ei5ib3iCD3d3dy5teWZyaXR6LmJveIIIUm9CbzY2OTCC\n"
"CWZyaXR6Lm5hc4INd3d3LmZyaXR6Lm5hczANBgkqhkiG9w0BAQsFAAOCAQEAwNNN\n"
"QRkzgcSUY7HcbPFa1rNLtebLNkpe5NJZCkduU3CUyAr0pUK6ZAtVGevippzcDGlD\n"
"cZqic6bJ4/uSj4TJFWR/feM0wnCxbJzpisdc12aC63mKIyWT+xtHc1qDcsJPdzdG\n"
"YL7T49LZKg72OcDBgpKbS2/RXD/G45mxPINPpR/v3ZsTQAgrXt/eFjeXEX/zd+nF\n"
"AjU42O8LhwiE2Xw8hlVXcJyBcqdzX5QIOi5mEQy/CeS7L+OrfBNQmkbb6OLPZj3l\n"
"QOy5U5xMPjUbuDmHuCRLfFKfDT/GpQeG9/AJNbKdp+J/1UUOusT64ImIgsuwjIw3\n"
"P19RdeerE9mdgOy0VQ==\n"
"-----END CERTIFICATE-----";


#endif // _CONFIG_H