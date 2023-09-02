
#ifndef ADC_V_OUT_PIN
#define ADC_V_OUT_PIN 0
#endif

#ifndef ADC_I_OUT_PIN
#define ADC_I_OUT_PIN 15
#endif


dutycycleQueue = xQueueCreate( 10, sizeof( float ) );
finaldutycycleQueue = xQueueCreate( 10, sizeof( float ) );
tensionQueue = xQueueCreate( 10, sizeof( float ) );
corrienteQueue = xQueueCreate( 10, sizeof( float ) );
potenciaQueue = xQueueCreate( 10, sizeof( float ) );

void TaskMath(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

float funcion1[3];
float funcion2[3];
float funcion3[3];
float limitinf1=50;
float limitinf2=66;
float limitinf3=82;
int i1=0;
int i2=0;
int i3=0;
int X1=0;
int X2=0;
int X3=0;
float deltavariable1=5;
float deltavariable2=5;
float deltavariable3=5;
float XMAX=0;

float corriente = 0.0;
float tension = 0.0;
float potencia = 0.0;
float potmaxcalculada=0.0;

for (;;)
{
  tension   =  analogReadMilliVolts(ADC_V_OUT_PIN);
  corriente =  analogReadMilliVolts(ADC_I_OUT_PIN);
  potencia  =  tension*corriente;
  if(deltavariable1<0,3 && limitinf1<66){
  if (i1<3)
  {
    X1=limitinf1+(deltavariable1*i1);
    funcion1[i1]= potencia;
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
  if(deltavariable2<0,3 && limitinf1<82){
  if (i2<3)
  {
    X2=limitinf2+(deltavariable2*i2);
    funcion1[i2]= potencia;
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
if(deltavariable3<0,3){
  if (i3<3)
  {
    X3=limitinf3+(deltavariable3*i3);
    funcion1[i3]= potencia;
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

  if (potmaxcalculada<(potencia*1.1) || potmaxcalculada>(potencia*0.9))
  {
    deltavariable1=2,5;
    deltavariable2=2,5;
    deltavariable3=2,5;
    limitinf1=limitinf1-10;
    limitinf1=limitinf1-10;
    limitinf1=limitinf1-10;
  }
  xQueueSend(finaldutycycleQueue, &XMAX, portMAX_DELAY);
}
xQueueSend(tension, &XMAX, portMAX_DELAY);
xQueueSend(corriente, &XMAX, portMAX_DELAY);
xQueueSend(potencia, &XMAX, portMAX_DELAY);
TaskDelay(50);
}

}
