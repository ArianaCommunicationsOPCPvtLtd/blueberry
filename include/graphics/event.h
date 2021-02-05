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
** Description:    Raspberry Pi BareMetal GUI Event Header
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
#ifndef _UG_EVENT_H
#define _UG_EVENT_H

#include "macros.h"

/* Specify platform-dependent integer types here */

#define __UG_CONST   const
typedef uint8_t      UG_U8;
typedef int8_t       UG_S8;
typedef uint16_t     UG_U16;
typedef int16_t      UG_S16;
typedef uint32_t     UG_U32;
typedef int32_t      UG_S32;
typedef float        UG_FLOAT;


/* -------------------------------------------------------------------------- */
/* -- TYPEDEFS                                                             -- */
/* -------------------------------------------------------------------------- */
typedef struct S_OBJECT                               UG_OBJECT;
typedef struct S_WINDOW                               UG_WINDOW;
typedef UG_S8                                         UG_RESULT;
typedef UG_U32                                        UG_COLOR;
typedef UG_U16                                        UG_COLOR16;
typedef UG_U32                                        UG_EVENT;

/* Standard event source */
#define UG_MOUSE         0x01
#define UG_KEYBOARD      0x02
#define UG_JOYSTICK      0x04
#define UG_TOUCHSCREEN   0x08
#define UG_SYSTEM        0x10

/* Standard mouse button */
#define UG_MOUSE_LEFT    0x01
#define UG_MOUSE_RIGHT   0x02
#define UG_MOUSE_WHEEL   0x04

/* Mouse events */
#define UG_MOUSE_MOVE    0x01
#define UG_MOUSE_CLICK   0x02
#define UG_MOUSE_PRESS   0x04
#define UG_MOUSE_RELEASE 0x08
#define UG_MOUSE_SCROLL  0x10

/* System events */
#define UG_SYSTEM_UPDATE 0x01

/* Event composition */
#define UG_EVENT(SOUCE, EVENT)                  (((SOURCE)<<24) | (EVENT))
#define UG_EVENT_SOURCE(EVENT)                  ((EVENT)>>24)
#define UG_EVENT_TYPE(EVENT)                    ((EVENT)&0xFFF)

/* -------------------------------------------------------------------------- */
/* -- EVENT                                                               -- */
/* -------------------------------------------------------------------------- */
/* Event Class */
class UGEvent {

    friend class UGWindow;

    protected:
    UG_EVENT Type : 24;
    UG_EVENT Source : 8;

    public:
        UGEvent( UG_EVENT type, UG_EVENT source );
        ~UGEvent( );
        inline UG_EVENT GetSource() {return Source;}
        inline UG_EVENT GetType() {return Type;}
}
PACKED;

class UGMouseEvent : UGEvent {

    friend class UGWindow;

    public:
        UGMouseEvent( UG_EVENT type );
        ~UGMouseEvent( );
}
PACKED;

class UGMouseMoveEvent : UGMouseEvent {

    friend class UGWindow;

    private:
    UG_S8 X;
    UG_S8 Y;

    public:
        UGMouseMoveEvent( UG_S8 x, UG_S8 y );
        ~UGMouseMoveEvent( );
        inline UG_S8 GetX() {return X;}
        inline UG_S8 GetY() {return Y;}
}
PACKED;

class UGButtonClickEvent : UGMouseEvent {

    friend class UGWindow;

    private:
    UG_U8 ButtonID;

    public:
        UGButtonClickEvent( UG_U8 id );
        ~UGButtonClickEvent( );
        inline UG_U8 GetButton() {return ButtonID;}
}
PACKED;

class UGButtonPressEvent : UGMouseEvent {

    friend class UGWindow;

    private:
    UG_U8 ButtonID;

    public:
        UGButtonPressEvent( UG_U8 id );
        ~UGButtonPressEvent( );
        inline UG_U8 GetButton() {return ButtonID;}
}
PACKED;

class UGButtonReleaseEvent : UGMouseEvent {

    friend class UGWindow;

    private:
    UG_U8 ButtonID;

    public:
        UGButtonReleaseEvent( UG_U8 id );
        ~UGButtonReleaseEvent( );
        inline UG_U8 GetButton() {return ButtonID;}
}
PACKED;

class UGScrollEvent : UGMouseEvent {

    friend class UGWindow;

    private:
    UG_S8 S;

    public:
        UGScrollEvent( UG_S8 scroll );
        ~UGScrollEvent( );
        inline UG_S8 GetScroll() {return S;}
}
PACKED;

class UGSystemEvent : UGEvent {

    friend class UGWindow;

    public:
        UGSystemEvent( UG_EVENT type );
        ~UGSystemEvent( );
}
PACKED;

class UGUpdateEvent : UGSystemEvent {

    friend class UGWindow;

    private:
    UG_S8 ID;

    public:
        UGUpdateEvent( UG_S8 id );
        ~UGUpdateEvent( );
}
PACKED;

#endif
