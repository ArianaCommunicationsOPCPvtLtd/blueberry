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
** Company:        Ariana Communications (OPC) Private Limited
** Copyright (C) 2020 Ariana Communications - www.ariana-communications.com
**
** Description:    Raspberry Pi BareMetal Bluetooth Error Header
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
#ifndef _bt_error_h
#define _bt_error_h

////////////////////////////////////////////////////////////////////////////////
//
// Error codes
//
////////////////////////////////////////////////////////////////////////////////

#define BT_STATUS_SUCCESS                                                   0x00
#define BT_ERROR_UNKNOWN_HCI_COMMAND                                        0x01
#define BT_ERROR_NO_CONNECTION                                              0x02
#define BT_ERROR_HARDWARE_FAILURE                                           0x03
#define BT_ERROR_PAGE_TIMEOUT                                               0x04
#define BT_ERROR_AUTHENTICATION_FAILURE                                     0x05
#define BT_ERROR_KEY_MISSING                                                0x06
#define BT_ERROR_MEMORY_FULL                                                0x07
#define BT_ERROR_CONNECTION_TIMEOUT                                         0x08
#define BT_ERROR_MAX_NUMBER_OF_CONNECTIONS                                  0x09
#define BT_ERROR_MAX_NUMBER_OF_SCO_CONNECTIONS_TO_A_DEVICE                  0x0A
#define BT_ERROR_ACL_CONNECTION_ALREADY_EXISTS                              0x0B
#define BT_ERROR_COMMAND_DISALLOWED                                         0x0C
#define BT_ERROR_HOST_REJECTED_DUE_TO_LIMITED_RESOURCES                     0x0D
#define BT_ERROR_HOST_REJECTED_DUE_TO_SECURITY_REASONS                      0x0E
#define BT_ERROR_HOST_REJECTED_DUE_TO_REMOTE_DEVICE_IS_ONLY_PERSONAL_DEVICE 0x0F
#define BT_ERROR_HOST_TIMEOUT                                               0x10
#define BT_ERROR_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE                     0x11
#define BT_ERROR_INVALID_HCI_COMMAND_PARAMETERS                             0x12
#define BT_ERROR_OTHER_END_TERMINATED_CONNECTION_USER_ENDED_CONNECTION      0x13
#define BT_ERROR_OTHER_END_TERMINATED_CONNECTION_LOW_RESOURCES              0x14
#define BT_ERROR_OTHER_END_TERMINATED_CONNECTION_ABOUT_TO_POWER_OFF         0x15
#define BT_ERROR_CONNECTION_TERMINATED_BY_LOCAL_HOST                        0x16
#define BT_ERROR_REPEATED_ATTEMPTS                                          0x17
#define BT_ERROR_PAIRING_NOT_ALLOWED                                        0x18
#define BT_ERROR_UNKNOWN_LMP_PDU                                            0x19
#define BT_ERROR_UNSUPPORTED_REMOTE_FEATURE                                 0x1A
#define BT_ERROR_SCO_OFFSET_REJECTED                                        0x1B
#define BT_ERROR_SCO_INTERVAL_REJECTED                                      0x1C
#define BT_ERROR_SCO_AIR_MODE_REJECTED                                      0x1D
#define BT_ERROR_INVALID_LMP_PARAMETERS                                     0x1E
#define BT_ERROR_UNSPECIFIED_ERROR                                          0x1F
#define BT_ERROR_UNSUPPORTED_LMP_PARAMETER_VALUE                            0x20
#define BT_ERROR_ROLE_CHANGE_NOT_ALLOWED                                    0x21
#define BT_ERROR_LMP_RESPONSE_TIMEOUT                                       0x22
#define BT_ERROR_LMP_ERROR_TRANSACTION_COLLISION                            0x23
#define BT_ERROR_LMP_PDU_NOT_ALLOWED                                        0x24

#endif
