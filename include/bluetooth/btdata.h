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
** Description:    Raspberry Pi BareMetal Bluetooth Data Header
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
#ifndef _bt_data_h
#define _bt_data_h

#include <macros.h>
#include <types.h>
#include <stdlib.h>


////////////////////////////////////////////////////////////////////////////////
//
// HCI
//
////////////////////////////////////////////////////////////////////////////////

// ACL Data

class CBTHCIACLData
{
	public:
	u16	ConnectionHandle : 12;
	u16	PacketBoundaryFlag : 2;
#define BT_CONTINUING_FRAGMENT_PACKET	0x1
#define BT_FIRST_PACKET			0x2
	u16	BroadcastFlag : 2;
#define	BT_NO_BROADCAST			0x0
#define	BT_ACTIVE_BROADCAST		0x1
#define	BT_PICONET_BROADCAST		0x2
	u16	DataTotalLength;
	u8	Data[0];

	CBTHCIACLData();
	CBTHCIACLData(u16, u8*, u16);
	
	void* operator new(size_t T) { return (void *)malloc(T); }
	void* operator new(size_t T, size_t L) { return (void *)malloc(T+L); }
	void operator delete (void *ptr) { free(ptr); }
}
PACKED;

// SCO Data

class CBTHCISCOData
{
	public:
	u16	ConnectionHandle : 12;
	u16	Reserved : 4;
	u8	DataTotalLength;
	u8	Data[0];

	CBTHCISCOData();
	CBTHCISCOData(u16, u8*, u8);
	
	void* operator new(size_t T) { return (void *)malloc(T); }
	void operator delete (void *ptr) { free(ptr); }
}
PACKED;


#endif
