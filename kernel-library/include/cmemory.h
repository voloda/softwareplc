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
 *  Version: $Revision: 1.7 $
 */

#ifndef _RTL_PLC_CMEMORY_H
	#define _RTL_PLC_CMEMORY_H

	#ifdef __RTL__
		#include <rtl_cpp.h>

		#define PLC_MEM_ERROR	rtl_printf
	#else
		#include <string.h>
		#include <stdio.h>
		#include <iostream>

		using namespace std;

		#define PLC_MEM_ERROR	printf
	#endif


	#include "plc_errors.h"
	#include "data_types.h"
	#include "cspinlock.h"
	#include "cdevicemanager.h"
	#include "plc_compat.h"

	extern CDeviceManager<PLC_MAX_DEVICES>	g_KernelPLCDeviceManager;
	/*
	 * Macros for access to PLC memory.
	 * See IEC-1131-3, tab.15
	 */

	/**
	 * Mark for PLC input memory location
	 * (memory location of type I)
	 */
	#define	PLC_INPUT	(1 << 0)
	/**
	 * Mark for PLC output memory location
	 * (memory location of type Q)
	 */
	#define PLC_OUTPUT	(1 << 1)
	/**
	 * Mark for PLC internal memory location
	 * (memory location of type M)
	 */
	#define PLC_MEMORY	(1 << 2)
	/**
	 * Mark for PLC bit memory access
	 * (access of type X or none)
	 */
	#define PLC_BIT		(1 << 3)
	/**
	 * Mark for PLC byte (8bits) memory access
	 * (access of type B)
	 */
	#define PLC_BYTE	(1 << 4)
	/**
	 * Mark for PLC word (16bits) memory access
	 * (access of type W)
	 */
	#define PLC_WORD	(1 << 5)
	/**
	 * Mark for PLC double word (32bits) memory access
	 * (access of type D)
	 */
	#define PLC_DWORD	(1 << 6)

	/**
	 * Implementation of PLC memory access
	 * 
	 * This class implements complete memory access
	 * for emulated PLC device.
	 * Class provides internal data memory with
	 * user defined size, provides conversion
	 * for inputs and outputs of A/D cards based
	 * on given address.
	 * 
	 * Memory is conforming to IEC-1131-3, tab.15
	 * 
	 * @param iInternalSize - size of PLC memory in Bytes
	 *
	 * @version $Revision: 1.7 $
	 * @author Voloda <Vladimir.Kloz@dtg.cz>
	 */
	template<int iInternalSize = 16384> class CMemory
	{
		private:
			/**
			 * Static buffer for internal memory allocation.
			 * Size may be given as parameter for template
			 */
			unsigned char	m_Data[iInternalSize];
			
			/**
			 * Mutex for memory access locking when
			 * multiple PLC threads are defined
			 */
			CSpinLock	m_SpinLock;

			/**
			 * Function parses address given for methods SetValue() and GetValue()
			 * 
			 * @param iAddrType	Contains parsed informations about given address as mask of
			 * 			values: PLC_MEMORY - internal memory address, PLC_OUTPUT - address
			 * 			is output of D/A card, PLC_INPUT - address is input of A/D card,
			 * 			PLC_BIT - bit address, PLC_BYTE - byte address, PLC_WORD - word
			 * 			address, PLC_DWORD - dword address
			 * @param iByte		Byte part of address
			 * @param iBit		Bit part of address - valid only if mask iAddrType contains mask
			 * 			PLC_BIT
			 * @param pAddr		Given PLC address as defined in IEC-1131-3
			 *
			 * @return If address is valid return 1, else 0
			 */
			inline int ParseAddr(unsigned int& iAddrType, unsigned int& iByte, unsigned int& iBit, const char *pAddr)
			{
				char	*pPointPos;
				char	pBuff[32];	// static buffer - may be an buffer overflow if address len is > 31 bytes

				iAddrType = 0;
				iBit = -1;

				switch(pAddr[0])
				{
					case 'M':
						iAddrType = PLC_MEMORY;
						break;
					case 'Q':
						iAddrType = PLC_OUTPUT;
						break;
					case 'I':
						iAddrType = PLC_INPUT;
						break;
				}
				
				pAddr++;

				switch(pAddr[0])
				{
					case 'B':
						iAddrType |= PLC_BYTE;
						pAddr++;
						break;
					case 'W':
						iAddrType |= PLC_WORD;
						pAddr++;
						break;
					case 'D':
						iAddrType |= PLC_DWORD;
						pAddr++;
						break;
					case 'X':
						iAddrType |= PLC_BIT;
						pAddr++;
						break;
					default:
						iAddrType |= PLC_BIT;
						break;
				}

				// if it's bit address and no separator is found,
				// split address into bit and byte
				if (iAddrType & PLC_BIT)
				{
					if ((pPointPos = strchr(pAddr, '.')) == NULL)
					{
						int	iAddr;

						iAddr = plc_atoui(pAddr);

						iByte = (iAddr / 8);
						iBit = (iAddr % 8);
					
						return 1;
					}
					
					strncpy(pBuff, pAddr, pPointPos - pAddr);
					pBuff[pPointPos - pAddr] = 0;

					iByte = plc_atoui(pBuff);
					iBit = plc_atoui(pPointPos + 1);

					return 1;
				}

				iByte = plc_atoui(pAddr);

				if (iAddrType & PLC_BYTE)
				{
					return 1;
				}
				if (iAddrType & PLC_WORD)
				{
					iByte *= 2;
					return 1;
				}

				if (iAddrType & PLC_DWORD)
				{
					iByte *= 4;
					return 1;
				}
				
				return 0;
			};

			/**
			 * Function checks given address range for
			 * internal memory address
			 *
			 * @param iByte		Byte address (range 0-iInternalSize)
			 * @param iBit		Bit address (range 0-7)
			 *
			 * @return	If given range is valid, return 1, else 0
			 */
			inline int CheckMemoryRange(unsigned int iByte, unsigned int iBit, unsigned int iVarSize)
			{
				if (((iByte + iVarSize - 1) >= iInternalSize) || ((iBit > 7) && (iBit != ~((unsigned int ) 0))))
				{
					PLC_MEM_ERROR("CMemory: Memory address out of range (%d, %d)\n", iByte, iBit);
					return 0;
				}
				return 1;
			};
		public:
			/**
			 * Method implementing access to values stored
			 * in internal memory or on digital inputs.
			 * Function is automaticaly generated for
			 * data types, which are used.
			 *
			 * @param Value		Returned value on specified PLC memory address
			 * @param pAddr		Given PLC address as defined in IEC-1131-3
			 */
			template<class GetType> void GetValue(GetType& Value, const char *pAddr)
			{
				unsigned int iAddrType, iByteAddr, iBitAddr;

				if (ParseAddr(iAddrType, iByteAddr, iBitAddr, pAddr) == 0)
					return;
				
				// internal memory access
				if (iAddrType & PLC_MEMORY)
				{
					if (CheckMemoryRange(iByteAddr, iBitAddr, sizeof(GetType)) == 0)
						return;
					
					m_SpinLock.Lock();

					if (iAddrType & PLC_BIT)
					{
						if (m_Data[iByteAddr] & (1 << iBitAddr))
							Value = 1;
						else
							Value = 0;

						m_SpinLock.Unlock();
						return;
					}
					
					Value = (*(GetType *) (m_Data + iByteAddr));

					m_SpinLock.Unlock();

					return;
				}

				// I/O card adddress - redirect call to card driver
				g_KernelPLCDeviceManager.GetValue(Value, (iAddrType & PLC_INPUT), iByteAddr, iBitAddr);
			};
			
			/**
			 * Method implementing storage of values into
			 * requested memory or to digital output.
			 * Function is automaticaly generated for
			 * data types, which are used
			 *
			 * @param Value		Value, which will be stored into specified address
			 * @param pAddr		Given PLC address as defined in IEC-1131-3
			 */
			template<class SetType> void SetValue(const SetType Value, const char *pAddr)
			{
				unsigned int iAddrType, iByteAddr, iBitAddr;

				if (ParseAddr(iAddrType, iByteAddr, iBitAddr, pAddr) == 0)
					return;


				// internal memory access
				if (iAddrType & PLC_MEMORY)
				{
					if (CheckMemoryRange(iByteAddr, iBitAddr, sizeof(SetType)) == 0)
						return;

					m_SpinLock.Lock();
					
					if (iAddrType & PLC_BIT)
					{
						if (Value == 0)
							m_Data[iByteAddr] &= (unsigned char) ~(1 << iBitAddr);
						else
							m_Data[iByteAddr] |= (unsigned char) (1 << iBitAddr);
						
						m_SpinLock.Unlock();

						return;
					}

					(* (SetType*) (m_Data + iByteAddr)) = Value;

					m_SpinLock.Unlock();

					return;
				}
				
				// I/O card address - redirect call to card driver
				g_KernelPLCDeviceManager.SetValue(Value, (iAddrType & PLC_INPUT), iByteAddr, iBitAddr);
				

			};

	};
#endif

