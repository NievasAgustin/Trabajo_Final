#include "WiFi.h"
#include <HTTPClient.h>
#include "time.h"
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;
// WiFi credentials
const char* ssid = "ESP_DATA";         // change SSID
const char* password = "esp32iot";    // change password
// Google script ID and required credentials
//String GOOGLE_SCRIPT_ID = "script ID";    // change Gscript ID
String GOOGLE_SCRIPT_ID = "script ID";
float arraydate[200];
float arraypotencia[200];
float arraytension[200];
float arraydutycycle[200];
float dutycyclefinal[200];
  int i = 0;
void sendtoexcel(float arraydate[], float arraypotencia[],float arraytension[], float arraydutycycle[], float dutycyclefinal[]);
void setup() {
  i=0;
  Serial.begin(115200);
  // connect to WiFi
  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}
void loop() {

    Serial.print(i);
  if (i < 200) {
    struct timeval tv;
    unsigned long seconds;
    unsigned long milliseconds;
    unsigned long timeinsecandmil;
    gettimeofday(&tv, NULL);
    milliseconds = tv.tv_usec / 1000;

    arraydate[i] = milliseconds;
    arraypotencia[i] = i ;
    arraytension[i] = 2*i ;
    arraydutycycle[i] = 3*i ;
    dutycyclefinal[i] = 4*i ;
    i++;
    delay(100);
  } else {
    
    Serial.print(".");
    i = 0;
    sendtoexcel(arraydate, arraypotencia,arraytension, arraydutycycle, dutycyclefinal);
  }
}



void sendtoexcel(float arraydate[], float arraypotencia[],float arraytension[], float arraydutycycle[], float dutycyclefinal[]) {
  int count;
  for (count = 0; count < 200; count = count + 1) {
    
    Serial.print(count);
    String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + "tiempo=" + String(arraydate[count]) + "&outpotencia=" + String(arraypotencia[count]) + "&dutycycle=" + String(arraydutycycle[count]) + "&dutycyclefinal=" + String(dutycyclefinal[count]) + "&outtension=" + String(arraytension[count]); //String(count)
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    http.end();
  }
}















#include "WiFi.h"
#include <HTTPClient.h>
#include "time.h"
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;
// WiFi credentials
const char* ssid = "ESP_DATA";         // change SSID
const char* password = "esp32iot";    // change password
// Google script ID and required credentials
//String GOOGLE_SCRIPT_ID = "AKfycbzQGON6v4WCPH79F7UuJsrIrY-VadOIdUCAyvORTrJt49MYy0p9r8U0IaLoxIeUCdJ-";    // change Gscript ID
String GOOGLE_SCRIPT_ID = "AKfycbyLk-wi6iUJtESNdCLKxU4AVB5EBhn6_Iax-qnjLv3kkVBruvVLCQ9oxhlRNvETfYSA";
float arraydate[200];
float arraysesor[200];
  int i = 0;
void sendtoexcel(float arraydate[], float arraysensor[]);
void setup() {
  i=0;
  Serial.begin(115200);
  // connect to WiFi
  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}
void loop() {

    Serial.print(i);
  if (i < 200) {
    struct timeval tv;
    unsigned long seconds;
    unsigned long milliseconds;
    unsigned long timeinsecandmil;
    gettimeofday(&tv, NULL);
    milliseconds = tv.tv_usec / 1000;

    arraydate[i] = milliseconds;
    arraysesor[i] = i ;
    i++;
    delay(100);
  } else {
    
    Serial.print(".");
    i = 0;
    sendtoexcel(arraydate, arraysesor);
  }
}



void sendtoexcel(float arraydate[], float arraysensor[]) {
  int count;
  float outtension;
  float dutycyclefinal;
  for (count = 0; count < 200; count = count + 1) {
    
    Serial.print(count);
    outtension = arraysensor[count] / 4;
    dutycyclefinal = count * 1.5;
    String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + "tiempo=" + String(arraydate[count]) + "&outpotencia=" + String(arraysensor[count]) + "&dutycycle=" + String(count) + "&dutycyclefinal=" + String(dutycyclefinal) + "&outtension=" + String(outtension); //String(count)
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    http.end();
  }
}