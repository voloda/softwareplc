/*
 *  This file is part of Software PLC for RT-Linux.
 *
 *  Software PLC for RT-Linux is free software; you can redistribute 
 *  it and/or modify it under the terms of the GNU General Public 
 *  License as published by the Free Software Foundation; either 
 *  version 2 of the License, or (at your option) any later version.
 *
 *  Software PLC for RT-Linux is distributed in the hope that it 
 *  will be useful, but WITHOUT ANY WARRANTY; without even the implied 
 *  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
 *  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Software PLC for RT-Linux; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Author: Vladimir Kloz <Vladimir.Kloz@dtg.cz>
 *  Project home: http://sourceforge.net/projects/softwareplc
 *  Version: $Revision: 1.4 $
 */

/*
 * $Revision: 1.4 $
 */

#ifndef _RTL_PLC_CMEMORYMANAGEMENT_H
	#define _RTL_PLC_CMEMORYMANAGEMENT_H

	#include <stdlib.h>
	#include "cmutex.h"

	/*
	 * Structure used for marking allocated
	 * and freed memory blocks
	 */
	struct MemBlock
	{
		/*
		 * Pointer to next block. If is NULL,
		 * this is last block
		 */
		struct MemBlock		*pNextBlock;
		/*
		 * Size of this blocks (usable memory)
		 * in bytes
		 */
		unsigned int		iBlockSize;
		/*
		 * If 1, memory block is used, if 0,
		 * block is free
		 */
		unsigned short int	Used:	1;
		/*
		 * Index of memory fragment to which
		 * is this allocated block positioned
		 */
		unsigned short int	iPageIndex:	15;
		/*
		 * Checksum of pointer to next address.
		 * Checksum is computed as mod 57203 of
		 * sum of pointer pNextBlock and iBlockSize
		 */
		unsigned short int	iCheckSum;
	};

	/*
	 * Class for implementation of memory management,
	 * which is necessary in RT-Linux.
	 * In RT-Linux thread can be memory allocated only
	 * when kernel module is loaded. After this, it's
	 * not recomended allocate or release memory, because
	 * system can become unstable
	 */
	class CMemoryManagement
	{
		private:
			/*
			 * Array used for storing pointers to
			 * kernel allocated pages in memory.
			 * After last page NULL record is
			 * inserted
			 */
			void	**m_pAllocatedMemory;
			/*
			 * Pointer to structure with last
			 * freed memory block or NULL.
			 */
			struct MemBlock	*m_pLastFreedBlock;
			/*
			 * Pointer to structure with last
			 * allocated block in memory
			 */
			struct MemBlock *m_pLastAllocBlock;
			/*
			 * Mutex for locking access to 
			 * memory management class
			 */
			CMutex		m_Lock;
			/*
			 * Computes checksum for given memory
			 * block and returns it
			 */
			inline unsigned short int ComputeCheckSum(const struct MemBlock& Block);
			/*
			 * Returns 1 (true), if checksum for block is
			 * valid, or 0 (false), if block is corrupted
			 */
			inline int isCheckSumValid(const struct MemBlock& Block);     	
			/*
			 * Initialize memory block structure
			 */
			inline struct MemBlock * InitMemoryBlock(void *pWhere, unsigned short int iFragmentIndex = 0);
			
			/*
			 * Searches given memory page for free
			 * block of given index. If exists, return
			 * pointer to struc MemBlock. If not enough
			 * space in fragment, return NULL
			 */
			inline struct MemBlock * FindSpaceInPage(int iPageIndex, unsigned int iRequestedSpace);
			
			/*
			 * Calculates number of bytes, which will be
			 * allocated from requested size
			 */
			inline unsigned int ComputeRealMemSize(unsigned int iMemSize);
		public:
			/*
			 * Class constructor. Parameter iMemorySize
			 * defines allocated memory size. Under
			 * RT-Linux memory is allocated via kmalloc
			 * kernel call.
			 */
			CMemoryManagement(int iMemorySize = 16384);
			/*
			 * Class destruction
			 */
			~CMemoryManagement(void);
			/*
			 * Allocates memory with iSize bytes.
			 * If allocation is successfull, returns
			 * pointer to free memory, else returns
			 * NULL
			 */
			void *Alloc(size_t iSize);
			/*
			 * Free previously allocated memory by
			 * CMemoryManagement class.
			 */
			void Free(void *pMemory);
	};

#endif

