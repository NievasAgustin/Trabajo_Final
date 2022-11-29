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
//String GOOGLE_SCRIPT_ID = "your_google_script_ID";    // change Gscript ID


int arraydate[400];
int arraypotencia[400];
int arraytension[400];
float arraydutycycle[400];
float dutycyclefinal[400];


float guardadepotmax[10];
float controldepotmax[10];
int contadordepotmax = 0;
int limitedecontadordepotmax = 10;
int potenciamaxima = 0;
int controldepotenciamaxima = 0;


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
int pasofiltro = 120;

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

  controldepotenciamaxima = 0;
  potenciamaxima = 0;
  contadordepotmax = 0;
  limitedecontadordepotmax = 10;
  pasofiltro = 120;

  ciclodetrabajo = 0;
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
    outcorrientefilter =  outcorrientefilter + 2550 - analogReadMilliVolts(ADC_I_OUT_PIN); // analogReadMilliVolts  ADC_I_OUT_PIN
    outtensionfilter   =  outtensionfilter + analogReadMilliVolts(ADC_V_OUT_PIN); //  ADC_V_OUT_PIN
    delay(25);
  }

  outcorriente = ((outcorrientefilter) / (pasofiltro));//pasofiltro
  outtension = ((outtensionfilter) / (pasofiltro));
  outpotencia  =  outtension * outcorriente;

  if (outpotencia < 0) {
    outpotencia = 0;
  }

  outcorrientefilter = 0;
  outtensionfilter = 0;
  struct timeval tv;
  unsigned long seconds;
  unsigned long milliseconds;
  unsigned long timeinsecandmil;
  gettimeofday(&tv, NULL);
  milliseconds = tv.tv_usec / 1000;
  if (contadordepotmax < limitedecontadordepotmax) {
    guardadepotmax[contadordepotmax] = outpotencia;
    controldepotmax[contadordepotmax] = ciclodetrabajo;
    contadordepotmax = contadordepotmax + 1;
    if ((deltavariable1 > 0, 3) && (limitinf1 < 66)) { //lower cycle serch
      if (i1 < 3)
      {
        X1 = (limitinf1 + (deltavariable1 * i1));
        funcion1[i1] = outpotencia;
        i1 = i1 + 1;
      } else {
        if ((funcion1[0] < funcion1[1]) && (funcion1[1] < funcion1[2]))
        {
          limitinf1 = limitinf1 + (deltavariable1 * 2);
        } else {
          if ((funcion1[0] < funcion1[1]) && (funcion1[1] > funcion1[2]))
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
      if ((deltavariable2 > 0, 3) && (limitinf1 < 82)) { //medium cycle search
        if (i2 < 3)
        {
          X2 = limitinf2 + (deltavariable2 * i2);
          funcion1[i2] = outpotencia;
          i2 = i2 + 1;
        } else {
          if ((funcion2[0] < funcion2[1]) && (funcion2[1] < funcion2[2]))
          {
            limitinf2 = limitinf2 + (deltavariable2 * 2);
          } else {
            if ((funcion2[0] < funcion2[1]) && (funcion2[1] > funcion2[2]))
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
            if ((funcion3[0] < funcion3[1]) && (funcion3[1] < funcion3[2]))
            {
              limitinf3 = limitinf3 + (deltavariable3 * 2);
            } else {
              if ((funcion3[0] < funcion3[1]) && (funcion3[1] > funcion3[2]))
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
      if ((potmaxcalculada < (outpotencia * 1.1)) || (potmaxcalculada > (outpotencia * 0.9))) //restart of mesuarment of output power changes ~10% over time with same duty cycle
      {
        deltavariable1 = 5;
        deltavariable2 = 5;
        deltavariable3 = 5;
        limitinf1 = limitinf1 - 10;
        limitinf2 = limitinf2 - 10;
        limitinf3 = limitinf3 - 10;
      }
      ciclodetrabajofinal = XMAX;
    }
  } else {
    contadordepotmax = 0;
    for (int arraydepotmax = 1; arraydepotmax < limitedecontadordepotmax; ++arraydepotmax) {
      if (guardadepotmax[arraydepotmax] > guardadepotmax[arraydepotmax - 1]) {
        potenciamaxima = guardadepotmax[arraydepotmax];       // En teoria no haría falta potenciamaxima como variable y solo el valor de arraydepotmax
        controldepotenciamaxima = arraydepotmax;
      }
    }
    outpotencia = guardadepotmax[controldepotenciamaxima];

    outpotenciaviejo = 0;// Forzar que crezca en el siguiente ciclo
    ciclodetrabajo = controldepotmax[controldepotenciamaxima] - 1;
    deltavariable1 = 2.5;
    deltavariable2 = 2.5;
    deltavariable3 = 2.5;
    limitinf1 = ciclodetrabajo - 10;
    limitinf2 = ciclodetrabajo ;
    limitinf3 = ciclodetrabajo + 10;
  }

  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, ciclodetrabajo);
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, ciclodetrabajo);
  arraydate[numeroenvectores] = milliseconds;
  arraydutycycle[numeroenvectores] = ciclodetrabajo;
  dutycyclefinal[numeroenvectores] = ciclodetrabajofinal;
  arraytension[numeroenvectores] = outtension;
  arraypotencia[numeroenvectores] = outpotencia;



  timecounterdiference = timecounter - oldtimecounter;

  if (timecounterdiference > 399) {
    oldtimecounter = timecounter;
    numeroenvectores = 0;

    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, 0);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, 0);

    sendtoexcel(arraydate, arraypotencia, arraytension, arraydutycycle, dutycyclefinal);
  }
  timecounter = timecounter + 1;
  numeroenvectores = numeroenvectores + 1;
  delay(100);
}


//  arraydate[i] = milliseconds;
//  arraypotencia[i] = i ;
//  arraytension[i] = 2 * i ;
//  arraydutycycle[i] = 3 * i ;
//  dutycyclefinal[i] = 1.5 * i ;


void sendtoexcel(int arraydate[], int arraypotencia[], int arraytension[], float arraydutycycle[], float dutycyclefinal[]) {
  int count;
  String GOOGLE_SCRIPT_ID = "your_google_script_ID";
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  for (count = 0; count < 399; count = count + 1) {
    String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + "tiempo=" + arraydate[count] + "&outpotencia=" + arraypotencia[count] + "&dutycycle=" + arraydutycycle[count] + "&dutycyclefinal=" + dutycyclefinal[count] + "&outtension=" + arraytension[count]; //String(count)
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    http.end();
  }
}