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
** Description:    Raspberry Pi BareMetal Bluetooth Command Header
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
#ifndef _bt_command_h
#define _bt_command_h

#include <macros.h>
#include <types.h>
#include <bluetooth/bterror.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/btdevicemanager.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////
//
// HCI
//
////////////////////////////////////////////////////////////////////////////////

// HCI Commands

class CBTHCICommand
{
	u16	OpCode;
#define OGF_LINK_CONTROL		(1 << 10)
	#define OP_CODE_INQUIRY						(OGF_LINK_CONTROL | 0x001)
	#define OP_CODE_INQUIRY_CANCEL				(OGF_LINK_CONTROL | 0x002)
	#define OP_CODE_PERIODIC_INQUIRY_MODE		(OGF_LINK_CONTROL | 0x003)
	#define OP_CODE_EXIT_PERIODIC_INQUIRY_MODE	(OGF_LINK_CONTROL | 0x004)
	#define OP_CODE_CREATE_CONNECTION			(OGF_LINK_CONTROL | 0x005)
	#define OP_CODE_DISCONNECT					(OGF_LINK_CONTROL | 0x006)
	#define OP_CODE_ACCEPT_CONNECTION_REQUEST	(OGF_LINK_CONTROL | 0x009)
	#define OP_CODE_REJECT_CONNECTION_REQUEST	(OGF_LINK_CONTROL | 0x00A)
	#define OP_CODE_LINK_KEY_REQUEST_REPLY		(OGF_LINK_CONTROL | 0x00B)
	#define OP_CODE_LINK_KEY_REQUEST_NEGATIVE_REPLY	(OGF_LINK_CONTROL | 0x00C)
	#define OP_CODE_PIN_CODE_REQUEST_REPLY		(OGF_LINK_CONTROL | 0x00D)
	#define OP_CODE_PIN_CODE_REQUEST_NEGATIVE_REPLY	(OGF_LINK_CONTROL | 0x00E)
	#define OP_CODE_AUTHENTICATION_REQUESTED	(OGF_LINK_CONTROL | 0x011)
	#define OP_CODE_REMOTE_NAME_REQUEST	(OGF_LINK_CONTROL | 0x019)
	#define OP_CODE_READ_REMOTE_SUPPORTED_FEATURES	(OGF_LINK_CONTROL | 0x01B)
	#define OP_CODE_READ_REMOTE_VERSION_INFORMATION	(OGF_LINK_CONTROL | 0x01D)
#define OGF_LINK_POLICY	(0x02 << 10)
	#define OP_CODE_HOLD_MODE				(OGF_LINK_POLICY | 0x0001)
	#define OP_CODE_SNIFF_MODE				(OGF_LINK_POLICY | 0x0003)
	#define OP_CODE_EXIT_SNIFF_MODE			(OGF_LINK_POLICY | 0x0004)
	#define OP_CODE_PARK_MODE				(OGF_LINK_POLICY | 0x0005)
	#define OP_CODE_EXIT_PARK_MODE			(OGF_LINK_POLICY | 0x0006)
#define OGF_HCI_CONTROL_BASEBAND	(3 << 10)
	#define OP_CODE_RESET					(OGF_HCI_CONTROL_BASEBAND | 0x003)
	#define OP_CODE_READ_STORED_LINK_KEY	(OGF_HCI_CONTROL_BASEBAND | 0x00D)
	#define OP_CODE_WRITE_STORED_LINK_KEY	(OGF_HCI_CONTROL_BASEBAND | 0x011)
	#define OP_CODE_WRITE_LOCAL_NAME		(OGF_HCI_CONTROL_BASEBAND | 0x013)
	#define OP_CODE_WRITE_SCAN_ENABLE		(OGF_HCI_CONTROL_BASEBAND | 0x01A)
	#define OP_CODE_WRITE_CLASS_OF_DEVICE	(OGF_HCI_CONTROL_BASEBAND | 0x024)
#define OGF_INFORMATIONAL_COMMANDS	(4 << 10)
	#define OP_CODE_READ_BD_ADDR			(OGF_INFORMATIONAL_COMMANDS | 0x009)
#define OGF_VENDOR_COMMANDS		(0x3F << 10)
	#define OP_CODE_DOWNLOAD_MINIDRIVER	(OGF_VENDOR_COMMANDS | 0x02E)
	#define OP_CODE_WRITE_RAM		(OGF_VENDOR_COMMANDS | 0x04C)
	#define OP_CODE_LAUNCH_RAM		(OGF_VENDOR_COMMANDS | 0x04E)

	protected:
	u8	ParameterTotalLength;

	public:
	CBTHCICommand(u16 nOpCode);
}
PACKED;

#define PARAM_TOTAL_LEN(cmd)		(sizeof (cmd) - sizeof (CBTHCICommand))

class CBTHCIReadStoredLinkKeyCommand : public CBTHCICommand
{
	u8	BDAddr[BT_BD_ADDR_SIZE];
	u8	ReadAllFlag;
#define RETURN_LINK_KEY_FOR_SPECIFIED_BD_ADDR		0x00
#define RETURN_ALL_STORED_LINK_KEYS					0x01

	public:
	CBTHCIReadStoredLinkKeyCommand();
	CBTHCIReadStoredLinkKeyCommand(u8* sBDAddr);
}
PACKED;

class CBTHCIWriteStoredLinkKeyCommand : public CBTHCICommand
{
	u8	NumKeysToWrite;
	u8	BDAddr[BT_BD_ADDR_SIZE];
	u8	LinkKey[BT_MAX_LINK_KEY_SIZE];

	public:
	CBTHCIWriteStoredLinkKeyCommand();
	CBTHCIWriteStoredLinkKeyCommand(u8* sBDAddr, u8* sLinkKey);
}
PACKED;

class CBTHCIWriteLocalNameCommand : public CBTHCICommand
{
	u8	LocalName[BT_NAME_SIZE];

	public:
	CBTHCIWriteLocalNameCommand();
	CBTHCIWriteLocalNameCommand(u8* sLocalName);
}
PACKED;

class CBTHCIWriteScanEnableCommand : public CBTHCICommand
{
	u8	ScanEnable;
#define SCAN_ENABLE_NONE		0x00
#define SCAN_ENABLE_INQUIRY_ENABLED	0x01
#define SCAN_ENABLE_PAGE_ENABLED	0x02
#define SCAN_ENABLE_BOTH_ENABLED	0x03

	public:
	CBTHCIWriteScanEnableCommand();
	CBTHCIWriteScanEnableCommand(u8 nScanEnable);
}
PACKED;

class CBTHCIWriteClassOfDeviceCommand : public CBTHCICommand
{
	TBTCOD	ClassOfDevice;

	public:
	CBTHCIWriteClassOfDeviceCommand();
	CBTHCIWriteClassOfDeviceCommand(TBTCOD sClassOfDevice);
}
PACKED;

// Vencor Specific Commands

class CBTHCIBcmVendorCommand : public CBTHCICommand
{
	u8	Data[255];

	public:
	CBTHCIBcmVendorCommand(u16 nOpCode, u8 nLength, u8* pData, unsigned *);
}
PACKED;

// Link Control Commands

class CBTHCIInquiryCommand : public CBTHCICommand
{
	u8	LAP[BT_CLASS_SIZE];
#define INQUIRY_LAP_GIAC		0x9E8B33	// General Inquiry Access Code
#define INQUIRY_LAP_LIAC		0x9E8B00	// Limited Inquiry Access Code
	u8	InquiryLength;
#define INQUIRY_LENGTH_MIN		0x01		// 1.28s
#define INQUIRY_LENGTH_MAX		0x30		// 61.44s
#define INQUIRY_LENGTH(secs)		(((secs) * 100 + 64) / 128)
	u8	NumResponses;
#define INQUIRY_NUM_RESPONSES_UNLIMITED	0x00

	public:
	CBTHCIInquiryCommand();
	CBTHCIInquiryCommand(u8 nInquiryLength);
}
PACKED;

class CBTHCIPeriodicInquiryModeCommand : public CBTHCICommand
{
	u16	MaxPeriodLength;
	u16	MinPeriodLength;
	u8	LAP[BT_CLASS_SIZE];
#define INQUIRY_LAP_GIAC		0x9E8B33	// General Inquiry Access Code
#define INQUIRY_LAP_LIAC		0x9E8B00	// Limited Inquiry Access Code
	u8	InquiryLength;
#define INQUIRY_LENGTH_MIN		0x01		// 1.28s
#define INQUIRY_LENGTH_MAX		0x30		// 61.44s
#define INQUIRY_LENGTH(secs)		(((secs) * 100 + 64) / 128)
	u8	NumResponses;
#define INQUIRY_NUM_RESPONSES_UNLIMITED	0x00

	public:
	CBTHCIPeriodicInquiryModeCommand();
	CBTHCIPeriodicInquiryModeCommand(u8 nInquiryLength);
}
PACKED;

class CBTHCICreateConnectionCommand : public CBTHCICommand
{
	u8	BDAddr[BT_BD_ADDR_SIZE];
	u16	PacketType;
#define PACKET_TYPE_DM1						0x0008
#define PACKET_TYPE_DH1						0x0010
#define PACKET_TYPE_DM3						0x0400
#define PACKET_TYPE_DH3						0x0800
#define PACKET_TYPE_DM5						0x4000
#define PACKET_TYPE_DH5						0x8000
	u8	PageScanRepetitionMode;
#define PAGE_SCAN_REPETITION_R0		0x00
#define PAGE_SCAN_REPETITION_R1		0x01
#define PAGE_SCAN_REPETITION_R2		0x02
	u8	PageScanMode;
#define MANDATORY_PAGE_SCAN_MODE	0x00
#define PAGE_SCAN_MODE_I					0x01
#define PAGE_SCAN_MODE_II					0x02
#define PAGE_SCAN_MODE_III				0x03
	u16	ClockOffset;
#define CLOCK_OFFSET_INVALID			0					// bit 15 is not set
#define CLOCK_OFFSET_VALID				0x8000		// bit 15 is set
	u8	AllowRoleSwitch;
#define DISALLOW_ROLE_SWITCH			0x00
#define ALLOW_ROLE_SWITCH					0x01

	public:
	CBTHCICreateConnectionCommand();
	CBTHCICreateConnectionCommand(u8* sBDAddr, u8 nPageScanRepetitionMode);
}
PACKED;

class CBTHCIDisconnectCommand : public CBTHCICommand
{
	u16	ConnectionHandle;
	u8	Reason;

	public:
	CBTHCIDisconnectCommand();
	CBTHCIDisconnectCommand(u16 nConnectionHandle, u8 nReason);
}
PACKED;

class CBTHCIAcceptConnectionRequestCommand : public CBTHCICommand
{
	u8	BDAddr[BT_BD_ADDR_SIZE];
	u8	Role;
#define ROLE_MASTER	0x00
#define ROLE_SLAVE	0x01

	public:
	CBTHCIAcceptConnectionRequestCommand();
	CBTHCIAcceptConnectionRequestCommand(u8* sBDAddr, u8 nRole);
}
PACKED;

class CBTHCIRejectConnectionRequestCommand : public CBTHCICommand
{
	u8	BDAddr[BT_BD_ADDR_SIZE];
	u8	Reason;

	public:
	CBTHCIRejectConnectionRequestCommand();
	CBTHCIRejectConnectionRequestCommand(u8* sBDAddr, u8 nReason);
}
PACKED;

class CBTHCILinkKeyRequestReplyCommand : public CBTHCICommand
{
	u8	BDAddr[BT_BD_ADDR_SIZE];
	u8	LinkKey[BT_MAX_LINK_KEY_SIZE];

	public:
	CBTHCILinkKeyRequestReplyCommand();
	CBTHCILinkKeyRequestReplyCommand(u8* sBDAddr, u8* sLinkKey);
}
PACKED;

class CBTHCILinkKeyRequestNegativeReplyCommand : public CBTHCICommand
{
	u8	BDAddr[BT_BD_ADDR_SIZE];

	public:
	CBTHCILinkKeyRequestNegativeReplyCommand();
	CBTHCILinkKeyRequestNegativeReplyCommand(u8* nBDAddr);
}
PACKED;

class CBTHCIPINCodeRequestReplyCommand : public CBTHCICommand
{
	u8	BDAddr[BT_BD_ADDR_SIZE];
	u8	PINCodeLength;
	u8	PINCode[BT_MAX_PIN_CODE_SIZE];

	public:
	CBTHCIPINCodeRequestReplyCommand();
	CBTHCIPINCodeRequestReplyCommand(u8* sBDAddr, u8 nLength, u8* sPINCode);
}
PACKED;

class CBTHCIPINCodeRequestNegativeReplyCommand : public CBTHCICommand
{
	u8	BDAddr[BT_BD_ADDR_SIZE];

	public:
	CBTHCIPINCodeRequestNegativeReplyCommand();
	CBTHCIPINCodeRequestNegativeReplyCommand(u8* sBDAddr);
}
PACKED;

class CBTHCIAuthenticationRequestedCommand : public CBTHCICommand
{
	u16	ConnectionHandle;

	public:
	CBTHCIAuthenticationRequestedCommand();
	CBTHCIAuthenticationRequestedCommand(u16 nConnectionHandle);
}
PACKED;

class CBTHCIRemoteNameRequestCommand : public CBTHCICommand
{
	u8	BDAddr[BT_BD_ADDR_SIZE];
	u8	PageScanRepetitionMode;
//#define PAGE_SCAN_REPETITION_R0		0x00
//#define PAGE_SCAN_REPETITION_R1		0x01
//#define PAGE_SCAN_REPETITION_R2		0x02
	u8	Reserved;				// set to 0
	u16	ClockOffset;
//#define CLOCK_OFFSET_INVALID		0		// bit 15 is not set

	public:
	CBTHCIRemoteNameRequestCommand();
	CBTHCIRemoteNameRequestCommand(u8* sBDAddr, u8 nPageScanRepetitionMode);
}
PACKED;

class CBTHCIReadRemoteSupportedFeaturesCommand : public CBTHCICommand
{
	u16	ConnectionHandle;

	public:
	CBTHCIReadRemoteSupportedFeaturesCommand();
	CBTHCIReadRemoteSupportedFeaturesCommand(u16 nConnectionHandle);
}
PACKED;

class CBTHCIReadRemoteVersionInformationCommand : public CBTHCICommand
{
	u16	ConnectionHandle;

	public:
	CBTHCIReadRemoteVersionInformationCommand();
	CBTHCIReadRemoteVersionInformationCommand(u16 nConnectionHandle);
}
PACKED;

class CBTHCILPHoldModeCommand : public CBTHCICommand
{
	u16	ConnectionHandle;
	u16	HoldModeMaxInterval;
	u16	HoldModeMinInterval;

	public:
	CBTHCILPHoldModeCommand();
	CBTHCILPHoldModeCommand(u16, u16, u16);
}
PACKED;

class CBTHCILPSniffModeCommand : public CBTHCICommand
{
	u16	ConnectionHandle;
	u16	SniffMaxInterval;
	u16	SniffMinInterval;
	u16	SniffAttempt;
	u16	SniffTimeout;

	public:
	CBTHCILPSniffModeCommand();
	CBTHCILPSniffModeCommand(u16, u16, u16, u16, u16);
}
PACKED;

class CBTHCILPExitSniffModeCommand : public CBTHCICommand
{
	u16	ConnectionHandle;

	public:
	CBTHCILPExitSniffModeCommand();
	CBTHCILPExitSniffModeCommand(u16);
}
PACKED;

class CBTHCILPParkModeCommand : public CBTHCICommand
{
	u16	ConnectionHandle;
	u16	BeaconMaxInterval;
	u16	BeaconMinInterval;

	public:
	CBTHCILPParkModeCommand();
	CBTHCILPParkModeCommand(u16, u16, u16);
}
PACKED;

class CBTHCILPExitParkModeCommand : public CBTHCICommand
{
	u16	ConnectionHandle;

	public:
	CBTHCILPExitParkModeCommand();
	CBTHCILPExitParkModeCommand(u16);
}
PACKED;

#endif
