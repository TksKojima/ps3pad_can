#include <ps3pad.h>

// ps3 pad
int ps3_battery = 0;
int ctrlMode = 0;

int ps3_analogLX=0;
int ps3_analogLY=0;
int ps3_analogRX=0;
int ps3_analogRY=0;

float ps3_analogLX_std=0;
float ps3_analogLY_std=0;
float ps3_analogRX_std=0;
float ps3_analogRY_std=0;

// buttonVal 
int ps3_crossKey = 0;       // 0,1,2,3 -> Up Left Right Down
int ps3_shapeButton = 0;    // 0,1,2,3 -> Tri, Square, O, X
int ps3_shoulderButton = 0; // 0,1,2,3 -> L1,R1, L2, R2
int ps3_optionButton = 0;   // 0,1,2     -> start, select, PS
const int ps3_crossKey_size = 4;
const int ps3_shapeButton_size = 4;
const int ps3_shoulderButton_size = 4;
const int ps3_optionButton_size = 3;


void ps3pad_init()
{
    Serial.begin(115200);
    setup_bt_mac_addres();

    Ps3.attach(ps3_notify);
    Ps3.attachOnConnect(ps3_onConnect);
    //Ps3.begin("F0:08:D1:D8:29:FE");
    Ps3.begin("24:6F:28:AA:A8:86");
    
    //Ps3.begin("C8:F0:9E:A2:4E:AE");
    Ps3.setPlayer(ctrlMode+1);
    
    Serial.println("Ready.");
}

void ps3pad_loop(){
  proc_SelectButton();

  //ps3_serialMonitor();

}

void setup_bt_mac_addres() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  uint8_t btmac[6];
  esp_read_mac(btmac, ESP_MAC_BT);
  Serial.println("");
  Serial.printf("[Bluetooth] Mac Address = %02X:%02X:%02X:%02X:%02X:%02X\r\n", btmac[0], btmac[1], btmac[2], btmac[3], btmac[4], btmac[5]);

}


int analog2val( int analogval ){
  int ret;

  ret = (analogval*100/127);
  if( ret >= 100 ){ ret = 100; }
  else if( ret <= -100 ){ ret = -100; }

  return ret;
}

int setButtonDown( int* buttonVal, int num ){ return *buttonVal |=  (1 << num); }
int setButtonUp(   int* buttonVal, int num ){ return *buttonVal &= ~(1 << num); }

int getButtonState( int buttonVal, int num ){ 
  return  ( (buttonVal &=  (1 << num)) >> num ) & 0x1; 
}

void getButtonStateArr( int buttonVal, int *buttonValArr, int size ){ 
  for( int i=0; i<size; i++  ){
    buttonValArr[i] = getButtonState( buttonVal, i );
  }
}


void getButtonStateArr_shapeButton( int *buttonValArr ){ // size = 4
  getButtonStateArr( ps3_shapeButton, buttonValArr, ps3_shapeButton_size );
}
void getButtonStateArr_shoulderButton( int *buttonValArr ){ // size = 4
  getButtonStateArr( ps3_shoulderButton, buttonValArr, ps3_shoulderButton_size );
}
void getButtonStateArr_optionButton( int *buttonValArr ){ // size = 3
  getButtonStateArr( ps3_optionButton, buttonValArr, ps3_optionButton_size );
}
void getButtonStateArr_crossKey( int *buttonValArr ){ // size = 4
  getButtonStateArr( ps3_crossKey, buttonValArr, ps3_crossKey_size );
}
void getButtonStateArr_analogStick( int *buttonValArr ){
  *(buttonValArr + 0) = ps3_analogLX;
  *(buttonValArr + 1) = ps3_analogLY;
  *(buttonValArr + 2) = ps3_analogRX;
  *(buttonValArr + 3) = ps3_analogRY;

}


void proc_SelectButton(){
  int onoff = 0;
  static int prev_onoff = 0;
  int optbtn[3];
  getButtonStateArr_optionButton( optbtn );
  //onoff = getButtonState( ps3_optionButton, PS3_BTN_SEL );
  onoff = optbtn[ PS3_BTN_SEL ];

  if( prev_onoff == 0 && onoff == 1 ){
      ctrlMode += 1;
      if( ctrlMode >=4){
        ctrlMode=0;
      }
      Ps3.setPlayer(ctrlMode+1);
  }
  prev_onoff = onoff;

}

void ps3_notify()
{
    // //--- Digital cross/square/triangle/circle button events ---
    if( Ps3.event.button_down.triangle )  setButtonDown( &ps3_shapeButton, PS3_BTN_TRI );
    if( Ps3.event.button_down.square )    setButtonDown( &ps3_shapeButton, PS3_BTN_SQR );
    if( Ps3.event.button_down.circle )    setButtonDown( &ps3_shapeButton, PS3_BTN_CIR );
    if( Ps3.event.button_down.cross )     setButtonDown( &ps3_shapeButton, PS3_BTN_CRS );
    if( Ps3.event.button_up.triangle )    setButtonUp(   &ps3_shapeButton, PS3_BTN_TRI );
    if( Ps3.event.button_up.square )      setButtonUp(   &ps3_shapeButton, PS3_BTN_SQR );
    if( Ps3.event.button_up.circle )      setButtonUp(   &ps3_shapeButton, PS3_BTN_CIR );
    if( Ps3.event.button_up.cross )       setButtonUp(   &ps3_shapeButton, PS3_BTN_CRS );

    if( Ps3.event.button_down.l1 )  setButtonDown( &ps3_shoulderButton, PS3_BTN_L1 );
    if( Ps3.event.button_down.r1 )  setButtonDown( &ps3_shoulderButton, PS3_BTN_R1 );
    if( Ps3.event.button_down.l2 )  setButtonDown( &ps3_shoulderButton, PS3_BTN_L2 );
    if( Ps3.event.button_down.r2 )  setButtonDown( &ps3_shoulderButton, PS3_BTN_R2 );
    if( Ps3.event.button_up.l1   )  setButtonUp(   &ps3_shoulderButton, PS3_BTN_L1 );  
    if( Ps3.event.button_up.r1   )  setButtonUp(   &ps3_shoulderButton, PS3_BTN_R1 ); 
    if( Ps3.event.button_up.l2   )  setButtonUp(   &ps3_shoulderButton, PS3_BTN_L2 );  
    if( Ps3.event.button_up.r2   )  setButtonUp(   &ps3_shoulderButton, PS3_BTN_R2 );  

    if( Ps3.event.button_down.start  )  setButtonDown( &ps3_optionButton, PS3_BTN_STA );
    if( Ps3.event.button_down.select )  setButtonDown( &ps3_optionButton, PS3_BTN_SEL ); 
    if( Ps3.event.button_down.ps     )  setButtonDown( &ps3_optionButton, PS3_BTN_PS );    
    if( Ps3.event.button_up.start    )  setButtonUp(   &ps3_optionButton, PS3_BTN_STA );   
    if( Ps3.event.button_up.select   )  setButtonUp(   &ps3_optionButton, PS3_BTN_SEL );
    if( Ps3.event.button_up.ps       )  setButtonUp(   &ps3_optionButton, PS3_BTN_PS );      

    if( Ps3.event.button_down.up   )  setButtonDown( &ps3_crossKey, PS3_KEY_UP ); 
    if( Ps3.event.button_down.left )  setButtonDown( &ps3_crossKey, PS3_KEY_LEFT );   
    if( Ps3.event.button_down.right)  setButtonDown( &ps3_crossKey, PS3_KEY_RIGHT );  
    if( Ps3.event.button_down.down )  setButtonDown( &ps3_crossKey, PS3_KEY_DOWN );   
    if( Ps3.event.button_up.up     )  setButtonUp(   &ps3_crossKey, PS3_KEY_UP );       
    if( Ps3.event.button_up.left   )  setButtonUp(   &ps3_crossKey, PS3_KEY_LEFT );    
    if( Ps3.event.button_up.right  )  setButtonUp(   &ps3_crossKey, PS3_KEY_RIGHT );   
    if( Ps3.event.button_up.down   )  setButtonUp(   &ps3_crossKey, PS3_KEY_DOWN );     

    // //--- Digital cross/square/triangle/circle button events ---
    // if( Ps3.event.button_down.cross ) Serial.println("Started pressing the cross button");
    // if( Ps3.event.button_up.cross ) Serial.println("Released the cross button");
    // if( Ps3.event.button_down.square ) Serial.println("Started pressing the square button");
    // if( Ps3.event.button_up.square ) Serial.println("Released the square button");
    // if( Ps3.event.button_down.triangle )  Serial.println("Started pressing the triangle button");
    // if( Ps3.event.button_up.triangle )  Serial.println("Released the triangle button");
    // if( Ps3.event.button_down.circle )  Serial.println("Started pressing the circle button");
    // if( Ps3.event.button_up.circle )  Serial.println("Released the circle button");

    // //--------------- Digital D-pad button events --------------
    // if( Ps3.event.button_down.up ) Serial.println("Started pressing the up button");
    // if( Ps3.event.button_up.up )  Serial.println("Released the up button");
    // if( Ps3.event.button_down.right )  Serial.println("Started pressing the right button");
    // if( Ps3.event.button_up.right )  Serial.println("Released the right button");
    // if( Ps3.event.button_down.down )  Serial.println("Started pressing the down button");
    // if( Ps3.event.button_up.down )  Serial.println("Released the down button");
    // if( Ps3.event.button_down.left )  Serial.println("Started pressing the left button");
    // if( Ps3.event.button_up.left )  Serial.println("Released the left button");

    // //------------- Digital shoulder button events -------------
    // if( Ps3.event.button_down.l1 )  Serial.println("Started pressing the left shoulder button");
    // if( Ps3.event.button_up.l1 )  Serial.println("Released the left shoulder button");
    // if( Ps3.event.button_down.r1 )  Serial.println("Started pressing the right shoulder button");
    // if( Ps3.event.button_up.r1 )  Serial.println("Released the right shoulder button");

    // // //-------------- Digital trigger button events -------------
    // if( Ps3.event.button_down.l2 )  Serial.println("Started pressing the left trigger button");
    // if( Ps3.event.button_up.l2 )  Serial.println("Released the left trigger button");
    // if( Ps3.event.button_down.r2 )  Serial.println("Started pressing the right trigger button");
    // if( Ps3.event.button_up.r2 )  Serial.println("Released the right trigger button");

    // //--------------- Digital stick button events --------------
    // if( Ps3.event.button_down.l3 )  Serial.println("Started pressing the left stick button");
    // if( Ps3.event.button_up.l3 )  Serial.println("Released the left stick button");

    // if( Ps3.event.button_down.r3 )  Serial.println("Started pressing the right stick button");
    // if( Ps3.event.button_up.r3 )  Serial.println("Released the right stick button");

    // //---------- Digital select/start/ps button events ---------
    //if( Ps3.event.button_down.select )  Serial.println("Started pressing the select button");
    //if( Ps3.event.button_up.select ){ } 

    // if( Ps3.event.button_down.start )  Serial.println("Started pressing the start button");
    // if( Ps3.event.button_up.start )  Serial.println("Released the start button");

    // if( Ps3.event.button_down.ps )  Serial.println("Started pressing the Playstation button");
    // if( Ps3.event.button_up.ps )  Serial.println("Released the Playstation button");



    //---------------- Analog stick value events ---------------

    ps3_analogLY  = Ps3.data.analog.stick.ly;
    ps3_analogLX = Ps3.data.analog.stick.lx;
    ps3_analogLX_std  = analog2val( ps3_analogLX );
    ps3_analogLY_std = analog2val( ps3_analogLY )*-1;    

    ps3_analogRY  = Ps3.data.analog.stick.ry;
    ps3_analogRX = Ps3.data.analog.stick.rx;
    ps3_analogRX_std = analog2val( ps3_analogRX );
    ps3_analogRY_std = analog2val( ps3_analogRY )*-1;

   //---------------------- Battery events ---------------------
    if( ps3_battery != Ps3.data.status.battery ){
        ps3_battery = Ps3.data.status.battery;
        Serial.print("The controller battery is ");
        if( ps3_battery == ps3_status_battery_charging )      Serial.println("charging");
        else if( ps3_battery == ps3_status_battery_full )     Serial.println("FULL");
        else if( ps3_battery == ps3_status_battery_high )     Serial.println("HIGH");
        else if( ps3_battery == ps3_status_battery_low)       Serial.println("LOW");
        else if( ps3_battery == ps3_status_battery_dying )    Serial.println("DYING");
        else if( ps3_battery == ps3_status_battery_shutdown ) Serial.println("SHUTDOWN");
        else Serial.println("UNDEFINED");
    }

}

void ps3_serialMonitor(){
    int ps3input[4];
    static int prev_ps3_crossKey = 0;       // 0,1,2,3 -> Up Left Right Down
    static int prev_ps3_shapeButton = 0;    // 0,1,2,3 -> Tri, Square, O, X
    static int prev_ps3_shoulderButton = 0; // 0,1,2,3 -> L1,R1, L2, R2
    static int prev_ps3_optionButton = 0;   // 0,1     -> start, select

    static int prev_ps3_analogLX = 0;
    static int prev_ps3_analogLY = 0;
    static int prev_ps3_analogRX = 0;
    static int prev_ps3_analogRY = 0;

    getButtonStateArr_analogStick( ps3input );

   //if( abs( prev_ps3_analogLX - ps3_analogLX )>1 || abs( prev_ps3_analogLY - ps3_analogLY) > 1 ){
   if( abs( prev_ps3_analogLX - ps3input[0] )>1 || abs( prev_ps3_analogLY - ps3input[1] ) > 1 ){
       Serial.print("left stick:");
       Serial.print(" x="); Serial.print(Ps3.data.analog.stick.lx, DEC);
       Serial.print(" y="); Serial.print(Ps3.data.analog.stick.ly, DEC);
       Serial.println();
   }

   //if( abs( prev_ps3_analogRX - ps3_analogRX )>1 || abs( prev_ps3_analogRY - ps3_analogRY ) > 1 ){
   if( abs( prev_ps3_analogRX - ps3input[2]  )>1 || abs( prev_ps3_analogRY - ps3input[3] ) > 1 ){
       Serial.print("right stick:");
       Serial.print(" x="); Serial.print(Ps3.data.analog.stick.rx, DEC);
       Serial.print(" y="); Serial.print(Ps3.data.analog.stick.ry, DEC);
       Serial.println();
   }

    prev_ps3_analogLX = ps3input[0];
    prev_ps3_analogLY = ps3input[1];
    prev_ps3_analogRX = ps3input[2];
    prev_ps3_analogRY = ps3input[3];


    if( prev_ps3_crossKey != ps3_crossKey ) {
      Serial.print(" cross: "); 
      getButtonStateArr_crossKey( ps3input );
      for( int i=0; i<4; i++ ){
        Serial.print( ps3input[i] ); Serial.print( ", " ); 
      }
      Serial.println( "" ); 

      prev_ps3_crossKey = ps3_crossKey;
    }
    if( prev_ps3_shapeButton != ps3_shapeButton ) {
      Serial.print(" shape: ");       
      getButtonStateArr_shapeButton( ps3input );
      for( int i=0; i<4; i++ ){
        Serial.print( ps3input[i] ); Serial.print( ", " ); 
      }
      Serial.println( "" ); 
 
      prev_ps3_shapeButton = ps3_shapeButton;
    }

    if( prev_ps3_shoulderButton != ps3_shoulderButton ) {
      Serial.print(" LR: "); 
      getButtonStateArr_shoulderButton( ps3input );
      for( int i=0; i<4; i++ ){
        Serial.print( ps3input[i] ); Serial.print( ", " ); 
      }
      Serial.println( "" ); 
      prev_ps3_shoulderButton = ps3_shoulderButton;
    }

    if( prev_ps3_optionButton != ps3_optionButton ) {
      Serial.print(" Option: "); 
      getButtonStateArr_optionButton( ps3input );
      for( int i=0; i<4; i++ ){
        Serial.print( ps3input[i] ); Serial.print( ", " ); 
      }      
      Serial.println( "" ); 
      prev_ps3_optionButton = ps3_optionButton;
    }
}

void ps3_onConnect(){
    Serial.println("Connected.");
}


