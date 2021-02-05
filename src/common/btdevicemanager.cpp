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
** Description:    Raspberry Pi BareMetal Bluetooth Device Routines
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
#include <bluetooth/btdevicemanager.h>
#include <bluetooth/btlogicallayer.h>
#include <bluetooth/bthcilayer.h>
#include <bluetooth/bcmvendor.h>
#include <bluetooth/btcommand.h>
#include <bluetooth/btevent.h>
#include <logger.h>
#include <assert.h>
#include <task.h>
#include <string.h>
#include <stdlib.h>

static const char FromDeviceManager[] = "btdev";

CBTDeviceManager::CBTDeviceManager (
	CBTHCILayer *pHCILayer,
	CBTQueue *pEventQueue,
    TBTCOD nClassOfDevice,
	const char *pLocalName)
:	m_pHCILayer (pHCILayer),
	m_pEventQueue (pEventQueue),
	m_nClassOfDevice (nClassOfDevice),
	m_pConnection (NULL),
	m_State (BTDeviceStateUnknown),
	m_pBuffer (0)
{
	memset (m_LocalName, 0, sizeof m_LocalName);
	strncpy ((char *) m_LocalName, pLocalName, sizeof m_LocalName);
}

CBTDeviceManager::~CBTDeviceManager (void)
{
	free (m_pBuffer);
	m_pBuffer = 0;

	m_pHCILayer = 0;
	m_pEventQueue = 0;
}

boolean CBTDeviceManager::Initialize (void)
{
	assert (m_pHCILayer != 0);

	CBTHCIEventCommandComplete e1;
	CBTHCIEventCommandStatus e2;
	m_pBuffer = (u8 *)malloc(BT_MAX_HCI_EVENT_SIZE);
	assert (m_pBuffer != 0);
	CBTHCICommand Cmd(OP_CODE_RESET);
	m_pHCILayer->SendCommand (&Cmd, sizeof Cmd);

	m_State = BTDeviceStateResetPending;

	return TRUE;
}

bool CBTDeviceManager::SendHCICommand(const void* pData, unsigned nLength)
{
	if (pData && nLength) {
		m_pHCILayer->SendCommand (pData, nLength);
		return false;
	}

	return true;
}

void CBTDeviceManager::SetHCICommandPackets (unsigned nDataPackets) {

	m_pHCILayer->SetCommandPackets(nDataPackets);
	return;
}

void CBTDeviceManager::Process (void)
{
	assert (m_pHCILayer != 0);
	assert (m_pEventQueue != 0);
	assert (m_pBuffer != 0);

	unsigned nLength;
	while ((nLength = m_pEventQueue->Dequeue (m_pBuffer)) > 0) {
		assert (nLength >= sizeof (CBTHCIEvent));
		CBTHCIEvent* pHeader = (CBTHCIEvent*) m_pBuffer;
		pHeader->Process(this, nLength);
	}
}

void CBTDeviceManager::SetConnection (CBTConnection* pConnection)
{
	m_pConnection = pConnection;
}

u8* CBTDeviceManager::GetBDAddr (void)
{
	return m_LocalBDAddr;
}

void CBTDeviceManager::SetBDAddr (u8* sBDAddr)
{
	memcpy (m_LocalBDAddr, sBDAddr, BT_BD_ADDR_SIZE);
}

boolean CBTDeviceManager::DeviceIsRunning (void) const
{
	return m_State == BTDeviceStateRunning ? TRUE : FALSE;
}
