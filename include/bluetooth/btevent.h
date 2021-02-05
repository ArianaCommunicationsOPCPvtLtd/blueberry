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
** Description:    Raspberry Pi BareMetal Bluetooth Event Header
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
#ifndef _bt_event_h
#define _bt_event_h

#include <macros.h>
#include <types.h>
#include <stdlib.h>


// Sizes

#define BT_MAX_HCI_EVENT_SIZE	257

typedef void TBTLMPEventHandler(void*, void*, u16);

////////////////////////////////////////////////////////////////////////////////
//
// HCI
//
////////////////////////////////////////////////////////////////////////////////

// Events

class CBTHCIEvent
{
	protected:
	void* operator new(size_t T) { return (void *)malloc(T); }
	void operator delete (void *ptr) { free(ptr); }

	u8	EventCode;
#define BT_EVENT_CODE_INQUIRY_COMPLETE		0x01
#define BT_EVENT_CODE_INQUIRY_RESULT		0x02
#define BT_EVENT_CODE_CONNECTION_COMPLETE	0x03
#define BT_EVENT_CODE_CONNECTION_REQUEST	0x04
#define BT_EVENT_CODE_DISCONNECTION_COMPLETE		0x05
#define BT_EVENT_CODE_AUTHENTICATION_COMPLETE		0x06
#define BT_EVENT_CODE_REMOTE_NAME_REQUEST_COMPLETE	0x07
#define BT_EVENT_CODE_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE	0x0B
#define BT_EVENT_CODE_READ_REMOTE_VERSION_INFORMATION_COMPLETE	0x0C
#define BT_EVENT_CODE_COMMAND_COMPLETE		0x0E
#define BT_EVENT_CODE_COMMAND_STATUS		0x0F
#define BT_EVENT_CODE_HARDWARE_ERROR		0x10
#define BT_EVENT_CODE_FLUSH_OCCURRED		0x11
#define BT_EVENT_CODE_ROLE_CHANGE			0x12
#define BT_EVENT_CODE_NUMBER_OF_COMPLETED_PACKETS	0x13
#define BT_EVENT_CODE_MODE_CHANGE			0x14
#define BT_EVENT_CODE_RETURN_LINK_KEYS		0x15
#define BT_EVENT_CODE_PIN_CODE_REQUEST		0x16
#define BT_EVENT_CODE_LINK_KEY_REQUEST		0x17
#define BT_EVENT_CODE_LINK_KEY_NOTIFICATION	0x18
#define BT_EVENT_CODE_MAX_SLOTS_CHANGE		0x1B
#define BT_EVENT_NUM_EVENTS					0x20
	u8	ParameterTotalLength;


	public:
	CBTHCIEvent();
	void Process(void*, u16);
	static void Register(u8, void*);
	static TBTLMPEventHandler* Handler[BT_EVENT_NUM_EVENTS];

	friend class CBTHCILayer;	
	friend class CBTLogicalLayer;	
}
PACKED;

class CBTHCIEventInquiryComplete : public CBTHCIEvent
{
	u8	Status;

	void Process(void*, u16);
	public:
	CBTHCIEventInquiryComplete();
	static void Handler(void*, void*, u16);
}
PACKED;

class CBTHCIEventInquiryResult : public CBTHCIEvent
{
	u8	NumResponses;

//	u8	BDAddr[NumResponses][BT_BD_ADDR_SIZE];
//	u8	PageScanRepetitionMode[NumResponses];
//	u8	Reserved[NumResponses][2];
//	u8	ClassOfDevice[NumResponses][BT_CLASS_SIZE];
//	u16	ClockOffset[NumResponses];
	u8	Data[0];
#define INQUIRY_RESP_SIZE			14
#define INQUIRY_RESP_BD_ADDR(p, i)		(&(p)->Data[(i)*BT_BD_ADDR_SIZE])
#define INQUIRY_RESP_PAGE_SCAN_REP_MODE(p, i)	((p)->Data[(p)->NumResponses*BT_BD_ADDR_SIZE + (i)])
#define INQUIRY_RESP_CLASS_OF_DEVICE(p, i)	(&(p)->Data[(p)->NumResponses*(BT_BD_ADDR_SIZE+1+2) \
							   + (i)*BT_CLASS_SIZE])

	void Process(void*, u16);
	public:
	CBTHCIEventInquiryResult();
	static void Handler(void*, void*, u16);
	friend class CBTInquiryResults;
}
PACKED;

class CBTHCIEventConnectionComplete : public CBTHCIEvent
{
	u8	Status;
	u16	ConnectionHandle;
	u8	BDAddr[BT_BD_ADDR_SIZE];
	u8	LinkType;
#define LINK_TYPE_SCO_CONNECTION	0x00
#define LINK_TYPE_ACL_CONNECTION	0x01
	u8	EncryptionMode;
#define ENCRYPTION_DISABLED	    			0x00
#define ENCRYPTION_ONLY_FOR_P2P 			0x01
#define ENCRYPTION_FOR_P2P_AND_BROADCAST	0x02

	void Process(void*, u16);
	public:
	CBTHCIEventConnectionComplete();
	static void Handler(void*, void*, u16);
}
PACKED;

class CBTHCIEventConnectionRequest : public CBTHCIEvent
{
	u8	BDAddr[BT_BD_ADDR_SIZE];
	u8	ClassOfDevice[BT_CLASS_SIZE];
	u8	LinkType;
//#define LINK_TYPE_SCO_CONNECTION	0x00
//#define LINK_TYPE_ACL_CONNECTION	0x01

	void Process(void*, u16);
	public:
	CBTHCIEventConnectionRequest();
	static void Handler(void*, void*, u16);
}
PACKED;

class CBTHCIEventDisconnectionComplete : public CBTHCIEvent
{
	u8	Status;
	u16	ConnectionHandle;
	u8	Reason;
#define REASON_CONNECTION_TIMED_OUT	0x00
#define REASON_CONNECTION_TERMINATED_BY_LOCAL_HOST	0x16
#define REASON_UNSUPPORTED_REMOTE_FEATURE	0x1A

	void Process(void*, u16);
	public:
	CBTHCIEventDisconnectionComplete();
	static void Handler(void*, void*, u16);
}
PACKED;

class CBTHCIEventAuthenticationComplete : public CBTHCIEvent
{
	u8	Status;
	u16	ConnectionHandle;

	void Process(void*, u16);
	public:
	CBTHCIEventAuthenticationComplete();
	static void Handler(void*, void*, u16);
}
PACKED;

class CBTHCIEventFlushOccurred : public CBTHCIEvent
{
	u16	ConnectionHandle;

	void Process(void*, u16);
	public:
	CBTHCIEventFlushOccurred();
	static void Handler(void*, void*, u16);
}
PACKED;

class CBTHCIEventRoleChange : public CBTHCIEvent
{
	u8	Status;
	u8	BDAddr[BT_BD_ADDR_SIZE];
	u8	NewRole;
#define ROLE_MASTER 0x00
#define ROLE_SLAVE  0x01

	void Process(void*, u16);
	public:
	CBTHCIEventRoleChange();
	static void Handler(void*, void*, u16);
}
PACKED;

class CBTHCIEventNumberOfCompletedPackets : public CBTHCIEvent
{
	u8	NumberOfHandles;
	u16	ConnectionHandle[1];
	u16	HCNumOfCompletedPackets[1];

	void Process(void*, u16);
	public:
	CBTHCIEventNumberOfCompletedPackets();
	static void Handler(void*, void*, u16);
}
PACKED;

class CBTHCIEventModeChange : public CBTHCIEvent
{
	u8	Status;
	u16	ConnectionHandle;
	u8	CurrentMode;
#define ACTIVE_MODE 0x00
#define HOLD_MODE   0x01
#define SNIFF_MODE  0x02
#define PARK_MODE   0x03
	u16	Interval;

	void Process(void*, u16);
	public:
	CBTHCIEventModeChange();
	static void Handler(void*, void*, u16);
}
PACKED;

class CBTHCIEventRemoteNameRequestComplete : public CBTHCIEvent
{
	u8	Status;
	u8	BDAddr[BT_BD_ADDR_SIZE];
	u8	RemoteName[BT_NAME_SIZE];

	void Process(void*, u16);
	public:
	CBTHCIEventRemoteNameRequestComplete();
	static void Handler(void*, void*, u16);
	friend class CBTInquiryResults;
}
PACKED;

class CBTHCIEventReadRemoteSupportedFeaturesComplete : public CBTHCIEvent
{
	u8	Status;
	u16	ConnectionHandle;
	u8	LMPFeatures[BT_LMP_FEATURE_SIZE];

	void Process(void*, u16);
	public:
	CBTHCIEventReadRemoteSupportedFeaturesComplete();
	static void Handler(void*, void*, u16);
}
PACKED;

class CBTHCIEventReadRemoteVersionInformationComplete : public CBTHCIEvent
{
	u8	Status;
	u16	ConnectionHandle;
	u8	LMPVersion;
	u16	ManufacturerName;
	u16	LMPSubversion;

	void Process(void*, u16);
	public:
	CBTHCIEventReadRemoteVersionInformationComplete();
	static void Handler(void*, void*, u16);
}
PACKED;

class CBTHCIEventCommandComplete : public CBTHCIEvent
{
	u8	NumHCICommandPackets;
	u16	CommandOpCode;
	u8	Status;				// normally part of ReturnParameter[]
	u8	ReturnParameter[0];

	void Process(void*, u16);
	public:
	CBTHCIEventCommandComplete();
	static void Handler(void*, void*, u16);
}
PACKED;

class CBTHCIEventReadBDAddrComplete : public CBTHCIEventCommandComplete
{
	u8	BDAddr[BT_BD_ADDR_SIZE];

	public:
	CBTHCIEventReadBDAddrComplete();
	friend class CBTHCIEventCommandComplete;
}
PACKED;

class CBTHCIEventReadStoredLinkKeyComplete : public CBTHCIEventCommandComplete
{
	u16	MaxNumKeys;
	u16	NumKeysRead;

	public:
	CBTHCIEventReadStoredLinkKeyComplete();
	friend class CBTHCIEventCommandComplete;
}
PACKED;

class CBTHCIEventWriteStoredLinkKeyComplete : public CBTHCIEventCommandComplete
{
	u8	NumKeysWritten;

	public:
	CBTHCIEventWriteStoredLinkKeyComplete();
	friend class CBTHCIEventCommandComplete;
}
PACKED;

class CBTHCIEventCommandStatus : public CBTHCIEvent
{
	u8	Status;
	u8	NumHCICommandPackets;
	u16	CommandOpCode;

	void Process(void*, u16);
	public:
	CBTHCIEventCommandStatus();
	static void Handler(void*, void*, u16);
}
PACKED;

class CBTHCIEventReturnLinkKeys : public CBTHCIEvent
{
	u8	NumKeys;
	u8	BDAddr[BT_BD_ADDR_SIZE];
	u8	LinkKey[BT_MAX_LINK_KEY_SIZE];

	void Process(void*, u16);
	public:
	CBTHCIEventReturnLinkKeys();
	static void Handler(void*, void*, u16);
}
PACKED;

class CBTHCIEventPINCodeRequest : public CBTHCIEvent
{
	u8	BDAddr[BT_BD_ADDR_SIZE];

	void Process(void*, u16);
	public:
	CBTHCIEventPINCodeRequest();
	static void Handler(void*, void*, u16);
}
PACKED;

class CBTHCIEventLinkKeyRequest : public CBTHCIEvent
{
	u8	BDAddr[BT_BD_ADDR_SIZE];

	void Process(void*, u16);
	public:
	CBTHCIEventLinkKeyRequest();
	static void Handler(void*, void*, u16);
}
PACKED;

class CBTHCIEventLinkKeyNotification : public CBTHCIEvent
{
	u8	BDAddr[BT_BD_ADDR_SIZE];
	u8	LinkKey[BT_MAX_LINK_KEY_SIZE];

	void Process(void*, u16);
	public:
	CBTHCIEventLinkKeyNotification();
	static void Handler(void*, void*, u16);
}
PACKED;

class CBTHCIEventMaxSlotsChange : public CBTHCIEvent
{
	u16	ConnectionHandle;
	u8	LMPMaxSlots;

	void Process(void*, u16);

	public:
	CBTHCIEventMaxSlotsChange();
	static void Handler(void*, void*, u16);
}
PACKED;

#endif
