// UDP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "AKAv1-MD5/Authentication.cpp"
// Load TCPIP Library
#pragma comment(lib,"ws2_32.lib")
#include <winsock2.h>
#include <stdlib.h>
#include "Sip_Response/Sip_Response.cpp"

int caller = 0;

int main(int argc, char* argv[])
{
	// Variable Definition
	WSADATA     wsa;
	SOCKET      Sock;
	sockaddr_in Addr;
	int         i,Len=sizeof(sockaddr);
	char        S1[101000],S2[8000],S3[8000],S5[1000];
	int j=0;

    system("ipconfig");
    // 1. Start UDP Server
    WSAStartup(0x202,&wsa);
    Sock=socket(AF_INET,SOCK_DGRAM,0);
	
	Addr.sin_family=AF_INET;
	Addr.sin_port=htons(5060);
	Addr.sin_addr.s_addr=htonl(INADDR_ANY);
    bind(Sock,(sockaddr *)&Addr,Len);

	// 2. recv UDP Packet and print out
	printf("Waiting for UDP Packet Come-In\n");

	//get_res_autn_k();
	while (1) 
	{  i=recvfrom(Sock,S1,sizeof(S1),0,(sockaddr *)&Addr,&Len);
	   if (i>0) 
	   {  S1[i]=0;
	      printf("Recv(%s) Length:%d => %s\n",inet_ntoa(Addr.sin_addr),i,S1);


		  int new_cmd = Get_request(S1,caller);
		  char S4[8000],ch;
		  int con_flag=0;
		  int send_len=0;
		  char szTest[1000] = {0};

		  printf("New_Cmd:%d\n",new_cmd);

		  if(new_cmd==1)
			res_auth_401(S4,S1);
			//res_test_200_ok(S4,S1);

		  else if(new_cmd==2)
			res_200_ok(S4,S1);

		  else if(new_cmd==3){
			res_sub_200_ok(S4,S1);
			con_flag++;
		  }

		  //代刚ノINVITE
		  else if(new_cmd==4){
			  
			if(caller)
			{
				FILE *fp = fopen("Test_invite2.txt", "r");
				memset(S4, 0, sizeof(S4));
				while(!feof(fp))
				{
					memset(szTest, 0, sizeof(szTest));
					fgets(szTest, sizeof(szTest) - 1, fp); // 传︽才
					printf("%s", szTest);

					strcat(S4,szTest);
				}
				fclose(fp);
			}

			else
				continue;
		  }

		  //代刚ノClient
		  else if(new_cmd==12){
			res_test_200_ok(S4,S1);
		  }


		  //代刚ノ100 trying
		  else if(new_cmd==5){
			  memset(S4, 0, sizeof(S4));
			  res_test_100_trying(S4,S1);
			  con_flag=2;

		  }

		  else if(new_cmd==7){
			res_test_200_prack(S4,S1);
		  }
		  
		  else if(new_cmd==6){
			printf("Can not handle the message\n");
			continue;
		  }
		  
		  else if(new_cmd==9){
			  res_test_update(S4,S1);		  
		  }

		  else if(new_cmd==8){
			  FILE *fp = fopen("Test_183.txt", "a+");
			  fprintf(fp,"%s",S1);
			  fclose(fp);
			  continue;
		  }

		  //代刚ノRTP
		   else if(new_cmd==-6){
			  con_flag = -6;
			  printf("##################");
			  
		  }
		  else
		  {
			  printf("\nCan not find the response\n");
			  continue;
		  }
	  

		  if(con_flag>=0)
		  {
			  //Addr.sin_addr.s_addr = inet_addr("192.168.7.114");
			  Addr.sin_port=htons(5060);
			  //sprintf(S4,"SIP/2.0 200 OK\r\n");
			  //sprintf(S4,"1234");
			  send_len = sendto(Sock,S4,strlen(S4),0,(sockaddr *)&Addr,sizeof(sockaddr));
			  printf("Sendto(%s) Port:%d Len:%d=> %s\n",inet_ntoa(Addr.sin_addr),ntohs(Addr.sin_port),send_len,S4);
		  }

		  if(con_flag==1){
				char S6[8000];
				res_notify(S6,S1);
				sendto(Sock,S6,strlen(S6),0,(sockaddr *)&Addr,sizeof(sockaddr));
				printf("Sendto(%s) Port:%d=> %s\n",inet_ntoa(Addr.sin_addr),ntohs(Addr.sin_port),S6);
				con_flag=0;
		  }

		  if(con_flag==2){
			  char S6[8000];
			  char szTest2[1000] = {0};

			  res_test_183(S6,S1);	  
			  //res_test_180_ring(S6,S1);
			  sendto(Sock,S6,strlen(S6),0,(sockaddr *)&Addr,sizeof(sockaddr));
		      printf("Sendto(%s) Port:%d=> %s\n",inet_ntoa(Addr.sin_addr),ntohs(Addr.sin_port),S6);

		      con_flag=0;
		  }

		  if(con_flag==-6){
		
			unsigned char buf[]={0x80,0x68,0x00,0x00,0x00,0x00,0x01,0x40,0xc1,0x07,0x21,0x8e,0xf4,0x44,0x41,0x8c,0x0c,0xef,0xb3,0xac,0xe8,0x34,0xc0,0x00,0x3a,0xd4,0x21,0xed,0x3f,0xf5,0x90,0x90,0x06,0x02,0x06,0xb9,0x40,0x8a,0xa5,0xa4,0x4a,0x52,0x12,0x72,0xd4,0x88,0xa2,0x41,0x9e,0x30,0x0a,0x00,0x2c,0x46,0x22,0xe4,0xc9,0x31,0x96,0x1d,0x2b,0x06,0x43,0x75,0xec,0x16,0xc2,0x22,0x32,0xee,0xab,0xfc,0x96};
			Addr.sin_port=htons(40000);
			
			sendto(Sock,(char *)buf,73,0,(sockaddr *)&Addr,sizeof(sockaddr));
			printf("Sendto(%s) Port:%d=> \n",inet_ntoa(Addr.sin_addr),ntohs(Addr.sin_port));
			
			//con_flag=0;
		  }

	   }
	}

	return 0;
}

