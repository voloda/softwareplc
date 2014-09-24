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

/*
 * $Revision: 1.2 $
 */
#ifndef _RTL_PLC_ERRORS_H
	#define _RTL_PLC_ERRORS_H

	/*
	 * Exception value when undefined data
	 * conversion on CResult class is requested
	 */
	#define INVALID_DATA_TYPE	0x1

	/*
	 * Exception value when invalid memory
	 * access is requested on CMemory class
	 */
	#define SEGMENTATION_FAULT	0x2
	/*
	 * Exception value, when unsupported memory
	 * access is requested on CMemory class
	 */
	#define MEMORY_NOT_SUPPORTED	0x3

	/*
	 * Exception value triggered when in resource
	 * locking occurs an error
	 */
	#define LOCK_ERROR		0x4

	/*
	 * Exception value triggered when invalid access
	 * to function block or function parameter is detected
	 */
	#define INVALID_ARGUMENT_ACCESS	0x5
#endif

