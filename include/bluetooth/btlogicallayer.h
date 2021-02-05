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
** Description:    Raspberry Pi BareMetal Bluetooth LMP Layer Header
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
#ifndef _bt_logicallayer_h
#define _bt_logicallayer_h

#include <bluetooth/bluetooth.h>
#include <bluetooth/bthcilayer.h>
#include <bluetooth/btinquiryresults.h>
#include <bluetooth/ptrarray.h>
#include <bluetooth/btlayer.h>
#include <types.h>

// LMP Connection State

typedef enum {
	BTConnectionStateDisconnecting = 1,
	BTConnectionStateDisconnectionFailed = 2,
	BTConnectionStateDisconnected = 3,
	BTConnectionStateAuthenticating = 4,
	BTConnectionStateAuthenticationFailed = 5,
	BTConnectionStateAuthenticated = 6,
	BTConnectionStateConnecting = 7,
	BTConnectionStateConnectionFailed = 8,
	BTConnectionStateConnected = 9
} TBTConnectionState;

// LMP Connection
class CBTDevice;
class CBTConnection
{
	u8		RemoteName[BT_NAME_SIZE];
	TBTCOD	ClassOfDevice;
	u16		ConnectionHandle;
	u8 		BDAddr[BT_BD_ADDR_SIZE];
	u16		ManufacturerName;
	u8		LinkType;
	u8		EncryptionMode;
	u8		PageScanRepetitionMode;
	u8		PINSize;
	u8		PIN[BT_MAX_PIN_CODE_SIZE];
	u8		LinkKeyValid;
	u8		LinkKey[BT_MAX_LINK_KEY_SIZE];
	u8		LMPMaxSlots;
	u8		LMPVersion;
	u16		LMPSubversion;
	u8		LMPFeatures[BT_LMP_FEATURE_SIZE];
	u8		Status;
	u8		Role;
	TBTMode	Mode;
	u16		Interval;
	volatile TBTConnectionState	ConnectionState;
	CBTDevice* Device;

	friend class CBTLogicalLayer;
	friend class CBTSubSystem;
	friend class CBTDevice;

public:

	CBTConnection (void);

	// Type of device
	bool IsHID (void);
	bool IsMouse (void);
	bool IsKeyboard (void);
	bool IsJoystick (void);
	bool IsComputer (void);

	// Status of connection
	bool IsConnected (void);
	bool IsConnecting (void);
	bool IsAuthenticated (void);
	bool IsDisconnected (void);
	bool HasBDAddress (u8*);
	bool HasConnectionHandle (u16);

	// Get params
	const u8 *GetBDAddress (void) const;
	const u8 *GetLinkKey (void) const;
	const u8 *GetPIN (void) const;
	CBTDevice *GetDevice (void) const;
	const u8 GetPINSize (void) const;
	inline const TBTMode GetMode (void) const {return Mode;}

	// Set params
	void SetBDAddress (u8*);
	void SetClassOfDevice (u8*);
	void SetRemoteName (u8*);
	void SetLinkKey (u8*, bool=false);
	void SetLinkType (u8);
	void SetStatus (u8);
	void SetSlots (u8);
	void SetVersion (u8);
	void SetSubversion (u16);
	void SetManufacturer (u16);
	void SetState (TBTConnectionState);
	void SetDevice (CBTDevice *);
	void SetConnectionHandle (u16);
	void SetEncryptionMode (u8);
	void SetRole (u8);
	inline void SetMode (TBTMode eMode, u16 nInt) {Mode = eMode; Interval=nInt;}

	// Disconnect
	bool Disconnect (u8);

	void* operator new(size_t T) { return (void *)malloc(T); }
	void operator delete (void *ptr) { free(ptr); }
};

// LP Events

class CBTLPEvent
{
	protected:
#define BT_EVENT_LP_CONNECT_CFM	 		0x01
#define BT_EVENT_LP_CONNECT_CFM_NEG		0x02
#define BT_EVENT_LP_CONNECT_IND			0x03
#define BT_EVENT_LP_DISCONNECT_IND		0x04
#define BT_EVENT_LP_QOS_CFM				0x05
#define BT_EVENT_LP_QOS_CFM_NEG			0x06
#define BT_EVENT_LP_QOS_VIOLATION_IND	0x07
	u16	Event;
	friend class CBTL2CAPLayer;
	friend class CBTLogicalLayer;
}
PACKED;

class CBTLPConnectCfm : public CBTLPEvent
{
	u8	BDAddr[BT_BD_ADDR_SIZE];

	friend class CBTL2CAPLayer;
	friend class CBTHCIEventConnectionComplete;
}
PACKED;

class CBTLPConnectCfmNeg : public CBTLPEvent
{
	u8	BDAddr[BT_BD_ADDR_SIZE];

	friend class CBTL2CAPLayer;
	friend class CBTHCIEventConnectionComplete;
}
PACKED;

class CBTLPConnectInd : public CBTLPEvent
{
	u8	BDAddr[BT_BD_ADDR_SIZE];

	friend class CBTL2CAPLayer;
	friend class CBTHCIEventConnectionRequest;
}
PACKED;

class CBTLPDisconnectInd : public CBTLPEvent
{
	u16	Handle;

	friend class CBTL2CAPLayer;
	friend class CBTHCIEventDisconnectionComplete;
}
PACKED;

// LMP Layer

class CBTL2CAPLayer;

class CBTLogicalLayer : public CBTLayer {
public:
	CBTLogicalLayer (CBTHCILayer *pHCILayer);
	~CBTLogicalLayer (void);

	boolean Initialize (void);

	// returns 0 on failure, result must be deleted by caller otherwise
	CBTInquiryResults *Inquiry (unsigned nSeconds);		// 1 <= nSeconds <= 61

	bool Connect (CBTConnection*);
	bool ConnectResponse (CBTConnection*, u8, char*);
	bool Authenticate (CBTConnection*, char*);
	bool Disconnect (CBTConnection*, u8);
	bool GetInfo (CBTConnection*);
	bool GetFeatures (CBTConnection*);

	// inline functions used by event classes
	inline CBTInquiryResults*& GetInquiryResults (void) {
		return m_pInquiryResults;}
	inline unsigned& GetNameRequestsPending (void) {
		return m_nNameRequestsPending;}
	inline CPtrArray& GetConnections (void) {
		return m_Connections;}
	inline CBTConnection*& GetConnectionPtr (void) {
		return m_pConnection;}
	inline CBTDeviceManager* GetDeviceManager (void) {
		return m_pHCILayer->GetDeviceManager();}
	inline void SetHCIDataPackets (unsigned nDataPackets) {
		m_pHCILayer->SetDataPackets(nDataPackets);
		return;}
	inline void SetConnectionPtr (CBTConnection* pConnection) {
		m_pConnection = pConnection;}

	// send functions
	bool SendACLData (CBTConnection*, void*, u16);
	bool SendHCICommand (const void*, unsigned);

	void Process (void);

	void ListDevices (void);
	CBTConnection* GetConnection (u8*);
	CBTConnection* GetConnection (u16);
	void SetConnectingFlag (bool);
	void RegisterLayer (CBTL2CAPLayer *pL2CAPLayer);
	void RegisterLPCallback (TBTL2CAPCallback *pHandler);
	void RegisterL2CAPCallback (TBTL2CAPCallback *pHandler);

	TBTL2CAPCallback *m_pLPCallback;
	TBTL2CAPCallback *m_pL2CAPCallback;

private:
	CBTHCILayer *m_pHCILayer;
	CBTL2CAPLayer *m_pL2CAPLayer;

	CBTInquiryResults *m_pInquiryResults;

	CPtrArray m_Connections;
	CBTConnection *m_pConnection;

	bool m_bConnecting;
	unsigned m_nNameRequestsPending;

	u8 *m_pBuffer;
};

#endif
