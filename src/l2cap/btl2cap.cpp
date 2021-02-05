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
** Description:    Raspberry Pi BareMetal Bluetooth L2CAP Layer Routines
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
#include <bluetooth/btl2cap.h>
#include <bluetooth/btlogicallayer.h>
#include <bluetooth/btdevice.h>
#include <bluetooth/btsubsystem.h>
#include <synchronize.h>
#include <logger.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static const char FromL2CAPLayer[] = "btl2cap";
////////////////////////////////////////////////////////////////////////////////
//
// L2CAP Channel
//
////////////////////////////////////////////////////////////////////////////////
CBTL2CAPChannel::CBTL2CAPChannel(u16 nPSM, CBTConnection *pConnection)
{
	CID = CBTL2CAPChannel::GetCID();
	PSM = nPSM;
	Connection = pConnection;
	State = BT_L2CAP_CLOSED;
	RTX = BT_L2CAP_DEFAULT_RTX;
	ERTX = BT_L2CAP_DEFAULT_ERTX;
}

CBTL2CAPChannel::CBTL2CAPChannel(u16 nPSM, CBTConnection *pConnection, u16 nCID)
{
	CID = 0;
	PSM = nPSM;
	RemoteCID = nCID;
	Connection = pConnection;
	State = BT_L2CAP_CLOSED;
	RTX = BT_L2CAP_DEFAULT_RTX;
	ERTX = BT_L2CAP_DEFAULT_ERTX;
}

u16 CBTL2CAPChannel::GetCID(void)
{
	static u16 cid = BT_CID_DYNAMICALLY_ALLOCATED;
	u16 nCID = cid;
	cid += 2;
	return nCID;
}

////////////////////////////////////////////////////////////////////////////////
//
// L2CAP Packets
//
////////////////////////////////////////////////////////////////////////////////
CBTL2CAPPacket::CBTL2CAPPacket(u16 nLength, u16 nChannelID)
{
	Length = nLength;
	ChannelID = nChannelID;
}

u16 CBTL2CAPPacket::GetCID(void)
{
	return ChannelID;
}

CBTL2CAPConnectionOrientedDataPacket::CBTL2CAPConnectionOrientedDataPacket(
	u16 nLength,
	u16 nCID,
	u8* pInformation)
:	CBTL2CAPPacket(nLength, nCID)
{
	memcpy(Information, pInformation, nLength);
}

CBTL2CAPConnectionLessDataPacket::CBTL2CAPConnectionLessDataPacket(
	u16 nPSM,
	u8* pInformation,
	u16 nLength)
:	CBTL2CAPPacket(nLength+2, BT_CID_CONNECTIONLESS_DATA_CHANNEL)
{
	PSM = nPSM;
	memcpy(Information, pInformation, nLength);
}

CBTL2CAPSignallingPacket::CBTL2CAPSignallingPacket(u8* pCommand, u16 nLength)
:	CBTL2CAPPacket(nLength, BT_CID_SIGNALLING_CHANNEL)
{
	memcpy(Command, pCommand, nLength);
}

CBTL2CAPSignallingCommand* CBTL2CAPSignallingPacket::GetCommand(u8 nIndex)
{
	CBTL2CAPSignallingCommand *pSignallingCommand = NULL;
	u8 *pCommand = Command;
	u16 nLength = Length;

	while (nIndex && nLength) {
		pSignallingCommand = (CBTL2CAPSignallingCommand *)pCommand;
		u8 nSize = sizeof(CBTL2CAPSignallingCommand)+pSignallingCommand->Length;
		nLength -= nSize;
		if (nLength) {
			pCommand += nSize;
			nIndex--;
		}
	}
	if (nLength && !nIndex)
		pSignallingCommand = (CBTL2CAPSignallingCommand *)pCommand;
	else
		pSignallingCommand = NULL;

	return pSignallingCommand;
}

////////////////////////////////////////////////////////////////////////////////
//
// L2CAP Configuration Option Friend Functions
//
////////////////////////////////////////////////////////////////////////////////

u8* InsertMTU(u8* pOption, u16 nMTU)
{
	CBTL2CAPMTU *pMTU = (CBTL2CAPMTU *)pOption;

	if (pMTU) {
		pMTU->Type = BT_L2CAP_OPTION_MTU;
		pMTU->Option = 0;
		pMTU->Length = BT_L2CAP_OPTION_MTU_LEN;
		pMTU->MTU = nMTU;
		pOption += sizeof(CBTL2CAPMTU);
	}

	return pOption;
}

u8* InsertFlow(u8* pOption, TBTL2CAPFlowSpec* pFlow)
{
	CBTL2CAPQoS *pQoS = (CBTL2CAPQoS *)pOption;

	if (pQoS) {
		pQoS->Type = BT_L2CAP_OPTION_QOS;
		pQoS->Option = 0;
		pQoS->Length = BT_L2CAP_OPTION_QOS_LEN;
		pQoS->Flow = *pFlow;
		pOption += sizeof(CBTL2CAPQoS);
	}

	return pOption;
}

u8* InsertFlushTO(u8* pOption, u16 nFlushTO)
{
	CBTL2CAPFlushTimeout *pFlushTO = (CBTL2CAPFlushTimeout *)pOption;

	if (pFlushTO) {
		pFlushTO->Type = BT_L2CAP_OPTION_FLUSH_TIMEOUT;
		pFlushTO->Option = 0;
		pFlushTO->Length = BT_L2CAP_OPTION_FLUSH_TIMEOUT_LEN;
		pFlushTO->FlushTimeout = nFlushTO;
		pOption += sizeof(CBTL2CAPFlushTimeout);
	}

	return pOption;
}

////////////////////////////////////////////////////////////////////////////////
//
// L2CAP Layer
//
////////////////////////////////////////////////////////////////////////////////

CBTL2CAPLayer *CBTL2CAPLayer::s_pThis = 0;

CBTL2CAPLayer::CBTL2CAPLayer (
	CBTLogicalLayer *pLogicalLayer, CBTSubSystem *pSubSystem)
:	m_pLogicalLayer (pLogicalLayer),
	m_pSubSystem (pSubSystem)
{
	assert (s_pThis == 0);
	s_pThis = this;
	m_pEventBuffer = (u8 *)malloc(BT_L2CAP_MIN_SIG_MTU_LEN);
	assert (m_pEventBuffer != 0);

	// Initialize connection pointer
	m_pIncomingConnection = NULL;	// Keeps track of any incoming Connection
	m_eConnectRsp.Identifier = 0;

	// Assign the L2CAP Callbacks to NULL
	for (int i=0; i<BT_L2CAP_MAX_PSM_SLOT; i++)
		m_pL2CAPSignallingCallback[i] = NULL;
	for (int i=0; i<BT_L2CAP_MAX_PSM_SLOT; i++)
		m_pPSMSlot[i] = NULL;

	// Register the Logical Layer Callbacks
	pLogicalLayer->RegisterLayer(this);
	pLogicalLayer->RegisterLPCallback(LPEventStub);
	pLogicalLayer->RegisterL2CAPCallback(L2CAPEventStub);

	// Register the events
	CBTL2CAPSignallingCommand cmd1;
	CBTL2CAPSignallingCommandReject cmd2;
	CBTL2CAPConnectionRequest cmd3;
	CBTL2CAPConnectionResponse cmd4;
	CBTL2CAPConfigurationRequest cmd5;
	CBTL2CAPConfigurationResponse cmd6;
	CBTL2CAPDisconnectionRequest cmd7;
	CBTL2CAPDisconnectionResponse cmd8;
}

CBTL2CAPLayer::~CBTL2CAPLayer (void)
{
	s_pThis = 0;
}

u8 CBTL2CAPLayer::GetID(void)
{
	static u8 id = 1;
	return id++;
}

void CBTL2CAPLayer::RegisterCallback (u16 nPSM, TBTL2CAPCallback *pCallback)
{
	m_pL2CAPSignallingCallback[nPSM] = pCallback;
}

void CBTL2CAPLayer::RegisterDataCallback (
	u16 nPSM, TBTL2CAPDataCallback *pCallback)
{
	m_pPSMSlot[nPSM] = pCallback;
}

void CBTL2CAPLayer::DeregisterDataCallback (u16 nPSM)
{
	m_pL2CAPSignallingCallback[nPSM] = NULL;
	m_pPSMSlot[nPSM] = NULL;
}

u16 CBTL2CAPLayer::Connect (
	u16 nPSM,
	TBDAddr sBDAddr,
	u16 *pLCID,
	u16 *pStatus)
{
	bool connected = false,
		 connecting = false,
		 connect = false,
		 found = false;
	CBTL2CAPChannel *pChannel = NULL;
	CBTConnection *pConnection = NULL;
	u16 nResult = BT_L2CAP_RESULT_PSM_NOT_SUPPORTED;
	CPtrArray& pConnections = m_pLogicalLayer->GetConnections();

	LOG_DEBUG("L2CAP: CONNECT\r\n");
	// Find the device descriptor to be connected
	for (int i=0; i<pConnections.GetCount(); i++) {
    	const u8* bda = ((CBTConnection *)pConnections[i])->GetBDAddress();
		if (!memcmp(sBDAddr, bda, BT_BD_ADDR_SIZE)) {
    		pConnection = (CBTConnection *)pConnections[i];
			if (pConnection) {
    			connected = pConnection->IsConnected()
						|| pConnection->IsAuthenticated();
    			connecting = pConnection->GetDevice()->IsConnecting();
			}
			break;
		}
	}
	if (connecting) return nResult;

	// If a device descriptor exists
	if (pConnection) {

		// Search for an existing channel
		for (int i=0; i<m_Channels.GetCount(); i++) {
			pChannel = (CBTL2CAPChannel *)m_Channels[i];
			if (pChannel->PSM == nPSM && pChannel->Connection == pConnection) {
				found = true;
				break;
			}
		}

		// If no existing channel is found ... create one
		if (!found) {
			pChannel = new CBTL2CAPChannel(nPSM, pConnection);
			assert(pChannel != 0);
		}

		// Check if channel is already open
		if (pChannel->IsOpen()) {
			*pLCID = pChannel->CID;
			*pStatus = BT_L2CAP_STATUS_NO_FURTHER_INFORMATION;
			nResult = BT_L2CAP_RESULT_CONNECTION_SUCCESSFUL;

		} else if (!connected) {
			// Connect the device if not connected
			pChannel->Initiator = true;
			pChannel->State = BT_L2CAP_CLOSED;
			pConnection->GetDevice()->SetState(BT_DEVICE_CONNECTING);
			bool status = m_pLogicalLayer->Connect(pConnection);
			if (!status)
				status = m_pLogicalLayer->Authenticate(
					pConnection, (char *)BT_DEFAULT_PIN);
			if (status) {		// connection failed
				// Close the created channel
				pConnection->GetDevice()->SetState(BT_DEVICE_IDLE);
				if (!found && pChannel) delete pChannel;
			} else connect = true;
		} else connect = true;
	}

	// Connect the channel
	if (connect) {
		LOG_DEBUG("L2CAP: Channel PSM = %d\r\n", (int)nPSM);
		pConnection->GetDevice()->SetState(BT_DEVICE_CONNECTING);
		m_Channels.Append(pChannel);
		Clear();
		pChannel->State = BT_L2CAP_W4_L2CAP_CONNECT_RSP;
		u8 ID = GetID();
		CBTL2CAPConnectionRequest cmd(ID, nPSM, pChannel->CID);
		CBTL2CAPSignallingPacket pkt((u8*)&cmd, (sizeof cmd));
		m_eConnectRsp.Identifier = ID;
		m_eConnectRsp.SCID = pLCID;
		m_eConnectRsp.DCID = &pChannel->RemoteCID;
		m_eConnectRsp.Result = &nResult;
		m_eConnectRsp.Status = pStatus;
		m_eConnectRsp.State = &pChannel->State;
		m_pLogicalLayer->SendACLData(pConnection, (void *)&pkt, (sizeof cmd)+4);
		Wait();
		m_eConnectRsp.Identifier = 0;
	}

	return nResult;
}

u16 CBTL2CAPLayer::ConnectResponse (
	TBDAddr sBDAddr,
	u8 nIdentifier,
	u16 nLCID,
	u16 nResponse,
	u16 nStatus)
{
	bool found = false;
	CBTL2CAPChannel *pChannel = NULL;
	CBTConnection *pConnection = NULL;
	u16 nResult = BT_L2CAP_RESULT_FAILURE_TO_MATCH;

	LOG_DEBUG("L2CAP: CONNECT RESPONSE\r\n");
	// Find the device descriptor to be connected
	pConnection = m_pLogicalLayer->GetConnection(sBDAddr);

	// If a connection descriptor exists
	if (pConnection) {
		// Search for an existing channel
		for (int i=0; i<m_Channels.GetCount(); i++) {
			pChannel = (CBTL2CAPChannel *)m_Channels[i];
			if (pChannel->State == BT_L2CAP_W4_L2CA_CONNECT_RSP) {
				found = true;
				break;
			}
		}

		// Check if channel is already open
		if (found) {
			pChannel->Initiator = false;
			pChannel->State = BT_L2CAP_CONFIG;
			pChannel->CID = nLCID;
			pChannel->Connection = pConnection;
			CBTL2CAPConnectionResponse cmd(
				nIdentifier, nLCID, pChannel->RemoteCID, nResponse, nStatus);
			CBTL2CAPSignallingPacket pkt((u8*)&cmd, (sizeof cmd));
			m_pLogicalLayer->SendACLData(
				pConnection,(void*)&pkt,(sizeof cmd)+4);
			nResult = BT_L2CAP_RESULT_RESPONSE_SUCCESSFULLY_SENT;
		}
	}

	return nResult;
}

u16 CBTL2CAPLayer::Configure (
	u16 nCID,
	u16 nInMTU,
	TBTL2CAPFlowSpec* sOutFlow,
	u16 nOutFlushTO,
	u16 nLinkTO,
	u16 *pInMTU,
	TBTL2CAPFlowSpec *pOutFlow,
	u16 *pOutFlushTO,
	bool blocking)
{
	u8 Config[32];
	u16 nLength = 0;
	bool found = false;
	CBTL2CAPChannel *pChannel = NULL;
	u16 nResult = BT_L2CAP_RESULT_REJECTED;

	LOG_DEBUG("L2CAP: CONFIGURE\r\n");
	// Search for an existing channel
	for (int i=0; i<m_Channels.GetCount(); i++) {
		pChannel = (CBTL2CAPChannel *)m_Channels[i];
		if (pChannel->CID == nCID && (pChannel->State == BT_L2CAP_CONFIG
			|| pChannel->State == BT_L2CAP_OPEN)) {
			found = true;
			break;
		}
	}
	if (found) {
		if (blocking) Clear();
		u8 ID = GetID();
		u8 *pConfig = Config;
		if (nInMTU) {
			pConfig = InsertMTU(pConfig, nInMTU);
			nLength += sizeof(CBTL2CAPMTU);
		}
		if (nOutFlushTO) {
			pConfig = InsertFlushTO(pConfig, nOutFlushTO);
			nLength += sizeof(CBTL2CAPFlushTimeout);
		}
		if (sOutFlow) {
			pConfig = InsertFlow(pConfig, sOutFlow);
			nLength += sizeof(CBTL2CAPQoS);
		}
		if (blocking ^ pChannel->Initiator == 0) {
			CBTL2CAPConfigurationRequest cmd(
				ID, pChannel->RemoteCID, 0, (CBTL2CAPOption *)Config, nLength);
			CBTL2CAPSignallingPacket pkt((u8*)&cmd, cmd.GetLength() );
			m_pLogicalLayer->SendACLData(
				pChannel->Connection, (void *)&pkt, cmd.GetLength() +4);
		}
		if (blocking) {
			m_eConfigRsp.Identifier = ID;
			Wait();
			if (ID == m_eConfigRsp.Identifier) {
				nResult = m_eConfigRsp.Result;
				*pInMTU = m_eConfigRsp.MTU;
				*pOutFlow = *m_eConfigRsp.Flow;
				*pOutFlushTO = m_eConfigRsp.FlushTO;
				if (!pChannel->Initiator && nResult == BT_L2CAP_RESULT_SUCCESS)
					pChannel->State = BT_L2CAP_OPEN; //initiator stays in CONFIG
			}
		}
	}
	return nResult;
}

u16 CBTL2CAPLayer::ConfigureResponse (
	u8  nID,
	u16 nLCID,
	u16 nFlags,
	u16 nResult,
	u16 nOutMTU,
	u16 nInFlushTO,
	TBTL2CAPFlowSpec* sInFlow)
{
	u8 Config[32];
	u16 nLength = 0;
	bool found = false;
	CBTL2CAPChannel *pChannel = NULL;

	LOG_DEBUG("L2CAP: CONFIGURE RESPONSE\r\n");
	// Search for an existing channel
	for (int i=0; i<m_Channels.GetCount(); i++) {
		pChannel = (CBTL2CAPChannel *)m_Channels[i];
		if (pChannel->CID == nLCID && (pChannel->State == BT_L2CAP_CONFIG
			|| pChannel->State == BT_L2CAP_OPEN)) {
			found = true;
			break;
		}
	}
	if (found) {
		u8 *pConfig = Config;
		if (nOutMTU) {
			pConfig = InsertMTU(pConfig, nOutMTU);
			nLength += sizeof(CBTL2CAPMTU);
		}
		if (nInFlushTO) {
			pConfig = InsertFlushTO(pConfig, nInFlushTO);
			nLength += sizeof(CBTL2CAPFlushTimeout);
		}
		if (sInFlow) {
			pConfig = InsertFlow(pConfig, sInFlow);
			nLength += sizeof(CBTL2CAPQoS);
		}
		CBTL2CAPConfigurationResponse cmd(
			nID, pChannel->RemoteCID, nFlags, nResult,
			(CBTL2CAPOption *)Config, nLength);
		CBTL2CAPSignallingPacket pkt((u8*)&cmd, cmd.GetLength());
		m_pLogicalLayer->SendACLData(
			pChannel->Connection, (void *)&pkt, cmd.GetLength() + 4);
		if (pChannel->Initiator && nResult == BT_L2CAP_RESULT_SUCCESS)
			pChannel->State = BT_L2CAP_OPEN; //Acceptor stays in CONFIG
	}

	return nResult;
}

u16 CBTL2CAPLayer::Disconnect (u16 nLCID)
{
	u16 nResult = BT_L2CAP_RESULT_DISCONNECTION_TIMEOUT_OCCURRED;
	bool found = false;
	CBTL2CAPChannel *pChannel = NULL;

	LOG_DEBUG("L2CAP: DISCONNECT\r\n");
	// Search for an existing channel
	for (int i=0; i<m_Channels.GetCount(); i++) {
		pChannel = (CBTL2CAPChannel *)m_Channels[i];
		if (pChannel->CID == nLCID && (pChannel->State == BT_L2CAP_CONFIG
			|| pChannel->State == BT_L2CAP_OPEN)) {
			found = true;
			break;
		}
	}
	if (found) {
		Clear();
		u8 nID = GetID();
		pChannel->State = BT_L2CAP_W4_L2CAP_DISCONNECT_RSP;
		CBTL2CAPDisconnectionRequest cmd(
			nID, pChannel->RemoteCID, pChannel->CID);
		CBTL2CAPSignallingPacket pkt((u8*)&cmd, cmd.GetLength());
		m_pLogicalLayer->SendACLData(
			pChannel->Connection, (void *)&pkt, cmd.GetLength() + 4);
		Wait(pChannel->RTX);
		if (m_eDisconnectRsp.Identifier == nID) 
			nResult = BT_L2CAP_RESULT_DISCONNECTION_SUCCESSFUL;
		m_eDisconnectRsp.Identifier = 0;	// reset the id
		pChannel->State = BT_L2CAP_CLOSED;	// mark it closed
		m_Channels.Delete(pChannel);		// delete the channel
	}
	return nResult;
}

u16 CBTL2CAPLayer::DisconnectResponse (u8 nID, u16 nLCID)
{
	u16 nResult = BT_L2CAP_RESULT_DISCONNECTION_FAILURE;
	bool found = false;
	CBTL2CAPChannel *pChannel = NULL;

	LOG_DEBUG("L2CAP: DISCONNECT RESPONSE\r\n");
	// Search for an existing channel
	for (int i=0; i<m_Channels.GetCount(); i++) {
		pChannel = (CBTL2CAPChannel *)m_Channels[i];
		if (pChannel->CID == nLCID && 
			pChannel->State == BT_L2CAP_W4_L2CA_DISCONNECT_RSP) {
			found = true;
			break;
		}
	}
	if (found) {
		CBTL2CAPDisconnectionResponse cmd(
			nID, pChannel->CID, pChannel->RemoteCID);
		CBTL2CAPSignallingPacket pkt((u8*)&cmd, cmd.GetLength());
		m_pLogicalLayer->SendACLData(
			pChannel->Connection, (void *)&pkt, cmd.GetLength() + 4);
		pChannel->State = BT_L2CAP_CLOSED;	// mark it closed
		m_Channels.Delete(pChannel);		// delete the channel
		nResult = BT_L2CAP_RESULT_DISCONNECTION_SUCCESSFUL;
	}
	return nResult;
}

u16 CBTL2CAPLayer::Write (
	u16 nCID,
	u16 nLength,
	u8 *pOutBuffer,
	u16 *pSize)
{
	u16 nResult = BT_L2CAP_RESULT_DISCONNECTION_TIMEOUT_OCCURRED;
	bool found = false;
	CBTL2CAPChannel *pChannel = NULL;

	LOG_DEBUG("L2CAP: WRITE\r\n");
	// Search for an existing channel
	for (int i=0; i<m_Channels.GetCount(); i++) {
		pChannel = (CBTL2CAPChannel *)m_Channels[i];
		if (pChannel->CID == nCID && (pChannel->State == BT_L2CAP_OPEN)) {
			found = true;
			break;
		}
	}
	if (found) {
		Clear();
		CBTL2CAPConnectionOrientedDataPacket pkt(
			nLength, pChannel->RemoteCID, pOutBuffer);
		m_pLogicalLayer->SendACLData(
			pChannel->Connection, (void *)&pkt, nLength + 4);
		Wait(pChannel->RTX);
		nResult = BT_L2CAP_RESULT_SUCCESS;
	}
	return nResult;
}

u16 CBTL2CAPLayer::Read (
	u16 nCID,
	u16 nLength,
	u8 *pBuffer,
	u16 *pN)
{
	u16 nResult = BT_L2CAP_RESULT_DISCONNECTION_TIMEOUT_OCCURRED;
	bool found = false;
	CBTL2CAPChannel *pChannel = NULL;

	LOG_DEBUG("L2CAP: READ\r\n");
	// Search for an existing channel
	for (int i=0; i<m_Channels.GetCount(); i++) {
		pChannel = (CBTL2CAPChannel *)m_Channels[i];
		if (pChannel->CID == nCID && (pChannel->State == BT_L2CAP_OPEN)) {
			found = true;
			break;
		}
	}
	if (found) {
		Clear();
		// Set up the read request
		m_eDataRead.CID = nCID;
		m_eDataRead.Length = nLength;
		m_eDataRead.InBuffer = pBuffer;
		m_eDataRead.Result = nResult;
		m_eDataRead.N = 0;
		Wait(pChannel->RTX);
		// Reset the read request
		m_eDataRead.CID = 0;
		m_eDataRead.Length = 0;
		m_eDataRead.InBuffer = 0;
		nResult = m_eDataRead.Result;
	}
	return nResult;
}

u16 CBTL2CAPLayer::GroupCreate (u16 nPSM)
{
	u16 nCID = BT_L2CAP_RESULT_SUCCESS;
	return nCID;
}

u16 CBTL2CAPLayer::GroupClose (u16 nCID)
{
	u16 nResult = BT_L2CAP_RESULT_SUCCESS;
	return nResult;
}

u16 CBTL2CAPLayer::Ping (
	TBDAddr sBDAddr,
	u8 *pEchoData,
	u16 nLength,
	u8** ppEchoData,
	u16 *pSize)
{
	u16 nResult = BT_L2CAP_RESULT_SUCCESS;
	return nResult;
}

u16 CBTL2CAPLayer::GetInfo (
	TBDAddr sBDAddr,
	u16 nInfoType,
	u8** ppInfoData,
	u16 *pSize)
{
	u16 nResult = BT_L2CAP_RESULT_SUCCESS;
	return nResult;
}

u16 CBTL2CAPLayer::DisableConnectionlessTraffic (u16 nPSM)
{
	u16 nResult = BT_L2CAP_RESULT_SUCCESS;
	return nResult;
}

u16 CBTL2CAPLayer::EnableConnectionlessTraffic (u16 nPSM)
{
	u16 nResult = BT_L2CAP_RESULT_SUCCESS;
	return nResult;
}

u16 CBTL2CAPLayer::GetPSM (u16 nCID)
{
	u16 nPSM = 0;

	// Search for an existing channel
	for (int i=0; i<m_Channels.GetCount(); i++) {
		CBTL2CAPChannel* pChannel = (CBTL2CAPChannel *)m_Channels[i];
		if (pChannel->CID == nCID) {
			nPSM = pChannel->PSM;
			break;
		}
	}
	return nPSM;
}

CBTL2CAPChannel* CBTL2CAPLayer::GetChannel (u16 nCID, bool bRemote/* false */)
{
	bool found = false;
	CBTL2CAPChannel* pChannel = NULL;

	// Search for an existing channel
	for (int i=0; i<m_Channels.GetCount() && !found; i++) {
		pChannel = (CBTL2CAPChannel *)m_Channels[i];
		if (bRemote) {
			if (pChannel->RemoteCID == nCID) found = true;
		} else {
			if (pChannel->CID == nCID) found = true;
		}
	}
	return pChannel;
}

bool CBTL2CAPLayer::AddChannel (u16 nPSM, u16 nCID)
{
	CBTL2CAPChannel *pChannel = new CBTL2CAPChannel(
		nPSM, m_pIncomingConnection, nCID);
	assert(pChannel != 0);
	if (pChannel) {
		pChannel->SetInitiator(false);  // this is an acceptor channel
		pChannel->SetState(BT_L2CAP_W4_L2CA_CONNECT_RSP);
		m_Channels.Append(pChannel);
	}
	return ((pChannel) ? true : false);
}

bool CBTL2CAPLayer::DeleteChannel (u16 nCID)
{
	bool found = false;
	CBTL2CAPChannel* pChannel = 0;

	// Search for an existing channel
	for (int i=0; i<m_Channels.GetCount() && !found; i++) {
		pChannel = (CBTL2CAPChannel *)m_Channels[i];
		if (pChannel->CID == nCID) found = true;
	}
	if (found) {
		m_Channels.Delete(pChannel);
		delete pChannel;
	}
	return found;
}

CBTDevice* CBTL2CAPLayer::GetDevice (u16 nCID)
{
	return (CBTDevice *)this->GetChannel(nCID)->Connection->GetDevice();
}

void CBTL2CAPLayer::SetCommandRsp (u8 ID, u16 Reason)
{
	m_eCommandRsp.Identifier = ID;
	m_eCommandRsp.Reason = Reason;
}

bool CBTL2CAPLayer::SetConnectRsp (
	u8 ID, u16 nDCID,u16 nSCID,u16 nResult,u16 nStatus)
{
	if (m_eConnectRsp.Identifier == ID) {
		*m_eConnectRsp.DCID = nDCID;
		*m_eConnectRsp.SCID = nSCID;
		*m_eConnectRsp.Result = nResult;
		*m_eConnectRsp.Status = nStatus;
		*m_eConnectRsp.State = (nResult==BT_L2CAP_RESULT_CONNECTION_SUCCESSFUL)
			? BT_L2CAP_CONFIG : BT_L2CAP_CLOSED;
		Set();
		return true;
	}
	return false;
}

bool CBTL2CAPLayer::SetConfigRsp (
	u8 ID, u16 nCID, u16 nFlags, u16 nResult, u16 nMTU, u16 nFlushTO, u8 *pFlow)
{
	if (m_eConfigRsp.Identifier == ID) {
		m_eConfigRsp.SourceCID = nCID;
		m_eConfigRsp.Flags = nFlags;
		m_eConfigRsp.Result = nResult;
		m_eConfigRsp.MTU = nMTU;
		m_eConfigRsp.Flow = (TBTL2CAPFlowSpec *)pFlow;
		m_eConfigRsp.FlushTO = nFlushTO;
		Set();
		return true;
	}
	return false;
}

void CBTL2CAPLayer::SetDisconnectRsp (u8 ID, u16 DCID, u16 SCID)
{
	m_eDisconnectRsp.Identifier = ID;
	m_eDisconnectRsp.DCID = DCID;
	m_eDisconnectRsp.SCID = SCID;
}

void CBTL2CAPLayer::SignallingCallback (u16 nPSM, void *cmd, size_t size)
{
	if (m_pL2CAPSignallingCallback[nPSM] != NULL) {
		m_pL2CAPSignallingCallback[nPSM](cmd, size);
	}
}

void CBTL2CAPLayer::LPEventHandler (const void *pBuffer, unsigned nLength)
{
	CBTLPEvent *pEvent;

	assert (pBuffer != 0);
	assert (nLength > 0);

	if (nLength < sizeof (CBTLPEvent)) {
		LOG_DEBUG ("Short event ignored\r\n");
		return;
	}

	pEvent = (CBTLPEvent *) pBuffer;
	switch (pEvent->Event) {
		case BT_EVENT_LP_CONNECT_CFM : {
			LOG_DEBUG("L2CAP: LP: received connect confirm\r\n");
			} break;
		case BT_EVENT_LP_CONNECT_CFM_NEG : {
			LOG_DEBUG("L2CAP: LP: received connect confirm neg\r\n");
			CBTConnection *pConnection
				= m_pLogicalLayer->GetConnection(
					((CBTLPConnectCfmNeg *)pEvent)->BDAddr);
			if (pConnection) 
				pConnection->GetDevice()->SetState(BT_DEVICE_IDLE);
			} break;
		case BT_EVENT_LP_CONNECT_IND : {
			LOG_DEBUG("L2CAP: LP: received connect indication\r\n");
			CBTConnection *pConnection
				= m_pLogicalLayer->GetConnection(
					((CBTLPConnectInd *)pEvent)->BDAddr);
			if (pConnection) {
				m_pIncomingConnection = pConnection;
				m_pSubSystem->CreateDevice(pConnection); 
				pConnection->GetDevice()->SetState(BT_DEVICE_CONNECTING);
				bool status = m_pLogicalLayer->ConnectResponse(
					pConnection, 0, (char *)BT_DEFAULT_PIN);
			}
			} break;
		case BT_EVENT_LP_DISCONNECT_IND : {
			LOG_DEBUG("L2CAP: LP: received disconnect indication\r\n");
			CBTConnection *pConnection
				= m_pLogicalLayer->GetConnection(
					((CBTLPDisconnectInd *)pEvent)->Handle);
			if (pConnection) 
				pConnection->GetDevice()->SetState(BT_DEVICE_IDLE);
			} break;
	}
}

void CBTL2CAPLayer::L2CAPEventHandler (const void *pBuffer, unsigned nLength)
{
	int index = 0;
	CBTL2CAPSignallingCommand *pCommand;

	assert (pBuffer != 0);
	assert (nLength > 0);

	if (nLength < sizeof (CBTL2CAPPacket)) {
		LOG_DEBUG ("L2CAPEventHandler: Short packet ignored\r\n");
		return;
	}

	assert (m_pEventBuffer != 0);
	memcpy (m_pEventBuffer, pBuffer, nLength);

	CBTL2CAPPacket *pHeader = (CBTL2CAPPacket *) m_pEventBuffer;
	switch (pHeader->GetCID()) {
	case BT_CID_SIGNALLING_CHANNEL : {
		CBTL2CAPSignallingPacket *pPacket = (CBTL2CAPSignallingPacket *)pHeader;
		while(pCommand=pPacket->GetCommand(index++))
			pCommand->Process(this, nLength);
		} break;
	case BT_CID_CONNECTIONLESS_DATA_CHANNEL : {
		LOG_DEBUG("Connectionless Data\r\n");
		CBTL2CAPConnectionLessDataPacket *pPacket
			= (CBTL2CAPConnectionLessDataPacket *)pHeader;
		} break;

	default: {
		CBTL2CAPPacket *pPacket = (CBTL2CAPPacket *)pHeader;
		u16 nPSM = GetPSM(pPacket->ChannelID);
		if (m_pPSMSlot[nPSM] != NULL)
			m_pPSMSlot[nPSM](pPacket->ChannelID,pPacket->Data,pPacket->Length);
		else {
			// Check if there's a blocked read on this CID
			if (pPacket->ChannelID == m_eDataRead.CID) {
				u16 rLength = m_eDataRead.Length - m_eDataRead.N;
				if (pPacket->Length < rLength) {
					memcpy( m_eDataRead.InBuffer+m_eDataRead.N, 
						pPacket->Data, pPacket->Length);
					m_eDataRead.N += pPacket->Length;
				} else {
					memcpy( m_eDataRead.InBuffer+m_eDataRead.N, 
						pPacket->Data, rLength);
					m_eDataRead.N += rLength;
					m_eDataRead.Result = BT_L2CAP_RESULT_SUCCESS;
					Set();	// signal when done
				}
			}
		}
		} break;
	}
}

void CBTL2CAPLayer::LPEventStub (const void *pBuffer, unsigned nLength)
{
	assert (s_pThis != 0);
	s_pThis->LPEventHandler (pBuffer, nLength);
}

void CBTL2CAPLayer::L2CAPEventStub (const void *pBuffer, unsigned nLength)
{
	assert (s_pThis != 0);
	s_pThis->L2CAPEventHandler (pBuffer, nLength);
}
