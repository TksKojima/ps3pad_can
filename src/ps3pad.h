#ifndef PS3PAD_H
#define PS3PAD_H

#include <Ps3Controller.h>


#define PS3_KEY_UP    (0)
#define PS3_KEY_LEFT  (1)
#define PS3_KEY_RIGHT (2)
#define PS3_KEY_DOWN  (3)

#define PS3_BTN_TRI  (0)
#define PS3_BTN_SQR  (1)
#define PS3_BTN_CIR  (2)
#define PS3_BTN_CRS  (3)

#define PS3_BTN_L1  (0)
#define PS3_BTN_R1  (1)
#define PS3_BTN_L2  (2)
#define PS3_BTN_R2  (3)

#define PS3_BTN_STA  (0)
#define PS3_BTN_SEL  (1)
#define PS3_BTN_PS   (2)

void ps3pad_init();
void ps3pad_loop();

void setup_bt_mac_addres();
int  analog2val( int analogval );
void ps3_notify();
void ps3_onConnect();
void ps3_serialMonitor();

int setButtonDown( int* buttonVal, int num );
int setButtonUp( int* buttonVal, int num );
int getButtonState( int buttonVal, int num );

// ボタンのONOFFを配列で取得　
// int buttonArr[4];
// getButtonStateArr_shapeButton( buttonArr );
// みたいな使い方　
void getButtonStateArr_shapeButton( int *buttonValArr );
void getButtonStateArr_shoulderButton( int *buttonValArr );
void getButtonStateArr_optionButton( int *buttonValArr );
void getButtonStateArr_crossKey( int *buttonValArr );
void getButtonStateArr_analogStick( int *buttonValArr );


void proc_SelectButton();



#endif

