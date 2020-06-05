// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__FEC714B3_74EA_469B_8A42_8E7E3327B803__INCLUDED_)
#define AFX_STDAFX_H__FEC714B3_74EA_469B_8A42_8E7E3327B803__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define	TraceLine()		printf("[TestMsg] %d @ %s\n", __LINE__, __FILE__)
#define RETURN			TraceLine();return

//--- controlling flags
const int caller = 0;			// 1: make the server be the "caller" role, 0: make the server be the "callee" role
const int direct_200 = 1;		// 1: respond 180 and 200 directly after receiving invite, 0: respond 183 first after receiving invite
const int g_Register = 1;		// 1: register when the server start up, 0: do nothing
const int send_0sipcqi1 = 1;	// 1: send 0SIP_CQI1 command before sending 183 or 200 OK for INVITE, 0: do not send


//--- identity and networking parameters of IMS
const char g_UAC_Id[]="001010123456075";	// Id for authentication
const char g_UAC_Phone[]="0987654322";		// phone number
const char g_Local_IP[32]="192.168.0.152";	// Local server IP
const unsigned short g_Local_Port=5060;		// Local server sip port
const char g_Meida_IP[32]="192.168.0.152";	// SDP IP
const unsigned short g_Meida_Port=40000;	// SDP Port

//--- identity and networking parameters of the target such as cellphone, p_cscf server/sip proxy/sip server
const char g_Target_Id[]="001010123456100";					// Id for authentication
const char g_Target_Phone[]="0987654321";					// phone number
const char g_Target_Real_IP[32]="192.168.0.148";			// real ip: UAS IP or SIP Proxy IP
const char g_Target_Virtual_IP[32]="192.168.200.10";		// virtual ip
const unsigned short g_Target_SIP_Port=5060;				// remote sip port
const char g_Target_External_Meida_IP[32]="192.168.0.152";	// for SDP IP


//--- amrwb parameters
#define	g_Audio_Play_File	"speech_audio/upset_song.enc"










enum SIP_Command_E
{
	SIP_REGISTER_UnAuth,
	SIP_REGISTER_OK,
	SIP_SUBSCRIBE,
	SIP_NOTIFY,
	SIP_PRACK,
	SIP_INVITE,
	SIP_183_INVITE,
	SIP_200_INVITE,
	SIP_480_INVITE,
	SIP_200_PRACK,
	SIP_200_UPDATE,
	SIP_200_BYE,
	SIP_MESSAGE,
	SIP_BYE,
	SIP_CANCEL,
	SIP_401_REGISTER,
	SIP_200_REGISTER,
	SIP_OTHER

};

// sudo ifconfig ens33:0 192.168.0.210 netmask 255.255.255.0
// sudo ifconfig ens33:0 down

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__FEC714B3_74EA_469B_8A42_8E7E3327B803__INCLUDED_)
