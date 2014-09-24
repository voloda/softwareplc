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

#define __CPP_INIT__

#include <rtl_cpp.h>
#include "drv.h"

static CLabCardDriver	plcLabCardDriver;

MODULE_PARM(iladdr, "i");
MODULE_PARM(oladdr, "i");
MODULE_PARM_DESC(iladdr, "Input lowest registration address");
MODULE_PARM_DESC(oladdr, "Output lowest registration address");

static int iladdr = 0;
static int oladdr = 0;
/*
 * Module initialization
 */
int init_module(void) {
	
	__do_global_ctors_aux();
	
	if (plcLabCardDriver.FindCard())
		plcLabCardDriver.RegisterDriver(iladdr, iladdr + 1, oladdr, oladdr + 1);
	else
		return -6;

	return 0;
}

/*
 * Module cleanup - deletes all initialized threads
 */
void cleanup_module(void) {

	__do_global_dtors_aux();
}

MODULE_LICENSE("GPL");

