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
** PLEASE REFER TO THE LICENSE INCLUDED IN THE DISTRIBUTION.
** 
*******************************************************************************/
#ifndef _RPI_SYNCHRONIZE_H_
#define _RPI_SYNCHRONIZE_H_

#define ARM_ALLOW_MULTI_CORE 1

#ifdef __cplusplus
extern "C" {
#endif

//
// Execution levels
//
#define TASK_LEVEL              0              // IRQs and FIQs enabled
#define IRQ_LEVEL               1              // IRQs disabled, FIQ enabled
#define FIQ_LEVEL               2              // IRQs and FIQs disabled

//
// Interrupt control
//
#define EnableIRQs()		asm volatile ("cpsie i")
#define DisableIRQs()		asm volatile ("cpsid i")
#define	EnableInterrupts()	EnableIRQs()
#define	DisableInterrupts()	DisableIRQs(

#define EnableFIQs()		asm volatile ("cpsie f")
#define DisableFIQs()		asm volatile ("cpsid f")
void EnterCritical(unsigned);
void LeaveCritical(void);

#if (defined  RPI == 2) || (defined RPI == 3 )

//
// Barriers
//
#define DataSyncBarrier()	__asm volatile ("dsb" ::: "memory")
#define DataMemBarrier() 	__asm volatile ("dmb" ::: "memory")

#define PeripheralEntry()	((void) 0)
#define PeripheralExit()	((void) 0)
#define InstructionSyncBarrier() __asm volatile ("isb" ::: "memory")
#define InstructionMemBarrier()	__asm volatile ("isb" ::: "memory")
#define isb() asm volatile ("isb" ::: "memory")
#define dsb() asm volatile ("dsb" ::: "memory")
#define dmb() asm volatile ("dmb" ::: "memory")

//
// Cache control
//
#define invalidate_instruction_cache()	asm volatile ("mcr p15, #0, %[zero], c7, c5,  #0" : : [zero] "r" (0) : "memory")
#define flush_prefetch_buffer()			asm volatile ("isb" ::: "memory")
#define flush_branch_target_cache() 	asm volatile ("mcr p15, #0, %[zero], c7, c5,  #6" : : [zero] "r" (0) : "memory")

extern void invalidate_data_cache(void) __attribute__ ((optimize (3)));
extern void clean_data_cache(void) __attribute__ ((optimize (3)));
extern void invalidate_data_cache_l1_only(void) __attribute__ ((optimize (3)));

extern void invalidate_data_cache_range (u32 nAddress, u32 nLength) __attribute__ ((optimize (3)));
extern void clean_data_cache_range (u32 nAddress, u32 nLength) __attribute__ ((optimize (3)));
extern void clean_and_invalidate_data_cache_range (u32 nAddress, u32 nLength) __attribute__ ((optimize (3)));

#elif !(defined  RPI == 3 )
	#define invalidate_instruction_cache()	asm volatile ("mcr p15, #0, %[zero], c7, c5,  #0" : : [zero] "r" (0) )
	#define flush_prefetch_buffer()			asm volatile ("mcr p15, #0, %[zero], c7, c5,  #4" : : [zero] "r" (0) )
	#define flush_branch_target_cache()		asm volatile ("mcr p15, #0, %[zero], c7, c5,  #6" : : [zero] "r" (0) )
	#define invalidate_data_cache()			asm volatile ("mcr p15, #0, %[zero], c7, c6,  #0" : : [zero] "r" (0) )
	#define clean_data_cache()				asm volatile ("mcr p15, #0, %[zero], c7, c10, #0" : : [zero] "r" (0) )


//
// Barriers
//
#define isb() asm volatile ("mcr p15, #0, %[zero], c7, c5,  #4" : : [zero] "r" (0) )
#define dsb() asm volatile ("mcr p15, #0, %[zero], c7, c10, #4" : : [zero] "r" (0) )
#define dmb() asm volatile ("mcr p15, #0, %[zero], c7, c10, #5" : : [zero] "r" (0) )
#define DataSyncBarrier()	dsb()
#define DataMemBarrier() 	dmb()

#define PeripheralEntry()	DataSyncBarrier()
#define PeripheralExit()	DataMemBarrier()

#define InstructionSyncBarrier() flush_prefetch_buffer()
#define InstructionMemBarrier()	flush_prefetch_buffer()
#endif

#define CompilerBarrier()	__asm volatile ("" ::: "memory")

#ifdef __cplusplus
}
#endif
#endif /* SYNCHRONIZE_H_ */
