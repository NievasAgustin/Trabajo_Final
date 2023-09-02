#include "WiFi.h"
#include <HTTPClient.h>
#include "time.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"
#include "driver/adc.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_reg.h"
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif
#define GPIO_PWM0A_OUT 19   //HV
#define GPIO_PWM0B_OUT 18   //HV
#ifndef Botton_Control_1
#define Botton_Control_1 36
#endif
#ifndef ADC_V_OUT_PIN
#define ADC_V_OUT_PIN 0
#endif
#ifndef ADC_I_OUT_PIN
#define ADC_I_OUT_PIN 15
#endif
int freq0 = 100000;
float duty0 =   50;
unsigned long int res = 100000000;
int i = 1;
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;
// WiFi credentials
const char* ssid = "ESP_DATA";         // change SSID
const char* password = "esp32iot";    // change password
// Google script ID and required credentials
String GOOGLE_SCRIPT_ID = "script ID";

//void sendtoexcel(float arraydate[], float arraysensor[]);
void sendtoexcel(float tiempo[], float potencia[], float duty[], float tension[], float dutyfinal);

void setuppwm() {
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, GPIO_PWM0A_OUT);
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, GPIO_PWM0B_OUT);

  mcpwm_group_set_resolution(MCPWM_UNIT_0, res);

  mcpwm_config_t pwm_config0;
  pwm_config0.frequency = freq0;  //frequency
  pwm_config0.cmpr_a = 0;      //duty cycle of PWMxA = 50.0%
  pwm_config0.cmpr_b = 0;      //duty cycle of PWMxB = 50.0%
  pwm_config0.counter_mode = MCPWM_UP_COUNTER; // Up-down counter (triangle wave)
  pwm_config0.duty_mode = MCPWM_DUTY_MODE_0; // Active HIGH

  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config0);
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config0);

  mcpwm_set_timer_sync_output(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_SWSYNC_SOURCE_TEZ);

  mcpwm_sync_enable(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_SELECT_TIMER0_SYNC, 0);

  mcpwm_sync_enable(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_SELECT_TIMER0_SYNC, 500);


  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, duty0);
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, duty0);

  mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, MCPWM_DUTY_MODE_0);

  mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, MCPWM_DUTY_MODE_1);

}


void setup() {
  Serial.begin(115200);
  //  pinMode(Botton_Control_1, INPUT_PULLUP);
  setuppwm();

  Serial.println(i);
  i = 0;

  Serial.println(i);
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
  Serial.println(i);
  float arraytiempo[200];
  float arrayoutpotencia[200] = {0};
  float arraydutycycle[200] = {0};
  float dutyfinal;
  float arrayouttension[200] = {0};
  struct timeval tv;
  unsigned long seconds;
  unsigned long milliseconds;
  unsigned long timeinsecandmil;
  float limitinf1 = 68;
  float limitinf3 = 80;
  Serial.println(i);
  i++;
  gettimeofday(&tv, NULL);
  milliseconds = tv.tv_usec / 1000;
  arraytiempo[i] = milliseconds;
  arrayoutpotencia[i] = (limitinf1 + (i * limitinf3)) - (i * limitinf3);
  arraydutycycle[i] = i;
  arrayouttension[i] = arrayoutpotencia[i] / 4;
  if (i > 20) {
    i = 0;
    dutyfinal = arraydutycycle[10];
    Serial.println(i);
    Serial.println("funcion send to excel");
    sendtoexcel(arraytiempo, arrayoutpotencia, arraydutycycle, arrayouttension, dutyfinal);
  }
  delay(100);
}




void sendtoexcel(float arraytiempo[], float arrayoutpotencia[], float arraydutycycle[], float arrayouttension[], float dutyfinal) {
  Serial.println("Entro a funcion");
  int count = 0;
  for (count = 0; count < 200; count = count + 1) {
    Serial.println("Entro a for");
    Serial.println(count);
    String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + "tiempo=" + String(arraytiempo[count]) +
                      "&outpotencia=" + String(arrayoutpotencia[count]) + "&dutycycle=" + String(arraydutycycle[count])  + "&outtension=" + String(arrayouttension[count]) + "&dutycyclefinal=" + String(dutyfinal);
    Serial.println("Mando algo");                  
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    http.end();
  }
}