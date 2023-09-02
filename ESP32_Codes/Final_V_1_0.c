#include <Arduino.h>
#include "esp_system.h" 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"
#include "WiFi.h"
#include <HTTPClient.h>
#include "time.h"
#include "driver/adc.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_reg.h"
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif
#ifndef ADC_V_IN_PIN
#define ADC_V_IN_PIN 4
#endif
#ifndef ADC_V_OUT_PIN
#define ADC_V_OUT_PIN 0
#endif
#ifndef ADC_I_IN_PIN
#define ADC_I_IN_PIN 2
#endif
#ifndef ADC_I_OUT_PIN
#define ADC_I_OUT_PIN 15
#endif
#ifndef Botton_Control_1
#define Botton_Control_1 36
#endif
#ifndef Botton_Control_2
#define Botton_Control_2 39
#endif
#ifndef Botton_Control_3
#define Botton_Control_3 34
#endif
#define GPIO_PWM0A_OUT 19   //HV
#define GPIO_PWM0B_OUT 18   //HV
// define two tasks for Blink & AnalogRead
void TaskUser_control( void *pvParameters );
void TaskDuty( void *pvParameters );
void TaskMath( void *pvParameters );
void TaskIot( void *pvParameters );
QueueHandle_t controlQueue;
QueueHandle_t dutycycleQueue;
QueueHandle_t finaldutycycleQueue;
QueueHandle_t intensionQueue;
QueueHandle_t incorrienteQueue;
QueueHandle_t inpotenciaQueue;
QueueHandle_t outtensionQueue;
QueueHandle_t outcorrienteQueue;
QueueHandle_t outpotenciaQueue;
void setuppwm(){
  int freq0 = 100000;
  unsigned long int res =100000000;
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, GPIO_PWM0A_OUT);
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, GPIO_PWM0B_OUT);
  
  mcpwm_group_set_resolution(MCPWM_UNIT_0, res);
  
    mcpwm_config_t pwm_config0;
    pwm_config0.frequency = freq0;  //frequency 
    pwm_config0.cmpr_a = 0;      //duty cycle of PWMxA = 0%
    pwm_config0.cmpr_b = 0;      //duty cycle of PWMxB = 0%
    pwm_config0.counter_mode = MCPWM_UP_COUNTER; // Up counter 
    pwm_config0.duty_mode = MCPWM_DUTY_MODE_0; // Active HIGH
     
   mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config0);    
   mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config0); 
   mcpwm_set_timer_sync_output(MCPWM_UNIT_0, MCPWM_TIMER_0,MCPWM_SWSYNC_SOURCE_TEZ);
   mcpwm_sync_enable(MCPWM_UNIT_0, MCPWM_TIMER_0,MCPWM_SELECT_TIMER0_SYNC, 0);   
   mcpwm_sync_enable(MCPWM_UNIT_0, MCPWM_TIMER_1,MCPWM_SELECT_TIMER0_SYNC, 500); 
   mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, 0);
   mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, 0);
   mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, MCPWM_DUTY_MODE_0);
   mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, MCPWM_DUTY_MODE_1);
   
 }
// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(Botton_Control_1, INPUT_PULLUP);
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  setuppwm();
  controlQueue  = xQueueCreate( 10, sizeof( int ) );
  dutycycleQueue = xQueueCreate( 10, sizeof( float ) );
  finaldutycycleQueue = xQueueCreate( 10, sizeof( float ) );
  intensionQueue = xQueueCreate( 10, sizeof( float ) );
  incorrienteQueue = xQueueCreate( 10, sizeof( float ) );
  inpotenciaQueue = xQueueCreate( 10, sizeof( float ) );
  outtensionQueue = xQueueCreate( 10, sizeof( float ) );
  outcorrienteQueue = xQueueCreate( 10, sizeof( float ) );
  outpotenciaQueue = xQueueCreate( 10, sizeof( float ) );
  const char* ntpServer = "pool.ntp.org";
  const long  gmtOffset_sec = 19800;
  const int   daylightOffset_sec = 0;
  // WiFi credentials
  const char* ssid = "ESP_DATA";         // change SSID
  const char* password = "esp32iot";    // change password
  // Google script ID and required credentials
  String GOOGLE_SCRIPT_ID = "script ID";    // change Gscript ID
  int count = 0;
  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(|);
  Serial.flush();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  // Now set up 4 tasks to run independently.
  xTaskCreatePinnedToCore(
    TaskUser_control
    ,  "TaskUser_control" // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  3  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);


  xTaskCreatePinnedToCore(
    TaskDuty
    ,  "TaskDuty"   
    ,  1024  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    TaskMath
    ,  "TaskMath"
    ,  1024  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);


  xTaskCreatePinnedToCore(
    TaskIot
    ,  "TaskIot"
    ,  1024  // Stack size
    ,  NULL
    ,  0  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}



void loop()
{
  // Empty. Things are done in Tasks.
}



/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/



void TaskUser_control(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
int lastState = HIGH; // the previous state from the input pin
int currentState;     // the current reading from the input pin
int counter=0;
  for (;;)
  {
  if (counter==7)
  {
    counter=0;
  }
  currentState = digitalRead(Botton_Control_1);
  if(lastState == LOW && currentState == HIGH){
    xQueueSend(controlQueue, &counter, portMAX_DELAY);
    counter=counter+1;
  }
  // save the last state
  lastState = currentState;
vTaskDelay(300);  
  }
}



void TaskDuty(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  float duty0 =   50;
  for (;;)
  {
    xQueueReceive(dutycycleQueue, &duty0, portMAX_DELAY);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, duty0);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, duty0);
  }
}


void TaskMath(void *pvParameters)  // This is a task.
{
(void) pvParameters;
float inpotenciaviejo = 0.0;
float outpotenciaviejo = 0.0;
float incorriente = 0.0;
float intension = 0.0;
float inpotencia = 0.0;
float ciclodetrabajo = 50;
float outcorriente = 0.0;
float outtension = 0.0;
float outpotencia = 0.0;
float maxcorriente = 0.0;
float maxtension = 0.0;
float maxpotencia = 0.0;
float ciclodetrabajofinal = 0.0;
float ciclodetrabajoneg=100;
int oscilationstop=0;
int methode=0;
int cyclecounter=0;
float funcion[3];
float funcion1[3];
float funcion2[3];
float funcion3[3];
float resetciclodetrabajo=50;
float limitinf=50;
float limitinf1=50;
float limitinf2=66;
float limitinf3=82;
int i=0;
int i1=0;
int i2=0;
int i3=0;
int X=0;
int X1=0;
int X2=0;
int X3=0;
float deltavariable=10;
float deltavariable1=5;
float deltavariable2=5;
float deltavariable3=5;
float XMAX=0;
float cien=100;
float cero=0.0;
float potmaxcalculada=0.0;  
int newmethode=0;
//[time,incorriente,intension,inpotencia,ciclodetrabajo,outcorriente,outtension,outpotencia,maxcorriente,maxtension,maxpotencia,ciclodetrabajofinal]
/*
  AnalogReadSerial
  Reads an analog input on pin A3, prints the result to the serial monitor.
  Graphical representation is available using serial plotter (Tools > Serial Plotter menu)
  Attach the center pin of a potentiometer to pin A3, and the outside pins to +5V and ground.

  This example code is in the public domain.
*/
 // ADC_V_IN_PIN ADC_V_OUT_PIN ADC_I_IN_PIN ADC_I_OUT_PIN
  for (;;)
  { //queue wait 0 Control boton change methode{}
    xQueueReceive(controlQueue, &methode, 0);
    if (newmethode!= methode)
    {
      if (methode==6)
      {
        xQueueSend(dutycycleQueue, &cien, portMAX_DELAY);
      }else{
        deltavariable=10;
        deltavariable1=5;
        deltavariable2=5;
        deltavariable3=5;
        XMAX=0;
        potmaxcalculada=0.0;
        i=0;
        i1=0;
        i2=0;
        i3=0;
        X=0;
        X1=0;
        X2=0;
        X3=0;
        limitinf=50;
        limitinf1=50;
        limitinf2=66;
        limitinf3=82;  
        oscilationstop=0;
        cyclecounter=0;
        xQueueSend(dutycycleQueue, &resetciclodetrabajo, portMAX_DELAY);
        }  
  vTaskDelay(50);
    }
    newmethode=methode;
    // read the input on analog pin A3:
    incorriente  =  analogReadMilliVolts(ADC_I_IN_PIN);
    intension    =  analogReadMilliVolts(ADC_V_IN_PIN);
    inpotencia   =  intension*incorriente;
    outcorriente =  analogReadMilliVolts(ADC_I_OUT_PIN);
    outtension   =  analogReadMilliVolts(ADC_V_OUT_PIN);
    outpotencia  =  outtension*outcorriente;
    switch(methode)
    {
      case 0:
      xQueueSend(dutycycleQueue, &cero, portMAX_DELAY);
      break;
      case 1:
      xQueueSend(dutycycleQueue, &cien, portMAX_DELAY);
      break;
      case 2:      
      if (cyclecounter<200)
      {
        ciclodetrabajo=ciclodetrabajo+0.5;
        cyclecounter=cyclecounter+1;
      }else{
          ciclodetrabajo=0;
          cyclecounter=0;
          }
    xQueueSend(dutycycleQueue, &ciclodetrabajo, portMAX_DELAY);
      break;
      case 3:
        if (inpotencia>inpotenciaviejo || outpotencia>outpotenciaviejo)
       {
         ciclodetrabajo=ciclodetrabajo+0.5;
         inpotenciaviejo=inpotencia;
         outpotenciaviejo=outpotencia;
         maxpotencia=outpotencia;
      }else{
          if ( outpotencia == maxpotencia || oscilationstop>1000)
           {
             ciclodetrabajofinal=ciclodetrabajo;
             inpotenciaviejo=inpotencia;
            outpotenciaviejo=outpotencia;
            oscilationstop=0;
            //send queue ciclofinal, 
          }else{ 
                 ciclodetrabajo=ciclodetrabajo-0.5;
                 inpotenciaviejo=inpotencia;
                 outpotenciaviejo=outpotencia;
                 oscilationstop=oscilationstop+1;
               }
        }
      xQueueSend(dutycycleQueue, &ciclodetrabajo, portMAX_DELAY);      
      break;
      case 4:
if(deltavariable<0,3){
  if (i<3)
  {
    X=limitinf+(deltavariable*i);
    funcion[i]= outpotencia;
    i = i + 1;
  }else{
    if (funcion[0]<funcion[1] && funcion[1] < funcion[2])
    {
      limitinf=limitinf+(deltavariable*2)
    }else{
      if (funcion[0]<funcion[1] && funcion[1] > funcion[2])
      {
        limitinf=limitinf+deltavariable;
        deltavariable=deltavariable/2;
      }else{
        limitinf=limitinf-deltavariable;
      }
    }
    i=0;
  }
  xQueueSend(dutycycleQueue, &X, portMAX_DELAY);
}else{
  if (funcion[1]<(outpotencia*1.1) || funcion[1]>(outpotencia*0.9))
  {
    deltavariable=10;
    limitinf=limitinf-10;
  }
  xQueueSend(finaldutycycleQueue, &X, portMAX_DELAY);
}
      break;
      case 5:
if(deltavariable1<0,3 && limitinf1<66){   //lower cycle serch
  if (i1<3)
  {
    X1=limitinf1+(deltavariable1*i1);
    funcion1[i1]= outpotencia;
    i1 = i1 + 1;
  }else{
    if (funcion1[0]<funcion1[1] && funcion1[1] < funcion1[2])
    {
      limitinf1=limitinf1+(deltavariable1*2);
    }else{
      if (funcion1[0]<funcion1[1] && funcion1[1] > funcion1[2])
      {
        limitinf1=limitinf1+deltavariable1;
        deltavariable1=deltavariable1/2;
      }else{
        limitinf1=limitinf1-deltavariable1;
      }
    }
    i1=0;
  }
  xQueueSend(dutycycleQueue, &X1, portMAX_DELAY);
}else{
if(deltavariable2<0,3 && limitinf1<82){ //medium cycle search
  if (i2<3)
  {
    X2=limitinf2+(deltavariable2*i2);
    funcion1[i2]= outpotencia;
    i2 = i2 + 1;
  }else{
    if (funcion2[0]<funcion2[1] && funcion2[1] < funcion2[2])
    {
      limitinf2=limitinf2+(deltavariable2*2);
    }else{
      if (funcion2[0]<funcion2[1] && funcion2[1] > funcion2[2])
      {
        limitinf2=limitinf2+deltavariable2;
        deltavariable2=deltavariable2/2;
      }else{
        limitinf2=limitinf2-deltavariable2;
      }
    }
    i2=0;
  }
  xQueueSend(dutycycleQueue, &X2, portMAX_DELAY);
}else{
if(deltavariable3<0,3){ //top cycle search
  if (i3<3)
  {
    X3=limitinf3+(deltavariable3*i3);
    funcion1[i3]= outpotencia;
    i3 = i3 + 1;
  }else{
    if (funcion3[0]<funcion3[1] && funcion3[1] < funcion3[2])
    {
      limitinf3=limitinf3+(deltavariable3*2);
    }else{
      if (funcion3[0]<funcion3[1] && funcion3[1] > funcion3[2])
      {
        limitinf3=limitinf3+deltavariable3;
        deltavariable3=deltavariable3/2;
      }else{
        limitinf3=limitinf3-deltavariable3;
      }
    }
    i3=0;
  }
  xQueueSend(dutycycleQueue, &X3, portMAX_DELAY);
}
}
if (funcion1[1]<funcion2[1])
{
    if (funcion2[1]<funcion3[1])
    {
        XMAX=X3;
        potmaxcalculada=funcion3[1];
    }else{
        XMAX=X2;
        potmaxcalculada=funcion2[1];
        }
}else{
    XMAX=X1;
    potmaxcalculada=funcion1[1];
}
  if (potmaxcalculada<(outpotencia*1.1) || potmaxcalculada>(outpotencia*0.9)) //restart of mesuarment of output power changes ~10% over time with same duty cycle
  {
    deltavariable1=2,5;
    deltavariable2=2,5;
    deltavariable3=2,5;
    limitinf1=limitinf1-10;
    limitinf1=limitinf1-10;
    limitinf1=limitinf1-10;
  }
  xQueueSend(finaldutycycleQueue, &XMAX, portMAX_DELAY);
}//llave final de caso 5
      break;
      case 6:
if (inpotencia>inpotenciaviejo || outpotencia>outpotenciaviejo)
       {
         ciclodetrabajoneg=ciclodetrabajoneg-0.5;
         inpotenciaviejo=inpotencia;
         outpotenciaviejo=outpotencia;
         maxpotencia=outpotencia;
      }else{
          if ( outpotencia == maxpotencia || oscilationstop>1000)
           {
             ciclodetrabajofinal=ciclodetrabajoneg;
             inpotenciaviejo=inpotencia;
            outpotenciaviejo=outpotencia;
            oscilationstop=0;
            //send queue ciclofinal, 
          }else{ 
                 ciclodetrabajoneg=ciclodetrabajoneg+0.5;
                 inpotenciaviejo=inpotencia;
                 outpotenciaviejo=outpotencia;
                 oscilationstop=oscilationstop+1;
               }
        }
xQueueSend(dutycycleQueue, &ciclodetrabajoneg, portMAX_DELAY);
      break;

}//llave de final de switch


xQueueSend(outtension, &XMAX, portMAX_DELAY);
xQueueSend(outcorriente, &XMAX, portMAX_DELAY);
xQueueSend(outpotencia, &XMAX, portMAX_DELAY);
vTaskDelay(50);  // one tick delay (15ms) in between reads for stability
}//llave de final de task


void TaskIot(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  


  for (;;)
  {
   if (WiFi.status() == WL_CONNECTED) {
    static bool flag = false;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      return;
    }
    char timeStringBuff[50]; //50 chars should be enough
    strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
    String asString(timeStringBuff);
    asString.replace(" ", "-");
    Serial.print("Time:");
    Serial.println(asString);
    String urlFinal = "https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+"date=" + asString + "&incorriente=" + String(incorriente)+ "&intension=" + String(intension)+ "&inpotencia=" + String(inpotencia)+ "&ciclodetrabajo=" + String(ciclodetrabajo)+ "&outcorriente=" + String(outcorriente)+ "&outtension=" + String(outtension)+ "&outpotencia=" + String(outpotencia)+ "&maxcorriente=" + String(maxcorriente)+ "&maxtension=" + String(maxtension)+ "&maxpotencia=" + String(maxpotencia)+ "&ciclodetrabajofinal=" + String(ciclodetrabajofinal);
    Serial.print("POST data to spreadsheet:");  //[time,incorriente,intension,inpotencia,ciclodetrabajo,outcorriente,outtension,outpotencia,maxcorriente,maxtension,maxpotencia,ciclodetrabajofinal]
    Serial.println(urlFinal);
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET(); 
    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);
    //---------------------------------------------------------------------
    //getting response from google sheet
    String payload;
    if (httpCode > 0) {
        payload = http.getString();
        Serial.println("Payload: "+payload);    
    }
    //---------------------------------------------------------------------
    http.end();
  }
 
  vTaskDelay(1000);
  }
}

