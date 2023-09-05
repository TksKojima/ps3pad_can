#include <Arduino.h>

#include <ps3pad.h>
#include <eep.h>
#include <CAN_appLite.h>
#include "pcb_setting.h"
#include "wifi_ps3padcan.h"

// PS3パッド入力 → CAN出力
// 自前PCBボード前提(使用しているのはCANとDIPスイッチ)
// DIPSW2(2番目)がOnだとWifiを発信（SSIDとMDNS名＝esp32ps3 ）、CAN IDと送信周期を設定可能

// 値はデフォルト値でEEPROMの保存値やWifi経由の設定で上書きされる
int can_id    = 0x600; 
int can_cycle = 50;
int can_para[2];

// CAN IDと周期設定のためのwifi機能が不要のときに止めるためのフラグ
int use_wifi = 1;


void main_getParaFromEep(){
  int temp_id    = eep_read_num( EEP_ADDR_ID );
  int temp_cycle = eep_read_num( EEP_ADDR_CYCLE );

  if( 0x00 < temp_id && temp_id < 0x800 ){
    can_id = temp_id;
  }
  if( 5 <= temp_cycle && temp_cycle < 5000 ){
    can_cycle = temp_cycle;
  }

  can_para[0] = can_id;
  can_para[1] = can_cycle;
  Serial.print( "can_id: " );
  Serial.println( can_id );
  Serial.print( "can_cycle: " );
  Serial.println( can_cycle );


}

void main_setPs3padToCan_loop(){

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

  canTxbuf_set( can_id, 6, can_cycle, candata, 1 );

}


void setup(void) {

  Serial.begin(115200);

    Serial.println("[ Start ]");
  while (!Serial);


  pcb_init();
  // DIP SWの2番目(1-4)がONのときにCAN IDと周期を設定するwifiを機能させる
  if(  digitalRead(DIPSW_PIN2) == HIGH ){ use_wifi = 1; }
  else{ use_wifi = 0; }

  eep_init();
  can_init();
  ps3pad_init();  

  main_getParaFromEep();

  if( use_wifi ){
    wifi_setup( " " + ps3_getBtMacAddr() );
    wifi_setPara_int( can_para );
  }

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
  main_setPs3padToCan_loop();

  if( use_wifi ){
    wifi_loop();
  }

  
}




