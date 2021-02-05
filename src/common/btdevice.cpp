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
#include <bluetooth/btdevice.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

CBTDevice::CBTDevice (void)
{
	m_pConnection = NULL;
	m_tState = BT_DEVICE_IDLE;
}

CBTDevice::CBTDevice (CBTConnection* pConnection)
{
	m_pConnection = pConnection;
	m_tState = BT_DEVICE_IDLE;
	if (pConnection) pConnection->SetDevice(this);
}

CBTDevice::~CBTDevice (void)
{
	if (m_pConnection) free(m_pConnection);
}

u16 CBTDevice::Connect (void)
{
	return 0;
}

bool CBTDevice::Disconnect(u8 nReason)
{
	return m_pConnection->Disconnect(nReason);
}

bool CBTDevice::RemoveDevice (void)
{
	return FALSE;
}

bool CBTDevice::IsMouse(void)
{
	return m_pConnection->IsMouse();
}

bool CBTDevice::IsKeyboard(void)
{
	return m_pConnection->IsKeyboard();
}

bool CBTDevice::IsJoystick(void)
{
	return m_pConnection->IsJoystick();
}

bool CBTDevice::IsComputer(void)
{
	return m_pConnection->IsComputer();
}

bool CBTDevice::IsConnecting (void)
{
	return (m_tState == BT_DEVICE_CONNECTING);
}

const u8* CBTDevice::GetBDAddress (void) const
{
	return m_pConnection->BDAddr;
}

const u8* CBTDevice::GetClassOfDevice (void) const
{
	return (u8 *)&m_pConnection->ClassOfDevice;
}

const u8* CBTDevice::GetRemoteName (void) const
{
	return m_pConnection->RemoteName;
}

u8 CBTDevice::GetPageScanRepetitionMode (void) const
{
	return m_pConnection->PageScanRepetitionMode;
}

CBTConnection* CBTDevice::GetConnection (void) const
{
	return m_pConnection;
}

void CBTDevice::SetState (TBTDeviceConnectionState tState)
{
	m_tState = tState;
}

void CBTDevice::SetBDAddress (u8* sBDAddr)
{
	m_pConnection->SetBDAddress(sBDAddr);
}

void CBTDevice::SetClassOfDevice (u8* sClassOfDevice)
{
	m_pConnection->SetClassOfDevice(sClassOfDevice);
}

void CBTDevice::SetRemoteName (u8* sRemoteName)
{
	m_pConnection->SetRemoteName(sRemoteName);
}

void CBTDevice::SetPageScanRepetitionMode (u8 nPageScanRepetitionMode)
{
	m_pConnection->PageScanRepetitionMode = nPageScanRepetitionMode;
}

void CBTDevice::Parser (u8* pBuffer, u16 nLength)
{
}
