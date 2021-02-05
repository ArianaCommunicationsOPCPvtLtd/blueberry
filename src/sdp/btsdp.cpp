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
** Description:    Raspberry Pi BareMetal Bluetooth SDP Layer Routines
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
#include <bluetooth/btsdp.h>
#include <bluetooth/btlogicallayer.h>
#include <bluetooth/btdevice.h>
#include <bluetooth/btsubsystem.h>
#include <synchronize.h>
#include <logger.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static const char FromSDPLayer[] = "btsdp";

////////////////////////////////////////////////////////////////////////////////
//
// SDP Layer
//
////////////////////////////////////////////////////////////////////////////////

CBTSDPLayer *CBTSDPLayer::s_pThis = 0;

CBTSDPLayer::CBTSDPLayer (CBTL2CAPLayer *pL2CAPLayer)
:	m_pL2CAPLayer (pL2CAPLayer)
{
	assert (s_pThis == 0);
	s_pThis = this;

	// Register the L2CAP Layer Callbacks
	pL2CAPLayer->RegisterCallback(BT_PSM_SERVICE_DISCOVERY_PROTOCOL, EventStub);
	pL2CAPLayer->RegisterDataCallback(
		BT_PSM_SERVICE_DISCOVERY_PROTOCOL, DataStub);

	// Register the events
	CBTSDPPDUHeader cmd1;
	CBTSDPPDUErrorResponse cmd2;
	CBTSDPPDUServiceSearchRequest cmd3;
	CBTSDPPDUServiceSearchResponse cmd4;
	CBTSDPPDUServiceAttributeRequest cmd5;
	CBTSDPPDUServiceAttributeResponse cmd6;
	CBTSDPPDUServiceSearchAttributeRequest cmd7;
	CBTSDPPDUServiceSearchAttributeResponse cmd8;
}

CBTSDPLayer::~CBTSDPLayer (void)
{
	s_pThis = 0;
}

void CBTSDPLayer::Callback (const void *pBuffer, unsigned nLength)
{
}

void CBTSDPLayer::DataHandler (u16 nCID, u8 *pBuffer, u16 nLength)
{
}

void CBTSDPLayer::EventStub (const void *pBuffer, unsigned nLength)
{
	assert (s_pThis != 0);
	s_pThis->Callback (pBuffer, nLength);
}

void CBTSDPLayer::DataStub (u16 nCID, const void *pBuffer, unsigned nLength)
{
	assert (nCID >= BT_CID_DYNAMICALLY_ALLOCATED);
	assert (s_pThis != 0);

	if (nCID < BT_CID_DYNAMICALLY_ALLOCATED) return;
	u16 nID = nCID - BT_CID_DYNAMICALLY_ALLOCATED;
	s_pThis->DataHandler (nID, (u8 *)pBuffer, nLength);
}
