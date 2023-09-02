#include "WiFi.h"
#include <HTTPClient.h>
#include "time.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"
#include "driver/adc.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_reg.h"
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

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;
// WiFi credentials
const char* ssid = "ESP_DATA";         // change SSID
const char* password = "esp32iot";    // change password
// Google script ID and required credentials
String GOOGLE_SCRIPT_ID = "script ID";

void sendtoexcel(float arraydate[], float arraysensor[]);

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
  pinMode(Botton_Control_1, INPUT_PULLUP);
  setuppwm();
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
  float arraytiempo[200];
  float arrayoutpotencia[200];
  float arraydutycycle[200];
  float arraydutycyclefinal;
  float arrayouttension[200];
  float outpotenciaviejo = 0.0;
  float ciclodetrabajo = 50;
  float outcorriente = 0.0;
  float outtension = 0.0;
  float outpotencia = 0.0;
  float maxpotencia = 0.0;
  float ciclodetrabajofinal = 0.0;
  float ciclodetrabajoneg = 100;
  int oscilationstop = 0;
  int methode = 0;
  int cyclecounter = 0;
  float funcion[3];
  float funcion1[3];
  float funcion2[3];
  float funcion3[3];
  float resetciclodetrabajo = 50;
  float limitinf = 50;
  float limitinf1 = 50;
  float limitinf2 = 66;
  float limitinf3 = 82;
  int i = 0;
  int i1 = 0;
  int i2 = 0;
  int i3 = 0;
  int X = 0;
  int X1 = 0;
  int X2 = 0;
  int X3 = 0;
  float deltavariable = 10;
  float deltavariable1 = 5;
  float deltavariable2 = 5;
  float deltavariable3 = 5;
  float XMAX = 0;
  float cien = 100;
  float cero = 0.0;
  float potmaxcalculada = 0.0;
  int newmethode = 0;
  struct timeval tv;
  unsigned long seconds;
  unsigned long milliseconds;
  unsigned long timeinsecandmil;
  if (newmethode != methode)
  {
    if (methode == 6)
    {
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, 95);
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, 95);
    } else {
      deltavariable = 10;
      deltavariable1 = 5;
      deltavariable2 = 5;
      deltavariable3 = 5;
      XMAX = 0;
      potmaxcalculada = 0.0;
      i = 0;
      i1 = 0;
      i2 = 0;
      i3 = 0;
      X = 0;
      X1 = 0;
      X2 = 0;
      X3 = 0;
      limitinf = 50;
      limitinf1 = 50;
      limitinf2 = 66;
      limitinf3 = 82;
      oscilationstop = 0;
      cyclecounter = 0;
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, resetciclodetrabajo);
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, resetciclodetrabajo);
    }
    outcorriente =  analogReadMilliVolts(ADC_I_OUT_PIN);
    outtension   =  analogReadMilliVolts(ADC_V_OUT_PIN);
    outpotencia  =  outtension * outcorriente;
    gettimeofday(&tv, NULL);
    milliseconds = tv.tv_usec / 1000;

    arraytiempo[i] = milliseconds;
    arrayoutpotencia[i] = outpotencia ;
    

    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, duty0);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, duty0);
    delay(100);
  }
  sendtoexcel(arraytiempo, arrayoutpotencia, arraydutycycle, arrayouttension, arraydutycyclefinal);
}



void sendtoexcel(float tiempo[], float potencia[], float duty[], float tension[], float dutyfinal) {
  int count;
  for (count = 0; count < 200; count = count + 1) {
    String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + "tiempo=" + String(tiempo[count]) + "&outpotencia=" + String(potencia[count]) + "&dutycycle=" + String(duty[count])  + "&outtension=" + String(tension[count])+ "&dutycyclefinal=" + String(dutyfinal); //String(count)
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    http.end();
  }
}