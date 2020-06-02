#ifndef __SIP_RESPONSE_H__
#define __SIP_RESPONSE_H__

#include "../StdAfx.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include "string.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define REGISTER1 "CSeq: 1 REGISTER"
#define REGISTER2 "CSeq: 2 REGISTER"
#define REGISTER3 "CSeq: 3 REGISTER"
#define SUBSCRIBE1 "CSeq: 1 SUBSCRIBE"
#define SUBSCRIBE2 "CSeq: 2 SUBSCRIBE"
#define REGISTER "REGISTER"
#define SUBSCRIBE "SUBSCRIBE"
#define NOTIFY "NOTIFY"
#define INVITE "INVITE"
#define PRACK "PRACK"
#define MESSAGE "MESSAGE"
#define UPDATE "UPDATE"
#define CANCEL "CANCEL"
#define BYE "BYE"


int Get_Position(char *S1, const char *Head);
int Get_NextLine(char *S1);
SIP_Command_E Get_request(char *S11, int caller, int register_flag);
void res_auth_401(char *S44, char *S11);
void res_200_ok(char *S44, char *S11);
void res_202_Accepted(char *S44, char *S11);
void res_test_200_ok(char *S44, char *S11);
void res_test_100_trying(char *S44, char *S11, sockaddr_in *lpAddr);
void res_test_180_ringing(char *S44, char *S11, sockaddr_in *lpAddr);
void res_183_for_invite(char *S44, char *S11);
void res_200_for_invite(char *S44, char *S11, sockaddr_in *lpAddr);
void make_register_test(char *S44);
void make_register_UnAuth(char *S44);
void make_register_Auth(char *S44, char *S11);
void make_invite(char *S44);
void make_prack(char *S44, char *S11);
void make_update(char *S44, char *S11);
void make_ack(char *S44, char *S11);
void make_bye(char *S44, char *S11);
void res_200_for_register(char *S44, char *S11);
void res_200_ok_for_prack(char *S45, char *S11);
void res_test_update(char *S44, char *S11);
void res_test_180_ring(char *S44, char *S11);
void res_test_200_ok_ring(char *S44, char *S11);
void res_sub_200_ok(char *S44, char *S11);
void res_notify(char *S44, char *S11);
void get_sdp_addr(char *S11, sockaddr_in *pAddr);



#endif	// __SIP_RESPONSE_H__