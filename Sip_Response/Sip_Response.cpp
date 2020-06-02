
#include "Sip_Response.h"

int g_PhoneNum = 987654321;

int Get_Position(char *S1, const char *Head)
{
	int i, j;
	for (i = 0; i < (int)(strlen(S1) - strlen(Head)); i++)
	{
		j = 0;
		if ((S1[i] | 0x20) == (Head[0] | 0x20))
		{
			for (j = 1; j < (int)strlen(Head); j++)
				if ((S1[i + j] | 0x20) != (Head[j] | 0x20))
					break;
		}
		if (j >= (int)strlen(Head))
			break;
	}

	if (i < (int)(strlen(S1) - strlen(Head)))
		{ return i; }
	else
		{ return -1; }
}

int Get_NextLine(char *S1)
{
	int i;
	for (i = 0; i < (int)strlen(S1); i++)
		if ((S1[i] == 0x0a) || (S1[i] == 0x0d))
			break;
	if (i < (int)strlen(S1))
	{
		if ((S1[i] == 0x0d) && (S1[i + 1] == 0x0a))
			{ return i + 2; }
		else
			{ return -1; }
	}
	else
		return -1;
}

SIP_Command_E Get_request(char *S11, int caller, int register_flag)
{
	TraceLine();
	const char *header_name[] = {"CSeq", "Expires"};
	char q[100] = {0};
	int len = Get_NextLine(&S11[Get_Position(S11, header_name[0])]) - 2;
	memcpy(q, &S11[Get_Position(S11, header_name[0])], len);
	printf("[%s] Len:%d %s", caller? "caller":"callee", len, q);

	char qq[100] = {0};
	int i_len = Get_NextLine(&S11[0]) - 2;
	memcpy(qq, S11, i_len);

	if (!strncmp(q, REGISTER1, len) && !strncmp(qq, REGISTER, strlen(REGISTER)))
	{
		TraceLine();
		printf("\nENTER 200_OK\n");
		RETURN SIP_REGISTER_UnAuth;
	}

	else if (!strncmp(q, REGISTER2, len) && !strncmp(qq, REGISTER, strlen(REGISTER)))
	{
		TraceLine();
		printf("\nENTER 200_OK\n");
		RETURN SIP_REGISTER_OK;
	}

	else if (!strncmp(q, REGISTER3, len) && !strncmp(qq, REGISTER, strlen(REGISTER)))
	{
		TraceLine();
		printf("\nENTER 200_OK\n");
		RETURN SIP_REGISTER_OK;
	}

	else if (!strncmp(q, SUBSCRIBE1, len) && !strncmp(qq, SUBSCRIBE, strlen(SUBSCRIBE)))
	{
		TraceLine();
		printf("\nENTER SUBSCRIBE_200_OK\n");
		RETURN SIP_SUBSCRIBE;
	}
	else if (!strncmp(q, SUBSCRIBE2, len) && !strncmp(qq, SUBSCRIBE, strlen(SUBSCRIBE)))
	{
		TraceLine();
		printf("\nENTER SUBSCRIBE_200_OK\n");
		RETURN SIP_SUBSCRIBE;
	}

	else if (strstr(q, NOTIFY) && !strncmp(qq, NOTIFY, strlen(NOTIFY)))
	{
		TraceLine();
		printf("\nENTER TESY INVITE\n");
		RETURN SIP_NOTIFY;
	}

	else if (strstr(q, MESSAGE) && !strncmp(qq, MESSAGE, strlen(MESSAGE)))
	{
		TraceLine();
		RETURN SIP_MESSAGE;
	}

	else if (strstr(q, CANCEL) && !strncmp(qq, CANCEL, strlen(CANCEL)))
	{
		TraceLine();
		RETURN SIP_CANCEL;
	}

	else if (strstr(q, REGISTER1) && !strncmp(qq, "SIP/2.0 401 Unauthorized", i_len))
	{
		TraceLine();
		RETURN SIP_401_REGISTER; 
	}

	else if (strstr(q, REGISTER2) && !strncmp(qq, "SIP/2.0 200 OK", i_len))
	{
		TraceLine();
		RETURN SIP_200_REGISTER; 
	}

	//--- caller
	else if (caller)
	{
		TraceLine();

		if (strstr(q, INVITE))
		{
			if (!strncmp(qq, "SIP/2.0 100 Trying", i_len))
			{
				TraceLine();
				printf("Waitng for next status\n");
				RETURN SIP_OTHER;
			}

			else if (!strncmp(qq, "SIP/2.0 183 Session Progress", i_len))
			{
				TraceLine();
				printf("\nReturn SIP PRACK\n");
				RETURN SIP_183_INVITE;
			}

			else if (!strncmp(qq, "SIP/2.0 200 OK", i_len))
			{
				TraceLine();
				RETURN SIP_200_INVITE;
			}

			else if (!strncmp(qq, "SIP/2.0 480 Temporarily not available", i_len))
			{
				TraceLine();
				RETURN SIP_480_INVITE;
			}

			else
				printf("Not Find Response \n");
			RETURN SIP_OTHER;
		}

		else if (strstr(q, PRACK) && !strncmp(qq, "SIP/2.0 200 OK", i_len))
		{
			TraceLine();
			printf("\nReturn SIP PRACK\n");
			RETURN SIP_200_PRACK;
		}

		else if (strstr(q, UPDATE) && !strncmp(qq, "SIP/2.0 200 OK", i_len))
		{
			TraceLine();
			printf("\nReturn SIP UPDATE\n");
			RETURN SIP_200_UPDATE;
		}

		else if (strstr(q, BYE) && !strncmp(qq, "SIP/2.0 200 OK", i_len))
		{
			TraceLine();
			printf("\nReturn SIP PRACK\n");
			RETURN SIP_200_BYE;
		}

		else if (strstr(q, BYE) && strstr(qq, BYE))
		{
			TraceLine();
			printf("\nReturn SIP PRACK\n");
			RETURN SIP_BYE;
		}

		else
		{
			printf("\nCan not find the Response, %d @ %s\n", __LINE__, __FILE__);
		}
		
	}
	// callee
	else
	{	
		TraceLine();

		//回復100 Trying 183
		if (!strncmp(qq, "INVITE", strlen("INVITE")))
		{
			TraceLine();
			RETURN SIP_INVITE; 
		}

		else if (!strncmp(qq, "PRACK", strlen("PRACK")))
		{
			printf("\nENTER update\n");
			RETURN SIP_PRACK;
		}

		else if (!strncmp(qq, "UPDATE", strlen("UPDATE")))
		{
			printf("\nENTER RTP\n");
			RETURN SIP_OTHER;
		}

		else if (strstr(q, BYE) && strstr(qq, BYE))
		{
			TraceLine();
			printf("\nReturn SIP PRACK\n");
			RETURN SIP_BYE;
		}

		else
		{
			printf("\nCan not find the Response, %d @ %s\n", __LINE__, __FILE__);
		}
	}
	TraceLine();
}

void res_auth_401(char *S44, char *S11)
{
	TraceLine();
	char S401[4000], SS[2000], SS2[2000];

	int flag = 0;
	const char *header_name[] = {"Via:","From:","To:","Call-ID:","Max-Forwards:","Expires:"
							,"User-Agent:","WWW-Authenticate:"
							,"Security-Server:","CSeq:","Require:","Content-Length:"};

	sprintf(S401, "SIP/2.0 401 Unauthorized\r\n");

	while(flag<11){
		char q[500]={0};
		if(flag==7){
			sprintf(SS2,"WWW-Authenticate: Digest realm=\"ims.mnc001.mcc001.3gppnetwork.org\",nonce=\"AAAAAAAAAA5LmYAAvfNHPvh1+8w=%d\",algorithm=AKAv1-MD5,qop=\"auth\"\r\n", time(NULL));
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
	strcpy(S44, S401);
}

void res_200_for_register(char *S44, char *S11)
{
	char S200[4000], SS2[2000];
	int flag = 0;
	const char *header_name[] = {"Via:", "From:", "To:", "Call-ID:", "User-Agent:", "P-Associated-URI:", "P-Associated-URI:", "Contact:", "CSeq:", "Content-Length:"};

	sprintf(S200, "SIP/2.0 200 OK\r\n");

	while (flag < 10)
	{
		char q[800] = {0};
	
		if (flag == 5)
		{
			sprintf(SS2, "P-Associated-URI: <sip:0%d@test.3gpp.com>\r\n", g_PhoneNum++);
			strcat(S200, SS2);
			flag++;
			continue;
		}

		else if (flag == 6)
		{
			flag++;
			continue;
		}

		else if (flag == 7)
		{
			sprintf(SS2, "Contact: <sip:%s:%d>\r\n", g_Local_IP, g_Local_Port);
			strcat(S200, SS2);
			flag++;
			continue;
		}

		else if (flag == 9)
		{
			sprintf(SS2, "Content-Length: 0\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}

		else
		{
			memcpy(q, &S11[Get_Position(S11, header_name[flag])], Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
			strcat(S200, q);
			printf("+++++++++++++++\n");
			printf("Start:%d Length:%d\n", Get_Position(S11, header_name[flag]), Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
			printf("%s", q);
			flag++;
		}
	}
	strcat(S200, "\r\n");
	strcpy(S44, S200);
}

void res_200_ok(char *S44, char *S11)
{
	char S200[4000], SS2[2000];
	int flag = 0;
	const char *header_name[] = {"Via", "From", "To", "Call-ID", "Contact", "CSeq", "Content-Length"};

	sprintf(S200, "SIP/2.0 200 OK\r\n");

	while (flag < 7)
	{
		char q[800] = {0};
		if (flag == 4)
		{
			TraceLine();
			sprintf(SS2, "Contact: <sip:%s:%d>\r\n", g_Local_IP, g_Local_Port);
			strcat(S200, SS2);
			flag++;
			continue;
		}

		else if (flag == 7)
		{
			TraceLine();
			sprintf(SS2, "Content-Length: 0\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}

		else
		{
			TraceLine();
			memcpy(q, &S11[Get_Position(S11, header_name[flag])], Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
			strcat(S200, q);
			printf("+++++++++++++++\n");
			printf("Start:%d Length:%d\n", Get_Position(S11, header_name[flag]), Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
			printf("%s", q);
			flag++;
		}
	}
	strcat(S200, "\r\n");
	strcpy(S44, S200);
}

void res_202_Accepted(char *S44, char *S11)
{
	char S200[4000], SS2[2000];
	int flag = 0;
	const char *header_name[] = {"Via", "From", "To", "Call-ID", "User-Agent", "P-Associated-URI", "P-Associated-URI", "Contact", "CSeq", "Content-Length"};

	sprintf(S200, "SIP/2.0 202 Accepted\r\n");
}

// 測試用Client******************************************************
void res_test_200_ok(char *S44, char *S11)
{
	char S200[4000], SS2[2000];
	int flag = 0;
	const char *header_name[] = {"Via", "From", "To", "Call-ID", "CSeq", "Expires", "Max-Forwards", "Content-Length", "Contact"};

	sprintf(S200, "SIP/2.0 200 OK\r\n");

	while (flag < 9)
	{
		char q[800] = {0};
		if (flag == 4)
		{
			sprintf(SS2, "CSeq: 2 REGISTER\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}

		else if (flag == 5)
		{
			sprintf(SS2, "Expires: 125\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}

		else if (flag == 8)
		{
			sprintf(SS2, "Contact: <SIP:123@192.168.7.115>\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}

		else
		{
			memcpy(q, &S11[Get_Position(S11, header_name[flag])], Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
			strcat(S200, q);
			printf("+++++++++++++++\n");
			printf("Start:%d Length:%d\n", Get_Position(S11, header_name[flag]), Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
			printf("%s", q);
			flag++;
		}
	}
	strcat(S200, "\r\n");
	strcpy(S44, S200);
}
//********************************************************************************

//測試用100 Trying****************************************************************
void res_test_100_trying(char *S44, char *S11, sockaddr_in *lpAddr)
{
	char S200[4000], SS2[2000];
	int flag = 0;
	const char *header_name[] = {"Via", "To:", "From", "Call-ID", "CSeq", "Content-Length"};

	sprintf(S200, "SIP/2.0 100 Trying\r\n");

	while (flag < 6)
	{
		char q[800] = {0};

		if (flag == 5)
		{
			sprintf(SS2, "Content-Length: 0\r\n");
			strcat(S200, SS2);
			flag++;
			break;
		}

		if (flag == 0)
		{

			sprintf(SS2, "Via: SIP/2.0/UDP %s;rport=%d;", g_Target_Virtual_IP, ntohs(lpAddr->sin_port));
			memcpy(q, &S11[Get_Position(S11, "branch=")], Get_NextLine(&S11[Get_Position(S11, "branch=")]));
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

		memcpy(q, &S11[Get_Position(S11, header_name[flag])], Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
		strcat(S200, q);
		printf("+++++++++++++++\n");
		printf("Start:%d Length:%d\n", Get_Position(S11, header_name[flag]), Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
		printf("%s", q);
		flag++;
	}
	strcat(S200, "\r\n");
	strcpy(S44, S200);
}

void res_test_180_ringing(char *S44, char *S11, sockaddr_in *lpAddr)
{
	char S200[4000], SS2[2000];
	int flag = 0;
	const char *header_name[] = {"Via", "To:", "From", "Call-ID", "CSeq", "Content-Length"};

	sprintf(S200, "SIP/2.0 180 Ringing\r\n");

	while (flag < 6)
	{
		char q[800] = {0};

		if (flag == 5)
		{
			sprintf(SS2, "Content-Length: 0\r\n");
			strcat(S200, SS2);
			flag++;
			break;
		}

		if (flag == 0)
		{

			sprintf(SS2, "Via: SIP/2.0/UDP %s;rport=%d;", g_Target_Virtual_IP, ntohs(lpAddr->sin_port));
			memcpy(q, &S11[Get_Position(S11, "branch=")], Get_NextLine(&S11[Get_Position(S11, "branch=")]));
			strcat(SS2, q);
			strcat(S200, SS2);
			flag++;
			continue;
		}

		memcpy(q, &S11[Get_Position(S11, header_name[flag])], Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
		strcat(S200, q);
		printf("+++++++++++++++\n");
		printf("Start:%d Length:%d\n", Get_Position(S11, header_name[flag]), Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
		printf("%s", q);
		flag++;
	}
	strcat(S200, "\r\n");
	strcpy(S44, S200);
}
//*********************************************************************************

//測試用183 Session Progress*******************************************************
void res_183_for_invite(char *S44, char *S11)
{
	TraceLine();
	char S200[4000], SS2[4000];
	int flag = 0;
	const char *header_name[] = {"Via:", "To:", "From:", "Call-ID:", "CSeq", "P-Access-Network-Info:", "P-Charging-Vector:", "P-Early-Media:", "P-Asserted-Service:", "P-Preferred-Service:"};

	sprintf(S200, "SIP/2.0 183 Session Progress\r\n");

	while (flag < 9)
	{
		char q[2000] = {0};

		if (flag == 0)
		{
			TraceLine();
			sprintf(SS2, "Via: SIP/2.0/UDP 192.168.200.10;rport=5060;received=192.168.200.10;");
			memcpy(q, &S11[Get_Position(S11, "branch=")], Get_NextLine(&S11[Get_Position(S11, "branch=")]));
			strcat(SS2, q);
			strcat(S200, SS2);
			flag++;
			continue;
		}

		if (flag == 1)
		{
			TraceLine();
			sprintf(SS2, "To: <sip:%s;phone-context=001.01.eps.ims.mnc001.mcc001.3gppnetwork.org>;tag=iwBMWaf7t00MBG\r\n", g_UAC_Phone);
			strcat(S200, SS2);
			flag++;
			continue;
		}

		if (Get_Position(S11, header_name[flag])<0)	
		{ 
			TraceLine();
			flag++; 
			continue; 
		}
		TraceLine();
		memcpy(q, &S11[Get_Position(S11, header_name[flag])], Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
		strcat(S200, q);
		printf("+++++++++++++++\n");
		printf("Start:%d Length:%d\n", Get_Position(S11, header_name[flag]), Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
		printf("%s", q);
		flag++;
	}
	if (S200[strlen(S200)-1]==0x0d)	TraceLine();

	TraceLine();
	char szBody[3000];
	sprintf(szBody,
		"v=0\r\n"
		"o=- 0 0 IN IP4 %s\r\n"
		"s=-\r\n"
		"c=IN IP4 %s\r\n"
		"b=AS:41\r\n"
		"b=RS:512\r\n"
		"b=RR:1537\r\n"
		"t=0 0\r\n"
		"m=audio %d RTP/AVP 98 105\r\n"
		"b=AS:41\r\n"
		"b=RS:512\r\n"
		"b=RR:1537\r\n"
		"a=rtpmap:98 AMR-WB/16000/1\r\n"
		"a=ptime:20\r\n"
		"a=maxptime:240\r\n"
		"a=rtpmap:105 telephone-event/16000/1\r\n"
		"a=fmtp:98 mode-change-capability=2;max-red=0\r\n"
		"a=fmtp:105 0-15\r\n"
		"a=curr:qos local none\r\n"
		"a=curr:qos remote none\r\n"
		"a=des:qos mandatory local sendrecv\r\n"
		"a=des:qos mandatory remote sendrecv\r\n"
		"a=conf:qos remote sendrecv\r\n"
		"a=sendrecv\r\n",
		g_Meida_IP,
		g_Meida_IP,
		g_Meida_Port
	);
	int body_len = strlen(szBody);
	sprintf(SS2, 
		"Contact: <sip:%s:%d>;+g.3gpp.icsi-ref=\"urn%%3Aurn-7%%3A3gpp-service.ims.icsi.mmtel\r\n"
		"Allow: INVITE, ACK, CANCEL, BYE, UPDATE, REFER, NOTIFY, MESSAGE, PRACK, OPTIONS\r\n"
		// "Record-Route: <sip:%s:%d;lr>\r\n"
		"Require: 100rel\r\n"
		"RSeq: 1234\r\n"
		"Content-Type: application/sdp\r\n"
		"P-Preferred-Identity: <sip::%s\r\n"
		// "P-Access-Network-Info: 3GPP-E-UTRAN-FDD;utran-cell-id-3gpp=001010001000000a\r\n"
		"Content-Length: %d\r\n"
		"\r\n"
		"%s",
		g_Local_IP, g_Local_Port,	// Contact
		// g_Local_IP, g_Local_Port,	// Record-Route
		g_UAC_Phone,				// P-Preferred-Identity
		body_len,					// Content-Length
		szBody						// Body
	);
	strcat(S200, SS2);

	strcpy(S44, S200);
	RETURN;
}

void res_200_for_invite(char *S44, char *S11, sockaddr_in *lpAddr)
{
	TraceLine();
	char S200[4000], SS2[4000];
	int flag = 0;
	const char *header_name[] = {"Via:", "To:", "From:", "Call-ID:", "CSeq", "P-Access-Network-Info:", "P-Charging-Vector:", "P-Early-Media:", "P-Asserted-Service:", "P-Preferred-Service:"};

	sprintf(S200, "SIP/2.0 200 OK\r\n");

	while (flag < 9)
	{
		char q[2000] = {0};

		if (flag == 0)
		{
			TraceLine();
			sprintf(SS2, "Via: SIP/2.0/UDP %s;rport=5060;received=%s;", g_Target_Virtual_IP, g_Target_Virtual_IP);
			memcpy(q, &S11[Get_Position(S11, "branch=")], Get_NextLine(&S11[Get_Position(S11, "branch=")]));
			strcat(SS2, q);
			strcat(S200, SS2);
			flag++;
			continue;
		}

		if (flag == 1)
		{
			TraceLine();
			sprintf(SS2, "To: <sip:%s;phone-context=001.01.eps.ims.mnc001.mcc001.3gppnetwork.org>;tag=iwBMWaf7t00MBG\r\n", g_UAC_Phone);
			strcat(S200, SS2);
			flag++;
			continue;
		}

		if (Get_Position(S11, header_name[flag])<0)	
		{ 
			TraceLine();
			flag++; 
			continue; 
		}
		TraceLine();
		memcpy(q, &S11[Get_Position(S11, header_name[flag])], Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
		strcat(S200, q);
		printf("+++++++++++++++\n");
		printf("Start:%d Length:%d\n", Get_Position(S11, header_name[flag]), Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
		printf("%s", q);
		flag++;
	}
	if (S200[strlen(S200)-1]==0x0d)	TraceLine();

	TraceLine();
	char szBody[3000];
	sprintf(szBody,
		"v=0\r\n"
		"o=- 0 0 IN IP4 %s\r\n"
		"s=-\r\n"
		"c=IN IP4 %s\r\n"
		"b=AS:41\r\n"
		"b=RS:512\r\n"
		"b=RR:1537\r\n"
		"t=0 0\r\n"
		"m=audio %d RTP/AVP 98 105\r\n"
		"b=AS:41\r\n"
		"b=RS:512\r\n"
		"b=RR:1537\r\n"
		"a=rtpmap:98 AMR-WB/16000/1\r\n"
		"a=ptime:20\r\n"
		"a=maxptime:240\r\n"
		"a=rtpmap:105 telephone-event/16000/1\r\n"
		"a=fmtp:98 mode-change-capability=2;max-red=0\r\n"
		"a=fmtp:105 0-15\r\n"
		"a=curr:qos local none\r\n"
		"a=curr:qos remote none\r\n"
		"a=des:qos mandatory local sendrecv\r\n"
		"a=des:qos mandatory remote sendrecv\r\n"
		"a=conf:qos remote sendrecv\r\n"
		"a=sendrecv\r\n",
		g_Meida_IP,
		g_Meida_IP,
		g_Meida_Port
	);
	int body_len = strlen(szBody);
	sprintf(SS2, 
		"Contact: <sip:%s:%d>;+g.3gpp.icsi-ref=\"urn%%3Aurn-7%%3A3gpp-service.ims.icsi.mmtel\r\n"
		"Allow: INVITE, ACK, CANCEL, BYE, UPDATE, REFER, NOTIFY, MESSAGE, PRACK, OPTIONS\r\n"
		// "Record-Route: <sip:%s:%d;lr>\r\n"
		"Require: 100rel\r\n"
		"RSeq: 1234\r\n"
		"Content-Type: application/sdp\r\n"
		"P-Preferred-Identity: <sip:%s>\r\n"
		// "P-Access-Network-Info: 3GPP-E-UTRAN-FDD;utran-cell-id-3gpp=001010001000000a\r\n"
		"Content-Length: %d\r\n"
		"\r\n"
		"%s",
		g_Local_IP, g_Local_Port,	// Contact
		// g_Local_IP, g_Local_Port,	// Record-Route
		g_UAC_Phone,				// P-Preferred-Identity
		body_len,					// Content-Length
		szBody						// Body
	);
	strcat(S200, SS2);

	strcpy(S44, S200);
	RETURN;
}



//********************************************************************************
char g_Via_Brnach[128] = "";
char g_From_Tag[128] = "";
char g_Call_ID[128] = "";


void make_invite(char *S44)
{
	TraceLine();
	char szBody[2000];
	sprintf(szBody,
		"v=0\r\n"
		"o=- 0 0 IN IP4 %s\r\n"
		"s=-\r\n"
		"c=IN IP4 %s\r\n"
		"b=AS:41\r\n"
		"b=RS:512\r\n"
		"b=RR:1537\r\n"
		"t=0 0\r\n"
		"m=audio %d RTP/AVP 98 97 100 105\r\n"
		"b=AS:41\r\n"
		"b=RS:512\r\n"
		"b=RR:1537\r\n"
		"a=rtpmap:98 AMR-WB/16000/1\r\n"
		"a=ptime:20\r\n"
		"a=maxptime:240\r\n"
		"a=rtpmap:97 AMR/8000/1\r\n"
		"a=rtpmap:100 telephone-event/8000/1\r\n"
		"a=rtpmap:105 telephone-event/16000/1\r\n"
		"a=fmtp:98 mode-change-capability=2;max-red=0\r\n"
		"a=fmtp:97 mode-change-capability=2;max-red=0\r\n"
		"a=fmtp:100 0-15\r\n"
		"a=fmtp:105 0-15\r\n"
		"a=curr:qos local none\r\n"
		"a=curr:qos remote none\r\n"
		"a=des:qos mandatory local sendrecv\r\n"
		"a=des:qos optional remote sendrecv\r\n"
		"a=sendrecv\r\n",
		g_Meida_IP,
		g_Meida_IP,
		g_Meida_Port
	);

	TraceLine();
	sprintf(g_Via_Brnach, "z9hG4bKk3qJna8%ld", time(NULL));
	sprintf(g_From_Tag, "yP6dx0STGfLecnI%ld", time(NULL));
	sprintf(g_Call_ID, "AbgA2IFeUNAoABNGNBA%ld", time(NULL));
	int body_len = strlen(szBody);

	sprintf(S44, 
		"INVITE sip:%s;phone-context=001.01.eps.ims.mnc001.mcc001.3gppnetwork.org SIP/2.0\r\n"
		"Via: SIP/2.0/UDP %s;rport;branch=%s\r\n"
		"From: <sip:%s@test.3gpp.com>;tag=%s\r\n"
		"To: <sip:%s;phone-context=001.01.eps.ims.mnc001.mcc001.3gppnetwork.org>\r\n"
		"Contact: <sip:%s@%s:%d>;+g.3gpp.icsi-ref=\"urn%%3Aurn-7%%3A3gpp-service.ims.icsi.mmtel\r\n"
		"Allow: INVITE, ACK, CANCEL, BYE, UPDATE, REFER, NOTIFY, MESSAGE, PRACK, OPTIONS\r\n"
		"P-Preferred-Identity: <sip:%s@test.3gpp.com>\r\n"
		"Require: 100rel\r\n"
		"RSeq: 1234\r\n"
		"Content-Type: application/sdp\r\n"
		"Supported: 100rel, precondition, replaces, timer\r\n"
		"Accept: application/sdp\r\n"
		"Max-Forwards: 70\r\n"
		"User-Agent: VoLTE/WFC UA\r\n"
		"P-Preferred-Service: urn:urn-7:3gpp-service.ims.icsi.mmtel\r\n"
		"Accept-Contact: *;+g.3gpp.icsi-ref=\"urn%%3Aurn-7%%3A3gpp-service.ims.icsi.mmtel\"\r\n"
		"P-Early-Media: supported\r\n"
		"CSeq: 1 INVITE\r\n"
		"Call-ID: %s\r\n"
		"Session-Expires: 60000\r\n"
		"Min-SE: 1800\r\n"
		"Route: <sip:%s:%d;lr>\r\n"
		"P-Access-Network-Info: 3GPP-E-UTRAN-FDD;utran-cell-id-3gpp=001010001000000a\r\n"
		"Require: sec-agree\r\n"
		"Proxy-Require: sec-agree\r\n"
		"Security-Verify: ipsec-3gpp;prot=esp;mod=trans;spi-c=2815619481;spi-s=136158516;port-c=50717;port-s=50718;ealg=null;alg=hmac-md5-96;q=0.1\r\n"
		"Content-Length: %d\r\n"
		"\r\n"
		"%s",
		g_Target_Phone,							// INVITE
		g_Local_IP, g_Via_Brnach,				// Via
		g_UAC_Phone, g_From_Tag,				// From
		g_Target_Phone,							// To
		g_UAC_Phone, g_Local_IP, g_Local_Port,	// Cantact
		g_UAC_Phone, 							// P-Preferred-Identity
		g_Call_ID,								// Call-ID
		g_Local_IP,	g_Local_Port,				// Route
		body_len,								// Content-Length
		szBody									// Body
	);

	RETURN;
}

//測試用200 PRACK*****************************************************************
void make_prack(char *S44, char *S11)
{
	char S200[4000], SS2[2000], Cseq[100];
	int flag = 0;
	const char *header_name[] = {"Via", "From", "To:", "Call-ID", "CSeq", "Max-Forwards", "RAck", "Content-Length"};

	sprintf(S200, "PRACK sip:%s:%d SIP/2.0\r\n", g_Target_Real_IP, g_Target_SIP_Port);

	TraceLine();
	while (flag < 8)
	{
		
		char q[2000] = {0};

		if (flag == 4)
		{
			TraceLine();
			char *beg, *end, len;
			strcpy(SS2, "CSeq: 2 PRACK\r\n");
			memcpy(q, &S11[Get_Position(S11, header_name[flag])], Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
			// strcat(SS2, q);
			strcat(S200, SS2);
			beg = strchr(q,':')+2;
			if (end = strstr(q,"\r\n")) 
				*end='\0';	
			else	q[strlen(q)-1]='\0';
			if (end>beg)	memcpy(Cseq, beg, end-beg);
			flag++;
			continue;
		}

		if (flag == 5)
		{
			TraceLine();
			sprintf(SS2, "Max-Forwards: 70\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}

		if (flag == 6)
		{
			TraceLine();
			char* end;
			TraceLine();
			memcpy(q, &S11[Get_Position(S11, "RSeq:")], Get_NextLine(&S11[Get_Position(S11, "RSeq:")]));
			TraceLine();
			if (end=strstr(q,"\r\n"))	*end = '\0';
			TraceLine();
			sprintf(SS2, "RAck:%s %s\r\n", &q[strlen("RSeq:")], Cseq);
			TraceLine();
			strcat(S200, SS2);
			TraceLine();
			flag++;
			continue;
		}
		
		if (flag == 7)
		{
			TraceLine();
			sprintf(SS2, "Content-Length: 0\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}

		if (Get_Position(S11, header_name[flag])<0)	
		{ 
			TraceLine();
			flag++; 
			continue; 
		}

		TraceLine();
		printf("flag=%d, header_name=%s, start=%d, end=%d\n", flag, header_name[flag], Get_Position(S11, header_name[flag]), Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
		memcpy(q, &S11[Get_Position(S11, header_name[flag])], Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
		TraceLine();
		strcat(S200, q);
		printf("+++++++++++++++\n");
		printf("Start:%d Length:%d\n", Get_Position(S11, header_name[flag]), Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
		printf("%s", q);
		flag++;
	}
	strcat(S200, "\r\n");
	strcpy(S44, S200);


	RETURN;
}

void make_register_test(char *S44)
{
	// strcpy(S44, 
	// 	"REGISTER sip:ims.mnc001.mcc001.3gppnetwork.org SIP/2.0\r\n"
	// 	"Via: SIP/2.0/UDP 192.168.200.6;rport;branch=z9hG4bKSD2qqNlolSk3ht\r\n"
	// 	"From: <sip:001010123456100@ims.mnc001.mcc001.3gppnetwork.org>;tag=LRDQvT1aoW6AZu\r\n"
	// 	"To: <sip:001010123456100@ims.mnc001.mcc001.3gppnetwork.org>\r\n"
	// 	"Call-ID: 7SPLosBwnc0PAbnUsyj\r\n"
	// 	"Max-Forwards: 70\r\n"
	// 	"Expires: 600000\r\n"
	// 	"User-Agent: VoLTE/WFC UA\r\n"
	// 	"Contact: <sip:192.168.200.6:5060;transport=udp>;+sip.instance=\"<urn:gsma:imei:35611207-122680-0>\";+g.3gpp.icsi-ref=\"urn%3Aurn-7%3A3gpp-service.ims.icsi.mmtel\";+g.3gpp.smsip;audio\r\n"
	// 	"Supported: path, gruu, sec-agree\r\n"
	// 	"Allow: INVITE, ACK, CANCEL, BYE, UPDATE, REFER, NOTIFY, MESSAGE, PRACK, OPTIONS\r\n"
	// 	"Authorization: Digest username=\"001010123456100@ims.mnc001.mcc001.3gppnetwork.org\",realm=\"ims.mnc001.mcc001.3gppnetwork.org\",nonce=\"\",uri=\"sip:ims.mnc001.mcc001.3gppnetwork.org\",response=\"\"\r\n"
	// 	"Require: sec-agree\r\n"
	// 	"Proxy-Require: sec-agree\r\n"
	// 	"Security-Client: ipsec-3gpp;prot=esp;mod=trans;spi-c=1356870044;spi-s=1007850693;port-c=50000;port-s=50001;ealg=null;alg=hmac-md5-96, ipsec-3gpp;prot=esp;mod=trans;spi-c=1356870044;spi-s=1007850693;port-c=50000;port-s=50001;ea\r\n"
	// 	"CSeq: 1 REGISTER\r\n"
	// 	"P-Access-Network-Info: 3GPP-E-UTRAN-FDD;utran-cell-id-3gpp=001010001000000A\r\n"
	// 	"Content-Length: 0\r\n\r\n"
	// );

	sprintf(S44, 
    	"REGISTER sip:ims.mnc001.mcc001.3gppnetwork.org SIP/2.0\r\n"
        "Via: SIP/2.0/UDP 192.168.200.6;rport;branch=z9hG4bKlzx5bBhnRhUj7B\r\n"
        "From: <sip:001010123456100@ims.mnc001.mcc001.3gppnetwork.org>;tag=87hXmnpTnRpLPA\r\n"
        "To: <sip:001010123456100@ims.mnc001.mcc001.3gppnetwork.org>\r\n"
        "Call-ID: sScu5FiYZZKXtj1DsyA\r\n"
        "Max-Forwards: 70\r\n"
        "Expires: 600000\r\n"
        "User-Agent: VoLTE/WFC UA\r\n"
        "Contact: <sip:%s:%d;transport=udp>;+sip.instance=\"<urn:gsma:imei:35611207-122680-0>\";+g.3gpp.icsi-ref=\"urn%%3Aurn-7%%3A3gpp-service.ims.icsi.mmtel\";+g.3gpp.smsip;audio\r\n"
        "Supported: path, gruu, sec-agree\r\n"
        "Allow: INVITE, ACK, CANCEL, BYE, UPDATE, REFER, NOTIFY, MESSAGE, PRACK, OPTIONS\r\n"
        "Authorization: Digest username=\"001010123456100@ims.mnc001.mcc001.3gppnetwork.org\",realm=\"ims.mnc001.mcc001.3gppnetwork.org\",nonce=\"\",uri=\"sip:ims.mnc001.mcc001.3gppnetwork.org\",response=\"\"\r\n"
        "Require: sec-agree\r\n"
        "Proxy-Require: sec-agree\r\n"
        "Security-Client: ipsec-3gpp;prot=esp;mod=trans;spi-c=1601726504;spi-s=1924536925;port-c=50000;port-s=50001;ealg=null;alg=hmac-md5-96, ipsec-3gpp;prot=esp;mod=trans;spi-c=1601726504;spi-s=1924536925;port-c=50000;port-s=50001;ealg=null;alg=hmac-sha-1-96, ipsec-3gpp;prot=esp;mod=trans;spi-c=1601726504;spi-s=1924536925;port-c=50000;port-s=50001;ealg=des-ede3-cbc;alg=hmac-md5-96, ipsec-3gpp;prot=esp;mod=trans;spi-c=1601726504;spi-s=1924536925;port-c=50000;port-s=50001;ealg=des-ede3-cbc;alg=hmac-sha-1-96, ipsec-3gpp;prot=esp;mod=trans;spi-c=1601726504;spi-s=1924536925;port-c=50000;port-s=50001;ealg=aes-cbc;alg=hmac-md5-96, ipsec-3gpp;prot=esp;mod=trans;spi-c=1601726504;spi-s=1924536925;port-c=50000;port-s=50001;ealg=aes-cbc;alg=hmac-sha-1-96\r\n"
        "CSeq: 1 REGISTER\r\n"
        "P-Access-Network-Info: 3GPP-E-UTRAN-FDD;utran-cell-id-3gpp=001010001000000A\r\n"
        "Content-Length: 0\r\n\r\n",
		g_Local_IP, g_Local_Port
	);

}

void make_register_UnAuth(char *S44)
{
	char S200[4000], SS2[2000], Cseq[100];
	int flag = 0;
	const char *header_name[] = {
		"Via:", "From:", "To:", "Call-ID", "CSeq:", "Contact:", "Max-Forwards:", "Expires:", "User-Agent:", "Supported:", 
		"Allow:", "Authorization:", "Require", "Proxy-Require:", "Security-Client:", "P-Access-Network-Info:", "Content-Length:"
	};

	sprintf(S200, "REGISTER sip:%s:%d SIP/2.0\r\n", g_Target_Real_IP, g_Target_SIP_Port);

	TraceLine();
	while (flag < sizeof(header_name)/sizeof(header_name[0]))
	{
		// char q[2000] = {0};

		switch (flag)
		{
			case 0: 
				TraceLine();
				sprintf(SS2, "Via: SIP/2.0/UDP %s;rport;branch=z9hG4bKk3qJna8%ld\r\n", g_Local_IP, time(NULL));
				strcat(S200, SS2);
				break;
			case 1: 
				TraceLine();
				sprintf(SS2, "From: <sip:%s@ims.mnc001.mcc001.3gppnetwork.org>;tag=yP6dx0STGfLecnI%ld\r\n", g_UAC_Id, time(NULL));
				strcat(S200, SS2);
				break;
			case 2: 
				TraceLine();
				sprintf(SS2, "To: <sip:%s@ims.mnc001.mcc001.3gppnetwork.org>\r\n", g_UAC_Id);
				strcat(S200, SS2);
				break;
			case 3: 
				TraceLine();
				sprintf(SS2, "Call-ID: AbgA2IFeUNAoABNGNBA%ld\r\n", time(NULL));
				strcat(S200, SS2);
				break;
			case 4: 
				TraceLine();
				sprintf(SS2, "CSeq: 1 REGISTER\r\n");
				strcat(S200, SS2);
				break;
			case 5: 
				TraceLine();
				sprintf(SS2, "Contact: <sip:%s:%d;transport=udp>;+sip.instance=\"<urn:gsma:imei:35611207-122680-0>\";+g.3gpp.icsi-ref=\"urn%3Aurn-7%%3A3gpp-service.ims.icsi.mmtel\";+g.3gpp.smsip;audio\r\n", g_Local_IP, g_Local_Port);
				strcat(S200, SS2);
				break;
			case 6: 
				TraceLine();
				sprintf(SS2, "Max-Forwards: 70\r\n");
				strcat(S200, SS2);
				break;
			case 7: 
				TraceLine();
				sprintf(SS2, "Expires: 120\r\n");
				strcat(S200, SS2);
				break;
			case 8: 
				TraceLine();
				sprintf(SS2, "User-Agent: VoLTE/WFC UA\r\n");
				strcat(S200, SS2);
				break;
			case 9: 
				TraceLine();
				sprintf(SS2, "Supported: path, gruu, sec-agree\r\n");
				strcat(S200, SS2);
				break;
			case 10: 
				TraceLine();
				sprintf(SS2, "Allow: INVITE, ACK, CANCEL, BYE, UPDATE, REFER, NOTIFY, MESSAGE, PRACK, OPTIONS\r\n");
				strcat(S200, SS2);
				break;
			case 11: 
				TraceLine();
				sprintf(SS2, "Authorization: Digest username=\"%s@ims.mnc001.mcc001.3gppnetwork.org\",realm=\"ims.mnc001.mcc001.3gppnetwork.org\",nonce=\"\",uri=\"sip:ims.mnc001.mcc001.3gppnetwork.org\",response=\"\"\r\n", g_UAC_Id);
				strcat(S200, SS2);
				break;
			case 12: 
				TraceLine();
				sprintf(SS2, "Require: sec-agree\r\n");
				strcat(S200, SS2);
				break;
			case 13: 
				TraceLine();
				sprintf(SS2, "Proxy-Require: sec-agree\r\n");
				strcat(S200, SS2);
				break;
			case 14: 
				TraceLine();
				sprintf(SS2, "Security-Client: ipsec-3gpp;prot=esp;mod=trans;spi-c=1253250742;spi-s=583981170;port-c=50000;port-s=50001;ealg=null;alg=hmac-md5-96, ipsec-3gpp;prot=esp;mod=trans;spi-c=1253250742;spi-s=583981170;port-c=50000;port-s=50001;ealg\r\n");
				strcat(S200, SS2);
				break;
			case 15: 
				TraceLine();
				sprintf(SS2, "P-Access-Network-Info: 3GPP-E-UTRAN-FDD;utran-cell-id-3gpp=001010001000000A\r\n");
				strcat(S200, SS2);
				break;
			case 16: 
				TraceLine();
				sprintf(SS2, "Content-Length: 0\r\n");
				strcat(S200, SS2);
				break;
		}
		flag++;
	}
	strcat(S200, "\r\n");
	strcpy(S44, S200);

	RETURN;
}

void make_register_Auth(char *S44, char *S11)
{
	char S200[4000], SS2[2000], buf[100];
	int flag = 0;
	const char *header_name[] = {
		"Via:", "From:", "To:", "Call-ID", "CSeq:", "Contact:", "Max-Forwards:", "Expires:", "User-Agent:", "Supported:", 
		"Allow:", "Authorization:", "Require", "Proxy-Require:", "Security-Client:", "P-Access-Network-Info:", "Content-Length:"
	};

	sprintf(S200, "REGISTER sip:%s:%d SIP/2.0\r\n", g_Target_Real_IP, g_Target_SIP_Port);

	TraceLine();
	while (flag < sizeof(header_name)/sizeof(header_name[0]))
	{
		char q[2000] = {0};

		switch (flag)
		{
			case 0: 
				TraceLine();
				sprintf(SS2, "Via: SIP/2.0/UDP %s;rport;branch=z9hG4bKk3qJna8%ld\r\n", g_Local_IP, time(NULL));
				strcat(S200, SS2);
				break;
			case 1: 
				TraceLine();
				sprintf(SS2, "From: <sip:%s@ims.mnc001.mcc001.3gppnetwork.org>;tag=yP6dx0STGfLecnI%ld\r\n", g_UAC_Id, time(NULL));
				strcat(S200, SS2);
				break;
			case 2: 
				TraceLine();
				sprintf(SS2, "To: <sip:%s@ims.mnc001.mcc001.3gppnetwork.org>\r\n", g_UAC_Id);
				strcat(S200, SS2);
				break;
			case 3: 
				TraceLine();
				sprintf(SS2, "Call-ID: AbgA2IFeUNAoABNGNBA%ld\r\n", time(NULL));
				strcat(S200, SS2);
				break;
			case 4: 
				TraceLine();
				sprintf(SS2, "CSeq: 2 REGISTER\r\n");
				strcat(S200, SS2);
				break;
			case 5: 
				TraceLine();
				sprintf(SS2, "Contact: <sip:%s:%d;transport=udp>;+sip.instance=\"<urn:gsma:imei:35611207-122680-0>\";+g.3gpp.icsi-ref=\"urn%3Aurn-7%%3A3gpp-service.ims.icsi.mmtel\";+g.3gpp.smsip;audio\r\n", g_Local_IP, g_Local_Port);
				strcat(S200, SS2);
				break;
			case 6: 
				TraceLine();
				sprintf(SS2, "Max-Forwards: 70\r\n");
				strcat(S200, SS2);
				break;
			case 7: 
				TraceLine();
				sprintf(SS2, "Expires: 120\r\n");
				strcat(S200, SS2);
				break;
			case 8: 
				TraceLine();
				sprintf(SS2, "User-Agent: VoLTE/WFC UA\r\n");
				strcat(S200, SS2);
				break;
			case 9: 
				TraceLine();
				sprintf(SS2, "Supported: path, gruu, sec-agree\r\n");
				strcat(S200, SS2);
				break;
			case 10: 
				TraceLine();
				sprintf(SS2, "Allow: INVITE, ACK, CANCEL, BYE, UPDATE, REFER, NOTIFY, MESSAGE, PRACK, OPTIONS\r\n");
				strcat(S200, SS2);
				break;
			case 11: 
				TraceLine();
				char *beg, *end;
				// WWW-Authenticate: Digest realm="ims.mnc001.mcc001.3gppnetwork.org",nonce="AAAAAAAAAA5LmYAAvfNHPvh1+8w=1588827128",algorithm=AKAv1-MD5,qop="auth"
				memcpy(q, &S11[Get_Position(S11, "WWW-Authenticate:")], Get_NextLine(&S11[Get_Position(S11, "WWW-Authenticate:")]));
				TraceLine();
				buf[0]=0;
				if (beg = strstr(q, "nonce=\""))	
				{ 
					beg+=strlen("nonce=\""); 
					if (end = strchr(beg, '"'))
					{
						memcpy(buf, beg, end-beg);	
						buf[end-beg]=0;
					}
				}
				TraceLine();
				// Authorization: Digest username="001010123456075@ims.mnc001.mcc001.3gppnetwork.org",realm="ims.mnc001.mcc001.3gppnetwork.org",nonce="AAAAAAAAAA5LmYAAvfNHPvh1+8w=1588827128",uri="sip:ims.mnc001.mcc001.3gppnetwork.org",response="",algorithm="AKAv1-MD5",qop=auth,nc=00000001
				sprintf(SS2, "Authorization: Digest username=\"%s@ims.mnc001.mcc001.3gppnetwork.org\",realm=\"ims.mnc001.mcc001.3gppnetwork.org\",nonce=\"%s\",uri=\"sip:ims.mnc001.mcc001.3gppnetwork.org\",response=\"%s\",algorithm=\"AKAv1-MD5\",qop=auth,nc=00000001\r\n", 
					g_UAC_Id, buf, "");
				strcat(S200, SS2);
				break;
			case 12: 
				TraceLine();
				sprintf(SS2, "Require: sec-agree\r\n");
				strcat(S200, SS2);
				break;
			case 13: 
				TraceLine();
				sprintf(SS2, "Proxy-Require: sec-agree\r\n");
				strcat(S200, SS2);
				break;
			case 14: 
				TraceLine();
				sprintf(SS2, "Security-Client: ipsec-3gpp;prot=esp;mod=trans;spi-c=1253250742;spi-s=583981170;port-c=50000;port-s=50001;ealg=null;alg=hmac-md5-96, ipsec-3gpp;prot=esp;mod=trans;spi-c=1253250742;spi-s=583981170;port-c=50000;port-s=50001;ealg\r\n");
				strcat(S200, SS2);
				break;
			case 15: 
				TraceLine();
				sprintf(SS2, "P-Access-Network-Info: 3GPP-E-UTRAN-FDD;utran-cell-id-3gpp=001010001000000A\r\n");
				strcat(S200, SS2);
				break;
			case 16: 
				TraceLine();
				sprintf(SS2, "Content-Length: 0\r\n");
				strcat(S200, SS2);
				break;
			default:
				TraceLine();
				printf("flag=%d, header_name=%s, start=%d, end=%d\n", flag, header_name[flag], Get_Position(S11, header_name[flag]), Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
				memcpy(q, &S11[Get_Position(S11, header_name[flag])], Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
				strcat(S200, q);
				printf("+++++++++++++++\n");
				printf("Start:%d Length:%d\n", Get_Position(S11, header_name[flag]), Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
				printf("%s", q);
				break;
		}
		flag++;
	}
	strcat(S200, "\r\n");
	strcpy(S44, S200);

	RETURN;
}

void make_ack(char *S44, char *S11)
{
	char S200[4000], SS2[2000], Cseq[100];
	int flag = 0;
	const char *header_name[] = {"Via:", "From:", "To:", "Call-ID", "CSeq:", "Contact:", "Max-Forwards:", "Content-Length:"};

	sprintf(S200, "ACK sip:%s:%d SIP/2.0\r\n", g_Target_Real_IP, g_Target_SIP_Port);

	TraceLine();
	while (flag < 8)
	{
		
		char q[2000] = {0};

		if (flag == 4)
		{
			TraceLine();
			char *beg, *end, len;
			memcpy(q, &S11[Get_Position(S11, header_name[flag])], Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
			strcat(S200, SS2);
			beg = strchr(q,':')+2;
			if (end = strstr(q," INVITE")) 
				*end='\0';	
			else	q[strlen(q)-1]='\0';
			if (end>beg)	memcpy(Cseq, beg, end-beg);

			sprintf(SS2, "CSeq: %s ACK\r\n", Cseq);
			strcat(S200, SS2);
			flag++;
			continue;
		}

		if (flag == 5)
		{
			TraceLine();
			sprintf(SS2, "Contact: <sip:%s:%d>\r\n", g_Local_IP, g_Local_Port);
			strcat(S200, SS2);
			flag++;
			continue;
		}

		if (flag == 6)
		{
			TraceLine();
			sprintf(SS2, "Max-Forwards: 70\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}

		if (flag == 7)
		{
			TraceLine();
			sprintf(SS2, "Content-Length: 0\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}

		if (Get_Position(S11, header_name[flag])<0)	
		{ 
			TraceLine();
			flag++; 
			continue; 
		}

		TraceLine();
		printf("flag=%d, header_name=%s, start=%d, end=%d\n", flag, header_name[flag], Get_Position(S11, header_name[flag]), Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
		memcpy(q, &S11[Get_Position(S11, header_name[flag])], Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
		TraceLine();
		strcat(S200, q);
		printf("+++++++++++++++\n");
		printf("Start:%d Length:%d\n", Get_Position(S11, header_name[flag]), Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
		printf("%s", q);
		flag++;
	}
	strcat(S200, "\r\n");
	strcpy(S44, S200);


	RETURN;
}

void make_update(char *S44, char *S11)
{
	char S200[4000], SS2[2000], Cseq[100];
	int flag = 0;
	const char *header_name[] = {"Via:", "From:", "To:", "Call-ID:", "CSeq:"};

	sprintf(S200, "UPDATE sip:%s:%d SIP/2.0\r\n", g_Target_Real_IP, g_Target_SIP_Port);

	while (flag < 5)
	{
		char q[1000] = {0};

		if (flag == 4)
		{
			TraceLine();
			strcpy(SS2, "CSeq: 3 UPDATE\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}

		if (Get_Position(S11, header_name[flag])<0)	
		{ 
			TraceLine();
			flag++; 
			continue; 
		}

		TraceLine();
		printf("flag=%d, header_name=%s, start=%d, end=%d\n", flag, header_name[flag], Get_Position(S11, header_name[flag]), Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
		memcpy(q, &S11[Get_Position(S11, header_name[flag])], Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
		TraceLine();
		strcat(S200, q);
		printf("+++++++++++++++\n");
		printf("Start:%d Length:%d\n", Get_Position(S11, header_name[flag]), Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
		printf("%s", q);
		flag++;
	}
	strcpy(S44, S200);


	TraceLine();
	char szBody[2000];
	sprintf(szBody,
		"v=0\r\n"
		"o=- 0 0 IN IP4 %s\r\n"
		"s=-\r\n"
		"c=IN IP4 %s\r\n"
		"b=AS:41\r\n"
		"b=RS:512\r\n"
		"b=RR:1537\r\n"
		"t=0 0\r\n"
		"m=audio %d RTP/AVP 98 105\r\n"
		"b=AS:41\r\n"
		"b=RS:512\r\n"
		"b=RR:1537\r\n"
		"a=rtpmap:98 AMR-WB/16000/1\r\n"
		"a=ptime:20\r\n"
		"a=maxptime:240\r\n"
		"a=rtpmap:105 telephone-event/16000/1\r\n"
		"a=fmtp:98 mode-change-capability=2;max-red=0\r\n"
		"a=fmtp:105 0-15\r\n"
		"a=curr:qos local none\r\n"
		"a=curr:qos remote none\r\n"
		"a=des:qos mandatory local sendrecv\r\n"
		"a=des:qos optional remote sendrecv\r\n"
		"a=sendrecv\r\n",
		g_Meida_IP,
		g_Meida_IP,
		g_Meida_Port
	);

	TraceLine();
	int body_len = strlen(szBody);
	sprintf(S200, 
		"Max-Forwards: 70\r\n"
		"Route: <sip:%s:%d;lr>\r\n"
		"Content-Type: application/sdp\r\n"
		"Content-Length: %d\r\n"
		"\r\n"
		"%s",
		g_Local_IP, g_Local_Port,	// Route
		body_len,					// Content-Length
		szBody						// Body
	);
	strcat(S44, S200);

	RETURN;
}

void make_bye(char *S44, char *S11)
{
	char S200[4000], SS2[2000], Cseq[100];
	int flag = 0;
	const char *header_name[] = {"Via:", "From:", "To:", "Call-ID:", "CSeq:", "Contact:", "Max-Forwards:", "Content-Length:"};

	sprintf(S200, "BYE sip:%s:%d SIP/2.0\r\n", g_Target_Real_IP, g_Target_SIP_Port);

	TraceLine();
	while (flag < 8)
	{
		
		char q[2000] = {0};

		if (flag == 4)
		{
			sprintf(SS2, "CSeq: %d BYE\r\n", 1);
			strcat(S200, SS2);
			flag++;
			continue;
		}

		if (flag == 5)
		{
			TraceLine();
			sprintf(SS2, "Contact: <sip:%s:%d>\r\n", g_Local_IP, g_Local_Port);
			strcat(S200, SS2);
			flag++;
			continue;
		}

		if (flag == 6)
		{
			TraceLine();
			sprintf(SS2, "Max-Forwards: 70\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}

		if (flag == 7)
		{
			TraceLine();
			sprintf(SS2, "Content-Length: 0\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}

		if (Get_Position(S11, header_name[flag])<0)	
		{ 
			TraceLine();
			flag++; 
			continue; 
		}

		TraceLine();
		printf("flag=%d, header_name=%s, start=%d, end=%d\n", flag, header_name[flag], Get_Position(S11, header_name[flag]), Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
		memcpy(q, &S11[Get_Position(S11, header_name[flag])], Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
		TraceLine();
		strcat(S200, q);
		printf("+++++++++++++++\n");
		printf("Start:%d Length:%d\n", Get_Position(S11, header_name[flag]), Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
		printf("%s", q);
		flag++;
	}
	strcat(S200, "\r\n");
	strcpy(S44, S200);


	RETURN;
}

void get_sdp_addr(char *S11, sockaddr_in *pAddr)
{
	//--- get sdp
	char *beg, *end;
	char SS2[100];
	TraceLine();

	pAddr->sin_family = AF_INET;

	if (!(beg = strstr(S11, "c=IN IP4 ")))
	{	
		printf("get c= failed, can't find beg\n");
		RETURN;
	}
	if (end = strstr(beg, "\r\n"))
	{	
		beg = beg + strlen("c=IN IP4 ");
		int len = end-beg;
		memcpy(SS2, beg, len);
		SS2[len]=0;
		pAddr->sin_addr.s_addr = inet_addr(SS2);
		printf("c=%s\n", SS2);
	}
	else
	{
		printf("get c= failed, can't find end\n");
		RETURN;
	}

	// enforce to change rtp ip to PDN gw
	printf("enforce to change rtp ip to PDN gw:\n");
	pAddr->sin_addr.s_addr = inet_addr(g_Target_External_Meida_IP);
	printf("c=%s\n", g_Target_External_Meida_IP);

	if (!(beg = strstr(S11, "m=audio ")))	printf("get m= failed, can't find beg\n");
	if (end = strstr(beg, " RTP/AVP"))
	{	
		beg = beg+strlen("m=audio ");
		int len = end-beg;
		memcpy(SS2, beg, len);
		SS2[len]=0;
		pAddr->sin_port = htons((uint16_t)atoi(SS2));
		printf("m=%s\n", SS2);
	}
	else
	{	
		printf("get m= failed, can't find end\n");
		RETURN;
	}

	printf("RTP_Addr, IP=%s, Port=%d\n\n\n", inet_ntoa(pAddr->sin_addr), ntohs(pAddr->sin_port));
	RETURN;
}

//********************************************************************************

void res_200_ok_for_prack(char *S45, char *S11)
{
	TraceLine();
	char S200[4000], SS2[2000];
	int flag = 0;
	const char *header_name[] = {"Via:", "From:", "To:", "Call-ID:", "User-Agent:", "CSeq:", "Content-Type:", "Content-Length:", "P-Access-Network-Info"};
   
	sprintf(S200, "SIP/2.0 200 OK\r\n");

	while (flag < 9)
	{
		char q[2000] = {0};
		if (flag == 4)
		{
			TraceLine();
			sprintf(SS2, "User-Agent: VoLTE/WFC UA\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}

		if (flag == 6)
		{
			TraceLine();
			sprintf(SS2, "Content-Type: application/sdp\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}

		if (flag == 8)
		{
			TraceLine();
			sprintf(SS2, "P-Access-Network-Info: 3GPP-E-UTRAN-FDD;utran-cell-id-3gpp=001010001000000a\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}

		if (Get_Position(S11, header_name[flag])<0)	
		{ 
			TraceLine();
			flag++; 
			continue; 
		}

		TraceLine();
		memcpy(q, &S11[Get_Position(S11, header_name[flag])], Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
		strcat(S200, q);
		printf("+++++++++++++++\n");
		printf("Start:%d Length:%d\n", Get_Position(S11, header_name[flag]), Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
		printf("%s", q);
		flag++;
	}
	strcat(S200, "\r\n");
	strcpy(S45, S200);
}

//********************************************************************************

//測試用Update *******************************************************************
void res_test_update(char *S44, char *S11)
{
	char S200[4000], SS2[2000];
	int flag = 0;
	const char *header_name[] = {"Call-ID", "To:", "From", "CSeq", "Via", "Content-Length", "Content-Type"};

	sprintf(S200, "UPDATE sip:%s:%d SIP/2.0\r\n", g_Target_Real_IP, g_Target_SIP_Port);

	while (flag < 7)
	{
		char q[800] = {0};

		if (flag == 3)
		{

			sprintf(SS2, "CSeq: 3 UPDATE\r\n");
			strcat(SS2, q);
			strcat(S200, SS2);
			flag++;
			continue;
		}

		else if (flag == 6)
		{

			sprintf(SS2, "Content-Type: application/sdp\r\n");
			strcat(SS2, q);
			strcat(S200, SS2);
			flag++;
			continue;
		}

		memcpy(q, &S11[Get_Position(S11, header_name[flag])], Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
		strcat(S200, q);
		printf("+++++++++++++++\n");
		printf("Start:%d Length:%d\n", Get_Position(S11, header_name[flag]), Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
		printf("%s", q);
		flag++;
	}

	char szTest[1000] = {0};
	FILE *fp = fopen("Test_update.txt", "r");
	while (!feof(fp))
	{
		memset(szTest, 0, sizeof(szTest));
		fgets(szTest, sizeof(szTest) - 1, fp); // 包含了換行符
		printf("%s", szTest);

		strcat(S200, szTest);
	}
	fclose(fp);

	strcpy(S44, S200);
}
//********************************************************************************

//測試用180 Ringing***************************************************************
void res_test_180_ring(char *S44, char *S11)
{
	char S200[4000], SS2[2000];
	int flag = 0;
	const char *header_name[] = {"Call-ID", "Via", "To:", "From", "CSeq", "Content-Length", "Contact", "Allow"};

	sprintf(S200, "SIP/2.0 180 Ringing\r\n");

	while (flag < 8)
	{
		char q[800] = {0};

		if (flag == 1)
		{
			sprintf(SS2, "Via: SIP/2.0/UDP 192.168.200.10;rport=5060;received=192.168.200.10;");
			memcpy(q, &S11[Get_Position(S11, "branch=")], Get_NextLine(&S11[Get_Position(S11, "branch=")]));
			strcat(SS2, q);
			strcat(S200, SS2);
			flag++;
			continue;
		}

		else if (flag == 2)
		{

			sprintf(SS2, "To: <sip:%s;phone-context=001.01.eps.ims.mnc001.mcc001.3gppnetwork.org>;tag=PfZGsALGfGIAuK\r\n", g_UAC_Phone);
			strcat(S200, SS2);

			flag++;
			continue;
		}

		else if (flag == 5)
		{
			sprintf(SS2, "Content-Length: 0\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}

		else if (flag == 6)
		{
			sprintf(SS2, "Contact: <sip:boting-000001@%s:%d;comp=sigcomp;encode-parm=QbkRBthOEgsTXgkTBA0HHiUrKz1CQEFERUZBNhkLGh1sMTcgK2ghOyAnOCs.ITogYX9jZGV4OzQ6PQAcRVtZQlIWWEBaSU8BDAlLEhBSXFpdXA__;transport=udp>;+sip.instance=\"<urn:gsma:imei:35611207-004302-0>\";+g.3gpp.icsi-ref=\"urn%%3Aurn-7%%3A3gpp-service.ims.icsi.mmtel\";+g.3gpp.mid-call;+g.3gpp.srvcc-alerting\r\n", g_Local_IP, g_Local_Port);
			strcat(S200, SS2);

			flag++;
			continue;
		}

		else
		{
			memcpy(q, &S11[Get_Position(S11, header_name[flag])], Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
			strcat(S200, q);
			printf("+++++++++++++++\n");
			printf("Start:%d Length:%d\n", Get_Position(S11, header_name[flag]), Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
			printf("%s", q);
			flag++;
		}
	}
	strcat(S200, "\r\n");
	strcpy(S44, S200);
}
//********************************************************************************

//測試用200(for 180) OK *******************************************************************
void res_test_200_ok_ring(char *S44, char *S11)
{
	char S200[4000], SS2[2000];
	int flag = 0;
	const char *header_name[] = {"Call-ID", "Via", "To:", "From", "CSeq", "Content-Length", "Contact", "Allow", "Supported", "Require"};

	sprintf(S200, "SIP/2.0 200 OK\r\n");
	while (flag < 10)
	{
		char q[800] = {0};

		if (flag == 1)
		{
			sprintf(SS2, "Via: SIP/2.0/UDP 192.168.200.10;rport=5060;received=192.168.200.10;");
			memcpy(q, &S11[Get_Position(S11, "branch=")], Get_NextLine(&S11[Get_Position(S11, "branch=")]));
			strcat(SS2, q);
			strcat(S200, SS2);
			flag++;
			continue;
		}

		else if (flag == 2)
		{

			sprintf(SS2, "To: <sip:%s;phone-context=001.01.eps.ims.mnc001.mcc001.3gppnetwork.org>;tag=PfZGsALGfGIAuK\r\n", g_UAC_Phone);
			strcat(S200, SS2);

			flag++;
			continue;
		}

		else if (flag == 5)
		{
			sprintf(SS2, "Content-Length: 0\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}

		else if (flag == 6)
		{
			sprintf(SS2, "Contact: <sip:boting-000001@%s:%d;comp=sigcomp;encode-parm=QbkRBthOEgsTXgkTBA0HHiUrKz1CQEFERUZBNhkLGh1sMTcgK2ghOyAnOCs.ITogYX9jZGV4OzQ6PQAcRVtZQlIWWEBaSU8BDAlLEhBSXFpdXA__;transport=udp>;+sip.instance=\"<urn:gsma:imei:35611207-004302-0>\";+g.3gpp.icsi-ref=\"urn%%3Aurn-7%%3A3gpp-service.ims.icsi.mmtel\";+g.3gpp.mid-call;+g.3gpp.srvcc-alerting\r\n", g_Local_IP, g_Local_Port);
			strcat(S200, SS2);

			flag++;
			continue;
		}

		else if (flag == 8)
		{
			sprintf(SS2, "Supported: timer\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}

		else if (flag == 9)
		{
			sprintf(SS2, "Require: 100rel\r\n");
			strcat(S200, SS2);
			flag++;
			continue;
		}

		else
		{
			memcpy(q, &S11[Get_Position(S11, header_name[flag])], Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
			strcat(S200, q);
			printf("+++++++++++++++\n");
			printf("Start:%d Length:%d\n", Get_Position(S11, header_name[flag]), Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
			printf("%s", q);
			flag++;
		}
	}
	strcat(S200, "\r\n");
	strcpy(S44, S200);
}
//********************************************************************************

void res_sub_200_ok(char *S44, char *S11)
{
	char Sub200[4000], SS2[2000];
	int flag = 0;
	const char *header_name[] = {"Via", "From", "To:", "Call-ID", "Expires", "Contact", "Record-Route", "CSeq", "Content-Length"};

	sprintf(Sub200, "SIP/2.0 200 OK\r\n");

	while (flag < 9)
	{
		char q[800] = {0};
		if (flag == 6)
		{
			sprintf(SS2, "Record-Route: <sip:%s:%d;lr>\r\n", g_Local_IP, g_Local_Port);
			strcat(Sub200, SS2);
			flag++;
			continue;
		}
		//目前無用
		else if (flag == 100)
		{

			continue;
		}

		else
		{
			memcpy(q, &S11[Get_Position(S11, header_name[flag])], Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
			strcat(Sub200, q);
			printf("+++++++++++++++\n");
			printf("Start:%d Length:%d\n", Get_Position(S11, header_name[flag]), Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
			printf("%s", q);
			flag++;
		}
	}
	strcat(Sub200, "\r\n");
	strcpy(S44, Sub200);
}

void res_notify(char *S44, char *S11)
{
	char notify[4000], SS2[2000];
	int flag = 0;
	const char *header_name[] = {"Via", "From", "To:", "Subscription-State", "EVENT", "Expires", "Call-ID", "Contact", "Max-Forwards", "CSeq", "Content-Type", "Content-Length"};

	sprintf(notify, "NOTIFY sip:+886987654321@test.3gpp.com SIP/2.0\r\n");

	while (flag < 12)
	{
		char q[800] = {0};
		if (flag == 3)
		{
			sprintf(SS2, "Subscription-State: active\r\n");
			strcat(notify, SS2);
			flag++;
			continue;
		}

		else if (flag == 9)
		{
			sprintf(SS2, "CSeq: 1 NOTIFY\r\n");
			strcat(notify, SS2);
			flag++;
			continue;
		}

		else if (flag == 10)
		{
			sprintf(SS2, "Content-Type: application/reginfo+xml\r\n");
			strcat(notify, SS2);
			flag++;
			continue;
		}

		else if (flag == 11)
		{
			sprintf(SS2, "Content-Length: 417\r\n");
			strcat(notify, SS2);
			flag++;
			continue;
		}

		else
		{
			memcpy(q, &S11[Get_Position(S11, header_name[flag])], Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
			strcat(notify, q);
			printf("+++++++++++++++\n");
			printf("Start:%d Length:%d\n", Get_Position(S11, header_name[flag]), Get_NextLine(&S11[Get_Position(S11, header_name[flag])]));
			printf("%s", q);
			flag++;
		}
	}

	strcat(notify, "\r\n");

	sprintf(SS2, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
	strcat(notify, SS2);
	sprintf(SS2, "<reginfo xmlns=\"urn:ietf:params:xml:ns:reginfo\" version=\"0\" state=\"full\">\r\n");
	strcat(notify, SS2);
	sprintf(SS2, "<registration aor=\"sip:+886987654321@test.3gpp.com\" id=\"sip:+886987654321@test.3gpp.com\" state=\"active\">\r\n");
	strcat(notify, SS2);
	sprintf(SS2, "<contact id=\"sip:001010123456061@%s:%d\" state=\"active\" event=\"registered\" expires=\"600000\">\r\n", g_Local_IP, g_Local_Port);
	strcat(notify, SS2);
	sprintf(SS2, "<uri>sip:+886987654321@%s:%d</uri>\r\n", g_Target_Real_IP, g_Target_SIP_Port);
	strcat(notify, SS2);
	sprintf(SS2, "</contact>\r\n");
	strcat(notify, SS2);
	sprintf(SS2, "</registration>\r\n");
	strcat(notify, SS2);
	sprintf(SS2, "</reginfo>");
	strcat(notify, SS2);

	strcpy(S44, notify);
}
