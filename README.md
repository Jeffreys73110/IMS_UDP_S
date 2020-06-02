# IMS_UDP_S

The server supports basic initial attach and volte functions.
The version is developed based on the purpose of feasibility verification of internal calls of cellphones, so the server is somewhat unstable, incomplete.
In the purpose, the server send 0sSIP_CQI1 command as "create bearer request" which use s1ap protocol channel to deliver this command instead of sgi-s5-s11 protocols.
The server has two roles, Caller and Callee, and it can play one of the caller or callee roles at the same time.


## Architecture
```
    Architecture 1: the server acts as an IMS server without p_cscf server

    ┌───────────────┐       ┌───────────────┐       ┌───────────────┐       ┌───────────────────┐       ┌───────────────┐
    │               │       │               │       │               │       │                   │       │               │
    │               │       │               │       │               │       │                   │       │               │
    │       UE      ├──────▶│       eNB     ├──────▶│       MME     ├──────▶│       SGW/PGW     ├──────▶│   IMS_UDP_S   │
    │               │       │               │       │               │       │                   │       │  (IMS Server/ │
    │               │       │               │       │               │       │                   │       │     SIP UA)   │
    │               │       │               │       │               │       │                   │       │               │
    └───┬───────────┘       └───┬───────────┘       └───┬───────────┘       └───┬───────────┬───┘       └───┬───────────┘
        │                       │                       │                       │           │               │
        │                       │                       │                       │           │               │
        │                   ────▼───────────────────────▼───────────────────────▼──         │               │
        │                 10.102.81.59            10.102.81.100           10.102.81.102     │               │
        │                                            (MME_IP)                 (SGW_IP)      │               │
        │                                                                                   │               │
    ────▼───────────────────────────────────────────────────────────────────────────────────▼───────────────▼───
     (PDN_IP)                                                                            (PGW_IP)       (P_CSCF_IP)

    ┌───────────────┐       ┌───────────────┐               ┌───────────────┐
    │       UE      │       │    SGW/PGW    │               │   IMS_UDP_S   │
    │  (Cellphone)  │       │               │               │   (SIP UA)    │
    │               │       │               │               │               │
    │               │       │               │   REGISTER    │               │
    │     (PDN_IP)  ├──────▶│               │──────────────▶│ g_Local_IP:   │
    │ g_Target_Virtual_IP:  │ g_Target_Real_IP:             │ g_Local_Port  │
    │ g_Target_SIP_Port     │ g_Target_SIP_Port             │               │
    │               │       │               │               │               │
    │               │       │               │               │               │
    │               │       │               │               │               │
    │               │       │ g_Target_External_Meida_IP    │               │
    │               │       │           (PGW_IP)            │ g_Meida_IP:   │
    │               ├◀=====▶│               │◀=============▶│ g_Meida_Port  │
    └───────┬───────┘       └───────┬───────┘               └───────┬───────┘
            │                       │                               │
            │                       │                               │
            │                       │                               │
    ────────▼───────────────────────▼───────────────────────────────▼──────────
         (PDN_IP)                (PGW_IP)                      (SIP UA IP)


    ------------------------------------------------------------------------------------------------------------------------
    Architecture 2: the server acts as an SIP UA with p_cscf server

    ┌───────────────┐       ┌───────────────┐       ┌───────────────┐       ┌───────────────────┐       ┌───────────────┐       ┌───────────────┐
    │               │       │               │       │               │       │                   │       │               │       │               │
    │               │       │               │       │               │       │                   │       │               │       │               │
    │       UE      ├──────▶│       eNB     ├──────▶│       MME     ├──────▶│       SGW/PGW     ├──────▶│     P_CSCF    │◀──────┤   IMS_UDP_S   │
    │               │       │               │       │               │       │                   │       │               │       │   (SIP UA)    │
    │               │       │               │       │               │       │                   │       │               │       │               │
    │               │       │               │       │               │       │                   │       │               │       │               │
    └───┬───────────┘       └───┬───────────┘       └───┬───────────┘       └───┬───────────┬───┘       └───┬───────────┘       └───┬───────────┘
        │                       │                       │                       │           │               │                       │
        │                       │                       │                       │           │               │                       │
        │                   ────▼───────────────────────▼───────────────────────▼──         │               │                       │
        │                 10.102.81.59            10.102.81.100           10.102.81.102     │               │                       │
        │                                            (MME_IP)                 (SGW_IP)      │               │                       │
        │                                                                                   │               │                       │
    ────▼───────────────────────────────────────────────────────────────────────────────────▼───────────────▼───────────────────────▼───
     (PDN_IP)                                                                            (PGW_IP)       (P_CSCF_IP)            (SIP UA IP)

    ┌───────────────┐       ┌───────────────┐               ┌───────────────┐               ┌───────────────┐
    │       UE      │       │    SGW/PGW    │               │    P_CSCF     │               │   IMS_UDP_S   │
    │  (CellPhone ) │       │               │               │               │               │   (SIP UA)    │
    │               │       │               │               │               │               │               │
    │               │       │               │   REGISTER    │               │   REGISTER    │               │
    │      (PDN_IP) ├──────▶│               │──────────────▶│               │◀──────────────┤ g_Local_IP:   │
    │ g_Target_Virtual_IP:  │               │               │ g_Target_Real_IP:             │ g_Local_Port  │
    │ g_Target_SIP_Port     │               │               │ g_Target_SIP_Port             │               │
    │               │       │               │               │               │               │               │
    │               │       │               │               │               │               │               │
    │               │       │               │               │               │               │               │
    │               │       │ g_Target_External_Meida_IP    │               │               │               │
    │               │       │           (PGW_IP)            │               │               │ g_Meida_IP:   │
    │               ├◀=====▶│               │◀=============▶│               │◀=============▶│ g_Meida_Port  │
    └───────┬───────┘       └───────┬───────┘               └───────┬───────┘               └───────┬───────┘
            │                       │                               │                               │
            │                       │                               │                               │
            │                       │                               │                               │
    ────────▼───────────────────────▼───────────────────────────────▼───────────────────────────────▼──────────
         (PDN_IP)                (PGW_IP)                      (P_CSCF_IP)                     (SIP UA IP)

```

## OS
ubuntu 18.04

## Installation and Configuration
* set network environment
	```shell
 	 $ sudo sh setip.sh
	```
* Adjust configuration
	```shell
	# edit parameters and save file function
	$ vim StdAfx.h
	```

* Build code and run
	```shell
	# build amrwb objects first
	$ make amrwb

	# build code
	$ make

	# run server before the cellphone registering if the server acts as the IMS server
	# run server after the p_cscf server start up if the server acts as a SIP UA
	$ sudo ./ims.out
	```

* unset network environment
	```shell
  	$ sudo sh unset_ip.sh
	```

## Troubleshooting
#### Register failed
If wireshark shows the server always responds 401 responses for register packets, it has follows causes:
* **Check if the parameters of SIP content are filled out wrong values, such as Call-ID, Branch?**
	It probably has bugs. Debug or re-execute the server.
* **Check if 401 responses with wrong gtp teids?**
	It probably has bugs. Debug or re-execute the server.
* Check if SIP TCP connection established or rejected successfully?
	The cause of the problem is that TCP forwarding failed, which has bugs. Debug or re-execute the server.

#### 404 Not Found
If wireshark shows 404 Not Found when inviting, please check follows:
* **Check if g_Target_Real_IP and g_Target_SIP_Port parameters are set the correct values?**
  	Give it correct values.
* **Check if g_UAC_Phone/g_Target_Phone parameter are set the correct value?**
  	Give it correct values.

#### No RTP packet
If wireshark don't show any rtp packets after sip inviting procedure acknowledgement, please check follows:
* **Check if g_UAC_Id (IMS_UDP_S UA's IMSI) and g_Target_Id (call phone's IMSI) are correct?**
	Give it correct values.
* **Check if the config of p_cscf's SIP_ALG_ENABLE is disabled when p_cscf act as the IMS server?**
	Give it a false value.
* **Check if wireshark shows E-RABSetupResponse [RadioNetwork-cause=unknown-enb-ue-s1ap-id]**
	Enalbe the cellphone's airplan mode and disable it, and make a call again.
* **Check if wireshark shows E-RABSetupResponse [RadioNetwork-cause=multiple-E-RAB-ID-instances]**
	Enalbe the cellphone's airplan mode and disable it, and make a call again.

#### No sound but Wireshark shows rpt packets
If there is no sound but Wireshark shows rpt packets, please check follows:
* **Check if g_Audio_Play_File is not existed?**
  	Give a AMRWB encoded audio file.
* **Check if g_Meida_IP/g_Meida_Port/g_Target_External_Meida_IP are correct?**
	Give it correct values.
* **Check if send_0sipcqi1 is disable when p_cscf act as the IMS server?**
	Give it a false value;

#### Intermittent sound
The cause of intermittent sound is probably related to RTP timestamp.
It needs to research the timestamp of the cellphone in the future, especially the timestamp of Voice Activity Detection (VAD).

#### Program crash
Debug or re-execute the server.