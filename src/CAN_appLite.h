// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef CAN_APP_H
#define CAN_APP_H

#include <Arduino.h>
#include <CAN.h>

#include "ArduinoJson-v6.19.4.h"

//#include "wifi_Server.h"


// typedef struct canApp{
// }canApp;

typedef struct canTxRxFlag
{
    static const int NON = 0;
    static const int TX = 1;
    static const int RX = 2;

}canTxRxFlag;

typedef struct canRxBuffer
{
    char  txrxFlag;
    char  dlc;
    short id;  
    unsigned long prevTime;
    short cycleTime;  
    //unsigned int setTimeCnt;

	union {
		unsigned char  u1[8];   /**< \brief Payload byte access*/
		unsigned short u2[4]; /**< \brief Payload u32 access*/
		unsigned long  u4[2];    /**< \brief Payload u64 access*/
	} data;

	union {
        unsigned char rxCnt[8];
        unsigned long txCnt[2];

    }noChange;

    unsigned char noRecvCnt[8];

}canRxBuffer;

typedef struct canTxBuffer
{
    char  txrxFlag;
    char  dlc;
    short id;  
    unsigned long prevTime;
    unsigned int setTimeCnt;
    short cycleTime;  

	union {
		unsigned char  u1[8];   /**< \brief Payload byte access*/
		unsigned short u2[4]; /**< \brief Payload u32 access*/
		unsigned long  u4[2];    /**< \brief Payload u64 access*/
	} data;

}canTxBuffer;


//canBuffer canbuf[0x800];

extern char can_json[];

void can_init();
void can_setTestFlag( int txtest, int rxtest );
void can_loop();
void canbuf_init();
void canTxbuf_set( int id, char dlc, int cycle, unsigned char *data, int txflag  );
void canTxbuf_set_test( );
void canbuf_send();
void canbuf_sendSingle( int id );
void onReceive(int packetSize);
void printRecv();
void setup_CallBack();
void makeCanMsgJsonDummy();
void makeCanMsgJson();
int id2idx( int id );

//extern void canTxbuf_set( int id, int dlc, int cycle, char *data, int txflag  );
//extern void canTxbuf_test( );

#endif
