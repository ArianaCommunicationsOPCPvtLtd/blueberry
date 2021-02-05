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
#include <bluetooth/btsubsystem.h>
#include <bluetooth/devicenameservice.h>
#include <bluetooth/btinquiryresults.h>
#include <bluetooth/btmouse.h>
#include <bluetooth/bluetooth.h>
#include <logger.h>
#include <assert.h>
#include <task.h>

#ifdef __cplusplus
extern "C" {
#endif

void* BT_Init(void *pInterruptSystem)
{
    CDeviceNameService *pDeviceNameService;
    CBTSubSystem *pBluetooth;

    pDeviceNameService = new CDeviceNameService;
    assert(pDeviceNameService != 0);
    pBluetooth = new CBTSubSystem((TInterruptSystem *)pInterruptSystem);
    assert(pBluetooth != 0);
    pBluetooth->Initialize();
    LOG_DEBUG("Bluetooth initialized successfully\r\n");
    while (!pBluetooth->Status());
    LOG_DEBUG("Bluetooth system running\r\n");
    return (void *)pBluetooth;
}

pBT_device_map BT_Listen(void *ptr, unsigned sec)
{
    pBT_device_map head = NULL, tail = NULL, tmp = NULL;
    CBTSubSystem *pBluetooth = (CBTSubSystem *)ptr;
    CBTInquiryResults *pInquiryResults = NULL;
    do {
        pInquiryResults = pBluetooth->Listen(sec);
        if (pInquiryResults) {
            LOG_DEBUG("Inquiry got %u device\r\n",pInquiryResults->GetCount());
            for (int i; i<pInquiryResults->GetCount(); i++) {
                tmp = (pBT_device_map)malloc(sizeof(tBT_device_map));
                memcpy(tmp->address,
                    pInquiryResults->GetBDAddress(i), BT_BD_ADDR_SIZE);
                memcpy(tmp->type,
                    pInquiryResults->GetClassOfDevice(i), BT_CLASS_SIZE);
                tmp->next = NULL;
                if (!head) head = tail = tmp;
                else { tail->next = tmp; tail = tail->next; }
            }
        }
    } while (!pInquiryResults);
    delete pInquiryResults;
    return head;
}

pBT_device_map BT_Check(void *ptr)
{
    pBT_device_map head = NULL, tail = NULL, tmp = NULL;
    CBTSubSystem *pBluetooth = (CBTSubSystem *)ptr;
    for (int i = 0; i < pBluetooth->GetDeviceCount(); i++ ) {
        tmp = (pBT_device_map)malloc(sizeof(tBT_device_map));
        memcpy(tmp->address,
            pBluetooth->GetDevice(i)->GetBDAddress(), BT_BD_ADDR_SIZE);
        memcpy(tmp->type,
            pBluetooth->GetDevice(i)->GetClassOfDevice(), BT_CLASS_SIZE);
        tmp->next = NULL;
        if (!head) head = tail = tmp;
        else { tail->next = tmp; tail = tail->next; }
    }
    return head;
}

void BT_Free(pBT_device_map ptr)
{
	pBT_device_map tmp = NULL;

	while (ptr) { tmp = ptr; ptr = ptr->next; free(ptr); }
}

u8* BT_Find(pBT_device_map ptr)
{
	u8 *paddr = NULL;

	while (ptr) {
		if(IS_MOUSE(ptr)) { paddr = ptr->address; ptr = NULL; }
		else ptr = ptr->next; 
	}
	return paddr;
}

pBT_device_descriptor BT_Accept(void *ptr, void *paddr)
{
    pBT_device_descriptor pdesc = NULL;
    CBTSubSystem *pbluetooth = (CBTSubSystem *)ptr;
    CBTDevice* pdevice = (CBTDevice *)pbluetooth->Accept(paddr);

    if (pdevice) {
        LOG_DEBUG("Bluetooth device found 0x%08X\r\n", (int)pdevice);
        pdesc = (pBT_device_descriptor)malloc(sizeof(tBT_device_descriptor));
        if (pdesc) {
            pdesc->device = pdevice;
            memcpy(pdesc->address, paddr, BT_BD_ADDR_SIZE);
            pdesc->event_handler = BT_GetEvent;
        }
    }
    return pdesc;
}

bool BT_GetEvent(void *device, void *pBuffer, unsigned *pLength)
{
	bool flag = false;
	CBTHIDDevice *pDevice = (CBTHIDDevice *)device;
	if (pDevice) flag = pDevice->ReceiveEvent(pBuffer, pLength);
	return flag;
}
#ifdef __cplusplus
}
#endif
