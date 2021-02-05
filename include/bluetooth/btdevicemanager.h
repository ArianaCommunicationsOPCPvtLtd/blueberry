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
** Description:    Raspberry Pi BareMetal Bluetooth Device Manager Header
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
#ifndef _bt_btdevicemanager_h
#define _bt_btdevicemanager_h

#include <bluetooth/bluetooth.h>
#include <bluetooth/btqueue.h>
#include <bluetooth/btlayer.h>
#include <types.h>

enum TBTDeviceState
{
	BTDeviceStateResetPending,
	BTDeviceStateWriteRAMPending,
	BTDeviceStateLaunchRAMPending,
	BTDeviceStateReadBDAddrPending,
	BTDeviceStateWriteClassOfDevicePending,
	BTDeviceStateWriteLocalNamePending,
	BTDeviceStateWriteScanEnabledPending,
	BTDeviceStateRunning,
	BTDeviceStateFailed,
	BTDeviceStateUnknown
};

typedef u8 TBDAddr[BT_BD_ADDR_SIZE];

typedef enum {
	BT_BD_ADDR_TYPE_LE_PUBLIC = 0,
	BT_BD_ADDR_TYPE_LE_RANDOM = 1,
	BT_BD_ADDR_TYPE_LE_PRIVATE_FALLBACK_PUBLIC = 2,
	BT_BD_ADDR_TYPE_LE_PRIVATE_FALLBACK_RANDOM = 3,
	BT_BD_ADDR_TYPE_SCO = 0xfe,
	BT_BD_ADDR_TYPE_CLASSIC = 0xff,
	BT_BD_ADDR_TYPE_UNKNOWN = 0xfe
} TBTBDAddrType;

typedef enum {
	BT_MODE_ACTIVE = 0x00,
	BT_MODE_HOLD = 0x01,
	BT_MODE_SNIFF = 0x02,
	BT_MODE_PARK = 0x03
} TBTMode;

struct t_bt_cod {
    u8  MinorDeviceClass:8;
    u8  MajorDeviceClass:5;
    u16 ServiceClass:11;
} PACKED;
typedef struct t_bt_cod TBTCOD;

class CBTHCILayer;
class CBTConnection;

class CBTDeviceManager : public CBTLayer
{
public:
	CBTDeviceManager (CBTHCILayer *pHCILayer, CBTQueue *pEventQueue,
			  TBTCOD nClassOfDevice, const char *pLocalName);
	~CBTDeviceManager (void);

	boolean Initialize (void);

	void Process (void);

	void SetConnection (CBTConnection*);

	inline void SetState (TBTDeviceState eState) {m_State = eState;}
	inline bool CheckState (TBTDeviceState eState) {return (m_State == eState);}

	void SetHCICommandPackets (unsigned);

	u8*  GetBDAddr (void);

	void  SetBDAddr (u8*);

	boolean DeviceIsRunning (void) const;

	bool SendHCICommand (const void*, unsigned);

private:
	CBTHCILayer *m_pHCILayer;
	CBTQueue    *m_pEventQueue;
	TBTCOD	     m_nClassOfDevice;
	u8	     m_LocalName[BT_NAME_SIZE];
	CBTConnection    *m_pConnection;

	TBTDeviceState m_State;

	u8 m_LocalBDAddr[BT_BD_ADDR_SIZE];

	u8 *m_pBuffer;

	unsigned m_nFirmwareOffset;

	friend class CBTHCIEventCommandComplete;
};

#endif
