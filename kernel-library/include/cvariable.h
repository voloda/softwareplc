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
 *  Version: $Revision: 1.5 $
 */

#ifndef _RTL_PLC_CVARIABLE_H
	#define _RTL_PLC_CVARIABLE_H

	#ifdef __RTL__
		#include <rtl_cpp.h>
	#else
	#endif

	#include "cmemory.h"

	extern CMemory<PLC_MEMORY_SIZE> g_KernelPLCMemory;

	/**
	 * Template class fully implementing variables with
	 * all features as defined in IEC-1131-3
	 *
	 * For some data types, exceptions will be needed!
	 *
	 * @version	$Revision: 1.5 $
	 * @author	Voloda <Vladimir.Kloz@dtg.cz>
	 */
	template<class DataType> class CVariable
	{
		private:
			/**
			 * Current value of variable if value
			 * isn't stored in PLC emulated memory
			 * (no PLC address is given)
			 */
			DataType	m_StoredData;
			/**
			 * Static buffer for storage of PLC memory
			 * address. If first character == 0, no address
			 * is given.
			 */
			char	m_pAddress[30];

			/**
			 * Method for valid initialization of variable
			 * address
			 *
			 * @param pAddr		NULL if value is stored localy or address of variable
			 * 			as zero terminated string
			 */
			inline void InitVarAddr(const char *pAddr)
			{
				if (pAddr != NULL)
					memcpy(m_pAddress, pAddr, plc_strlen(pAddr) + 1);
				else
					m_pAddress[0] = 0;
			};

			/**
			 * If variable is stored in instance variable
			 * m_Stored data, return TRUE, else FALSE
			 *
			 * @return	TRUE if variable is stored in m_StoredData,
			 * 		FALSE if stored in PLC emulated memory
			 */
			inline int isLocal(void)
			{
				return (m_pAddress[0] == 0);
			};

			/**
			 * Set current value of variable depends on variable
			 * location (local or address)
			 *
			 * @param NewValue	New value of variable
			 */
			inline void SetVarValue(const DataType NewValue)
			{
				if (this->isLocal())
					m_StoredData = NewValue;
				else
					g_KernelPLCMemory.SetValue(NewValue, m_pAddress);
			};
		public:
			/**
			 * Class construction (default or with given variable
			 * location)
			 * 
			 * @param pAddr		Address of variable in PLC memory
			 * @param iInit		If != 0, initializes value, else
			 * 			use actual value of variable
			 */
			CVariable(const char *pAddr = NULL, int iInit = 1)
			{
				InitVarAddr(pAddr);

				if (iInit)
					SetVarValue(0);
			};

			/**
			 * Class construction with specified initial value
			 * and with or without given variable address
			 *
			 * @param Value		Initial value of internal variable
			 * @param pAddr		Address of variable in PLC memory
			 */
			CVariable(DataType Value, const char *pAddr = NULL)
			{
				InitVarAddr(pAddr);

				SetVarValue(Value);
			};
			
			/**
			 * Operator for returning current value
			 * of variable. Proper location and address of variable
			 * is used if defined
			 *
			 * @return Current value stored in variable
			 */
			operator const DataType()
			{
				DataType Value;
				
				if (isLocal())
					return m_StoredData;
				
				g_KernelPLCMemory.GetValue(Value, m_pAddress);

				return Value;
			};
			
			/**
			 * Operator for setting current value of variable.
			 * Proper location and address of variable is used
			 * if defined
			 *
			 * @param NewValue	New associated value of variable
			 */
			CVariable<DataType>& operator =(const DataType NewValue)
			{
				SetVarValue(NewValue);

				return(*this);
			};

			/**
			 * Operator for class copying
			 *
			 * @param Variable	Variable from which copy attributes
			 */
			CVariable<DataType>&	operator=(const CVariable<DataType> &Variable)
			{
				if (this != &Variable)
				{
					DataType	TempValue;
					
					if (Variable.m_pAddress[0] == 0)
					{
						InitVarAddr(NULL);

						TempValue = Variable.m_StoredData;
					}
					else
					{
						InitVarAddr(Variable.m_pAddress);

						g_KernelPLCMemory.GetValue(TempValue, Variable.m_pAddress);
					}

					SetVarValue(TempValue);
				}

				return(*this);
			}
	};
#endif

