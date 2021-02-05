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
** Description:    Raspberry Pi BareMetal Bluetooth UART Transport Header
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
#ifndef _bt_btuarttransport_h
#define _bt_btuarttransport_h

#include <bluetooth/device.h>
#include <bluetooth/bttransportlayer.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/gpiopin.h>
#include <platform/rpi/rpi-interrupt-system.h>
#include <types.h>
#include <stdlib.h>

#define BT_UART_BUFFER_SIZE (BT_MAX_HCI_EVENT_SIZE)

class CBTUARTTransport : public CDevice
{
public:
	CBTUARTTransport (TInterruptSystem *pInterruptSystem);
	~CBTUARTTransport (void);

	boolean Initialize (unsigned nBaudrate = 115200);

	boolean SendHCICommand (const void *pBuffer, unsigned nLength);
	boolean SendHCIData (const void *pBuffer, unsigned nLength);

	void RegisterHCIEventHandler (TBTHCIEventHandler *pHandler);
	void RegisterHCIDataHandler (TBTHCIDataHandler *pHandler);
	void* operator new(size_t T) { return (void *)malloc(T); }
	void operator delete (void *ptr) { free(ptr); }

private:
	void Write (u8 nChar);

	void IRQHandler (void);
	static void IRQStub (void *pParam);

private:
	CGPIOPin m_GPIO14;
	CGPIOPin m_GPIO15;
	CGPIOPin m_TxDPin;
	CGPIOPin m_RxDPin;

	TInterruptSystem *m_pInterruptSystem;
	boolean m_bIRQConnected;

	TBTHCIEventHandler *m_pEventHandler;
	TBTHCIDataHandler *m_pDataHandler;

	u8 m_RxBuffer[BT_UART_BUFFER_SIZE];
	unsigned m_nRxState;
	unsigned m_nRxParamLength;
	u16 m_nRxACLDataLength;
	unsigned m_nRxInPtr;
	unsigned m_nRxOutPtr;
};

#endif
