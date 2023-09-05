#include <eep.h>


void eep_init(){
    EEPROM.begin(EEP_SIZE_ALL); // EEPROMのサイズを指定
    int value = EEPROM.read(0); // アドレス0から値を読み取る
    //Serial.println(value);
    if (value == 0xFF) { // EEPROMが初期化されていない場合は、初期化する
        Serial.println("Initializing EEPROM...");
        for (int i = 0; i < EEP_SIZE_ALL; i++) {
          EEPROM.write(i, 0);
        }
        EEPROM.commit(); // EEPROMの変更を保存
    }

    //eep_write();
}

void eep_loop(){

}

void eep_write_num( int addrOffset, int num ){
  //EEPROM.write(addrOffset, num);
  EEPROM.put(addrOffset, num);
  EEPROM.commit();
}

void eep_write_str( int addrOffset, const char* str, int size ){
  for (int i = 0; i < size; i++) {
    EEPROM.write(addrOffset + i, str[i]);
  }
  EEPROM.commit();
}


void eep_write( int para[] ){
  Serial.println( "[EEP Write]" );
  
  Serial.println( para[0] );    
  eep_write_num( EEP_ADDR_ID,  para[0] );

  Serial.println( para[1] );
  eep_write_num( EEP_ADDR_CYCLE, para[0] );

}

void eep_read_wifi_setting( int para[] ){
  
    Serial.println("");
    Serial.println( "[ EEP Read ]" );
    Serial.print( "CAN ID: " );
    *(para+0) = eep_read_num( EEP_ADDR_ID );
    Serial.println( *(para+0) );

    Serial.print( "Tx cycle time: " );
    *(para+1) =  eep_read_num( EEP_ADDR_CYCLE );
    Serial.println( *(para+1) );

}

int eep_read_num( int addrOffset ){
  int retval;
  EEPROM.get(  addrOffset, retval );
  return  retval;

}
void eep_read_str(int addrOffset, char* str, int size) {
  for (int i = 0; i < size; i++) {
    str[i] = EEPROM.read(addrOffset + i);
  }
}


// void eep_write_wifimode( int mode ){
// }
// void eep_write_espid(){}
// void eep_write_pass(){}
// void eep_write_ssid(){}

// void eep_read(){}
// void eep_read_wifimode(){}
// void eep_read_espid(){}
// void eep_read_pass(){}
// void eep_read_ssid(){}

