#include <CAN_appLite.h>

// Lite版はcanbufの配列数を0x800にせず、1000以下ぐらいで運用

const int bufNum = 500; 
canRxBuffer canbuf[bufNum];

short id2idx_arr[ 0x800 ];
int now_idx = 0;

int tx_test_flag = 0;
int rx_test_flag = 0;

int countInterval = 1000;
int countMax = 5;


void can_init(){
   //Serial.println("CAN Sender");
   CAN.setPins(32, 27); // ESP-tx:2562-tx,  ESP-rx:2562-rx
   //CAN.setPins(4, 5);

  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }

  canbuf_init();

  // register the receive callback
  CAN.onReceive(onReceive);
}

// tx_test_flagが１だと、テスト用のID１～３０の適当なCANメッセージを送信
// rx_test_flagが１だと、ESP32のシリアルで受信データを表示
void can_setTestFlag( int txtest, int rxtest ){ //loopの前ぐらいに置く
  tx_test_flag = txtest;
  rx_test_flag = rxtest;

}

void can_loop(){

  if( tx_test_flag ){
    canTxbuf_set_test();
  }

  canbuf_send();
  
}

void canbuf_init(){
  for( int i=0; i<bufNum; i++){
    canbuf[i].id = -1;

    canbuf[i].dlc = 1;
    canbuf[i].txrxFlag = 0;    
    canbuf[i].cycleTime = 1500;
    canbuf[i].data.u2[0] = 0;
    canbuf[i].data.u2[1] = 0;
    canbuf[i].data.u2[2] = 0;
    canbuf[i].data.u2[3] = 0;
    
    canbuf[i].prevTime = millis();
  }

  for( int i=0; i<0x800; i++ ){
    id2idx_arr[i] = -1;

  }
}

int id2idx( int id ){
  if( id2idx_arr[id] == -1 ){
    id2idx_arr[id] = now_idx;
    now_idx++;

    if( now_idx >= bufNum ){
      now_idx = bufNum - 1;
    }

  }

  return id2idx_arr[id];

}

void canTxbuf_set_test(){
  unsigned char  testdata[8];
  testdata[0] = 192;
  testdata[1] = 168;
  testdata[2] = 10;
  testdata[3] = 10;
  testdata[4] = 0;
  testdata[5] = 4;
  testdata[6] = 0;
  testdata[7] = 0;
  canTxbuf_set( 0x732, 8, 732, testdata, 1);

  // for( int i=1; i<=30; i++){
  //   canbuf[i].dlc = i%8 + 1;
  //   canbuf[i].txrxFlag = 1;
  //   canbuf[i].cycleTime = canbuf[i].dlc * 100;
  //   if( i<5 ) {
  //       canbuf[i].cycleTime = canbuf[i].dlc  * 10;
  //   }
  //   canbuf[i].data.u2[0] = 0x1234;
  //   canbuf[i].data.u2[1] = 0x5678;
  //   canbuf[i].data.u2[2] = 0x9abc ;
  //   canbuf[i].data.u2[3] = 0;
  //   canTxbuf_set( i, canbuf[i].dlc, canbuf[i].cycleTime, canbuf[i].data.u1, 1 );
  // }  
}

void canTxbuf_set( int id, char dlc, int cycle, unsigned char *data, int txflag ){
  int tx_idx = id2idx( id );

  canbuf[tx_idx].id = id;  
  canbuf[tx_idx].dlc = dlc;  
  canbuf[tx_idx].cycleTime = cycle;
  for( int n=0; n<dlc; n++){
    canbuf[tx_idx].data.u1[n] = data[n];
  }
  if( txflag == 1 ){
    canbuf[tx_idx].txrxFlag = canTxRxFlag::TX;
  }else{
    canbuf[tx_idx].txrxFlag = 0;
  }
  //canbuf[id].prevTime = millis();
  canbuf[tx_idx].noChange.txCnt[0] = millis();

  //Serial.println("can seted ");

}

void canbuf_sendSingle( int idx ){
  CAN.beginPacket( canbuf[idx].id );
  for( int i=0; i<canbuf[idx].dlc; i++){
    CAN.write( canbuf[idx].data.u1[i] );
    // Serial.print("u1: ");
    // Serial.print(canbuf[id].data.u1[i]);     
  }
  CAN.endPacket();

  // Serial.print("canid: ");
  // Serial.print(id); 
  // Serial.print(" data");
  // Serial.print(" ");
  // Serial.print(canbuf[id].data.u1[i]);  

  if( tx_test_flag == 1 ){
    Serial.print("canid: ");
    Serial.print(canbuf[idx].id); 
    Serial.print(" cycle: ");
    Serial.print(canbuf[idx].cycleTime);    
    Serial.print(" dlc: ");
    Serial.print( (int)(canbuf[idx].dlc));    
    Serial.print(" ");

    for( int n=0; n<canbuf[idx].dlc; n++){
      Serial.print(" ");
      Serial.print(canbuf[idx].data.u1[n]);    

    } 
    Serial.print(" time: ");
    Serial.println(canbuf[idx].prevTime);    
  }
}

void canbuf_send(){
  for( int i=0; i<bufNum; i++){
    if( canbuf[i].txrxFlag == canTxRxFlag::TX){
      if( millis() - canbuf[i].noChange.txCnt[0]  >= canbuf[i].cycleTime + 2000 ){ //サイクルタイム＋閾値ミリ秒の間、setされなかったら途絶判定 
        canbuf[i].txrxFlag = canTxRxFlag::NON;
        continue;
      }

      if( millis() - canbuf[i].prevTime >= canbuf[i].cycleTime ){
        //Serial.print("before send");
        canbuf_sendSingle(i);
        //Serial.print("after send");
        canbuf[i].prevTime = millis();
        //canbuf[i].data.u2[3]++; 
        // Serial.print("send id: ");
        // Serial.println(i);
      }
    }
  }
}

void onReceive(int packetSize) {

  if (CAN.packetExtended()) {
    Serial.print("extended ");
  }

  // if (CAN.packetRtr()) {
  //   // Remote transmission request, packet contains no data
  //   //Serial.print("RTR ");
  // }



 int rx_id = -1;
 int rx_dlc = -1;
  if (CAN.packetRtr()) {
    //Serial.print(" and requested length ");
    //Serial.println(CAN.packetDlc());
  } else {


    rx_id  = CAN.packetId();
    rx_dlc = CAN.packetDlc();
    int rx_idx = id2idx( rx_id );
    canbuf[rx_idx].dlc = rx_dlc;
    canbuf[rx_idx].cycleTime = millis() - canbuf[rx_id].prevTime;
    canbuf[rx_idx].prevTime  = millis();
    canbuf[rx_idx].txrxFlag = canTxRxFlag::RX;

  if( rx_test_flag == 1 ){
    Serial.print("packet with id 0x");
    Serial.print(CAN.packetId(), HEX);    //Serial.print(" and length ");
    //Serial.println(packetSize);
    Serial.print(" dlc: ");
    Serial.print( rx_dlc );
    Serial.print(" size: ");
    Serial.print( packetSize );

  }

    // only print packet data for non-RTR packets
    int idx = 0;
    while (CAN.available()) {
      char readData = (char)CAN.read();
      if( rx_test_flag == 1 ){
        Serial.print(" ");
        Serial.print( readData, HEX );
        Serial.print(", ");
      }
      if( canbuf[rx_idx].data.u1[idx] != readData){
          canbuf[rx_idx].noChange.rxCnt[idx] = 0;
      }
      canbuf[rx_idx].data.u1[idx] = readData;
      canbuf[rx_idx].noRecvCnt[idx] = 0;
      idx++;
    }

    if( rx_test_flag == 1 ){
      Serial.println();
    }

      // received a packet
    static unsigned long prevtime=millis();  
    if( millis() - prevtime > 3000 ){
        //  makeCanMsgJson();
        //  wifi_websocket_broad_loop( can_json, strlen(can_json) );
      Serial.print("Received "); Serial.println(idx);
      prevtime=millis(); 
    }       
 
  }
  //Serial.println();    
}

void recvDataTimeCount(){
  static unsigned long prevtime=millis();
  if( millis() - prevtime > countInterval ){
    for( int i=0; i<bufNum; i++){
      if( canbuf[i].txrxFlag == canTxRxFlag::RX ){
        for( int k=0; k<canbuf[i].dlc; k++){
          canbuf[i].noChange.rxCnt[k] += ( canbuf[i].noChange.rxCnt[k] >= countMax ? 0 : 1 );
          canbuf[i].noRecvCnt[k] += ( canbuf[i].noRecvCnt[k] >= countMax ? 0 : 1 );

        }
      }
    }
    prevtime =  millis();
  }
}

void printRecv(){
   Serial.println("printRecv");
//    for( int i=0; i<0x800; i++){
    for( int i=0; i<bufNum; i++){
      if( canbuf[i].txrxFlag == canTxRxFlag::RX ){
        canbuf[i].txrxFlag =  canTxRxFlag::NON;
        Serial.print(" ID: ");
        Serial.print(i, HEX);
        Serial.print(" cycl: ");
        Serial.print(canbuf[i].cycleTime);

        Serial.print(" data: ");
        for( int k=0; k<canbuf[i].dlc; k++){
          Serial.print(canbuf[i].data.u1[k], HEX);
          Serial.print(", ");

        }
        Serial.println("");
      }
    }

}
void setup_CallBack() {
  Serial.println("CAN Receiver Callback");

  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }

  // register the receive callback
  CAN.onReceive(onReceive);
}

StaticJsonDocument<4000> candoc;
char can_json[3000];

void makeCanMsgJson(){
  int cnt=0;
  for( int i=0; i<bufNum; i++){
//  for( int i=0; i<100; i++){
    if( canbuf[i].txrxFlag == canTxRxFlag::RX ){
      cnt++;
    }
  }

  candoc.clear();
  JsonArray canmsgs = candoc.createNestedArray("canmsg");
  canmsgs[0]["rxnum"] = cnt;

  int json_cnt=0;
  static int last_idx=0;
  for( int j=0; j<bufNum; j++){
  //for( int i=0; i<100; i++){
    int i = last_idx + j;
    if( i >= bufNum ){
      i = i - bufNum;
    }
    if( canbuf[i].txrxFlag == canTxRxFlag::RX ){
      canbuf[i].txrxFlag =  canTxRxFlag::NON;
      JsonObject canmsg = canmsgs.createNestedObject();
      canmsg["id"]  = canbuf[i].id;
      canmsg["dlc"] = canbuf[i].dlc;
      canmsg["cycle"] = canbuf[i].cycleTime;

      JsonArray canmsg_data = canmsg.createNestedArray("data");
    
      for( int k=0; k<canbuf[i].dlc; k++){
        canmsg_data.add( canbuf[i].data.u1[k] );    
      }

      json_cnt++;
    }
    
    if( json_cnt > 25 ){
      last_idx = i; 
      break;
    }

    if( j == (bufNum-1)){
      last_idx = 0; 
    }

  }

  serializeJson(candoc, can_json);
  //Serial.print("RX_ID_num:"); Serial.println(cnt);
}

void makeCanMsgJsonDummy(){
  //
  candoc.clear();

  JsonArray canmsg = candoc.createNestedArray("canmsg");

  int rndfornum = random(1,3);
  for(int k=0;k<rndfornum;k++){
    JsonObject canmsg_0 = canmsg.createNestedObject();
    int rndID = random(1,0x7ff);

    canmsg_0["id"] = rndID;
    canmsg_0["dlc"] = (rndID%8) + 1;
    canmsg_0["cycle"] = rndID*10;


    JsonArray canmsg_0_data = canmsg_0.createNestedArray("data");
    canmsg_0_data.add(random(0,255));
    canmsg_0_data.add(random(0,127));
    canmsg_0_data.add(random(0,63));
    canmsg_0_data.add(random(0,31));
    canmsg_0_data.add(random(0,15));
    canmsg_0_data.add(random(0,7));
    canmsg_0_data.add(random(0,3));
    canmsg_0_data.add(1);

  };

  serializeJson(candoc, can_json);

}

