/*=========================================================
 * ESP8266 example To set LED ON/OFF
 * 阿布拉機
 * http://arbu00.blogspot.tw/
 * 
 * Ashing tsai
 * 2016/09/01
 =========================================================*/
#include <SoftwareSerial.h>
#include "Timer.h"
SoftwareSerial WifiSerial(2,3); /* RX:D2, TX:D3 */
Timer T1;
#define SSID        "Buffalo-G-2.4" 
//#define SSID_2        "Xperia C4" 
#define PASSWORD    "guitar100"
String cmdAT = "";
String data = ""; 
String StrWebMsg="";
int Wifi_State,delaytime;
byte connID;
String WebMsgIPD="";
int GetPin=0,GetPinH=0,GetPinL=0;
int PinState=0;
void setup(void)
{
      WifiSerial.begin(9600);
      Serial.begin(9600);
      Serial.print("setup begin\r\n");
      pinMode(10,OUTPUT);
      digitalWrite(10,LOW);
      pinMode(11,OUTPUT);
      digitalWrite(11,LOW);
      pinMode(12,OUTPUT);
      digitalWrite(12,LOW);
      
      T1.every(5, ESP8266_Main);

        //1.Reset esp8266
        // cmdAT ="AT+RST";
        // WifiSerial.println(cmdAT);
        sendATcmd("AT+RST",2000); // 重置ESP模組，等待2秒

        //2.TEST ESP8266
           for (int i=0;i<=10;i++){
              cmdAT ="AT";
              WifiSerial.println(cmdAT);
           if (WifiSerial.find("OK")) {  
            Serial.print("Start+++++++++++++++++++++++++++++++++++\r\n");
            Serial.print("ESP8266 Operation ok\r\n");
            break;
           } else {
            Serial.print("ESP8266 Operation error\r\n");
           }
          }
        //3.把ESP8266 的 Wifi連線模式設成station模式：
          for (int i=0;i<=10;i++){
              cmdAT ="AT+CWMODE=1";
              WifiSerial.println(cmdAT);
           if (WifiSerial.find("OK")) {  
              Serial.print("Operate to station ok\r\n");
            break;
           } else {
             Serial.print("Operate to station err\r\n");
           }
          }
         //4.連線到家裡的Wifi網路得到IP adr:
         //AT+CWJAP="Buffalo-G-2.4","guitar100"
            for (int i=0;i<=10;i++){
         cmdAT="AT+CWJAP=\"";
         cmdAT+=SSID;
         cmdAT+="\",\"";
         cmdAT+=PASSWORD;
         cmdAT+="\"";
              WifiSerial.println(cmdAT);
           if (WifiSerial.find("OK")) {  
              Serial.print("Join AP success \r\n");
              sendATcmd("AT+CIFSR",1000);  //AT+CIFSR     #印出 IP 狀態
            break;
           } else {
             Serial.print("Try to Connectto AP,Loading...\r\n");
           }
          }
  
         //5.把「多重連線」打開
         for (int i=0;i<=10;i++){
              cmdAT ="AT+CIPMUX=1";
              WifiSerial.println(cmdAT);
           if (Loding("AT+CWMODE=1")){  
              Serial.print("Set AT+CIPMUX=1 success\r\n");
            break;
           } else {
              Serial.print("Set AT+CIPMUX=1  failure\r\n");
           }
          }

          //6.讓 ESP8266進入伺服器模式，等候其他裝置與它連線：
           for (int i=0;i<=10;i++){
               cmdAT ="AT+CIPSERVER=1,9000";// 在9000埠啟動伺服器
               WifiSerial.println(cmdAT);
           if (Loding("AT+CIPSERVER=1")){  
              Serial.print("Set port 9000  success\r\n");
              Serial.print("ESP8266 configuration Done!\r\n");
              Serial.print("End====================================\r\n");
            break;
           } else {
              Serial.print("Set port 9000  failure\r\n");
              Serial.print("End====================================\r\n");
           }
          }
          //-END-Set to serve mode and connect to Home AP.

}
void loop() {
T1.update();
//chat();
}
void ESP8266_Main(){
     int Debugflag=0;
     if(WifiSerial.available()) {
          StrWebMsg="";
               //Just Debug  for get all message from client
               if (Debugflag==1) {
                   while(WifiSerial.available())
                  {
                   StrWebMsg+=(char)WifiSerial.read(); 
                   delay(2);
                  }
                   Serial.println(StrWebMsg);
                    connID=0;
                    cmdAT = "AT+CIPCLOSE=";
                    cmdAT+=connID;   // 附加連線編號
                    sendATcmd(cmdAT,1000);  // 送出「中斷連線」命令
                    connID=1;
                    cmdAT = "AT+CIPCLOSE=";
                    cmdAT+=connID;   // 附加連線編號
                    sendATcmd(cmdAT,1000);  // 送出「中斷連線」命令
                    
               }else if (WifiSerial.find("+IPD,")) {  // 若接收到"+IPD,"，代表有用戶連線了…
                 delay(2);
                 connID = WifiSerial.read();// 讀取連線編號（1~5）並轉成數字[最多 5 個連線] ,48 is ASCII '0'
                 delay(2);
                 connID = connID-48;  // 讀取連線編號（1~5）並轉成數字[最多 5 個連線] ,48 is ASCII '0'
                while(WifiSerial.available())
                 {
                 StrWebMsg+=(char)WifiSerial.read(); 
                 delay(2);
                }
                Serial.print("connID=");
                Serial.println(connID);
                Serial.println(StrWebMsg);
                
                GetPinH=(char)StrWebMsg[16]-48;   //48 is ASCII '0', Pin :十位數
                GetPinL=(char)StrWebMsg[17]-48;   //48 is ASCII '0', Pin :個位數
                GetPin=GetPinH*10+GetPinL;
                
                Serial.print("Pin=");
                Serial.println(GetPin);

                PinState=(char)StrWebMsg[18]-48;   //48 is ASCII '0'
                Serial.print("PinStatus=");
                Serial.println(PinState);
                digitalWrite(GetPin,PinState);
  
                    cmdAT = "AT+CIPCLOSE=";
                    cmdAT+=connID;   // 附加連線編號
                    sendATcmd(cmdAT,500);  // 送出「中斷連線」命令

           }
     }
  
}

void chat(void) {
 
  if (WifiSerial.available()) {
    Serial.write(WifiSerial.read());
  }
 
  if (Serial.available()) {
    char chars = Serial.read();
    WifiSerial.write(chars);
  }
}
String get_response() {  //get esp responce without "Serial.find()".
  String response="";  
  char c; 
  while (WifiSerial.available()) {  
    c=WifiSerial.read();
    response.concat(c);   
    delay(10);  
    }
  response.trim();  
  return response;
}
boolean Loding(String state){
  String response=get_response();
  for (int timeout=0 ; timeout<30 ; timeout++)
  {
    if(response.indexOf("OK") != -1 || response.indexOf("no change") != -1) 
    {
        Serial.print(" OK ! ");
        if(timeout>=2){
          Serial.print("This commend waste ");
          Serial.print(timeout/2);
          Serial.println(" sec.");
        }
        else Serial.println("");
        return 1; 
        break;
    }
    else if(timeout==29){ //after 15 sec for wait esp without responce.
      Serial.print(state);
      Serial.println(" fail...\nExit2");
      return 0; 
    }
    else{
      response=get_response(); //reget
      if(timeout==0)
        Serial.print("Wifi Loading.");
      else
        Serial.print(".");
      delay(500);
    }
  }
}
void sendATcmd(String cmd, unsigned int time){
    String response = "";  // 接收ESP回應值的變數  
    WifiSerial.println(cmd); // 送出AT命令到ESP模組
    unsigned long timeout = time + millis();
    
    while (WifiSerial.available() || millis() < timeout) {
      while(WifiSerial.available()) {
        char c = WifiSerial.read(); // 接收ESP傳入的字元
        response += c;
      }
    }

    Serial.print(response);  // 顯示ESP的回應
}


 
//AT+GMR       #取得韌體版本
//AT+RST       #重設 
//AT+CWLAP     #取得 ap 列表
//AT+CWMODE?   #查詢目前模式 1 = Station模式, 2 = AP, 3 = BOTH
//AT+CWMODE=3  #設成 ap + client 模式
//AT+CWMODE=3  #設成 ap 模式
//AT+CWMODE=1  #把的 Wifi連線模式設為 Station模式：

//AT+CWLAP     #列出 SID
//AT+CWJAP="Buffalo-G-2.4","guitar100"  # Buffalo-G-2.4
//AT+CIFSR     #印出 IP 狀態
//AT+CWQAP     #切斷已連線的無線網路。
//AT+CIPMUX
//AT+CIPMUX?   #查詢目前的設定值
//AT+CIPMUX=1  #允許一對多的多重連線。
//AT+CIPMUX=0  #關閉多重連線，只能一對一連線。此時無法進入伺服器模式。

//讓 ESP8266扮演伺服器，等候客戶端的連線
//AT+CIPSERVER= <mode>[,<port>]  #開啟(或關閉)伺服器，等候客戶端透過指定的連接埠(Port)連線。<mode>：0關閉伺服器，1開啟。<port>：網路連接埠號碼。範圍從 0~65535皆可

//讓 ESP8266扮演客戶端，對伺服器進行連線，我們需要使用以下指令：
//AT+CIPSTART
//「AT+CIPSTART=?」查詢目前已經建立的連線資訊。
//「AT+CIPSTART =<type>,<addr>,<port>」建立連線，只適用於CIPMUX=0的情況。
//「AT+CIPSTART=<id>,<type>,<addr>,<port>」建立連線，只適用於CIPMUX=1的情況。
//<id>：範圍 0~4。推測最多可以連線到5台伺服器，這是用來代表此連線的代號。
//<type>：連線類型，可為"TCP"或"UDP"(要加引號)。
//<addr>：連線的 IP位址，如"192.168.43.31"(要加引號)。
//<port>：伺服器接受連線的連接埠(Port)號碼(這個不用加引號)。

//AT+CIPCLOSE
//「AT+CIPCLOSE=<id>」中斷指定的連線。適用於 CIPMUX=1 的情況
//「AT+CIPCLOSE」中斷連線。適用於 CIPMUX=0 的情況
//<id>：要中斷的連線代號。

//AT+CIPSEND
//「AT+CIPSEND=<length>」只適用於 CIPMUX=0 的情況。
//「AT+CIPSEND=<id>,<length>」只適用於 CIPMUX=1 的情況。
//<id>：在多重連線的情況下，需要輸入連線的代號，ESP8266才會知道要把訊息發送給哪一條連線。
//<length>要發送的訊息長度。等到此指令發送出去之後，會出現「>」符號在螢幕，這時就可以輸入符合訊息長度的訊息。
//EX:
//AT+CIPSEND=0,5
//> HELLO
//busy s...
//SEND OK
//第一行命令的 0是連線的代號。第一個跟伺服器連線的裝置，連線代號就是0，
//第二個就是1，依此類推，也可以透過「AT+CIPSTATUS」指令去查詢共有幾個裝置連線到伺服器，
//以及這些連線的代號。第一行命令的5，是要傳送的訊息字元數，我們想傳送的"HELLO"訊息共5個字元。
//第一行命令發送出去後，出現「>」符號時，就可以輸入訊息"HELLO"了。
//AT+CIPSTATUS   #查詢共有幾個裝置連線到伺服器，以及這些連線的代號

