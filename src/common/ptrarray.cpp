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
** Description:    Raspberry Pi BareMetal Pointer Array Routines
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
#include <bluetooth/ptrarray.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

CPtrArray::CPtrArray (unsigned nInitialSize, unsigned nSizeIncrement)
:	m_nReservedSize (nInitialSize),
	m_nSizeIncrement (nSizeIncrement),
	m_nUsedCount (0),
	m_ppArray (0)
{
	assert (m_nReservedSize > 0);
	assert (m_nSizeIncrement > 0);

	m_ppArray = (void **)malloc(sizeof( void *)* m_nReservedSize);
	assert (m_ppArray != 0);
}

CPtrArray::~CPtrArray (void)
{
	m_nReservedSize = 0;
	m_nSizeIncrement = 0;

	free (m_ppArray);
	m_ppArray = 0;
}

unsigned CPtrArray::GetCount (void) const
{
	return m_nUsedCount;
}

void *&CPtrArray::operator[] (unsigned nIndex)
{
	assert (nIndex < m_nUsedCount);
	assert (m_nUsedCount <= m_nReservedSize);
	assert (m_ppArray != 0);

	return m_ppArray[nIndex];
}

void *CPtrArray::operator[] (unsigned  nIndex) const
{
	assert (nIndex < m_nUsedCount);
	assert (m_nUsedCount <= m_nReservedSize);
	assert (m_ppArray != 0);

	return m_ppArray[nIndex];
}

unsigned CPtrArray::Append (void *pPtr)
{
	assert (m_nReservedSize > 0);
	assert (m_ppArray != 0);

	assert (m_nUsedCount <= m_nReservedSize);
	if (m_nUsedCount == m_nReservedSize) {
		assert (m_nSizeIncrement > 0);
		void **ppNewArray = (void **)
			malloc(sizeof(void *)*(m_nReservedSize + m_nSizeIncrement));
		assert (ppNewArray != 0);

		memcpy (ppNewArray, m_ppArray, m_nReservedSize * sizeof (void *));

		free (m_ppArray);
		m_ppArray = ppNewArray;

		m_nReservedSize += m_nSizeIncrement;
	}

	m_ppArray[m_nUsedCount] = pPtr;

	return m_nUsedCount++;
}

unsigned CPtrArray::Delete (void *pPtr)
{
	int i;
	assert (m_nUsedCount > 0);
	assert (m_ppArray != 0);
	bool found = false;

	for (i=0; i<m_nUsedCount && !found; i++)
		if (m_ppArray[i] == pPtr) found = true;
	if (found) {
		void *tmp = m_ppArray[--m_nUsedCount];
		m_ppArray[m_nUsedCount] = m_ppArray[i];
		m_ppArray[i] = tmp;
	}

	return m_nUsedCount;
}

bool CPtrArray::Find (void *pPtr)
{
	assert (m_ppArray != 0);
	bool found = false;

    for (int i=0; i<m_nUsedCount; i++)
		if (m_ppArray[i] == pPtr) found = true;

	return found;
}

void CPtrArray::RemoveLast (void)
{
	assert (m_nUsedCount > 0);
	m_nUsedCount--;
}
