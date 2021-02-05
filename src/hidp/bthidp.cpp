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
** Description:    Raspberry Pi BareMetal Bluetooth HIDP Layer Routines
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
#include <bluetooth/btdevice.h>
#include <bluetooth/bthidp.h>
#include <logger.h>
#include <assert.h>
#include <synchronize.h>
#include <stdlib.h>
#include <string.h>

CBTHIDPLayer *CBTHIDPLayer::s_pThis = 0;

CBTHIDPLayer::CBTHIDPLayer (CBTL2CAPLayer *pL2CAPLayer)
:	m_pBuffer (0),
	m_pL2CAPLayer (pL2CAPLayer),
	m_nRxPackets (1)
{
	// Asign the static pointer
	assert (s_pThis == 0);
	s_pThis = this;

	// Assign the default flow specification for mouse
	m_sFlowMouse.Flags = 0;
	m_sFlowMouse.ServiceType = BT_L2CAP_SERVICE_TYPE_GUARANTEED;
	m_sFlowMouse.TokenRate = 300; //100
	m_sFlowMouse.TokenBucketSize = 4;
	m_sFlowMouse.PeakBandwidth = 300; //100
	m_sFlowMouse.Latency = 10000;
	m_sFlowMouse.DelayVariation = 10000;

	// Assign the default flow specification for keyboard
	m_sFlowKeyboard.Flags = 0;
	m_sFlowKeyboard.ServiceType = BT_L2CAP_SERVICE_TYPE_GUARANTEED;
	m_sFlowKeyboard.TokenRate = 900;
	m_sFlowKeyboard.TokenBucketSize = 9;
	m_sFlowKeyboard.PeakBandwidth = 900;
	m_sFlowKeyboard.Latency = 10000;
	m_sFlowKeyboard.DelayVariation = 10000;

	// Assign the default flow specification for joystick
	m_sFlowJoystick.Flags = 0;
	m_sFlowJoystick.ServiceType = BT_L2CAP_SERVICE_TYPE_GUARANTEED;
	m_sFlowJoystick.TokenRate = 600;
	m_sFlowJoystick.TokenBucketSize = 6;
	m_sFlowJoystick.PeakBandwidth = 600;
	m_sFlowJoystick.Latency = 10000;
	m_sFlowJoystick.DelayVariation = 10000;

	// Assign the HIDP Callbacks to NULL
	for (int i=0; i<MAX_HID_DEVICE; i++) m_DeviceTable[i] = NULL;

	// Register the L2CAP Callbacks
	pL2CAPLayer->RegisterCallback(BT_PSM_HID_CONTROL, EventStub);
	pL2CAPLayer->RegisterCallback(BT_PSM_HID_INTERRUPT, EventStub);
	pL2CAPLayer->RegisterDataCallback(BT_PSM_HID_CONTROL, DataStub);
	pL2CAPLayer->RegisterDataCallback(BT_PSM_HID_INTERRUPT, DataStub);
}

CBTHIDPLayer::~CBTHIDPLayer (void)
{
	m_pBuffer = 0;

	// Deregister the L2CAP Callbacks
	m_pL2CAPLayer->DeregisterDataCallback(BT_PSM_HID_CONTROL);
	m_pL2CAPLayer->DeregisterDataCallback(BT_PSM_HID_INTERRUPT);

	s_pThis = 0;
}

void CBTHIDPLayer::RegisterCallback (u16 nCID, CBTDevice *pDevice)
{
	m_DeviceTable[nCID-BT_CID_DYNAMICALLY_ALLOCATED] = pDevice;
	pDevice->SetState(BT_DEVICE_CONNECTED);
	LOG_DEBUG("HIDP Callback registered [%d] 0x%08X\r\n", nCID, (int)(pDevice));
}

void CBTHIDPLayer::DeregisterCallback (u16 nCID)
{
	m_DeviceTable[nCID-BT_CID_DYNAMICALLY_ALLOCATED] = NULL;
	LOG_DEBUG("HIDP Callback deregistered\r\n");
}

u16 CBTHIDPLayer::Connect (u16 nPSM, u8* sBDAddr, u16 nMTU)
{
	u16 nCID = 0;
	u16 nStatus = 0;
	u16 nResult = 0;
	u16 nInMTU = 0;
	u16 nOutFlushTO = 0;
	TBTL2CAPFlowSpec sOutFlow;

	if (m_pL2CAPLayer) {
		nResult = m_pL2CAPLayer->Connect(nPSM, sBDAddr, &nCID, &nStatus);
		if (!nResult) {
			if (nPSM == BT_PSM_HID_INTERRUPT) {
				CBTDevice* pDevice = m_pL2CAPLayer->GetDevice(nCID);
				if (pDevice->IsMouse())
					nResult = m_pL2CAPLayer->Configure(nCID, nMTU,
						&m_sFlowMouse,0x0000,0,&nInMTU,&sOutFlow,&nOutFlushTO);
				if (pDevice->IsKeyboard())
					nResult = m_pL2CAPLayer->Configure(nCID, nMTU,
						&m_sFlowKeyboard,0x0000,0,
						&nInMTU,&sOutFlow,&nOutFlushTO);
				if (pDevice->IsJoystick())
					nResult = m_pL2CAPLayer->Configure(nCID, nMTU,
						&m_sFlowJoystick,0x0000,0,
						&nInMTU,&sOutFlow,&nOutFlushTO);
			} else
				nResult = m_pL2CAPLayer->Configure(nCID, nMTU, NULL, 0x0000,
					0, &nInMTU, &sOutFlow, &nOutFlushTO);
		}
	}

	return nCID;
}

u16 CBTHIDPLayer::Disconnect (u16 nCID, u16 nReason)
{
	u16 nResult = 0;

	if (m_pL2CAPLayer)
		nResult = m_pL2CAPLayer->Disconnect(nCID);

	return nResult;
}

u16 CBTHIDPLayer::Send (u16 nCID, const void *pBuffer, u16 nLength)
{
	m_pL2CAPLayer->Write (nCID, nLength, (u8 *)pBuffer, &nLength);
	return nLength;
}

u16 CBTHIDPLayer::Receive (u16 nCID, const void *pBuffer, u16 *pLength)
{
	m_pL2CAPLayer->Read (nCID, *pLength, (u8 *)pBuffer, pLength);
	return *pLength;
}

void CBTHIDPLayer::Callback (const void *pBuffer, unsigned nLength)
{
	CBTL2CAEvent *pEvent;

	assert (pBuffer != 0);
	assert (nLength > 0);

	if (nLength < sizeof (CBTL2CAEvent)) {
		LOG_DEBUG ("BTHIDP: Short packet ignored\r\n");
		return;
	}

	pEvent = (CBTL2CAEvent *) pBuffer;
	switch (pEvent->Event) {
		case BT_EVENT_L2CA_CONNECT_IND : {
			CBTL2CAConnectInd *pConnInd = (CBTL2CAConnectInd *)pEvent;
			LOG_DEBUG("HIDP: connect indication:PSM=%d\r\n",(int)pConnInd->PSM);
			if (pConnInd->PSM == BT_PSM_HID_INTERRUPT) ;
			CBTL2CAPChannel *pChannel
				= m_pL2CAPLayer->GetChannel(pConnInd->CID, true);
			if (pChannel) {
				u16 nStatus = BT_L2CAP_STATUS_NO_FURTHER_INFORMATION;
				m_pL2CAPLayer->ConnectResponse(
					(u8*)pChannel->GetConnection()->GetBDAddress(),
					pConnInd->Identifier, CBTL2CAPChannel::GetCID(),
					BT_L2CAP_CONNECTION_SUCCESSFUL,
					nStatus);
			} else
				LOG_DEBUG("HIDP: CID %d doesn't exist\r\n", pConnInd->CID);
			} break;
		case BT_EVENT_L2CA_CONNECT_CFM : {
			CBTL2CAConnectCfm *pConnCfm = (CBTL2CAConnectCfm *)pEvent;
			LOG_DEBUG("HIDP: connect confirm:CID=%d\r\n",(int)pConnCfm->SCID);
			} break;
		case BT_EVENT_L2CA_CONFIG_IND : {
			CBTL2CAConfigInd *pConfigInd = (CBTL2CAConfigInd *)pEvent;
			LOG_DEBUG("HIDP:config indication:PSM=%d\r\n",(int)pConfigInd->PSM);
			if (pConfigInd->PSM == BT_PSM_HID_INTERRUPT) {
				CBTDevice* pDevice 
					= m_pL2CAPLayer->GetDevice(pConfigInd->CID);
				if (pDevice->IsMouse())
					pConfigInd->InFlow = &m_sFlowMouse;
				if (pDevice->IsKeyboard())
					pConfigInd->InFlow = &m_sFlowKeyboard;
				if (pDevice->IsJoystick())
					pConfigInd->InFlow = &m_sFlowJoystick;
			}
			m_pL2CAPLayer->ConfigureResponse(
				pConfigInd->Identifier, pConfigInd->CID, 0,
				BT_L2CAP_RESULT_SUCCESS,
				pConfigInd->OutMTU, pConfigInd->InFlushTO, pConfigInd->InFlow);
			// Send a configure request only from the acceptor
			if (pConfigInd->PSM == BT_PSM_HID_INTERRUPT) 
				m_pL2CAPLayer->Configure(pConfigInd->CID,
					pConfigInd->OutMTU, pConfigInd->InFlow,
					0x0000, 0, NULL, NULL, NULL, false);
			else
				m_pL2CAPLayer->Configure(pConfigInd->CID,
				pConfigInd->OutMTU, NULL, 0x0000, 0, NULL, NULL, NULL, false);
			} break;
		case BT_EVENT_L2CA_CONFIG_CFM : {
			CBTL2CAConfigCfm *pConfigCfm = (CBTL2CAConfigCfm *)pEvent;
			LOG_DEBUG("HIDP: config confirm:CID=%d\r\n",
				(int)pConfigCfm->SourceCID);
			CBTHIDDevice *pDevice = (CBTHIDDevice *)
				m_pL2CAPLayer->GetDevice(pConfigCfm->SourceCID);
			if (m_pL2CAPLayer->GetPSM(pConfigCfm->SourceCID)
				== BT_PSM_HID_INTERRUPT) {
				pDevice->SetInterruptCID( pConfigCfm->SourceCID );
				RegisterCallback(pConfigCfm->SourceCID, pDevice);
			} else
				pDevice->SetControlCID( pConfigCfm->SourceCID );
			} break;
		case BT_EVENT_L2CA_DISCONNECT_IND : {
			LOG_DEBUG("HIDP: disconn indication\r\n");
			CBTL2CADisconnectInd *pDisconnInd = (CBTL2CADisconnectInd *)pEvent;
			m_pL2CAPLayer->DisconnectResponse(
				pDisconnInd->Identifier, pDisconnInd->CID);
			} break;
		case BT_EVENT_L2CA_QOS_VIOLATION_IND : {
			LOG_DEBUG("HIDP: qos indication\r\n");
			CBTL2CAQoSViolationInd *pQoSInd = (CBTL2CAQoSViolationInd *)pEvent;
			} break;
		default:
			break;
	}
}

void CBTHIDPLayer::DataHandler (u16 nCID, u8 *pBuffer, u16 nLength)
{
	CBTHIDPMessage *pMessage = (CBTHIDPMessage *)pBuffer;
	switch(pMessage->MessageType) {

		case BT_HIDP_HANDSHAKE: {
			} break;

		case BT_HIDP_HID_CONTROL: {
			} break;

		case BT_HIDP_GET_REPORT: {
			} break;

		case BT_HIDP_SET_REPORT: {
			} break;

		case BT_HIDP_GET_PROTOCOL: {
			} break;

		case BT_HIDP_SET_PROTOCOL: {
			} break;

		case BT_HIDP_DATA: {
				CBTHIDPMessageData *pData = (CBTHIDPMessageData *)pBuffer;
				if (m_DeviceTable[nCID])
					m_DeviceTable[nCID]->Parser(
						pData->ReportDataPayload, nLength-1);
			} break;

		default: break;
	}
}

void CBTHIDPLayer::EventStub (const void *pBuffer, unsigned nLength)
{
	assert (s_pThis != 0);
	s_pThis->Callback (pBuffer, nLength);
}

void CBTHIDPLayer::DataStub (u16 nCID, const void *pBuffer, unsigned nLength)
{
	assert (nCID >= BT_CID_DYNAMICALLY_ALLOCATED);
	assert (s_pThis != 0);

	if (nCID < BT_CID_DYNAMICALLY_ALLOCATED) return;
	u16 nID = nCID - BT_CID_DYNAMICALLY_ALLOCATED;
	s_pThis->DataHandler (nID, (u8 *)pBuffer, nLength);
}

////////////////////////////////////////////////////////////////////////////////
//
// HIDP Messages
//
////////////////////////////////////////////////////////////////////////////////

CBTHIDPMessage::CBTHIDPMessage(u8 nMessageType)
:	MessageType(nMessageType)
{
}

CBTHIDPMessageHandshake::CBTHIDPMessageHandshake(void)
:	CBTHIDPMessage(BT_HIDP_HANDSHAKE)
{
	Parameter = BT_HIDP_HANDSHAKE_SUCCESSFUL;
}

CBTHIDPMessageHandshake::CBTHIDPMessageHandshake(u8 nResult)
:	CBTHIDPMessage(BT_HIDP_HANDSHAKE)
{
	Parameter = nResult;
}

CBTHIDPMessageControl::CBTHIDPMessageControl(void)
:	CBTHIDPMessage(BT_HIDP_HID_CONTROL)
{
	Parameter = BT_HIDP_CONTROL_SUSPEND;
}

CBTHIDPMessageControl::CBTHIDPMessageControl(u8 nCommand)
:	CBTHIDPMessage(BT_HIDP_HID_CONTROL)
{
	Parameter = nCommand;
}

CBTHIDPMessageGetReport::CBTHIDPMessageGetReport(void)
:	CBTHIDPMessage(BT_HIDP_GET_REPORT)
{
	Parameter = 0;
}

CBTHIDPMessageGetReport::CBTHIDPMessageGetReport(
	u8 nSize, u8 nType, u8 nID, u16 nBuffSize)
:	CBTHIDPMessage(BT_HIDP_GET_REPORT)
{
	Parameter = (nSize | nType);
	ReportID = nID;
	BufferSize = nBuffSize;
}

CBTHIDPMessageSetReport::CBTHIDPMessageSetReport(void)
:	CBTHIDPMessage(BT_HIDP_SET_REPORT)
{
	Parameter = 0;
}

CBTHIDPMessageSetReport::CBTHIDPMessageSetReport(u8 nReportType, u8* pData)
:	CBTHIDPMessage(BT_HIDP_SET_REPORT)
{
	Parameter = nReportType;
}

CBTHIDPMessageData::CBTHIDPMessageData(void)
:	CBTHIDPMessage(BT_HIDP_DATA)
{
	Parameter = 0;
}

CBTHIDPMessageData::CBTHIDPMessageData(u8* pData, u8 nLength)
:	CBTHIDPMessage(BT_HIDP_DATA)
{
	memcpy(ReportDataPayload, pData, nLength);
}

////////////////////////////////////////////////////////////////////////////////
//
// HID Device
//
////////////////////////////////////////////////////////////////////////////////

CBTHIDDevice::CBTHIDDevice(
	CBTHIDPLayer *pHIDPLayer,
	CBTConnection *pConnection)
:	CBTDevice(pConnection),
	m_EventQueue()
{
	m_pHIDPLayer = pHIDPLayer;
	m_nControlCID = 0;
	m_nInterruptCID = 0;
}

CBTHIDDevice::~CBTHIDDevice(void)
{
	if (m_nInterruptCID) m_pHIDPLayer->DeregisterCallback(m_nInterruptCID);
}

void CBTHIDDevice::SetControlCID(u16 nCID)
{
	if (m_nControlCID && m_nControlCID != nCID)
		m_pHIDPLayer->m_pL2CAPLayer->DeleteChannel(m_nControlCID);
	m_nControlCID = nCID;
	return;
}

void CBTHIDDevice::SetInterruptCID(u16 nCID)
{
	if (m_nInterruptCID && m_nInterruptCID != nCID)
		m_pHIDPLayer->m_pL2CAPLayer->DeleteChannel(m_nInterruptCID);
	m_nInterruptCID = nCID;
	return;
}

void CBTHIDDevice::Parser(u8* pBuffer, u16 nLen)
{
	LOG_DEBUG("HIDP: Parser\r\n");
}

u16 CBTHIDDevice::Connect(void)
{
	u16 nCID = 0;
	if (m_tState == BT_DEVICE_CONNECTED) return 0;
	if (m_tState != BT_DEVICE_IDLE) return 1;
	u8* sBDAddr = (u8 *)m_pConnection->GetBDAddress();

	nCID = m_pHIDPLayer->Connect(BT_PSM_HID_CONTROL, sBDAddr, 48);
	if (nCID) m_nControlCID = nCID;
	LOG_DEBUG("Connect: CID = %d\r\n", nCID);
	nCID = m_pHIDPLayer->Connect(BT_PSM_HID_INTERRUPT, sBDAddr, 48);
	m_nInterruptCID = nCID;
	LOG_DEBUG("Interrupt: CID = %d\r\n", nCID);
	if (nCID) m_pHIDPLayer->RegisterCallback(nCID, this);

	return 0;
}

bool CBTHIDDevice::DisconnectInterruptChannel(void)
{
	if (m_nInterruptCID)
		m_pHIDPLayer->Disconnect(m_nInterruptCID, 0);
}

bool CBTHIDDevice::DisconnectControlChannel(void)
{
	if (m_nControlCID)
		m_pHIDPLayer->Disconnect(m_nControlCID, 0);
}

bool CBTHIDDevice::IsInBootProtocolMode(void)
{
	return (m_tProtocolMode == BT_HIDP_PROTOCOL_MODE_BOOT);
}

bool CBTHIDDevice::SendInterruptMessage(const u8* pBuffer, u16 nLength)
{
	if (!m_nInterruptCID) return true;
	m_pHIDPLayer->Send(m_nInterruptCID, pBuffer, nLength);
	return false;
}

bool CBTHIDDevice::SendControlMessage(const u8* pBuffer, u16 nLength)
{
	if (!m_nControlCID) return true;
	m_pHIDPLayer->Send(m_nControlCID, pBuffer, nLength);
	return false;
}

bool CBTHIDDevice::ReceiveEvent (void *pBuffer, unsigned *pResultLength)
{
	unsigned nLength;
	if (!m_EventQueue.IsEmpty()) {
		nLength = m_EventQueue.Dequeue (pBuffer);
		if (nLength > 0) {
			assert (pResultLength != 0);
			if (pResultLength) *pResultLength = nLength;
			return TRUE;
		}
	}

	return FALSE;
}

void CBTHIDDevice::PostEvent (const void *pBuffer, unsigned nLength)
{
	assert (pBuffer != 0);
	assert (nLength > 0);
	m_EventQueue.Enqueue (pBuffer, nLength);
}
