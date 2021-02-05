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
** Description:    Raspberry Pi BareMetal Bluetooth Device Header
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
#ifndef _bt_device_h
#define _bt_device_h

#include <types.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/btlogicallayer.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum 
{
	BT_DEVICE_IDLE,
	BT_DEVICE_CONNECTED,
	BT_DEVICE_CONNECTING,
	BT_DEVICE_RESERVED0,
	BT_DEVICE_RESERVED1,
	BT_DEVICE_RESERVED2,
	BT_DEVICE_RESERVED3,
	BT_DEVICE_RESERVED4,
	BT_DEVICE_RESERVED5,
	BT_DEVICE_RESERVED6,
	BT_DEVICE_RESERVED7
} TBTDeviceConnectionState;

class CBTDevice
{
protected:
	CBTConnection*				m_pConnection;
	TBTDeviceConnectionState	m_tState;
public:
	CBTDevice (void);
	CBTDevice (CBTConnection*);
	~CBTDevice (void);

	// Connect the device
	virtual u16 Connect (void);

	// Disconnect the device
	bool Disconnect (u8 nReason);		// byte offset

	// Remove the device
	bool RemoveDevice (void);

	// Type of device
	bool IsMouse (void);
	bool IsKeyboard (void);
	bool IsJoystick (void);
	bool IsComputer (void);
	bool IsConnecting (void);

	// Get params
	const u8 *GetBDAddress (void) const;
	const u8 *GetClassOfDevice (void) const;
	const u8 *GetRemoteName (void) const;
	u8 GetPageScanRepetitionMode (void) const;
	CBTConnection* GetConnection (void) const;

	// Set params
	void SetBDAddress (u8*);
	void SetClassOfDevice (u8*);
	void SetRemoteName (u8*);
	void SetPageScanRepetitionMode (u8);
	void SetState (TBTDeviceConnectionState);

	// Packet parser
	virtual void Parser(u8*, u16);

	void* operator new(size_t T) { return (void *)malloc(T); }
	void operator delete (void *ptr) { free(ptr); }
};

#endif
