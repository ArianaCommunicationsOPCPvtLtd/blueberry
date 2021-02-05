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
** Description:    Raspberry Pi BareMetal Bluetooth L2CAP Layer Commands
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


TBTL2CAPCmdHandler* CBTL2CAPSignallingCommand::Handler[BT_SIG_NUM_COMMANDS]={};

////////////////////////////////////////////////////////////////////////////////
//
// L2CAP Configuration Option Friend Functions
//
////////////////////////////////////////////////////////////////////////////////

u16 ExtractMTU(u8* cOption, u16 nLength)
{
	u16 nMTU = 0;

	while (nLength) {
		CBTL2CAPOption *pOption = (CBTL2CAPOption *)cOption;
		switch(pOption->Type) {
			case BT_L2CAP_OPTION_MTU	: {
					CBTL2CAPMTU *pMTU = (CBTL2CAPMTU *)cOption;
					nMTU = pMTU->MTU;
					nLength = 0;
				} break;
			case BT_L2CAP_OPTION_FLUSH_TIMEOUT	: {
					cOption += sizeof(CBTL2CAPFlushTimeout);
					nLength -= sizeof(CBTL2CAPFlushTimeout);
				} break;
			case BT_L2CAP_OPTION_QOS	: {
					cOption += sizeof(CBTL2CAPQoS);
					nLength -= sizeof(CBTL2CAPQoS);
				} break;
		}
	}
	return nMTU;
}

TBTL2CAPFlowSpec* ExtractFlow(u8* cOption, u16 nLength)
{
	TBTL2CAPFlowSpec* nFlow = NULL;

	while (nLength) {
		CBTL2CAPOption *pOption = (CBTL2CAPOption *)cOption;
		switch(pOption->Type) {
			case BT_L2CAP_OPTION_MTU	: {
					cOption += sizeof(CBTL2CAPMTU);
					nLength -= sizeof(CBTL2CAPMTU);
				} break;
			case BT_L2CAP_OPTION_FLUSH_TIMEOUT	: {
					cOption += sizeof(CBTL2CAPFlushTimeout);
					nLength -= sizeof(CBTL2CAPFlushTimeout);
				} break;
			case BT_L2CAP_OPTION_QOS	: {
					CBTL2CAPQoS *pQoS = (CBTL2CAPQoS *)cOption;
					nFlow = &pQoS->Flow;
					nLength = 0;
				} break;
		}
	}
	return nFlow;
}

u16 ExtractFlushTO(u8* cOption, u16 nLength)
{
	u16 nFlushTimeout = 0;

	while (nLength) {
		CBTL2CAPOption *pOption = (CBTL2CAPOption *)cOption;
		switch(pOption->Type) {
			case BT_L2CAP_OPTION_MTU	: {
					cOption += sizeof(CBTL2CAPMTU);
					nLength -= sizeof(CBTL2CAPMTU);
				} break;
			case BT_L2CAP_OPTION_FLUSH_TIMEOUT	: {
					CBTL2CAPFlushTimeout *pFlushTimeout
						= (CBTL2CAPFlushTimeout *)cOption;
					nFlushTimeout = pFlushTimeout->FlushTimeout;
					nLength = 0;
				} break;
			case BT_L2CAP_OPTION_QOS	: {
					cOption += sizeof(CBTL2CAPQoS);
					nLength -= sizeof(CBTL2CAPQoS);
				} break;
		}
	}
	return nFlushTimeout;
}

////////////////////////////////////////////////////////////////////////////////
//
// L2CAP Signalling Commands
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// L2CAP Command Handlers
//
////////////////////////////////////////////////////////////////////////////////

CBTL2CAPSignallingCommand::CBTL2CAPSignallingCommand()
{
	static bool init = false;

	if (!init) {
		for (int i=0; i<BT_SIG_NUM_COMMANDS; i++) Handler[i] = NULL;
		init = true;
	}
	return;
}

CBTL2CAPSignallingCommand::CBTL2CAPSignallingCommand(u8 nCode)
:	Code(nCode)
{
	Identifier = 0;
	Length = 0;
}

CBTL2CAPSignallingCommand::CBTL2CAPSignallingCommand(u8 nCode, u8 nIdentifier)
:	Code(nCode),
	Identifier(nIdentifier)
{
	Length = 0;
}

u16 CBTL2CAPSignallingCommand::GetLength(void)
{
	return (Length + sizeof(CBTL2CAPSignallingCommand));
}

void CBTL2CAPSignallingCommand::Process(void *pLayer, u16 nLength)
{
	if (CBTL2CAPSignallingCommand::Handler[Code])
		CBTL2CAPSignallingCommand::Handler[Code](this, pLayer, nLength);
}

void CBTL2CAPSignallingCommand::Register(u8 nEventCode, void* ptr)
{
	Handler[nEventCode] = (TBTL2CAPCmdHandler *)ptr;
}

CBTL2CAPSignallingCommandReject::CBTL2CAPSignallingCommandReject()
:	CBTL2CAPSignallingCommand(BT_SIG_COMMAND_REJECT)
{
	CBTL2CAPSignallingCommand::Register(BT_SIG_COMMAND_REJECT, (void *)Handler);
	Identifier = 0;
	Reason = BT_L2CAP_COMMAND_NOT_UNDERSTOOD;
	Length = 2;
}

CBTL2CAPSignallingCommandReject::CBTL2CAPSignallingCommandReject(
	u8 nIdentifier, u16 nReason, u8* pData)
:	CBTL2CAPSignallingCommand(BT_SIG_COMMAND_REJECT, nIdentifier)
{
	Reason = nReason;
	switch (Reason) {
		case BT_L2CAP_COMMAND_NOT_UNDERSTOOD:
			Length = 2;
			break;
		case BT_L2CAP_SIGNALLING_MTU_EXCEEDED:
			Length = 4;
			memcpy(Data, pData, 2);
			break;
		case BT_L2CAP_INVALID_CID_IN_REQUEST:
			Length = 6;
			memcpy(Data, pData, 4);
			break;
		default :
			Length = 2;
			break;
	}
}

void CBTL2CAPSignallingCommandReject::Handler(void *ptr,void *lptr,u16 nLength)
{
	((CBTL2CAPSignallingCommandReject *)ptr)->Process(lptr, nLength);
}

void CBTL2CAPSignallingCommandReject::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTL2CAPSignallingCommandReject));
	CBTL2CAPLayer *pL2CAPLayer = (CBTL2CAPLayer *) pLayer;
	LOG_DEBUG("CBTL2CAPSignallingCommandReject\r\n");
	pL2CAPLayer->SetCommandRsp( Identifier, Reason);
	pL2CAPLayer->Set();

}

CBTL2CAPConnectionRequest::CBTL2CAPConnectionRequest()
:	CBTL2CAPSignallingCommand(BT_SIG_CONNECTION_REQUEST)
{
	CBTL2CAPSignallingCommand::Register(
		BT_SIG_CONNECTION_REQUEST, (void *)Handler);
	Identifier = 0;
	Length = 4;
}

CBTL2CAPConnectionRequest::CBTL2CAPConnectionRequest(
	u8 nIdentifier, u16 nPSM, u16 nSourceCID)
:	CBTL2CAPSignallingCommand(BT_SIG_CONNECTION_REQUEST, nIdentifier)
{
	Length = 4;
	PSM = nPSM;
	SourceCID = nSourceCID;
}

void CBTL2CAPConnectionRequest::Handler(void *ptr,void *lptr,u16 nLength)
{
	((CBTL2CAPConnectionRequest *)ptr)->Process(lptr, nLength);
}

void CBTL2CAPConnectionRequest::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTL2CAPConnectionRequest));
	CBTL2CAPLayer *pL2CAPLayer = (CBTL2CAPLayer *) pLayer;

	LOG_DEBUG("CBTL2CAPConnectionRequest : 0x%04X\r\n", PSM);
	CBTL2CAConnectInd ind;
	ind.Identifier = Identifier;
	ind.Event = BT_EVENT_L2CA_CONNECT_IND;
	ind.CID = SourceCID;
	ind.PSM = PSM;
	pL2CAPLayer->AddChannel(PSM, SourceCID);
	pL2CAPLayer->SignallingCallback(PSM, &ind,sizeof ind);
}

CBTL2CAPConnectionResponse::CBTL2CAPConnectionResponse()
:	CBTL2CAPSignallingCommand(BT_SIG_CONNECTION_RESPONSE)
{
	CBTL2CAPSignallingCommand::Register(
		BT_SIG_CONNECTION_RESPONSE, (void *)Handler);
	Identifier = 0;
	Length = 8;
}

CBTL2CAPConnectionResponse::CBTL2CAPConnectionResponse(
	u8 nIdentifier,u16 nDestinationCID,u16 nSourceCID,u16 nResult,u16 nStatus)
:	CBTL2CAPSignallingCommand(BT_SIG_CONNECTION_RESPONSE, nIdentifier)
{
	Length = 8;
	DestinationCID = nDestinationCID;
	SourceCID = nSourceCID;
	Result = nResult;
	Status = nStatus;
}

void CBTL2CAPConnectionResponse::Handler(void *ptr,void *lptr,u16 nLength)
{
	((CBTL2CAPConnectionResponse *)ptr)->Process(lptr, nLength);
}

void CBTL2CAPConnectionResponse::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTL2CAPConnectionResponse));
	CBTL2CAPLayer *pL2CAPLayer = (CBTL2CAPLayer *) pLayer;

	LOG_DEBUG("CBTL2CAPConnectionResponse\r\n");
	if (!pL2CAPLayer->SetConnectRsp(
		Identifier, DestinationCID, SourceCID, Result, Status)) {
		CBTL2CAConnectCfm cfm;
		cfm.Identifier = Identifier;
		cfm.Event = BT_EVENT_L2CA_CONNECT_CFM;
		cfm.DCID = DestinationCID;
		cfm.SCID = SourceCID;
		cfm.Result = Result;
		cfm.Status = Status;
		u16 nPSM = pL2CAPLayer->GetPSM(SourceCID);
		pL2CAPLayer->SignallingCallback(nPSM, &cfm, sizeof cfm);
	}
}

CBTL2CAPConfigurationRequest::CBTL2CAPConfigurationRequest()
:	CBTL2CAPSignallingCommand(BT_SIG_CONFIGURATION_REQUEST)
{
	CBTL2CAPSignallingCommand::Register(
		BT_SIG_CONFIGURATION_REQUEST, (void *)Handler);
	Identifier = 0;
	Length = 4;
	DestinationCID = 0;
	Flags = 0;
}

CBTL2CAPConfigurationRequest::CBTL2CAPConfigurationRequest(
	u8 nIdentifier,
	u16 nDestinationCID,
	u16 nFlags,
	CBTL2CAPOption* pOption,
	u16 nLength)
:	CBTL2CAPSignallingCommand(BT_SIG_CONFIGURATION_REQUEST, nIdentifier)
{
	Length = nLength + 4;
	DestinationCID = nDestinationCID;
	Flags = nFlags;
	memcpy(Options, (u8*)pOption, nLength);
}

u16 CBTL2CAPConfigurationRequest::GetMTU(void)
{
	u16 nLength = Length - sizeof(DestinationCID) - sizeof(Flags);

	return ExtractMTU(Options, nLength);
}

TBTL2CAPFlowSpec* CBTL2CAPConfigurationRequest::GetFlow(void)
{
	u16 nLength = Length - sizeof(DestinationCID) - sizeof(Flags);

	return ExtractFlow(Options, nLength);
}

u16 CBTL2CAPConfigurationRequest::GetFlushTO(void)
{
	u16 nLength = Length - sizeof(DestinationCID) - sizeof(Flags);

	return ExtractFlushTO(Options, nLength);
}

void CBTL2CAPConfigurationRequest::Handler(void *ptr,void *lptr,u16 nLength)
{
	((CBTL2CAPConfigurationRequest *)ptr)->Process(lptr, nLength);
}

void CBTL2CAPConfigurationRequest::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTL2CAPConfigurationRequest));
	CBTL2CAPLayer *pL2CAPLayer = (CBTL2CAPLayer *) pLayer;

	LOG_DEBUG("CBTL2CAPConfigurationRequest\r\n");
	CBTL2CAConfigInd ind;
	ind.Identifier = Identifier;
	ind.Event = BT_EVENT_L2CA_CONFIG_IND;
	ind.CID = DestinationCID;
	ind.PSM = pL2CAPLayer->GetPSM(ind.CID);
	ind.OutMTU = GetMTU();
	ind.InFlow = GetFlow();
	ind.InFlushTO = GetFlushTO();
	pL2CAPLayer->SignallingCallback(ind.PSM, &ind, sizeof ind);
}

CBTL2CAPConfigurationResponse::CBTL2CAPConfigurationResponse()
:	CBTL2CAPSignallingCommand(BT_SIG_CONFIGURE_RESPONSE)
{
	CBTL2CAPSignallingCommand::Register(
		BT_SIG_CONFIGURE_RESPONSE, (void *)Handler);
	Identifier = 0;
	Length = 6;
	SourceCID = 0;
	Flags = 0;
	Result = 0;
}

CBTL2CAPConfigurationResponse::CBTL2CAPConfigurationResponse(
	u8 nIdentifier,
	u16 nSourceCID,
	u16 nFlags,
	u16 nResult,
	CBTL2CAPOption* pConfig,
	u16 nLength)
:	CBTL2CAPSignallingCommand(BT_SIG_CONFIGURE_RESPONSE, nIdentifier)
{
	Length = nLength + 6;
	SourceCID = nSourceCID;
	Flags = nFlags;
	Result = nResult;
	memcpy(Config, (u8*)pConfig, nLength);
}

u16 CBTL2CAPConfigurationResponse::GetMTU(void)
{
	u16 nLength = Length - sizeof(SourceCID) - sizeof(Flags) - sizeof(Result);

	return ExtractMTU(Config, nLength);
}

TBTL2CAPFlowSpec* CBTL2CAPConfigurationResponse::GetFlow(void)
{
	u16 nLength = Length - sizeof(SourceCID) - sizeof(Flags) - sizeof(Result);

	return ExtractFlow(Config, nLength);
}

u16 CBTL2CAPConfigurationResponse::GetFlushTO(void)
{
	u16 nLength = Length - sizeof(SourceCID) - sizeof(Flags) - sizeof(Result);

	return ExtractFlushTO(Config, nLength);
}

void CBTL2CAPConfigurationResponse::Handler(void *ptr,void *lptr,u16 nLength)
{
	((CBTL2CAPConfigurationResponse *)ptr)->Process(lptr, nLength);
}

void CBTL2CAPConfigurationResponse::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTL2CAPConfigurationResponse));
	CBTL2CAPLayer *pL2CAPLayer = (CBTL2CAPLayer *) pLayer;

	LOG_DEBUG("L2CAP:Configuration Response\r\n");
	u16 nMTU = GetMTU();
	u16 nFlushTO = GetFlushTO();
	u8* pFlow = (u8 *)GetFlow();
	if (!pL2CAPLayer->SetConfigRsp(
	Identifier, SourceCID, Flags,Result, nMTU, nFlushTO, pFlow)) {
		CBTL2CAConfigCfm cfm;
		cfm.Identifier = Identifier;
		cfm.Event = BT_EVENT_L2CA_CONFIG_CFM;
		cfm.SourceCID = SourceCID;
		cfm.Flags = Flags;
		cfm.Result = Result;
		cfm.MTU = nMTU; 
		cfm.Flow = (TBTL2CAPFlowSpec *)pFlow; 
		cfm.FlushTO = nFlushTO; 
		u16 nPSM = pL2CAPLayer->GetPSM(SourceCID);
		pL2CAPLayer->SignallingCallback(nPSM, &cfm, sizeof cfm);
	}
}

CBTL2CAPDisconnectionRequest::CBTL2CAPDisconnectionRequest()
:	CBTL2CAPSignallingCommand(BT_SIG_DISCONNECTION_REQUEST)
{
	CBTL2CAPSignallingCommand::Register(
		BT_SIG_DISCONNECTION_REQUEST, (void *)Handler);
	Identifier = 0;
	Length = 4;
}

CBTL2CAPDisconnectionRequest::CBTL2CAPDisconnectionRequest(
	u8 nIdentifier, u16 nDestinationCID, u16 nSourceCID)
:	CBTL2CAPSignallingCommand(BT_SIG_DISCONNECTION_REQUEST, nIdentifier)
{
	Length = 4;
	DestinationCID = nDestinationCID;
	SourceCID = nSourceCID;
}

void CBTL2CAPDisconnectionRequest::Handler(void *ptr,void *lptr,u16 nLength)
{
	((CBTL2CAPDisconnectionRequest *)ptr)->Process(lptr, nLength);
}

void CBTL2CAPDisconnectionRequest::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTL2CAPDisconnectionRequest));
	CBTL2CAPLayer *pL2CAPLayer = (CBTL2CAPLayer *) pLayer;

	LOG_DEBUG("CBTL2CAPDisconnectionRequest\r\n");
	CBTL2CADisconnectInd ind;
	ind.Identifier = Identifier;
	ind.Event = BT_EVENT_L2CA_DISCONNECT_IND;
	ind.CID = DestinationCID;
	LOG_DEBUG("L2CAP:CID=0x%x\r\n", DestinationCID);
	CBTL2CAPChannel* pChannel = pL2CAPLayer->GetChannel(DestinationCID);
	if (pChannel) pChannel->SetState(BT_L2CAP_W4_L2CA_DISCONNECT_RSP);
	u16 nPSM = pL2CAPLayer->GetPSM(ind.CID);
	pL2CAPLayer->SignallingCallback(nPSM, &ind, sizeof ind);
}

CBTL2CAPDisconnectionResponse::CBTL2CAPDisconnectionResponse()
:	CBTL2CAPSignallingCommand(BT_SIG_DISCONNECTION_RESPONSE)
{
	CBTL2CAPSignallingCommand::Register(
		BT_SIG_DISCONNECTION_RESPONSE, (void *)Handler);
	Identifier = 0;
	Length = 4;
}

CBTL2CAPDisconnectionResponse::CBTL2CAPDisconnectionResponse(
	u8 nIdentifier, u16 nDestinationCID, u16 nSourceCID)
:	CBTL2CAPSignallingCommand(BT_SIG_DISCONNECTION_RESPONSE, nIdentifier)
{
	Length = 4;
	DestinationCID = nDestinationCID;
	SourceCID = nSourceCID;
}

void CBTL2CAPDisconnectionResponse::Handler(void *ptr,void *lptr,u16 nLength)
{
	((CBTL2CAPDisconnectionResponse *)ptr)->Process(lptr, nLength);
}

void CBTL2CAPDisconnectionResponse::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTL2CAPDisconnectionResponse));
	CBTL2CAPLayer *pL2CAPLayer = (CBTL2CAPLayer *) pLayer;

	LOG_DEBUG("CBTL2CAPDisconnectionResponse\r\n");
	pL2CAPLayer->SetDisconnectRsp(Identifier, DestinationCID, SourceCID);
	pL2CAPLayer->Set();
}

CBTL2CAPEchoRequest::CBTL2CAPEchoRequest()
:	CBTL2CAPSignallingCommand(BT_SIG_ECHO_REQUEST)
{
	CBTL2CAPSignallingCommand::Register(BT_SIG_ECHO_REQUEST, (void *)Handler);
	Identifier = 0;
	Length = 0;
}

CBTL2CAPEchoRequest::CBTL2CAPEchoRequest(
	u8 nIdentifier, u8* pBuffer, u16 nLength)
:	CBTL2CAPSignallingCommand(BT_SIG_ECHO_REQUEST, nIdentifier)
{
	Length = nLength;
	memcpy(Data, pBuffer, nLength);
}

void CBTL2CAPEchoRequest::Handler(void *ptr,void *lptr,u16 nLength)
{
	((CBTL2CAPEchoRequest *)ptr)->Process(lptr, nLength);
}

void CBTL2CAPEchoRequest::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTL2CAPEchoRequest));
	CBTL2CAPLayer *pL2CAPLayer = (CBTL2CAPLayer *) pLayer;

}

CBTL2CAPEchoResponse::CBTL2CAPEchoResponse()
:	CBTL2CAPSignallingCommand(BT_SIG_ECHO_RESPONSE)
{
	CBTL2CAPSignallingCommand::Register(BT_SIG_ECHO_RESPONSE, (void *)Handler);
	Identifier = 0;
	Length = 0;
}

CBTL2CAPEchoResponse::CBTL2CAPEchoResponse(
	u8 nIdentifier, u8* pBuffer, u16 nLength)
:	CBTL2CAPSignallingCommand(BT_SIG_ECHO_RESPONSE, nIdentifier)
{
	Length = nLength;
	memcpy(Data, pBuffer, nLength);
}

void CBTL2CAPEchoResponse::Handler(void *ptr,void *lptr,u16 nLength)
{
	((CBTL2CAPEchoResponse *)ptr)->Process(lptr, nLength);
}

void CBTL2CAPEchoResponse::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTL2CAPEchoResponse));
	CBTL2CAPLayer *pL2CAPLayer = (CBTL2CAPLayer *) pLayer;

}

CBTL2CAPInformationRequest::CBTL2CAPInformationRequest()
:	CBTL2CAPSignallingCommand(BT_SIG_INFORMATION_REQUEST)
{
	CBTL2CAPSignallingCommand::Register(
		BT_SIG_INFORMATION_REQUEST, (void *)Handler);
	Identifier = 0;
	Length = 2;
}

CBTL2CAPInformationRequest::CBTL2CAPInformationRequest(
	u8 nIdentifier, u16 nInfoType)
:	CBTL2CAPSignallingCommand(BT_SIG_INFORMATION_REQUEST, nIdentifier)
{
	Length = 2;
	InfoType = nInfoType;
}

void CBTL2CAPInformationRequest::Handler(void *ptr,void *lptr,u16 nLength)
{
	((CBTL2CAPInformationRequest *)ptr)->Process(lptr, nLength);
}

void CBTL2CAPInformationRequest::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTL2CAPInformationRequest));
	CBTL2CAPLayer *pL2CAPLayer = (CBTL2CAPLayer *) pLayer;

}

CBTL2CAPInformationResponse::CBTL2CAPInformationResponse()
:	CBTL2CAPSignallingCommand(BT_SIG_INFORMATION_RESPONSE)
{
	CBTL2CAPSignallingCommand::Register(
		BT_SIG_INFORMATION_RESPONSE, (void *)Handler);
	Identifier = 0;
	Length = 2;
}

CBTL2CAPInformationResponse::CBTL2CAPInformationResponse(
	u8 nIdentifier, u16 nInfoType, u16 nResult, u16 nData)
:	CBTL2CAPSignallingCommand(BT_SIG_INFORMATION_RESPONSE, nIdentifier)
{
	InfoType = nInfoType;
	switch(InfoType) {
		case BT_L2CAP_CONNECTIONLESS_MTU:
			Data = nData;
			Length = 6;
			break;
		default:
			Length = 4;
			break;
	}
}

void CBTL2CAPInformationResponse::Handler(void *ptr,void *lptr,u16 nLength)
{
	((CBTL2CAPInformationResponse *)ptr)->Process(lptr, nLength);
}

void CBTL2CAPInformationResponse::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTL2CAPInformationResponse));
	CBTL2CAPLayer *pL2CAPLayer = (CBTL2CAPLayer *) pLayer;

}
