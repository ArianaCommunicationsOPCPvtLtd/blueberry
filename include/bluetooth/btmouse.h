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
** Description:    Raspberry Pi BareMetal Bluetooth Mouse Header
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
#ifndef _bt_mouse_h
#define _bt_mouse_h

#include <macros.h>
#include <types.h>
#include <stdlib.h>
#include <bluetooth/bthidp.h>
#include <graphics/event.h>


////////////////////////////////////////////////////////////////////////////////
//
// Message 
//
////////////////////////////////////////////////////////////////////////////////
typedef enum 
{
	BT_MOUSE_STATE_NORMAL = 0x0,
	BT_MOUSE_STATE_BUTTON_PRESSED = 0x1,
	BT_MOUSE_STATE_WHEEL_PRESSED = 0x2
} TBTMouseState;

////////////////////////////////////////////////////////////////////////////////
//
// Mouse Device 
//
////////////////////////////////////////////////////////////////////////////////

struct mouse_data {
	signed char	x;
	signed char y;
	signed char w;
}
PACKED;
typedef struct mouse_data TBTMouseData;

class CBTMouse: public CBTHIDDevice
{
public:
	CBTMouse(CBTHIDPLayer *pHIDPLayer, CBTConnection *pConnection);
	~CBTMouse(void);

	// Packet handler
	void Parser(u8*, u16);

private:
	u16						m_nX;
	u16						m_nY;
	u16						m_nScroll;
	bool					m_bLeftButton;
	bool					m_bRightButton;
	bool					m_bWheel;
	bool					m_bConnected;
	TBTMouseState			m_tState;
	u16						m_nCount;
};

#endif
