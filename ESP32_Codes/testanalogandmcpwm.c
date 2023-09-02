#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

#include "driver/adc.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_reg.h"
#define GPIO_PWM0A_OUT 19   //HV
#define GPIO_PWM0B_OUT 18   //HV
#define Analogread 36
int freq0 = 100000;
float duty0 =   50;
unsigned long int res =100000000;

void setuppwm(){
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


   //mcpwm_deadtime_enable(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_ACTIVE_RED_FED_FROM_PWMXA, 10,  10);
   //mcpwm_deadtime_enable(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_ACTIVE_RED_FED_FROM_PWMXA, 10,  10);
   
   
   mcpwm_set_timer_sync_output(MCPWM_UNIT_0, MCPWM_TIMER_0,MCPWM_SWSYNC_SOURCE_TEZ);
   
   mcpwm_sync_enable(MCPWM_UNIT_0, MCPWM_TIMER_0,MCPWM_SELECT_TIMER0_SYNC, 0);   
   
   mcpwm_sync_enable(MCPWM_UNIT_0, MCPWM_TIMER_1,MCPWM_SELECT_TIMER0_SYNC, 500); 
  
    
   mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, duty0);
   mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, duty0);
   
   mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, MCPWM_DUTY_MODE_0);
   
   mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, MCPWM_DUTY_MODE_1);
   
 }
void setup() {
 setuppwm();

}

void loop() {
   int analogvalue=0;
  while(1){
    analogvalue=analogReadMilliVolts(Analogread);
//    if(analogvalue==2047 || analogvalue>2074){
//      duty0=75;
//      }else{
//        if(analogvalue==1023 || analogvalue>1023){duty0=50;}else{ duty0=25;} 
//      }
    duty0=(analogvalue/100);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, duty0);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, duty0);
    delay(100);
    }
 
}
