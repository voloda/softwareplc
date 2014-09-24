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


#ifndef _RTL_PLC_CSPINLOCK_H
	#define _RTL_PLC_CSPINLOCK_H
	
	#ifdef __RTL__
		#include <rtl_cpp.h>
		#include <rtl_spinlock.h>
	#else
		#include <pthread.h>
	#endif

	/**
	 * Implementation of CSpinLock class - class
	 * provides Locking and Unlocking functionality
	 * needed when accesing shared resources as
	 * memory, digital I/O etc.
	 *
	 * @version $Revision: 1.4 $
	 * @author Voloda <Vladimir.Kloz@dtg.cz>
	 */
	class CSpinLock
	{
		private:
			/**
			 * Space for informations about SpinLock
			 * data
			 */
			pthread_spinlock_t	m_SpinLock;
		public:
			/**
			 * Class constructor
			 */
			CSpinLock(void)
			{
				pthread_spin_init(&m_SpinLock, 0);
			}
			
			/**
			 * Class destructor
			 */
			~CSpinLock(void)
			{
				pthread_spin_destroy(&m_SpinLock);
			}
			/**
			 * Provides locking functionality for
			 * spinlock. If iBlocking == 1, then
			 * function waits until spinlock is locked.
			 * else call is unblocking
			 *
			 * @param iBlocking	If value is 1, call is blocking, else only
			 * 			test for mutex lock is called.
			 * 			
			 * @return If an error occurs, return value is 0, else 1.
			 */
			inline int Lock(int iBlocking = 1)
			{
				if (iBlocking)
				{
					if (pthread_spin_lock(&m_SpinLock) != 0)
						return 0;
				}
				else
				{
					if (pthread_spin_trylock(&m_SpinLock) != 0)
						return 0;
				}
				return 1;

			}
			
			/**
			 * Unlocking function for unlock of previously
			 * locked spinlock
			 *
			 * @return If an error occurs, return value is 0, else 1.
			 */
			inline int Unlock(void)
			{
				if (pthread_spin_unlock(&m_SpinLock) != 0)
					return 0;
				return 1;
			}
	};
	
#endif

