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
** Description:    Raspberry Pi BareMetal Bluetooth Sub System Routines
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
#include <bluetooth/btsubsystem.h>
#include <bluetooth/devicenameservice.h>
#include <bluetooth/btinquiryresults.h>
#include <bluetooth/btmouse.h>
#include <logger.h>
#include <assert.h>
#include <task.h>

CBTSubSystem::CBTSubSystem (TInterruptSystem *pInterruptSystem, TBTCOD nClassOfDevice, const char *pLocalName)
:	m_pInterruptSystem (pInterruptSystem),
	m_pUARTTransport (0),
	m_HCILayer (nClassOfDevice, pLocalName),
	m_LogicalLayer (&m_HCILayer),
	m_L2CAPLayer (&m_LogicalLayer, this),
	m_HIDPLayer (&m_L2CAPLayer)
{
}

CBTSubSystem::~CBTSubSystem (void)
{
	delete m_pUARTTransport;
	m_pUARTTransport = 0;
}

boolean CBTSubSystem::Initialize (void)
{
	// if USB transport not available, UART still free and this is a RPi 3B or Zero W:
	//	use UART transport
	if (   CDeviceNameService::Get ()->GetDevice ("ubt1", FALSE) == 0
	    && CDeviceNameService::Get ()->GetDevice ("ttyS1", FALSE) == 0)
	{
		assert (m_pUARTTransport == 0);
		assert (m_pInterruptSystem != 0);
		m_pUARTTransport = new CBTUARTTransport (m_pInterruptSystem);
		assert (m_pUARTTransport != 0);
		if (!m_pUARTTransport->Initialize ()) {
			return FALSE;
		}
	}

	if (!m_HCILayer.Initialize ()) {
		return FALSE;
	}

	if (!m_LogicalLayer.Initialize ()) {
		return FALSE;
	}

	int pid;
	pid = forkTask(this);

	if (!pid) {
		lDevRun:
		while (!m_HCILayer.GetDeviceManager ()->DeviceIsRunning ()) {
			Process();
		}
		LOG_DEBUG("Device running\r\n");
		while (m_HCILayer.GetDeviceManager ()->DeviceIsRunning ()) {
			Process();
		}
		LOG_DEBUG("Device not running\r\n");
		goto lDevRun;
	}

	return TRUE;
}

void CBTSubSystem::Process (void)
{
	m_HCILayer.Process ();

	m_LogicalLayer.Process ();
}

CBTInquiryResults *CBTSubSystem::Listen (unsigned nSeconds)
{
	CBTInquiryResults* inq = m_LogicalLayer.Inquiry (nSeconds);
	m_LogicalLayer.ListDevices ();
	return inq;
}

CBTDevice* CBTSubSystem::Accept (void *ptr)
{
	u16 nResult = 0;
	u8 *paddr = (u8 *)ptr;
	CBTDevice *pDevice = NULL;
	CBTConnection *pConnection = NULL;
	CPtrArray& m_Connections = m_LogicalLayer.GetConnections();

	if (ptr) pConnection = m_LogicalLayer.GetConnection((u8 *)ptr);
	else {
		for (int i=0; i<m_Connections.GetCount(); i++) {
			if(((CBTConnection*)m_Connections[i])->IsMouse()) {
				pConnection = (CBTConnection *)m_Connections[i];
				break;
			}
		}
	}
	pDevice = CreateDevice(pConnection);
	assert(pDevice != 0);
	if (pDevice) nResult = pDevice->Connect ();

	return nResult ? NULL : (CBTDevice *)pDevice;
}

CBTDevice* CBTSubSystem::GetDevice (CBTConnection* pConnection)
{
	bool found = false;
	CBTDevice* pDevice = NULL;
	for (int i=0; i<m_Devices.GetCount() && !found; i++) {
		pDevice = (CBTDevice *)m_Devices[i];
		found = (pDevice->GetConnection() == pConnection);
	}
	return pDevice;
}

CBTDevice* CBTSubSystem::GetDevice (u16 nIndex)
{
	return (CBTDevice *)m_Devices[nIndex];
}

u16 CBTSubSystem::GetDeviceCount (void)
{
	return m_Devices.GetCount();
}

CBTDevice* CBTSubSystem::CreateDevice (CBTConnection *pConnection)
{
	CBTDevice *pDevice = NULL;
	if (pConnection) {
		pDevice = GetDevice(pConnection);
		if (!pDevice) {
			if (pConnection->IsHID()) {
				if (pConnection->IsMouse()) 
					pDevice = new CBTMouse(&m_HIDPLayer, pConnection);
				else
					pDevice = new CBTHIDDevice(&m_HIDPLayer, pConnection);
			} else
				pDevice = new CBTDevice(pConnection);
			m_Devices.Append(pDevice);
		}
	}
	LOG_DEBUG("Created device 0x%08X\r\n", (int)pDevice);
	return pDevice;
}

boolean CBTSubSystem::Status (void)
{
	return m_HCILayer.GetDeviceManager ()->DeviceIsRunning ();
}
