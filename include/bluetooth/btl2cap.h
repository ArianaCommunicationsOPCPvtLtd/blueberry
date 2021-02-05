/*******************************************************************************
**             __                                            __
**            /  \       ___    _       _      ___          /  \
**           /    \     |   |  | |     / \    |   \        /    \
**          /  /\  \    |  /   | |    /   \   |    \      /  /\  \
**         /  /  \  \   |  \   | |   /  _  \  | |\  \    /  /  \  \
**        /  /    \  \__|   \__| |__/  / \  \_| | \  \__/  /    \  \
**       /  /      \______/\__________/   \_____|  \______/      \  \
**      /  /  _  _                        _     ___    _        _ \  \
**  ___/  /  |  | | |\/| |\/| | | |\ | | |   /\  |  | | | |\ | |_  \  \___
**  \____/   |_ |_| |  | |  | |_| | \| | |_ /--\ |  | |_| | \|  _|  \____/
**  
** Company:        Ariana Communications OPC Private Limited
** Copyright (C) 2020 Ariana Communications - www.ariana-communications.com
**
** Description:    Raspberry Pi BareMetal Bluetooth L2CAP Header
**
** Dependencies:
** 
** Revision:
** Revision 0.1 - File Created
** Additional Comments:
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
** PLEASE REFER TO THE LICENSE INCLUDED IN THIS DISTRIBUTION.
** 
*******************************************************************************/
#ifndef _bt_l2cap_h
#define _bt_l2cap_h

#include <macros.h>
#include <types.h>
#include <bluetooth/bterror.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/btlogicallayer.h>
#include <bluetooth/bttransportlayer.h>
#include <stdlib.h>

#define BT_L2CAP_MAX_PSM_SLOT		40
#define BT_L2CAP_MAX_OPTION_LEN		50
#define BT_L2CAP_MIN_SIG_MTU_LEN	48
#define BT_L2CAP_MIN_CNL_MTU_LEN	670
#define BT_L2CAP_MAX_MTU_LEN		65535

typedef enum {
	BT_L2CAP_CLOSED,
	BT_L2CAP_W4_L2CAP_CONNECT_RSP,
	BT_L2CAP_W4_L2CA_CONNECT_RSP,
	BT_L2CAP_CONFIG,
	BT_L2CAP_OPEN,
	BT_L2CAP_W4_L2CAP_DISCONNECT_RSP,
	BT_L2CAP_W4_L2CA_DISCONNECT_RSP
} TBTChannelState;

typedef void TBTL2CAPCmdHandler(void*, void*, u16);

////////////////////////////////////////////////////////////////////////////////
//
// L2CAP
//
////////////////////////////////////////////////////////////////////////////////

// L2CAP Channel

class CBTL2CAPChannel
{
	u16	CID;
	u16	PSM;
	u16	MTU;
	u16	RemoteCID;
	u16	RemoteMTU;
	u32	RTX;
	u32	ERTX;
    bool Initiator;
	TBTChannelState State;
	CBTConnection*  Connection;
#define BT_L2CAP_DEFAULT_RTX	1000000	// 1sec
#define BT_L2CAP_DEFAULT_ERTX	1000000	// 1sec
	friend class CBTL2CAPLayer;

	public:
	CBTL2CAPChannel(u16 nPSM, CBTConnection *pConnection);
	CBTL2CAPChannel(u16 nPSM, CBTConnection *pConnection, u16 nCID);
	static u16 GetCID(void);
	inline CBTConnection* GetConnection(void) { return Connection; }
	inline void SetInitiator(bool init) { Initiator = init; }
	inline void SetState(TBTChannelState state) { State = state; }
	inline bool IsOpen(void) { return (State == BT_L2CAP_OPEN); }
	inline bool IsClosed(void) { return (State == BT_L2CAP_CLOSED); }
	inline bool IsConnectWait(void) {
		return (State == BT_L2CAP_W4_L2CAP_CONNECT_RSP
				|| State == BT_L2CAP_W4_L2CA_CONNECT_RSP); }
	inline bool IsDisconnectWait(void) {
		return (State == BT_L2CAP_W4_L2CAP_DISCONNECT_RSP
				|| State == BT_L2CAP_W4_L2CA_DISCONNECT_RSP); }
	void* operator new(size_t T) { return (void *)malloc(T); }
	void operator delete (void *ptr) { free(ptr); }
}
PACKED;

// L2CAP Packet

class CBTL2CAPPacket
{
	protected:
	u16	Length;
	u16	ChannelID;
#define BT_CID_NULL_IDENTIFIER				0x0000
#define BT_CID_SIGNALLING_CHANNEL			0x0001
#define BT_CID_CONNECTIONLESS_DATA_CHANNEL	0x0002
#define BT_CID_DYNAMICALLY_ALLOCATED		0x0040
	u8	Data[0];

	friend class CBTL2CAPLayer;

	public:
	CBTL2CAPPacket(u16 nLength, u16 nChannelID, u8* pData);
	CBTL2CAPPacket(u16 nLength, u16 nChannelID);

	u16 GetCID(void);
		
	void* operator new(size_t T) { return (void *)malloc(T); }
	void operator delete (void *ptr) { free(ptr); }
}
PACKED;

class CBTL2CAPConnectionOrientedDataPacket : public CBTL2CAPPacket
{
	private:
	u8	Information[BT_L2CAP_MIN_CNL_MTU_LEN];

	public:
	CBTL2CAPConnectionOrientedDataPacket(u16, u16, u8*);
		
	void* operator new(size_t T) { return (void *)malloc(T); }
	void operator delete (void *ptr) { free(ptr); }
}
PACKED;

class CBTL2CAPConnectionLessDataPacket : public CBTL2CAPPacket
{
	private:
	u16	PSM;
	u8	Information[BT_L2CAP_MIN_CNL_MTU_LEN];

	public:
	CBTL2CAPConnectionLessDataPacket(u16, u8*, u16);
		
	void* operator new(size_t T) { return (void *)malloc(T); }
	void operator delete (void *ptr) { free(ptr); }
}
PACKED;

class CBTL2CAPSignallingCommand;
class CBTL2CAPSignallingPacket : public CBTL2CAPPacket
{
	private:
	u8	Command[BT_L2CAP_MIN_SIG_MTU_LEN];

	public:
	CBTL2CAPSignallingPacket(u8*, u16);

	CBTL2CAPSignallingCommand* GetCommand(u8);
		
	void* operator new(size_t T) { return (void *)malloc(T); }
	void operator delete (void *ptr) { free(ptr); }
}
PACKED;

// L2CAP Option

struct SBTL2CAPFlowSpec {
	u8	Flags;
	u8	ServiceType;
#define BT_L2CAP_SERVICE_TYPE_NO_TRAFFIC	0x00
#define BT_L2CAP_SERVICE_TYPE_BEST_EFFORT	0x01
#define BT_L2CAP_SERVICE_TYPE_GUARANTEED	0x02
	u32	TokenRate;
	u32	TokenBucketSize;
	u32	PeakBandwidth;
	u32	Latency;
	u32	DelayVariation;
}
PACKED;
typedef struct SBTL2CAPFlowSpec TBTL2CAPFlowSpec;

class CBTL2CAPOption
{
	protected:
	u8	Type : 7;
	u8	Option : 1;
#define BT_L2CAP_OPTION_MTU				0x01
#define BT_L2CAP_OPTION_FLUSH_TIMEOUT	0x02
#define BT_L2CAP_OPTION_QOS				0x03
	u8	Length;

	friend u16 ExtractMTU(u8*, u16);
	friend TBTL2CAPFlowSpec* ExtractFlow(u8*, u16);
	friend u16 ExtractFlushTO(u8*, u16);
}
PACKED;

class CBTL2CAPMTU : public CBTL2CAPOption
{
#define BT_L2CAP_OPTION_MTU_LEN		0x0002
	u16	MTU;

	friend u16 ExtractMTU(u8*, u16);
	friend u8* InsertMTU(u8*, u16);
}
PACKED;

class CBTL2CAPFlushTimeout : public CBTL2CAPOption
{
#define BT_L2CAP_OPTION_FLUSH_TIMEOUT_LEN	0x0002
	u16	FlushTimeout;

	friend u16 ExtractFlushTO(u8*, u16);
	friend u8* InsertFlushTO(u8*, u16);
}
PACKED;

class CBTL2CAPQoS : public CBTL2CAPOption
{
#define BT_L2CAP_OPTION_QOS_LEN		0x0016
	TBTL2CAPFlowSpec	Flow;

	friend TBTL2CAPFlowSpec* ExtractFlow(u8*, u16);
	friend u8* InsertFlow(u8*, TBTL2CAPFlowSpec*);
}
PACKED;

// L2CAP Signalling Commands

class CBTL2CAPSignallingCommand
{
	u8	Code;
#define BT_SIG_RESERVED					0x00
#define BT_SIG_COMMAND_REJECT			0x01
#define BT_SIG_CONNECTION_REQUEST		0x02
#define BT_SIG_CONNECTION_RESPONSE		0x03
#define BT_SIG_CONFIGURATION_REQUEST	0x04
#define BT_SIG_CONFIGURE_RESPONSE		0x05
#define BT_SIG_DISCONNECTION_REQUEST	0x06
#define BT_SIG_DISCONNECTION_RESPONSE	0x07
#define BT_SIG_ECHO_REQUEST				0x08
#define BT_SIG_ECHO_RESPONSE			0x09
#define BT_SIG_INFORMATION_REQUEST		0x0A
#define BT_SIG_INFORMATION_RESPONSE		0x0B
#define BT_SIG_NUM_COMMANDS				0x0B

	protected:
	u8	Identifier;
	u16	Length;

	public:
	CBTL2CAPSignallingCommand();
	CBTL2CAPSignallingCommand(u8 nCode);
	CBTL2CAPSignallingCommand(u8 nCode, u8 nIdentifier);
	
	void Process(void*, u16);
	static void Register(u8, void*);
	static TBTL2CAPCmdHandler* Handler[BT_SIG_NUM_COMMANDS];

	u16 GetLength(void);
	
	void* operator new(size_t T) { return (void *)malloc(T); }
	void operator delete (void *ptr) { free(ptr); }

	friend class CBTL2CAPSignallingPacket;
}
PACKED;

class CBTL2CAPSignallingCommandReject : public CBTL2CAPSignallingCommand
{
	u16	Reason;
#define BT_L2CAP_COMMAND_NOT_UNDERSTOOD		0x0000
#define BT_L2CAP_SIGNALLING_MTU_EXCEEDED	0x0001
#define BT_L2CAP_INVALID_CID_IN_REQUEST		0x0002
	u8	Data[4];

	public:
	CBTL2CAPSignallingCommandReject();
	CBTL2CAPSignallingCommandReject(u8 nIdentifier, u16 nReason, u8 *pData);
	
	static void Handler(void*, void*, u16);
	void Process(void*, u16);
}
PACKED;

class CBTL2CAPConnectionRequest : public CBTL2CAPSignallingCommand
{
	u16	PSM;
	u16	SourceCID;
#define BT_L2CAP_RESULT_CONNECTION_SUCCESSFUL	0x0000
#define BT_L2CAP_RESULT_CONNECTION_PENDING		0x0001
#define BT_L2CAP_RESULT_PSM_NOT_SUPPORTED		0x0002
#define BT_L2CAP_RESULT_SECURITY_BLOCK			0x0003
#define BT_L2CAP_RESULT_CONNECTION_TIMEOUT		0xEEEE

	public:
	CBTL2CAPConnectionRequest();
	CBTL2CAPConnectionRequest(u8 nIdentifier, u16 nPSM, u16 nSourceCID);

	static void Handler(void*, void*, u16);
	void Process(void*, u16);
}
PACKED;

class CBTL2CAPConnectionResponse : public CBTL2CAPSignallingCommand
{
	u16	DestinationCID;
	u16	SourceCID;
	u16	Result;
#define BT_L2CAP_CONNECTION_SUCCESSFUL		0x0000
#define BT_L2CAP_CONNECTION_PENDING			0x0001
#define BT_L2CAP_PSM_NOT_SUPPORTED			0x0002
#define BT_L2CAP_SECURITY_BLOCK				0x0003
#define BT_L2CAP_NO_RESOURCES_AVAILABLE		0x0004
	u16	Status;
#define BT_L2CAP_STATUS_NO_FURTHER_INFORMATION	0x0000
#define BT_L2CAP_STATUS_AUTHENTICATION_PENDING	0x0001
#define BT_L2CAP_STATUS_AUTHORIZATION_PENDING	0x0002

#define BT_L2CAP_RESULT_RESPONSE_SUCCESSFULLY_SENT  0x0000
#define BT_L2CAP_RESULT_FAILURE_TO_MATCH            0x0001

	public:
	CBTL2CAPConnectionResponse();
	CBTL2CAPConnectionResponse(u8, u16, u16, u16, u16);

	static void Handler(void*, void*, u16);
	void Process(void*, u16);
}
PACKED;

class CBTL2CAPConfigurationRequest : public CBTL2CAPSignallingCommand
{
	u16	DestinationCID;
	u16	Flags;
#define BT_L2CAP_MORE_CONFIGURATION_REQUESTS	0x0001
	u8	Options[BT_L2CAP_MAX_OPTION_LEN];

	public:
	CBTL2CAPConfigurationRequest();
	CBTL2CAPConfigurationRequest(u8, u16, u16, CBTL2CAPOption*, u16);
	
	u16 GetMTU(void);
	TBTL2CAPFlowSpec* GetFlow(void);
	u16 GetFlushTO(void);

	static void Handler(void*, void*, u16);
	void Process(void*, u16);
}
PACKED;

class CBTL2CAPConfigurationResponse : public CBTL2CAPSignallingCommand
{
	u16	SourceCID;
	u16	Flags;
	u16	Result;
#define BT_L2CAP_RESULT_SUCCESS					0x0000
#define BT_L2CAP_RESULT_UNACCEPTABLE_PARAMETERS	0x0001
#define BT_L2CAP_RESULT_REJECTED				0x0002
#define BT_L2CAP_RESULT_UNKNOWN_OPTION			0x0003
	u8	Config[BT_L2CAP_MAX_OPTION_LEN];

	public:
	CBTL2CAPConfigurationResponse();
	CBTL2CAPConfigurationResponse(u8, u16, u16, u16, CBTL2CAPOption*, u16);
	
	u16 GetMTU(void);
	TBTL2CAPFlowSpec* GetFlow(void);
	u16 GetFlushTO(void);

	static void Handler(void*, void*, u16);
	void Process(void*, u16);
}
PACKED;

class CBTL2CAPDisconnectionRequest : public CBTL2CAPSignallingCommand
{
	u16	DestinationCID;
	u16	SourceCID;
#define BT_L2CAP_RESULT_DISCONNECTION_SUCCESSFUL		0x0000
#define BT_L2CAP_RESULT_DISCONNECTION_TIMEOUT_OCCURRED	0xEEEE

	public:
	CBTL2CAPDisconnectionRequest();
	CBTL2CAPDisconnectionRequest(u8, u16, u16);
	
	static void Handler(void*, void*, u16);
	void Process(void*, u16);
}
PACKED;

class CBTL2CAPDisconnectionResponse : public CBTL2CAPSignallingCommand
{
	u16	DestinationCID;
	u16	SourceCID;
#define BT_L2CAP_RESULT_DISCONNECTION_FAILURE	0x0001

	public:
	CBTL2CAPDisconnectionResponse();
	CBTL2CAPDisconnectionResponse(u8, u16, u16);
	
	static void Handler(void*, void*, u16);
	void Process(void*, u16);
}
PACKED;

class CBTL2CAPEchoRequest : public CBTL2CAPSignallingCommand
{
	u8	Data[BT_L2CAP_MIN_SIG_MTU_LEN];

	public:
	CBTL2CAPEchoRequest();
	CBTL2CAPEchoRequest(u8, u8*, u16);

	static void Handler(void*, void*, u16);
	void Process(void*, u16);
}
PACKED;

class CBTL2CAPEchoResponse : public CBTL2CAPSignallingCommand
{
	u8	Data[BT_L2CAP_MIN_SIG_MTU_LEN];

	public:
	CBTL2CAPEchoResponse();
	CBTL2CAPEchoResponse(u8, u8*, u16);

	static void Handler(void*, void*, u16);
	void Process(void*, u16);
}
PACKED;

class CBTL2CAPInformationRequest : public CBTL2CAPSignallingCommand
{
	u16	InfoType;
#define BT_L2CAP_CONNECTIONLESS_MTU	0x0001

	public:
	CBTL2CAPInformationRequest();
	CBTL2CAPInformationRequest(u8, u16);

	static void Handler(void*, void*, u16);
	void Process(void*, u16);
}
PACKED;

class CBTL2CAPInformationResponse : public CBTL2CAPSignallingCommand
{
	u16	InfoType;
	u16	Result;
#define BT_L2CAP_RESULT_NOT_SUPPORTED	0x0001
	u16	Data;

	public:
	CBTL2CAPInformationResponse();
	CBTL2CAPInformationResponse(u8, u16, u16, u16);

	static void Handler(void*, void*, u16);
	void Process(void*, u16);
}
PACKED;

// L2CA Events
class CBTL2CAEvent
{
	protected:
	u8	Identifier;
#define BT_EVENT_L2CA_CONNECT_IND		0x01
#define BT_EVENT_L2CA_CONNECT_CFM		0x02
#define BT_EVENT_L2CA_CONFIG_IND		0x03
#define BT_EVENT_L2CA_CONFIG_CFM		0x04
#define BT_EVENT_L2CA_DISCONNECT_IND	0x05
#define BT_EVENT_L2CA_QOS_VIOLATION_IND	0x06
	u16	Event;
	friend class CBTHIDPLayer;
}
PACKED;

class CBTL2CAConnectInd : public CBTL2CAEvent
{
	u8	BDAddr[BT_BD_ADDR_SIZE];
	u16	CID;
	u16	PSM;

	friend class CBTHIDPLayer;
	friend class CBTL2CAPConnectionRequest;
}
PACKED;

class CBTL2CAConnectCfm : public CBTL2CAEvent
{
	u16	DCID;
	u16	SCID;
	u16	Result;
	u16	Status;

	friend class CBTHIDPLayer;
	friend class CBTL2CAPConnectionResponse;
}
PACKED;

class CBTL2CAConfigInd : public CBTL2CAEvent
{
	u16	CID;
	u16	PSM;
	u16	OutMTU;
	TBTL2CAPFlowSpec*	InFlow;
	u16	InFlushTO;

	friend class CBTHIDPLayer;
	friend class CBTL2CAPConfigurationRequest;
}
PACKED;

class CBTL2CAConfigCfm : public CBTL2CAEvent
{
	u16	SourceCID;
	u16	Flags;
	u16	Result;
	u16	MTU;
	TBTL2CAPFlowSpec*	Flow;
	u16	FlushTO;

	friend class CBTHIDPLayer;
	friend class CBTL2CAPConfigurationResponse;
}
PACKED;

class CBTL2CADisconnectInd : public CBTL2CAEvent
{
	u16	CID;

	friend class CBTHIDPLayer;
	friend class CBTL2CAPDisconnectionRequest;
}
PACKED;

class CBTL2CAQoSViolationInd : public CBTL2CAEvent
{
	u8	BDAddr[BT_BD_ADDR_SIZE];

	friend class CBTL2CAPLayer;
	friend class CBTHIDPLayer;
}
PACKED;

class CBTL2CACommandRsp : public CBTL2CAEvent
{
	u16	Reason;

	friend class CBTL2CAPLayer;
}
PACKED;

class CBTL2CAConnectRsp : public CBTL2CAEvent
{
	u16	*DCID;
	u16	*SCID;
	u16	*Result;
	u16	*Status;
	TBTChannelState	*State;

	friend class CBTL2CAPLayer;
}
PACKED;

class CBTL2CAConfigRsp : public CBTL2CAEvent
{
	u16	SourceCID;
	u16	Flags;
	u16	Result;
	u16	MTU;
	TBTL2CAPFlowSpec*	Flow;
	u16	FlushTO;

	friend class CBTL2CAPLayer;
}
PACKED;

class CBTL2CADisconnectRsp : public CBTL2CAEvent
{
	u16	DCID;
	u16	SCID;

	friend class CBTL2CAPLayer;
}
PACKED;

class CBTL2CADataRead : public CBTL2CAEvent
{
	u16	CID;
	u16	Length;
	u8*	InBuffer;
	u16	Result;
	u16	N;

	friend class CBTL2CAPLayer;
}
PACKED;

// L2CAP Layer
class CBTSubSystem;
class CBTL2CAPLayer : public CBTLayer
{
public:
	CBTL2CAPLayer (CBTLogicalLayer *pLogicalLayer, CBTSubSystem *pSubSystem);
	~CBTL2CAPLayer (void);

	u8 GetID(void);
	void RegisterCallback(u16, TBTL2CAPCallback *);
	void RegisterDataCallback(u16, TBTL2CAPDataCallback *);
	void DeregisterDataCallback(u16);

	u16 Connect(u16 PSM, TBDAddr BDAddr, u16*, u16*);
	u16 ConnectResponse(TBDAddr, u8, u16, u16, u16);
	u16 Configure(u16,u16,TBTL2CAPFlowSpec*,u16,u16,u16*,TBTL2CAPFlowSpec*,u16*, bool=true);
	u16 ConfigureResponse(u8, u16, u16, u16, u16, u16, TBTL2CAPFlowSpec*);
	u16 Disconnect(u16);
	u16 DisconnectResponse(u8, u16);
	u16 Write(u16, u16, u8*, u16*);
	u16 Read(u16, u16, u8*, u16*);
	u16 GroupCreate(u16);
	u16 GroupClose(u16);
	u16 Ping(TBDAddr, u8*, u16, u8**, u16*);
	u16 GetInfo(TBDAddr, u16, u8**, u16*);
	u16 DisableConnectionlessTraffic(u16);
	u16 EnableConnectionlessTraffic(u16);
	u16 GetPSM(u16);
	CBTL2CAPChannel* GetChannel(u16,bool=false);
	bool AddChannel(u16, u16);
	bool DeleteChannel(u16);
	CBTDevice* GetDevice(u16);
	void SetCommandRsp(u8, u16);
	bool SetConnectRsp(u8, u16, u16, u16, u16);
	bool SetConfigRsp(u8, u16, u16, u16, u16, u16, u8*);
	void SetDisconnectRsp(u8, u16, u16);
	void SignallingCallback(u16, void*, size_t);

private:
	void LPEventHandler (const void *pBuffer, unsigned nLength);
	static void LPEventStub (const void *pBuffer, unsigned nLength);
	void L2CAPEventHandler (const void *pBuffer, unsigned nLength);
	static void L2CAPEventStub (const void *pBuffer, unsigned nLength);

	CBTConnection		*m_pIncomingConnection;
	CBTL2CACommandRsp	m_eCommandRsp;
	CBTL2CAConnectRsp	m_eConnectRsp;
	CBTL2CAConfigRsp	m_eConfigRsp;
	CBTL2CADisconnectRsp	m_eDisconnectRsp;
	CBTL2CADataRead		m_eDataRead;

	TBTL2CAPCallback* m_pL2CAPSignallingCallback[BT_L2CAP_MAX_PSM_SLOT];
	TBTL2CAPDataCallback* m_pPSMSlot[BT_L2CAP_MAX_PSM_SLOT];

	CBTLogicalLayer *m_pLogicalLayer;
	CBTSubSystem *m_pSubSystem;

	CPtrArray m_Channels;

	u8 *m_pEventBuffer;

	static CBTL2CAPLayer *s_pThis;
};

#endif
