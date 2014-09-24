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
 *  Version: $Revision: 1.6 $
 */

#ifndef _SOFTWARE_PLC_H_
	#define _SOFTWARE_PLC_H_

	#include <rtl_cpp.h>

	/*
	 * Software PLC main header file
	 */

	#include "data_types.h"

	#include "cmemory.h"
//	#include "cmemorymanagement.h"
	#include "cmutex.h"
	#include "cspinlock.h"
	#include "cpropertyin.h"
	#include "cpropertyout.h"
	#include "cpropertyio.h"
	#include "cresult.h"
	#include "cvariable.h"
	#include "plc_errors.h"
	#include "plc_compat.h"

	/*
	 * Module global variable for access to
	 * PLC memory emulation
	 */
	extern CMemory<PLC_MEMORY_SIZE>	g_KernelPLCMemory;
#endif

