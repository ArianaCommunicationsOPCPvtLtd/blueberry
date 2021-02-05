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
** Description:    Raspberry Pi BareMetal Bluetooth HCI Command Routines
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
#include <bluetooth/btcommand.h>
#include <string.h>

CBTHCICommand::CBTHCICommand(u16 nOpCode)
:	OpCode(nOpCode)
{
	ParameterTotalLength = 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// HCI Commands
//
////////////////////////////////////////////////////////////////////////////////

CBTHCIReadStoredLinkKeyCommand::CBTHCIReadStoredLinkKeyCommand(void)
:	CBTHCICommand(OP_CODE_READ_STORED_LINK_KEY)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(CBTHCIReadStoredLinkKeyCommand);
}

CBTHCIReadStoredLinkKeyCommand::CBTHCIReadStoredLinkKeyCommand(u8* sBDAddr)
:	CBTHCICommand(OP_CODE_READ_STORED_LINK_KEY)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(CBTHCIReadStoredLinkKeyCommand);
	memcpy (BDAddr, sBDAddr, BT_BD_ADDR_SIZE);
	ReadAllFlag = RETURN_LINK_KEY_FOR_SPECIFIED_BD_ADDR;
}

CBTHCIWriteStoredLinkKeyCommand::CBTHCIWriteStoredLinkKeyCommand(void)
:	CBTHCICommand(OP_CODE_WRITE_STORED_LINK_KEY)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(CBTHCIWriteStoredLinkKeyCommand);
}

CBTHCIWriteStoredLinkKeyCommand::CBTHCIWriteStoredLinkKeyCommand(u8* sBDAddr, u8* sLinkKey)
:	CBTHCICommand(OP_CODE_WRITE_STORED_LINK_KEY)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(CBTHCIWriteStoredLinkKeyCommand);
	memcpy (BDAddr, sBDAddr, BT_BD_ADDR_SIZE);
	memcpy (LinkKey, sLinkKey, BT_MAX_LINK_KEY_SIZE);
	NumKeysToWrite = 1;
}

CBTHCIWriteLocalNameCommand::CBTHCIWriteLocalNameCommand(void)
:	CBTHCICommand(OP_CODE_WRITE_LOCAL_NAME)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(CBTHCIWriteLocalNameCommand);
}

CBTHCIWriteLocalNameCommand::CBTHCIWriteLocalNameCommand(u8* sLocalName)
:	CBTHCICommand(OP_CODE_WRITE_LOCAL_NAME)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(CBTHCIWriteLocalNameCommand);
	memcpy (LocalName, sLocalName, sizeof LocalName);
}

CBTHCIWriteScanEnableCommand::CBTHCIWriteScanEnableCommand(void)
:	CBTHCICommand(OP_CODE_WRITE_SCAN_ENABLE)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(CBTHCIWriteScanEnableCommand);
}

CBTHCIWriteScanEnableCommand::CBTHCIWriteScanEnableCommand(u8 nScanEnable)
:	CBTHCICommand(OP_CODE_WRITE_SCAN_ENABLE),
	ScanEnable(nScanEnable)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(CBTHCIWriteScanEnableCommand);
}

CBTHCIWriteClassOfDeviceCommand::CBTHCIWriteClassOfDeviceCommand(void)
:	CBTHCICommand(OP_CODE_WRITE_CLASS_OF_DEVICE)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(CBTHCIWriteClassOfDeviceCommand);
}

CBTHCIWriteClassOfDeviceCommand::CBTHCIWriteClassOfDeviceCommand(
	TBTCOD sClassOfDevice)
:	CBTHCICommand(OP_CODE_WRITE_CLASS_OF_DEVICE)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(CBTHCIWriteClassOfDeviceCommand);
	ClassOfDevice = sClassOfDevice;
}

////////////////////////////////////////////////////////////////////////////////
//
// Vendor Specific Commands
//
////////////////////////////////////////////////////////////////////////////////

CBTHCIBcmVendorCommand::CBTHCIBcmVendorCommand(
	u16 nOpCode, u8 nLength, u8* pData, unsigned* pOffset)
:	CBTHCICommand(nOpCode)
{
	unsigned offset = *pOffset;
	ParameterTotalLength = nLength;

	for (unsigned i = 0; i < nLength; i++) {
		Data[i] = pData[offset++];
	}
    *pOffset = offset;
}

////////////////////////////////////////////////////////////////////////////////
//
// L2CAP Commands
//
////////////////////////////////////////////////////////////////////////////////

CBTHCIInquiryCommand::CBTHCIInquiryCommand(void)
:	CBTHCICommand(OP_CODE_INQUIRY)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(CBTHCIInquiryCommand);
}

CBTHCIInquiryCommand::CBTHCIInquiryCommand(u8 nInquiryLength)
:	CBTHCICommand(OP_CODE_INQUIRY),
	InquiryLength(nInquiryLength)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(CBTHCIInquiryCommand);
	LAP[0] = INQUIRY_LAP_GIAC       & 0xFF;
	LAP[1] = INQUIRY_LAP_GIAC >> 8  & 0xFF;
	LAP[2] = INQUIRY_LAP_GIAC >> 16 & 0xFF;
	NumResponses = INQUIRY_NUM_RESPONSES_UNLIMITED;
}

CBTHCIPeriodicInquiryModeCommand::CBTHCIPeriodicInquiryModeCommand(void)
:	CBTHCICommand(OP_CODE_PERIODIC_INQUIRY_MODE)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(CBTHCIPeriodicInquiryModeCommand);
}

CBTHCIPeriodicInquiryModeCommand::CBTHCIPeriodicInquiryModeCommand(
	u8 nInquiryLength)
:	CBTHCICommand(OP_CODE_PERIODIC_INQUIRY_MODE),
	InquiryLength(nInquiryLength)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(CBTHCIPeriodicInquiryModeCommand);
}

CBTHCICreateConnectionCommand::CBTHCICreateConnectionCommand(void)
:	CBTHCICommand(OP_CODE_CREATE_CONNECTION)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(CBTHCICreateConnectionCommand);
}

CBTHCICreateConnectionCommand::CBTHCICreateConnectionCommand(
	u8* sBDAddr, u8 nPageScanRepetitionMode)
:	CBTHCICommand(OP_CODE_CREATE_CONNECTION),
	PageScanRepetitionMode(nPageScanRepetitionMode)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(CBTHCICreateConnectionCommand);
	memcpy (BDAddr, sBDAddr, BT_BD_ADDR_SIZE);
	PacketType = PACKET_TYPE_DM1;
	PageScanMode = MANDATORY_PAGE_SCAN_MODE;
	ClockOffset = CLOCK_OFFSET_INVALID;
	AllowRoleSwitch = DISALLOW_ROLE_SWITCH;
}

CBTHCIDisconnectCommand::CBTHCIDisconnectCommand(void)
:	CBTHCICommand(OP_CODE_DISCONNECT)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(CBTHCIDisconnectCommand);
}

CBTHCIDisconnectCommand::CBTHCIDisconnectCommand(
	u16 nConnectionHandle, u8 nReason)
:	CBTHCICommand(OP_CODE_DISCONNECT),
	ConnectionHandle(nConnectionHandle),
	Reason(nReason)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(CBTHCIDisconnectCommand);
}

CBTHCIAcceptConnectionRequestCommand::CBTHCIAcceptConnectionRequestCommand(void)
:	CBTHCICommand(OP_CODE_ACCEPT_CONNECTION_REQUEST)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(
		CBTHCIAcceptConnectionRequestCommand);
}

CBTHCIAcceptConnectionRequestCommand::CBTHCIAcceptConnectionRequestCommand(
	u8* sBDAddr, u8 nRole)
:	CBTHCICommand(OP_CODE_ACCEPT_CONNECTION_REQUEST),
	Role(nRole)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(
		CBTHCIAcceptConnectionRequestCommand);
	memcpy (BDAddr, sBDAddr, BT_BD_ADDR_SIZE);
}

CBTHCIRejectConnectionRequestCommand::CBTHCIRejectConnectionRequestCommand(void)
:	CBTHCICommand(OP_CODE_REJECT_CONNECTION_REQUEST)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(
		CBTHCIRejectConnectionRequestCommand);
}

CBTHCIRejectConnectionRequestCommand::CBTHCIRejectConnectionRequestCommand(
	u8* sBDAddr, u8 nReason)
:	CBTHCICommand(OP_CODE_REJECT_CONNECTION_REQUEST),
	Reason(nReason)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(
		CBTHCIRejectConnectionRequestCommand);
	memcpy (BDAddr, sBDAddr, BT_BD_ADDR_SIZE);
}

CBTHCILinkKeyRequestReplyCommand::CBTHCILinkKeyRequestReplyCommand(void)
:	CBTHCICommand(OP_CODE_LINK_KEY_REQUEST_REPLY)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(
		CBTHCILinkKeyRequestReplyCommand);
}

CBTHCILinkKeyRequestReplyCommand::CBTHCILinkKeyRequestReplyCommand(
	u8* sBDAddr, u8* sLinkKey)
:	CBTHCICommand(OP_CODE_LINK_KEY_REQUEST_REPLY)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(
		CBTHCILinkKeyRequestReplyCommand);
	memcpy (BDAddr, sBDAddr, BT_BD_ADDR_SIZE);
	memcpy (LinkKey, sLinkKey, BT_MAX_LINK_KEY_SIZE);
}

CBTHCILinkKeyRequestNegativeReplyCommand
	::CBTHCILinkKeyRequestNegativeReplyCommand(void)
:	CBTHCICommand(OP_CODE_LINK_KEY_REQUEST_NEGATIVE_REPLY)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(
		CBTHCILinkKeyRequestNegativeReplyCommand);
}

CBTHCILinkKeyRequestNegativeReplyCommand
	::CBTHCILinkKeyRequestNegativeReplyCommand(u8* sBDAddr)
:	CBTHCICommand(OP_CODE_LINK_KEY_REQUEST_NEGATIVE_REPLY)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(
		CBTHCILinkKeyRequestNegativeReplyCommand);
	memcpy (BDAddr, sBDAddr, BT_BD_ADDR_SIZE);
}

CBTHCIPINCodeRequestReplyCommand::CBTHCIPINCodeRequestReplyCommand(void)
:	CBTHCICommand(OP_CODE_PIN_CODE_REQUEST_REPLY)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(
		CBTHCIPINCodeRequestReplyCommand);
}

CBTHCIPINCodeRequestReplyCommand::CBTHCIPINCodeRequestReplyCommand(
	u8* sBDAddr, u8 nLength, u8* sPINCode)
:	CBTHCICommand(OP_CODE_PIN_CODE_REQUEST_REPLY),
	PINCodeLength(nLength)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(
		CBTHCIPINCodeRequestReplyCommand);
	memcpy (BDAddr, sBDAddr, BT_BD_ADDR_SIZE);
	memset (PINCode, 0, BT_MAX_PIN_CODE_SIZE);
	memcpy (PINCode, sPINCode, nLength);
}

CBTHCIPINCodeRequestNegativeReplyCommand
	::CBTHCIPINCodeRequestNegativeReplyCommand(void)
:	CBTHCICommand(OP_CODE_PIN_CODE_REQUEST_NEGATIVE_REPLY)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(
		CBTHCIPINCodeRequestNegativeReplyCommand);
}

CBTHCIPINCodeRequestNegativeReplyCommand
	::CBTHCIPINCodeRequestNegativeReplyCommand(u8* sBDAddr)
:	CBTHCICommand(OP_CODE_PIN_CODE_REQUEST_NEGATIVE_REPLY)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(
		CBTHCIPINCodeRequestNegativeReplyCommand);
	memcpy (BDAddr, sBDAddr, BT_BD_ADDR_SIZE);
}

CBTHCIAuthenticationRequestedCommand::CBTHCIAuthenticationRequestedCommand(void)
:	CBTHCICommand(OP_CODE_AUTHENTICATION_REQUESTED)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(CBTHCIAuthenticationRequestedCommand);
}

CBTHCIAuthenticationRequestedCommand::CBTHCIAuthenticationRequestedCommand(
	u16 nConnectionHandle)
:	CBTHCICommand(OP_CODE_AUTHENTICATION_REQUESTED)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(CBTHCIAuthenticationRequestedCommand);
	ConnectionHandle = nConnectionHandle;
}

CBTHCIRemoteNameRequestCommand::CBTHCIRemoteNameRequestCommand(void)
:	CBTHCICommand(OP_CODE_REMOTE_NAME_REQUEST)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(CBTHCIRemoteNameRequestCommand);
}

CBTHCIRemoteNameRequestCommand::CBTHCIRemoteNameRequestCommand(
	u8* sBDAddr,
	u8 nPageScanRepetitionMode)
:	CBTHCICommand(OP_CODE_REMOTE_NAME_REQUEST),
	PageScanRepetitionMode(nPageScanRepetitionMode)
{
	ParameterTotalLength = PARAM_TOTAL_LEN(CBTHCIRemoteNameRequestCommand);
	memcpy (BDAddr, sBDAddr, BT_BD_ADDR_SIZE);
	Reserved = 0;
	ClockOffset = CLOCK_OFFSET_INVALID;
}

CBTHCIReadRemoteSupportedFeaturesCommand::CBTHCIReadRemoteSupportedFeaturesCommand(void)
:	CBTHCICommand(OP_CODE_READ_REMOTE_SUPPORTED_FEATURES)
{
	ParameterTotalLength=PARAM_TOTAL_LEN(CBTHCIReadRemoteSupportedFeaturesCommand);
}

CBTHCIReadRemoteSupportedFeaturesCommand::CBTHCIReadRemoteSupportedFeaturesCommand(
	u16 nConnectionHandle)
:	CBTHCICommand(OP_CODE_READ_REMOTE_SUPPORTED_FEATURES),
	ConnectionHandle(nConnectionHandle)
{
	ParameterTotalLength=PARAM_TOTAL_LEN(CBTHCIReadRemoteSupportedFeaturesCommand);
}

CBTHCIReadRemoteVersionInformationCommand::CBTHCIReadRemoteVersionInformationCommand(void)
:	CBTHCICommand(OP_CODE_READ_REMOTE_VERSION_INFORMATION)
{
	ParameterTotalLength=PARAM_TOTAL_LEN(CBTHCIReadRemoteVersionInformationCommand);
}

CBTHCIReadRemoteVersionInformationCommand::CBTHCIReadRemoteVersionInformationCommand(
	u16 nConnectionHandle)
:	CBTHCICommand(OP_CODE_READ_REMOTE_VERSION_INFORMATION),
	ConnectionHandle(nConnectionHandle)
{
	ParameterTotalLength=PARAM_TOTAL_LEN(CBTHCIReadRemoteVersionInformationCommand);
}

CBTHCILPHoldModeCommand::CBTHCILPHoldModeCommand(void)
:	CBTHCICommand(OP_CODE_HOLD_MODE)
{
	ParameterTotalLength=PARAM_TOTAL_LEN(CBTHCILPHoldModeCommand);
}

CBTHCILPHoldModeCommand::CBTHCILPHoldModeCommand(
	u16 nConnectionHandle,
	u16 nMaxInterval,
	u16 nMinInterval)
:	CBTHCICommand(OP_CODE_HOLD_MODE),
	ConnectionHandle(nConnectionHandle),
	HoldModeMaxInterval(nMaxInterval),
	HoldModeMinInterval(nMinInterval)
{
	ParameterTotalLength=PARAM_TOTAL_LEN(CBTHCILPHoldModeCommand);
}

CBTHCILPSniffModeCommand::CBTHCILPSniffModeCommand(void)
:	CBTHCICommand(OP_CODE_SNIFF_MODE)
{
	ParameterTotalLength=PARAM_TOTAL_LEN(CBTHCILPSniffModeCommand);
}

CBTHCILPSniffModeCommand::CBTHCILPSniffModeCommand(
	u16 nConnectionHandle,
	u16 nMaxInterval,
	u16 nMinInterval,
	u16 nAttempt,
	u16 nTimeout)
:	CBTHCICommand(OP_CODE_SNIFF_MODE),
	ConnectionHandle(nConnectionHandle),
	SniffMaxInterval(nMaxInterval),
	SniffMinInterval(nMinInterval),
	SniffAttempt(nAttempt),
	SniffTimeout(nTimeout)
{
	ParameterTotalLength=PARAM_TOTAL_LEN(CBTHCILPSniffModeCommand);
}

CBTHCILPExitSniffModeCommand::CBTHCILPExitSniffModeCommand(void)
:	CBTHCICommand(OP_CODE_EXIT_SNIFF_MODE)
{
	ParameterTotalLength=PARAM_TOTAL_LEN(CBTHCILPExitSniffModeCommand);
}

CBTHCILPExitSniffModeCommand::CBTHCILPExitSniffModeCommand(
	u16 nConnectionHandle)
:	CBTHCICommand(OP_CODE_EXIT_SNIFF_MODE),
	ConnectionHandle(nConnectionHandle)
{
	ParameterTotalLength=PARAM_TOTAL_LEN(CBTHCILPExitSniffModeCommand);
}

CBTHCILPParkModeCommand::CBTHCILPParkModeCommand(void)
:	CBTHCICommand(OP_CODE_PARK_MODE)
{
	ParameterTotalLength=PARAM_TOTAL_LEN(CBTHCILPParkModeCommand);
}

CBTHCILPParkModeCommand::CBTHCILPParkModeCommand(
	u16 nConnectionHandle,
	u16 nMaxInterval,
	u16 nMinInterval)
:	CBTHCICommand(OP_CODE_PARK_MODE),
	ConnectionHandle(nConnectionHandle),
	BeaconMaxInterval(nMaxInterval),
	BeaconMinInterval(nMinInterval)
{
	ParameterTotalLength=PARAM_TOTAL_LEN(CBTHCILPParkModeCommand);
}

CBTHCILPExitParkModeCommand::CBTHCILPExitParkModeCommand(void)
:	CBTHCICommand(OP_CODE_EXIT_PARK_MODE)
{
	ParameterTotalLength=PARAM_TOTAL_LEN(CBTHCILPExitParkModeCommand);
}

CBTHCILPExitParkModeCommand::CBTHCILPExitParkModeCommand(u16 nConnectionHandle)
:	CBTHCICommand(OP_CODE_EXIT_PARK_MODE),
	ConnectionHandle(nConnectionHandle)
{
	ParameterTotalLength=PARAM_TOTAL_LEN(CBTHCILPExitParkModeCommand);
}
