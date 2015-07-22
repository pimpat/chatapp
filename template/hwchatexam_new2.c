//
//  hwchatexam.c
//  Router_Client
//
//  Created by Pimpat on 9/10/2557 BE.
//  Copyright (c) 2557 Pimpat. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "transport.h"
#include <pthread.h>
#include <jansson.h>
#define FUNCHANDLE_N 17


typedef struct threadParam {
    int starter;
    int total;
    char myUser[100][10];
    TPTransportCTX* myContext;
    
}threadParam;
extern threadParam* tParam;
threadParam* tParam = NULL;

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

void inviteMsg(TPTransportCTX *context, char* username) {
    TPUserID userID;
    TPTransportMsg *message;
    
    memset(&userID, 0, sizeof(userID));
    strcpy((char*)userID,username);
    
    if (strcmp((char *)&userID, "NULL") != 0) {
        message = TPMsgGet(context, &userID, TP_MSG_TYPE_CHATCONTROL);
    }
    else {
        message = TPMsgGet(context, NULL, TP_MSG_TYPE_CHATCONTROL);
    }
    if (message != NULL) {
        message->msg[message->bytesCount] = '\0';
        printf("'%s'->'%s': '%s' @%lu\n", message->from, message->to, message->msg, message->timestamp);
        
        TPGroupID groupRef;
        memset(&groupRef, 0, sizeof(groupRef));
//        strcpy((char *)groupRef,message->msg);
        
//        printf("json-msg(recv): %s\n",message->msg);
        json_t *root = load_json(message->msg);
        json_t *result = json_object_get(root,"groupID");
        char* myresult = (char*)json_string_value(result);
        strcpy((char *)groupRef,myresult);

        int ret = TPGroupJoin(context, &userID, &groupRef);
        if (ret != 0) {
            fprintf(stderr, "Client joinGroup return (%d)\n", ret);
        }
        else{
            printf("'%s' join group '%s' done!!\n",userID,groupRef);
        }
        
        TPMsgFree(message);
    } else {
//        printf("not found message\n");
    }
}

void *test_func(void *arg){
    threadParam* my_struct = arg;
    while(1){
        if(my_struct->starter==1){
            int i;
            for(i=0;i<my_struct->total;i++){
                if(strcmp(tParam->myUser[i],"") != 0){
//                    printf("name: %s\n",my_struct->myUser[i]);
                    inviteMsg(my_struct->myContext,my_struct->myUser[i]);
                }
            }
            sleep(5);
        }
    }
}

static void cleanStdin() { while (getchar()!='\n') ;}
int getIdFromConfig(TPTransportCTX *context, char *configFile, char *id);

// function call by tag (set in main 'setTagFuncCall(callHello, 1);' )
static unsigned int callHello(TPTransportMsg *message) {
    char *msg = malloc(message->bytesCount+1);
    memcpy(msg, message->msg, message->bytesCount);
    msg[message->bytesCount] = '\0';
    printf("tag1:%s\n", message->msg);
    free(msg);
    return 0;
}

static void _1ReadMessage(TPTransportCTX *context) {
    TPUserID userID;
    TPTransportMsg *message;
    printf("user_id:\n");
    printf("> ");
    memset(&userID, 0, sizeof(userID));
    scanf("%s", (char *)&userID);
    if (strcmp((char *)&userID, "NULL") != 0) {
        message = TPMsgGet(context, &userID, TP_MSG_TYPE_CHAT);
    }
    else {
        message = TPMsgGet(context, NULL, TP_MSG_TYPE_CHAT);
    }
    if (message != NULL) {
        message->msg[message->bytesCount] = '\0';
        printf("'%s'->'%s': '%s' @%lu\n", message->from, message->to, message->msg, message->timestamp);
        TPMsgFree(message);
    } else {
        printf("not found message\n");
    }
}
static void _2SendToClient(TPTransportCTX *context) {
    int ret;
    char msg[256];
    TPTransportMsg message;
    TPDeviceID target;
    printf("receiver_id:\n");
    printf("> ");
    memset(&target, 0, sizeof(target));
    scanf("%s", (char *)&target);
    printf("msg:\n");
    printf("> ");
    scanf("%s", msg);
    message.msg = msg;
    message.bytesCount = (int)strlen(msg);
    message.msgType = TP_MSG_TYPE_CHAT;
    ret = TPSendToClient(context, &target, &message);
    if (ret != 0) {
        fprintf(stderr, "Client sendToClient return (%d)\n", ret);
    }
}
static void _3SendToUser(TPTransportCTX *context) {
    int ret;
    char msg[256];
    TPTransportMsg message;
    TPUserID sender;
    TPUserID target;
    printf("sender_name:\n");
    printf("> ");
    memset(&sender, 0, sizeof(sender));
    scanf("%s", (char *)&sender);
    printf("receiver_name:\n");
    printf("> ");
    memset(&target, 0, sizeof(target));
    scanf("%s", (char *)&target);
    printf("msg:\n");
    printf("> ");
    scanf("%s", msg);
    message.msg = msg;
    message.bytesCount = (int)strlen(msg);
    message.msgType = TP_MSG_TYPE_CHAT;
    ret = TPSendToUser(context, &sender, &target, &message);
    if (ret != 0) {
        fprintf(stderr, "Client sendToUser return (%d)\n", ret);
    }
}
static void _4SendToGroup(TPTransportCTX *context) {
    int ret;
    char msg[256];
    TPTransportMsg message;
    TPUserID sender;
    TPGroupID groupRef;
    printf("sender:\n");
    printf("> ");
    memset(&sender, 0, sizeof(sender));
    scanf("%s", (char *)&sender);
    printf("group_id:\n");
    printf("> ");
    memset(&groupRef, 0, sizeof(groupRef));
    scanf("%s", (char *)&groupRef);
    printf("msg:\n");
    printf("> ");
    scanf("%s", msg);
    message.msg = msg;
    message.bytesCount = (int)strlen(msg);
    message.msgType = TP_MSG_TYPE_CHAT;
    ret = TPSendToGroup(context, &sender, &groupRef, &message);
    if (ret != 0) {
        fprintf(stderr, "Client sendToGroup return (%d)\n", ret);
    }
}
static void _5SendTagToClient(TPTransportCTX *context) {
    int ret;
    TPTransportMsg message;
    int tag;
    char msg[256];
    TPDeviceID target;
    printf("tag:\n");
    printf("> ");
    scanf("%u", &tag);
    printf("receiver_id:\n");
    printf("> ");
    memset(&target, 0, sizeof(target));
    scanf("%s", (char *)&target);
    printf("msg:\n");
    printf("> ");
    scanf("%s", msg);
    message.msg = msg;
    message.bytesCount = (int)strlen(msg);
    ret = TPSendTagToClient(context, &target, &message, tag);
    if (ret != 0) {
        fprintf(stderr, "Client sendTagToClient return (%d)\n", ret);
    }
}
static void _6SendTagToGroup(TPTransportCTX *context) {
    int ret;
    int tag;
    char msg[256];
    TPTransportMsg message;
    TPUserID sender;
    TPGroupID groupRef;
    printf("tag:\n");
    printf("> ");
    scanf("%d", &tag);
    printf("sender:\n");
    printf("> ");
    memset(&sender, 0, sizeof(sender));
    scanf("%s", (char *)&sender);
    printf("group_id:\n");
    printf("> ");
    memset(&groupRef, 0, sizeof(groupRef));
    scanf("%s", (char *)&groupRef);
    printf("msg:\n");
    printf("> ");
    scanf("%s", msg);
    message.msg = msg;
    message.bytesCount = (int)strlen(msg);
    ret = TPSendTagToGroup(context, &sender, &groupRef, &message, tag);
    if (ret != 0) {
        fprintf(stderr, "Client sendTagToGroup return (%d)\n", ret);
    }
}
static void _7ListChatHistory(TPTransportCTX *context) {
    int ret;
    char refID[16];
    char senderID[16];
    time_t timestamp = time(0);
    int limit = 20;
    TPTransportMsg **arrayMsg;
    printf("refID:\n> ");
    memset(refID, 0, sizeof(refID));
    scanf("%s", refID);
    printf("senderID:\n> ");
    memset(senderID, 0, sizeof(senderID));
    scanf("%s", senderID);
    if (strcmp(senderID, "NULL") == 0) {
        ret = TPMsgGetFromDBWithSender(context, refID, sizeof(refID), NULL, 0, timestamp, limit, &arrayMsg);
    }
    else {
        ret = TPMsgGetFromDBWithSender(context, refID, sizeof(refID), senderID, sizeof(senderID), timestamp, limit, &arrayMsg);
    }
    if (ret != -1) {
        int i;
        for (i=0; i<ret; i++) {
            printf("'%s'->'%s': '%s' @%lu\n", arrayMsg[i]->from, arrayMsg[i]->to, arrayMsg[i]->msg, arrayMsg[i]->timestamp);
            TPMsgFree(arrayMsg[i]);
        }
    }
    else {
        fprintf(stderr, "Client getChatFromDb return (%d)\n", ret);
    }
}
static void _8NewGroup(TPTransportCTX *context) {
    int ret;
    TPGroupID groupRef;
    printf("groupRef:\n");
    printf("> ");
    memset(&groupRef, 0, 16);
    scanf("%s", (char *)&groupRef);
    ret = TPGroupNew(context, &groupRef);
    if (ret != 0) {
        fprintf(stderr, "Client newTPGroupID return (%d)\n", ret);
    }
}
static void _9JoinGroup(TPTransportCTX *context) {
    int ret;
    TPUserID userID;
    TPGroupID groupRef;
    printf("userID:\n");
    printf("> ");
    memset(&userID, 0, sizeof(userID));
    scanf("%s", (char *)&userID);
    printf("group_id:\n");
    printf("> ");
    memset(&groupRef, 0, sizeof(groupRef));
    scanf("%s", (char *)&groupRef);
    ret = TPGroupJoin(context, &userID, &groupRef);
    if (ret != 0) {
        fprintf(stderr, "Client joinGroup return (%d)\n", ret);
    }
}
static void _10LeaveGroup(TPTransportCTX *context) {
    int ret;
    TPUserID userID;
    TPGroupID groupRef;
    printf("userID:\n");
    printf("> ");
    memset(&userID, 0, sizeof(userID));
    scanf("%s", (char *)&userID);
    printf("group_id:\n");
    printf("> ");
    memset(&groupRef, 0, sizeof(groupRef));
    scanf("%s", (char *)&groupRef);
    ret = TPGroupLeave(context, &userID, &groupRef);
    if (ret != 0) {
        fprintf(stderr, "Client leaveGroup return (%d)\n", ret);
    }
}
static void _11ListUser(TPTransportCTX *context) {
    int ret;
    TPArrayUser *arrayUser = NULL;
    ret = TPListAllUser(context, &arrayUser);
    if (ret == 0) {
        int i;
        for (i=0; i<arrayUser->userLen; i++) {
            printf("'%s': %s\n", arrayUser->users[i].userID, arrayUser->users[i].status==TP_STATUS_ONLINE? "online": "offline");
        }
    }
    else {
        fprintf(stderr, "Client listAllUser return (%d)\n", ret);
    }
    TPArrayUserFree(arrayUser);
}
static void _12ListGroup(TPTransportCTX *context) {
    int ret;
    TPUserID userID;
    TPGroupID *groupIDs = NULL;
    printf("userID:\n> ");
    memset(&userID, 0, sizeof(userID));
    scanf("%s", (char *)userID);
    ret = TPListGroup(context, &userID, &groupIDs);
    if (ret != -1) {
        printf("num group:%d\n", ret);
        int i;
        for (i=0; i<ret; i++) {
            printf("groupID: '%s'\n", groupIDs[i]);
        }
        free(groupIDs);
    }
    else {
        fprintf(stderr, "Client listGroup return (%d\n", ret);
    }
}
static void _13ListMemberInGroup(TPTransportCTX *context) {
    int ret;
    TPUserID userID;
    TPGroupID groupID;
    TPArrayUser *arrayUser = NULL;
    printf("userID:\n> ");
    memset(&userID, 0, sizeof(userID));
    scanf("%s", (char *)userID);
    printf("groupID:\n> ");
    memset(&groupID, 0, sizeof(groupID));
    scanf("%s", (char *)groupID);
    ret = TPListMemberInGroup(context, &userID, &groupID, &arrayUser);
    if (ret == 0) {
        printf("'%s': member %d\n", (char *)groupID, arrayUser->userLen);
        int i;
        for (i=0; i<arrayUser->userLen; i++) {
            printf("'%s': \n", arrayUser->users[i].userID);
        }
    }
}
static void _14ManageSocket(TPTransportCTX *context) {
    printf("Manage: \n");
    printf("\t1: Reset\n");
    printf("\t2: Pause\n");
    printf("\t3: Resume\n");
    printf("> ");
    int ret = -1;
    scanf("%d", &ret);
    cleanStdin();
    if (ret == 1) {
        ret = TPTransportSocketReset(context);
    }
    else if (ret == 2) {
        ret = TPTransportSocketPause(context);
    }
    else if (ret == 3) {
        ret = TPTransportSocketResume(context);
    }
    else {
        return ;
    }
    if (ret != 0) {
        fprintf(stderr, "Client manage socket return (%d)\n", ret);
    }
}

static void _15GetShadowFromRouter(TPTransportCTX *context) {
    TPGetShadowFromRouter(context);
}
static void _16ListChatHistoryByRange(TPTransportCTX *context) {
    int ret;
    char refID[16];
    time_t start,end;
    int limit;
    TPTransportMsg **arrayMsg;
    printf("refID:\n> ");
    memset(refID, 0, sizeof(refID));
    scanf("%s", refID);
    printf("startTime:\n> ");
    scanf("%ld", &start);
    printf("stopTime:\n> ");
    scanf("%ld", &end);
    printf("Number of msg :\n> ");
    scanf("%d", &limit);
    ret = TPMsgGetFromDBByRange(context, refID, sizeof(refID), start, end, limit, &arrayMsg);
    if (ret != -1) {
        int i;
        for (i=0; i<ret; i++) {
            printf("'%s'->'%s': '%s' @%lu\n", arrayMsg[i]->from, arrayMsg[i]->to, arrayMsg[i]->msg, arrayMsg[i]->timestamp);
            TPMsgFree(arrayMsg[i]);
        }
    }
    else {
        fprintf(stderr, "Client getChatFromDb return (%d)\n", ret);
    }
}
static void _17ListChatHistoryFromShadow(TPTransportCTX *context) {
    int ret;
    char refID[16];
    time_t start,end;
    int limit;
    TPTransportMsg *arrayMsg;
    printf("refID:\n> ");
    memset(refID, 0, sizeof(refID));
    scanf("%s", refID);
    printf("startTime:\n> ");
    scanf("%ld", &start);
    printf("stopTime:\n> ");
    scanf("%ld", &end);
    printf("Number of msg :\n> ");
    scanf("%d", &limit);
    ret = TPGetMsgFromShadow(context, refID, sizeof(refID), start, end, limit, &arrayMsg);
    if(ret != -1) {
        int i;
        for(i=0;i<ret;i++) {
            printf("'%s'->'%s': '%s' @%lu\n",arrayMsg[i].from,arrayMsg[i].to,arrayMsg[i].msg,arrayMsg[i].timestamp);
            free(arrayMsg[i].msg);
        }
        free(arrayMsg);
    }
}

static int callTransport() {
    int ret;
    
START:
    printf("Select Router or Client \n");
    printf("\t1: Router\n");
    printf("\t2: Client\n");
    printf("\t0: Exit\n");
    printf("> ");
    ret = -1;
    scanf("%d", &ret);
    cleanStdin();
    if (ret == 1) {
        TPRouterCTX *router = NULL;
        char configFile[256];
        
        printf("config file:\n");
        printf("> ");
        memset(configFile, 0, sizeof(configFile));
        scanf("%s", configFile);
        ret = TPRouterStartFromConfigFile(&router, configFile);
        if (ret != 0) {
            printf("startRouter return (%d)\n", ret);
            return ret;
        }
        printf("[----Router----]\n");
        do {
            printf("\t0: Exit\n");
            printf("> ");
            ret = -1;
            scanf("%d", &ret);
            cleanStdin();
        } while (ret != 0);
        ret = TPRouterStop(router);
        if (ret == 0) {
            printf("Router is stopped\n");
        }
    }
    else if (ret == 2) {
        void (*funcHandle[FUNCHANDLE_N+1])(TPTransportCTX *);
        funcHandle[1] = _1ReadMessage;
        funcHandle[2] = _2SendToClient;
        funcHandle[3] = _3SendToUser;
        funcHandle[4] = _4SendToGroup;
        funcHandle[5] = _5SendTagToClient;
        funcHandle[6] = _6SendTagToGroup;
        funcHandle[7] = _7ListChatHistory;
        funcHandle[8] = _8NewGroup;
        funcHandle[9] = _9JoinGroup;
        funcHandle[10] = _10LeaveGroup;
        funcHandle[11] = _11ListUser;
        funcHandle[12] = _12ListGroup;
        funcHandle[13] = _13ListMemberInGroup;
        funcHandle[14] = _14ManageSocket;
        funcHandle[15] = _15GetShadowFromRouter;
        funcHandle[16] = _16ListChatHistoryByRange;
        funcHandle[17] = _17ListChatHistoryFromShadow;
        
        TPTransportCTX *context = NULL;
        
        char configFile[256];
        TPDeviceID deviceID;
    CLIENT:
        printf("[----Client----]\n");
        printf("\t1: Start Client\n");
        printf("\t2: Login User\n");
        printf("\t3: Logout User\n");
        printf("\t0: StopClient\n");
        printf("> ");
        ret = -1;
        scanf("%d", &ret);
        cleanStdin();
        if (ret == 1) {
            printf("config file:\n");
            printf("> ");
            memset(configFile, 0, sizeof(configFile));
            scanf("%s", configFile);
            ret = TPTransportInitFromConfigFile(&context, configFile);
            if (ret == 0) {
                TPTagFuncCallSet(context, callHello, 1);
                getIdFromConfig(context, configFile, (char *)deviceID);
                
                char path[256];
                system("mkdir -p chat_history");
                sprintf(path, "./chat_history/%s.db", deviceID);
            }
            else {
                fprintf(stderr, "Client startClient return (%d)\n", ret);
            }
            //  fix
            tParam->myContext = context;
            tParam->starter=1;
            tParam->total=0;
        }
        else if (ret == 2) {
            TPUserID userID;
            printf("userID:\n");
            printf("> ");
            memset(&userID, 0, 16);
            scanf("%s", (char *)&userID);
            
            //  fix
            strcpy(tParam->myUser[tParam->total],(char*)userID);
            tParam->total++;
            
            ret = TPLoginUser(context, &userID);
            if (ret != 0) {
                fprintf(stderr, "Client loginUser return (%d)\n", ret);
                goto CLIENT;
            }
            
        LOGIN:
            printf("[%s]\n", (char *)&userID);
            printf("\t1: Read a message\n");
            printf("\t2: Send message to Client by DeviceID\n");
            printf("\t3: Send message to Client by Name\n");
            printf("\t4: Send message to Group\n");
            printf("\t5: Send tag to Client by DeviceID\n");
            printf("\t6: Send tag to Group\n");
            printf("\t7: List chat history\n");
            printf("\t8: New group\n");
            printf("\t9: Join group\n");
            printf("\t10: Leave group\n");
            printf("\t11: List user\n");
            printf("\t12: List group\n");
            printf("\t13: List member in group\n");
            printf("\t14: Manage Socket\n");
            printf("\t15: Request shadow peer\n");
            printf("\t16: List chat By range\n");
            printf("\t17: List chat By range From Shadow(must request shadow first)\n");
            printf("\t0: Back\n");
        INPUT:
            printf("> ");
            ret = -1;
            scanf("%d", &ret);
            cleanStdin();
            if (ret > 0 && ret <= FUNCHANDLE_N) {
                funcHandle[ret](context);
            }
            if (ret == 0) {
                goto CLIENT;
            }
            else if (ret == -1) {
                goto LOGIN;
            }
            goto INPUT;
        }
        else if (ret == 3) {
            TPUserID userID;
            printf("userID:\n");
            printf("> ");
            memset(&userID, 0, 16);
            scanf("%s", (char *)&userID);
            ret = TPLogoutUser(context, &userID);
            if (ret != 0) {
                fprintf(stderr, "Client logoutUser return (%d)\n", ret);
            }
            //  fix
            int i;
            for(i=0;i<tParam->total;i++){
                if(strcmp(tParam->myUser[i],(char*)userID) == 0){
                    memset(tParam->myUser[i],0,sizeof(tParam->myUser[0]));
                }
            }
        }
        else if (ret == 0) {
            ret = TPTransportClose(context);
            if (ret == 0) {
                printf("Client is stopped\n");
            }
            else {
                fprintf(stderr, "Client stopClient return (%d)\n", ret);
            }
            goto EXIT;
        }
        goto CLIENT;
    }
    else if (ret == 0) {
        goto EXIT;
    }
    else {
        goto START;
    }
EXIT:
    printf("Exit\n");
    
    return 0;
}

int main()
{
    tParam = (threadParam*)malloc(sizeof(threadParam));
//    printf("size: %lu\n",sizeof(tParam->myUser)/sizeof(tParam->myUser[0]));
//    int i;
//    strcpy(tParam->myUser[0],"123");
//    i = strcmp(tParam->myUser[0],"123");
//    printf("%d\n",i);
//    
//    memset(tParam->myUser[0],0,sizeof(tParam->myUser[0]));
//    i = strcmp(tParam->myUser[0],"");
//    printf("%d\n",i);
    
    pthread_t test_thread;
    pthread_create(&test_thread,NULL,test_func,tParam);
    return callTransport();
}

//--------------------------------------------------------------------------------------------------

int getIdFromConfig(TPTransportCTX *context, char *configFile, char *id) {
    TPConfigGet(context, TP_CONFIG_UUID, id, NULL);
    if (strcmp(id, "") == 0) {
        TPConfigGet(context, TP_CONFIG_ID, id, NULL);
    }
    return 0;
}
