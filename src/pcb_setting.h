#ifndef PCB_H
#define PCB_H

#include <Arduino.h>

#define DIPSW_PIN1 (-1) // 120 orm  H:On,  L:Off
#define DIPSW_PIN2 (13) // pcb setting  H:setting mode, L:normal mode
#define DIPSW_PIN3 (16) // CAN Tx test
#define DIPSW_PIN4 (17) // CAN Rx test : serial monitor

#define CAN_RX_PIN (27)
#define CAN_TX_PIN (32)

#define DSUB_PIN1 (21)  //SDA
#define DSUB_PIN2 (-1)  //CAN L
#define DSUB_PIN3 (-1)  //GND
#define DSUB_PIN4 (22)  //SCL
#define DSUB_PIN5 (25)  //DAC1
#define DSUB_PIN6 (26)  //DAC2
#define DSUB_PIN7 (-1)  //CAN H
#define DSUB_PIN8 (33)  //
#define DSUB_PIN9 (-1)  // Vin

#define LED_PIN (14)  // 

void pcb_init();

#endif


