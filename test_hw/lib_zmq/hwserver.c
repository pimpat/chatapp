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
        printf("str: %s\n",str);
        
        int numtype;
        char *token = strtok(str,"::");
        numtype = atoi(token);
        
        //  login
        if(numtype==1){
            printf("----------------------\n");
            token = strtok(NULL,"::");
            char *usrname = strdup(token);
            free(str);
            printf("new_con_user: %s\n",usrname);

            //  Login User
            TPUserID userID;
//            memcpy(userID,usrname,strlen(usrname)+1);
            memset(&userID,0,sizeof(userID));
            strcpy(userID,usrname);
            ret = TPLoginUser(context, &userID);
            if (ret != 0) {
                fprintf(stderr, "Client loginUser return (%d)\n", ret);
            }
            s_send(responder,"1:login success");
        }
        //  logout
        if(numtype==0){
            printf("----------------------\n");
            token = strtok(NULL,"::");
            char *usrname = strdup(token);
            free(str);
            
            //  Logout User
            printf("dis_con_user: %s\n",usrname);
            TPUserID userID;
//            memcpy(userID,usrname,strlen(usrname)+1);
            memset(&userID,0,sizeof(userID));
            strcpy(userID,usrname);
            ret = TPLogoutUser(context, &userID);
            if (ret != 0) {
                fprintf(stderr, "Client logoutUser return (%d)\n", ret);
            }
            s_send(responder,"0:disconnect success");
        }
        //  send msg
        if(numtype==2){
            printf("----------------------\n");
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
            strcpy(sender,mySender);
//            memcpy(target,myTarget,strlen(myTarget)+1);
            memset(&target,0,sizeof(target));
            strcpy(target,myTarget);
            
            message.msg = msg;
            message.bytesCount = (int)strlen(msg);
            printf("msg: %s\tbytes: %d\n",message.msg,message.bytesCount);
            message.msgType = TP_MSG_TYPE_CHAT;
            ret = TPSendToUser(context, &sender, &target, &message);
            if (ret != 0) {
                fprintf(stderr, "Client sendToUser return (%d)\n", ret);
            }
            s_send(responder,"2:received msg");
        }
        //  read msg
        if(numtype==3){
            printf("----------------------\n");
            token = strtok(NULL,"::");
            char *myUserID = strdup(token);
            free(str);
            char myMsg[400]="3";
            printf("myUserID: %s\n",myUserID);
            
            TPUserID userID;
            TPTransportMsg *message;
//            memcpy(userID,myUserID,strlen(myUserID)+1);
            memset(&userID,0,sizeof(userID));
            strcpy(userID,myUserID);
            if (strcmp((char *)&userID, "NULL") != 0) {
                message = TPMsgGet(context, &userID, TP_MSG_TYPE_CHAT);
            }
            else {
                message = TPMsgGet(context, NULL, TP_MSG_TYPE_CHAT);
            }
            if (message != NULL) {
                message->msg[message->bytesCount] = '\0';
                printf("%s: %s\n", message->from, message->msg);
                strcat(myMsg,":");
                strcat(myMsg,myUserID);
                strcat(myMsg,":");
                strcat(myMsg,message->from);
                strcat(myMsg,":");
                strcat(myMsg,message->msg);
                TPMsgFree(message);
                printf("myMsg: %s\n",myMsg);
                s_send(responder,myMsg);
            } else {
                printf("not found message\n");
                s_send(responder,"3:none");
            }
            
        }
        //  list online users
        if(numtype==4){
            free(str);
            printf("----------------------\n");
            TPArrayUser *arrayUser = NULL;
            char userList[200] ="4";
            ret = TPListAllUser(context, &arrayUser);
            if (ret == 0) {
                int i;
                for (i=0; i<arrayUser->userLen; i++) {
//                    printf("'%s': %s\n", arrayUser->users[i].userID, arrayUser->users[i].status==TP_STATUS_ONLINE? "online": "offline");
                    if(arrayUser->users[i].status==TP_STATUS_ONLINE){
                        strcat(userList,":");
                        strcat(userList,arrayUser->users[i].userID);
                    }
                }
                printf("userList: %s\n",userList);
                s_send(responder,userList);
            }
            else {
                fprintf(stderr, "Client listAllUser return (%d)\n", ret);
            }
            TPArrayUserFree(arrayUser);
        }
        if(numtype==5){
            char refID[16];
            char senderID[16];
            time_t timestamp = time(0);
            int limit = 10;
            
            char chatHist[1000] ="5";
            
            printf("----------------------\n");
            
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
                    printf("%s: '%s' @%lu\n", arrayMsg[i]->from, arrayMsg[i]->msg, arrayMsg[i]->timestamp);
                    free(arrayMsg[i]);
                    strcat(chatHist,":");
                    strcat(chatHist,arrayMsg[i]->from);
                    strcat(chatHist,":");
                    strcat(chatHist,arrayMsg[i]->msg);
                }
            }
            else {
                fprintf(stderr, "Client getChatFromDb return (%d)\n", ret);
            }
            s_send(responder,chatHist);
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
