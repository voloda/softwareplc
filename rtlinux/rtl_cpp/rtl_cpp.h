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
 * rtlcpp.h - C++ support for RTL
 * Michael Barabanov, <baraban@fsmlabs.com>
 *
 * Ideas and code from David Olofson, Yunho Jeon and myself
 */

#ifndef __KERNELCPP__
#define __KERNELCPP__

#ifdef __cplusplus
extern "C" {
#endif

#define new _new
#define virtual _virtual
#define NULL 0
#include <linux/kernel.h>
#include <linux/module.h>
#include <rtl_sched.h>
#include <rtl_fifo.h>
#include <rtl_time.h>
#include <pthread.h>

#undef new
#undef virtual
#ifdef __cplusplus
}
#endif

extern "C" {

void *kmalloc(unsigned size, int prio);
void kfree(const void *p);
};

extern "C" {
	int __do_global_ctors_aux();
	int __do_global_dtors_aux();
}

#ifdef __CPP_INIT__

void *operator new (unsigned size) {
  return kmalloc(size,0);
}

void *operator new[] (unsigned size) {
  return kmalloc(size,0);
}

void operator delete (void *p) {
  kfree(p);
}

void operator delete[] (void *p) {
  kfree(p);
}

extern "C" {
	int init_module();
	void cleanup_module();
}

#endif
#endif
