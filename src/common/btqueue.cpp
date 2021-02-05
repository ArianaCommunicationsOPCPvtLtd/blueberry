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
** Description:    Raspberry Pi BareMetal Bluetooth Queue Routines
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
#include <bluetooth/btqueue.h>
#include <bluetooth/bluetooth.h>
#include <platform/bt_interrupt-system.h>
#include <mutex.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

struct TBTQueueEntry
{
	volatile TBTQueueEntry	*pPrev;
	volatile TBTQueueEntry	*pNext;
	unsigned		 nLength;
	unsigned char		 Buffer[BT_MAX_DATA_SIZE];
	void			*pParam;
};

CBTQueue::CBTQueue (void)
:	m_pFirst (0),
	m_pLast (0)
{
	m_nCount = 0;
	m_SpinLock = get_mutex(MUTEX_BT);
	m_bInit = true;
}

CBTQueue::~CBTQueue (void)
{
	Flush ();
}

boolean CBTQueue::IsEmpty (void) const
{
	return m_pFirst == 0 ? TRUE : FALSE;
}

void CBTQueue::Flush (void)
{
	if (!m_bInit) return;	
	InterruptSystemDisableIRQ(ARM_IRQ_UART);
	spin_lock(m_SpinLock);
	while (m_pFirst != 0) {

		volatile TBTQueueEntry *pEntry = m_pFirst;
		assert (pEntry != 0);

		m_pFirst = pEntry->pNext;
		if (m_pFirst != 0)
			m_pFirst->pPrev = 0;
		else {
			assert (m_pLast == pEntry);
			m_pLast = 0;
		}

		free( (void *)pEntry);
	}
	spin_unlock(m_SpinLock);
	InterruptSystemEnableIRQ(ARM_IRQ_UART);
}
	
void CBTQueue::Enqueue (const void *pBuffer, unsigned nLength, void *pParam)
{
	if (!m_bInit) return;	
	TBTQueueEntry *pEntry = (TBTQueueEntry *)malloc(sizeof(TBTQueueEntry));
	assert (pEntry != 0);

	assert (nLength > 0);
	assert (nLength <= BT_MAX_DATA_SIZE);
	pEntry->nLength = nLength;

	assert (pBuffer != 0);

	if (pEntry) {
		memcpy (pEntry->Buffer, pBuffer, nLength);
		pEntry->pParam = pParam;

		InterruptSystemDisableIRQ(ARM_IRQ_UART);
		spin_lock(m_SpinLock);

		pEntry->pPrev = m_pLast;
		pEntry->pNext = 0;

		if (m_pFirst == 0)
			m_pFirst = pEntry;
		else {
			assert (m_pLast != 0);
			assert (m_pLast->pNext == 0);
			m_pLast->pNext = pEntry;
		}
		m_pLast = pEntry;

		spin_unlock(m_SpinLock);
		InterruptSystemEnableIRQ(ARM_IRQ_UART);
	}
}

unsigned CBTQueue::Dequeue (void *pBuffer, void **ppParam)
{
	unsigned nResult = 0;

	if (!m_bInit) return nResult;
	if (!m_pFirst) return nResult;
	InterruptSystemDisableIRQ(ARM_IRQ_UART);
	spin_lock(m_SpinLock);
	if (m_pFirst != 0) {

		volatile TBTQueueEntry *pEntry = m_pFirst;
		assert (pEntry != 0);

		m_pFirst = pEntry->pNext;
		if (m_pFirst != 0)
			m_pFirst->pPrev = 0;
		else {
			assert (m_pLast == pEntry);
			m_pLast = 0;
		}

		spin_unlock(m_SpinLock);
		InterruptSystemEnableIRQ(ARM_IRQ_UART);

		nResult = pEntry->nLength;
		assert (nResult > 0);
		assert (nResult <= BT_MAX_DATA_SIZE);

		memcpy (pBuffer, (const void *) pEntry->Buffer, nResult);

		if (ppParam != 0) {
			*ppParam = pEntry->pParam;
		}

		free( (void *)pEntry);
	} else {
		spin_unlock(m_SpinLock);
		InterruptSystemEnableIRQ(ARM_IRQ_UART);
	}

	return nResult;
}
