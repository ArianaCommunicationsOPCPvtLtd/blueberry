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
** Description:    Raspberry Pi BareMetal Device Name Service Header
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
#ifndef _devicenameservice_h
#define _devicenameservice_h

#include <bluetooth/device.h>
#include <types.h>
#include <stdlib.h>

struct TDeviceInfo
{
	TDeviceInfo	*pNext;
	char		*pName;
	CDevice		*pDevice;
	boolean		 bBlockDevice;
};

class CDeviceNameService  /// Devices can be registered by name and retrieved later by this name
{
public:
	CDeviceNameService (void);
	~CDeviceNameService (void);

	/// \param pName	Device name string
	/// \param pDevice	Pointer to the device object
	/// \param bBlockDevice TRUE if this is a block device, otherwise character device
	void AddDevice (const char *pName, CDevice *pDevice, boolean bBlockDevice);
	/// \param pPrefix	Device name prefix string
	/// \param nIndex	Device name index
	/// \param pDevice	Pointer to the device object
	/// \param bBlockDevice TRUE if this is a block device, otherwise character device
	void AddDevice (const char *pPrefix, unsigned nIndex, CDevice *pDevice, boolean bBlockDevice);

	/// \param pName	Device name string
	/// \param bBlockDevice TRUE if this is a block device, otherwise character device
	void RemoveDevice (const char *pName, boolean bBlockDevice);
	/// \param pPrefix	Device name prefix string
	/// \param nIndex	Device name index
	/// \param bBlockDevice TRUE if this is a block device, otherwise character device
	void RemoveDevice (const char *pPrefix, unsigned nIndex, boolean bBlockDevice);

	/// \param pName	Device name string
	/// \param bBlockDevice TRUE if this is a block device, otherwise character device
	/// \return Pointer to the device object or 0 if not found
	CDevice *GetDevice (const char *pName, boolean bBlockDevice);
	/// \param pPrefix	Device name prefix string
	/// \param nIndex	Device name index
	/// \param bBlockDevice TRUE if this is a block device, otherwise character device
	/// \return Pointer to the device object or 0 if not found
	CDevice *GetDevice (const char *pPrefix, unsigned nIndex, boolean bBlockDevice);

	/// \brief Generate device listing
	/// \param pTarget Device to be used for output
	void ListDevices (CDevice *pTarget);

	/// \return The single CDeviceNameService instance of the system
	static CDeviceNameService *Get (void);

	/// \brief new & delete operator overloading
	/// \return pointer to allocated object
	void* operator new(size_t T) { return (void *)malloc(T); }
	void operator delete (void *ptr) { free(ptr); }

private:
	TDeviceInfo *m_pList;

//	CSpinLock m_SpinLock;
	unsigned int * m_SpinLock;

	static CDeviceNameService *s_This;
};

#endif
