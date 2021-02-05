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
** Description:    Raspberry Pi BareMetal Bluetooth HCI Layer Header
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
#ifndef _bt_hcilayer_h
#define _bt_hcilayer_h

#include <bluetooth/bttransportlayer.h>
//#include <usb/usbbluetooth.h>
#include <bluetooth/btuarttransport.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/btdevicemanager.h>
#include <bluetooth/btqueue.h>
#include <types.h>

#undef BTUSB

class CBTHCILayer
{
public:
	CBTHCILayer (TBTCOD nClassOfDevice, const char *pLocalName);
	~CBTHCILayer (void);

	boolean Initialize (void);

	TBTTransportType GetTransportType (void) const;

	void Process (void);

	void SendCommand (const void *pBuffer, unsigned nLength);
	void SendData (const void *pBuffer, unsigned nLength);

	// pBuffer must have size BT_MAX_HCI_EVENT_SIZE
	boolean ReceiveLinkEvent (void *pBuffer, unsigned *pResultLength);
	// pBuffer must have size BT_MAX_DATA_SIZE
	boolean ReceiveData (void *pBuffer, unsigned *pResultLength);

	void SetCommandPackets (unsigned nCommandPackets);	// set commands allowed to be sent
	void SetDataPackets (unsigned nDataPackets);	// set data allowed to be sent

	CBTDeviceManager *GetDeviceManager (void);

private:
	void EventHandler (const void *pBuffer, unsigned nLength);
	static void EventStub (const void *pBuffer, unsigned nLength);
	void DataHandler (const void *pBuffer, unsigned nLength);
	static void DataStub (const void *pBuffer, unsigned nLength);

private:
#ifdef BTUSB
	CUSBBluetoothDevice *m_pHCITransportUSB;
#endif
	CBTUARTTransport *m_pHCITransportUART;

	CBTDeviceManager m_DeviceManager;

	CBTQueue m_CommandQueue;
	CBTQueue m_DeviceEventQueue;
	CBTQueue m_LinkEventQueue;
	CBTQueue m_RxDataQueue;
	CBTQueue m_TxDataQueue;

	u8 *m_pEventBuffer;
	unsigned m_nEventLength;
	unsigned m_nEventFragmentOffset;

	u8 *m_pDataBuffer;
	unsigned m_nDataLength;
	unsigned m_nDataFragmentOffset;

	u8 *m_pBuffer;

	volatile unsigned m_nCommandPackets;		// commands allowed to be sent
	volatile unsigned m_nDataPackets;		// data allowed to be sent

	static CBTHCILayer *s_pThis;
};

#endif
