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
** Description:    Raspberry Pi BareMetal Device Name Service Routines
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
## PLEASE REFER TO THE LICENSE INCLUDED IN THIS DISTRIBUTION.
** 
*******************************************************************************/
#include <bluetooth/devicenameservice.h>
#include <logger.h>
#include <assert.h>
#include <mutex.h>
#include <string.h>
#include <stdlib.h>

CDeviceNameService *CDeviceNameService::s_This = 0;

CDeviceNameService::CDeviceNameService (void)
:	m_pList (0)
{
	assert (s_This == 0);
	m_SpinLock = get_mutex(MUTEX_BT);
	s_This = this;
}

CDeviceNameService::~CDeviceNameService (void)
{
	while (m_pList != 0) {
		TDeviceInfo *pNext = m_pList->pNext;

		free (m_pList->pName);
		m_pList->pName = 0;
		m_pList->pDevice = 0;
		free(m_pList);

		m_pList = pNext;
	}
	
	s_This = 0;
}

void CDeviceNameService::AddDevice (const char *pName, CDevice *pDevice, boolean bBlockDevice)
{
	spin_lock(m_SpinLock);

	TDeviceInfo *pInfo = (TDeviceInfo *)malloc(sizeof(TDeviceInfo));
	assert (pInfo != 0);

	assert (pName != 0);
	pInfo->pName = (char*)malloc(strlen (pName)+1);
	assert (pInfo->pName != 0);
	strcpy (pInfo->pName, pName);

	assert (pDevice != 0);
	pInfo->pDevice = pDevice;
	
	pInfo->bBlockDevice = bBlockDevice;

	pInfo->pNext = m_pList;
	m_pList = pInfo;

	spin_unlock(m_SpinLock);
}

void CDeviceNameService::AddDevice (const char *pPrefix, unsigned nIndex,
				    CDevice *pDevice, boolean bBlockDevice)
{
	char Name[40];
	sprintf(Name, "%s%u", pPrefix, nIndex);

	AddDevice (Name, pDevice, bBlockDevice);
}

void CDeviceNameService::RemoveDevice (const char *pName, boolean bBlockDevice)
{
	assert (pName != 0);

	spin_lock(m_SpinLock);

	TDeviceInfo *pInfo = m_pList;
	TDeviceInfo *pPrev = 0;
	while (pInfo != 0) {
		assert (pInfo->pName != 0);
		if (   strcmp (pName, pInfo->pName) == 0
		    && pInfo->bBlockDevice == bBlockDevice)
			break;

		pPrev = pInfo;
		pInfo = pInfo->pNext;
	}

	if (pInfo == 0) {
		spin_unlock(m_SpinLock);

		return;
	}

	if (pPrev == 0)
		m_pList = pInfo->pNext;
	else
		pPrev->pNext = pInfo->pNext;

	spin_unlock(m_SpinLock);

	free (pInfo->pName);
	pInfo->pName = 0;
	pInfo->pDevice = 0;
	free( pInfo);
}

void CDeviceNameService::RemoveDevice (const char *pPrefix, unsigned nIndex, boolean bBlockDevice)
{
	char Name[40];
	sprintf(Name, "%s%u", pPrefix, nIndex);

	RemoveDevice (Name, bBlockDevice);
}

CDevice *CDeviceNameService::GetDevice (const char *pName, boolean bBlockDevice)
{
	assert (pName != 0);
	spin_lock(m_SpinLock);

	TDeviceInfo *pInfo = m_pList;
	while (pInfo != 0) {
		assert (pInfo->pName != 0);
		if (   strcmp (pName, pInfo->pName) == 0
		    && pInfo->bBlockDevice == bBlockDevice) {
			CDevice *pResult = pInfo->pDevice;

			spin_unlock(m_SpinLock);

			assert (pResult != 0);
			return pResult;
		}

		pInfo = pInfo->pNext;
	}

	spin_unlock(m_SpinLock);

	return 0;
}

CDevice *CDeviceNameService::GetDevice (const char *pPrefix, unsigned nIndex, boolean bBlockDevice)
{
	char Name[40];
	sprintf(Name, "%s%u", pPrefix, nIndex);

	return GetDevice (Name, bBlockDevice);
}

void CDeviceNameService::ListDevices (CDevice *pTarget)
{

	unsigned i = 0;

	TDeviceInfo *pInfo = m_pList;
	while (pInfo != 0) {
		assert (pInfo->pName != 0);
		char String[40];
		sprintf (String, "%c %-12s%c",
			       pInfo->bBlockDevice ? 'b' : 'c',
			       (const char *) pInfo->pName,
			       ++i % 4 == 0 ? '\n' : ' ');

		LOG_DEBUG(String);

		pInfo = pInfo->pNext;
	}

	if (i % 4 != 0) {
		LOG_DEBUG("\r\n");
	}
}

CDeviceNameService *CDeviceNameService::Get (void)
{
	assert (s_This != 0);
	return s_This;
}
