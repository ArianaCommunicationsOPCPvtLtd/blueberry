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
** Dependencies:
** 
** Revision:
** Revision 0.1 - File Created
** Additional Comments:
**
** This list is taken from Linux and is:
**	Copyright (C) Broadcom
**	Licensed under GPL2
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
#ifndef _bcm2836_h
#define _bcm2836_h

#if defined (RPI2) || defined (RPI3)

#define ARM_LOCAL_BASE			0x40000000UL

#define ARM_LOCAL_CONTROL		(ARM_LOCAL_BASE + 0x000)
#define ARM_LOCAL_PRESCALER		(ARM_LOCAL_BASE + 0x008)
#define ARM_LOCAL_GPU_INT_ROUTING	(ARM_LOCAL_BASE + 0x00C)
#define ARM_LOCAL_PM_ROUTING_SET	(ARM_LOCAL_BASE + 0x010)
#define ARM_LOCAL_PM_ROUTING_CLR	(ARM_LOCAL_BASE + 0x014)
#define ARM_LOCAL_TIMER_LS		(ARM_LOCAL_BASE + 0x01C)
#define ARM_LOCAL_TIMER_MS		(ARM_LOCAL_BASE + 0x020)
#define ARM_LOCAL_INT_ROUTING		(ARM_LOCAL_BASE + 0x024)
#define ARM_LOCAL_AXI_COUNT		(ARM_LOCAL_BASE + 0x02C)
#define ARM_LOCAL_AXI_IRQ		(ARM_LOCAL_BASE + 0x030)
#define ARM_LOCAL_TIMER_CONTROL		(ARM_LOCAL_BASE + 0x034)
#define ARM_LOCAL_TIMER_WRITE		(ARM_LOCAL_BASE + 0x038)

#define ARM_LOCAL_TIMER_INT_CONTROL0	(ARM_LOCAL_BASE + 0x040)
#define ARM_LOCAL_TIMER_INT_CONTROL1	(ARM_LOCAL_BASE + 0x044)
#define ARM_LOCAL_TIMER_INT_CONTROL2	(ARM_LOCAL_BASE + 0x048)
#define ARM_LOCAL_TIMER_INT_CONTROL3	(ARM_LOCAL_BASE + 0x04C)

#define ARM_LOCAL_MAILBOX_INT_CONTROL0	(ARM_LOCAL_BASE + 0x050)
#define ARM_LOCAL_MAILBOX_INT_CONTROL1	(ARM_LOCAL_BASE + 0x054)
#define ARM_LOCAL_MAILBOX_INT_CONTROL2	(ARM_LOCAL_BASE + 0x058)
#define ARM_LOCAL_MAILBOX_INT_CONTROL3	(ARM_LOCAL_BASE + 0x05C)

#define ARM_LOCAL_IRQ_PENDING0		(ARM_LOCAL_BASE + 0x060)
#define ARM_LOCAL_IRQ_PENDING1		(ARM_LOCAL_BASE + 0x064)
#define ARM_LOCAL_IRQ_PENDING2		(ARM_LOCAL_BASE + 0x068)
#define ARM_LOCAL_IRQ_PENDING3		(ARM_LOCAL_BASE + 0x06C)

#define ARM_LOCAL_FIQ_PENDING0		(ARM_LOCAL_BASE + 0x070)
#define ARM_LOCAL_FIQ_PENDING1		(ARM_LOCAL_BASE + 0x074)
#define ARM_LOCAL_FIQ_PENDING2		(ARM_LOCAL_BASE + 0x078)
#define ARM_LOCAL_FIQ_PENDING3		(ARM_LOCAL_BASE + 0x07C)

#define ARM_LOCAL_MAILBOX0_SET0		(ARM_LOCAL_BASE + 0x080)
#define ARM_LOCAL_MAILBOX1_SET0		(ARM_LOCAL_BASE + 0x084)
#define ARM_LOCAL_MAILBOX2_SET0		(ARM_LOCAL_BASE + 0x088)
#define ARM_LOCAL_MAILBOX3_SET0		(ARM_LOCAL_BASE + 0x08C)

#define ARM_LOCAL_MAILBOX0_SET1		(ARM_LOCAL_BASE + 0x090)
#define ARM_LOCAL_MAILBOX1_SET1		(ARM_LOCAL_BASE + 0x094)
#define ARM_LOCAL_MAILBOX2_SET1		(ARM_LOCAL_BASE + 0x098)
#define ARM_LOCAL_MAILBOX3_SET1		(ARM_LOCAL_BASE + 0x09C)

#define ARM_LOCAL_MAILBOX0_SET2		(ARM_LOCAL_BASE + 0x0A0)
#define ARM_LOCAL_MAILBOX1_SET2		(ARM_LOCAL_BASE + 0x0A4)
#define ARM_LOCAL_MAILBOX2_SET2		(ARM_LOCAL_BASE + 0x0A8)
#define ARM_LOCAL_MAILBOX3_SET2		(ARM_LOCAL_BASE + 0x0AC)

#define ARM_LOCAL_MAILBOX0_SET3		(ARM_LOCAL_BASE + 0x0B0)
#define ARM_LOCAL_MAILBOX1_SET3		(ARM_LOCAL_BASE + 0x0B4)
#define ARM_LOCAL_MAILBOX2_SET3		(ARM_LOCAL_BASE + 0x0B8)
#define ARM_LOCAL_MAILBOX3_SET3		(ARM_LOCAL_BASE + 0x0BC)

#define ARM_LOCAL_MAILBOX0_CLR0		(ARM_LOCAL_BASE + 0x0C0)
#define ARM_LOCAL_MAILBOX1_CLR0		(ARM_LOCAL_BASE + 0x0C4)
#define ARM_LOCAL_MAILBOX2_CLR0		(ARM_LOCAL_BASE + 0x0C8)
#define ARM_LOCAL_MAILBOX3_CLR0		(ARM_LOCAL_BASE + 0x0CC)

#define ARM_LOCAL_MAILBOX0_CLR1		(ARM_LOCAL_BASE + 0x0D0)
#define ARM_LOCAL_MAILBOX1_CLR1		(ARM_LOCAL_BASE + 0x0D4)
#define ARM_LOCAL_MAILBOX2_CLR1		(ARM_LOCAL_BASE + 0x0D8)
#define ARM_LOCAL_MAILBOX3_CLR1		(ARM_LOCAL_BASE + 0x0DC)

#define ARM_LOCAL_MAILBOX0_CLR2		(ARM_LOCAL_BASE + 0x0E0)
#define ARM_LOCAL_MAILBOX1_CLR2		(ARM_LOCAL_BASE + 0x0E4)
#define ARM_LOCAL_MAILBOX2_CLR2		(ARM_LOCAL_BASE + 0x0E8)
#define ARM_LOCAL_MAILBOX3_CLR2		(ARM_LOCAL_BASE + 0x0EC)

#define ARM_LOCAL_MAILBOX0_CLR3		(ARM_LOCAL_BASE + 0x0F0)
#define ARM_LOCAL_MAILBOX1_CLR3		(ARM_LOCAL_BASE + 0x0F4)
#define ARM_LOCAL_MAILBOX2_CLR3		(ARM_LOCAL_BASE + 0x0F8)
#define ARM_LOCAL_MAILBOX3_CLR3		(ARM_LOCAL_BASE + 0x0FC)

#define ARM_LOCAL_END			(ARM_LOCAL_BASE + 0x3FFFF)

#endif

#endif
