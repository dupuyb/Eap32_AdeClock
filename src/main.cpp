
// Frame wifi
#include "FrameWeb.h"
FrameWeb frame;

#include <WebSocketsServer.h>
#include <HTTPClient.h>
#include <time.h>
#include "DotFlipper.h"
// Reset Reason 
#include <rom/rtc.h>

DotFlipper display = DotFlipper();

const char VERSION[] ="0.0.1";
const String WeekDays[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
// Debug macro 
#define DEBUG_MAIN

// Serial command
int8_t cmd;
int8_t wifiLost = 0;

// Time facilities
const long gmtOffset_sec     = 3600;
int daylightOffset_sec = 3600; // heure d'ete 3600
struct tm timeinfo;            // time struct
const char* ntpServer        = "fr.pool.ntp.org";
String rebootTime;
// Internal led
#define EspLedBlue 2
long previousMillis = 0;

// Time HH:MM.ss
String getTime() {
  static char temp[10];
  snprintf(temp, 10, "%02d:%02d:%02d", timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec );
  return String(temp);
}

// Date as europeen format
String getDate(int sh = -1){
  static char temp[20];
  switch (sh) {
  case 0: 
    // snprintf(temp, 20, "%02d/%02d/%04d", timeinfo.tm_mday, (timeinfo.tm_mon+1), (1900+timeinfo.tm_year) );
    snprintf(temp, 20, "%02d:%02d   %s %d", timeinfo.tm_hour, timeinfo.tm_min, WeekDays[timeinfo.tm_wday].c_str(),  timeinfo.tm_mday);
    break;
  case 1:
    snprintf(temp, 20, "%02d/%02d/%02d %02d:%02d", timeinfo.tm_mday, (timeinfo.tm_mon+1), (timeinfo.tm_year-100),  timeinfo.tm_hour,timeinfo.tm_min );
    break;
  default:
    snprintf(temp, 20, "%02d/%02d/%04d %02d:%02d:%02d", timeinfo.tm_mday, (timeinfo.tm_mon+1), (1900+timeinfo.tm_year),  timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec );
    break;
  }
  return String(temp);
}

// Frame option
void saveConfigCallback() {}

// -------------------- WebSockwt functions see Data/websocket.html/js ------------------------
int numero = -1;
boolean toggleClock = true;
int utcRemote = 3600;

boolean toggleText = false;
char text[20];
boolean toggleCount = false;
char textDot[25];

// Json Variable to Hold Sensor Readings
void webSocketSend(uint8_t num) {
  String jsonString;
  JsonDocument readings;
  readings["toggleClock"] = toggleClock;
  readings["Utc"] = String(utcRemote);
  readings["toggleText"] = toggleText;
  readings["text"] = String(text);
  readings["textDot"] = String(textDot);
  serializeJson(readings, jsonString);
  frame.webSocket.sendTXT(num, jsonString);
}

void sendToDotDisplay() { 
  if(toggleClock) {
    strlcpy(textDot, getDate(0).c_str(), 25);
    display.displayText(textDot);
  }
  if (toggleText) {
    strlcpy(textDot, text, 25);
    display.displayText(textDot);
  }
  if (numero!=-1)
    webSocketSend(numero);
#ifdef DEBUG_MAIN
  Serial.printf("sendToDotDisplay numero:%d\n\r", numero);
#endif
}

void decodeJson(uint8_t num, WStype_t type, uint8_t * payload, size_t length)  {
#ifdef DEBUG_MAIN
      String strjson = "null";
      if (length>0)
        strjson = String(payload, length);
      Serial.printf("type:%d payload:%s\n\r", type, strjson.c_str());
#endif
  JsonDocument rootcfg;
  DeserializationError error = deserializeJson(rootcfg, payload, length);
  if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
  }
  toggleClock = rootcfg["toggleClock"];
  utcRemote = rootcfg["Utc"];
  toggleText = rootcfg["toggleText"];
  strlcpy(text, rootcfg["text"], 20);
  if (toggleClock && toggleText) { 
    toggleText = false,
    strlcpy(text, "", 20);
  }
#ifdef DEBUG_MAIN
  Serial.printf("decodeJson toggleClock=%d utc=%d toggleText=%d text=%s\n",toggleClock, utcRemote, toggleText, text);
#endif
}

// Receved value from Browser
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch (type) {
      case WStype_CONNECTED:
        webSocketSend(num); // Default value at startup
        break;
      case WStype_DISCONNECTED:   
        numero=-1;    
        break;
      case WStype_TEXT:
        decodeJson(num, type, payload, length);
        numero = num;
        sendToDotDisplay();
        break;
      default:
        numero = -1;
        return;
        break;
    }
}

// -------- WatchDog ----------------------------
uint32_t wdCounter = 0;
void watchdog(void *pvParameter) {
  while (1) {
    vTaskDelay(5000/portTICK_RATE_MS); // Wait 5 sec
    wdCounter++;
    if (wdCounter > 400) { // 
      // We have a problem no connection if crash or waitting 
      if (wdCounter == 401 ) {
      } else {
        // Perhapse force ??? WiFi.begin(ssid, password);
        ESP.restart(); // Restart after 5sec * 180 => 15min
        delay(2000);
      }
    }
  }
}

//  configModeCallback callback when entering into AP mode
void configModeCallback (WiFiManager *myWiFiManager) {
  #ifdef DEBUG_MAIN
    Serial.printf("Mode Access Point is running \n\r");
  #endif
  snprintf(textDot, 25, "AP:%s",frame.config.HostName);
  display.displaySmallText(textDot);
}

// setup -------------------------------------------------------------------------
void setup() {

  Serial.begin(115200);
#ifdef DEBUG_MAIN
  Serial.printf("Start setup Ver:%s\n\r",VERSION);
#endif

  // init the display
  display.begin();

  // Set pin mode  I/O Directions
  pinMode(EspLedBlue, OUTPUT);     // Led is BLUE at statup
  digitalWrite(EspLedBlue, HIGH);  // After 5 seconds blinking indicate WiFI ids OK
   // Start my WatchDog olso used to reset AP evey 15m (Some time after general cut off Wifi host is started after Eps)
  xTaskCreate(&watchdog, "wd task", 2048, NULL, 5, NULL);
  
  // Start Html framework
  frame.setup();

  // Append /websocket access html 
  frame.externalHtmlTools="Specific home page is visible at :<a class='button' href='/websocket.html'>Web Socket Demo</a>";

  // Init time / Summer time 3600=Summer 0=Winter
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); //init and get the time
  setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
  tzset();

  wifiLost = 0;

  // Start time
  getLocalTime(&timeinfo);

  for (int i=0; i<10; i++) {

    // Ip adresse
    snprintf(textDot, 25, "%s Starting...", WiFi.localIP().toString().c_str()),
    display.displaySmallText(textDot);

    // Wait little bit dot display init.
    delay(5000);
  }

  // reboot time
  rebootTime = getDate(1);

  #ifdef DEBUG_MAIN
    Serial.printf("End setup\n\r");
  #endif
}

// Main loop -----------------------------------------------------------------
void loop() {

  while (Serial.available() > 0) {
    uint8_t c = (uint8_t)Serial.read();
    if (c != 13 && c != 10 ) {
      cmd = c;
      if (cmd=='h') { Serial.println(); Serial.println("- Help info: r=reboot i=myip v=version"); cmd=' ';}
      else if (cmd=='r') { ESP.restart(); cmd=' ';}
      else if (cmd=='i') { Serial.printf("Heap:%u Mac:%s IP:%s \n\r",ESP.getFreeHeap(), WiFi.macAddress().c_str(), WiFi.localIP().toString().c_str() ); cmd=' ' ;}
      else if (cmd=='v') { Serial.printf("Stop serial: %s \n\r",VERSION); cmd=' '; }
    }
  }

  // Call Html_frame loop
  frame.loop();

  // Is alive executed every 1 sec.
  if ( millis() - previousMillis > 1000L) {
    previousMillis = millis();

    // Update led and time
    if (utcRemote != daylightOffset_sec){
      daylightOffset_sec = utcRemote;
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); //init 
      setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
      tzset();
    }

    getLocalTime(&timeinfo);
    digitalWrite(EspLedBlue, !digitalRead(EspLedBlue));
    
    int wifistat = WiFi.status();
    // if wifi is down, try reconnecting every 60 seconds
    if (wifistat != WL_CONNECTED) {
      wifiLost++;
      if (wifiLost==10) {
        #ifdef DEBUG_MAIN
          Serial.printf("%s -WiFi Lost:%s wifiLost:%d sec. localIP:%s", getDate().c_str(), frame.wifiStatus(wifistat), wifiLost, WiFi.localIP().toString().c_str() );
        #endif
      }
      if (wifiLost == 50) {
        #ifdef DEBUG_MAIN
          Serial.printf("%s -WiFi disconnect OK after 50s (%s).",getDate().c_str(), frame.wifiStatus(wifistat));
        #endif
        WiFi.disconnect();
      }
      if (wifiLost == 60) {
        if (WiFi.reconnect()) {
          wifistat = WL_CONNECTED;
        }
      }
    } else {
      wdCounter = 0;
      wifiLost = 0;
    }

    // every day. minute 
    boolean newMinute = ( (timeinfo.tm_sec == 00));
    if (newMinute ) {
      sendToDotDisplay();
      if (timeinfo.tm_min == 00)
        display.invert();
    }
    // ....
 
  } // End second

} // End loop
