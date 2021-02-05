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
** Description:    Raspberry Pi BareMetal BCM2835 Interrupt System  Header
**
** Dependencies:
** 
** Revision:
** Revision 0.1 - File Created
** Additional Comments:
// The IRQ list is taken from Linux and is:
//	Copyright (C) 2010 Broadcom
//	Copyright (C) 2003 ARM Limited
//	Copyright (C) 2000 Deep Blue Solutions Ltd.
//	Licensed under GPL2
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
#ifndef _rpi_interrupt_system_h
#define _rpi_interrupt_system_h

#include <platform/rpi/rpi-base.h>
#include <types.h>

#define ARM_IO_BASE		PERIPHERAL_BASE
//
// Interrupt Controller
//
#define ARM_IC_BASE		(ARM_IO_BASE + 0xB000)

#define ARM_IC_IRQ_BASIC_PENDING  (ARM_IC_BASE + 0x200)
#define ARM_IC_IRQ_PENDING_1	  (ARM_IC_BASE + 0x204)
#define ARM_IC_IRQ_PENDING_2	  (ARM_IC_BASE + 0x208)
#define ARM_IC_FIQ_CONTROL	  (ARM_IC_BASE + 0x20C)
#define ARM_IC_ENABLE_IRQS_1	  (ARM_IC_BASE + 0x210)
#define ARM_IC_ENABLE_IRQS_2	  (ARM_IC_BASE + 0x214)
#define ARM_IC_ENABLE_BASIC_IRQS  (ARM_IC_BASE + 0x218)
#define ARM_IC_DISABLE_IRQS_1	  (ARM_IC_BASE + 0x21C)
#define ARM_IC_DISABLE_IRQS_2	  (ARM_IC_BASE + 0x220)
#define ARM_IC_DISABLE_BASIC_IRQS (ARM_IC_BASE + 0x224)

#define TRUE 1
#ifdef __cplusplus
extern "C" {
#endif
typedef void TIRQHandler (void *pParam);

typedef struct TInterruptSystem
{
	TIRQHandler	*m_apIRQHandler[IRQ_LINES];
	void		*m_pParam[IRQ_LINES];
} TInterruptSystem;

void InterruptSystem (TInterruptSystem *pThis);
void _InterruptSystem (TInterruptSystem *pThis);

//int InterruptSystemInitialize (TInterruptSystem *pThis);
int InterruptSystemInitialize (void);

void InterruptSystemConnectIRQ (TInterruptSystem *pThis, unsigned nIRQ, TIRQHandler *pHandler, void *pParam);
void InterruptSystemDisconnectIRQ (TInterruptSystem *pThis, unsigned nIRQ);

void InterruptSystemEnableIRQ (unsigned nIRQ);
void InterruptSystemDisableIRQ (unsigned nIRQ);

TInterruptSystem *InterruptSystemGet (void);

void InterruptHandler (void);

#ifdef __cplusplus
}
#endif
#endif
