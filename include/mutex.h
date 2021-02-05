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
#ifndef _MUTEX_H_
#define _MUTEX_H_
#include <sys/types.h>
#include <stdbool.h>
#include "sysconfig.h"

/*****************************************************************************/
/*
** Macros which can be passed to Shared APIs
*/
#define MUTEX_WRITE        0 // Mutex for write 
#define MUTEX_READ         1 // Mutex for read 
#define MUTEX_CBLOCK       2 // Mutex for control block 
#define MUTEX_ALLOC        3 // Mutex for allocator 
#define MUTEX_TASK         4 // Mutex for task 
#define MUTEX_GPIO         5 // Mutex for gpio 
#define MUTEX_BT           6 // Mutex for bluetooth 
#define MUTEX_UG           7 // Mutex for graphics 

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
** Mutex APIs
******************************************************************************/
extern unsigned int *get_mutex(int);
extern void spin_lock(register void * mutex);
extern void spin_unlock(register void * mutex);

#ifdef __cplusplus
}
#endif
#endif /* __MUTEX_H__ */
