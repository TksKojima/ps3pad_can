
#include "wifi_ps3padcan.h"

WebServer server(80);

const char* mdns_name = "esp32ps3can";

//セッティングONのときの設定
const char* set_ssid = "esp32ps3can";
const char* set_password = "98765432";
int set_ip[] ={ 192, 168, 0, 135 };
int set_gt[] ={ 192, 168, 0, 1 };
int set_subnet[] ={ 255,255,255,0 };


// パラメータ 
const int para_num = 2;
String arg_str[] = {"id", "cycle"};
String para[ para_num ];
int para_int[ para_num ];



// センサのデータ(JSON形式)
//const char SENSOR_JSON[] PROGMEM = R"=====({"val1":%.1f})=====";
//const char SENSOR_JSON[] PROGMEM = R"=====({"val0":%.1f, "val1":%.1f, "val2":%.1f, "str0":"%s", "str1":"%s", "str2":"%s"})=====";
//const char SENSOR_JSON[] PROGMEM = R"=====({"val0":%.1f, "val1":%.1f, "val2":%.1f, "mode":%d})=====";

// // Webコンテンツのイベントハンドラ
// // センサのデータ(JSON形式)
// const char SENSOR_JSON[] PROGMEM = R"=====({"val1":%.1f})=====";

//  MIMEタイプを推定
String getContentType(String filename){
  if(filename.endsWith(".html") || filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else return "text/plain";
}


void wifi_setup( String btMac ){

  char set_ssid_withid[32];
  strcpy(set_ssid_withid, set_ssid);  // ベースの文字列をコピー
  strcat(set_ssid_withid, btMac.c_str() );  // 数値を追加

  wifi_setup_input( 0, MODE_AP_ONLY, set_ip, set_gt, set_subnet, const_cast<char*>( set_ssid_withid ), const_cast<char*>( set_password ));
  server_setup();

}

int wifi_setup_input(  int device_id, int connect_mode,  
  int in_ip[], int in_gw[], int in_subnet[], char* in_ssid, char* in_password ){

  IPAddress ip(     in_ip[0], in_ip[1], in_ip[2], in_ip[3] );
  IPAddress gateway( in_gw[0], in_gw[1], in_gw[2], in_gw[3] ); //gatewayのIPアドレス
  IPAddress subnet( in_subnet[0], in_subnet[1], in_subnet[2], in_subnet[3]);

  unsigned long connectTimeout = 10000; //10 seconds

  Serial.println("");
  Serial.println("[ WiFi setup ]");
    
  if( connect_mode == MODE_STA_ONLY  ){
    // Station mode
    Serial.println("STA mode started");
    WiFi.mode(WIFI_STA);
    //WiFi.config(ip, gateway, subnet);

    // Connect to WiFi network
    Serial.print("Connecting to ");
    Serial.println(in_ssid);

    WiFi.begin(in_ssid, in_password);

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED ) {
      delay(500);
      Serial.print(".");

      if( millis() - start > connectTimeout ){
        Serial.print("Time Out!");

      }

    }
    Serial.println("");
    Serial.println("Connected! ");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Gateway: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("Subnet: ");
    Serial.println(WiFi.subnetMask());

  }
  else{  //if( connect_mode == MODE_AP_ONLY ){
    Serial.println("AP mode started");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(in_ssid, in_password, 1, 0, 4);
    delay(200);
    WiFi.softAPConfig( ip,gateway,subnet );

    // Connect to WiFi network
    Serial.print("AP SSID: ");
    Serial.println( in_ssid);
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
  
  }

  if (MDNS.begin( mdns_name ) ) {
    Serial.print("MDNS: ");
    Serial.println( mdns_name );
  }

  
  return 0;  
}

void server_setup(){

  // Webサーバーのコンテンツ設定

  server.enableCORS(); //This is the magic

  server.on("/", handleRoot);
  server.on("/set", handleSetPara); 
  server.on("/get", handleRoot); 

  server.onNotFound(handleNotFound);

  //server.on("/post",  handlePost);
  // server.on("/vue.js", handleVueJs);
  // server.on("/axios.min.js", handleAxiosJs);

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

}

void wifi_loop(){
  server.handleClient();
}


String paraString(){
  String retstr = "";
  for( int i=0; i<para_num; i++){
    retstr += arg_str[i] + " : " + para[i] + ",  ";
  }
  return retstr;
}

String htmlString( String headstr,  String footstr ){
  String retstr = "";
  retstr += "<HTML><BODY>";
  retstr += headstr;

  retstr += "Now Para:<BR>   ";
  retstr += paraString();

  retstr += "<BR><BR>";  
  retstr += "Usage: http://esp32ps3can/set?";
  for( int i=0; i<para_num; i++){
    if( i!=0 ){ 
      retstr += "&";
    }
    retstr += arg_str[i] + "=" + "00";
  }

  retstr += footstr;
  retstr += "</HTML></BODY>";

  return retstr;
}

void handleRoot() { //ブラウザのUI

  server.send(200, "text/html", htmlString( "config PS3pad CAN ID & Tx cycle time <BR><BR>","" ) );
  //handleGet();

}

void wifi_setPara_str( String _para[] ){ // size = para_num
  for( int i=0; i<para_num; i++ ){
    para[i] = _para[i];
    para_int[i] = para[i].toInt();
  }

}
void wifi_setPara_int( int  _para[] ){ // size = para_num
  for( int i=0; i<para_num; i++ ){
    para_int[i] = _para[i];
    para[i] = String( para_int[i] );

  }

}

void wifi_writeParaToEep(){
  eep_write_num( EEP_ADDR_ID,    para_int[0] );
  eep_write_num( EEP_ADDR_CYCLE, para_int[1] );

}

void handleSetPara() {
  // String para[0] = server.arg("id");
  // String para[1] = server.arg("cycle");
  for( int i=0; i<para_num; i++ ){
    String temppara = server.arg( arg_str[i] );
    if( temppara != "" ){
      para[i] = temppara;
      para_int[i] = para[i].toInt() ;
    }
  }
  wifi_writeParaToEep();
  server.send(200, "text/html", htmlString( "Set parameter <BR><BR>", "") );  // 応答を返す
}

// void handlePostCheck() { //
//   IPAddress nowIP;
//   if( WiFi.getMode() == WIFI_STA ){
//     nowIP = WiFi.localIP();
//   }else{
//     nowIP = WiFi.softAPIP();
//   }
   
//   server.send(200, "text/html", nowIP.toString() ); 

//   // IPアドレスを取得し、シリアルモニターに表示
//   // Serial.print("IP address: ");
//   // Serial.println( nowIP.toString() );    
//   // Serial.println( WiFi.localIP().toString() );    
  
// }

void handleGet() { //ブラウザのUIを操作した結果のJSからアクセスされる
  // for (int i = 0; i < server.args(); i++) {
  //   int Val_i = server.arg(i).toInt();
  //   Serial.print(server.argName(i) + "=" + server.arg(i) + ", ");
    
  //   if(      server.argName(i) == "D0" ) { dat0_input = Val_i; }
  //   else if( server.argName(i) == "D1" ) { dat1_input = Val_i; }

  // }
  Serial.println();
  server.send(200, "text/plain", "\n\n\n");
}

void handlePost() {
  // if (server.method() == HTTP_POST) {
  //   String postData = server.arg("plain");  // POSTデータを文字列として取得

  //   // JSONパーサーを使用してJSONデータを解析する
  //   DynamicJsonDocument doc(6144);   // 解析用のJSONオブジェクトを生成
  //   DeserializationError error = deserializeJson(doc, postData);  // JSONデータを解析する
  //   //Serial.println( postData );

  //   if (error) {
  //     Serial.print("deserializeJson() failed: ");
  //     Serial.println(error.f_str());

  //   } else {
  //     // 配列要素を1つずつ処理する
  //     JsonArray array = doc.as<JsonArray>();
  //     for (JsonVariant obj : array) {
  //       int id = obj["canid"];                 // "id"プロパティを取得
  //       int cycle = obj["cycle"];           // "cycle"プロパティを取得
  //       int dlc = obj["dlc"];           // "cycle"プロパティを取得
  //       JsonArray data = obj["data"];       // "data"プロパティを取得
  //       int trans = obj["trans"];           // "cycle"プロパティを取得

  //       //  Serial.print("canid: ");
  //       //  Serial.print(id);
  //       //  Serial.print(", dlc: ");
  //       //  Serial.print(dlc);
  //       //  Serial.print(", cycle: ");
  //       //  Serial.print(cycle);
  //       //  Serial.print(", trans: ");
  //       //  Serial.print(trans);
  //       //  Serial.print(", data: ");
  //       unsigned char inputdata[8];
  //       int i=0;
  //       for (JsonVariant value : data) {
  //         unsigned char v = value.as<int>();          // 配列要素をint型に変換
  //         // ここで取得したデータを処理する
  //       //  Serial.print(v);
  //       //  Serial.print(", ");
  //        inputdata[i] = v; i++;

  //       }
  //      // Serial.print("posted ");

  //       canTxbuf_set( id, dlc, cycle, inputdata, trans);
  //       //  Serial.println("");
  //     }
  //   }

  //   //Serial.println("data recv");
  //   //CROSエラー回避のヘッダを追加
  //   // server.sendHeader("Access-Control-Allow-Origin", "http://localhost:8080/");
  //   // server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
     
  //   server.send(200, "text/plain", "Data received");
  //   //http.StatusOK
  // } else {
  //   //server.send(405, "text/plain", "Method Not Allowed");
  //   server.send(200, "text/plain", "Method Not Allowed");

  // }
}

void handlePostEep() {
  // if (server.method() == HTTP_POST) {
  //   String postData = server.arg("plain");  // POSTデータを文字列として取得

  //   // JSONパーサーを使用してJSONデータを解析する
  //   DynamicJsonDocument doc(6144);   // 解析用のJSONオブジェクトを生成
  //   DeserializationError error = deserializeJson(doc, postData);  // JSONデータを解析する

  //   Serial.println( "[ POST EEP Data ]" );
  //   Serial.println( postData );

  //   if (error) {
  //     Serial.print("deserializeJson() failed: ");
  //     Serial.println(error.f_str());

  //   } else {
  //     int deviceId = doc["device_id"];  // int型の値を取得
  //     int connect_mode = doc["connect_mode"]; 

  //     int ip_addr[]={0,0,0,0};
  //     int subnet_mask[]={0,0,0,0};
  //     int def_gateway[]={0,0,0,0};
  //     const char*  ssid_str = doc["ssid"]; 
  //     const char*  password_str = doc["password"];  

  //     ip_addr[0] = doc["ip_addr0"];
  //     ip_addr[1] = doc["ip_addr1"];
  //     ip_addr[2] = doc["ip_addr2"];
  //     ip_addr[3] = doc["ip_addr3"];

  //     subnet_mask[0] = doc["subnet0"];
  //     subnet_mask[1] = doc["subnet1"];
  //     subnet_mask[2] = doc["subnet2"];
  //     subnet_mask[3] = doc["subnet3"];

  //     def_gateway[0] = doc["gateway0"];
  //     def_gateway[1] = doc["gateway1"];
  //     def_gateway[2] = doc["gateway2"];
  //     def_gateway[3] = doc["gateway3"];

  //     Serial.print("DeviceID: ");
  //     Serial.println(deviceId);
  //     Serial.print("connect_mode: ");
  //     Serial.println(connect_mode);
  //     Serial.print("ip_addr: ");
  //     Serial.print(ip_addr[0]);
  //     Serial.print(ip_addr[1]);
  //     Serial.print(ip_addr[2]);
  //     Serial.println(ip_addr[3]);
  //     Serial.print("subnet: ");
  //     Serial.print(subnet_mask[0]);
  //     Serial.print(subnet_mask[1]);
  //     Serial.print(subnet_mask[2]);
  //     Serial.println(subnet_mask[3]);
  //     Serial.print("gateway: ");
  //     Serial.print(def_gateway[0]);
  //     Serial.print(def_gateway[1]);
  //     Serial.print(def_gateway[2]);
  //     Serial.println(def_gateway[3]);
  //     Serial.print( "pass: ");
  //     Serial.println( password_str);
  //     Serial.print( "ssid: ");
  //     Serial.println( ssid_str);


  //     eep_write_num( EEP_ADDR_ID,   deviceId );
  //     eep_write_num( EEP_ADDR_MODE, connect_mode );

  //     for( int i=0; i<4; i++ ){
  //       eep_write_num( EEP_ADDR_ST_IP + i,   ip_addr[i] );
  //       eep_write_num( EEP_ADDR_ST_SN + i,   subnet_mask[i] );
  //       eep_write_num( EEP_ADDR_ST_GW + i,   def_gateway[i] );
  //     }

  //     eep_write_str( EEP_ADDR_ST_PASS, password_str, EEP_SIZE_ST_PASS );
  //     eep_write_str( EEP_ADDR_ST_SSID, ssid_str, EEP_SIZE_ST_SSID );

  //     Serial.println( "EEP Wrote ");

  //     // Serial.println( "EEP Read ");

  //     // eep_read_str( EEP_ADDR_AP_PASS, char* str, int size) 


  //   }

  //   //Serial.println("data recv");
  //   //CROSエラー回避のヘッダを追加
  //   // server.sendHeader("Access-Control-Allow-Origin", "http://localhost:8080/");
  //   // server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
     
  //   server.send(200, "text/plain", "Data received");
  //   //http.StatusOK

  // } else {
  //   //server.send(405, "text/plain", "Method Not Allowed");
  //   server.send(200, "text/plain", "Method Not Allowed");

  // }
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}


