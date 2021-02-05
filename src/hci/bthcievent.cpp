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
** Copyright (C) 2016 Ariana Communications - www.ariana-communications.com
**
** Description:    Raspberry Pi BareMetal Bluetooth HCI Layer Events
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
#include <sysconfig.h>
#include <synchronize.h>
#include <logger.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

static const u8 Firmware[] =
{
#ifdef RPI
	#include "platform/rpi/BCM43430A1.h"
#else
#endif
};

TBTLMPEventHandler* CBTHCIEvent::Handler[BT_EVENT_NUM_EVENTS] = {};

////////////////////////////////////////////////////////////////////////////////
//
// LMP Event Handlers
//
////////////////////////////////////////////////////////////////////////////////
void CBTHCIEvent::Process(void *pLayer, u16 nLength)
{
	if (CBTHCIEvent::Handler[EventCode])
		CBTHCIEvent::Handler[EventCode](this, pLayer, nLength);
}

CBTHCIEvent::CBTHCIEvent()
{
	static bool init = false;

	if (!init) {
		for (int i=0; i<BT_EVENT_NUM_EVENTS; i++) Handler[i] = NULL;
		init = true;
	}
	return;
}

void CBTHCIEvent::Register(u8 nEventCode, void* ptr)
{
	Handler[nEventCode] = (TBTLMPEventHandler *)ptr;
}

CBTHCIEventInquiryComplete::CBTHCIEventInquiryComplete()
{
	CBTHCIEvent::Register(BT_EVENT_CODE_INQUIRY_COMPLETE, (void *)Handler);
}

void CBTHCIEventInquiryComplete::Handler(void *ptr, void *lptr, u16 nLength)
{
	((CBTHCIEventInquiryComplete *)ptr)->Process(lptr, nLength);
}

void CBTHCIEventInquiryComplete::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTHCIEventInquiryComplete));
	CBTLogicalLayer *pLogicalLayer = (CBTLogicalLayer *) pLayer;
	CBTInquiryResults*& rInquiryResults = pLogicalLayer->GetInquiryResults();
	unsigned& rNameRequestsPending = pLogicalLayer->GetNameRequestsPending();

	if (Status != BT_STATUS_SUCCESS) {
		delete rInquiryResults;
		rInquiryResults = 0;
		pLogicalLayer->Set ();
		return;
	}

	assert (rInquiryResults != 0);
	if (rInquiryResults) {
		rNameRequestsPending = rInquiryResults->GetCount ();
		if (rNameRequestsPending == 0) {
			pLogicalLayer->Set ();
			return;
		}

		for (unsigned nResponse = 0; nResponse < rInquiryResults->GetCount ();
			nResponse++) {
			CBTHCIRemoteNameRequestCommand Cmd(
				(u8 *)rInquiryResults->GetBDAddress(nResponse),
				rInquiryResults->GetPageScanRepetitionMode(nResponse));
			pLogicalLayer->SendHCICommand (&Cmd, sizeof Cmd);
		}
	}
}

CBTHCIEventInquiryResult::CBTHCIEventInquiryResult()
{
	CBTHCIEvent::Register(BT_EVENT_CODE_INQUIRY_RESULT, (void *)Handler);
}

void CBTHCIEventInquiryResult::Handler(void *ptr, void *lptr, u16 nLength)
{
	((CBTHCIEventInquiryResult *)ptr)->Process(lptr, nLength);
}

void CBTHCIEventInquiryResult::Process(void *pLayer, u16 nLength)
{
	assert (nLength >=   sizeof (CBTHCIEventInquiryResult)
			   + NumResponses * INQUIRY_RESP_SIZE);
	CBTLogicalLayer *pLogicalLayer = (CBTLogicalLayer *) pLayer;
	CBTInquiryResults*& rInquiryResults = pLogicalLayer->GetInquiryResults();

	assert (rInquiryResults != 0);
	if (rInquiryResults) rInquiryResults->AddInquiryResult (this);
}

CBTHCIEventConnectionComplete::CBTHCIEventConnectionComplete()
{
	CBTHCIEvent::Register(BT_EVENT_CODE_CONNECTION_COMPLETE, (void *)Handler);
}

void CBTHCIEventConnectionComplete::Handler(void *ptr, void *lptr, u16 nLength)
{
	((CBTHCIEventConnectionComplete *)ptr)->Process(lptr, nLength);
}

void CBTHCIEventConnectionComplete::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTHCIEventConnectionComplete));
	CBTLogicalLayer *pLogicalLayer = (CBTLogicalLayer *) pLayer;
	CBTConnection*& rConnection = pLogicalLayer->GetConnectionPtr();;
	LOG_DEBUG("LMP connection complete status: 0x%02X\r\n", Status);
	LOG_DEBUG("Connection handle: 0x%02X\r\n", ConnectionHandle);

	if (Status == BT_STATUS_SUCCESS) {
		assert(rConnection != 0);

		if (rConnection) {
			rConnection->SetBDAddress(BDAddr);
			rConnection->SetConnectionHandle(ConnectionHandle);
			rConnection->SetLinkType(LinkType);
			rConnection->SetEncryptionMode(EncryptionMode);
			rConnection->SetState(BTConnectionStateConnected);
			rConnection->SetStatus(Status);
		}

		LOG_DEBUG("LMP: Connection pointer = 0x%08X\r\n", (int)rConnection);
		LOG_DEBUG("Link type: 0x%02X\r\n", LinkType);
		LOG_DEBUG("Encrypt mode: 0x%02X\r\n", EncryptionMode);
		LOG_DEBUG ("BD address: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
			(unsigned) BDAddr[5], (unsigned) BDAddr[4], (unsigned) BDAddr[3],
			(unsigned) BDAddr[2], (unsigned) BDAddr[1], (unsigned) BDAddr[0]);

		if (pLogicalLayer->m_pLPCallback) {
			CBTLPConnectCfm event;
			event.Event = BT_EVENT_LP_CONNECT_CFM;
			memcpy(event.BDAddr, BDAddr, BT_BD_ADDR_SIZE);
			pLogicalLayer->m_pLPCallback(&event, sizeof event);
		}
	} else {
		if (rConnection) {
			rConnection->SetState(BTConnectionStateConnectionFailed);
			rConnection->SetStatus(Status);
		}
		if (pLogicalLayer->m_pLPCallback) {
			CBTLPConnectCfmNeg event;
			event.Event = BT_EVENT_LP_CONNECT_CFM_NEG;
			memcpy(event.BDAddr, BDAddr, BT_BD_ADDR_SIZE);
			pLogicalLayer->m_pLPCallback(&event, sizeof event);
		}
	}
	rConnection = NULL;
	pLogicalLayer->SetConnectingFlag(false);
	pLogicalLayer->Set();
}

CBTHCIEventConnectionRequest::CBTHCIEventConnectionRequest()
{
	CBTHCIEvent::Register(BT_EVENT_CODE_CONNECTION_REQUEST, (void *)Handler);
}

void CBTHCIEventConnectionRequest::Handler(void *ptr, void *lptr, u16 nLength)
{
	((CBTHCIEventConnectionRequest *)ptr)->Process(lptr, nLength);
}

void CBTHCIEventConnectionRequest::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTHCIEventConnectionRequest));
	CBTLogicalLayer *pLogicalLayer = (CBTLogicalLayer *) pLayer;
	LOG_DEBUG("LP: Received a Connection request\r\n");
	CBTConnection* pConnection = pLogicalLayer->GetConnection(BDAddr);
	if (!pConnection) {
		pConnection = new CBTConnection;
		assert(pConnection != 0);
		pConnection->SetBDAddress((u8*)BDAddr);
		pConnection->SetClassOfDevice((u8*)ClassOfDevice);
		pConnection->SetLinkType(LinkType);
		pLogicalLayer->GetConnections().Append(pConnection);
	}
	if (pConnection) {
		pConnection->SetState(BTConnectionStateConnecting);
		pLogicalLayer->SetConnectionPtr(pConnection);
		pLogicalLayer->SetConnectingFlag(true);
		if (pLogicalLayer->m_pLPCallback) {
			CBTLPConnectInd event;
			event.Event = BT_EVENT_LP_CONNECT_IND;
			memcpy(event.BDAddr, BDAddr, BT_BD_ADDR_SIZE);
			pLogicalLayer->m_pLPCallback(&event, sizeof event);
		}
	}
}

CBTHCIEventDisconnectionComplete::CBTHCIEventDisconnectionComplete()
{
	CBTHCIEvent::Register(BT_EVENT_CODE_DISCONNECTION_COMPLETE,(void *)Handler);
}

void CBTHCIEventDisconnectionComplete::Handler(void *ptr,void *lptr,u16 nLength)
{
	((CBTHCIEventDisconnectionComplete *)ptr)->Process(lptr, nLength);
}

void CBTHCIEventDisconnectionComplete::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTHCIEventDisconnectionComplete));
	LOG_DEBUG("LMP disconnection complete status: 0x%02X\r\n", Status);
	CBTLogicalLayer *pLogicalLayer = (CBTLogicalLayer *) pLayer;
	CPtrArray& rConnections = pLogicalLayer->GetConnections();;

	if (Status == BT_STATUS_SUCCESS) {
		for (int i=0; i<rConnections.GetCount(); i++) {
			CBTConnection *pConnection = (CBTConnection *)rConnections[i];
			if(pConnection->HasConnectionHandle(ConnectionHandle)) {
				pConnection->SetState(BTConnectionStateDisconnected);
				pConnection->SetStatus(Status);
				break;
			}
		}
		if (pLogicalLayer->m_pLPCallback) {
			CBTLPDisconnectInd event;
			event.Event = BT_EVENT_LP_DISCONNECT_IND;
			event.Handle = ConnectionHandle;
			pLogicalLayer->m_pLPCallback(&event, sizeof event);
		}
	} else if (pLogicalLayer->GetConnectionPtr()) {
		CBTConnection*& m_pConnection = pLogicalLayer->GetConnectionPtr();
		m_pConnection->SetState(BTConnectionStateDisconnectionFailed);
		m_pConnection->SetStatus(Status);
	}
}

CBTHCIEventAuthenticationComplete::CBTHCIEventAuthenticationComplete()
{
	CBTHCIEvent::Register(BT_EVENT_CODE_AUTHENTICATION_COMPLETE,
		(void *)Handler);
}

void CBTHCIEventAuthenticationComplete::Handler(
	void *ptr, void *lptr, u16 nLength)
{
	((CBTHCIEventAuthenticationComplete *)ptr)->Process(lptr, nLength);
}

void CBTHCIEventAuthenticationComplete::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTHCIEventAuthenticationComplete));
	LOG_DEBUG("LMP authentication complete status: 0x%02X\r\n", Status);
	CBTLogicalLayer *pLogicalLayer = (CBTLogicalLayer *) pLayer;
	CBTConnection*& m_pConnection = pLogicalLayer->GetConnectionPtr();

	if (Status == BT_STATUS_SUCCESS) 
		m_pConnection->SetState(BTConnectionStateAuthenticated);
	else
		m_pConnection->SetState(BTConnectionStateAuthenticationFailed);
	m_pConnection->SetStatus(Status);
	pLogicalLayer->Set();

}

CBTHCIEventFlushOccurred::CBTHCIEventFlushOccurred()
{
	CBTHCIEvent::Register(BT_EVENT_CODE_FLUSH_OCCURRED, (void *)Handler);
}

void CBTHCIEventFlushOccurred::Handler(void *ptr, void *lptr, u16 nLength)
{
	((CBTHCIEventFlushOccurred *)ptr)->Process(lptr, nLength);
}

void CBTHCIEventFlushOccurred::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTHCIEventFlushOccurred));
	LOG_DEBUG("LMP Flush Occurred: Conn Handle = %d\r\n", ConnectionHandle);
	CBTLogicalLayer *pLogicalLayer = (CBTLogicalLayer *) pLayer;
}

CBTHCIEventRoleChange::CBTHCIEventRoleChange()
{
	CBTHCIEvent::Register(BT_EVENT_CODE_ROLE_CHANGE, (void *)Handler);
}

void CBTHCIEventRoleChange::Handler(void *ptr, void *lptr, u16 nLength)
{
	((CBTHCIEventRoleChange *)ptr)->Process(lptr, nLength);
}

void CBTHCIEventRoleChange::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTHCIEventRoleChange));
	LOG_DEBUG("LMP Role Change: New Role = %d\r\n", NewRole);
	CBTLogicalLayer *pLogicalLayer = (CBTLogicalLayer *) pLayer;
	CBTConnection* pConnection = pLogicalLayer->GetConnection(BDAddr);
	if (pConnection) {
		if (Status == BT_STATUS_SUCCESS) pConnection->SetRole(NewRole);
		else pConnection->SetRole(
			(NewRole==ROLE_MASTER) ? ROLE_SLAVE : ROLE_MASTER);
	}
}

CBTHCIEventNumberOfCompletedPackets::CBTHCIEventNumberOfCompletedPackets()
{
	CBTHCIEvent::Register(BT_EVENT_CODE_NUMBER_OF_COMPLETED_PACKETS,
		(void *)Handler);
}

void CBTHCIEventNumberOfCompletedPackets::Handler(
	void *ptr, void *lptr, u16 nLength)
{
	((CBTHCIEventNumberOfCompletedPackets *)ptr)->Process(lptr, nLength);
}

void CBTHCIEventNumberOfCompletedPackets::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTHCIEventNumberOfCompletedPackets));
	LOG_DEBUG("LMP data pack sent = %d\r\n", HCNumOfCompletedPackets[0]);
	CBTLogicalLayer *pLogicalLayer = (CBTLogicalLayer *) pLayer;
	pLogicalLayer->SetHCIDataPackets(HCNumOfCompletedPackets[0]);
}

CBTHCIEventModeChange::CBTHCIEventModeChange()
{
	CBTHCIEvent::Register(BT_EVENT_CODE_MODE_CHANGE, (void *)Handler);
}

void CBTHCIEventModeChange::Handler(void *ptr, void *lptr, u16 nLength)
{
	((CBTHCIEventModeChange *)ptr)->Process(lptr, nLength);
}

void CBTHCIEventModeChange::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTHCIEventModeChange));
	LOG_DEBUG("LMP Mode Change: New Mode = %d\r\n", CurrentMode);
	CBTLogicalLayer *pLogicalLayer = (CBTLogicalLayer *) pLayer;
	CBTConnection* pConnection = pLogicalLayer->GetConnection(ConnectionHandle);
	if (Status == BT_STATUS_SUCCESS && pConnection)
		pConnection->SetMode((TBTMode)CurrentMode, Interval);
}

CBTHCIEventRemoteNameRequestComplete::CBTHCIEventRemoteNameRequestComplete()
{
	CBTHCIEvent::Register(BT_EVENT_CODE_REMOTE_NAME_REQUEST_COMPLETE,
		(void *)Handler);
}

void CBTHCIEventRemoteNameRequestComplete::Handler(
	void *ptr, void *lptr, u16 nLength)
{
	((CBTHCIEventRemoteNameRequestComplete *)ptr)->Process(lptr, nLength);
}

void CBTHCIEventRemoteNameRequestComplete::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTHCIEventRemoteNameRequestComplete));
	CBTLogicalLayer *pLogicalLayer = (CBTLogicalLayer *) pLayer;
	CBTInquiryResults*& rInquiryResults = pLogicalLayer->GetInquiryResults();
	unsigned& rNameRequestsPending = pLogicalLayer->GetNameRequestsPending();

	if (Status == BT_STATUS_SUCCESS) {
		assert (rInquiryResults != 0);
		if (rInquiryResults) rInquiryResults->SetRemoteName (this);
	}

	if (--rNameRequestsPending == 0) {
		pLogicalLayer->Set ();
	}
}

CBTHCIEventReadRemoteSupportedFeaturesComplete::CBTHCIEventReadRemoteSupportedFeaturesComplete()
{
	CBTHCIEvent::Register(BT_EVENT_CODE_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE,
		(void *)Handler);
}

void CBTHCIEventReadRemoteSupportedFeaturesComplete::Handler(
	void *ptr, void *lptr, u16 nLength)
{
	((CBTHCIEventReadRemoteSupportedFeaturesComplete *)ptr)->Process(
		lptr, nLength);
}

void CBTHCIEventReadRemoteSupportedFeaturesComplete::Process(
	void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTHCIEventReadRemoteSupportedFeaturesComplete));
	LOG_DEBUG("LMP read remote supported features complete status: 0x%02X\r\n", Status);
	CBTLogicalLayer *pLogicalLayer = (CBTLogicalLayer *) pLayer;
	CBTConnection*& m_pConnection = pLogicalLayer->GetConnectionPtr();

	if (Status == BT_STATUS_SUCCESS) {

		LOG_DEBUG ("LMP features: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
			(unsigned) LMPFeatures[7], (unsigned) LMPFeatures[6],
			(unsigned) LMPFeatures[5], (unsigned) LMPFeatures[4],
			(unsigned) LMPFeatures[3], (unsigned) LMPFeatures[2],
			(unsigned) LMPFeatures[1], (unsigned) LMPFeatures[0]);
	}
	m_pConnection->SetStatus(Status);
	pLogicalLayer->Set();
}

CBTHCIEventReadRemoteVersionInformationComplete::CBTHCIEventReadRemoteVersionInformationComplete()
{
	CBTHCIEvent::Register(
		BT_EVENT_CODE_READ_REMOTE_VERSION_INFORMATION_COMPLETE,(void *)Handler);
}

void CBTHCIEventReadRemoteVersionInformationComplete::Handler(
	void *ptr, void *lptr, u16 nLength)
{
	((CBTHCIEventReadRemoteVersionInformationComplete *)ptr)->Process(
		lptr, nLength);
}

void CBTHCIEventReadRemoteVersionInformationComplete::Process(
	void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof(CBTHCIEventReadRemoteVersionInformationComplete));
	LOG_DEBUG("LMP read remote ver-info complete status: 0x%02X\r\n", Status);
	CBTLogicalLayer *pLogicalLayer = (CBTLogicalLayer *) pLayer;
	CBTConnection*& m_pConnection = pLogicalLayer->GetConnectionPtr();

	if (Status == BT_STATUS_SUCCESS) {

		m_pConnection->SetVersion(LMPVersion);
		m_pConnection->SetManufacturer(ManufacturerName);
		m_pConnection->SetSubversion(LMPSubversion);
		LOG_DEBUG("LMP version: 0x%02X\r\n", LMPVersion);
		LOG_DEBUG("Manufacturer name: 0x%04X\r\n", ManufacturerName);
		LOG_DEBUG("LMP subversion: 0x%04X\r\n", LMPSubversion);
	}
	m_pConnection->SetStatus(Status);
	pLogicalLayer->Set();
}

CBTHCIEventCommandComplete::CBTHCIEventCommandComplete()
{
	CBTHCIEvent::Register(BT_EVENT_CODE_COMMAND_COMPLETE,(void *)Handler);
}

void CBTHCIEventCommandComplete::Handler(void *ptr, void *lptr, u16 nLength)
{
	((CBTHCIEventCommandComplete *)ptr)->Process(lptr, nLength);
}

void CBTHCIEventCommandComplete::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTHCIEventCommandComplete));
	CBTDeviceManager *pDeviceManager = (CBTDeviceManager *)pLayer;

	pDeviceManager->SetHCICommandPackets (NumHCICommandPackets);

	if (Status != BT_STATUS_SUCCESS) {
		LOG_DEBUG ( "Command 0x%X failed (status 0x%X)\r\n",
					(unsigned) CommandOpCode, (unsigned) Status);

		pDeviceManager->SetState(BTDeviceStateFailed);

		return;
	}

	switch (CommandOpCode) {

		case OP_CODE_RESET:
			if (pDeviceManager->CheckState(BTDeviceStateResetPending)) {

			if (pDeviceManager->m_pHCILayer->GetTransportType ()
				!= BTTransportTypeUART) {
				goto NoFirmwareLoad;
			}
			CBTHCICommand Cmd(OP_CODE_DOWNLOAD_MINIDRIVER);
			pDeviceManager->SendHCICommand (&Cmd, sizeof Cmd);
			pDeviceManager->Sleep (50000);

			pDeviceManager->m_nFirmwareOffset = 0;
			pDeviceManager->SetState(BTDeviceStateWriteRAMPending);
			} break;

		case OP_CODE_DOWNLOAD_MINIDRIVER:
		case OP_CODE_WRITE_RAM:
			if (pDeviceManager->CheckState(BTDeviceStateWriteRAMPending)) {

			assert (pDeviceManager->m_nFirmwareOffset+3 <= sizeof Firmware);
			u16 nOpCode  = Firmware[pDeviceManager->m_nFirmwareOffset++];
			    nOpCode |= Firmware[pDeviceManager->m_nFirmwareOffset++] << 8;
			u8 nLength = Firmware[pDeviceManager->m_nFirmwareOffset++];

			CBTHCIBcmVendorCommand Cmd(
				nOpCode, nLength, (u8 *)Firmware,
				&pDeviceManager->m_nFirmwareOffset);
			pDeviceManager->SendHCICommand(&Cmd, sizeof(CBTHCICommand)+nLength);

			if (nOpCode == OP_CODE_LAUNCH_RAM)
				pDeviceManager->SetState(BTDeviceStateLaunchRAMPending);
			} break;

		case OP_CODE_LAUNCH_RAM:
			if (pDeviceManager->CheckState(BTDeviceStateLaunchRAMPending)) {
			pDeviceManager->Sleep (250000);

		NoFirmwareLoad:
			CBTHCICommand Cmd(OP_CODE_READ_BD_ADDR);
			pDeviceManager->SendHCICommand (&Cmd, sizeof Cmd);

			pDeviceManager->SetState(BTDeviceStateReadBDAddrPending);
			} break;

		case OP_CODE_READ_BD_ADDR:
			if (pDeviceManager->CheckState(BTDeviceStateReadBDAddrPending)) {

			assert (nLength >= sizeof (CBTHCIEventReadBDAddrComplete));
			CBTHCIEventReadBDAddrComplete *pEvent
				= (CBTHCIEventReadBDAddrComplete *) this;
			pDeviceManager->SetBDAddr(pEvent->BDAddr);

			LOG_DEBUG ("Local BD ADDR is %02X:%02X:%02X:%02X:%02X:%02X\r\n",
				(unsigned) pDeviceManager->m_LocalBDAddr[5],
				(unsigned) pDeviceManager->m_LocalBDAddr[4],
				(unsigned) pDeviceManager->m_LocalBDAddr[3],
				(unsigned) pDeviceManager->m_LocalBDAddr[2],
				(unsigned) pDeviceManager->m_LocalBDAddr[1],
				(unsigned) pDeviceManager->m_LocalBDAddr[0]);

			CBTHCIWriteClassOfDeviceCommand Cmd(pDeviceManager->m_nClassOfDevice);
			pDeviceManager->SendHCICommand (&Cmd, sizeof Cmd);

			pDeviceManager->SetState(BTDeviceStateWriteClassOfDevicePending);
			} break;

		case OP_CODE_READ_STORED_LINK_KEY:
			if (pDeviceManager->CheckState(BTDeviceStateRunning)) {
			assert (nLength >= sizeof (CBTHCIEventReadStoredLinkKeyComplete));
			CBTHCIEventReadStoredLinkKeyComplete *pEvent
				= (CBTHCIEventReadStoredLinkKeyComplete *) this;
			if (pEvent->NumKeysRead) {
				assert(pDeviceManager->m_pConnection != 0);
				CBTHCILinkKeyRequestReplyCommand Cmd(
					(u8*)pDeviceManager->m_pConnection->GetBDAddress(),
					(u8*)pDeviceManager->m_pConnection->GetLinkKey());
				pDeviceManager->SendHCICommand (&Cmd, sizeof Cmd);
			} else {
				CBTHCILinkKeyRequestNegativeReplyCommand Cmd(
					(u8*)pDeviceManager->m_pConnection->GetBDAddress());
				pDeviceManager->SendHCICommand (&Cmd, sizeof Cmd);
			}
			pDeviceManager->m_pConnection = NULL;

			pDeviceManager->SetState(BTDeviceStateRunning);
			} break;

		case OP_CODE_WRITE_STORED_LINK_KEY:
			if (pDeviceManager->CheckState(BTDeviceStateRunning)) {
			assert (nLength >= sizeof (CBTHCIEventWriteStoredLinkKeyComplete));
			CBTHCIEventWriteStoredLinkKeyComplete *pEvent
				= (CBTHCIEventWriteStoredLinkKeyComplete *) this;

			pDeviceManager->SetState(BTDeviceStateRunning);
			} break;

		case OP_CODE_WRITE_CLASS_OF_DEVICE:
			if (pDeviceManager->CheckState(BTDeviceStateWriteClassOfDevicePending)) {

			CBTHCIWriteLocalNameCommand Cmd(pDeviceManager->m_LocalName);
			pDeviceManager->SendHCICommand (&Cmd, sizeof Cmd);

			pDeviceManager->SetState(BTDeviceStateWriteLocalNamePending);
			} break;

		case OP_CODE_WRITE_LOCAL_NAME:
			if (pDeviceManager->CheckState(BTDeviceStateWriteLocalNamePending)){

			CBTHCIWriteScanEnableCommand Cmd(SCAN_ENABLE_BOTH_ENABLED);
			pDeviceManager->SendHCICommand (&Cmd, sizeof Cmd);

			pDeviceManager->SetState(BTDeviceStateWriteScanEnabledPending);
			} break;

		case OP_CODE_WRITE_SCAN_ENABLE:
			if (pDeviceManager->CheckState(BTDeviceStateWriteScanEnabledPending))
				pDeviceManager->SetState(BTDeviceStateRunning);
			break;

		default:
			break;
	}

}

CBTHCIEventCommandStatus::CBTHCIEventCommandStatus()
{
	CBTHCIEvent::Register(BT_EVENT_CODE_COMMAND_STATUS,(void *)Handler);
}

void CBTHCIEventCommandStatus::Handler(void *ptr, void *lptr, u16 nLength)
{
	((CBTHCIEventCommandStatus *)ptr)->Process(lptr, nLength);
}

void CBTHCIEventCommandStatus::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTHCIEventCommandStatus));
	CBTDeviceManager *pDeviceManager = (CBTDeviceManager *)pLayer;

	pDeviceManager->SetHCICommandPackets (NumHCICommandPackets);
}

CBTHCIEventLinkKeyRequest::CBTHCIEventLinkKeyRequest()
{
	CBTHCIEvent::Register(BT_EVENT_CODE_LINK_KEY_REQUEST,(void *)Handler);
}

void CBTHCIEventLinkKeyRequest::Handler(void *ptr, void *lptr, u16 nLength)
{
	((CBTHCIEventLinkKeyRequest *)ptr)->Process(lptr, nLength);
}

void CBTHCIEventLinkKeyRequest::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTHCIEventLinkKeyRequest));
	CBTLogicalLayer *pLogicalLayer = (CBTLogicalLayer *) pLayer;

	pLogicalLayer->GetDeviceManager()->SetConnection(
		pLogicalLayer->GetConnectionPtr());
	pLogicalLayer->GetConnectionPtr()->SetBDAddress(BDAddr);
	CBTHCIReadStoredLinkKeyCommand Cmd(BDAddr);
	pLogicalLayer->SendHCICommand (&Cmd, sizeof Cmd);
}

CBTHCIEventLinkKeyNotification::CBTHCIEventLinkKeyNotification()
{
	CBTHCIEvent::Register(BT_EVENT_CODE_LINK_KEY_NOTIFICATION,(void *)Handler);
}

void CBTHCIEventLinkKeyNotification::Handler(void *ptr, void *lptr, u16 nLength)
{
	((CBTHCIEventLinkKeyNotification *)ptr)->Process(lptr, nLength);
}

void CBTHCIEventLinkKeyNotification::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTHCIEventLinkKeyNotification));
	LOG_DEBUG("LMP Link Key Notification Event\r\n");
	CBTLogicalLayer *pLogicalLayer = (CBTLogicalLayer *) pLayer;
	CBTConnection*& m_pConnection = pLogicalLayer->GetConnectionPtr();
	assert (m_pConnection != NULL);
	if (m_pConnection) {
		m_pConnection->SetLinkKey (LinkKey);
		CBTHCIWriteStoredLinkKeyCommand Cmd(BDAddr, LinkKey);
		pLogicalLayer->SendHCICommand (&Cmd, sizeof Cmd);
	} else {
		LOG_DEBUG("LMP Link Key Not Found\r\n");
	}
}

CBTHCIEventReturnLinkKeys::CBTHCIEventReturnLinkKeys()
{
	CBTHCIEvent::Register(BT_EVENT_CODE_RETURN_LINK_KEYS,(void *)Handler);
}

void CBTHCIEventReturnLinkKeys::Handler(void *ptr, void *lptr, u16 nLength)
{
	((CBTHCIEventReturnLinkKeys *)ptr)->Process(lptr, nLength);
}

void CBTHCIEventReturnLinkKeys::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTHCIEventReturnLinkKeys));
	CBTLogicalLayer *pLogicalLayer = (CBTLogicalLayer *) pLayer;
	CBTConnection*& m_pConnection = pLogicalLayer->GetConnectionPtr();

	LOG_DEBUG("LMP: Received %d Link Key\r\n", NumKeys);
	if (NumKeys) {
		assert(m_pConnection != NULL);
		m_pConnection->SetLinkKey (LinkKey, true);
	} 
}

CBTHCIEventPINCodeRequest::CBTHCIEventPINCodeRequest()
{
	CBTHCIEvent::Register(BT_EVENT_CODE_PIN_CODE_REQUEST,(void *)Handler);
}

void CBTHCIEventPINCodeRequest::Handler(void *ptr, void *lptr, u16 nLength)
{
	((CBTHCIEventPINCodeRequest *)ptr)->Process(lptr, nLength);
}

void CBTHCIEventPINCodeRequest::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTHCIEventPINCodeRequest));
	CBTLogicalLayer *pLogicalLayer = (CBTLogicalLayer *) pLayer;
	CBTConnection*& pConnection = pLogicalLayer->GetConnectionPtr();

	LOG_DEBUG("LMP: Got a PIN Code request\r\n");
	CBTHCIPINCodeRequestReplyCommand Cmd(
		BDAddr, pConnection->GetPINSize(), (u8*)pConnection->GetPIN());
	pLogicalLayer->SendHCICommand (&Cmd, sizeof Cmd);
}

CBTHCIEventMaxSlotsChange::CBTHCIEventMaxSlotsChange()
{
	CBTHCIEvent::Register(BT_EVENT_CODE_MAX_SLOTS_CHANGE,(void *)Handler);
}

void CBTHCIEventMaxSlotsChange::Handler(void *ptr, void *lptr, u16 nLength)
{
	((CBTHCIEventMaxSlotsChange *)ptr)->Process(lptr, nLength);
}

void CBTHCIEventMaxSlotsChange::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTHCIEventMaxSlotsChange));
	CBTLogicalLayer *pLogicalLayer = (CBTLogicalLayer *) pLayer;
	CPtrArray& rConnections = pLogicalLayer->GetConnections();

	for (int i=0; i<rConnections.GetCount(); i++) {
		CBTConnection *pConnection = (CBTConnection *)rConnections[i];
		if (pConnection && pConnection->HasConnectionHandle(ConnectionHandle))
			pConnection->SetSlots(LMPMaxSlots);
	}
}
