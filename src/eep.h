#ifndef EPP_H
#define EPP_H

#include <Arduino.h>
#include <EEPROM.h>

#define EEP_SIZE_ALL 512

#define EEP_SIZE_ID     8
#define EEP_SIZE_CYCLE  8


#define EEP_ADDR_ID    0
#define EEP_ADDR_CYCLE  (EEP_ADDR_ID   + EEP_SIZE_ID)

void eep_init();
void eep_loop();

void eep_write();
void eep_write_num( int addrOffset, int num );
void eep_write_str( int addrOffset, const char* str, int size );

void eep_read_wifi_setting( int* _id, int* _mode, int _ip[], int _gateway[], int _subnet[],  char* _ssid, char* pass, int charsize  );
int  eep_read_num( int addrOffset );
void eep_read_str( int addrOffset, char* str, int size );


// void eep_write_mode( int mode );
// void eep_write_espid( int id );
// void eep_write_pass();
// void eep_write_ssid();

// void eep_read_mode();
// void eep_read_espid();
// void eep_read_pass();
// void eep_read_ssid();



#endif
