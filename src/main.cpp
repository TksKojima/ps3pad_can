#include <Arduino.h>

#include <ps3pad.h>
#include <CAN_app.h>
#include "pcb_setting.h"


void setPs3padToCan_loop(){

// 0,1,2,3 -> Up Left Right Down
// 0,1,2,3 -> Tri, Square, O, X
// 0,1,2,3 -> L1,R1, L2, R2
// 0,1,2   -> start, select, ps

// CAN Message struct 
//  data0: analogLX, 
//  data1: analogLY, 
//  data2: analogRX, 
//  data3: analogRY
//  data4 [0,1,2,3,4,5,6,7]:[up, left, right, down, Tri, square, o, x]
//  data5 [0,1,2,3,4,5,6,7]:[L1, R1, L2, R2, start, select, ps3btn]

  int ps3input0_3[4];
  int ps3input4_7[4];

  unsigned char candata[8];
  unsigned char can1byte=0;

  getButtonStateArr_analogStick( ps3input0_3 );    //
  for( int i=0; i<4; i++ ){
    candata[i] = ps3input0_3[i] + 128;
  }

  for( int i=0; i<4; i++){ 
    ps3input0_3[i] = 0; 
    ps3input4_7[i] = 0; 
  }

  getButtonStateArr_crossKey(    ps3input0_3 );   //
  getButtonStateArr_shapeButton( ps3input4_7 ); //
  can1byte = 0;
  for( int i=0; i<4; i++ ){
    can1byte |= ( ps3input0_3[i] <<  i );
    can1byte |= ( ps3input4_7[i] << (i+4) );
  } 
  candata[4] = can1byte;

  for( int i=0; i<4; i++){ 
    ps3input0_3[i] = 0; 
    ps3input4_7[i] = 0; 
  }

  getButtonStateArr_shoulderButton( ps3input0_3 ); //
  getButtonStateArr_optionButton(  ps3input4_7 );   //
  can1byte = 0;
  for( int i=0; i<4; i++ ){
    can1byte |= ( ps3input0_3[i] <<  i );
    can1byte |= ( ps3input4_7[i] << (i+4) );
  } 
  candata[5] = can1byte;

  canTxbuf_set( 0x600, 6, 50, candata, 1 );

}


void setup(void) {

  Serial.begin(115200);

    Serial.println("[ Start ]");
  while (!Serial);
  pcb_init();
  can_init();
  ps3pad_init();

  Serial.println("");
  Serial.println("[ SetUp End ]");
}


void loop(void) {
  static unsigned long curr_prev=0;
  unsigned long curr = millis(); // 現在時刻を更新

  if( curr - curr_prev < 5 ){
      return;
  }
  curr_prev = curr;
  ps3pad_loop();
  can_setTestFlag( digitalRead(DIPSW_PIN3) , digitalRead(DIPSW_PIN4) );
  can_loop();
  setPs3padToCan_loop();
  
}




