#include <pcb_setting.h>


void pcb_init(){
// PCB Use
  pinMode( DIPSW_PIN2, INPUT); 
  pinMode( DIPSW_PIN3, INPUT); 
  pinMode( DIPSW_PIN4, INPUT); 
  gpio_set_pull_mode( GPIO_NUM_13, GPIO_PULLDOWN_ONLY );
  gpio_set_pull_mode( GPIO_NUM_16, GPIO_PULLDOWN_ONLY );
  gpio_set_pull_mode( GPIO_NUM_17, GPIO_PULLDOWN_ONLY );

  pinMode( DSUB_PIN1, INPUT); 
  pinMode( DSUB_PIN4, INPUT); 
  pinMode( DSUB_PIN5, INPUT); 
  pinMode( DSUB_PIN6, INPUT); 
  pinMode( DSUB_PIN8, INPUT); 

  gpio_set_pull_mode(GPIO_NUM_21, GPIO_PULLDOWN_ONLY);
  gpio_set_pull_mode(GPIO_NUM_22, GPIO_PULLDOWN_ONLY);
  gpio_set_pull_mode(GPIO_NUM_25, GPIO_PULLDOWN_ONLY);
  gpio_set_pull_mode(GPIO_NUM_26, GPIO_PULLDOWN_ONLY);
  gpio_set_pull_mode(GPIO_NUM_33, GPIO_PULLDOWN_ONLY);

  pinMode( LED_PIN, OUTPUT); 
  digitalWrite( LED_PIN, LOW) ;


}