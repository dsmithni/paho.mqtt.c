//==============================================================================
//
// Title:		LabVIEW MQTT
// Purpose:		A short description of the interface.
//
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

#include "MQTTAsync.h"
#include <extcode.h>
		
//==============================================================================
// Types
		
//structure used to maintain all connection state
typedef struct {
	//core client reference
	MQTTAsync client;
	//event indicating a subscription was created (event(lvmqtt_subMsg))
	LVUserEventRef subMsg;
	//event indicating the system disconnected (event(lvmqtt_statusMsg))
	LVUserEventRef disconnect;
	//event indicating a send completed successfully (event(int))
	LVUserEventRef QoSAck;
	//event for a connection success (event(lvmqtt_statusMsg))
	LVUserEventRef connSuccess;
	//event for a connection failure (event(lvmqtt_statusMsg))
	LVUserEventRef connFail;
	//event for a send success (event(int))
	LVUserEventRef sendS;
	//event for a send faulure (event(int))
	LVUserEventRef sendF;
} lvmqtt_context;

//simple status message sent to labview
typedef struct {
	int32 code;
	LStrHandle data;
} lvmqtt_statusMsg;

//message from a subscription
typedef struct {
	int32 id;
	LStrHandle Topic;
	LStrHandle MsgData;
} lvmqtt_subMsg;

//==============================================================================
// Global functions



DLLExport int32 LVMQTT_Create(lvmqtt_context ** lvctx, LVUserEventRef * subMsg, LVUserEventRef * disconnect,
					LVUserEventRef * QoSAck, LVUserEventRef * connSuccess, LVUserEventRef * connFail,
					LVUserEventRef * sendS, LVUserEventRef * sendF, char * serverURI, char * clientID);
DLLExport int32 LVMQTT_Destroy(lvmqtt_context * lvctx);
DLLExport int32 LVMQTT_Connect(lvmqtt_context * lvctx, int timeout, char * uname, char * pass);
DLLExport int32 LVMQTT_Disconnect(lvmqtt_context * lvctx, int timeout);
DLLExport LVBoolean LVMQTT_IsConnected(lvmqtt_context * lvctx);
DLLExport int32 LVMQTT_Publish(lvmqtt_context * lvctx, char * topic, int32 topiclen, char * msgdata, int32 msglen, int qos);
DLLExport int32 LVMQTT_Subscribe(lvmqtt_context * lvctx, char * topic, int qos);

void LVMQTT_disconnected(void* context, char* cause);
int LVMQTT_subMsg(void* context, char* topicName, int topicLen, MQTTAsync_message* message);
void LVMQTT_QoSAck(void* context, MQTTAsync_token dt);
void LVMQTT_connSuccess(void* context, MQTTAsync_successData* response);
void LVMQTT_connFail(void* context,  MQTTAsync_failureData* response);
void LVMQTTAsync_SendS(void* context, MQTTAsync_successData* response);
void LVMQTTAsync_SendF(void* context,  MQTTAsync_failureData* response);


#ifdef __cplusplus
    }
#endif

#endif  /* ndef __LabVIEWMQTT_H__ */
