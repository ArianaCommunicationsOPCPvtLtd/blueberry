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
** Description:    Raspberry Pi BareMetal Bluetooth Mouse Routines
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
#include <bluetooth/btdevice.h>
#include <bluetooth/btmouse.h>
#include <assert.h>
#include <synchronize.h>
#include <stdlib.h>
#include <string.h>
#include <logger.h>
////////////////////////////////////////////////////////////////////////////////
//
// Mouse Device
//
////////////////////////////////////////////////////////////////////////////////

CBTMouse::CBTMouse(
	CBTHIDPLayer *pHIDPLayer,
	CBTConnection *pConnection)
:	CBTHIDDevice(pHIDPLayer, pConnection)
{
	m_nX = 320;
	m_nY = 240;
	m_nScroll = 240;
	m_bLeftButton = false;
	m_bRightButton = false;
	m_bWheel = false;
	m_nCount = 0;
}

CBTMouse::~CBTMouse(void)
{
}

void CBTMouse::Parser(u8* pBuffer, u16 nLen)
{
	assert(pBuffer[0] == 0x02);
	bool bLeftButton = (pBuffer[1] & 0x01) ? true : false;
	bool bRightButton = (pBuffer[1] & 0x02) ? true : false;
	bool bWheel = (pBuffer[1] & 0x04) ? true : false;
	TBTMouseData *mData = (TBTMouseData *)(pBuffer+2);
	m_nX += mData->x;
	m_nY += mData->y;
	m_nScroll += mData->w;

	if (bLeftButton) {
        if (!m_bLeftButton) m_nCount = 0;
        else {
            m_tState = BT_MOUSE_STATE_BUTTON_PRESSED;
            if (m_nCount++) {
		        UGMouseMoveEvent event(mData->x, mData->y);
		        PostEvent(&event, sizeof event);
            } else {
		        UGButtonPressEvent event(UG_MOUSE_LEFT);
		        PostEvent(&event, sizeof event);
            }
        }
        m_bLeftButton = true;
	} else if (bRightButton) {
        if (!m_bRightButton) m_nCount = 0;
        else {
            m_tState = BT_MOUSE_STATE_BUTTON_PRESSED;
            if (m_nCount++) {
		        UGMouseMoveEvent event(mData->x, mData->y);
		        PostEvent(&event, sizeof event);
            } else {
		        UGButtonPressEvent event(UG_MOUSE_RIGHT);
		        PostEvent(&event, sizeof event);
            }
        }
        m_bRightButton = true;
	} else if (bWheel) {
        m_bWheel = true;
        m_tState = BT_MOUSE_STATE_WHEEL_PRESSED;
		UGButtonPressEvent event(UG_MOUSE_WHEEL);
		PostEvent(&event, sizeof event);
	} else if (mData->x || mData->y) {
		UGMouseMoveEvent event(mData->x, mData->y);
		PostEvent(&event, sizeof event);
	} else if (mData->w) {
		UGScrollEvent event(mData->w);
		PostEvent(&event, sizeof event);
	} else if (m_bLeftButton && !bLeftButton) {
        m_bLeftButton = false;
        m_tState = BT_MOUSE_STATE_NORMAL;
        if (!m_nCount) {
		    UGButtonClickEvent event(UG_MOUSE_LEFT);
		    PostEvent(&event, sizeof event);
        } else {
		    UGButtonReleaseEvent event(UG_MOUSE_LEFT);
		    PostEvent(&event, sizeof event);
        }
    } else if (m_bRightButton && !bRightButton) {
        m_bRightButton = false;
        m_tState = BT_MOUSE_STATE_NORMAL;
        if (!m_nCount) {
		    UGButtonClickEvent event(UG_MOUSE_RIGHT);
		    PostEvent(&event, sizeof event);
        } else {
		    UGButtonReleaseEvent event(UG_MOUSE_RIGHT);
		    PostEvent(&event, sizeof event);
        }
    } else if (m_bWheel && !bWheel) {
        m_bWheel = true;
        m_tState = BT_MOUSE_STATE_NORMAL;
		UGButtonReleaseEvent event(UG_MOUSE_WHEEL);
		PostEvent(&event, sizeof event);
    }
}
