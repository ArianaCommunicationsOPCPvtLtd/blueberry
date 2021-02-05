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
** Description:    Raspberry Pi BareMetal Bluetooth SDP Layer PDUs
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
#include <bluetooth/btl2cap.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/btcommand.h>
#include <bluetooth/btevent.h>
#include <bluetooth/btdevice.h>
#include <bluetooth/btsdp.h>
#include <synchronize.h>
#include <logger.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>


TBTL2CAPCmdHandler* CBTSDPPDUHeader::Handler[BT_SDP_PDU_MAX]={};

////////////////////////////////////////////////////////////////////////////////
//
// L2CAP Signalling Commands
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// L2CAP Command Handlers
//
////////////////////////////////////////////////////////////////////////////////

CBTSDPPDUHeader::CBTSDPPDUHeader()
{
	static bool init = false;

	if (!init) {
		for (int i=0; i<BT_SDP_PDU_MAX; i++) Handler[i] = NULL;
		init = true;
	}
	return;
}

CBTSDPPDUHeader::CBTSDPPDUHeader(u8 nPDUID)
:	PDU_ID(nPDUID)
{
	TransactionID = 0;
	ParameterLength = 0;
}

CBTSDPPDUHeader::CBTSDPPDUHeader(u8 nPDUID, u16 nTransactionID)
:	PDU_ID(nPDUID),
	TransactionID(nTransactionID)
{
	ParameterLength = 0;
}

void CBTSDPPDUHeader::Process(void *pLayer, u16 nLength)
{
	if (CBTSDPPDUHeader::Handler[PDU_ID])
		CBTSDPPDUHeader::Handler[PDU_ID](this, pLayer, nLength);
}

void CBTSDPPDUHeader::Register(u8 nEventCode, void* ptr)
{
	Handler[nEventCode] = (TBTL2CAPCmdHandler *)ptr;
}

CBTSDPPDUErrorResponse::CBTSDPPDUErrorResponse()
{
	CBTSDPPDUHeader::Register(BT_SDP_PDU_ERROR_RESPONSE, (void *)Handler);
}

CBTSDPPDUErrorResponse::CBTSDPPDUErrorResponse(u16 nTransactionID, u16 nErrorCode)
:	CBTSDPPDUHeader(BT_SDP_PDU_ERROR_RESPONSE, nTransactionID)
{
	ErrorCode = nErrorCode;
}

void CBTSDPPDUErrorResponse::Handler(void *ptr,void *lptr,u16 nLength)
{
	((CBTSDPPDUErrorResponse *)ptr)->Process(lptr, nLength);
}

void CBTSDPPDUErrorResponse::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTSDPPDUErrorResponse));
	CBTSDPLayer *pSDPLayer = (CBTSDPLayer *) pLayer;
	LOG_DEBUG("CBTSDPPDUErrorResponse\r\n");
}

CBTSDPPDUServiceSearchRequest::CBTSDPPDUServiceSearchRequest()
{
	CBTSDPPDUHeader::Register(BT_SDP_PDU_SERVICE_SEARCH_REQUEST,(void*)Handler);
}

CBTSDPPDUServiceSearchRequest::CBTSDPPDUServiceSearchRequest(u16 nTransactionID)
:	CBTSDPPDUHeader(BT_SDP_PDU_SERVICE_SEARCH_REQUEST, nTransactionID)
{
}

void CBTSDPPDUServiceSearchRequest::Handler(void *ptr,void *lptr,u16 nLength)
{
	((CBTSDPPDUServiceSearchRequest *)ptr)->Process(lptr, nLength);
}

void CBTSDPPDUServiceSearchRequest::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTSDPPDUServiceSearchRequest));
	CBTSDPLayer *pSDPLayer = (CBTSDPLayer *) pLayer;
	LOG_DEBUG("CBTSDPPDUServiceSearchRequest\r\n");
}

CBTSDPPDUServiceSearchResponse::CBTSDPPDUServiceSearchResponse()
{
	CBTSDPPDUHeader::Register(BT_SDP_PDU_SERVICE_SEARCH_RESPONSE,(void*)Handler);
}

CBTSDPPDUServiceSearchResponse::CBTSDPPDUServiceSearchResponse(u16 nTransactionID)
:	CBTSDPPDUHeader(BT_SDP_PDU_SERVICE_SEARCH_RESPONSE, nTransactionID)
{
}

void CBTSDPPDUServiceSearchResponse::Handler(void *ptr,void *lptr,u16 nLength)
{
	((CBTSDPPDUServiceSearchResponse *)ptr)->Process(lptr, nLength);
}

void CBTSDPPDUServiceSearchResponse::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTSDPPDUServiceSearchResponse));
	CBTSDPLayer *pSDPLayer = (CBTSDPLayer *) pLayer;
	LOG_DEBUG("CBTSDPPDUServiceSearchResponse\r\n");
}

CBTSDPPDUServiceAttributeRequest::CBTSDPPDUServiceAttributeRequest()
{
	CBTSDPPDUHeader::Register(
		BT_SDP_PDU_SERVICE_ATTRIBUTE_REQUEST, (void *)Handler);
}

CBTSDPPDUServiceAttributeRequest::CBTSDPPDUServiceAttributeRequest(
	u16 nTransactionID)
:	CBTSDPPDUHeader(BT_SDP_PDU_SERVICE_ATTRIBUTE_REQUEST, nTransactionID)
{
}

void CBTSDPPDUServiceAttributeRequest::Handler(void *ptr,void *lptr,u16 nLength)
{
	((CBTSDPPDUServiceAttributeRequest *)ptr)->Process(lptr, nLength);
}

void CBTSDPPDUServiceAttributeRequest::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTSDPPDUServiceAttributeRequest));
	CBTSDPLayer *pSDPLayer = (CBTSDPLayer *) pLayer;
	LOG_DEBUG("CBTSDPPDUServiceAttributeRequest\r\n");
}

CBTSDPPDUServiceAttributeResponse::CBTSDPPDUServiceAttributeResponse()
{
	CBTSDPPDUHeader::Register(
		BT_SDP_PDU_SERVICE_ATTRIBUTE_RESPONSE, (void *)Handler);
}

CBTSDPPDUServiceAttributeResponse::CBTSDPPDUServiceAttributeResponse(
	u16 nTransactionID)
:	CBTSDPPDUHeader(BT_SDP_PDU_SERVICE_ATTRIBUTE_RESPONSE, nTransactionID)
{
}

void CBTSDPPDUServiceAttributeResponse::Handler(
	void *ptr, void *lptr, u16 nLength)
{
	((CBTSDPPDUServiceAttributeResponse *)ptr)->Process(lptr, nLength);
}

void CBTSDPPDUServiceAttributeResponse::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTSDPPDUServiceAttributeResponse));
	CBTSDPLayer *pSDPLayer = (CBTSDPLayer *) pLayer;
	LOG_DEBUG("CBTSDPPDUServiceAttributeResponse\r\n");
}

CBTSDPPDUServiceSearchAttributeRequest::CBTSDPPDUServiceSearchAttributeRequest()
{
	CBTSDPPDUHeader::Register(
		BT_SDP_PDU_SERVICE_SEARCH_ATTRIBUTE_REQUEST, (void *)Handler);
}

CBTSDPPDUServiceSearchAttributeRequest::CBTSDPPDUServiceSearchAttributeRequest(
	u16 nTransactionID)
:	CBTSDPPDUHeader(BT_SDP_PDU_SERVICE_SEARCH_ATTRIBUTE_REQUEST, nTransactionID)
{
}

void CBTSDPPDUServiceSearchAttributeRequest::Handler(
	void *ptr,void *lptr,u16 nLength)
{
	((CBTSDPPDUServiceSearchAttributeRequest *)ptr)->Process(lptr, nLength);
}

void CBTSDPPDUServiceSearchAttributeRequest::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTSDPPDUServiceSearchAttributeRequest));
	CBTSDPLayer *pSDPLayer = (CBTSDPLayer *) pLayer;
	LOG_DEBUG("CBTSDPPDUServiceSearchAttributeRequest\r\n");
}

CBTSDPPDUServiceSearchAttributeResponse
	::CBTSDPPDUServiceSearchAttributeResponse()
{
	CBTSDPPDUHeader::Register(
		BT_SDP_PDU_SERVICE_SEARCH_ATTRIBUTE_RESPONSE, (void *)Handler);
}

CBTSDPPDUServiceSearchAttributeResponse
	::CBTSDPPDUServiceSearchAttributeResponse(u16 nTransactionID)
:	CBTSDPPDUHeader(BT_SDP_PDU_SERVICE_SEARCH_ATTRIBUTE_RESPONSE, nTransactionID)
{
}

void CBTSDPPDUServiceSearchAttributeResponse::Handler(
	void *ptr, void *lptr, u16 nLength)
{
	((CBTSDPPDUServiceSearchAttributeResponse *)ptr)->Process(lptr, nLength);
}

void CBTSDPPDUServiceSearchAttributeResponse::Process(void *pLayer, u16 nLength)
{
	assert (nLength >= sizeof (CBTSDPPDUServiceSearchAttributeResponse));
	CBTSDPLayer *pSDPLayer = (CBTSDPLayer *) pLayer;
	LOG_DEBUG("CBTSDPPDUServiceSearchAttributeResponse\r\n");
}
