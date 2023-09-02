#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

#include "driver/adc.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_reg.h"
#define GPIO_PWM0A_OUT 19   //HV
#define GPIO_PWM0B_OUT 18   //HV
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
  while(1){
    if(duty0==95 || duty0>95){duty0=50;}
    duty0=duty0+0.1;
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, duty0);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, duty0);
    delay(500);
    }
 
}