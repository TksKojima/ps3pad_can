#ifndef _H_WIFIPS3CAN_
#define _H_WIFIPS3CAN_

#include <Arduino.h>

#include <WiFi.h>
//#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <eep.h>

//#include <CAN_app.h>

#define  MODE_AP_ONLY    (0)
#define  MODE_STA_ONLY   (1)
#define  MODE_STA_TO_AP  (2)

void wifi_setup( String btMac );
int wifi_setup_input(int device_id, int connect_mode,  
  int in_ip[],   int in_gw[], int in_subnet[],  char* in_ssid,  char* in_password  );

void server_setup();
void wifi_loop();
void wifi_setPara_str( String _para[] );
void wifi_setPara_int( int  _para[] );
void handleRoot();
void handleSetPara();
void handleGet();
void handlePost();
void handleNotFound();

#endif




