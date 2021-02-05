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
** Description:    Raspberry Pi BareMetal Bluetooth HIDP Header
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
#ifndef _bt_hidp_h
#define _bt_hidp_h

#include <macros.h>
#include <types.h>
#include <stdlib.h>
#include <bluetooth/btqueue.h>
#include <bluetooth/btdevice.h>
#include <bluetooth/btl2cap.h>


// Sizes
#define MAX_HID_DEVICE 20

////////////////////////////////////////////////////////////////////////////////
//
// HIDP 
//
////////////////////////////////////////////////////////////////////////////////

typedef enum 
{
	BT_HIDP_REPORT_TYPE_RESERVED = 0x0,
	BT_HIDP_REPORT_TYPE_INPUT = 0x1,
	BT_HIDP_REPORT_TYPE_OUTPUT = 0x2,
	BT_HIDP_REPORT_TYPE_FEATURE = 0x3
} TBTHIDPReportType;

typedef enum 
{
	BT_HIDP_PROTOCOL_MODE_BOOT = 0x0,
	BT_HIDP_PROTOCOL_MODE_REPORT = 0x1
} TBTHIDPProtocolMode;

typedef enum 
{
	BT_HIDP_HANDSHAKE_SUCCESSFUL = 0x0,
	BT_HIDP_HANDSHAKE_NOT_READY = 0x1,
	BT_HIDP_HANDSHAKE_ERR_INVALID_REPORT = 0x2,
	BT_HIDP_HANDSHAKE_ERR_UNSUPPORTED_REQUEST = 0x3,
	BT_HIDP_HANDSHAKE_ERR_INVALID_PARAMETER = 0x4,
	BT_HIDP_HANDSHAKE_ERR_UNKNOWN = 0xE,
	BT_HIDP_HANDSHAKE_ERR_FATAL = 0xF
} TBTHIDPHandshakeParam;

// Messages

class CBTHIDPMessage
{
	protected:
	u8	Parameter : 4;
	u8	MessageType : 4;
#define BT_HIDP_HANDSHAKE		0x0
#define BT_HIDP_HID_CONTROL		0x1
#define BT_HIDP_GET_REPORT		0x4
#define BT_HIDP_SET_REPORT		0x5
#define BT_HIDP_GET_PROTOCOL	0x6
#define BT_HIDP_SET_PROTOCOL	0x7
#define BT_HIDP_DATA			0xA

	friend class CBTHIDPLayer;
	friend class CBTHIDDevice;

	public:
	CBTHIDPMessage();
	CBTHIDPMessage(u8 nMessageType);
	
	void* operator new(size_t T) { return (void *)malloc(T); }
	void operator delete (void *ptr) { free(ptr); }
}
PACKED;

class CBTHIDPMessageHandshake : public CBTHIDPMessage
{
	public:

	CBTHIDPMessageHandshake();
	CBTHIDPMessageHandshake(u8 nResult);
	
	void* operator new(size_t T) { return (void *)malloc(T); }
	void operator delete (void *ptr) { free(ptr); }
}
PACKED;

class CBTHIDPMessageControl : public CBTHIDPMessage
{
#define BT_HIDP_CONTROL_SUSPEND					0x3
#define BT_HIDP_CONTROL_EXIT_SUSPEND			0x4
#define BT_HIDP_CONTROL_VIRTUAL_CABLE_UNPLUG	0x5
	public:

	CBTHIDPMessageControl();
	CBTHIDPMessageControl(u8 nCommand);
	
	void* operator new(size_t T) { return (void *)malloc(T); }
	void operator delete (void *ptr) { free(ptr); }
}
PACKED;

class CBTHIDPMessageGetReport : public CBTHIDPMessage
{
#define BT_HIDP_GET_REPORT_SIZE_EQUAL	0x0
#define BT_HIDP_GET_REPORT_SIZE_BUFFER	0x8
	public:
	u8	ReportID;
	u16	BufferSize;

	CBTHIDPMessageGetReport();
	CBTHIDPMessageGetReport(u8 nSize, u8 nType, u8 nID, u16 nBuffSize);
	
	void* operator new(size_t T) { return (void *)malloc(T); }
	void operator delete (void *ptr) { free(ptr); }
}
PACKED;

class CBTHIDPMessageSetReport : public CBTHIDPMessage
{
	public:
	u8	ReportDataPayload[0];

	CBTHIDPMessageSetReport();
	CBTHIDPMessageSetReport(u8 nReportType, u8* pData);
	
	void* operator new(size_t T) { return (void *)malloc(T); }
	void* operator new(size_t T, size_t L) { return (void *)malloc(T+L); }
	void operator delete (void *ptr) { free(ptr); }
}
PACKED;

class CBTHIDPMessageGetProtocol : public CBTHIDPMessage
{
	public:
	u8	GetProtocolDataPayload;

	CBTHIDPMessageGetProtocol();
	CBTHIDPMessageGetProtocol(u8 nMode);
	
	void* operator new(size_t T) { return (void *)malloc(T); }
	void operator delete (void *ptr) { free(ptr); }
}
PACKED;

class CBTHIDPMessageSetProtocol : public CBTHIDPMessage
{
	public:

	CBTHIDPMessageSetProtocol();
	CBTHIDPMessageSetProtocol(u8 nProtocolMode);
	
	void* operator new(size_t T) { return (void *)malloc(T); }
	void operator delete (void *ptr) { free(ptr); }
}
PACKED;

class CBTHIDPMessageData : public CBTHIDPMessage
{
	public:
	u8	ReportDataPayload[0];

	CBTHIDPMessageData();
	CBTHIDPMessageData(u8* pData, u8 nLength);
	
	void* operator new(size_t T) { return (void *)malloc(T); }
	void* operator new(size_t T, size_t L) { return (void *)malloc(T+L); }
	void operator delete (void *ptr) { free(ptr); }
}
PACKED;

// Class HIDP Layer

class CBTHIDPLayer : public CBTLayer
{
public:
	CBTHIDPLayer(CBTL2CAPLayer *pL2CAPLayer);
	~CBTHIDPLayer(void);

	void RegisterCallback (u16, CBTDevice *);
	void DeregisterCallback (u16);

	u16 Connect (u16, u8*, u16);
	u16 Disconnect (u16, u16);

	u16 Send (u16, const void *pBuffer, u16);
	u16 Receive (u16, const void *pBuffer, u16 *);

private:
	void Callback(const void *, unsigned);
	void DataHandler(u16, u8*, u16);

	CBTDevice* m_DeviceTable[MAX_HID_DEVICE];
	CBTL2CAPLayer *m_pL2CAPLayer;

	TBTL2CAPFlowSpec m_sFlowMouse;
	TBTL2CAPFlowSpec m_sFlowKeyboard;
	TBTL2CAPFlowSpec m_sFlowJoystick;

	u8 *m_pBuffer;

	unsigned m_nRxPackets;		// packets allowed to be received

	static TBTL2CAPCallback EventStub;
	static TBTL2CAPDataCallback DataStub;
	static CBTHIDPLayer *s_pThis;

	friend class CBTHIDDevice;
};

// Class HID Device

typedef enum 
{
	BT_HID_DEVICE_IDLE,
	BT_HID_DEVICE_CONNECTED,
	BT_HID_DEVICE_CONNECTING,
	BT_HID_DEVICE_W2_GET_REPORT,
	BT_HID_DEVICE_W2_SET_REPORT,
	BT_HID_DEVICE_W2_GET_PROTOCOL,
	BT_HID_DEVICE_W2_SET_PROTOCOL,
	BT_HID_DEVICE_W2_ANSWER_SET_PROTOCOL,
	BT_HID_DEVICE_W2_SEND_UNSUPPORTED_REQUEST
} TBTHIDState;

class CBTHIDDevice : public CBTDevice
{
protected:
	CBTHIDPLayer *m_pHIDPLayer;

public:
	CBTHIDDevice(CBTHIDPLayer *pHIDPLayer, CBTConnection *pConnection);
	~CBTHIDDevice(void);

	// Connect the device
	u16 Connect (void);

	// Disconnect the device
	bool DisconnectInterruptChannel (void);
	bool DisconnectControlChannel (void);

	// Status of device
	bool IsInBootProtocolMode (void);

	// Send HID message on interrupt channel
	bool SendInterruptMessage (const u8*, u16);

	// Send HID message on control channel
	bool SendControlMessage (const u8*, u16);

	// Receive event from event queue
	bool ReceiveEvent (void *pBuffer, unsigned *pResultLength);

	// Post event to event queue
	void PostEvent (const void *pBuffer, unsigned nLength);

	// Set CID
	void SetControlCID (u16 nCID);
	void SetInterruptCID (u16 nCID);

	// Packet parser
	virtual void Parser(u8*, u16);

private:
	u16						m_nControlCID;
	u16						m_nInterruptCID;
	bool					m_bIncoming;
	bool					m_bConnected;
	TBTHIDPReportType		m_tReportType;
	u16						m_nReportId;
	u16						m_nExtectedReportSize;
	u16						m_nReportSize;
	bool					m_bUserRequestCanSendNow;
	TBTHIDPHandshakeParam	m_tReportStatus;
	TBTHIDPProtocolMode		m_tProtocolMode;
	CBTQueue 				m_EventQueue;
};

#endif
