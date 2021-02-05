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
** Description:    Raspberry Pi BareMetal Bluetooth Sub System Header
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
#ifndef _bt_subsystem_h
#define _bt_subsystem_h

#include <types.h>
#include <stdlib.h>
#include <platform/bt_interrupt-system.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/btinquiryresults.h>
#include <bluetooth/btuarttransport.h>
#include <bluetooth/bthcilayer.h>
#include <bluetooth/btlogicallayer.h>
#include <bluetooth/btl2cap.h>
#include <bluetooth/bthidp.h>
#include <bluetooth/btdevice.h>

class CBTSubSystem
{
public:
	CBTSubSystem (TInterruptSystem *pInterruptSystem,
		      TBTCOD nClassOfDevice = BT_CLASS_DESKTOP_COMPUTER,
		      const char *pLocalName = "Raspberry Pi");
	~CBTSubSystem (void);

	boolean Initialize (void);

	void Process (void);

	// returns 0 on failure, result must be deleted by caller otherwise
	CBTInquiryResults *Listen (unsigned nSeconds);		// 1 <= nSeconds <= 61

	CBTDevice* Accept (void *);	

	CBTDevice* GetDevice (CBTConnection *);	

	CBTDevice* GetDevice (u16);	

	u16 GetDeviceCount (void);	

	CBTDevice* CreateDevice (CBTConnection *);	
	
	boolean Status (void);

	void* operator new(size_t T) { return (void *)malloc(T); }
	void operator delete (void *ptr) { free(ptr); }

private:
	TInterruptSystem *m_pInterruptSystem;

	CBTUARTTransport *m_pUARTTransport;

	CBTHCILayer	m_HCILayer;
	CBTLogicalLayer	m_LogicalLayer;
	CBTL2CAPLayer	m_L2CAPLayer;
	CBTHIDPLayer	m_HIDPLayer;

	CPtrArray m_Devices;
};

#endif
