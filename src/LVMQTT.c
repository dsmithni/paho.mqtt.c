//==============================================================================
//
// Title:		LabVIEW MQTT
// Purpose:		A short description of the library.
//
// Created on:	12/19/2014 at 11:12:59 PM by daniel smith.
// Copyright:	DS. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include "LVMQTT.h"
#include "MQTTAsync.h" 
#include <string.h>
#include <windows.h>

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions



void LVMQTT_disconnect(void* context, char* cause) {
	if(!context) return;
	lvmqtt_context * ctx = (lvmqtt_context *) context;
	if(!ctx) return;
	LVUserEventRef devent = ctx->connSuccess;
	if (devent) {
		lvmqtt_statusMsg msgdata = {0, NULL};
		msgdata.code = 66;//disconnect by peer
		size_t l = strlen(cause);
		msgdata.data = (LStrHandle)DSNewHandle(sizeof(int32) + l);
		if (msgdata.data) {
			LStrLen(*msgdata.data) = l;
			MoveBlock(cause, LHStrBuf(msgdata.data), l);
			PostLVUserEvent(devent, &msgdata);
		}
	}
	if(!ctx) return;
	lvmqtt_conn(ctx);
	return;
}


int LVMQTT_subMsg(void* context, char* topicName, int topicLen, MQTTAsync_message* message) {
	LVUserEventRef subMsg = ((lvmqtt_context *)context)->subMsg;
	if (!subMsg) { 
		return 1; //not true, but basically we're saying that if there is no event, go ahead and say its correctly fired
	}
	lvmqtt_subMsg e = {0, NULL, NULL};
	
	LStrHandle t = (LStrHandle)DSNewHandle(sizeof(int32) + topicLen);
	if (!t) {
		return 0;
	}
	LStrHandle m = (LStrHandle)DSNewHandle(sizeof(int32) + message->payloadlen);
	if (!m) {
		return 0;
	}
	e.Topic=t;
	e.MsgData=m;
	e.id=message->msgid;
	if (topicLen == 0) {
		topicLen = strlen(topicName);
	}
	LStrLen(*t) = topicLen;
	LStrLen(*m) = message->payloadlen;
	
	MoveBlock(topicName, LHStrBuf(t), topicLen);
	MoveBlock(message->payload, LHStrBuf(m), message->payloadlen);
	
	MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);
	
	MgErr r = PostLVUserEvent(subMsg, &e);
	if (r == mgNoErr) {return 1;}
	return 0;
}

void LVMQTT_QoSAck(void* context, MQTTAsync_token dt) {
	LVUserEventRef qosevent = ((lvmqtt_context *)context)->QoSAck;
	if (qosevent) { 	
		PostLVUserEvent(qosevent, &dt);		
	}
	return;
}

void LVMQTT_connSuccess(void* context, MQTTAsync_successData* response) {
	((lvmqtt_context *)context)->connected=LVTRUE;
	LVUserEventRef csevent = ((lvmqtt_context *)context)->connSuccess;
	if (csevent) {
		lvmqtt_statusMsg msgdata = {0, NULL};
		msgdata.code = 0;
		size_t l = strlen(response->alt.connect.serverURI);
		msgdata.data = (LStrHandle)DSNewHandle(sizeof(int32) + l);
		if (!msgdata.data) {
			return;
		}
		LStrLen(*msgdata.data) = l;
		MoveBlock(response->alt.connect.serverURI, LHStrBuf(msgdata.data), l);
		MgErr r = PostLVUserEvent(csevent, &msgdata);
		if (r != mgNoErr) {
			return;
		}
	}
	return;
}


void LVMQTT_connFail(void* context,  MQTTAsync_failureData* response) {
	((lvmqtt_context *)context)->connected = LVFALSE;
	LVUserEventRef cfevent = ((lvmqtt_context *)context)->connFail;
	if (cfevent) {
		lvmqtt_statusMsg msgdata = {0, NULL};
		msgdata.code = response->code;
		size_t l = strlen(response->message);
		msgdata.data = (LStrHandle)DSNewHandle(sizeof(int32) + l);
		if (!msgdata.data) {
			return;
		}
		LStrLen(*msgdata.data) = l;
		MoveBlock(response->message, LHStrBuf(msgdata.data), l);
		PostLVUserEvent(cfevent, &msgdata);
	}
	lvmqtt_conn((lvmqtt_context *) context);
	return;
}


void LVMQTTAsync_SendS(void* context, MQTTAsync_successData* response) {
	LVUserEventRef sendSevent = ((lvmqtt_context *)context)->sendS;
	if (sendSevent) { 
		int t = response->token;
		PostLVUserEvent(sendSevent, &t);		
	}
	return;
}
void LVMQTTAsync_SendF(void* context,  MQTTAsync_failureData* response) {
	LVUserEventRef sendFevent = ((lvmqtt_context *)context)->sendF;
	if (sendFevent) { 
		int t = response->token;
		PostLVUserEvent(sendFevent, &t);		
	}
	return;
}




int32 LVMQTT_Create(lvmqtt_context ** lvctx, LVUserEventRef * subMsg, LVUserEventRef * disconnect, 
					LVUserEventRef * QoSAck, LVUserEventRef * connSuccess, LVUserEventRef * connFail,
					LVUserEventRef * sendS, LVUserEventRef * sendF, char * serverURI, char * clientID)
{
	
	MQTTAsync mqttc;
	lvmqtt_context * lvctxp = (lvmqtt_context *) DSNewPClr(sizeof(lvmqtt_context));
	
	lvctxp->subMsg=*subMsg;
	lvctxp->disconnect=*disconnect;
	lvctxp->QoSAck=*QoSAck;
	lvctxp->connFail=*connFail;
	lvctxp->connSuccess=*connSuccess;
	lvctxp->sendF=*sendF;
	lvctxp->sendS=*sendS;
	
	
	if (strlen(clientID) > 23) {
		*(clientID+22)=0;	
	}
    int r = MQTTAsync_create(&mqttc, serverURI, clientID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
	if (r!=MQTTASYNC_SUCCESS) {
		DSDisposePtr(lvctxp);
		return r;
	}
	
	lvctxp->client = mqttc;

    r = MQTTAsync_setCallbacks(mqttc, lvctxp, LVMQTT_disconnect, LVMQTT_subMsg, LVMQTT_QoSAck);
	if (r!=MQTTASYNC_SUCCESS) {
		DSDisposePtr(lvctxp);
		return r;
	}
	*lvctx = lvctxp;
	
	return MQTTASYNC_SUCCESS;
	
}


LVBoolean LVMQTT_IsConnected(lvmqtt_context * lvctx) {
	return lvctx->connected;
}


int32 LVMQTT_Connect(lvmqtt_context * lvctx, int timeout, char * uname, char * pass) {
	MQTTAsync_connectOptions connectOpts = MQTTAsync_connectOptions_initializer;
	lvctx->connectOpts=&connectOpts;
	connectOpts.keepAliveInterval = 60;
	if (timeout > 0) { connectOpts.connectTimeout = timeout; }
    connectOpts.cleansession = 1;
    connectOpts.onSuccess = LVMQTT_connSuccess;
    connectOpts.onFailure = LVMQTT_connFail;
    connectOpts.context = lvctx;
	connectOpts.username = uname;
	connectOpts.password = pass;
	return MQTTAsync_connect(lvctx->client, &connectOpts);
}

int lvmqtt_conn(lvmqtt_context * lvctx) {
	if (lvctx->connectOpts) {
		return MQTTAsync_connect(lvctx->client, lvctx->connectOpts);
	}
	return MQTTASYNC_FAILURE;
}


int32 LVMQTT_Destroy(lvmqtt_context * lvctx) {
	if (lvctx && lvctx->client) {
		MQTTAsync c = lvctx->client;
		MQTTAsync_destroy(&c);
		DSDisposePtr(lvctx);
		return 0;
	}
	return -1;
}


int32 LVMQTT_Publish(lvmqtt_context * lvctx, char * topic, int32 topiclen, char * msgdata, int32 msglen, int qos) {
    MQTTAsync client = lvctx->client;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
	MQTTAsync_responseOptions * respOpts = &opts;
    MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
    
    opts.onSuccess = LVMQTTAsync_SendS;
	opts.onFailure = LVMQTTAsync_SendF;
    opts.context = lvctx;

	//LToCStrN
    pubmsg.payload = msgdata;
    pubmsg.payloadlen = msglen;
    pubmsg.qos = qos;
    pubmsg.retained = 0;
	int r = MQTTAsync_sendMessage(client, topic, &pubmsg, respOpts);
    if (r != MQTTASYNC_SUCCESS) {
		return r;
	}
	return respOpts->token;
}



int32 LVMQTT_Subscribe(lvmqtt_context * lvctx, char * topic, int qos) {
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    int rc;
    opts.onSuccess = LVMQTTAsync_SendS;
	opts.onFailure = LVMQTTAsync_SendF;
    opts.context = lvctx;

	if ((rc = MQTTAsync_subscribe(lvctx->client, topic, qos, &opts)) != MQTTASYNC_SUCCESS) {
		return rc;
	}
	return opts.token;
	
}
