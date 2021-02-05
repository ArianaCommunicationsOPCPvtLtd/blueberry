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
** Description:    Raspberry Pi BareMetal Bluetooth Inquiry Results Routines
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
#include <bluetooth/btinquiryresults.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

CBTInquiryResults::CBTInquiryResults (void)
{
}

CBTInquiryResults::~CBTInquiryResults (void)
{
	for (unsigned nResponse=0; nResponse<m_Responses.GetCount(); nResponse++) {
		TBTInquiryResponse *pResponse = (TBTInquiryResponse *)
			m_Responses[nResponse];
		assert (pResponse != 0);

		free(pResponse);
	}
}

void CBTInquiryResults::AddInquiryResult (CBTHCIEventInquiryResult *pEvent)
{
	assert (pEvent != 0);

	for (unsigned i = 0; i < pEvent->NumResponses; i++) {
		TBTInquiryResponse *pResponse = (TBTInquiryResponse *)
			malloc(sizeof(TBTInquiryResponse));
		assert (pResponse != 0);

		memcpy (pResponse->BDAddress, 
			INQUIRY_RESP_BD_ADDR (pEvent, i), BT_BD_ADDR_SIZE);
		memcpy ((u8 *)&pResponse->ClassOfDevice, 
			INQUIRY_RESP_CLASS_OF_DEVICE (pEvent, i), BT_CLASS_SIZE);
		pResponse->PageScanRepetitionMode = 
			INQUIRY_RESP_PAGE_SCAN_REP_MODE (pEvent, i);
		strcpy ((char *) pResponse->RemoteName, "Unknown");

		m_Responses.Append (pResponse);
	}
}

boolean CBTInquiryResults::SetRemoteName (
	CBTHCIEventRemoteNameRequestComplete *pEvent)
{
	assert (pEvent != 0);

	for (unsigned nResponse=0; nResponse<m_Responses.GetCount(); nResponse++) {
		TBTInquiryResponse *pResponse = (TBTInquiryResponse *) 
			m_Responses[nResponse];
		assert (pResponse != 0);

		if (memcmp (pResponse->BDAddress,pEvent->BDAddr,BT_BD_ADDR_SIZE) == 0) {
			memcpy (pResponse->RemoteName, pEvent->RemoteName, BT_NAME_SIZE);

			return TRUE;
		}
	}

	return FALSE;
}

unsigned CBTInquiryResults::GetCount (void) const
{
	return m_Responses.GetCount ();
}

const u8 *CBTInquiryResults::GetBDAddress (unsigned nResponse) const
{
	TBTInquiryResponse *pResponse = (TBTInquiryResponse*)m_Responses[nResponse];
	assert (pResponse != 0);

	return pResponse->BDAddress;
}

const u8 *CBTInquiryResults::GetClassOfDevice (unsigned nResponse) const
{
	TBTInquiryResponse *pResponse = (TBTInquiryResponse*)m_Responses[nResponse];
	assert (pResponse != 0);

	return (u8 *)&pResponse->ClassOfDevice;
}

const u8 *CBTInquiryResults::GetRemoteName (unsigned nResponse) const
{
	TBTInquiryResponse *pResponse = (TBTInquiryResponse*)m_Responses[nResponse];
	assert (pResponse != 0);

	return pResponse->RemoteName;
}

u8 CBTInquiryResults::GetPageScanRepetitionMode (unsigned nResponse) const
{
	TBTInquiryResponse *pResponse = (TBTInquiryResponse*)m_Responses[nResponse];
	assert (pResponse != 0);

	return pResponse->PageScanRepetitionMode;
}

bool CBTInquiryResults::HasDevice (TBTCOD nClassOfDevice) const
{
	for (unsigned nResponse=0; nResponse<m_Responses.GetCount(); nResponse++) {
		TBTInquiryResponse *pResponse = (TBTInquiryResponse *) 
			m_Responses[nResponse];
		assert (pResponse != 0);

		if (pResponse->ClassOfDevice.MinorDeviceClass == 
				nClassOfDevice.MinorDeviceClass && 
				pResponse->ClassOfDevice.MajorDeviceClass == 
				nClassOfDevice.MajorDeviceClass)
			return TRUE;
	}

	return FALSE;
}
