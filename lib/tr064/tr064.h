// Modification of tr064.h by RoSchmi, 2021
//
// adapted from:

/*!
 * @file tr064.h
 * 
 * Library for communicating via TR-064 protocol (e.g. Fritz!Box)
 * This library allows for easy communication of TR-064 (and possibly TR-069) enabled devices,
 * such as Routers, smartplugs, DECT telephones etc.
 * Details, examples and the latest Version of this library can be found <a href='https://github.com/Aypac/Arduino-TR-064-SOAP-Library'>on my Github page</a>.
 * A descriptor of the protocol can be found <a href='https://avm.de/fileadmin/user_upload/Global/Service/Schnittstellen/AVM_TR-064_first_steps.pdf'>here</a>.
 * 
 * This library depends on:
 *    MD5Builder
 *  ESP8266HTTPClient or HTTPClient, depending on the intended platform (ESP8266 or ESP32).
 *
 * Written by René Vollmer "Aypac" in November 2016.
 *
 * MIT License, all text here must be included in any redistribution.
 *
 */

#ifndef tr064_h
#define tr064_h

#include "Arduino.h"
#include <MD5Builder.h>

#if defined(ESP8266)
    //if(Serial) Serial.println(F("Version compiled for ESP8266."));
    #include <ESP8266HTTPClient.h>
#elif defined(ESP32)
    //if(Serial) Serial.println(F("Version compiled for ESP32."));
    #include <HTTPClient.h>
#else
    //INCOMPATIBLE!
#endif

/// HTTP codes see RFC7231



enum tr064_codes{          
            TR064_CODE_UNKNOWNACTION = 401, ////If an unknown action is used the returned code is 401. This return code is used for obsoleted actions, too. 
            TR064_CODE_FALSEARGUMENTS = 402, //If a user is not authenticated, 401 (“Unauthorized”) will be returned.
            TR064_CODE_AUTHFAILED = 503, //If a user is not authenticated, 401 (“Unauthorized”) will be returned.
            TR064_CODE_ARGUMENTVALUEINVALIDE = 600, //  Argument Value Invalid
            TR064_CODE_ACTIONNOTAUTHORIZED = 606, //If a user is authenticated but has not the needed rights, 606 (“Action not authorized”) will be returned
            TR064_CODE_ARRAYINDEXINVALID = 713, //SpecifiedArrayIndexInvalid
            TR064_CODE_NOSUCHENTRY = 714, //No such array entry in array
            TR064_CODE_INTERNALERROR = 820, // Internal Error
            TR064_CODE_SECONDFACTORAUTHREQUIRED = 866, //If an action needs 2FA, the status code 866 (“second factor authentication required”)
            TR064_CODE_SECONDFACTORAUTHBLOCKED = 867, // (“second factor authentication blocked”) or 
            TR064_CODE_SECONDFACTORAUTHBUSY = 868, // (“second factorauthentication busy”) will be returned and the 2FA procedure    
        } ;

#define arr_len( x )  ( sizeof( x ) / sizeof( *x ) ) ///< Gives the length of an array

// Possible values for client.state()
#define TR064_NO_SERVICES           -1 ///< No Service actions will not execute
#define TR064_SERVICES_LOADED       0 ///< Service loaded

typedef enum {
      useHttp,
      useHttps
  } Protocol;

typedef const char* X509Certificate;


/**************************************************************************/
/*! 
    @brief Class to easily make TR-064 calls. This is the main class
             of this library. An object of this class keeps track of
             
*/
/**************************************************************************/

class TR064 {
 
    
    public:
        /*!  Different debug level
         *   DEBUG_NONE         ///< Print no debug messages whatsoever
         *   DEBUG_ERROR        ///< Only print error messages
         *   DEBUG_WARNING      ///< Only print error and warning messages
         *   DEBUG_INFO         ///< Print error, warning and info messages
         *   DEBUG_VERBOSE      ///< Print all messages
         */
        enum LoggingLevels {DEBUG_NONE, DEBUG_ERROR, DEBUG_WARNING, DEBUG_INFO, DEBUG_VERBOSE}; 
        
        // Overloaded constructor to use either WiFiClient or WiFiClientSecure 
        TR064(int port, String ip, String user, String pass, Protocol protocol, WiFiClient pClient, HTTPClient * httpClient, X509Certificate pCertificate);
        TR064(int port, String ip, String user, String pass, Protocol protocol, WiFiClientSecure pClient, HTTPClient * httpClient, X509Certificate pCertificate);
        void init();
        void initNonce();
        bool action(const String& service, const String& act, String params[][2] = {}, int nParam = 0,const String& url = "");
        bool action(const String& service, const String& act, String params[][2], int nParam, String (*req)[2], int nReq, const String& url = "");
        bool xmlTakeParam(String (*params)[2], int nParam, Protocol takeProtocol = Protocol::useHttp);       
        bool xmlTakeParam(String& value, const String& needParam, Protocol takeProtocol = Protocol::useHttp);
         
        String xmlTakeInsensitiveParam(String inStr, String needParam);
        String xmlTakeSensitiveParam(String inStr, String needParam);
        String cleanOldServiceName(const String& service);
        String errorToString(int error);
        String md5String(String s);
        String byte2hex(byte number);
        int debug_level; ///< Available levels are `DEBUG_NONE`, `DEBUG_ERROR`, `DEBUG_WARNING`, `DEBUG_INFO`, and `DEBUG_VERBOSE`.
        
    private:
        //TODO: More consistent naming.
        void initServiceURLs();
        void deb_print(String message, int level);
        void deb_println(String message, int level);
        bool action_raw(const String& service,const String& act, String params[][2], int nParam, const String& url = "");       
        // RoSchmi: Addidtional parameter to inject protocol (http or https) 
        bool httpRequest(const String& url, const String& xml, const String& action, Protocol protocol = Protocol::useHttp);
        // RoSchmi: Addidtional parameter to inject protocol (http or https) 
        bool httpRequest(const String& url, const String& xml, const String& action, bool retry, Protocol protocol = Protocol::useHttp);
        
        String generateAuthToken();
        String generateAuthXML();
        String findServiceURL(String service);
        String _xmlTakeParam(String inStr, String needParam);

        int _state;
        String _ip;
        int _port;
        String _user;
        String _pass;
        String _realm; //To be requested from the router
        String _secretH; //to be generated
        String _nonce = "";
        String _status;
        const String _requestStart = "<?xml version=\"1.0\"?><s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">";
        const String _detectPage = "/tr64desc.xml";
        const char* const _servicePrefix = "urn:dslforum-org:service:";
        
        X509Certificate _certificate; 
        WiFiClient _client;
        WiFiClientSecure _sslClient;

        WiFiClient * _streamClient;

        Protocol _protocol;
        
        HTTPClient * _instHttp;

        /* TODO: We should give access to this data for users to inspect the
        * possibilities of their device(s) - see #9 on Github.
        TODO: Remove 100 services limits here
        */
        String _services[100][2];
};

    #include "tr064_Impl.h"

#endif