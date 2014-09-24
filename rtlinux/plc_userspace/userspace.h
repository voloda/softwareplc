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

#ifndef _PLC_USERSPACE_DRIVER_H_

	#define _PLC_USERSPACE_DRIVER_H_

	#include <rtl_cpp.h>
	#include <rtl_fifo.h>
	#include "data_types.h"
	#include "cdevicedriver.h"

/**
 * Size of internal memory for communication
 * between PLC and RT-Linux user-space
 */
#ifndef PLC_USPACE_MEMORY
	#define PLC_USPACE_MEMORY	64
#endif


	/**
	 * Driver for communication between PLC
	 * and RT-Linux user-space. May be used
	 * for visualisation of PLC state and control
	 * of PLC program by user
	 *
	 * @version $Revision: 1.2 $
	 * @author Voloda <Vladimir.Kloz@dtg.cz>
	 */
	class CUserSpaceDriver: public CDeviceDriver
	{
		private:
			/**
			 * Internal buffer for emulating inputs
			 */
			unsigned char	m_InBuff[PLC_USPACE_MEMORY];
			/**
			 * Internal buffer for emulating outputs
			 */
			unsigned char	m_OutBuff[PLC_USPACE_MEMORY];

			/**
			 * Index of first used FIFO for communication
			 * with Linux user-space
			 */
			unsigned int m_iFirstFIFO;

			/**
			 * Handler of RT thread for sending current
			 * values of I/O
			 */
			pthread_t	m_SendThread;

			static void *SendThreadCode(void *t);
		public:
			~CUserSpaceDriver();

			int ReadAddress(void *Value, int iValueSize, int iInputAddr, int iByteAddr, int iBitAddr);
			int SetAddress(void *Value, int iValueSize, int iInputAddr, int iByteAddr, int iBitAddr);
			const char *DriverInfo(void);
			/**
			 * Method for initialization of FIFO
			 * for communication between PLC and RT-Linux
			 * user-space
			 *
			 * @param	iFIFO	Number of FIFO for use
			 *
			 * @return	1 - successfully initialized, 0 - initialization
			 * 		failed
			 */
			int InitializeDriverFIFO(int iFIFO);
			/**
			 * Handler for FIFO incomming messages
			 *
			 * @param iFIFO	FIFO number
			 *
			 * @return	0 - Request handled successfully
			 */
			static int FIFOHandler(unsigned int iFIFO);

			static int FIFOControlHandler(unsigned int iFIFO);
			
	};

#endif

