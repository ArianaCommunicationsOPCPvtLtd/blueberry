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
** Description:    Raspberry Pi BareMetal Device Header
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
#ifndef _gpiopin_h
#define _gpiopin_h

#include <mutex.h>
#include <types.h>

#define GPIO_PINS	54

enum TGPIOVirtualPin
{
	GPIOPinAudioLeft	= GPIO_PINS,
	GPIOPinAudioRight,
	GPIOPinUnknown
};

enum TGPIOMode
{
	GPIOModeInput,
	GPIOModeOutput,
	GPIOModeInputPullUp,
	GPIOModeInputPullDown,
	GPIOModeAlternateFunction0,
	GPIOModeAlternateFunction1,
	GPIOModeAlternateFunction2,
	GPIOModeAlternateFunction3,
	GPIOModeAlternateFunction4,
	GPIOModeAlternateFunction5,
	GPIOModeUnknown
};

enum TGPIOInterrupt
{
	GPIOInterruptOnRisingEdge,
	GPIOInterruptOnFallingEdge,
	GPIOInterruptOnHighLevel,
	GPIOInterruptOnLowLevel,
	GPIOInterruptOnAsyncRisingEdge,
	GPIOInterruptOnAsyncFallingEdge,
	GPIOInterruptUnknown
};

typedef void TGPIOInterruptHandler (void *pParam);

class CGPIOManager;

class CGPIOPin		/// Encapsulates a GPIO pin
{
public:
	/// \brief Default constructor
	CGPIOPin (void);

	/// \param nPin Pin number, can be physical (Broadcom) number or TGPIOVirtualPin
	/// \param pManager Is only required for using interrupts (IRQ)
	CGPIOPin (unsigned nPin, TGPIOMode Mode, CGPIOManager *pManager = 0);
	virtual ~CGPIOPin (void);

	/// \param nPin Pin number, can be physical (Broadcom) number or TGPIOVirtualPin
	/// \note To be used together with the default constructor and SetMode()
	void AssignPin (unsigned nPin);

	/// \param Mode Pin mode to be set
	/// \param bInitPin Also init pullup/down mode and output level
	void SetMode (TGPIOMode	Mode,
		      boolean	bInitPin = TRUE);
	
	/// \param nValue Value to be written to the pin (LOW or HIGH)
	void Write (unsigned nValue);
	/// \return Value read from pin (LOW or HIGH)
	unsigned Read (void) const;
#ifndef LOW
#define LOW		0
#endif
#ifndef HIGH
#define HIGH		1
#endif

	/// \brief Write inverted value to pin
	void Invert (void);

	static void WriteAll (u32 nValue, u32 nMask);
	/// \return Level of GPIO0-31 in the respective bits
	static u32 ReadAll (void);

private:
	void SetPullUpMode (unsigned nMode);

	void SetAlternateFunction (unsigned nFunction);

protected:
	unsigned  m_nPin;
	unsigned  m_nRegOffset;
	u32       m_nRegMask;
	TGPIOMode m_Mode;
	unsigned  m_nValue;

	void			*m_pParam;

	unsigned int *s_SpinLock;
};

#endif
