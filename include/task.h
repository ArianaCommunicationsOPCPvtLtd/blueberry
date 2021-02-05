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
#ifndef _TASK_H_
#define _TASK_H_

#include "mutex.h"
#include <string.h>


/*****************************************************************************/
/*
** Macros which can be passed to Shared APIs
*/
#define SUCCESS 0
#define ERROR   1


#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

/*****************************************************************************/
/*
** API prototypes
*/

extern void initTasks( void );
extern void printTasks( void );
extern int execTask(void (*funcptr)(void*), void *funcblock);
extern int forkTask(void *funcblock);
extern void sleepTask( uint32 usec);
extern bool yieldTask( void );
extern void blockTask( void *ptr );
extern void sleepBlockedTask( void *ptr, uint32 usec );
extern void wakeTask(void *ptr);
extern void setTaskList(void* ptr);
extern void setCurrentTask(void);
extern void setTaskMutex(void);
extern void* getIdleTask();
extern bool scheduleLockedTask(void *);
extern unsigned getClockTicks(void);

#ifdef __cplusplus
}
#endif
#endif /* __TASK_H__ */
