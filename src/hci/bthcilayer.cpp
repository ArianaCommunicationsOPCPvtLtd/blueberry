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
** Description:    Raspberry Pi BareMetal Bluetooth HCI Layer Routines
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
#include <bluetooth/bthcilayer.h>
#include <bluetooth/devicenameservice.h>
#include <bluetooth/btevent.h>
#include <bluetooth/btdata.h>
#include <logger.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static const char FromHCILayer[] = "bthci";

CBTHCILayer *CBTHCILayer::s_pThis = 0;

CBTHCILayer::CBTHCILayer (TBTCOD nClassOfDevice, const char *pLocalName)
:	m_pHCITransportUART (0),
#ifdef BTUSB
	m_pHCITransportUSB (0),
#endif
	m_DeviceManager (this, &m_DeviceEventQueue, nClassOfDevice, pLocalName),
	m_pEventBuffer (0),
	m_nEventLength (0),
	m_nEventFragmentOffset (0),
	m_pDataBuffer (0),
	m_nDataLength (0),
	m_nDataFragmentOffset (0),
	m_pBuffer (0),
	m_nCommandPackets (1),
	m_nDataPackets (1)
{
	assert (s_pThis == 0);
	s_pThis = this;
}

CBTHCILayer::~CBTHCILayer (void)
{
#ifdef BTUSB
	m_pHCITransportUSB = 0;
#endif
	m_pHCITransportUART = 0;

	free (m_pBuffer);
	m_pBuffer = 0;

	free (m_pEventBuffer);
	m_pEventBuffer = 0;

	free (m_pDataBuffer);
	m_pDataBuffer = 0;

	s_pThis = 0;
}

boolean CBTHCILayer::Initialize (void)
{
#ifdef BTUSB
	m_pHCITransportUSB =(CUSBBluetoothDevice *) 
		CDeviceNameService::Get ()->GetDevice ("ubt1", FALSE);
	if (m_pHCITransportUSB == 0) {
#endif
		m_pHCITransportUART = (CBTUARTTransport *) 
			CDeviceNameService::Get ()->GetDevice ("ttyBT1", FALSE);
		if (m_pHCITransportUART == 0) {
			LOG_DEBUG ("Bluetooth controller not found\r\n");
			return FALSE;
		}
#if BTUSB
	}
#endif

	m_pEventBuffer = (u8 *)malloc(BT_MAX_HCI_EVENT_SIZE);
	assert (m_pEventBuffer != 0);

	m_pDataBuffer = (u8 *)malloc(BT_MAX_DATA_SIZE);
	assert (m_pEventBuffer != 0);

	m_pBuffer = (u8 *)malloc(BT_MAX_DATA_SIZE);
	assert (m_pBuffer != 0);

#if BTUSB
	if (m_pHCITransportUSB != 0) {
		m_pHCITransportUSB->RegisterHCIEventHandler (EventStub);
		m_pHCITransportUSB->RegisterHCIDataHandler (DataStub);
	} else {
#endif
		assert (m_pHCITransportUART != 0);
		m_pHCITransportUART->RegisterHCIEventHandler (EventStub);
		m_pHCITransportUART->RegisterHCIDataHandler (DataStub);
#if BTUSB
	}
#endif

	return m_DeviceManager.Initialize ();
}

TBTTransportType CBTHCILayer::GetTransportType (void) const
{
#if BTUSB
	if (m_pHCITransportUSB != 0) return BTTransportTypeUSB;
#endif

	if (m_pHCITransportUART != 0) return BTTransportTypeUART;

	return BTTransportTypeUnknown;
}

void CBTHCILayer::Process (void)
{
#if BTUSB
	assert (m_pHCITransportUSB != 0 || m_pHCITransportUART != 0);
#endif
	assert (m_pBuffer != 0);

	unsigned nLength;

	// Send command
	while (   m_nCommandPackets > 0
	       && (nLength = m_CommandQueue.Dequeue (m_pBuffer)) > 0) {
#if BTUSB
		if (  m_pHCITransportUSB != 0
		    ? !m_pHCITransportUSB->SendHCICommand (m_pBuffer, nLength)
		    : !m_pHCITransportUART->SendHCICommand (m_pBuffer, nLength))
#else
		if ( !m_pHCITransportUART->SendHCICommand (m_pBuffer, nLength))
#endif
		{
			LOG_DEBUG ("HCI command dropped\r\n");
			break;
		}
		m_nCommandPackets--;
	}

	// Send Data
	while (   m_nDataPackets > 0
	       && (nLength = m_TxDataQueue.Dequeue (m_pBuffer)) > 0) {
#if BTUSB
		if (  m_pHCITransportUSB != 0
		    ? !m_pHCITransportUSB->SendHCIData (m_pBuffer, nLength)
		    : !m_pHCITransportUART->SendHCIData (m_pBuffer, nLength))
#else
		if ( !m_pHCITransportUART->SendHCIData (m_pBuffer, nLength))
#endif
		{
			LOG_DEBUG ("HCI data dropped\r\n");
			break;
		}
		m_nDataPackets--;
	}
	m_DeviceManager.Process ();
}

void CBTHCILayer::SendCommand (const void *pBuffer, unsigned nLength)
{
	m_CommandQueue.Enqueue (pBuffer, nLength);
}

void CBTHCILayer::SendData (const void *pBuffer, unsigned nLength)
{
	m_TxDataQueue.Enqueue (pBuffer, nLength);
}

boolean CBTHCILayer::ReceiveLinkEvent (void *pBuffer, unsigned *pResultLength)
{
	unsigned nLength = m_LinkEventQueue.Dequeue (pBuffer);
	if (nLength > 0) {
		assert (pResultLength != 0);
		*pResultLength = nLength;

		return TRUE;
	}

	return FALSE;
}

boolean CBTHCILayer::ReceiveData (void *pBuffer, unsigned *pResultLength)
{
	unsigned nLength = m_RxDataQueue.Dequeue (pBuffer);
	if (nLength > 0) {
		assert (pResultLength != 0);
		*pResultLength = nLength;

		return TRUE;
	}

	return FALSE;
}

void CBTHCILayer::SetCommandPackets (unsigned nCommandPackets)
{
	m_nCommandPackets += nCommandPackets;
}

void CBTHCILayer::SetDataPackets (unsigned nDataPackets)
{
	m_nDataPackets += nDataPackets;
}

CBTDeviceManager *CBTHCILayer::GetDeviceManager (void)
{
	return &m_DeviceManager;
}

void CBTHCILayer::EventHandler (const void *pBuffer, unsigned nLength)
{
	assert (pBuffer != 0);
	assert (nLength > 0);

	if (m_nEventFragmentOffset == 0) {
		if (nLength < sizeof (CBTHCIEvent)) {
			LOG_DEBUG ("Short event ignored\r\n");
			return;
		}

		CBTHCIEvent *pHeader = (CBTHCIEvent *) pBuffer;

		assert (m_nEventLength == 0);
		m_nEventLength = sizeof (CBTHCIEvent) + pHeader->ParameterTotalLength;
	}

	assert (m_pEventBuffer != 0);
	memcpy (m_pEventBuffer + m_nEventFragmentOffset, pBuffer, nLength);

	m_nEventFragmentOffset += nLength;
	if (m_nEventFragmentOffset < m_nEventLength) return;

	CBTHCIEvent *pHeader = (CBTHCIEvent *) m_pEventBuffer;
	switch (pHeader->EventCode) {
	case BT_EVENT_CODE_COMMAND_COMPLETE:
	case BT_EVENT_CODE_COMMAND_STATUS:
		m_DeviceEventQueue.Enqueue (m_pEventBuffer, m_nEventLength);
		break;

	default:
		m_LinkEventQueue.Enqueue (m_pEventBuffer, m_nEventLength);
		break;
	}

	m_nEventLength = 0;
	m_nEventFragmentOffset = 0;
}

void CBTHCILayer::EventStub (const void *pBuffer, unsigned nLength)
{
	assert (s_pThis != 0);
	s_pThis->EventHandler (pBuffer, nLength);
}

void CBTHCILayer::DataHandler (const void *pBuffer, unsigned nLength)
{
	assert (pBuffer != 0);
	assert (nLength > 0);

	if (m_nDataFragmentOffset == 0) {
		if (nLength < sizeof (CBTHCIACLData)) {
			LOG_DEBUG ("Short data ignored\r\n");
			return;
		}
		CBTHCIACLData *pHeader = (CBTHCIACLData *) pBuffer;

		assert (m_nDataLength == 0);
		assert (pHeader->DataTotalLength <= BT_MAX_DATA_SIZE);
		m_nDataLength = sizeof (CBTHCIACLData) + pHeader->DataTotalLength;
	}

	assert (m_pDataBuffer != 0);

	CBTHCIACLData *pHeader = (CBTHCIACLData *) m_pDataBuffer;
	switch (pHeader->PacketBoundaryFlag) {
	case BT_CONTINUING_FRAGMENT_PACKET:
		memcpy (m_pDataBuffer + m_nDataFragmentOffset,
			(u8*)pBuffer + sizeof(CBTHCIACLData),
			nLength - sizeof(CBTHCIACLData));
		m_nDataFragmentOffset += nLength - sizeof(CBTHCIACLData);
		break;

	case BT_FIRST_PACKET:
		memcpy (m_pDataBuffer + m_nDataFragmentOffset, pBuffer, nLength);
		m_nDataFragmentOffset += nLength;
		break;

	default:
		memcpy (m_pDataBuffer + m_nDataFragmentOffset, pBuffer, nLength);
		m_nDataFragmentOffset += nLength;
		break;
	}

	if (m_nDataFragmentOffset < m_nDataLength) return;

	m_RxDataQueue.Enqueue (m_pDataBuffer, m_nDataLength);

	m_nDataLength = 0;
	m_nDataFragmentOffset = 0;
}

void CBTHCILayer::DataStub (const void *pBuffer, unsigned nLength)
{
	assert (s_pThis != 0);
	s_pThis->DataHandler (pBuffer, nLength);
}
