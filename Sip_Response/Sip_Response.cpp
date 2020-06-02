#include "stdafx.h"
#include <stdlib.h>
#include "string.h"
#include "Sip_Response.h"

int Get_Position(char *S1,char *Head)
{
	int  i,j;
	for (i=0;i<(int)(strlen(S1)-strlen(Head));i++)
	{   j=0;
		if ((S1[i]|0x20)==(Head[0]|0x20)) 
		{ 
			for (j=1;j<(int)strlen(Head);j++) 
				if ( (S1[i+j]|0x20) != (Head[j]|0x20) ) 
					break; 
		}
		if (j>=(int)strlen(Head)) break;  
	}

	if ( i< (int)(strlen(S1)-strlen(Head)) ) return i;
	else                                     return -1;
}


int Get_NextLine(char *S1)
{
	int  i;
	for (i=0;i<(int)strlen(S1);i++) if ((S1[i]==0x0a)||(S1[i]==0x0d)) break;
	if (i<(int)strlen(S1))
	{	 if ((S1[i]==0x0d)&&(S1[i+1]==0x0a))	return i+2; else return -1;
	}
	else return -1;
}


int Get_request(char* S11,int caller){
	
	char *header_name[] = {"CSeq","Expires"};
	char q[100]={0};
	int len = Get_NextLine(&S11[Get_Position(S11,header_name[0])])-2;
	
	memcpy(q,&S11[Get_Position(S11,header_name[0])],len);
	printf("Len:%d-%d %s",len,strncmp(q,REGISTER1,len),q);

	if(!strncmp(q,REGISTER1,len)){
		char p[50] = {0};
		int c_len = Get_NextLine(&S11[Get_Position(S11,header_name[1])])-2;
		memcpy(p,&S11[Get_Position(S11,header_name[1])],c_len);
		printf("AAAA:%s\n",p);
		if(!strncmp(p,"Expires: 120",c_len)){
			printf("\nENTER Test_Client_200\n");
			return 12;
		}
		
		printf("\nENTER 401\n");	
		return 1;
	}
	
	else if(!strncmp(q,REGISTER2,len)){
		printf("\nENTER 200_OK\n");
		return 2;
	}
	
	else if(!strncmp(q,REGISTER3,len)){
		printf("\nENTER SUBSCRIBE_200_OK\n");
		return 3;
	}
	
	else if(!strncmp(q,REGISTER4,len)){
		printf("\nENTER TESY INVITE\n");
		return 4;
	}
	
	else if(!strncmp(q,INVITE2,len)){
		printf("\nENTER update\n");
		return 9;
	}
	
	
	//回復100 Trying 183 
	else if(!strncmp(q,INVITE1,len)){
		
		if(!caller)
			return 5;
		
		char qq[100]={0};
		int i_len = Get_NextLine(&S11[Get_Position(S11,"SIP/2.0 ")])-2;
		memcpy(qq,&S11[Get_Position(S11,"SIP/2.0 ")],i_len);
		
		if(!strncmp(qq,"SIP/2.0 100 Trying",i_len)){
			printf("Waitng for next status\n");
			return -1;
		}
		
		else if(!strncmp(qq,"SIP/2.0 183 Session Progress",i_len)){
			printf("\nReturn SIP PRACK\n");
			return 7;
		}
		
		else
			printf("Not Find Response \n");
			return -1;
	}
	
	else if(!strncmp(q,MESSAGE1,len)){
		printf("\nENTER 202 Accepted\n");
		return 6;
	}
	
	else if(!strncmp(q,UPDATE3,len)){
		printf("\nENTER RTP\n");
		return -1;
	}
	
	else{
		printf("\nCan not find the Response\n");
		return 10;
	}
	
	
}



void res_auth_401(char* S44,char* S11){
	char S401[4000],SS[2000],SS2[2000];
	
	int flag=0;
	
	char *header_name[] = {"Via","From","To","Call-ID","Max-Forwards","Expires"
							,"User-Agent","WWW-Authenticate"
							,"Security-Server","CSeq","Require","Content-Length"};

/*
	FILE *fp ;
	if((fp = fopen("Test_cmd3.txt","r")) == NULL){
		printf("File not exist\n");
	}*/

	sprintf(S401,"SIP/2.0 401 Unauthorized\r\n");
	
	while(flag<11){
		char q[500]={0};
		if(flag==7){
			sprintf(SS2,"WWW-Authenticate: Digest realm=\"ims.mnc001.mcc001.3gppnetwork.org\",nonce=\"AAAAAAAAAAAAAAAAAAAAAM8tbD5LmYAAvfNHPvh1+8w=\",algorithm=AKAv1-MD5,qop=\"auth\"\r\n");
			strcat(S401, SS2);
			flag++;
			continue;
		}

		else if(flag==8){
			sprintf(SS2,"Security-Server: ipsec-3gpp;prot=esp;mod=trans;spi-c=2815619481;spi-s=136158516;port-c=50717;port-s=50718;ealg=null;alg=hmac-md5-96;q=0.1\r\n");
			strcat(S401, SS2);
			flag++;
			continue;
		}
		
		else if (flag==10){
			sprintf(SS2,"Require: ACK\r\n");
			strcat(S401, SS2);
			flag++;
			continue;
		}

		else{
			memcpy(q,&S11[Get_Position(S11,header_name[flag])],Get_NextLine(&S11[Get_Position(S11,header_name[flag])]) );
			strcat(S401, q);
			printf("+++++++++++++++\n");
			printf("Start:%d Length:%d\n",Get_Position(S11,header_name[flag]),Get_NextLine(&S11[Get_Position(S11,header_name[flag])]));
			printf("%s",q);
			flag++;
		}
	}
	 	 
	strcat(S401, "\r\n");
	sprintf(S44,S401);	
}


void res_200_ok(char* S44,char* S11){
	char S200[4000],SS2[2000];
	int flag=0;
	char *header_name[] = {"Via","From","To","Call-ID"
							,"User-Agent","P-Associated-URI","P-Associated-URI","Contact","CSeq","Content-Length"};
	
	sprintf(S200,"SIP/2.0 200 OK\r\n");
	
	while(flag<10){
		char q[800]={0};
		if(flag==5){
			sprintf(SS2,"P-Associated-URI: <sip:+886987654321@test.3gpp.com>\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}
		
		else if(flag==6){
			sprintf(SS2,"P-Associated-URI: <tel:+886987654321>\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}
		
		else if(flag==8){
			sprintf(SS2,"Cseq: 2 REGISTER\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}
		
		else{
			memcpy(q,&S11[Get_Position(S11,header_name[flag])],Get_NextLine(&S11[Get_Position(S11,header_name[flag])]) );
			strcat(S200, q);
			printf("+++++++++++++++\n");
			printf("Start:%d Length:%d\n",Get_Position(S11,header_name[flag]),Get_NextLine(&S11[Get_Position(S11,header_name[flag])]));
			printf("%s",q);
			flag++;
			
		}
	}
	strcat(S200, "\r\n");
	sprintf(S44,S200);
	
}


void res_202_Accepted(char* S44,char* S11){
	char S200[4000],SS2[2000];
	int flag=0;
	char *header_name[] = {"Via","From","To","Call-ID"
							,"User-Agent","P-Associated-URI","P-Associated-URI","Contact","CSeq","Content-Length"};
							
	sprintf(S200,"SIP/2.0 202 Accepted\r\n");
	
	
	
	
							
}

// 測試用Client******************************************************
void res_test_200_ok(char* S44,char* S11){
	char S200[4000],SS2[2000];
	int flag=0;
	char *header_name[] = {"Via","From","To","Call-ID","CSeq","Expires","Max-Forwards"
							,"Content-Length","Contact"};
							
	sprintf(S200,"SIP/2.0 200 OK\r\n");

	while(flag<9){
		char q[800]={0};
		if(flag==4){
			sprintf(SS2,"CSeq: 2 REGISTER\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}
		
		else if(flag==5){
			sprintf(SS2,"Expires: 125\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}
		
		else if(flag==8){
			sprintf(SS2,"Contact: <SIP:123@192.168.7.115>\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}
		
		else{
			memcpy(q,&S11[Get_Position(S11,header_name[flag])],Get_NextLine(&S11[Get_Position(S11,header_name[flag])]) );
			strcat(S200, q);
			printf("+++++++++++++++\n");
			printf("Start:%d Length:%d\n",Get_Position(S11,header_name[flag]),Get_NextLine(&S11[Get_Position(S11,header_name[flag])]));
			printf("%s",q);
			flag++;
			
		}
	}
	strcat(S200, "\r\n");
	sprintf(S44,S200);
}
//********************************************************************************

//測試用100 Trying****************************************************************
void res_test_100_trying(char* S44,char* S11){
	char S200[4000],SS2[2000];
	int flag=0;
	char *header_name[] = {"Via","To:","From","Call-ID"
							,"CSeq","Content-Length"};	
							
	sprintf(S200,"SIP/2.0 100 Trying\r\n");
	
	while(flag<6){
		char q[800]={0};
		
		if(flag==5){
			sprintf(SS2,"Content-Length: 0\r\n");
			strcat(S200, SS2);
			flag++;
			break;
		}
		
		if(flag==0){
			
			sprintf(SS2,"Via: SIP/2.0/UDP 192.168.200.10;rport=5060;");
			memcpy(q,&S11[Get_Position(S11,"branch=")],Get_NextLine(&S11[Get_Position(S11,"branch=")]) );
			strcat(SS2, q);
			strcat(S200, SS2);
			flag++;
			continue;
		}
		/*
		if(flag==4){
			sprintf(SS2,"CSeq: 2 INVITE\r\n");
			strcat(S200, SS2);
			flag++;
			break;
		}*/
	
		memcpy(q,&S11[Get_Position(S11,header_name[flag])],Get_NextLine(&S11[Get_Position(S11,header_name[flag])]) );
		strcat(S200, q);
		printf("+++++++++++++++\n");
		printf("Start:%d Length:%d\n",Get_Position(S11,header_name[flag]),Get_NextLine(&S11[Get_Position(S11,header_name[flag])]));
		printf("%s",q);
		flag++;
				
	}
	strcat(S200, "\r\n");
	sprintf(S44,S200);

}
//*********************************************************************************


//測試用183 Session Progress*******************************************************
void res_test_183(char* S44,char* S11){
	char S200[4000],SS2[4000];
	int flag=0;
	char *header_name[] = {"Via","To:","From","Call-ID"};	
	
	sprintf(S200,"SIP/2.0 183 Session Progress\r\n");
	
	while(flag<4){
		char q[800]={0};
		
		if(flag==0){
			
			sprintf(SS2,"Via: SIP/2.0/UDP 192.168.200.10;rport=5060;received=192.168.200.10;");
			memcpy(q,&S11[Get_Position(S11,"branch=")],Get_NextLine(&S11[Get_Position(S11,"branch=")]) );
			strcat(SS2, q);
			strcat(S200, SS2);
			flag++;
			continue;
		}
		
		if(flag==1){
			
			sprintf(SS2,"To: <sip:0987654322;phone-context=001.01.eps.ims.mnc001.mcc001.3gppnetwork.org>;tag=PfZGsALGfGIAuK\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}
		
		memcpy(q,&S11[Get_Position(S11,header_name[flag])],Get_NextLine(&S11[Get_Position(S11,header_name[flag])]) );
		strcat(S200, q);
		printf("+++++++++++++++\n");
		printf("Start:%d Length:%d\n",Get_Position(S11,header_name[flag]),Get_NextLine(&S11[Get_Position(S11,header_name[flag])]));
		printf("%s",q);
		flag++;
		
	}	
							
	char szTest[1000] = {0};						
	FILE *fp = fopen("Test_1832.txt", "r");
	while(!feof(fp))
	{
		memset(szTest, 0, sizeof(szTest));
		fgets(szTest, sizeof(szTest) - 1, fp); // 包含了換行符
		printf("%s", szTest);

		strcat(S200,szTest);
	}
	fclose(fp);
	
	sprintf(S44,S200);
}
							
//********************************************************************************

//測試用200 PRACK*****************************************************************
void res_test_200_prack(char* S44,char* S11){
	char S200[4000],SS2[2000];
	int flag=0;
	char *header_name[] = {"Call-ID","To:","From","CSeq","Content-Length","Via","Max-Forwards","RAck","Contact"};	
	
	sprintf(S200,"PRACK sip:192.168.200.10:5060 SIP/2.0\r\n");
	
	while(flag<9){
		char q[800]={0};
		
		if(flag==3){
			
			sprintf(SS2,"CSeq: 2 PRACK\r\n");
			strcat(SS2, q);
			strcat(S200, SS2);
			flag++;
			continue;
		}
		
		if(flag==4){
			
			sprintf(SS2,"Content-Length: 0\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}
		
		if(flag==6){
			
			sprintf(SS2,"Max-Forwards: 68\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}
		
		if(flag==7){
			
			sprintf(SS2,"RAck: 67 1 INVITE\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}
		
		if(flag==8){
			
			sprintf(SS2,"Contact: <sip:boting-000001@192.168.7.116:5060;comp=sigcomp;encode-parm=QbkRBthOEgsTXgkTBAOHHiUrKz1CQEFERUZBNhkLGh1sMTcgK2ghOyAnOCs.ITogYX9jZGV40zQ6PQAcRVtZQlIWWEBaSU8BDAlLEhBSXFpdXA__;transport=udp>;+sip.instance=\"<urn:gsma:imei:35611207-004302-0>\";+g.3gpp.icsi-ref=\"urn%%3Aurn-7%%3A3gpp-service.ims.icsi.mmtel\"\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}
		
		memcpy(q,&S11[Get_Position(S11,header_name[flag])],Get_NextLine(&S11[Get_Position(S11,header_name[flag])]) );
		strcat(S200, q);
		printf("+++++++++++++++\n");
		printf("Start:%d Length:%d\n",Get_Position(S11,header_name[flag]),Get_NextLine(&S11[Get_Position(S11,header_name[flag])]));
		printf("%s",q);
		flag++;
		
	}	
	strcat(S200, "\r\n");
	sprintf(S44,S200);
	
}

//********************************************************************************


//測試用Update *******************************************************************
void res_test_update(char* S44,char* S11){
	char S200[4000],SS2[2000];
	int flag=0;
	char *header_name[] = {"Call-ID","To:","From","CSeq","Via","Content-Length","Content-Type"};	
	
	sprintf(S200,"UPDATE sip:192.168.200.10:5060 SIP/2.0\r\n");
	
	
	while(flag<7){
		char q[800]={0};
		
		if(flag==3){
			
			sprintf(SS2,"CSeq: 3 UPDATE\r\n");
			strcat(SS2, q);
			strcat(S200, SS2);
			flag++;
			continue;
		}
		

		else if(flag==6){
			
			sprintf(SS2,"Content-Type: application/sdp\r\n");
			strcat(SS2, q);
			strcat(S200, SS2);
			flag++;
			continue;
		}
		
		
		
		memcpy(q,&S11[Get_Position(S11,header_name[flag])],Get_NextLine(&S11[Get_Position(S11,header_name[flag])]) );
		strcat(S200, q);
		printf("+++++++++++++++\n");
		printf("Start:%d Length:%d\n",Get_Position(S11,header_name[flag]),Get_NextLine(&S11[Get_Position(S11,header_name[flag])]));
		printf("%s",q);
		flag++;
		
	}	
	
	char szTest[1000] = {0};						
	FILE *fp = fopen("Test_update.txt", "r");
	while(!feof(fp))
	{
		memset(szTest, 0, sizeof(szTest));
		fgets(szTest, sizeof(szTest) - 1, fp); // 包含了換行符
		printf("%s", szTest);

		strcat(S200,szTest);
	}
	fclose(fp);
	
	sprintf(S44,S200);

}
//********************************************************************************

//測試用180 Ringing***************************************************************
void res_test_180_ring(char* S44,char* S11){
	char S200[4000],SS2[2000];
	int flag=0;
	char *header_name[] = {"Call-ID","Via","To:","From","CSeq","Content-Length","Contact","Allow"};	
							
							
	sprintf(S200,"SIP/2.0 180 Ringing\r\n");
	
	while(flag<8){
		char q[800]={0};
	
		if(flag==1){
			sprintf(SS2,"Via: SIP/2.0/UDP 192.168.200.10;rport=5060;received=192.168.200.10;");
			memcpy(q,&S11[Get_Position(S11,"branch=")],Get_NextLine(&S11[Get_Position(S11,"branch=")]) );
			strcat(SS2, q);
			strcat(S200, SS2);
			flag++;
			continue;
		}
		
		else if(flag==2){
			
			sprintf(SS2,"To: <sip:0987654322;phone-context=001.01.eps.ims.mnc001.mcc001.3gppnetwork.org>;tag=PfZGsALGfGIAuK\r\n");
			strcat(S200, SS2);
					
			flag++;
			continue;
		}
		
		else if(flag==5){
			sprintf(SS2,"Content-Length: 0\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}
		
		else if(flag==6){
			sprintf(SS2,"Contact: <sip:boting-000001@192.168.7.109:5060;comp=sigcomp;encode-parm=QbkRBthOEgsTXgkTBA0HHiUrKz1CQEFERUZBNhkLGh1sMTcgK2ghOyAnOCs.ITogYX9jZGV4OzQ6PQAcRVtZQlIWWEBaSU8BDAlLEhBSXFpdXA__;transport=udp>;+sip.instance=\"<urn:gsma:imei:35611207-004302-0>\";+g.3gpp.icsi-ref=\"urn%%3Aurn-7%%3A3gpp-service.ims.icsi.mmtel\";+g.3gpp.mid-call;+g.3gpp.srvcc-alerting\r\n");
			strcat(S200, SS2);
									
			flag++;
			continue;
		}
		
		else{
			memcpy(q,&S11[Get_Position(S11,header_name[flag])],Get_NextLine(&S11[Get_Position(S11,header_name[flag])]) );
			strcat(S200, q);
			printf("+++++++++++++++\n");
			printf("Start:%d Length:%d\n",Get_Position(S11,header_name[flag]),Get_NextLine(&S11[Get_Position(S11,header_name[flag])]));
			printf("%s",q);
			flag++;
		}
				
	}
	strcat(S200, "\r\n");
	sprintf(S44,S200);
}
//********************************************************************************

//測試用200(for 180) OK *******************************************************************
void res_test_200_ok_ring(char* S44,char* S11){
	char S200[4000],SS2[2000];
	int flag=0;
	char *header_name[] = {"Call-ID","Via","To:","From","CSeq","Content-Length","Contact","Allow","Supported","Require"};	
							
							
	sprintf(S200,"SIP/2.0 200 OK\r\n");
	while(flag<10){
		char q[800]={0};
		
		if(flag==1){
			sprintf(SS2,"Via: SIP/2.0/UDP 192.168.200.10;rport=5060;received=192.168.200.10;");
			memcpy(q,&S11[Get_Position(S11,"branch=")],Get_NextLine(&S11[Get_Position(S11,"branch=")]) );
			strcat(SS2, q);
			strcat(S200, SS2);
			flag++;
			continue;
		}
		
		else if(flag==2){
			
			sprintf(SS2,"To: <sip:0987654322;phone-context=001.01.eps.ims.mnc001.mcc001.3gppnetwork.org>;tag=PfZGsALGfGIAuK\r\n");
			strcat(S200, SS2);
					
			flag++;
			continue;
		}
		
		else if(flag==5){
			sprintf(SS2,"Content-Length: 0\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}
		
		else if(flag==6){
			sprintf(SS2,"Contact: <sip:boting-000001@192.168.7.109:5060;comp=sigcomp;encode-parm=QbkRBthOEgsTXgkTBA0HHiUrKz1CQEFERUZBNhkLGh1sMTcgK2ghOyAnOCs.ITogYX9jZGV4OzQ6PQAcRVtZQlIWWEBaSU8BDAlLEhBSXFpdXA__;transport=udp>;+sip.instance=\"<urn:gsma:imei:35611207-004302-0>\";+g.3gpp.icsi-ref=\"urn%%3Aurn-7%%3A3gpp-service.ims.icsi.mmtel\";+g.3gpp.mid-call;+g.3gpp.srvcc-alerting\r\n");
			strcat(S200, SS2);
									
			flag++;
			continue;
		}
		
		else if(flag==8){
			sprintf(SS2,"Supported: timer\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}
		
		else if(flag==9){
			sprintf(SS2,"Require: 100rel\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}
		
		else{
			memcpy(q,&S11[Get_Position(S11,header_name[flag])],Get_NextLine(&S11[Get_Position(S11,header_name[flag])]) );
			strcat(S200, q);
			printf("+++++++++++++++\n");
			printf("Start:%d Length:%d\n",Get_Position(S11,header_name[flag]),Get_NextLine(&S11[Get_Position(S11,header_name[flag])]));
			printf("%s",q);
			flag++;
		}
				
	}
	strcat(S200, "\r\n");
	sprintf(S44,S200);
}
//********************************************************************************




void res_sub_200_ok(char* S44,char* S11){
	char Sub200[4000],SS2[2000];
	int flag=0;
	char *header_name[] = {"Via","From","To:","Call-ID","Expires"
							,"Contact","Record-Route","CSeq","Content-Length"};
							
	sprintf(Sub200,"SIP/2.0 200 OK\r\n");
	
	while(flag<9){
		char q[800]={0};
		if(flag==6){
			sprintf(SS2,"Record-Route: <sip:192.168.7.115:5060;lr>\r\n");
			strcat(Sub200, SS2);
			flag++;
			continue;
		}
		//目前無用
		else if(flag==100){
		
			continue;
		}
		
		else{
			memcpy(q,&S11[Get_Position(S11,header_name[flag])],Get_NextLine(&S11[Get_Position(S11,header_name[flag])]) );
			strcat(Sub200, q);
			printf("+++++++++++++++\n");
			printf("Start:%d Length:%d\n",Get_Position(S11,header_name[flag]),Get_NextLine(&S11[Get_Position(S11,header_name[flag])]));
			printf("%s",q);
			flag++;
			
		}
	}
	strcat(Sub200, "\r\n");
	sprintf(S44,Sub200);
	
}

void res_notify(char* S44,char* S11){
	char notify[4000],SS2[2000];
	int flag=0;
	char *header_name[] = {"Via","From","To:","Subscription-State","EVENT","Expires","Call-ID"
							,"Contact","Max-Forwards","CSeq","Content-Type","Content-Length"};
							
	sprintf(notify,"NOTIFY sip:+886987654321@test.3gpp.com SIP/2.0\r\n");
	
	while(flag<12){
		char q[800]={0};
		if(flag==3){
			sprintf(SS2,"Subscription-State: active\r\n");
			strcat(notify, SS2);
			flag++;
			continue;
		}
		
		else if(flag==9){
			sprintf(SS2,"CSeq: 1 NOTIFY\r\n");
			strcat(notify, SS2);
			flag++;
			continue;
		}
		
		else if(flag==10){
			sprintf(SS2,"Content-Type: application/reginfo+xml\r\n");
			strcat(notify, SS2);
			flag++;
			continue;
		}
		
		else if(flag==11){
			sprintf(SS2,"Content-Length: 417\r\n");
			strcat(notify, SS2);
			flag++;
			continue;
		}
		
		else{
			memcpy(q,&S11[Get_Position(S11,header_name[flag])],Get_NextLine(&S11[Get_Position(S11,header_name[flag])]) );
			strcat(notify, q);
			printf("+++++++++++++++\n");
			printf("Start:%d Length:%d\n",Get_Position(S11,header_name[flag]),Get_NextLine(&S11[Get_Position(S11,header_name[flag])]));
			printf("%s",q);
			flag++;
			
		}
	}
	
	strcat(notify, "\r\n");
	
	sprintf(SS2,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
	strcat(notify, SS2);
	sprintf(SS2,"<reginfo xmlns=\"urn:ietf:params:xml:ns:reginfo\" version=\"0\" state=\"full\">\r\n");
	strcat(notify, SS2);
	sprintf(SS2,"<registration aor=\"sip:+886987654321@test.3gpp.com\" id=\"sip:+886987654321@test.3gpp.com\" state=\"active\">\r\n");
	strcat(notify, SS2);
	sprintf(SS2,"<contact id=\"sip:001010123456061@192.168.200.10:5060\" state=\"active\" event=\"registered\" expires=\"600000\">\r\n");
	strcat(notify, SS2);
	sprintf(SS2,"<uri>sip:+886987654321@192.168.200.10:5060</uri>\r\n");
	strcat(notify, SS2);
	sprintf(SS2,"</contact>\r\n");
	strcat(notify, SS2);
	sprintf(SS2,"</registration>\r\n");
	strcat(notify, SS2);
	sprintf(SS2,"</reginfo>");
	strcat(notify, SS2);
	
	
	//strcat(notify, "\r\n");
	sprintf(S44,notify);
}


