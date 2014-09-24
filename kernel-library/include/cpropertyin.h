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
 *  Version: $Revision: 1.2 $
 */

#ifndef _RTL_PLC_CPROPERTYIN_H
	#define _RTL_PLC_CPROPERTYIN_H

	#ifdef __RTL__
		#include <rtl_cpp.h>
	#else
		#include <stdio.h>
		
		#define rtl_printf	printf
	#endif

	#include "plc_errors.h"
	#include "cvariable.h"

	/**
	 * Template class implementing parameters
	 * for functions, function blocks and programs
	 * with access to variables as defined in IEC-1131-3
	 *
	 * @version	$Revision: 1.2 $
	 * @author	Voloda <Vladimir.Kloz@dtg.cz>
	 */
	template<class DataType> class CPropertyIn
	{
		private:
			/**
			 * Pointer to associated variable
			 * must be given to work properly
			 */
			CVariable<DataType>	*m_pStoredData;

		public:
			/**
			 * Class constructor
			 * 
			 * @param Variable	Pointer to initialized Variable, which
			 * 			is used for data manipulation
			 * 			
			 * @param iAccess	External access to property. It's
			 *              	combination of PLC_PROPERTY_*_ACCESS
			 *              	macros. When invalid access is done,
			 *              	warning through rtl_printf() is showed
			 */
			CPropertyIn(CVariable<DataType> *Variable = NULL)
			{
				m_pStoredData = Variable;
			}

			/**
			 * Insertion of new value into associated variable
			 */
			inline CPropertyIn<DataType>& operator=(const DataType Value)
			{
				
				(*m_pStoredData) = Value;

				return(*this);
			};

			/**
			 * Copying constructor for class
			 */
			CPropertyIn<DataType>& operator =(const CPropertyIn<DataType> &Property)
			{
				if (this != &Property)
				{
					m_pStoredData = Property.m_pStoredData;
				}
				return(*this);
			};
	};
#endif

