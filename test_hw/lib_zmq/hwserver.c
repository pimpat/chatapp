//  Hello World server

#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "zhelpers.h"
#include "transport.h"

int getIdFromConfig(TPTransportCTX *context, char *configFile, char *id);

int main (void)
{
    //  Socket to talk to clients
    void *context2 = zmq_ctx_new ();
    void *responder = zmq_socket (context2, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:5555");
    assert (rc == 0);
    
    TPTransportCTX *context = NULL;
    TPTransportMsg message;
    char configFile[256];
    char deviceID[64];
    int ret;
    
    //  set config
    sprintf(configFile,"%s","/Users/pimpat/Desktop/Transport2/TransportLayer/install/config/config1.conf");
    ret = TPTransportInitFromConfigFile(&context, configFile);
    if (ret == 0) {
        getIdFromConfig(context, configFile, deviceID);
        
        char path[256];
        system("mkdir -p /Users/pimpat/Desktop/Transport2/TransportLayer/install/chat_history");
        sprintf(path, "/Users/pimpat/Desktop/Transport2/TransportLayer/install/chat_history/%s.db", deviceID);
    }
    else {
        fprintf(stderr, "Client startClient return (%d)\n", ret);
    }

    while (1) {
        char *str = s_recv(responder);
//        printf("str: %s\n",str);
        
        int numtype;
        char *token = strtok(str,"::");
        numtype = atoi(token);
        
        //  login
        if(numtype==1){
            printf("[#%d login]----------------------\n",numtype);
            token = strtok(NULL,"::");
            char *usrname = strdup(token);
            free(str);
            printf("username(login): %s\n",usrname);

            //  Login User
            TPUserID userID;
//            memcpy(userID,usrname,strlen(usrname)+1);
            memset(&userID,0,sizeof(userID));
            strcpy((char*)userID,usrname);
            ret = TPLoginUser(context, &userID);
            if (ret != 0) {
                fprintf(stderr, "Client loginUser return (%d)\n", ret);
            }
            char rep_str[100]="1:login success";
            strcat(rep_str,":");
            strcat(rep_str,usrname);
            s_send(responder,rep_str);
//            s_send(responder,"1:login success");
            printf("\n\n");
        }
        //  logout
        if(numtype==0){
            printf("[#%d logout]----------------------\n",numtype);
            token = strtok(NULL,"::");
            char *usrname = strdup(token);
            free(str);
            
            //  Logout User
            printf("username(logout): %s\n",usrname);
            TPUserID userID;
//            memcpy(userID,usrname,strlen(usrname)+1);
            memset(&userID,0,sizeof(userID));
            strcpy((char*)userID,usrname);
            ret = TPLogoutUser(context, &userID);
            if (ret != 0) {
                fprintf(stderr, "Client logoutUser return (%d)\n", ret);
            }
            char rep_str[100]="0:logout success";
            strcat(rep_str,":");
            strcat(rep_str,usrname);
            s_send(responder,rep_str);
//            s_send(responder,"0:disconnect success");
            printf("\n\n");
        }
        //  send msg
        if(numtype==2){
            printf("[#%d sendMsg]----------------------\n",numtype);
            char msg[256];
            
            token = strtok(NULL,"::");
            char *mySender = strdup(token);
            
            token = strtok(NULL,"::");
            char *myTarget = strdup(token);
            
            token = strtok(NULL,"::");
            sprintf(msg,"%s",token);
            
            free(str);
            
            TPUserID sender;
            TPUserID target;
//            memcpy(sender,mySender,strlen(mySender)+1);
            memset(&sender,0,sizeof(sender));
            strcpy((char*)sender,mySender);
//            memcpy(target,myTarget,strlen(myTarget)+1);
            memset(&target,0,sizeof(target));
            strcpy((char*)target,myTarget);
            
            message.msg = msg;
            message.bytesCount = (int)strlen(msg);
            printf("msg: %s\nbytes: %d\n",message.msg,message.bytesCount);
            message.msgType = TP_MSG_TYPE_CHAT;
            ret = TPSendToUser(context, &sender, &target, &message);
            if (ret != 0) {
                fprintf(stderr, "Client sendToUser return (%d)\n", ret);
            }
            char rep_str[100]="2:send msg success";
            strcat(rep_str,":");
            strcat(rep_str,mySender);
            s_send(responder,rep_str);
//            s_send(responder,"2:received msg");
            printf("\n\n");
        }
        //  read msg
        if(numtype==3){
            printf("[#%d readMsg]----------------------\n",numtype);
            token = strtok(NULL,"::");
            char *myUserID = strdup(token);
            free(str);
            
            char myMsg[400]="3";
            printf("username(req): %s\n",myUserID);
            
            TPUserID userID;
            TPTransportMsg *message;
//            memcpy(userID,myUserID,strlen(myUserID)+1);
            memset(&userID,0,sizeof(userID));
            strcpy((char*)userID,myUserID);
            if (strcmp((char *)&userID, "NULL") != 0) {
                message = TPMsgGet(context, &userID, TP_MSG_TYPE_CHAT);
            }
            else {
                message = TPMsgGet(context, NULL, TP_MSG_TYPE_CHAT);
            }
            if (message != NULL) {
                message->msg[message->bytesCount] = '\0';
//                printf("%s: %s\n", message->from, message->msg);
//                printf("'%s'->'%s': '%s' @%lu\n", message->from, message->to, message->msg, message->timestamp);
                strcat(myMsg,":");
                strcat(myMsg,myUserID);
                strcat(myMsg,":");
                strcat(myMsg,message->from);
                strcat(myMsg,":");
                strcat(myMsg,message->msg);
                strcat(myMsg,":");
                char tstamp[20]="";
                sprintf(tstamp,"%lu",message->timestamp);
//                printf("time: @%lu\n",message->timestamp);
                strcat(myMsg,tstamp);
                TPMsgFree(message);
                printf("send this str: %s\n",myMsg);
                s_send(responder,myMsg);
            } else {
                printf("not found message\n");
                strcat(myMsg,":");
                strcat(myMsg,myUserID);
                strcat(myMsg,":none");
                s_send(responder,myMsg);
//                s_send(responder,"3:none");
            }
            printf("\n\n");
        }
        //  list online users
        if(numtype==4){
            printf("[#%d listOnlineUser]----------------------\n",numtype);
            free(str);
            TPArrayUser *arrayUser = NULL;
            char userList[200] ="4";
            ret = TPListAllUser(context, &arrayUser);
            if (ret == 0) {
                int i;
                for (i=0; i<arrayUser->userLen; i++) {
//                    printf("'%s': %s\n", arrayUser->users[i].userID, arrayUser->users[i].status==TP_STATUS_ONLINE? "online": "offline");
                    if(arrayUser->users[i].status==TP_STATUS_ONLINE){
                        strcat(userList,":");
                        strcat(userList,(char*)arrayUser->users[i].userID);
                    }
                }
                printf("send this str: %s\n",userList);
                s_send(responder,userList);
            }
            else {
                fprintf(stderr, "Client listAllUser return (%d)\n", ret);
            }
            TPArrayUserFree(arrayUser);
            printf("\n\n");
        }
        //  list chat history
        if(numtype==5){
            printf("[#%d listChatHistory]----------------------\n",numtype);
            char refID[16];
            char senderID[16];
            time_t timestamp = time(0);
            int limit = 10;
            
            char chatHist[1000] ="5";
            
            token = strtok(NULL,"::");
            char *myRefID = strdup(token);
            
            token = strtok(NULL,"::");
            char *mySenderID = strdup(token);
            
            free(str);
            
            strcat(chatHist,":");
            strcat(chatHist,myRefID);
//            strcat(chatHist,":");
//            strcat(chatHist,mySenderID);
            
            TPTransportMsg **arrayMsg;
            memset(refID, 0, sizeof(refID));
            strcpy(refID,myRefID);
            
            memset(senderID, 0, sizeof(senderID));
            strcpy(senderID,mySenderID);

            if (strcmp(senderID, "NULL") == 0) {
                ret = TPMsgGetFromDB(context, (void *)refID, timestamp, limit, &arrayMsg);
            }
            else {
                ret = TPMsgGetFromDBWithSender(context, refID, sizeof(refID), senderID, sizeof(senderID), timestamp, limit, &arrayMsg);
            }
            if (ret != -1) {
                int i;
                for (i=0; i<ret; i++) {
//                    printf("%s: '%s' @%lu\n", arrayMsg[i]->from, arrayMsg[i]->msg, arrayMsg[i]->timestamp);
                    free(arrayMsg[i]);
                    strcat(chatHist,":");
                    strcat(chatHist,arrayMsg[i]->from);
                    strcat(chatHist,":");
                    strcat(chatHist,arrayMsg[i]->msg);
                    strcat(chatHist,":");
                    char tstamp[20]="";
                    sprintf(tstamp,"%lu",arrayMsg[i]->timestamp);
                    strcat(chatHist,tstamp);
                }
            }
            else {
                fprintf(stderr, "Client getChatFromDb return (%d)\n", ret);
            }
            printf("send this str: %s\n",chatHist);
            s_send(responder,chatHist);
            printf("\n\n");
        }
    }
    return 0;
}

int getIdFromConfig(TPTransportCTX *context, char *configFile, char *id) {
    TPConfigGet(context, TP_CONFIG_UUID, id, NULL);
    if (strcmp(id, "") == 0) {
        TPConfigGet(context, TP_CONFIG_ID, id, NULL);
    }
    return 0;
}
