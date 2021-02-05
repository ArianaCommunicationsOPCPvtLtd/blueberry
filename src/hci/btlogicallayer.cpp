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
** Description:    Raspberry Pi BareMetal Bluetooth Logical Layer Routines
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
#include <bluetooth/btlogicallayer.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/btcommand.h>
#include <bluetooth/btevent.h>
#include <bluetooth/btdevice.h>
#include <bluetooth/btdata.h>
#include <synchronize.h>
#include <logger.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////
//
// LMP Connection
//
////////////////////////////////////////////////////////////////////////////////

CBTConnection::CBTConnection(void)
{
	memset(RemoteName, 0, sizeof(RemoteName));
	memset((u8*)&ClassOfDevice, 0, sizeof(ClassOfDevice));
	memset(BDAddr, 0, sizeof(BDAddr));
	ConnectionState = BTConnectionStateDisconnected;
}	

bool CBTConnection::IsHID(void)
{
	return (ClassOfDevice.MajorDeviceClass == BT_CODE_HID);
}

bool CBTConnection::IsMouse(void)
{
	return (ClassOfDevice.MinorDeviceClass == BT_CODE_MOUSE);
}

bool CBTConnection::IsKeyboard(void)
{
	return (ClassOfDevice.MinorDeviceClass == BT_CODE_KEYBOARD);
}

bool CBTConnection::IsJoystick(void)
{
	return (ClassOfDevice.MinorDeviceClass == BT_CODE_JOYSTICK);
}

bool CBTConnection::IsComputer(void)
{
	return (ClassOfDevice.MajorDeviceClass == BT_CODE_COMPUTER);
}

bool CBTConnection::IsConnected(void)
{
	return (ConnectionState == BTConnectionStateConnected);
}

bool CBTConnection::IsConnecting(void)
{
	return (ConnectionState == BTConnectionStateConnecting);
}

bool CBTConnection::IsAuthenticated(void)
{
	return (ConnectionState == BTConnectionStateAuthenticated);
}

bool CBTConnection::IsDisconnected(void)
{
	return (ConnectionState == BTConnectionStateDisconnected);
}

bool CBTConnection::HasBDAddress (u8* sBDAddr)
{
	return ((memcmp(BDAddr, sBDAddr, BT_BD_ADDR_SIZE)==0) ? true : false);
}

bool CBTConnection::HasConnectionHandle (u16 nConnHandle)
{
	return ((ConnectionHandle==nConnHandle) ? true : false);
}

const u8* CBTConnection::GetBDAddress (void) const
{
	return BDAddr;
}

const u8* CBTConnection::GetLinkKey (void) const
{
	return LinkKey;
}

const u8* CBTConnection::GetPIN (void) const
{
	return PIN;
}

CBTDevice* CBTConnection::GetDevice (void) const
{
	return Device;
}

const u8 CBTConnection::GetPINSize (void) const
{
	return PINSize;
}

void CBTConnection::SetBDAddress (u8* sBDAddr)
{
	memcpy(BDAddr, sBDAddr, BT_BD_ADDR_SIZE);
}

void CBTConnection::SetClassOfDevice (u8* sClassOfDevice)
{
	memcpy((u8 *)&ClassOfDevice, sClassOfDevice, BT_CLASS_SIZE);
}

void CBTConnection::SetRemoteName (u8* sRemoteName)
{
	memcpy(RemoteName, sRemoteName, strlen((char *)sRemoteName));
}

void CBTConnection::SetLinkKey (u8* sLinkKey, bool valid)
{
	memcpy(LinkKey, sLinkKey, BT_MAX_LINK_KEY_SIZE);
	if (valid) LinkKeyValid = true;
}

void CBTConnection::SetLinkType (u8 nLinkType)
{
	LinkType = nLinkType;
}

void CBTConnection::SetStatus (u8 nStatus)
{
	Status = nStatus;
}

void CBTConnection::SetSlots (u8 nSlots)
{
	LMPMaxSlots = nSlots;
}

void CBTConnection::SetVersion (u8 nVersion)
{
	LMPVersion = nVersion;
}

void CBTConnection::SetSubversion (u16 nSubversion)
{
	LMPSubversion = nSubversion;
}

void CBTConnection::SetManufacturer (u16 nManufacturer)
{
	ManufacturerName = nManufacturer;
}

void CBTConnection::SetState (TBTConnectionState eState)
{
	ConnectionState = eState;
}

void CBTConnection::SetDevice (CBTDevice* pDevice)
{
	Device = pDevice;
}

void CBTConnection::SetConnectionHandle (u16 nHandle)
{
	ConnectionHandle = nHandle;
}

void CBTConnection::SetEncryptionMode (u8 nMode)
{
	EncryptionMode = nMode;
}

void CBTConnection::SetRole (u8 nRole)
{
	Role = nRole;
}

////////////////////////////////////////////////////////////////////////////////
//
// LMP Layer
//
////////////////////////////////////////////////////////////////////////////////

CBTLogicalLayer::CBTLogicalLayer (CBTHCILayer *pHCILayer)
:	m_pHCILayer (pHCILayer),
	m_pInquiryResults (0),
	m_pBuffer (0)
{
}

CBTLogicalLayer::~CBTLogicalLayer (void)
{
	assert (m_pInquiryResults == 0);

	free (m_pBuffer);
	m_pBuffer = 0;

	m_pHCILayer = 0;
}

boolean CBTLogicalLayer::Initialize (void)
{
	CBTHCIEventInquiryComplete e1;
	CBTHCIEventInquiryResult e2;
	CBTHCIEventConnectionComplete e3;
	CBTHCIEventConnectionRequest e4;
	CBTHCIEventDisconnectionComplete e5;
	CBTHCIEventAuthenticationComplete e6;
	CBTHCIEventFlushOccurred e61;
	CBTHCIEventRoleChange e62;
	CBTHCIEventNumberOfCompletedPackets e7;
	CBTHCIEventModeChange e71;
	CBTHCIEventRemoteNameRequestComplete e8;
	CBTHCIEventReadRemoteSupportedFeaturesComplete e9;
	CBTHCIEventReadRemoteVersionInformationComplete e10;
	CBTHCIEventLinkKeyRequest e11;
	CBTHCIEventLinkKeyNotification e12;
	CBTHCIEventReturnLinkKeys e13;
	CBTHCIEventPINCodeRequest e14;
	CBTHCIEventMaxSlotsChange e15;
	m_pBuffer = (u8 *)malloc(BT_MAX_DATA_SIZE);
	assert (m_pBuffer != 0);

	return TRUE;
}

CBTInquiryResults *CBTLogicalLayer::Inquiry (unsigned nSeconds)
{
	assert (1 <= nSeconds && nSeconds <= 61);
	assert (m_pHCILayer != 0);

	if (m_bConnecting) return NULL; // return if a new device is connecting

	assert (m_pInquiryResults == 0);
	m_pInquiryResults = new CBTInquiryResults;
	assert (m_pInquiryResults != 0);

	Clear();
	CBTHCIInquiryCommand Cmd(INQUIRY_LENGTH(nSeconds));
	m_pHCILayer->SendCommand (&Cmd, sizeof Cmd);

	Wait ();

	CBTInquiryResults *pResult = m_pInquiryResults;
	m_pInquiryResults = 0;
	if (pResult->GetCount()) {
		for (int i=0; i<pResult->GetCount(); i++) {
			CBTConnection* pConnection;
			pConnection = GetConnection((u8 *)pResult->GetBDAddress(i));
			if (!pConnection) {
				pConnection = new CBTConnection;
				assert(pConnection != 0);
				pConnection->SetBDAddress((u8*)pResult->GetBDAddress(i));
				pConnection->SetClassOfDevice(
					(u8*)pResult->GetClassOfDevice(i));
				pConnection->SetRemoteName((u8*)pResult->GetRemoteName(i));
				pConnection->PageScanRepetitionMode
					= pResult->GetPageScanRepetitionMode(i);
				m_Connections.Append(pConnection);
			}
		}
	}
	
	return pResult;
}

bool CBTLogicalLayer::Connect(CBTConnection* pConnection)
{
	assert(pConnection != 0);
	Clear();
	CBTHCICreateConnectionCommand Cmd(
		pConnection->BDAddr, pConnection->PageScanRepetitionMode);
	m_pConnection = pConnection;
	m_pHCILayer->SendCommand (&Cmd, sizeof Cmd);
	m_pConnection->ConnectionState = BTConnectionStateConnecting;
	Wait();
	m_pConnection = NULL;

	return pConnection->Status;
}
// TBD
bool CBTLogicalLayer::ConnectResponse(
	CBTConnection* pConnection,
	u8 nResponse,
	char *pPIN)
{
	assert(pConnection != 0);
	m_pConnection = pConnection;
	// Setup the PIN for authentication
	pConnection->PINSize = strlen(pPIN);
    memset(pConnection->PIN, 0, sizeof(pConnection->PIN));
	if (!nResponse) {
		pConnection->Role = (pConnection->Role == ROLE_MASTER)
			? ROLE_SLAVE : ROLE_MASTER;
		CBTHCIAcceptConnectionRequestCommand Cmd(
		(u8 *)pConnection->BDAddr, pConnection->Role);
		m_pHCILayer->SendCommand (&Cmd, sizeof Cmd);
		m_pConnection->ConnectionState = BTConnectionStateConnecting;
	} else {
		CBTHCIRejectConnectionRequestCommand Cmd(
		(u8 *)pConnection->BDAddr, BT_ERROR_UNSUPPORTED_REMOTE_FEATURE);
		m_pHCILayer->SendCommand (&Cmd, sizeof Cmd);
		m_pConnection->ConnectionState = BTConnectionStateConnectionFailed;
		m_Connections.Delete(m_pConnection);
	}
//	m_pConnection = NULL; TBD

	return pConnection->Status;
}

bool CBTLogicalLayer::Authenticate(CBTConnection* pConnection, char* pPIN)
{
	assert(pConnection != 0);
	Clear();
	pConnection->PINSize = strlen(pPIN);
    memset(pConnection->PIN, 0, sizeof(pConnection->PIN));
	memcpy(pConnection->PIN, pPIN, strlen(pPIN));
	CBTHCIAuthenticationRequestedCommand Cmd(pConnection->ConnectionHandle);
	m_pConnection = pConnection;
	m_pHCILayer->SendCommand (&Cmd, sizeof Cmd);
	m_pConnection->ConnectionState = BTConnectionStateAuthenticating;
	Wait();
	m_pConnection = NULL;

	return pConnection->Status;
}

bool CBTLogicalLayer::Disconnect(CBTConnection* pConnection, u8 nReason)
{
	assert(pConnection != 0);
	CBTHCIDisconnectCommand Cmd(pConnection->ConnectionHandle, nReason);
	m_pConnection = pConnection;
	m_pHCILayer->SendCommand (&Cmd, sizeof Cmd);
	m_pConnection->ConnectionState = BTConnectionStateDisconnecting;

	return pConnection->Status;
}

bool CBTLogicalLayer::GetInfo(CBTConnection* pConnection)
{
	assert(pConnection != 0);
	Clear();
	CBTHCIReadRemoteVersionInformationCommand Cmd(pConnection->ConnectionHandle);
	m_pConnection = pConnection;
	m_pHCILayer->SendCommand (&Cmd, sizeof Cmd);
	Wait();
	m_pConnection = NULL;

	return pConnection->Status;
}

bool CBTLogicalLayer::GetFeatures(CBTConnection* pConnection)
{
	assert(pConnection != 0);
	Clear();
	CBTHCIReadRemoteSupportedFeaturesCommand Cmd(pConnection->ConnectionHandle);
	m_pConnection = pConnection;
	m_pHCILayer->SendCommand (&Cmd, sizeof Cmd);
	Wait();
	m_pConnection = NULL;

	return pConnection->Status;
}

bool CBTLogicalLayer::SendACLData(
	CBTConnection* pConnection, void* pData, u16 nLength)
{
	assert(pConnection != 0);
	CBTHCIACLData* Cmd = new(nLength) CBTHCIACLData(
		pConnection->ConnectionHandle, (u8*)pData, nLength);
	assert(Cmd != 0);
	m_pHCILayer->SendData (Cmd, sizeof(CBTHCIACLData) + nLength);
	free(Cmd);

	return pConnection->Status;
}

bool CBTLogicalLayer::SendHCICommand(const void* pData, unsigned nLength)
{
	if (pData && nLength) {
		m_pHCILayer->SendCommand (pData, nLength);
		return false;
	}

	return true;
}

void CBTLogicalLayer::Process (void)
{
	assert (m_pHCILayer != 0);
	assert (m_pBuffer != 0);

	unsigned nLength;
	while (m_pHCILayer->ReceiveLinkEvent (m_pBuffer, &nLength))
	{
		assert (nLength >= sizeof (CBTHCIEvent));
		CBTHCIEvent *pHeader = (CBTHCIEvent *) m_pBuffer;
		LOG_DEBUG("LMP event: 0x%02X\r\n", pHeader->EventCode);
		pHeader->Process(this, nLength);
	}
	while (m_pHCILayer->ReceiveData (m_pBuffer, &nLength))
	{
		assert (nLength >= sizeof (CBTHCIACLData));
		CBTHCIACLData *pHeader = (CBTHCIACLData *) m_pBuffer;
		if (m_pL2CAPCallback)
			m_pL2CAPCallback((const void *)pHeader->Data, nLength-4);
	}
}

void CBTLogicalLayer::ListDevices (void)
{
#if 0
	for (int i=0; i<m_Connections.GetCount(); i++) {
    const u8* rName = ((CBTConnection *)m_Connections[i])->RemoteName;
	LOG_DEBUG("Remote Name = %s\r\n", (char *)rName);
    const u8* bda = ((CBTConnection *)m_Connections[i])->BDAddr;
	LOG_DEBUG("BD Address = %02X:%02X:%02X:%02X:%02X:%02X\r\n", 
		(unsigned) bda[5], (unsigned) bda[4], (unsigned) bda[3], 
		(unsigned) bda[2], (unsigned) bda[1], (unsigned) bda[0]);
    const u8* cod = ((CBTConnection *)m_Connections[i])->ClassOfDevice;
	LOG_DEBUG("Class Of Device = 0x%02X%02X%02X\r\n", 
		(unsigned) cod[2], (unsigned) cod[1], (unsigned) cod[0]);
    u8 scan = ((CBTConnection *)m_Connections[i])->PageScanRepetitionMode;
	LOG_DEBUG("Page Scan Rep Mode = 0x%02X\r\n", (unsigned) scan); 
	}
#endif	
	return;
}

CBTConnection* CBTLogicalLayer::GetConnection (u8* sBDAddr)
{
	bool found = false;
	CBTConnection* pConnection = NULL;
	for (int i=0; i<m_Connections.GetCount() && !found; i++) {
		pConnection = (CBTConnection *)m_Connections[i];
		found = pConnection->HasBDAddress(sBDAddr);
	}
	return pConnection;
}

CBTConnection* CBTLogicalLayer::GetConnection (u16 nConnHandle)
{
	bool found = false;
	CBTConnection* pConnection = NULL;
	for (int i=0; i<m_Connections.GetCount() && !found; i++) {
		pConnection = (CBTConnection *)m_Connections[i];
		found = pConnection->HasConnectionHandle(nConnHandle);
	}
	return pConnection;
}

void CBTLogicalLayer::SetConnectingFlag (bool flag)
{
	m_bConnecting = flag;
}

void CBTLogicalLayer::RegisterLayer (CBTL2CAPLayer *pL2CAP)
{
	m_pL2CAPLayer = pL2CAP;
}

void CBTLogicalLayer::RegisterLPCallback (TBTL2CAPCallback *pHandler)
{
	assert (m_pLPCallback == 0);
	m_pLPCallback = pHandler;
	assert (m_pLPCallback != 0);
}

void CBTLogicalLayer::RegisterL2CAPCallback (TBTL2CAPCallback *pHandler)
{
	assert (m_pL2CAPCallback == 0);
	m_pL2CAPCallback = pHandler;
	assert (m_pL2CAPCallback != 0);
}
