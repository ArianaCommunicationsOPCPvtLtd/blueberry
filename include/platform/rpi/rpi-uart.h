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
** Description:    Raspberry Pi BareMetal System Uart Header
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
** PLEASE REFER TO THE LICENSE INCLUDED IN THE DISTRIBUTION.
** 
*******************************************************************************/
#ifndef RPI_UART_H
#define RPI_UART_H


// Definitions from Raspberry PI Remote Serial Protocol.
//     Copyright 2012 Jamie Iles, jamie@jamieiles.com.
//     Licensed under GPLv2
#define DR_OE_MASK		(1 << 11)
#define DR_BE_MASK		(1 << 10)
#define DR_PE_MASK		(1 << 9)
#define DR_FE_MASK		(1 << 8)

#define FR_TXFE_MASK		(1 << 7)
#define FR_RXFF_MASK		(1 << 6)
#define FR_TXFF_MASK		(1 << 5)
#define FR_RXFE_MASK		(1 << 4)
#define FR_BUSY_MASK		(1 << 3)

#define LCRH_SPS_MASK		(1 << 7)
#define LCRH_WLEN8_MASK		(3 << 5)
#define LCRH_WLEN7_MASK		(2 << 5)
#define LCRH_WLEN6_MASK		(1 << 5)
#define LCRH_WLEN5_MASK		(0 << 5)
#define LCRH_FEN_MASK		(1 << 4)
#define LCRH_STP2_MASK		(1 << 3)
#define LCRH_EPS_MASK		(1 << 2)
#define LCRH_PEN_MASK		(1 << 1)
#define LCRH_BRK_MASK		(1 << 0)

#define CR_CTSEN_MASK		(1 << 15)
#define CR_RTSEN_MASK		(1 << 14)
#define CR_OUT2_MASK		(1 << 13)
#define CR_OUT1_MASK		(1 << 12)
#define CR_RTS_MASK		(1 << 11)
#define CR_DTR_MASK		(1 << 10)
#define CR_RXE_MASK		(1 << 9)
#define CR_TXE_MASK		(1 << 8)
#define CR_LBE_MASK		(1 << 7)
#define CR_UART_EN_MASK		(1 << 0)

#define IFLS_RXIFSEL_SHIFT	3
#define IFLS_RXIFSEL_MASK	(7 << IFLS_RXIFSEL_SHIFT)
#define IFLS_TXIFSEL_SHIFT	0
#define IFLS_TXIFSEL_MASK	(7 << IFLS_TXIFSEL_SHIFT)
	#define IFLS_IFSEL_1_8		0
	#define IFLS_IFSEL_1_4		1
	#define IFLS_IFSEL_1_2		2
	#define IFLS_IFSEL_3_4		3
	#define IFLS_IFSEL_7_8		4

#define INT_OE			(1 << 10)
#define INT_BE			(1 << 9)
#define INT_PE			(1 << 8)
#define INT_FE			(1 << 7)
#define INT_RT			(1 << 6)
#define INT_TX			(1 << 5)
#define INT_RX			(1 << 4)
#define INT_DSRM		(1 << 3)
#define INT_DCDM		(1 << 2)
#define INT_CTSM		(1 << 1)


#endif
