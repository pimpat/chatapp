//  Hello World server

#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "zhelpers.h"
#include "transport.h"
#include <jansson.h>

#define TRANSPORT_PATH "/Users/Earth/code/transportlayer"

int getIdFromConfig(TPTransportCTX *context, char *configFile, char *id);

json_t *load_json(const char *text) {
    json_t *root;
    json_error_t error;
    
    root = json_loads(text, 0, &error);
    
    if (root) {
        return root;
    } else {
        fprintf(stderr, "json error on line %d: %s\n", error.line, error.text);
        return (json_t *)0;
    }
}

int main (void)
{
    //  test jansson
    //    char* myJson = "{ \"sender\" : \"pim\", \"action\" : \"join group\", \"groupID\" : \"group1\" }";
    //    char* myKey[] = {"sender","action","groupID"};
    //
    //    json_t *root = load_json(myJson);
    //    json_t *result;
    //    char* myresult;
    //
    //    char str[120];
    //    sprintf(str,"{ \"sender\" : \"%s\", \"action\" : \"join group\", \"groupID\" : \"%s\" }","poom","group2");
    //    json_t *root2 = load_json(str);
    //
    //    int i;
    //    for(i=0;i<3;i++){
    //        result = json_object_get(root2,myKey[i]);
    //        myresult = (char*)json_string_value(result);
    //        printf("value: %s\n",myresult);
    //    }
    
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
    sprintf(configFile,"%s/TransportLayer/install/config/config1.conf", TRANSPORT_PATH);
    ret = TPTransportInitFromConfigFile(&context, configFile);
    if (ret == 0) {
        getIdFromConfig(context, configFile, deviceID);
        
        char command[256];
        sprintf(command, "mkdir -p %s/TransportLayer/install/chat_history", TRANSPORT_PATH);
        system(command);
        
        char path[256];
        sprintf(path, "%s/TransportLayer/install/chat_history/%s.db", TRANSPORT_PATH, deviceID);
    }
    else {
        fprintf(stderr, "Client startClient return (%d)\n", ret);
    }
    
    while (1) {
        char *str = s_recv(responder);
//        printf("str: %s\n",str);
        
        int numtype;
        char *token = strtok(str,":");
        numtype = atoi(token);
        
        //  login
        if(numtype==1){
            printf("[#%d login]----------------------\n",numtype);
            
            token = strtok(NULL,":");
            char *usrname = strdup(token);
            free(str);
            
            printf("username(login): %s\n",usrname);
            
            TPUserID userID;
            memset(&userID,0,sizeof(userID));
            strcpy((char*)userID,usrname);
            
//            ret = TPLoginUser(context, &userID, NULL);
            ret = TPLoginUser(context, &userID);
            if (ret != 0) {
                fprintf(stderr, "Client loginUser return (%d)\n", ret);
            }
            
            char rep_str[100]="1:login success";
            strcat(rep_str,":");
            strcat(rep_str,usrname);
            s_send(responder,rep_str);
            printf("\n\n");
            
            free(usrname);
        }
        //  logout
        if(numtype==0){
            printf("[#%d logout]----------------------\n",numtype);
            
            token = strtok(NULL,":");
            char *usrname = strdup(token);
            free(str);
            
            printf("username(logout): %s\n",usrname);
            
            TPUserID userID;
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
            printf("\n\n");
            
            free(usrname);
        }
        //  send msg
        if(numtype==2){
            printf("[#%d sendMsg]----------------------\n",numtype);
            char msg[256];
            
            token = strtok(NULL,":");
            char *mySender = strdup(token);
            
            token = strtok(NULL,":");
            char *myTarget = strdup(token);
            
            token = strtok(NULL,":");
            sprintf(msg,"%s",token);
            
            free(str);
            
            TPUserID sender;
            TPUserID target;

            memset(&sender,0,sizeof(sender));
            strcpy((char*)sender,mySender);

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
            printf("\n\n");
            
            free(mySender);
            free(myTarget);
        }
        //  read invite msg
        if(numtype==9){
            printf("[#%d readInviteMsg]----------------------\n",numtype);
            
            token = strtok(NULL,":");
            char *myUserID = strdup(token);
            free(str);
            
            char myInviteMsg[400]="9";
            printf("username(req): %s\n",myUserID);
            
            TPUserID userID;
            TPTransportMsg *message;
            
            memset(&userID,0,sizeof(userID));
            strcpy((char*)userID,myUserID);
            
            if (strcmp((char *)&userID, "NULL") != 0) {
                message = TPMsgGet(context, &userID, TP_MSG_TYPE_CHATCONTROL);
            }
            else {
                message = TPMsgGet(context, NULL, TP_MSG_TYPE_CHATCONTROL);
            }
            if (message != NULL) {
                message->msg[message->bytesCount] = '\0';
                strcat(myInviteMsg,":");
                strcat(myInviteMsg,myUserID);
                strcat(myInviteMsg,":");
                strcat(myInviteMsg,message->from);
                strcat(myInviteMsg,":");
//                strcat(myInviteMsg,message->msg);
                
                printf("json-msg(recv): %s\n",message->msg);
                json_t *root = load_json(message->msg);
                json_t *result = json_object_get(root,"groupID");
                char* myresult = (char*)json_string_value(result);
//                printf("value: %s\n",myresult);
                strcat(myInviteMsg,myresult);
                
                TPMsgFree(message);
                
                printf("send this str: %s\n",myInviteMsg);
                s_send(responder,myInviteMsg);
            } else {
                printf("not found invite message\n");
                strcat(myInviteMsg,":");
                strcat(myInviteMsg,myUserID);
                strcat(myInviteMsg,":none");
                s_send(responder,myInviteMsg);
            }
            printf("\n\n");
            
            free(myUserID);
        }
        //  read msg
        if(numtype==3){
            printf("[#%d readMsg]----------------------\n",numtype);
            
            token = strtok(NULL,":");
            char *myUserID = strdup(token);
            free(str);
            
            char myMsg[400]="3";
            printf("username(req): %s\n",myUserID);
            
            TPUserID userID;
            TPTransportMsg *message;
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
                strcat(myMsg,message->to);
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
            }
            printf("\n\n");
            
            free(myUserID);
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
            
            token = strtok(NULL,":");
            char *myRefID = strdup(token);
            
            token = strtok(NULL,":");
            char *mySenderID = strdup(token);
            
            free(str);
            
            strcat(chatHist,":");
            strcat(chatHist,myRefID);
            
            TPTransportMsg **arrayMsg;
            memset(refID, 0, sizeof(refID));
            strcpy((char*)refID,myRefID);
            
            memset(senderID, 0, sizeof(senderID));
            strcpy((char*)senderID,mySenderID);
            
            ret = TPMsgGetFromDBWithSender(context, refID, sizeof(refID), senderID, sizeof(senderID), timestamp, limit, &arrayMsg);

            if (ret != -1) {
                int i;
                for (i=0; i<ret; i++) {
//                    printf("%s: '%s' @%lu\n", arrayMsg[i]->from, arrayMsg[i]->msg, arrayMsg[i]->timestamp);
                    strcat(chatHist,":");
                    strcat(chatHist,arrayMsg[i]->from);
                    strcat(chatHist,":");
                    strcat(chatHist,arrayMsg[i]->msg);
                    strcat(chatHist,":");
                    
                    char tstamp[20]="";
                    sprintf(tstamp,"%lu",arrayMsg[i]->timestamp);
                    strcat(chatHist,tstamp);
                    TPMsgFree(arrayMsg[i]);
                }
            }
            else {
                fprintf(stderr, "Client getChatFromDb return (%d)\n", ret);
            }
            printf("send this str: %s\n",chatHist);
            s_send(responder,chatHist);
            printf("\n\n");
            
            free(myRefID);
            free(mySenderID);
        }
        //  new group
        if(numtype==6){
            printf("[#%d newGroup]----------------------\n",numtype);
            
            token = strtok(NULL,":");
            char *myGroupRef = strdup(token);
//            printf("token1: %s\n",myGroupRef);
            
            token = strtok(NULL,"\n");
            char *myFriendsName = strdup(token);
//            printf("token2: %s\n",myFriendsName);
            
            free(str);
            
            TPGroupID groupRef;
            memset(&groupRef, 0, 16);
            strcpy((char*)groupRef,myGroupRef);
            
            ret = TPGroupNew(context, &groupRef);
            if (ret != 0) {
                fprintf(stderr, "Client newTPGroupID return (%d)\n", ret);
            }
            
            printf("groupName: %s\n",myGroupRef);
            
            char friendsGroupList[300]="6:";
            strcat(friendsGroupList,myGroupRef);
            strcat(friendsGroupList,":");
            strcat(friendsGroupList,myFriendsName);
            friendsGroupList[strlen(friendsGroupList)-1]='\0';
            
            char* token2 = strtok(myFriendsName,":");
            
            TPUserID sender;
            TPUserID target;
            
            char *mySender = strdup(token2);
            memset(&sender,0,sizeof(sender));
            strcpy((char*)sender,mySender);
            
            char msg[256];
//            sprintf(msg,"%s",myGroupRef);
            sprintf(msg,"{ \"sender\" : \"%s\", \"action\" : \"join group\", \"groupID\" : \"%s\" }",mySender,myGroupRef);
            printf("json-msg(send): %s\n",msg);
            
            while (token2!=NULL) {
                char* myTarget = strdup(token2);
                memset(&target,0,sizeof(target));
                strcpy((char*)target,myTarget);
                
                message.msg = msg;
                message.bytesCount = (int)strlen(msg)+1;
//                printf("msg: %s\nbytes: %d\n",message.msg,message.bytesCount);
                message.msgType = TP_MSG_TYPE_CHATCONTROL;
                
                printf("sender: %s\ntarget: %s\nmsg: %s\n",mySender,myTarget,msg);
                
                ret = TPSendToUser(context, &sender, &target, &message);
                if (ret != 0) {
                    fprintf(stderr, "Client sendToUser return (%d)\n", ret);
                }
                
                token2 = strtok(NULL,":");
                free(myTarget);
            }
            printf("send this str: %s\n",friendsGroupList);
            s_send(responder,friendsGroupList);
            printf("\n\n");
            
            free(myGroupRef);
            free(myFriendsName);
            free(mySender);
        }
        //  join group
        if(numtype==7){
            printf("[#%d joinGroup]----------------------\n",numtype);
            
            token = strtok(NULL,":");
            char *myUserID = strdup(token);
            
            token = strtok(NULL,":");
            char *myGroupRef = strdup(token);
            
            free(str);
            
            TPUserID userID;
            TPGroupID groupRef;
            
            memset(&userID, 0, sizeof(userID));
            strcpy((char*)userID,myUserID);
            
            memset(&groupRef, 0, sizeof(groupRef));
            strcpy((char*)groupRef,myGroupRef);
            
            ret = TPGroupJoin(context, &userID, &groupRef);
            if (ret != 0) {
                fprintf(stderr, "Client joinGroup return (%d)\n", ret);
            }
            
            char myMsg[400];
            sprintf(myMsg,"7:'%s' join group '%s' (done)",myUserID,myGroupRef);
            printf("send this str: %s\n",myMsg);
            s_send(responder,myMsg);
            printf("\n\n");
            
            free(myUserID);
            free(myGroupRef);
        }
        //  send msg to group
        if(numtype==8){
            printf("[#%d sendMsgToGroup]----------------------\n",numtype);
            
            char msg[256];
            TPTransportMsg message;
            TPUserID sender;
            TPGroupID groupRef;
            
            token = strtok(NULL,":");
            char *mySender = strdup(token);
            printf("token(sender): %s\n",token);
            
            token = strtok(NULL,":");
            char *myGroupRef = strdup(token);
            
            token = strtok(NULL,":");
            sprintf(msg,"%s",token);
            
            free(str);
            
            memset(&sender, 0, sizeof(sender));
            strcpy((char*)sender,mySender);
            
            memset(&groupRef, 0, sizeof(groupRef));
            strcpy((char*)groupRef,myGroupRef);
            
            message.msg = msg;
            message.bytesCount = (int)strlen(msg);
            
            printf("msg: %s\nbytes: %d\n",message.msg,message.bytesCount);
            message.msgType = TP_MSG_TYPE_CHAT;
            
            ret = TPSendToGroup(context, &sender, &groupRef, &message);
            if (ret != 0) {
                fprintf(stderr, "Client sendToGroup return (%d)\n", ret);
            }
            
            char rep_str[100]="8:send msg to group success";
            strcat(rep_str,":");
            strcat(rep_str,mySender);
            s_send(responder,rep_str);
            printf("\n\n");
            
            free(mySender);
            free(myGroupRef);
        }
        //  list chat group history
        if(numtype==10){
            printf("[#%d listChatGroupHistory]----------------------\n",numtype);
            
            char refID[16];
            time_t timestamp = time(0);
            int limit = 10;
            char chatHist[1000] ="10";
            
            token = strtok(NULL,":");
            char* myName = strdup(token);
            
            token = strtok(NULL,":");
            char *myRefID = strdup(token);
            
            free(str);
            
            strcat(chatHist,":");
            strcat(chatHist,myName);
            
            TPTransportMsg **arrayMsg;
            memset(refID, 0, sizeof(refID));
            strcpy((char*)refID,myRefID);
            
            ret = TPMsgGetFromDBWithSender(context, refID, sizeof(refID), NULL, 0, timestamp, limit, &arrayMsg);
            
            printf("ret: %d\n",ret);
            if (ret != -1) {
                int i;
                for (i=0; i<ret; i++) {
                    printf("'%s'->'%s': '%s' @%lu\n", arrayMsg[i]->from, arrayMsg[i]->to, arrayMsg[i]->msg, arrayMsg[i]->timestamp);
//                    free(arrayMsg[i]);
                    strcat(chatHist,":");
                    strcat(chatHist,arrayMsg[i]->from);
                    strcat(chatHist,":");
                    strcat(chatHist,arrayMsg[i]->msg);
                    strcat(chatHist,":");
                    char tstamp[20]="";
                    sprintf(tstamp,"%lu",arrayMsg[i]->timestamp);
                    strcat(chatHist,tstamp);
                    TPMsgFree(arrayMsg[i]);
                }
            }
            else {
                fprintf(stderr, "Client getChatFromDb return (%d)\n", ret);
            }
            printf("send this str: %s\n",chatHist);
            s_send(responder,chatHist);
            printf("\n\n");
            
            free(myName);
            free(myRefID);
        }
        //  list group
        if(numtype==11){
            printf("[#%d listGroup]----------------------\n",numtype);
            
            token = strtok(NULL,":");
            char *myUserID = strdup(token);
            
            free(str);
            
            TPUserID userID;
            TPGroupID *groupIDs = NULL;
            
            memset(&userID, 0, sizeof(userID));
            strcpy((char*)userID,myUserID);
            
            char myMsg[400]="11:";
            
            ret = TPListGroup(context, &userID, &groupIDs);
            if (ret != -1) {
                printf("num group:%d\n", ret);
                
                char myNum[3];
                strcat(myMsg,myUserID);
                sprintf(myNum,"%d",ret);
                strcat(myMsg,":");
                strcat(myMsg,myNum);
                
                int i;
                for (i=0; i<ret; i++) {
                    printf("groupID: '%s'\n", groupIDs[i]);
                    strcat(myMsg,":");
                    strcat(myMsg,(char*)groupIDs[i]);
                }
                free(groupIDs);
            }
            else {
                fprintf(stderr, "Client listGroup return (%d)\n", ret);
            }
            printf("send this str: %s\n",myMsg);
            s_send(responder,myMsg);
            printf("\n\n");
            
            free(myUserID);
        }
        //  get member group
        if(numtype==12){
            printf("[#%d getMemberGroup]----------------------\n",numtype);
            
            token = strtok(NULL,":");
            char *myUserID = strdup(token);
            
            token = strtok(NULL,":");
            char *myGroupID = strdup(token);
            
            free(str);
            
            TPUserID userID;
            TPGroupID groupID;
            TPArrayUser *arrayUser = NULL;
            
            memset(&userID, 0, sizeof(userID));
            strcpy((char*)userID,myUserID);
            
            memset(&groupID, 0, sizeof(groupID));
            strcpy((char*)groupID,myGroupID);
            
            char mbList[400]="12:";
            strcat(mbList,myUserID);
            strcat(mbList,":");
            strcat(mbList,myGroupID);
            
            ret = TPListMemberInGroup(context, &userID, &groupID, &arrayUser);
            if (ret == 0) {
                printf("'%s': member %d\n", (char *)groupID, arrayUser->userLen);
                
                strcat(mbList,":");
                char myNum[3];
                sprintf(myNum,"%d",arrayUser->userLen);
                strcat(mbList,myNum);
                
                int i;
                for (i=0; i<arrayUser->userLen; i++) {
                    printf("'%s': \n", arrayUser->users[i].userID);
                    strcat(mbList,":");
                    strcat(mbList,(char*)arrayUser->users[i].userID);
                }
            }
            printf("send this str: %s\n",mbList);
            s_send(responder,mbList);
            printf("\n\n");
            
            free(myUserID);
            free(myGroupID);
        }
        //  leave group
        if(numtype==13){
            printf("[#%d leaveGroup]----------------------\n",numtype);
            
            token = strtok(NULL,":");
            char *myUserID = strdup(token);
            
            token = strtok(NULL,":");
            char *myGroupRef = strdup(token);
            
            free(str);
            
            TPUserID userID;
            TPGroupID groupRef;
            
            memset(&userID, 0, sizeof(userID));
            strcpy((char*)userID,myUserID);
            
            memset(&groupRef, 0, sizeof(groupRef));
            strcpy((char*)groupRef,myGroupRef);
            
            ret = TPGroupLeave(context, &userID, &groupRef);
            if (ret != 0) {
                fprintf(stderr, "Client leaveGroup return (%d)\n", ret);
            }
            
            char myMsg[400];
            sprintf(myMsg,"13:'%s' leave group '%s' (done)",myUserID,myGroupRef);
            printf("send this str: %s\n",myMsg);
            s_send(responder,myMsg);
            printf("\n\n");
            
            free(myUserID);
            free(myGroupRef);
        }
        else if(numtype==14){
            printf("[#%d search]----------------------\n", numtype);
            
            token = strtok(NULL,":");
            char *keyword  = strdup(token);
            
            printf(">>>>>>>>>>>>>> keyword %s\n", keyword);
            
            free(str);
            
            // go search

            char myMsg[400];
            sprintf(myMsg,"14:search keyword '%s'", keyword);
            printf("send this str: %s\n", myMsg);
            s_send(responder, myMsg);
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


