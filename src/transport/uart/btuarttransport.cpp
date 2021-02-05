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
** Description:    Raspberry Pi BareMetal Bluetooth UART Transport Routines
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
#include <bluetooth/btuarttransport.h>
#include <bluetooth/devicenameservice.h>
#include <bluetooth/btevent.h>
#include <bluetooth/btdata.h>
#include <memio.h>
#include <platform/bt_clock.h>
#include <platform/bt_uart.h>
#include <logger.h>
#include <synchronize.h>
#include <assert.h>
#include <string.h>

enum TBTUARTRxState
{
	RxStateStart,
	RxStateCommand,
	RxStateACLData,
	RxStateLength,
	RxStateACLDataLength,
	RxStateParam,
	RxStateData,
	RxStateUnknown
};

static const char FromBTUART[] = "btuart";

CBTUARTTransport::CBTUARTTransport (TInterruptSystem *pInterruptSystem)
:	m_TxDPin (32, GPIOModeAlternateFunction3),
	m_RxDPin (33, GPIOModeAlternateFunction3),
	m_pInterruptSystem (pInterruptSystem),
	m_bIRQConnected (FALSE),
	m_pEventHandler (0),
	m_nRxState (RxStateStart)
{
}

CBTUARTTransport::~CBTUARTTransport (void)
{
	write32 (ARM_UART0_IMSC, 0);
	write32 (ARM_UART0_CR, 0);

	m_pEventHandler = 0;

	if (m_bIRQConnected) {
		assert (m_pInterruptSystem != 0);
		InterruptSystemDisconnectIRQ (m_pInterruptSystem, ARM_IRQ_UART);
	}

	m_pInterruptSystem = 0;
}

boolean CBTUARTTransport::Initialize (unsigned nBaudrate)
{
	unsigned nClockRate = BT_GetUartClockRate();
	assert (nClockRate > 0);

	assert (300 <= nBaudrate && nBaudrate <= 3000000);
	unsigned nBaud16 = nBaudrate * 16;
	unsigned nIntDiv = nClockRate / nBaud16;
	assert (1 <= nIntDiv && nIntDiv <= 0xFFFF);
	unsigned nFractDiv2 = (nClockRate % nBaud16) * 8 / nBaudrate;
	unsigned nFractDiv = nFractDiv2 / 2 + nFractDiv2 % 2;
	assert (nFractDiv <= 0x3F);

	m_nRxInPtr = m_nRxOutPtr = 0;

	assert (m_pInterruptSystem != 0);
	InterruptSystemConnectIRQ(m_pInterruptSystem,ARM_IRQ_UART,IRQStub,this);

	m_bIRQConnected = TRUE;


	write32 (ARM_UART0_IMSC, 0);
	write32 (ARM_UART0_ICR,  0x7FF);
	write32 (ARM_UART0_IBRD, nIntDiv);
	write32 (ARM_UART0_FBRD, nFractDiv);
	write32 (ARM_UART0_IFLS, IFLS_IFSEL_1_4 << IFLS_RXIFSEL_SHIFT);
	write32 (ARM_UART0_LCRH, LCRH_WLEN8_MASK | LCRH_FEN_MASK);		// 8N1
	write32 (ARM_UART0_CR,   CR_UART_EN_MASK | CR_TXE_MASK | CR_RXE_MASK);
	write32 (ARM_UART0_IMSC, INT_RX | INT_RT | INT_OE);


	CDeviceNameService::Get ()->AddDevice ("ttyBT1", this, FALSE);

	return TRUE;
}

boolean CBTUARTTransport::SendHCICommand (const void *pBuffer, unsigned nLength)
{
	u8 *pChar = (u8 *) pBuffer;

	Write (HCI_PACKET_COMMAND);

	assert (pChar != 0);
	
	while (nLength--) {
		Write (*pChar++);
	}


	return TRUE;
}

boolean CBTUARTTransport::SendHCIData (const void *pBuffer, unsigned nLength)
{
	u8 *pChar = (u8 *) pBuffer;

	Write (HCI_PACKET_ACL_DATA);

	assert (pChar != 0);
	
	while (nLength--) {
		Write (*pChar++);
	}


	return TRUE;
}

void CBTUARTTransport::RegisterHCIEventHandler (TBTHCIEventHandler *pHandler)
{
	assert (m_pEventHandler == 0);
	m_pEventHandler = pHandler;
	assert (m_pEventHandler != 0);
}

void CBTUARTTransport::RegisterHCIDataHandler (TBTHCIDataHandler *pHandler)
{
	assert (m_pDataHandler == 0);
	m_pDataHandler = pHandler;
	assert (m_pDataHandler != 0);
}

void CBTUARTTransport::Write (u8 nChar)
{
	while (read32 (ARM_UART0_FR) & FR_TXFF_MASK) {
		// do nothing
	}
		
	write32 (ARM_UART0_DR, nChar);
}

void CBTUARTTransport::IRQHandler (void)
{
	static bool aclFlag = false;

	volatile u32 nMIS = read32 (ARM_UART0_MIS);
	if (nMIS & INT_OE) {
		LOG_DEBUG ("Overrun error\r\n");
	}

	write32 (ARM_UART0_ICR, nMIS);

	while (1) {
		if (read32 (ARM_UART0_FR) & FR_RXFE_MASK) {
			break;
		}

		volatile u8 nData = read32 (ARM_UART0_DR) & 0xFF;

		switch (m_nRxState) {
		case RxStateStart:
			if (nData == HCI_PACKET_EVENT) {
				m_nRxInPtr = 0;
				m_nRxState = RxStateCommand;
			} else if (nData == HCI_PACKET_ACL_DATA) {
				m_nRxInPtr = 0;
				m_nRxState = RxStateACLData;
			}
			break;

		case RxStateCommand:
			m_RxBuffer[m_nRxInPtr++] = nData;
			m_nRxState = RxStateLength;
			break;

		case RxStateACLData:
			m_nRxState = (m_nRxInPtr) ? RxStateACLDataLength : RxStateACLData;
			m_RxBuffer[m_nRxInPtr++] = nData;
			break;

		case RxStateLength:
			m_RxBuffer[m_nRxInPtr++] = nData;
			if (nData > 0) {
				m_nRxParamLength = nData;
				m_nRxState = RxStateParam;
			} else {
				if (m_pEventHandler != 0) {
					(*m_pEventHandler) (m_RxBuffer, m_nRxInPtr);
				}

				m_nRxState = RxStateStart;
			}
			break;

		case RxStateACLDataLength:
			m_RxBuffer[m_nRxInPtr++] = nData;
			if (aclFlag) m_nRxACLDataLength |= (nData << 8);
			else m_nRxACLDataLength = nData;
			if (aclFlag) {
				if (m_nRxACLDataLength > BT_MAX_DATA_SIZE-4) {
					m_nRxInPtr = 0;
					m_nRxState = RxStateStart;
				} else if (m_nRxACLDataLength > 0) {
					m_nRxParamLength = m_nRxACLDataLength;
					m_nRxState = RxStateData;
				} else {
					if (m_pDataHandler != 0) {
						(*m_pDataHandler) (m_RxBuffer, m_nRxInPtr);
					}

					m_nRxState = RxStateStart;
				}
			}
			aclFlag = (aclFlag) ? false : true;
			break;

		case RxStateParam:
			m_RxBuffer[m_nRxInPtr++] = nData;
			if (--m_nRxParamLength == 0) {
				if (m_pEventHandler != 0) {
					(*m_pEventHandler) (m_RxBuffer, m_nRxInPtr);
				}

				m_nRxState = RxStateStart;
			}
			break;

		case RxStateData:
			m_RxBuffer[m_nRxInPtr++] = nData;
			if (--m_nRxParamLength == 0) {
				if (m_pDataHandler != 0) {
					(*m_pDataHandler) (m_RxBuffer, m_nRxInPtr);
				}

				m_nRxState = RxStateStart;
			}
			break;

		default:
//			assert (0);
			break;
		}
	}

}

void CBTUARTTransport::IRQStub (void *pParam)
{
	CBTUARTTransport *pThis = (CBTUARTTransport *) pParam;

	if (pThis) pThis->IRQHandler ();
}
