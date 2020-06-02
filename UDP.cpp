// UDP.cpp : Defines the entry point for the console application.
//

#if defined(_MSC_VER)
	// Load TCPIP Library
	#pragma comment(lib,"ws2_32.lib")
	#include <winsock2.h>
	#include <stdlib.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include<netinet/sctp.h>
#include <algorithm>    // std::min
#include <iostream>

	typedef int SOCKET;
#endif	// OS




#include "StdAfx.h"
#include "Sip_Response/Sip_Response.h"
#include "AKAv1-MD5/Authentication.cpp"
#include "rtp.h"
#include "amrwb/amrwb_codec.h"
#include "sctp_client.h"



SOCKET      Server_Sock=0;
sockaddr_in Server_Addr;
sockaddr_in Client_Addr;
SOCKET		RTP_Sock=0;
sockaddr_in RTP_Addr;
char RTP_Data[1024]={0};
int	RTP_Data_Len=0;
time_t start_time=0;
char SIP_Temp_200_packet[10000];
pthread_mutex_t session_mutex = PTHREAD_MUTEX_INITIALIZER;
// amr-wb
void *enc_st = NULL;
void *dec_st = NULL;
TX_State *tx_state = NULL;
RX_State *rx_state = NULL;
FILE *f_serial_dec = NULL;                        /* File of serial bits for transmission  */
FILE *f_serial_enc = NULL; 
FILE *f_rtp = NULL;
FILE *f_synth = NULL;
long dec_frame=0;
bool	sent_0SIP_Cqi1_flag = false;


// PrintBinary
#define	PrintBinary_ENABLE		true
int PrintBinary(const char *msg, int len)
{
	#if PrintBinary_ENABLE
		for(int i=0,j=0,l=0;i<len;i+=16)
		{
			printf("  ");
			for(j=0,l=std::min(len-i,8);j<l;j++)	printf("%02x ",(unsigned char)msg[i+j]);
			if (l<=0)	continue;
			printf(" ");
			for(j=0,l=std::min(len-i-8,8);j<l;j++)	printf("%02x ",(unsigned char)msg[i+j+8]);
			printf("\n");
		}	
		printf("\n\n");
	#endif
	return 0;
}

ssize_t send_sip_data(sockaddr_in *pAddr, const char *data)
{
	sockaddr_in Addr;
	if (!pAddr)
	{
		pAddr = &Addr;
		Addr.sin_family = AF_INET;
		Addr.sin_port = htons(g_Target_SIP_Port);
		Addr.sin_addr.s_addr=inet_addr(g_Target_Real_IP);
	}	

	ssize_t send_len = sendto(Server_Sock, data, strlen(data), 0, (sockaddr *)pAddr, sizeof(sockaddr));
	printf("Sendto(%s) Port:%d Len:%ld=> %s\n", inet_ntoa(pAddr->sin_addr), ntohs(pAddr->sin_port), send_len, data);
	return send_len;
}

ssize_t send_rtp_data(sockaddr_in *pAddr, const char *data, size_t data_len)
{
	sockaddr_in Addr;

	TraceLine();
	rtp_packet	rtp(RTP_Data, sizeof(rtp_header)+data_len);
	if (rtp.get_sequence_number()==0)
	{
		rtp.set_V(2);
		rtp.set_PT(98);
		rtp.set_timestamp(0);
		rtp.set_ssrc(0x01020304);
	}
	rtp.set_sequence_number(rtp.get_sequence_number()+1);
	rtp.set_timestamp(rtp.get_timestamp()+320);
	memcpy(&RTP_Data[rtp.header_size()], data, data_len);
	

	ssize_t send_len = sendto(RTP_Sock, rtp.data(), rtp.size(), 0, (sockaddr *)pAddr, sizeof(sockaddr));
	printf("RTP_Socket = %d, data=0x%x, data_len=%ld\n", RTP_Sock, data, data_len);
	printf("Send rtp packet (%ld) to %s:%d :\n", send_len, inet_ntoa(pAddr->sin_addr), ntohs(pAddr->sin_port));
	if (send_len>0)	PrintBinary(RTP_Data, send_len);
	printf("\n");
	return send_len;
}


// int get_headers_test()
// {
// 	int i,j,k,l;
// 	char rtp_hdr[12];
// 	uint8_t Amrwb_CMR;
// 	uint8_t Amrwb_F;
// 	uint8_t Amrwb_FT;
// 	uint8_t Amrwb_Q;
// 	char	Amrwb_data[60];
// 	int 	Amrwb_data_len;
// 	const char S1[] = 
// 	{
// 		0x80, 0x62, 0x00, 0x00, 0x00, 0x00, 0x01, 0x40, 0xF8, 0x80, 0x9A, 0x23, 
// 		0xF4, 0x54, 0x42, 0x00, 0x15, 0x34, 0xA1, 0x01, 0x80, 0x18, 0x3F, 0xB7, 0xA8, 0x62, 0x33, 0xE4, 
// 		0xFC, 0x5B, 0xF6, 0x9D, 0xF3, 0xBE, 0xD0, 0x9A, 0xF0, 0x8B, 0x45, 0xC7, 0x74, 0x3A, 0x00, 0x8D, 
// 		0xD2, 0x2D, 0xB7, 0x32, 0xF4, 0x90, 0x54, 0x41, 0x8E, 0x48, 0x99, 0xA8, 0xA5, 0xC9, 0x2F, 0x72, 
// 		0xE7, 0x4A, 0xB3, 0x79, 0xC5, 0x42, 0xF2, 0x23, 0x84, 0x44, 0x66, 0x31, 0x76
// 	};
// 	int	len = sizeof(S1);
// 	printf("get_headers_test, len=%d\n", len);

// 	for (i=0,j=0; i<len; i++)
// 	{
// 		//--- get headers 
// 		// rtp header
// 		if (i<12)	rtp_hdr[j++] = S1[i];
// 		// payload header
// 		else if (i==12)
// 		{
// 			Amrwb_CMR = (S1[i]&0xf0) >> 4;
// 			Amrwb_F = (S1[i]&0x08) >> 3;
// 			Amrwb_FT = (S1[i]&0x07) << 1;
// 		}
// 		else if (i==13)
// 		{
// 			Amrwb_FT += (S1[i]&0x80) >> 7;
// 			Amrwb_Q = (S1[i]&0x40) >> 6;
// 			j=0;
// 			Amrwb_data[j] = (S1[i]&0x3f) << 2;
// 		}
// 		else
// 		{
// 			Amrwb_data[j++] += (S1[i]&0xc0) >> 6;
// 			Amrwb_data[j] = (S1[i]&0x3f) << 2;
// 		}
// 	}
// 	Amrwb_data_len = j+1;
// 	printf("get_headers_test, Amrwb_CMR=%d, Amrwb_F=%d, Amrwb_FT=%d, Amrwb_Q=%d, Amrwb_data_len=%d\n", Amrwb_CMR, Amrwb_F, Amrwb_FT, Amrwb_Q, Amrwb_data_len);

// 	//--- write raw file
// 	FILE *fp = NULL;
// 	if (fp = fopen("record/get_headers_test.raw","wb"))
// 	{
// 		fwrite(Amrwb_data, 1, j+1, fp);
// 		fclose(fp);
// 	}

// 	//=== write encoding file
// 	// FILE *f_speech;                        /* File of speech data                   */
//     FILE *f_serial;                        /* File of serial bits for transmission  */
//     FILE *f_mode = NULL;                   /* File of modes for each frame          */
//     Word16 signal[L_FRAME16k];             /* Buffer for speech @ 16kHz             */
// 	Word16 prms[NB_BITS_MAX];
//     Word16 allow_dtx, mode_file, mode = 0;
//     Word16 bitstreamformat=0;
//     long frame;
//     void *st;
// 	TX_State *tx_state;
// 	const char output_file_name[] = "record/get_headers_test.enc";

// 	//--- initialize coder
// 	Init_Amrwb_Coder(&st, &tx_state);
// 	allow_dtx = 1;
// 	mode = 0;
// 	frame = 0;

// 	if ((f_serial = fopen(output_file_name, "wb")) == NULL)
//     {
//         fprintf(stderr, "Error opening output bitstream file %s !!\n", output_file_name);
//     }

// 	//--- wrtie encoded data
// 		// Encode_Amrwb(st, allow_dtx, mode, signal, prms, frame);
// 	mode = (Amrwb_FT<9)?	Amrwb_FT:8;
// 	for (i=0,k=0; i<Amrwb_data_len; i++)
// 	{
// 		for (l=0; l<8; l++)
// 		{
// 			prms[k++] = (Amrwb_data[i]&(0x80>>l))? 0x007f:0xff81;
// 			printf("prms[%d]=0x%08x ", k-1, Amrwb_data[i]&(0x80>>l));
// 			if (k >= nb_of_bits[mode])	break;
// 		}
// 		printf("\n");
// 		if (k >= nb_of_bits[mode])	break;
// 	}
// 	Write_serial(f_serial, prms, mode, mode, tx_state, bitstreamformat);

// 	//--- free allocated memory
//     Close_Amrwb_Coder(st, tx_state);
//     // fclose(f_speech);
//     fclose(f_serial);
//     if (f_mode != NULL)
//     {
//        fclose(f_mode);
//     }

// 	return 0;
// }

// int read_encoded_data_test()
// {
// 	TraceLine();
// 	// Word16 synth[L_FRAME16k];              /* Buffer for speech @ 16kHz             */
//     Word16 prms[NB_BITS_MAX];
// 	Word16 *p_prms = prms;
// 	char stream[NB_BITS_MAX+2];
//     Word16 nb_bits, mode, frame_type;
//     // long dec_frame;
// 	Word16 bitstreamformat = 0;
// 	// char magic[10];
// 	int	i, j, dnum;


// 	if (!f_serial_dec)
// 	{
// 		const char input_file_name[] = "sipaudio.enc";
// 		if ((f_serial_dec = fopen(input_file_name, "rb")) == NULL)
// 		{
// 			fprintf(stderr, "Input file '%s' does not exist !!\n", input_file_name);
// 		} else
// 			fprintf(stderr, "Input bitstream file:   %s\n", input_file_name);
// 	}
// 	for (dnum=0; (nb_bits = Read_serial(f_serial_dec, prms, &frame_type, &mode, rx_state, bitstreamformat)) != 0; dnum++)
// 	{
// 		if (dnum>=5)	break;
// 		printf("read_encoded_data_test, frame_type=%d, mode=%d\n", frame_type, mode);
// 		printf("prms:\n");
// 		PrintBinary((char*)prms, nb_bits*2);
// 		if(bitstreamformat == 0)				/* default file format */
// 		{
// 			stream[0] = mode << 4;
// 			if (frame_type==RX_SPEECH_GOOD)	stream[1] = (mode << 3) + 0x03;
// 			else if (frame_type==RX_SPEECH_BAD)	stream[1] = (mode << 3);
// 			else if (frame_type==RX_SPEECH_LOST)	stream[1] = (14 << 3);
// 			else if (frame_type==RX_SID_FIRST)	stream[1] = (9 << 3) + 0x03;
// 			else if (frame_type==RX_SID_UPDATE)	stream[1] = (9 << 3) + 0x03;
// 			else if (frame_type==RX_SID_BAD)	stream[1] = (9 << 3);
// 			else if (frame_type==RX_NO_DATA)	stream[1] = (15 << 3) + 0x03;
// 			else	stream[1] = (mode << 3) + 0x03;
			

// 			for (i=0,j=0,p_prms=prms; j<nb_bits; i++,j+=8)
// 			{
// 				if (nb_bits-j>=8)	stream[2+i] = (char)Serial_parm(8, &p_prms);
// 				else
// 				{
// 					stream[2+i] = (char)Serial_parm(nb_bits-j, &p_prms) << (8-(nb_bits-j));
// 					// Parm_serial(stream[2+i], nb_bits-j, &(&prms[j]));
// 					// stream[2+i] = (stream[2+i] << 8-(nb_bits-j));
// 				}	
// 			}
// 			printf("stream:\n");
// 			PrintBinary((char*)stream, 2+i);

// 		} 
// 	}
// 	if (f_serial_dec)
// 	{
// 		fclose(f_serial_dec);
// 		f_serial_dec = NULL;
// 	}
// 	return 0;
// }

// int sctp_client_test()
// {
// 	int	sip_cqi1_fd = 0;
// 	struct sockaddr_in sip_cqi1_addr[1];
// 	char aSIP_CQI1[256];

// 	//--- send 0SIP_CQI1 command
// 	TraceLine();
// 	sprintf(aSIP_CQI1, "0SIP_CQI1|CallerIP:%x|CallerPort:%x|CalleeIP:%x|CalleePort:%x|", 
// 		ntohl(RTP_Addr.sin_addr.s_addr), 
// 		ntohs(RTP_Addr.sin_port), 
// 		ntohl(inet_addr(g_Meida_IP)), 
// 		g_Meida_Port);
// 	TraceLine();
// 	sip_cqi1_addr->sin_family = AF_INET;
// 	sip_cqi1_addr->sin_addr.s_addr=inet_addr("10.102.81.100");
// 	sip_cqi1_addr->sin_port = htons(36412);

// 	sctp_connect(sip_cqi1_fd, sip_cqi1_addr);
// 	TraceLine();
// 	iovec	iov = {aSIP_CQI1, strlen(aSIP_CQI1)};
// 	TraceLine();
// 	send_data(sip_cqi1_fd, &iov, 1); 
// 	TraceLine();
// 	printf("send command = %d\n",aSIP_CQI1);

// 	sleep(3);
// 	disconnect(sip_cqi1_fd);
// 	printf("close\n");

// 	return 0;
// }

int start_session()
{
	TraceLine();
	pthread_mutex_lock(&session_mutex);

	if (!enc_st)	Init_Amrwb_Coder(&enc_st, &tx_state);
    if (!dec_st)	Init_Amrwb_Decoder(&dec_st, &rx_state);
	dec_frame = 0;

	time(&start_time);
	if (!f_serial_enc)
	{
		char output_file_name[100];
		sprintf(output_file_name, "record/sharp_%ld.enc", start_time);
		if ((f_serial_enc = fopen(output_file_name, "wb")) == NULL)
		{
			fprintf(stderr, "Error opening output bitstream file %s !!\n", output_file_name);
		}
	}

	if (!f_synth)
	{
		char output_file_name[100];
		sprintf(output_file_name, "record/sharp_%ld.ad.pcm", start_time);
		if ((f_synth = fopen(output_file_name, "wb")) == NULL)
		{
			fprintf(stderr, "Error opening output bitstream file %s !!\n", output_file_name);
		}
	}
	
	// Open file for synthesis and packed serial stream
	if (!f_serial_dec)
	{
		// const char input_file_name[] = "sipaudio3.enc";
		const char input_file_name[] = g_Audio_Play_File;
		if ((f_serial_dec = fopen(input_file_name, "rb")) == NULL)
		{
			fprintf(stderr, "Input file '%s' does not exist !!\n", input_file_name);
		} else
			fprintf(stderr, "Input bitstream file:   %s\n", input_file_name);
	}

	if (!f_rtp)
	{
		char file_name[100];
		sprintf(file_name, "record/sharp_%ld_recvdata.raw", start_time);
		f_rtp = fopen(file_name, "wb");
	}

	sent_0SIP_Cqi1_flag = false;
	pthread_mutex_unlock(&session_mutex);
	
	return 0;
}

int close_session()
{
	TraceLine();
	if (f_serial_enc)
	{
		fclose(f_serial_enc);
		f_serial_enc = NULL;
		//--- decode
		if (dec_frame>10)
		{
			TraceLine();
			char input_file_name[100], output_file_name[100];
			sprintf(input_file_name, "record/sharp_%ld.enc", start_time);
			sprintf(output_file_name, "record/sharp_%ld.dec.pcm", start_time);
			decode_audio_file(input_file_name, output_file_name);
		}
	}
	TraceLine();
	if (f_serial_dec)
	{
		fclose(f_serial_dec);
		f_serial_dec = NULL;
	}
	TraceLine();
	if (f_synth)
	{
		fclose(f_synth);
		f_synth = NULL;
	}
	TraceLine();
	if (f_rtp)
	{
		fclose(f_rtp);
		f_rtp = NULL;
	}
	TraceLine();
	if (dec_st)
	{
		Close_Amrwb_Coder(enc_st, tx_state);
		enc_st = NULL;
		tx_state = NULL;
	}
	TraceLine();
	if (dec_st)
	{
		Close_Amrwb_Decoder(dec_st, rx_state);
		dec_st = NULL;
		rx_state = NULL;
	}
	sent_0SIP_Cqi1_flag = false;

	RETURN 0;
}

void* rtp_start(void* arg){
	sockaddr_in	Addr;
	int         len;
	uint32_t	Len=sizeof(sockaddr);
	char        S1[101000],S2[8000],S3[8000],S5[1000];
	int			i, j, k, l;
	int			send_len;

	RTP_Sock = socket(AF_INET, SOCK_DGRAM, 0);

	Addr.sin_family = AF_INET;
	Addr.sin_port = htons(g_Meida_Port);
	Addr.sin_addr.s_addr=htonl(INADDR_ANY);
	int ret = bind(RTP_Sock, (sockaddr *)&Addr, Len);
	printf("Socket=%d, ret=%d\n", RTP_Sock, ret);

	time_t current_time=0;

	//--- declare variables of amrwb
    Word16 synth[L_FRAME16k];              /* Buffer for speech @ 16kHz             */
    Word16 prms[NB_BITS_MAX];
	Word16 *p_prms = prms;
	// char stream[NB_BITS_MAX+2];
    Word16 nb_bits, mode, frame_type;
	Word16 bitstreamformat = 0;
	char magic[10];

	// rtp data
	char rtp_hdr[12];
	uint8_t Amrwb_CMR;
	uint8_t Amrwb_F;
	uint8_t Amrwb_FT;
	uint8_t Amrwb_Q;
	char	Amrwb_data[60];
	int 	Amrwb_data_len;
	bool	old_mode = -1;

	TraceLine();
	//--- init amrwb
	bitstreamformat = 0;
    // Initialization of decoder
	dec_frame = 0;
	// initialize coder
	mode = 0;

	
	//--- unit test
	// get_headers_test();
	// read_encoded_data_test();
	// sctp_client_test();

	TraceLine();
	while (1)
	{
		len = recvfrom(RTP_Sock, S1, sizeof(S1), 0, (sockaddr *)&Addr, &Len);
		TraceLine();
		// send_len = sendto(RTP_Sock, S1, len, 0, (sockaddr *)&Addr, sizeof(sockaddr));	// for speech echo
		// continue;
		if (RTP_Addr.sin_addr.s_addr!=Addr.sin_addr.s_addr || RTP_Addr.sin_port!=Addr.sin_port)
			memcpy(&RTP_Addr, &Addr, sizeof(RTP_Addr));

		if (len > 0)
		{
			TraceLine();
			//--- print received rtp packets
			printf("Recv rtp packet (%d) =\n", len);
			PrintBinary(S1, len);
			printf("\n");

			//--- write received packet
			TraceLine();
			pthread_mutex_lock(&session_mutex);
			if (f_rtp)
			{
				char separation[17];
				sprintf(separation, "------%04d------", len);
				fwrite(S1, 1, len, f_rtp);
				fwrite(separation, 1, 16, f_rtp);
			}
			pthread_mutex_unlock(&session_mutex);
			TraceLine();
			if (f_serial_enc)
			{
				// transform to 3GPP format
				for (i=0,j=0; i<len; i++)
				{
					//--- get headers 
					// rtp header
					if (i<12)	rtp_hdr[j++] = S1[i];
					// payload header
					else if (i==12)
					{
						Amrwb_CMR = (S1[i]&0xf0) >> 4;
						Amrwb_F = (S1[i]&0x08) >> 3;
						Amrwb_FT = (S1[i]&0x07) << 1;
					}
					else if (i==13)
					{
						Amrwb_FT += (S1[i]&0x80) >> 7;
						Amrwb_Q = (S1[i]&0x40) >> 6;
						j=0;
						Amrwb_data[j] = (S1[i]&0x3f) << 2;
					}
					else
					{
						Amrwb_data[j++] += (S1[i]&0xc0) >> 6;
						Amrwb_data[j] = (S1[i]&0x3f) << 2;
					}
				}
				Amrwb_data_len = j+1;
				printf("get_headers_test, Amrwb_CMR=%d, Amrwb_F=%d, Amrwb_FT=%d, Amrwb_Q=%d, Amrwb_data_len=%d\n", Amrwb_CMR, Amrwb_F, Amrwb_FT, Amrwb_Q, Amrwb_data_len);
				
				TraceLine();
				old_mode = mode;
				Reset_WMOPS_counter();
				if (Amrwb_FT<9)
				{
					if (Amrwb_Q) frame_type = RX_SPEECH_GOOD;
					else	frame_type = RX_SPEECH_BAD;
					mode = Amrwb_FT;	// [Qbit] 1: SPEECH_GOOD, 0: SPEECH_BAD
				}
				else if (Amrwb_FT==9)
				{
					if (Amrwb_Q)
					{ 
						if (old_mode!=9)	frame_type = RX_SID_FIRST;
						else	frame_type = RX_SID_UPDATE;
					}
					else	frame_type = RX_SID_BAD;
					mode = Amrwb_FT;	// [Qbit] 1: SID_FIRST or SID_UPDATE, 0: SID_BAD 
				}	
				else if (Amrwb_FT==14)
				{	
					frame_type = RX_SPEECH_LOST;
					mode = 9;	// SPEECH_LOST
				}
				else if (Amrwb_FT==15)
				{	
					frame_type = RX_NO_DATA;
					mode = 9;	// NO_DATA 
				}
				else	mode = -1;
				
				for (i=0,k=0; i<Amrwb_data_len; i++)
				{
					for (l=0; l<8; l++)
					{
						prms[sort_ptr[mode][k++]] = (Amrwb_data[i]&(0x80>>l))? 0x007f:0xff81;
						// printf("prms[%d]=0x%08x ", k-1, Amrwb_data[i]&(0x80>>l));
						// if (i==7)	printf("\n");
						if (k >= nb_of_bits[mode])	break;
					}
					if (k >= nb_of_bits[mode])	break;
				}
				// WMOPS_output((Word16) (mode == MRDTX));
				printf("rtp_start, f_serial_enc=0x%x, mode=%d, tx_state=0x%x, bitstreamformat=%d\n", f_serial_enc, mode, tx_state, bitstreamformat);
				// Reset_encoder(enc_st, 1);
				if (f_serial_enc && mode>=0) 
				{
					decode_amrwb(dec_st, frame_type, mode, prms, synth, dec_frame);
					fwrite(synth, sizeof(Word16), L_FRAME16k, f_synth);

					Write_serial(f_serial_enc, prms, mode, mode, tx_state, bitstreamformat);
					fflush(f_serial_enc);
				}
				TraceLine();
				printf("dec_frame = %d\n", dec_frame);
				PrintBinary((char*)prms, nb_of_bits[mode]*2);
			}

			TraceLine();
			//--- send rtp packets
			if (f_serial_dec)
			{
				TraceLine();
				int		bit_num;
				uint8_t	data_temp[64];
				char	packet[64];
				int		packet_len;
				char	packet_ft_q;

				if ((nb_bits = Read_serial(f_serial_dec, prms, &frame_type, &mode, rx_state, bitstreamformat)) != 0)
				{
					TraceLine();
					PrintBinary((char*)prms, nb_bits*2);

					memset(data_temp, 0 ,sizeof(data_temp));
					for (i=0,j=0; j<nb_bits; i++)
					{
						bit_num = (nb_bits-j>=8)? 8:(nb_bits-j);
						for (k=0; k<bit_num; k++,j++)
						{	
							if (prms[sort_ptr[mode][j]]==BIT_1)
							{	
								data_temp[i]++;
							}
							if (k<bit_num-1)	data_temp[i] <<= 1;
						}
						if (bit_num<8)	data_temp[i] <<= (8-bit_num);
					}
					TraceLine();
					PrintBinary((char*)data_temp, packed_size[mode]);
					
					for (i=0,j=0,k=0; i<packed_size[mode]; i++)
					{
						if (i==0)
						{
							if (frame_type==RX_SPEECH_GOOD)	packet_ft_q = (mode << 1) + 1;
							else if (frame_type==RX_SPEECH_BAD)	packet_ft_q = (mode << 1);
							else if (frame_type==RX_SPEECH_LOST)	packet_ft_q = (14 << 1);
							else if (frame_type==RX_SID_FIRST)	packet_ft_q = (9 << 1) + 1;
							else if (frame_type==RX_SID_UPDATE)	packet_ft_q = (9 << 1) + 1;
							else if (frame_type==RX_SID_BAD)	packet_ft_q = (9 << 1);
							else if (frame_type==RX_NO_DATA)	packet_ft_q = (15 << 1) + 1;
							else	packet_ft_q = (9 << 1);
							
							printf("packet_ft_q=%d, mode=%d\n", packet_ft_q, mode);
							packet[j] = mode << 4;
							packet[j] += 0 << 3;	// F
							packet[j] += packet_ft_q >> 2;	// FT
							j++;
							packet[j] = (packet_ft_q & 0x03) << 6;	// FT and Q
							// append data
							packet[j] += data_temp[i] >> 2;
							printf("packet[j] += data_temp[%d]=0x%02x >> 2  <-- 0x%02x\n", i, data_temp[i], data_temp[i] >> 2);
							j++;
							packet[j] = (data_temp[i] & 0x03) << 6;
							printf("packet[j] = (data_temp[%d]=0x%02x & 0x03) << 6  <-- 0x%02x\n", i, data_temp[i], (data_temp[i] & 0x03) << 6);
						}	
						else
						{
							packet[j] += data_temp[i] >> 2;
							printf("packet[j] += data_temp[%d]=0x%02x >> 2  <-- 0x%02x\n", i, data_temp[i], data_temp[i] >> 2);
							if ((unpacked_size[mode]-i*8)<=6)	break;
							j++;
							packet[j] = (data_temp[i] & 0x03) << 6;
							printf("packet[j] = (data_temp[%d]=0x%02x & 0x03) << 6  <-- 0x%02x\n", i, data_temp[i], (data_temp[i] & 0x03) << 6);
						}
					}
					packet_len = j+1;

					PrintBinary(packet, packet_len);



					TraceLine();
					send_rtp_data(&RTP_Addr, (char*)packet, packet_len);
				}
				else
				{
					fclose(f_serial_dec);
					f_serial_dec = NULL;
					if (caller)
					{
						char S4[1500];
						make_bye(S4, SIP_Temp_200_packet);
						send_sip_data(&Client_Addr, S4);	
						close_session();
					}
				}
				// 8062000000000140f8809a23f45442001534a10180183fb7a86233e4fc5bf69df3bed09af08b45c7743a008dd22db732f49054418e4899a8a5c92f72e74ab379c542f2238444663176
				// 8062000100000280f8809a23f4444180183be0b47812ca262bc528dcbe0dff04b2c059b8628b491c9d40101c892029c9e3781f41dc530aa3ade9aa27f433e09bc98a8f42382520c550
				// 8062000800000b40439ef642f4c00000000200
				
			}
		}
	}

	return NULL;
}

char *transfer_imsi_data_format(const char *str_imsi, char *hexstr_imsi)
{
	sprintf(hexstr_imsi, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
		str_imsi[0]-'0',
		str_imsi[1]-'0',
		str_imsi[2]-'0',
		str_imsi[3]-'0',
		str_imsi[4]-'0',
		str_imsi[5]-'0',
		str_imsi[6]-'0',
		str_imsi[7]-'0',
		str_imsi[8]-'0',
		str_imsi[9]-'0',
		str_imsi[10]-'0',
		str_imsi[11]-'0',
		str_imsi[12]-'0',
		str_imsi[13]-'0',
		str_imsi[14]-'0',
		str_imsi[15]-'0'
	);
	return hexstr_imsi;
}

int send_sipcqi1_command(sockaddr_in *caller_rtp_addr, char* s1ap_server_ip)
{
	TraceLine();
	if (!caller_rtp_addr || !s1ap_server_ip) {RETURN -1;}
	if (!send_0sipcqi1) {RETURN 0;}

	int	sip_cqi1_fd = 0;
	struct sockaddr_in sip_cqi1_addr[1];
	char aSIP_CQI1[256];
	char hexstr_caller_imsi[32], hexstr_callee_imsi[32];

	//--- send 0SIP_CQI1 command
	
	if (caller)
		sprintf(aSIP_CQI1, "0SIP_CQI1|CallerId:%s|CallerIP:%04x|CallerPort:%04x|CalleeId:%s|CalleeIP:%08x|CalleePort:%04x|", 
			"000000000000000000000000000000",	// transfer_imsi_data_format(g_UAC_Id, hexstr_callee_imsi),
			ntohl(inet_addr(g_Meida_IP)), 
			g_Meida_Port,
			transfer_imsi_data_format(g_Target_Id, hexstr_caller_imsi),
			ntohl(caller_rtp_addr->sin_addr.s_addr), 
			ntohs(caller_rtp_addr->sin_port)
		);
	else
		sprintf(aSIP_CQI1, "0SIP_CQI1|CallerId:%s|CallerIP:%04x|CallerPort:%04x|CalleeId:%s|CalleeIP:%08x|CalleePort:%04x|", 
			transfer_imsi_data_format(g_Target_Id, hexstr_caller_imsi),
			ntohl(caller_rtp_addr->sin_addr.s_addr), 
			ntohs(caller_rtp_addr->sin_port), 
			"000000000000000000000000000000",	// transfer_imsi_data_format(g_UAC_Id, hexstr_callee_imsi),
			ntohl(inet_addr(g_Meida_IP)), 
			g_Meida_Port
		);
	TraceLine();
	sip_cqi1_addr->sin_family = AF_INET;
	sip_cqi1_addr->sin_addr.s_addr=inet_addr(s1ap_server_ip);
	sip_cqi1_addr->sin_port = htons(36412);

	sctp_connect(sip_cqi1_fd, sip_cqi1_addr);
	TraceLine(); printf("time=%d\n", time(NULL));
	iovec	iov = {aSIP_CQI1, strlen(aSIP_CQI1)};
	TraceLine(); printf("time=%d\n", time(NULL));
	send_data(sip_cqi1_fd, &iov, 1); 
	TraceLine(); printf("time=%d\n", time(NULL));
	printf("send command = %d\n",aSIP_CQI1);

	sleep(3);
	disconnect(sip_cqi1_fd);
	printf("close\n");

	return 0;
}

int initialize()
{
	TraceLine();
	Server_Sock = 0;
	memset(&Server_Addr, 0, sizeof(Server_Addr));
	memset(&Client_Addr, 0, sizeof(Client_Addr));
	memset(&RTP_Addr, 0, sizeof(RTP_Addr));
	memset(RTP_Data, 0, sizeof(RTP_Data));
	memset(SIP_Temp_200_packet, 0, sizeof(SIP_Temp_200_packet));
	RTP_Sock = 0;
	RTP_Data_Len = 0;
	start_time = 0;
	session_mutex = PTHREAD_MUTEX_INITIALIZER;
	// amr-wb
	enc_st = NULL;
	dec_st = NULL;
	tx_state = NULL;
	rx_state = NULL;
	f_serial_dec = NULL;                        /* File of serial bits for transmission  */
	f_serial_enc = NULL; 
	f_rtp = NULL;
	f_synth = NULL;
	dec_frame = 0;
	sent_0SIP_Cqi1_flag = false;

	RETURN 0;
}

int main(int argc, char *argv[])
{
	// Variable Definition
#if defined(_MSC_VER)
	WSADATA     wsa;
	WSAStartup(0x202,&wsa);
#endif
	
	// initialize global variables
	initialize();


	int         i;
	unsigned int Len=sizeof(sockaddr);
	char        S1[101000],S2[8000],S3[8000],S5[1000];
	int j=0;
	char S4[8000], ch;
	int new_cmd = 0;
	int con_flag = 0;
	int send_len = 0;
	char szTest[1000] = {0};

	//--- create rtp port
	pthread_t tid;
	pthread_create(&tid,NULL,rtp_start,NULL);

	//--- start UAC
	TraceLine();
	system("ipconfig");
	// 1. Start UDP Server
	Server_Sock = socket(AF_INET, SOCK_DGRAM, 0);

	Server_Addr.sin_family = AF_INET;
	Server_Addr.sin_port = htons(g_Local_Port);
	Server_Addr.sin_addr.s_addr=htonl(INADDR_ANY);
	int ret = bind(Server_Sock, (sockaddr *)&Server_Addr, Len);
	printf("Server_Sock=%d, ret=%d\n", Server_Sock, ret);

	// 2. recv UDP Packet and print out
	printf("Waiting for UDP Packet Come-In\n");

	if (g_Register)
	{
		TraceLine();
		memset(S4, 0, sizeof(S4));
		// make_register_test(S4);
		make_register_UnAuth(S4);
		send_sip_data(NULL, S4);
	}
	else
	{
		if (caller)
		{
			TraceLine();
			memset(S4, 0, sizeof(S4));

			make_invite(S4);

			send_sip_data(NULL, S4);

			start_session();
		}
	}
	

	//get_res_autn_k();
	while (1)
	{
		TraceLine();
		i = recvfrom(Server_Sock, S1, sizeof(S1), 0, (sockaddr *)&Client_Addr, &Len);
		

		printf("i=%d\n",i);
		if (i > 0)
		{
			if (i<10)	continue;

			S1[i] = 0;
			printf("\n\n\nRecv(%s) Length:%d => %s\n\n\n", inet_ntoa(Client_Addr.sin_addr), i, S1);

			new_cmd = Get_request(S1, caller, g_Register);
			

			printf("New_Cmd:%d\n", new_cmd);
			
			TraceLine();
			if (new_cmd == SIP_REGISTER_UnAuth)
			{
				TraceLine();
				res_auth_401(S4, S1);
				send_sip_data(&Client_Addr, S4);
			}

			else if (new_cmd == SIP_REGISTER_OK)
			{
				TraceLine();
				res_200_for_register(S4, S1);
				send_sip_data(&Client_Addr, S4);
			}
			
			else if (new_cmd == SIP_SUBSCRIBE)
			{
				TraceLine();
				res_sub_200_ok(S4, S1);
				send_sip_data(&Client_Addr, S4);
			}

			// IMS get 401 when registering other sip server
			else if (new_cmd == SIP_401_REGISTER)	
			{
				TraceLine();
				make_register_Auth(S4, S1);
				send_sip_data(&Client_Addr, S4);
			}

			// IMS get 200 when registering other sip server
			else if (new_cmd == SIP_200_REGISTER)	
			{
				TraceLine();
				if (g_Register)
				{
					if (caller)
					{
						TraceLine();
						memset(S4, 0, sizeof(S4));

						make_invite(S4);

						send_sip_data(NULL, S4);

						start_session();
					}
				}
			}

			else if (new_cmd == SIP_BYE)
			{
				TraceLine();
				res_200_ok(S4, S1);
				send_sip_data(&Client_Addr, S4);
				close_session();
			}

			else if (new_cmd == SIP_CANCEL)
			{
				TraceLine();
				res_200_ok(S4, S1);
				send_sip_data(&Client_Addr, S4);
				close_session();
			}
			


			//���ե�INVITE
			else if (new_cmd == SIP_NOTIFY)
			{
				TraceLine();

				continue;
			}

			//���ե�100 trying
			else if (new_cmd == SIP_INVITE)
			{
				TraceLine();
				start_session();
				get_sdp_addr(S1, &RTP_Addr);
				printf("RTP_Addr: IP=%s, Port=%d\n", inet_ntoa(RTP_Addr.sin_addr), ntohs(RTP_Addr.sin_port));
				
				memset(S4, 0, sizeof(S4));
				//--- send 100
				res_test_100_trying(S4, S1, &Client_Addr);
				send_sip_data(&Client_Addr, S4);

				if (!direct_200)
				{
					//--- send 183
					get_sdp_addr(S1, &RTP_Addr);
					res_183_for_invite(S4, S1);
					send_sip_data(&Client_Addr, S4);
				}
				else
				{
					//--- send 180
					res_test_180_ringing(S4, S1, &Client_Addr);
					send_sip_data(&Client_Addr, S4);

					//--- send 0SIP_CQI1 command
					TraceLine();
					if (!caller)
					{	
						if (!sent_0SIP_Cqi1_flag)
						{
							sent_0SIP_Cqi1_flag = true;
							send_sipcqi1_command(&RTP_Addr, "10.102.81.100");
						}
					}


					//--- send 200
					sleep(2);
					res_200_for_invite(S4, S1, &Client_Addr);
					send_sip_data(&Client_Addr, S4);
				}
				
			}

			else if (new_cmd == SIP_183_INVITE)
			{
				TraceLine();
				get_sdp_addr(S1, &RTP_Addr);
				//--- send 0SIP_CQI1 command
				if (caller)
				{	
					if (!sent_0SIP_Cqi1_flag)
					{
						sent_0SIP_Cqi1_flag = true;
						send_sipcqi1_command(&RTP_Addr, "10.102.81.100");
					}
				}
			
				TraceLine();
				//--- send sip
				make_prack(S4, S1);
				// sleep(5);
				send_sip_data(&Client_Addr, S4);
			}

			else if (new_cmd == SIP_200_INVITE)
			{
				TraceLine();
				memcpy(SIP_Temp_200_packet, S1, strlen(S1));
				//--- send sip
				make_ack(S4, S1);
				send_sip_data(&Client_Addr, S4);
				//--- send rtp
				get_sdp_addr(S1, &RTP_Addr);
				uint8_t rtp_data[]=
				{
					0x80, 0x62, 0x00, 0x0b, 0x00, 0x00, 0x0f, 0x00, 0x09, 0x75, 0xec, 0x72, 
					0xf4, 0x50, 0x44, 0x82, 0x0e, 0x2c, 0xb0, 0x72, 0xe8, 0x52, 0xf0, 0x16, 0xa1, 0x65, 0x01, 0x79,
					0xaf, 0xfe, 0xea, 0x6f, 0x2f, 0xd3, 0x93, 0xc5, 0x70, 0xfd, 0xcc, 0xb3, 0xf1, 0x81, 0x0d, 0xa2,
					0xa1, 0x73, 0xe9, 0x51, 0xc9, 0x87, 0x08, 0xe5, 0xcc, 0x70, 0x36, 0xc5, 0x80, 0x0f, 0xfc, 0xa6,
					0xd3, 0xb2, 0x17, 0xdc, 0xbe, 0xd4, 0x61, 0x53, 0x66, 0xf2, 0x82, 0x4a, 0xd2
				};
				send_rtp_data(&RTP_Addr, (char*)rtp_data, sizeof(rtp_data));
			}

			else if (new_cmd == SIP_480_INVITE)
			{
				TraceLine();
				//--- send sip
				make_ack(S4, S1);
				send_sip_data(&Client_Addr, S4);
			}

			else if (new_cmd == SIP_200_PRACK)
			{
				TraceLine();
				// sleep(5);
				make_update(S4, S1);
				send_sip_data(&Client_Addr, S4);
			}

			else if (new_cmd == SIP_200_UPDATE)
			{
				TraceLine();
				get_sdp_addr(S1, &RTP_Addr);
			}

			else if (new_cmd == SIP_PRACK)
			{
				TraceLine();
				res_200_ok_for_prack(S4, S1);
				send_sip_data(&Client_Addr, S4);

				if (!direct_200)
				{
					//--- send 180
					res_test_180_ringing(S4, S1, &Client_Addr);
					send_sip_data(&Client_Addr, S4);
					//--- send 200
					sleep(1);
					res_200_for_invite(S4, S1, &Client_Addr);
					send_sip_data(&Client_Addr, S4);
				}
			}

			else
			{
				TraceLine();
				printf("\nCan not find the Response, %d @ %s\n", __LINE__, __FILE__);
				continue;
			}

		}
	}

	pthread_join(tid, NULL);

	return 0;
}
