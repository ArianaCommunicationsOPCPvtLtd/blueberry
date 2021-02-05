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
** Description:    Raspberry Pi BareMetal Bluetooth SDP Header
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
#ifndef _bt_sdp_h
#define _bt_sdp_h

#include <macros.h>
#include <types.h>
#include <stdlib.h>
#include <bluetooth/btqueue.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/btl2cap.h>


// Sizes
#define BT_SDP_UUID_SIZE	128

////////////////////////////////////////////////////////////////////////////////
//
// SDP 
//
////////////////////////////////////////////////////////////////////////////////

typedef enum 
{
	BT_SDP_ATTRIBUTE_SERVICE_RECORD_HANDLE = 0x0000,
	BT_SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST = 0x0001,
	BT_SDP_ATTRIBUTE_SERVICE_ID = 0x0003,
	BT_SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR_LIST = 0x0004,
	BT_SDP_ATTRIBUTE_BROWSE_GROUP_LIST = 0x0005,
	BT_SDP_ATTRIBUTE_LANGUAGE_BASE_ATTRIBUTE_ID_LIST = 0x0006,
	BT_SDP_ATTRIBUTE_SERVICE_INFO_TIME_TO_LIVE = 0x0007,
	BT_SDP_ATTRIBUTE_SERVICE_AVAILABILITY = 0x0008,
	BT_SDP_ATTRIBUTE_BLUETOOTH_PROFILE_DESCRIPTOR_LIST = 0x0009,
	BT_SDP_ATTRIBUTE_DOCUMENTATION_URL = 0x000A,
	BT_SDP_ATTRIBUTE_CLIENT_EXECUTABLE_URL = 0x000B,
	BT_SDP_ATTRIBUTE_ICON_URL = 0x000C,
	BT_SDP_ATTRIBUTE_VERSION_NUMBER_LIST = 0x0200,
	BT_SDP_ATTRIBUTE_SERVER_DATABASE_STATE = 0x0201,
	BT_SDP_ATTRIBUTE_GROUP_ID = 0x0200
} TBTSDPAttributeID;

typedef enum 
{
	BT_SDP_ATTRIBUTE_ID_OFFSET_SERVICE_NAME = 0x0000,
	BT_SDP_ATTRIBUTE_ID_OFFSET_SERVICE_DESCRIPTION = 0x0001,
	BT_SDP_ATTRIBUTE_ID_OFFSET_PROVIDER_NAME = 0x0002
} TBTSDPAttributeIDOffset;

typedef uint8 TBTSDPUUID[128];
typedef struct {
	uint8 N;
	uint8 Data[0];
} TBTSDPContinuationState;

// PDUs

class CBTSDPPDUHeader
{
	protected:
	u8	PDU_ID;
#define BT_SDP_PDU_ERROR_RESPONSE						0x01
#define BT_SDP_PDU_SERVICE_SEARCH_REQUEST				0x02
#define BT_SDP_PDU_SERVICE_SEARCH_RESPONSE				0x03
#define BT_SDP_PDU_SERVICE_ATTRIBUTE_REQUEST			0x04
#define BT_SDP_PDU_SERVICE_ATTRIBUTE_RESPONSE			0x05
#define BT_SDP_PDU_SERVICE_SEARCH_ATTRIBUTE_REQUEST		0x06
#define BT_SDP_PDU_SERVICE_SEARCH_ATTRIBUTE_RESPONSE	0x07
#define BT_SDP_PDU_MAX									0x08
	u16	TransactionID;
	u16	ParameterLength;
	u8	Parameter[0];

	public:
	CBTSDPPDUHeader();
	CBTSDPPDUHeader(u8 nPDUID);
	CBTSDPPDUHeader(u8 nPDUID, u16 nTransactionID);
	
	void Process(void*, u16);
	static void Register(u8, void*);
	static TBTL2CAPCmdHandler* Handler[BT_SDP_PDU_MAX];

	void* operator new(size_t T) { return (void *)malloc(T); }
	void operator delete (void *ptr) { free(ptr); }
}
PACKED;

class CBTSDPPDUErrorResponse : public CBTSDPPDUHeader
{
	u16	ErrorCode;
#define BT_SDP_ERROR_CODE_UNSUPPORTED_SDP_VERSION		0x0001
#define BT_SDP_ERROR_CODE_INVALID_SERVICE_RECORD_HANDLE	0x0002
#define BT_SDP_ERROR_CODE_INVALID_REQUEST_SYNTAX		0x0003
#define BT_SDP_ERROR_CODE_INVALID_PDU_SIZE				0x0004
#define BT_SDP_ERROR_CODE_INVALID_CONTINUATION_STATE	0x0005
#define BT_SDP_ERROR_CODE_INSUFFICIENT_RESOURCES		0x0006
	u8	ErrorInfo[0];

	public:
	CBTSDPPDUErrorResponse();
	CBTSDPPDUErrorResponse(u16 nIdentifier, u16 nErrorCode);

	static void Handler(void*, void*, u16);
	void Process(void*, u16);
}
PACKED;

class CBTSDPPDUServiceSearchRequest : public CBTSDPPDUHeader
{
	TBTSDPUUID	ServiceSearchPattern[12];
	u16	MaximumServiceRecordCount;
	TBTSDPContinuationState	ContinuationState;

	public:
	CBTSDPPDUServiceSearchRequest();
	CBTSDPPDUServiceSearchRequest(u16 nIdentifier);

	static void Handler(void*, void*, u16);
	void Process(void*, u16);
}
PACKED;

class CBTSDPPDUServiceSearchResponse : public CBTSDPPDUHeader
{
	u16	TotalServiceRecordCount;
	u16	CurrentServiceRecordCount;
	u32	ServiceRecordHandleList[0]; //TBD
	TBTSDPContinuationState	ContinuationState;

	public:
	CBTSDPPDUServiceSearchResponse();
	CBTSDPPDUServiceSearchResponse(u16 nIdentifier);

	static void Handler(void*, void*, u16);
	void Process(void*, u16);
}
PACKED;

class CBTSDPPDUServiceAttributeRequest : public CBTSDPPDUHeader
{
	u32	ServiceSearchHandle;
	u16	MaximumAttributeByteCount;
	u32	AttributeIDList[0]; //TBD
	TBTSDPContinuationState	ContinuationState;

	public:
	CBTSDPPDUServiceAttributeRequest();
	CBTSDPPDUServiceAttributeRequest(u16 nIdentifier);

	static void Handler(void*, void*, u16);
	void Process(void*, u16);
	
	void* operator new(size_t T, size_t L) { return (void *)malloc(T+L); }
}
PACKED;

class CBTSDPPDUServiceAttributeResponse : public CBTSDPPDUHeader
{
	u16	AttributeListByteCount;
	u32	AttributeIDList[0]; //TBD
	TBTSDPContinuationState	ContinuationState;

	public:
	CBTSDPPDUServiceAttributeResponse();
	CBTSDPPDUServiceAttributeResponse(u16 nIdentifier);

	static void Handler(void*, void*, u16);
	void Process(void*, u16);
}
PACKED;

class CBTSDPPDUServiceSearchAttributeRequest : public CBTSDPPDUHeader
{
	TBTSDPUUID	ServiceSearchPattern[12];
	u16	MaximumAttributeByteCount;
	u32	AttributeIDList[0]; //TBD
	TBTSDPContinuationState	ContinuationState;

	public:
	CBTSDPPDUServiceSearchAttributeRequest();
	CBTSDPPDUServiceSearchAttributeRequest(u16 nIdentifier);

	static void Handler(void*, void*, u16);
	void Process(void*, u16);
}
PACKED;

class CBTSDPPDUServiceSearchAttributeResponse : public CBTSDPPDUHeader
{
	u16	AttributeListByteCount;
	u32	AttributeLists[0]; //TBD
	TBTSDPContinuationState	ContinuationState;

	public:
	CBTSDPPDUServiceSearchAttributeResponse();
	CBTSDPPDUServiceSearchAttributeResponse(u16 nIdentifier);

	static void Handler(void*, void*, u16);
	void Process(void*, u16);
	
	void* operator new(size_t T, size_t L) { return (void *)malloc(T+L); }
}
PACKED;

// Class

class CBTSDPLayer : public CBTLayer
{
public:
	CBTSDPLayer(CBTL2CAPLayer *pL2CAPLayer);
	~CBTSDPLayer(void);

private:
	static void Callback (const void *pBuffer, unsigned nLength);
	void DataHandler(u16, u8*, u16);

	CBTL2CAPLayer *m_pL2CAPLayer;

	CBTQueue m_RxPacketQueue;

	u8 *m_pBuffer;

	unsigned m_nRxPackets;		// packets allowed to be received

	static TBTL2CAPCallback EventStub;
	static TBTL2CAPDataCallback DataStub;
	static CBTSDPLayer *s_pThis;
};


#endif
