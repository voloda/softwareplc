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

#ifndef _RTL_PLC_DATA_TYPES_H
	#define _RTL_PLC_DATA_TYPES_H

	/*
	 * Conversion of standard PLC data types defined in
	 * IEC-1131-3, tab.10 to data types used in C/C++
	 * Numeric data types with 64bits len are not
	 * defined
	 */
	typedef char		SINT;
	typedef short int	INT;
	typedef int		DINT;
	typedef	unsigned char	USINT;
	typedef unsigned short int	UINT;
	typedef unsigned int 	UDINT;
	typedef double 		REAL;

	/*
	 * time is stored as multiplicator for 100us
	 * so 10 means 1ms, 100 == 10ms etc.
	 */
	typedef unsigned long	TIME;
/*	typedef DATE
	typedef TIME_OF_DAY
	typedef TOD	TIME_OF_DAY
	typedef DATE_AND_TIME
	typedef DT	DATE_AND_TIME
*/
	typedef unsigned char STRING;

//	typedef unsigned char		BIT;
//	typedef unsigned char		BYTE;
//	typedef unsigned short int	WORD;
//	typedef unsigned int		DWORD;
	#define BIT	USINT
	#define BOOL	BIT
	#define	BYTE	USINT
	#define WORD	UINT
	#define DWORD	UDINT
#endif

