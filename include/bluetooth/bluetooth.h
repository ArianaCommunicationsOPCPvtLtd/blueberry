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
** Description:    Raspberry Pi BareMetal Bluetooth Header
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
#ifndef _bt_bluetooth_h
#define _bt_bluetooth_h

#include <macros.h>
#include <types.h>

////////////////////////////////////////////////////////////////////////////////
//
// General definitions
//
////////////////////////////////////////////////////////////////////////////////

// Major Service Class

#define BT_SERVICE_CLASS_LDM                  0x001
#define BT_SERVICE_CLASS_POSITIONING          0x008
#define BT_SERVICE_CLASS_NETWORKING           0x010
#define BT_SERVICE_CLASS_RENDERING            0x020
#define BT_SERVICE_CLASS_CAPTURING            0x040
#define BT_SERVICE_CLASS_OBJECT_TRANSFER      0x080
#define BT_SERVICE_CLASS_AUDIO                0x100
#define BT_SERVICE_CLASS_TELEPHONY            0x200
#define BT_SERVICE_CLASS_INFORMATION          0x400

// Major Device Class

#define BT_DEVICE_COMPUTER                    0x01
#define BT_DEVICE_PHONE                       0x02
#define BT_DEVICE_LAN_NAP                     0x03
#define BT_DEVICE_AUDIO_VIDEO                 0x04
#define BT_DEVICE_PERIPHERAL                  0x05
#define BT_DEVICE_IMAGING                     0x06
#define BT_DEVICE_WEARABLE                    0x07
#define BT_DEVICE_TOY                         0x08
#define BT_DEVICE_HEALTH                      0x09
#define BT_DEVICE_UNCATEGORIZED               0x1F

// Class of Device

#define BT_CODE_COMPUTER          BT_DEVICE_COMPUTER
#define BT_CODE_DESKTOP           0x04
#define BT_CODE_SERVER            0x08
#define BT_CODE_LAPTOP            0x0C
#define BT_CODE_HANDHELD          0x10
#define BT_CODE_PALMTOP           0x14
#define BT_CODE_WEARABLE          0x18

#define BT_CODE_HID               BT_DEVICE_PERIPHERAL
#define BT_CODE_JOYSTICK          0x04
#define BT_CODE_KEYBOARD          0x40
#define BT_CODE_MOUSE             0x80
#define BT_CODE_COMBO             0xC0

#define BT_CLASS_DESKTOP_COMPUTER \
                                   { BT_CODE_DESKTOP, \
                                   BT_CODE_COMPUTER, \
                                   BT_SERVICE_CLASS_LDM \
                                  }
//#define BT_CLASS_JOYSTICK         ((BT_CODE_HID<<8) | BT_CODE_JOYSTICK)
#define BT_CLASS_JOYSTICK         \
                                   { BT_CODE_JOYSTICK, \
                                   BT_CODE_HID, \
                                   BT_SERVICE_CLASS_LDM \
                                  }
//#define BT_CLASS_KEYBOARD         ((BT_CODE_HID<<8) | BT_CODE_KEYBOARD)
#define BT_CLASS_KEYBOARD         \
                                   { BT_CODE_KEYBOARD, \
                                   BT_CODE_HID, \
                                   BT_SERVICE_CLASS_LDM \
                                  }
//#define BT_CLASS_MOUSE            ((BT_CODE_HID<<8) | BT_CODE_MOUSE)
#define BT_CLASS_MOUSE            \
                                   { BT_CODE_MOUSE, \
                                   BT_CODE_HID, \
                                   BT_SERVICE_CLASS_LDM \
                                  }

// Sizes

#define BT_MAX_HCI_EVENT_SIZE     257
#define BT_MAX_HCI_COMMAND_SIZE   258
#define BT_MAX_DATA_SIZE          BT_MAX_HCI_COMMAND_SIZE
#define BT_MAX_PIN_CODE_SIZE      16
#define BT_MAX_LINK_KEY_SIZE      16

#define BT_BD_ADDR_SIZE           6
#define BT_CLASS_SIZE             3
#define BT_DEVICE_NAME_SIZE       10
#define BT_NAME_SIZE              248
#define BT_LMP_FEATURE_SIZE       8

// Packet ID

#define HCI_PACKET_COMMAND        0x01
#define HCI_PACKET_ACL_DATA       0x02
#define HCI_PACKET_SYNCH_DATA     0x03
#define HCI_PACKET_EVENT          0x04
#define HCI_PACKET_L2CAP_DATA     0x06
#define HCI_PACKET_RFCOM_DATA     0x07

// PSM

#define BT_PSM_SERVICE_DISCOVERY_PROTOCOL 0x0001
#define BT_PSM_RFCOMM                     0x0003
#define BT_PSM_TELEPHONY_CONTROL_PROTOCOL 0x0005
#define BT_PSM_BNEP                       0x000F	// BNEP
#define BT_PSM_HID_CONTROL                0x0011
#define BT_PSM_HID_INTERRUPT              0x0013
#define BT_PSM_ATT                        0x001F
#define BT_PSM_IPSP                       0x0023

// Default values

#define BT_DEFAULT_PIN	"0000"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct t_bt_device_map {
    u8 address[BT_BD_ADDR_SIZE];
    u8 type[BT_CLASS_SIZE];
#define IS_HID(p)          ((p)->type[1] == BT_CODE_HID) 
#define IS_MOUSE(p)        ((p)->type[0] == BT_CODE_MOUSE) 
#define IS_JOYSTICK(p)     ((p)->type[0] == BT_CODE_JOYSTICK) 
#define IS_KEYBOARD(p)     ((p)->type[0] == BT_CODE_KEYBOARD) 
    struct t_bt_device_map *next;
} tBT_device_map;
typedef tBT_device_map* pBT_device_map;

typedef struct t_bt_device_descriptor {
    char  name[BT_DEVICE_NAME_SIZE];
    char  address[BT_BD_ADDR_SIZE];
    void* device;
    bool  (*read)(void* pdevice, void* pdata, unsigned length);
    bool  (*write)(void* pdevice, void* pdata, unsigned length);
    bool  (*event_handler)(void* pdevice, void* data, unsigned *plength);
    bool  (*close)(void* pdevice);
} tBT_device_descriptor;
typedef tBT_device_descriptor* pBT_device_descriptor;

extern void* BT_Init(void*);
extern pBT_device_map BT_Listen(void*, unsigned);
extern pBT_device_map BT_Check(void*);
extern void BT_Free(pBT_device_map);
extern u8* BT_Find(pBT_device_map);
extern bool BT_GetEvent(void*, void*, unsigned *);
#ifdef __cplusplus
}
#endif

#endif
