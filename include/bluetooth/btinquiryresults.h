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
** Description:    Raspberry Pi BareMetal Bluetooth Inquiry Results Header
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
#ifndef _bt_btinquiryresults_h
#define _bt_btinquiryresults_h

#include <bluetooth/bluetooth.h>
#include <bluetooth/btevent.h>
#include <bluetooth/btdevicemanager.h>
#include <bluetooth/ptrarray.h>
#include <types.h>
#include <stdlib.h>

typedef struct sBTInquiryResponse
{
	u8	BDAddress[BT_BD_ADDR_SIZE];
	TBTCOD	ClassOfDevice;
	u8	PageScanRepetitionMode;
	u8	RemoteName[BT_NAME_SIZE];
} TBTInquiryResponse;

class CBTInquiryResults
{
public:
	CBTInquiryResults (void);
	~CBTInquiryResults (void);
	void AddInquiryResult (CBTHCIEventInquiryResult *pEvent);
	boolean SetRemoteName (CBTHCIEventRemoteNameRequestComplete *pEvent);

	unsigned GetCount (void) const;

	const u8 *GetBDAddress (unsigned nResponse) const;
	const u8 *GetClassOfDevice (unsigned nResponse) const;
	const u8 *GetRemoteName (unsigned nResponse) const;
	u8 GetPageScanRepetitionMode (unsigned nResponse) const;
	bool HasDevice (TBTCOD nClassOfDevice) const;
	void* operator new(size_t T) { return (void *)malloc(T); }
	void operator delete (void *ptr) { free(ptr); }

private:
	CPtrArray m_Responses;
};

#endif
