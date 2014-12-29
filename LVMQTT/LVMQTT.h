//==============================================================================
//
// Title:		LabVIEW MQTT
// Purpose:		A short description of the interface.
//
// Created on:	12/19/2014 at 11:12:59 PM by daniel smith.
// Copyright:	DS. All Rights Reserved.
//
//==============================================================================

#ifndef __LabVIEWMQTT_H__
#define __LabVIEWMQTT_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"
#include <MQTTAsync.h>
#include <extcode.h>
		
//==============================================================================
// Constants

//==============================================================================
// Types
		
typedef struct {
	MQTTAsync client;
	LVUserEventRef subMsg;
	LVUserEventRef disconnect;
	LVUserEventRef QoSAck;
	LVUserEventRef connSuccess;
	LVUserEventRef connFail;
	LVUserEventRef sendS;
	LVUserEventRef sendF;
	LVBoolean connected;
	MQTTAsync_connectOptions * connectOpts
} lvmqtt_context;

#define lvmqtt_context_initializer { NULL, 0, 0, 0, 0, 0, 0, 0, LVFALSE, NULL} 


typedef struct {
	int32 code;
	LStrHandle data;
} lvmqtt_statusMsg;

typedef struct {
	int32 id;
	LStrHandle Topic;
	LStrHandle MsgData;
} lvmqtt_subMsg;

//==============================================================================
// External variables

//==============================================================================
// Global functions



int32 LVMQTT_Create(lvmqtt_context ** lvctx, LVUserEventRef * subMsg, LVUserEventRef * disconnect, 
					LVUserEventRef * QoSAck, LVUserEventRef * connSuccess, LVUserEventRef * connFail,
					LVUserEventRef * sendS, LVUserEventRef * sendF, char * serverURI, char * clientID);
int32 LVMQTT_Destroy(lvmqtt_context * lvctx);
int32 LVMQTT_Connect(lvmqtt_context * lvctx, int timeout, char * uname, char * pass);
LVBoolean LVMQTT_IsConnected(lvmqtt_context * lvctx);
int32 LVMQTT_Publish(lvmqtt_context * lvctx, char * topic, int32 topiclen, char * msgdata, int32 msglen, int qos);
int32 LVMQTT_Subscribe(lvmqtt_context * lvctx, char * topic, int qos);

void LVMQTT_disconnect(void* context, char* cause); 
int LVMQTT_subMsg(void* context, char* topicName, int topicLen, MQTTAsync_message* message);
void LVMQTT_QoSAck(void* context, MQTTAsync_token dt);
void LVMQTT_connSuccess(void* context, MQTTAsync_successData* response);
void LVMQTT_connFail(void* context,  MQTTAsync_failureData* response);
int lvmqtt_conn(lvmqtt_context * lvctx);
void LVMQTTAsync_SendS(void* context, MQTTAsync_successData* response);
void LVMQTTAsync_SendF(void* context,  MQTTAsync_failureData* response);


#ifdef __cplusplus
    }
#endif

#endif  /* ndef __LabVIEWMQTT_H__ */
