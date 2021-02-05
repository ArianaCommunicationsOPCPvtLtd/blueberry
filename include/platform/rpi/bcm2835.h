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
** Engineer:       Arindam Das
** Copyright (C) 2020 Ariana Communications - www.ariana-communications.com
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
#ifndef BCM2835_H_
#define BCM2835_H_
#include <platform/rpi/bcm2835int.h>

#if defined (RPI2) || defined (RPI3)
	#if ! defined (RPIPLUS)
	#define RPIPLUS
	#endif
#endif

#if defined ( RPI2 ) || defined (RPI3)
#define BCM2835_PERI_BASE		0x3F000000	///<
#else
#define BCM2835_PERI_BASE		0x20000000	///<
#endif

#define GPU_IO_BASE				0x7E000000	///<
#define GPU_CACHED_BASE			0x40000000	///<
#define GPU_UNCACHED_BASE		0xC0000000	///<

#if defined ( RPI2 ) || defined (RPI3)
#define GPU_MEM_BASE	GPU_UNCACHED_BASE
#else
#define GPU_MEM_BASE	GPU_CACHED_BASE
#endif

// Convert physical ARM address into bus address
// (does even work, if a bus address is provided already)
#define BUS_ADDRESS(phys)	(((phys) & ~0xC0000000) | GPU_MEM_BASE)

#if defined (RPI3)
#define MEM_COHERENT_REGION		0x400000	///< On Raspberry Pi 3 we need a coherent memory region (1 section) for the property mailbox.
#endif

///< Speed of the core clock core_clk
#if defined (RPI3)
#define BCM2835_CORE_CLK_HZ		300000000	///< 300 MHz
#else
#define BCM2835_CORE_CLK_HZ		250000000	///< 250 MHz
#endif

#ifndef HIGH
#define HIGH 0x1				///< HIGH state
#endif
#ifndef LOW
#define LOW  0x0				///< LOW state
#endif

// RPi Version 2
#define RPI_V2_GPIO_P1_03      2  ///< Version 2, Pin P1-03, SDA when I2C in use
#define RPI_V2_GPIO_P1_05      3  ///< Version 2, Pin P1-05, SCL when I2C in use
#define RPI_V2_GPIO_P1_07      4  ///< Version 2, Pin P1-07
#define RPI_V2_GPIO_P1_08     14  ///< Version 2, Pin P1-08, defaults to ALT function 0 PL011_TXD
#define RPI_V2_GPIO_P1_10     15  ///< Version 2, Pin P1-10, defaults to ALT function 0 PL011_RXD
#define RPI_V2_GPIO_P1_11     17  ///< Version 2, Pin P1-11
#define RPI_V2_GPIO_P1_12     18  ///< Version 2, Pin P1-12
#define RPI_V2_GPIO_P1_13     27  ///< Version 2, Pin P1-13
#define RPI_V2_GPIO_P1_15     22  ///< Version 2, Pin P1-15
#define RPI_V2_GPIO_P1_16     23  ///< Version 2, Pin P1-16
#define RPI_V2_GPIO_P1_18     24  ///< Version 2, Pin P1-18
#define RPI_V2_GPIO_P1_19     10  ///< Version 2, Pin P1-19, MOSI when SPI0 in use
#define RPI_V2_GPIO_P1_21      9  ///< Version 2, Pin P1-21, MISO when SPI0 in use
#define RPI_V2_GPIO_P1_22     25  ///< Version 2, Pin P1-22
#define RPI_V2_GPIO_P1_23     11  ///< Version 2, Pin P1-23, CLK when SPI0 in use
#define RPI_V2_GPIO_P1_24      8  ///< Version 2, Pin P1-24, CE0 when SPI0 in use
#define RPI_V2_GPIO_P1_26      7  ///< Version 2, Pin P1-26, CE1 when SPI0 in use

// System Timer
#define BCM2835_ST_CS_M0		((uint32_t)(1 << 0))	///< System Timer Match 0. DO NOT USE; is used by GPU.
#define BCM2835_ST_CS_M1		((uint32_t)(1 << 1))	///< System Timer Match 1
#define BCM2835_ST_CS_M2		((uint32_t)(1 << 2))	///< System Timer Match 2. DO NOT USE; is used by GPU.
#define BCM2835_ST_CS_M3		((uint32_t)(1 << 3))	///< System Timer Match 3

#define BCM2835_ST_BASE			(BCM2835_PERI_BASE + 0x003000)	///<
#define	BCM2835_DMA0_BASE		(BCM2835_PERI_BASE + 0x007000)	///<
#define	BCM2835_DMA1_BASE		(BCM2835_PERI_BASE + 0x007100)	///<
#define	BCM2835_DMA2_BASE		(BCM2835_PERI_BASE + 0x007200)	///<
#define	BCM2835_DMA3_BASE		(BCM2835_PERI_BASE + 0x007300)	///<
#define	BCM2835_DMA4_BASE		(BCM2835_PERI_BASE + 0x007400)	///<
#define	BCM2835_DMA5_BASE		(BCM2835_PERI_BASE + 0x007500)	///<
#define	BCM2835_DMA6_BASE		(BCM2835_PERI_BASE + 0x007600)	///<
#define BCM2835_IRQ_BASE		(BCM2835_PERI_BASE + 0x00B200)	///<
#define BCM2835_MAILBOX_BASE 	(BCM2835_PERI_BASE + 0x00B880)	///<
#define BCM2835_PM_WDOG_BASE	(BCM2835_PERI_BASE + 0x100000)	///<
#define BCM2835_GPIO_BASE		(BCM2835_PERI_BASE + 0x200000)	///<
#define BCM2835_SPI0_BASE		(BCM2835_PERI_BASE + 0x204000)	///< Base Physical Address of the SPI0 registers
#define BCM2835_PL011_BASE		(BCM2835_PERI_BASE + 0x201000)	///< Base Physical Address of the PL011 registers
#define BCM2835_UART1_BASE		(BCM2835_PERI_BASE + 0x215000)	///< Base Physical Address of the UART1 registers
#define BCM2835_EMMC_BASE		(BCM2835_PERI_BASE + 0x300000)	///< Base Physical Address of the EMMC registers
#define BCM2835_BSC1_BASE		(BCM2835_PERI_BASE + 0x804000)	///< Base Physical Address of the BSC1 registers
#define BCM2835_BSC2_BASE		(BCM2835_PERI_BASE + 0x805000)	///< Base Physical Address of the BSC2 registers
#define	BCM2835_USB_BASE		(BCM2835_PERI_BASE + 0x980000)	///<
#define	BCM2835_DMA15_BASE		(BCM2835_PERI_BASE + 0xE05000)	///<

#ifdef __ASSEMBLY__
#else
#include <stdint.h>

// https://github.com/raspberrypi/linux/blob/rpi-3.6.y/arch/arm/mach-bcm2708/include/mach/platform.h

#define INTERRUPT_TIMER1	(ARM_IRQ1_BASE + 1)		///<
#define INTERRUPT_TIMER3	(ARM_IRQ1_BASE + 3)		///<
#define INTERRUPT_AUX		(ARM_IRQ1_BASE + 29)	///<

#define INTERRUPT_VC_UART	(ARM_IRQ2_BASE + 25)	///< UART Interrupt

typedef enum {
	// ARM_IRQ1_BASE
	BCM2835_TIMER1_IRQn = 1 << (INTERRUPT_TIMER1 - ARM_IRQ1_BASE),		///<
	BCM2835_TIMER3_IRQn = 1	<< (INTERRUPT_TIMER3 - ARM_IRQ1_BASE),		///<
	BCM2835_UART1_IRQn  = 1 << (INTERRUPT_AUX - ARM_IRQ1_BASE),			///<
	// ARM_IRQ2_BASE
	BCM2835_VC_UART_IRQn  = 1 << (INTERRUPT_VC_UART - ARM_IRQ2_BASE)	///<
} BCM2835_IRQn_TypeDef;

#define BCM2835_FIQ_ENABLE	(1 << 7)///< 0x80

#ifdef __cplusplus
#define		__I		volatile		///< defines 'read only' permissions
#else
#define		__I		volatile const	///< defines 'read only' permissions
#endif
#define		__O		volatile		///< defines 'write only' permissions
#define		__IO	volatile		///< defines 'read / write' permissions

//
// System Timers
//
#define ARM_SYSTIMER_BASE	(BCM2835_PERI_BASE + 0x3000)

#define ARM_SYSTIMER_CS		(ARM_SYSTIMER_BASE + 0x00)
#define ARM_SYSTIMER_CLO	(ARM_SYSTIMER_BASE + 0x04)
#define ARM_SYSTIMER_CHI	(ARM_SYSTIMER_BASE + 0x08)
#define ARM_SYSTIMER_C0		(ARM_SYSTIMER_BASE + 0x0C)
#define ARM_SYSTIMER_C1		(ARM_SYSTIMER_BASE + 0x10)
#define ARM_SYSTIMER_C2		(ARM_SYSTIMER_BASE + 0x14)
#define ARM_SYSTIMER_C3		(ARM_SYSTIMER_BASE + 0x18)

typedef struct {
	__IO uint32_t CS;			///< 0x00	System Timer Control/Status 
	__IO uint32_t CLO;			///< 0x04	System Timer Counter Lower 32 bits
	__IO uint32_t CHI;			///< 0x08	System Timer Counter Higher 32 bits
	__I uint32_t C0;			///< 0x0C	System Timer Compare 0.  DO NOT USE; is used by GPU.
	__IO uint32_t C1;			///< 0x10	System Timer Compare 1
	__I uint32_t C2;			///< 0x14	System Timer Compare 2.  DO NOT USE; is used by GPU.
	__IO uint32_t C3;			///< 0x18	System Timer Compare 3
} BCM2835_ST_TypeDef;

typedef struct {
	__IO uint32_t CS;			///< 0x00, Control and Status
	__IO uint32_t CONBLK_AD;	///< 0x04, Control Block Address
	__IO uint32_t TI;			///< 0x08, Transfer Information
	__IO uint32_t SOURCE_AD;	///< 0x0C, Source Address
	__IO uint32_t DEST_AD;		///< 0x10, Destination Address
	__IO uint32_t TXFR_LEN;		///< 0x14, Transfer Length
	__IO uint32_t STRIDE;		///< 0x18, 2D Stride
	__IO uint32_t NEXTCONBK;	///< 0x1C, Next CB Address
	__IO uint32_t DBG;			///< 0x20, Debug
} BCM2835_DMA_TypeDef;

//
// UART 
//
#define ARM_UART0_BASE		(BCM2835_PERI_BASE + 0x201000)

#define ARM_UART0_DR		(ARM_UART0_BASE + 0x00)
#define ARM_UART0_FR		(ARM_UART0_BASE + 0x18)
#define ARM_UART0_IBRD   	(ARM_UART0_BASE + 0x24)
#define ARM_UART0_FBRD		(ARM_UART0_BASE + 0x28)
#define ARM_UART0_LCRH		(ARM_UART0_BASE + 0x2C)
#define ARM_UART0_CR		(ARM_UART0_BASE + 0x30)
#define ARM_UART0_IFLS		(ARM_UART0_BASE + 0x34)
#define ARM_UART0_IMSC		(ARM_UART0_BASE + 0x38)
#define ARM_UART0_RIS		(ARM_UART0_BASE + 0x3C)
#define ARM_UART0_MIS		(ARM_UART0_BASE + 0x40)
#define ARM_UART0_ICR		(ARM_UART0_BASE + 0x44)

typedef struct {
	__I uint32_t IRQ;			///< 0x00
	__IO uint32_t ENABLE;		///< 0x04
	__IO uint32_t PAD[14];		///< 0x08
	__IO uint32_t IO;			///< 0x40
	__IO uint32_t IER;			///< 0x44
	__IO uint32_t IIR;			///< 0x48
	__IO uint32_t LCR;			///< 0x4C
	__IO uint32_t MCR;			///< 0x50
	__IO uint32_t LSR;			///< 0x54
	__IO uint32_t MSR;			///< 0x58
	__IO uint32_t SCR;			///< 0x5C
	__IO uint32_t CNTL;			///< 0x60
	__I uint32_t STAT;			///< 0x64
	__IO uint32_t BAUD;			///< 0x68
} BCM2835_UART_TypeDef;

/// 13.4 Register View
typedef struct {
	__IO uint32_t DR;			///< 0x00, Data Register
	__IO uint32_t RSRECR;		///< 0x04, Receive status register/error clear register
	__IO uint32_t PAD[4];		///< 0x08, Padding
	__IO uint32_t FR;			///< 0x18, Flag register
	__IO uint32_t RES1;			///< 0x1C, Reserved
	__IO uint32_t ILPR;			///< 0x20, not in use
	__IO uint32_t IBRD;			///< 0x24
	__IO uint32_t FBRD;			///< 0x28
	__IO uint32_t LCRH;			///< 0x2C
	__IO uint32_t CR;			///< 0x30
	__IO uint32_t IFLS;			///< 0x34
	__IO uint32_t IMSC;			///< 0x38
	__IO uint32_t RIS;			///< 0x3C
	__I uint32_t MIS;			///< 0x40
	__IO uint32_t ICR;			///< 0x44
	__IO uint32_t DMACR;		///< 0x48
} BCM2835_PL011_TypeDef;

/// Defines for GPIO\n
/// The BCM2835 has 54 GPIO pins.\n
/// BCM2835 data sheet, Page 90 onwards.\n
#define ARM_GPIO_BASE           (BCM2835_PERI_BASE + 0x200000)

#define ARM_GPIO_GPFSEL0        (ARM_GPIO_BASE + 0x00)
#define ARM_GPIO_GPFSEL1        (ARM_GPIO_BASE + 0x04)
#define ARM_GPIO_GPFSEL4        (ARM_GPIO_BASE + 0x10)
#define ARM_GPIO_GPSET0         (ARM_GPIO_BASE + 0x1C)
#define ARM_GPIO_GPCLR0         (ARM_GPIO_BASE + 0x28)
#define ARM_GPIO_GPLEV0         (ARM_GPIO_BASE + 0x34)
#define ARM_GPIO_GPEDS0         (ARM_GPIO_BASE + 0x40)
#define ARM_GPIO_GPREN0         (ARM_GPIO_BASE + 0x4C)
#define ARM_GPIO_GPFEN0         (ARM_GPIO_BASE + 0x58)
#define ARM_GPIO_GPHEN0         (ARM_GPIO_BASE + 0x64)
#define ARM_GPIO_GPLEN0         (ARM_GPIO_BASE + 0x70)
#define ARM_GPIO_GPAREN0        (ARM_GPIO_BASE + 0x7C)
#define ARM_GPIO_GPAFEN0        (ARM_GPIO_BASE + 0x88)
#define ARM_GPIO_GPPUD          (ARM_GPIO_BASE + 0x94)
#define ARM_GPIO_GPPUDCLK0      (ARM_GPIO_BASE + 0x98)

typedef struct {
	__IO uint32_t GPFSEL0;		///< 0x00, GPIO Function Select 0
	__IO uint32_t GPFSEL1;		///< 0x04, GPIO Function Select 1
	__IO uint32_t GPFSEL2;		///< 0x08, GPIO Function Select 2
	__IO uint32_t GPFSEL3;		///< 0x0C, GPIO Function Select 3
	__IO uint32_t GPFSEL4;		///< 0x10, GPIO Function Select 4
	__IO uint32_t GPFSEL5;		///< 0x14, GPIO Function Select 5
	__IO uint32_t RES1;			///< 0x18, Reserved
	__O uint32_t GPSET0;		///< 0x1C, GPIO Pin Output Set 0
	__O uint32_t GPSET1;		///< 0x20, GPIO Pin Output Set 1
	__IO uint32_t RES2;			///< 0x24, Reserved
	__O uint32_t GPCLR0;		///< 0x28, GPIO Pin Output Clear 0
	__O uint32_t GPCLR1;		///< 0x2C, GPIO Pin Output Clear 1
	__IO uint32_t RES3;			///< 0x30, Reserved
	__I uint32_t GPLEV0;		///< 0x34, GPIO Pin Level 0
	__I uint32_t GPLEV1;		///< 0x38, GPIO Pin Level 1
	__IO uint32_t RES4;			///< 0x3C, Reserved
	__IO uint32_t GPEDS0;		///< 0x40, GPIO Pin Event Detect Status 0
	__IO uint32_t GPEDS1;		///< 0x44, GPIO Pin Event Detect Status 1
	__IO uint32_t RES5;			///< 0x48, Reserved
	__IO uint32_t GPREN0;		///< 0x4C, GPIO Pin Rising Edge Detect Enable 0
	__IO uint32_t GPREN1;		///< 0x50, GPIO Pin Rising Edge Detect Enable 1
	__IO uint32_t RES6;			///< 0x54, Reserved
	__IO uint32_t GPFEN0;		///< 0x58, GPIO Pin Falling Edge Detect Enable 0
	__IO uint32_t GPFEN1;		///< 0x5C, GPIO Pin Falling Edge Detect Enable 1
	__IO uint32_t RES7;			///< 0x60, Reserved
	__IO uint32_t GPHEN0;		///< 0x64, GPIO Pin High Detect Enable 0
	__IO uint32_t GPHEN1;		///< 0x68, GPIO Pin High Detect Enable 1
	__IO uint32_t RES8;			///< 0x6C, Reserved
	__IO uint32_t GPLEN0;		///< 0x70, GPIO Pin Low Detect Enable 0
	__IO uint32_t GPLEN1;		///< 0x74, GPIO Pin Low Detect Enable 1
	__IO uint32_t RES9;			///< 0x78, Reserved
	__IO uint32_t AREN[2];		///< 0x7C
	__IO uint32_t RES10;		///< 0x84, Reserved
	__IO uint32_t AFEN[2];		///< 0x88
	__IO uint32_t RES11;		///< 0x90
	__IO uint32_t GPPUD; 		///< 0x94, GPIO Pin Pull-up/down Enable
	__IO uint32_t GPPUDCLK0;	///< 0x98, GPIO Pin Pull-up/down Enable Clock 0
	__IO uint32_t GPPUDCLK1;	///< 0x9C, GPIO Pin Pull-up/down Enable Clock 1
} BCM2835_GPIO_TypeDef;


/// Defines for SPI\n
/// SPI register offsets from \ref BCM2835_SPI0_BASE.\n
/// per 10.5 SPI Register Map
typedef struct {
	__IO uint32_t CS;			///< 0x00, SPI Master Control and Status
	__IO uint32_t FIFO;			///< 0x04, SPI Master TX and RX FIFOs
	__IO uint32_t CLK;			///< 0x08, SPI Master Clock Divider
	__IO uint32_t DLEN;			///< 0x0C, SPI Master Data Length
	__IO uint32_t LTOH;			///< 0x10, SPI LOSSI mode TOH
	__IO uint32_t DC;			///< 0x14, SPI DMA DREQ Controls
} BCM2835_SPI_TypeDef;

/// Defines for I2C\n
/// BSC register offsets from BCM2835_BSC*_BASE.\n
/// per 3.1 BSC Register Map
typedef struct {
	__IO uint32_t C;		///< 0x00, BSC Master Control
	__IO uint32_t S;		///< 0x04, BSC Master Status
	__IO uint32_t DLEN;		///< 0x08, BSC Master Data Length
	__IO uint32_t A;		///< 0x0C, BSC Master Slave Address
	__IO uint32_t FIFO;		///< 0x10, BSC Master Data FIFO
	__IO uint32_t DIV;		///< 0x14, BSC Master Clock Divider
	__IO uint32_t DEL;		///< 0x18, BSC Master Data Delay
	__IO uint32_t CLKT;		///< 0x1C, BSC Master Clock Stretch Timeout
} BCM2835_BSC_TypeDef;

/// Defines for IRQ\n
typedef struct {
	__I uint32_t IRQ_BASIC_PENDING;		///< 0x00
	__I uint32_t IRQ_PENDING1;			///< 0x04
	__I uint32_t IRQ_PENDING2;			///< 0x08
	__IO uint32_t FIQ_CONTROL;			///< 0x0C
	__IO uint32_t IRQ_ENABLE1;			///< 0x10
	__IO uint32_t IRQ_ENABLE2;			///< 0x14
	__IO uint32_t IRQ_BASIC_ENABLE;		///< 0x18
	__IO uint32_t IRQ_DISABLE1;			///< 0x1C
	__IO uint32_t IRQ_DISABLE2;			///< 0x20
	__IO uint32_t IRQ_BASIC_DISABLE;	///< 0x24
} BCM2835_IRQ_TypeDef;

/// Defines for MAILBOX\n
typedef struct {
	__I uint32_t READ;		///< 0x00
	__I uint32_t RES1;		///< 0x04
	__I uint32_t RES2;		///< 0x08
	__I uint32_t RES3;		///< 0x0C
	__I uint32_t PEEK;		///< 0x10
	__I uint32_t SENDER;	///< 0x14
	__IO uint32_t STATUS;	///< 0x18
	__I uint32_t CONFIG;	///< 0x1C
	__O uint32_t WRITE;		///< 0x20
} BCM2835_MAILBOX_TypeDef;

/// Defines for WATCHDOG\n
typedef struct {
	__I uint32_t UNKWOWN0[7];	///< 0x00
	__IO uint32_t RSTC;			///< 0x1C
	__I uint32_t UNKWOWN1;		///< 0x20
	__IO uint32_t WDOG;			///< 0x24
} BCM2835_PM_WDOG_TypeDef;

/// Defines for EMMC\n
typedef struct {
	__IO uint32_t ARG2;				///< 0x00
	__IO uint32_t BLKSIZECNT;		///< 0x04
	__IO uint32_t ARG1;				///< 0x08
	__IO uint32_t CMDTM;			///< 0x0C
	__O uint32_t RESP0;				///< 0x10
	__O uint32_t RESP1;				///< 0x14
	__O uint32_t RESP2;				///< 0x18
	__O uint32_t RESP3;				///< 0x1C
	__IO uint32_t DATA;				///< 0x20
	__O uint32_t STATUS;			///< 0x24
	__IO uint32_t CONTROL0;			///< 0x28
	__IO uint32_t CONTROL1;			///< 0x2C
	__IO uint32_t INTERRUPT;		///< 0x30
	__IO uint32_t IRPT_MASK;		///< 0x34
	__IO uint32_t IRPT_EN;			///< 0x38
	__IO uint32_t CONTROL2;			///< 0x3C
	__IO uint32_t CAPABILITIES_0;	///< 0x40
	__IO uint32_t CAPABILITIES_1;	///< 0x44
	__IO uint32_t NOTINUSE1[2];
	__IO uint32_t FORCE_IRPT;		///< 0x50
	__IO uint32_t NOTINUSE2[7];
	__IO uint32_t BOOT_TIMEOUT;		///< 0x70
	__IO uint32_t DBG_SEL;			///< 0x74
	__IO uint32_t NOTINUSE3[2];
	__IO uint32_t EXRDFIFO_CFG;		///< 0x80
	__IO uint32_t EXRDFIFO_EN;		///< 0x84
	__IO uint32_t TUNE_STEP;		///< 0x88
	__IO uint32_t TUNE_STEPS_STD;	///< 0x8C
	__IO uint32_t TUNE_STEPS_DDR;	///< 0x90
	__IO uint32_t NOTINUSE4[23];
	__IO uint32_t SPI_INT_SPT;		///< 0xF0
	__IO uint32_t NOTINUSE5[2];
	__IO uint32_t SLOTISR_VER;		///< 0xFC
} BCM2835_EMMC_TypeDef;

#define BCM2835_ST			((BCM2835_ST_TypeDef *)   BCM2835_ST_BASE)			///< Base register address for SYSTEM TIMER
#define BCM2835_DMA0		((BCM2835_DMA_TypeDef) *) BCM2835_DMA0_BASE)		///< Base register address for DMA Channel 0
#define BCM2835_DMA1		((BCM2835_DMA_TypeDef) *) BCM2835_DMA1_BASE)		///< Base register address for DMA Channel 1
#define BCM2835_DMA2		((BCM2835_DMA_TypeDef) *) BCM2835_DMA2_BASE)		///< Base register address for DMA Channel 2
#define BCM2835_DMA3		((BCM2835_DMA_TypeDef) *) BCM2835_DMA3_BASE)		///< Base register address for DMA Channel 3
#define BCM2835_DMA4		((BCM2835_DMA_TypeDef) *) BCM2835_DMA4_BASE)		///< Base register address for DMA Channel 4
#define BCM2835_DMA5		((BCM2835_DMA_TypeDef) *) BCM2835_DMA5_BASE)		///< Base register address for DMA Channel 5
#define BCM2835_DMA6		((BCM2835_DMA_TypeDef) *) BCM2835_DMA6_BASE)		///< Base register address for DMA Channel 6
#define BCM2835_IRQ			((BCM2835_IRQ_TypeDef *)  BCM2835_IRQ_BASE)			///< Base register address for IRQ
#define BCM2835_MAILBOX		((BCM2835_MAILBOX_TypeDef *) BCM2835_MAILBOX_BASE)	///< Base register address for MAILBOX
#define BCM2835_PM_WDOG		((BCM2835_PM_WDOG_TypeDef *) BCM2835_PM_WDOG_BASE)	///< Base register address for WATCHDOG
#define BCM2835_GPIO		((BCM2835_GPIO_TypeDef *) BCM2835_GPIO_BASE)		///< Base register address for GPIO
#define BCM2835_SPI0		((BCM2835_SPI_TypeDef *)  BCM2835_SPI0_BASE)		///< Base register address for SPI
#define BCM2835_PL011		((BCM2835_PL011_TypeDef *) BCM2835_PL011_BASE)		///< Base register address for PL011
#define BCM2835_UART1		((BCM2835_UART_TypeDef *) BCM2835_UART1_BASE)		///< Base register address for UART1
#define BCM2835_EMMC		((BCM2835_EMMC_TypeDef *) BCM2835_EMMC_BASE)		///< Base register address for EMMC
#define BCM2835_BSC1		((BCM2835_BSC_TypeDef *)  BCM2835_BSC1_BASE)		///< Base register address for I2C (BSC1)
#define BCM2835_BSC2		((BCM2835_BSC_TypeDef *)  BCM2835_BSC2_BASE)		///< Base register address for I2C (BSC2)

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @ingroup TIMER
 *
 * @param offset_micros
 * @param micros
 */
extern void bcm2835_st_delay(uint64_t offset_micros, uint64_t micros);

/**
 * @ingroup TIMER
 *
 * @return
 */
inline static uint64_t bcm2835_st_read(void) {
	return *(volatile uint64_t *) (BCM2835_ST_BASE + 0x04);
}

/**
 * @ingroup TIMER
 *
 * @param usec
 */
extern void udelay(const uint64_t usec);

/**
 *
 */
extern void __disable_fiq(void);
/**
 *
 */
extern void __enable_fiq(void);

/**
 *
 */
extern void __disable_irq(void);

/**
 *
 */
extern void __enable_irq(void);
#ifdef __cplusplus
}
#endif

#endif

#endif /* BCM2835_H_ */
