#include "WiFi.h"
#include <HTTPClient.h>
#include "time.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_reg.h"
#define GPIO_PWM0A_OUT 19
#define GPIO_PWM0B_OUT 18

#ifndef ADC_I_OUT_PIN
#define ADC_I_OUT_PIN 35
#endif
#ifndef ADC_V_OUT_PIN
#define ADC_V_OUT_PIN 34
#endif

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;
// WiFi credentials
const char* ssid = "UCC-CAMPUS";         // change SSID
const char* password = "";    // change password
// Google script ID and required credentials
//String GOOGLE_SCRIPT_ID = "script ID";    // change Gscript ID
// Porque aca no? String GOOGLE_SCRIPT_ID = "script ID";

float arraydate[1400];
float arraypotencia[1400];
float arraytension[1400];
float arraydutycycle[1400];
float dutycyclefinal[1400];

int timecounter = 0;
int oldtimecounter = 0;
int timecounterdiference = 0;

int numeroenvectores = 0;

float outpotenciaviejo = 0.0;
float ciclodetrabajo = 50;
int outcorriente = 0.0;
int outtension = 0.0;
int outpotencia = 0.0;
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
float X = 0;
float X1 = 0;
float X2 = 0;
float X3 = 0;
float deltavariable = 10;
float deltavariable1 = 5;
float deltavariable2 = 5;
float deltavariable3 = 5;
float XMAX = 0;
float cien = 100;
float cero = 0.0;
float potmaxcalculada = 0.0;
int newmethode = 0;

void sendtoexcel(float arraydate[], float arraypotencia[], float arraytension[], float arraydutycycle[], float dutycyclefinal[]);

void setuppwm() {

  int freq0 = 100000;
  unsigned long int res = 100000000;

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


  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, 0);
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, 0);

  mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, MCPWM_DUTY_MODE_0);

  mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, MCPWM_DUTY_MODE_1);

}
void setup() {
  setuppwm();

  i = 0;
  timecounter = 1;
  oldtimecounter = 0;
  timecounterdiference = 0;

  numeroenvectores = 0;

  ciclodetrabajo = 50;
  outpotenciaviejo = 0.0;
  outcorriente = 0.0;
  outtension = 0.0;
  outpotencia = 0.0;
  maxpotencia = 0.0;
  ciclodetrabajofinal = 0.0;
  ciclodetrabajoneg = 100;
  oscilationstop = 0;
  methode = 0;
  cyclecounter = 0;
  funcion[3];
  funcion1[3];
  funcion2[3];
  funcion3[3];
  resetciclodetrabajo = 50;
  limitinf = 50;
  limitinf1 = 50;
  limitinf2 = 66;
  limitinf3 = 82;
  i = 0;
  i1 = 0;
  i2 = 0;
  i3 = 0;
  X = 0;
  X1 = 0;
  X2 = 0;
  X3 = 0;
  deltavariable = 10;
  deltavariable1 = 5;
  deltavariable2 = 5;
  deltavariable3 = 5;
  XMAX = 0;
  cien = 100;
  cero = 0.0;
  potmaxcalculada = 0.0;
  newmethode = 0;

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
  if (newmethode != methode)
  {
    if (methode == 4)
    {
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, 95);
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, 95);
    } else {
      if (methode == 5) {
        sendtoexcel(arraydate, arraypotencia, arraytension, arraydutycycle, dutycyclefinal);//16 minutos en realizar el print
        methode = 0;
        oldtimecounter = 0;
        timecounter = 0;

        ciclodetrabajo = 50;
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

      } else {
        ciclodetrabajo = 50;
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
    }
  }
  newmethode = methode;
  outcorriente =  2500-analogReadMilliVolts(ADC_I_OUT_PIN);  // analogReadMilliVolts  ADC_I_OUT_PIN
  outtension   =  analogReadMilliVolts(ADC_V_OUT_PIN); //  ADC_V_OUT_PIN
  outpotencia  =  outtension * outcorriente;
  struct timeval tv;
  unsigned long seconds;
  unsigned long milliseconds;
  unsigned long timeinsecandmil;
  gettimeofday(&tv, NULL);
  milliseconds = tv.tv_usec / 1000;

  switch (methode) {
    case 0:
      if (cyclecounter < 200)
      {
        ciclodetrabajo = ciclodetrabajo + 0.25;
        cyclecounter = cyclecounter + 1;
        ciclodetrabajofinal = ciclodetrabajo;
      } else {
        cyclecounter=0;
        ciclodetrabajo=50;
        break;
      }
      break;
    case 1:
      if (outpotencia > outpotenciaviejo || ciclodetrabajo < 50)
      {
        ciclodetrabajo = ciclodetrabajo + 0.5;
        //inpotenciaviejo=inpotencia;
        outpotenciaviejo = outpotencia;
        maxpotencia = outpotencia;
      } else {
        if ( outpotencia == maxpotencia || oscilationstop > 1000)
        {
          ciclodetrabajofinal = ciclodetrabajo;
          outpotenciaviejo = outpotencia;
          oscilationstop = 0;
        } else {
          ciclodetrabajo = ciclodetrabajo - 0.5;
          outpotenciaviejo = outpotencia;
          oscilationstop = oscilationstop + 1;
        }
      }
      break;
    case 2:
      if (deltavariable > 0, 3 || limitinf > 0 || limitinf < 100) {
        if (i < 3)
        {
          X = limitinf + (deltavariable * i);
          funcion[i] = outpotencia;
          i = i + 1;
        } else {
          if (funcion[0] < funcion[1] && funcion[1] < funcion[2])
          {
            limitinf = limitinf + (deltavariable * 2);
          } else {
            if (funcion[0] < funcion[1] && funcion[1] > funcion[2])
            {
              limitinf = limitinf + deltavariable;
              deltavariable = deltavariable / 2;
            } else {
              limitinf = limitinf - deltavariable;
            }
          }
          i = 0;
        }
        if (X < 5)
        {
          ciclodetrabajo = 5;
        } else {
          if (X > 95)
          {
            ciclodetrabajo = 100;
          } else {
            ciclodetrabajo = X;
          }
        }
      } else {
        ciclodetrabajofinal = X;
        if (funcion[1] < (outpotencia * 1.1) || funcion[1] > (outpotencia * 0.9))
        {
          deltavariable = 10;
          limitinf = limitinf - 10;
        }
        //xQueueSend(finaldutycycleQueue, &X, portMAX_DELAY);//hacer que solo mande queue cuando X es distinto al anterior X
        ciclodetrabajo = X;
      }
      break;
    case 3:
      if (deltavariable1 > 0, 3 && limitinf1 < 66) { //lower cycle serch
        if (i1 < 3)
        {
          X1 = limitinf1 + (deltavariable1 * i1);
          funcion1[i1] = outpotencia;
          i1 = i1 + 1;
        } else {
          if (funcion1[0] < funcion1[1] && funcion1[1] < funcion1[2])
          {
            limitinf1 = limitinf1 + (deltavariable1 * 2);
          } else {
            if (funcion1[0] < funcion1[1] && funcion1[1] > funcion1[2])
            {
              limitinf1 = limitinf1 + deltavariable1;
              deltavariable1 = deltavariable1 / 2;
            } else {
              limitinf1 = limitinf1 - deltavariable1;
            }
          }
          i1 = 0;
        }
        ciclodetrabajo = X1;
      } else {
        if (deltavariable2 > 0, 3 && limitinf1 < 82) { //medium cycle search
          if (i2 < 3)
          {
            X2 = limitinf2 + (deltavariable2 * i2);
            funcion1[i2] = outpotencia;
            i2 = i2 + 1;
          } else {
            if (funcion2[0] < funcion2[1] && funcion2[1] < funcion2[2])
            {
              limitinf2 = limitinf2 + (deltavariable2 * 2);
            } else {
              if (funcion2[0] < funcion2[1] && funcion2[1] > funcion2[2])
              {
                limitinf2 = limitinf2 + deltavariable2;
                deltavariable2 = deltavariable2 / 2;
              } else {
                limitinf2 = limitinf2 - deltavariable2;
              }
            }
            i2 = 0;
          }
          ciclodetrabajo = X2;
        } else {
          if (deltavariable3 > 0, 3) { //top cycle search
            if (i3 < 3)
            {
              X3 = limitinf3 + (deltavariable3 * i3);
              funcion1[i3] = outpotencia;
              i3 = i3 + 1;
            } else {
              if (funcion3[0] < funcion3[1] && funcion3[1] < funcion3[2])
              {
                limitinf3 = limitinf3 + (deltavariable3 * 2);
              } else {
                if (funcion3[0] < funcion3[1] && funcion3[1] > funcion3[2])
                {
                  limitinf3 = limitinf3 + deltavariable3;
                  deltavariable3 = deltavariable3 / 2;
                } else {
                  limitinf3 = limitinf3 - deltavariable3;
                }
              }
              i3 = 0;
            }
            ciclodetrabajo = X3;
          }
        }
        if (funcion1[1] < funcion2[1])
        {
          if (funcion2[1] < funcion3[1])
          {
            XMAX = X3;
            ciclodetrabajo = X3;
            potmaxcalculada = funcion3[1];
          } else {
            XMAX = X2;
            ciclodetrabajo = X2;
            potmaxcalculada = funcion2[1];
          }
        } else {
          XMAX = X1;
          ciclodetrabajo = X1;
          potmaxcalculada = funcion1[1];
        }
        if (potmaxcalculada < (outpotencia * 1.1) || potmaxcalculada > (outpotencia * 0.9)) //restart of mesuarment of output power changes ~10% over time with same duty cycle
        {
          deltavariable1 = 2.5;
          deltavariable2 = 2.5;
          deltavariable3 = 2.5;
          limitinf1 = limitinf1 - 10;
          limitinf1 = limitinf1 - 10;
          limitinf1 = limitinf1 - 10;
        }

        // xQueueSend(finaldutycycleQueue, &XMAX, portMAX_DELAY);
        ciclodetrabajofinal = XMAX;
      }
      break;
    case 4:
      if (outpotencia > outpotenciaviejo)
      {
        ciclodetrabajo = ciclodetrabajo - 0.5;
        outpotenciaviejo = outpotencia;
        maxpotencia = outpotencia;
      } else {
        if ( outpotencia == maxpotencia || oscilationstop > 1000)
        {
          ciclodetrabajofinal = ciclodetrabajo;
          outpotenciaviejo = outpotencia;
          oscilationstop = 0;
        } else {
          ciclodetrabajo = ciclodetrabajo + 0.5;
          outpotenciaviejo = outpotencia;
          oscilationstop = oscilationstop + 1;
        }
      }
      break;
    default:
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, 0);
      mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, 0);
      break;
  }

  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, ciclodetrabajo);
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, ciclodetrabajo);
  arraydate[numeroenvectores] = milliseconds;
  arraydutycycle[numeroenvectores] = ciclodetrabajo;
  dutycyclefinal[numeroenvectores] = ciclodetrabajofinal;
  arraytension[numeroenvectores] = outtension;
  arraypotencia[numeroenvectores] = outpotencia;



  timecounterdiference = timecounter - oldtimecounter;
  if (timecounterdiference > 400) {
    oldtimecounter = timecounter;
    methode = methode + 1;
  }
  numeroenvectores = numeroenvectores + 1;
  delay(100);
  timecounter = timecounter + 1;
}


//  arraydate[i] = milliseconds;
//  arraypotencia[i] = i ;
//  arraytension[i] = 2 * i ;
//  arraydutycycle[i] = 3 * i ;
//  dutycyclefinal[i] = 1.5 * i ;


void sendtoexcel(float arraydate[], float arraypotencia[], float arraytension[], float arraydutycycle[], float dutycyclefinal[]) {
  int count;
  String GOOGLE_SCRIPT_ID = "script ID";
   while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  for (count = 0; count < 1400; count = count + 1) {
    String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + "tiempo=" + arraydate[count] + "&outpotencia=" + arraypotencia[count] + "&dutycycle=" + arraydutycycle[count] + "&dutycyclefinal=" + dutycyclefinal[count] + "&outtension=" + arraytension[count]; //String(count)
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
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_reg.h"
#define GPIO_PWM0A_OUT 19
#define GPIO_PWM0B_OUT 18

#ifndef ADC_I_OUT_PIN
#define ADC_I_OUT_PIN 35
#endif
#ifndef ADC_V_OUT_PIN
#define ADC_V_OUT_PIN 34
#endif

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;
// WiFi credentials
const char* ssid = "UCC-CAMPUS";         // change SSID ESP_DATA
const char* password = "";    // change password  esp32iot
// Google script ID and required credentials
//String GOOGLE_SCRIPT_ID = "script ID";    // change Gscript ID
// Porque aca no? String GOOGLE_SCRIPT_ID = "script ID";

int arraydate[1400];
int arraypotencia[1400];
int arraytension[1400];
float arraydutycycle[1400];
float dutycyclefinal[1400];
int outcorrientefilter[10];
int outtensionfilter[10];
int timecounter = 0;
int oldtimecounter = 0;
int timecounterdiference = 0;

int numeroenvectores = 0;

float ciclodetrabajo = 0;
int outcorriente = 0.0;
int outtension = 0.0;
int outpotencia = 0.0;
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

int pasofiltro = 10;

int i = 0;
int i1 = 0;
int i2 = 0;
int i3 = 0;
float X = 0;
float X1 = 0;
float X2 = 0;
float X3 = 0;
float deltavariable = 10;
float deltavariable1 = 5;
float deltavariable2 = 5;
float deltavariable3 = 5;
float XMAX = 0;
float cien = 100;
float cero = 0.0;
float potmaxcalculada = 0.0;
int newmethode = 0;
int outpotenciaviejo = 0;
void sendtoexcel(int arraydate[], int arraypotencia[], int arraytension[], float arraydutycycle[], float dutycyclefinal[]);

void setuppwm() {

  int freq0 = 100000;
  unsigned long int res = 100000000;

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


  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, 0);
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, 0);

  mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, MCPWM_DUTY_MODE_0);

  mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, MCPWM_DUTY_MODE_1);

}
void setup() {
  setuppwm();
  outpotenciaviejo = 0;
  i = 0;
  timecounter = 1;
  oldtimecounter = 0;
  timecounterdiference = 0;

  numeroenvectores = 0;

  pasofiltro = 10;

  ciclodetrabajo = 50;
  outpotenciaviejo = 0.0;
  outcorriente = 0.0;
  outtension = 0.0;
  outpotencia = 0.0;
  maxpotencia = 0.0;
  ciclodetrabajofinal = 0.0;
  ciclodetrabajoneg = 100;
  oscilationstop = 0;
  methode = 0;
  cyclecounter = 0;
  funcion[3];
  funcion1[3];
  funcion2[3];
  funcion3[3];
  resetciclodetrabajo = 50;
  limitinf = 50;
  limitinf1 = 50;
  limitinf2 = 66;
  limitinf3 = 82;
  i = 0;
  i1 = 0;
  i2 = 0;
  i3 = 0;
  X = 0;
  X1 = 0;
  X2 = 0;
  X3 = 0;
  deltavariable = 10;
  deltavariable1 = 5;
  deltavariable2 = 5;
  deltavariable3 = 5;
  XMAX = 0;
  cien = 100;
  cero = 0.0;
  potmaxcalculada = 0.0;
  newmethode = 0;

//  outcorrientefilter = 0;
//  outtensionfilter = 0;
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
  for (int filtro = 0; filtro < pasofiltro; ++filtro)
  {
    outcorrientefilter[filtro] =  2500 - analogReadMilliVolts(ADC_I_OUT_PIN); // analogReadMilliVolts  ADC_I_OUT_PIN
    outtensionfilter[filtro]   =  analogReadMilliVolts(ADC_V_OUT_PIN); //  ADC_V_OUT_PIN
    Serial.println("Corriente en filtro");
    Serial.println(outcorrientefilter[filtro]);
    Serial.println("Tension en filtro");
    Serial.println(outtensionfilter[filtro]);
    delay(50);
  }
  for (int sumador = 0; sumador < pasofiltro; ++sumador)
  {
    outcorriente =  outtensionfilter[sumador] ;
    outtension  =  outtensionfilter[sumador];
  
  }
  Serial.println("Corriente sin dividir");
  Serial.println(outcorriente);
  Serial.println("Tension sin dividir");
  Serial.println(outtension);
  outcorriente = ((outcorriente) / 10);//pasofiltro
  Serial.println("Corriente dividido");
  Serial.println(outcorriente);
  outtension = ((outtension) / 10);
  Serial.println("Tension dividido");
  Serial.println(outtension);
  outpotencia  =  outtension * outcorriente;
 // outcorrientefilter = 0;
 // outtensionfilter = 0;
  struct timeval tv;
  unsigned long seconds;
  unsigned long milliseconds;
  unsigned long timeinsecandmil;
  gettimeofday(&tv, NULL);
  milliseconds = tv.tv_usec / 1000;


  if (cyclecounter < 400 && ciclodetrabajo < 101)
  {
    ciclodetrabajo = ciclodetrabajo + 0.25;
    cyclecounter = cyclecounter + 1;
    ciclodetrabajofinal = ciclodetrabajo;
  } else {
    Serial.println("Salio del if contador");
    cyclecounter = 0;
    ciclodetrabajo = 0;
  }

  if (outpotenciaviejo < outpotencia) {
    outpotenciaviejo = outpotencia;
    ciclodetrabajofinal = ciclodetrabajo;
  }


  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, ciclodetrabajo);
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, ciclodetrabajo);
  arraydate[numeroenvectores] = milliseconds;
  arraydutycycle[numeroenvectores] = ciclodetrabajo;
  dutycyclefinal[numeroenvectores] = ciclodetrabajofinal;
  arraytension[numeroenvectores] = outtension;
  arraypotencia[numeroenvectores] = outpotencia;



  timecounterdiference = timecounter - oldtimecounter;
  if (timecounterdiference > 400) {
    oldtimecounter = timecounter;
    Serial.println("Llamando funcion excel");
    sendtoexcel(arraydate, arraypotencia, arraytension, arraydutycycle, dutycyclefinal);
  }
  numeroenvectores = numeroenvectores + 1;
  delay(100);
  timecounter = timecounter + 1;
}


//  arraydate[i] = milliseconds;
//  arraypotencia[i] = i ;
//  arraytension[i] = 2 * i ;
//  arraydutycycle[i] = 3 * i ;
//  dutycyclefinal[i] = 1.5 * i ;


void sendtoexcel(int arraydate[], int arraypotencia[], int arraytension[], float arraydutycycle[], float dutycyclefinal[]) {
  int count;
  String GOOGLE_SCRIPT_ID = "script ID";
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  for (count = 0; count < 400; count = count + 1) {
    String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + "tiempo=" + arraydate[count] + "&outpotencia=" + arraypotencia[count] + "&dutycycle=" + arraydutycycle[count] + "&dutycyclefinal=" + dutycyclefinal[count] + "&outtension=" + arraytension[count]; //String(count)
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    Serial.println(urlFinal);
    http.end();
  }
}





-----------------------------------------------------------------------------------------------------------------------------------------------
#include "WiFi.h"
#include <HTTPClient.h>
#include "time.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_reg.h"
#define GPIO_PWM0A_OUT 19
#define GPIO_PWM0B_OUT 18

#ifndef ADC_I_OUT_PIN
#define ADC_I_OUT_PIN 35
#endif
#ifndef ADC_V_OUT_PIN
#define ADC_V_OUT_PIN 34
#endif

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;
// WiFi credentials
const char* ssid = "UCC-CAMPUS";         // change SSID ESP_DATA
const char* password = "";    // change password  esp32iot
// Google script ID and required credentials
//String GOOGLE_SCRIPT_ID = "script ID";    // change Gscript ID
// Porque aca no? String GOOGLE_SCRIPT_ID = "script ID";

float arraydate[1400];
float arraypotencia[1400];
float arraytension[1400];
float arraydutycycle[1400];
float dutycyclefinal[1400];

int timecounter = 0;
int oldtimecounter = 0;
int timecounterdiference = 0;

int numeroenvectores = 0;

float ciclodetrabajo = 50;
int outcorriente = 0.0;
int outtension = 0.0;
int outpotencia = 0.0;
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


int outcorrientefilter;
int outtensionfilter;
int pasofiltro = 40;


int i = 0;
int i1 = 0;
int i2 = 0;
int i3 = 0;
float X = 0;
float X1 = 0;
float X2 = 0;
float X3 = 0;
float deltavariable = 10;
float deltavariable1 = 5;
float deltavariable2 = 5;
float deltavariable3 = 5;
float XMAX = 0;
float cien = 100;
float cero = 0.0;
float potmaxcalculada = 0.0;
int newmethode = 0;
int outpotenciaviejo = 0;
void sendtoexcel(float arraydate[], float arraypotencia[], float arraytension[], float arraydutycycle[], float dutycyclefinal[]);

void setuppwm() {

  int freq0 = 100000;
  unsigned long int res = 100000000;

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


  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, 50);
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, 50);

  mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, MCPWM_DUTY_MODE_0);

  mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, MCPWM_DUTY_MODE_1);

}
void setup() {
  setuppwm();
  outpotenciaviejo = 0;
  i = 0;
  timecounter = 1;
  oldtimecounter = 0;
  timecounterdiference = 0;

  numeroenvectores = 0;


  pasofiltro = 40;

  ciclodetrabajo = 50;
  outcorriente = 0;
  outtension = 0;
  outpotencia = 0;
  maxpotencia = 0;
  ciclodetrabajofinal = 0.0;
  ciclodetrabajoneg = 100;
  oscilationstop = 0;
  methode = 0;
  cyclecounter = 0;
  funcion[3];
  funcion1[3];
  funcion2[3];
  funcion3[3];
  resetciclodetrabajo = 50;
  limitinf = 50;
  limitinf1 = 50;
  limitinf2 = 66;
  limitinf3 = 82;
  i = 0;
  i1 = 0;
  i2 = 0;
  i3 = 0;
  X = 0;
  X1 = 0;
  X2 = 0;
  X3 = 0;
  deltavariable = 10;
  deltavariable1 = 5;
  deltavariable2 = 5;
  deltavariable3 = 5;
  XMAX = 0;
  cien = 100;
  cero = 0.0;
  potmaxcalculada = 0.0;
  newmethode = 0;

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {

  for (int filtro = 0; filtro < pasofiltro; ++filtro)
  {
    outcorrientefilter =  outcorrientefilter + 2500 - analogReadMilliVolts(ADC_I_OUT_PIN); // analogReadMilliVolts  ADC_I_OUT_PIN
    outtensionfilter   =  outtensionfilter + analogReadMilliVolts(ADC_V_OUT_PIN); //  ADC_V_OUT_PIN
    delay(25);
  }

  outcorriente = ((outcorrientefilter) / pasofiltro);
  outtension = ((outtensionfilter) / pasofiltro);
  outpotencia  =  outtension * outcorriente;
  outcorrientefilter = 0;
  outtensionfilter = 0;
  struct timeval tv;
  unsigned long seconds;
  unsigned long milliseconds;
  unsigned long timeinsecandmil;
  gettimeofday(&tv, NULL);
  milliseconds = tv.tv_usec / 1000;

  if (outpotencia > (1.1 * outpotenciaviejo))
  {
    ciclodetrabajo = ciclodetrabajo + 0.5;
    //inpotenciaviejo=inpotencia;
    outpotenciaviejo = outpotencia;
    maxpotencia = outpotencia;
  } else {
    if ( outpotencia > (0.9 * maxpotencia) && outpotencia < (1.1 * maxpotencia) || oscilationstop > 1000)
    {
      ciclodetrabajofinal = ciclodetrabajo;
      outpotenciaviejo = outpotencia;
      oscilationstop = 0;
    } else {
      ciclodetrabajo = ciclodetrabajo - 0.5;
      outpotenciaviejo = outpotencia;
      oscilationstop = oscilationstop + 1;
    }
  }


  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, ciclodetrabajo);
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, ciclodetrabajo);
  arraydate[numeroenvectores] = milliseconds;
  arraydutycycle[numeroenvectores] = ciclodetrabajo;
  dutycyclefinal[numeroenvectores] = ciclodetrabajofinal;
  arraytension[numeroenvectores] = outtension;
  arraypotencia[numeroenvectores] = outpotencia;



  timecounterdiference = timecounter - oldtimecounter;
  if (timecounterdiference > 400) {
    oldtimecounter = timecounter;
    sendtoexcel(arraydate, arraypotencia, arraytension, arraydutycycle, dutycyclefinal);
  }
  if (numeroenvectores>400)
  {
    numeroenvectores=0;
  }
  numeroenvectores = numeroenvectores + 1;
  delay(100);
  timecounter = timecounter + 1;
}



void sendtoexcel(float arraydate[], float arraypotencia[], float arraytension[], float arraydutycycle[], float dutycyclefinal[]) {
  int count;
  String GOOGLE_SCRIPT_ID = "script ID";
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
  for (count = 0; count < 400; count = count + 1) {
    String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + "tiempo=" + arraydate[count] + "&outpotencia=" + arraypotencia[count] + "&dutycycle=" + arraydutycycle[count] + "&dutycyclefinal=" + dutycyclefinal[count] + "&outtension=" + arraytension[count]; //String(count)
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    http.end();
  }
}