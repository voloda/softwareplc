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

#ifndef _CDEVICEDRIVER_H_
	#define _CDEVICEDRIVER_H_

	#ifdef __RTL__
		#include <rtl_cpp.h>
	#else
		#include <stdio.h>
		#undef rtl_printf

		#define rtl_printf	printf
	#endif

	#include "cspinlock.h"

	#include "cdevicemanager.h"

	extern CDeviceManager<PLC_MAX_DEVICES>	g_KernelPLCDeviceManager;

	/**
	 * Macro for reading current state of
	 * specified bit
	 *
	 * @param val	Value from which to read bit state
	 * @param bit	Bit number for reading value (0, 1, ...)
	 */
	#define READ_BIT(val, bit)	((val & (1 << bit)) != 0)

	/**
	 * Macro for setting current state of
	 * bit to specified value
	 *
	 * @param val		Current value (BYTE, WORD, ...)
	 * @param BitVal	New value of bit (0, 1)
	 * @param bit		Bit number for giving specified 
	 * 			value (0, 1, ...)
	 */
	#define WRITE_BIT(val, BitVal, bit)	((BitVal == 0) ? (val & (~(1 << bit))) : (val | (1 << bit)))

	/**
	 * Virtual class for implementing device drivers
	 * usable for PLC
	 *
	 * @version $Revision: 1.6 $
	 * @author	Voloda <Vladimir.Kloz@dtg.cz>
	 */
	class CDeviceDriver
	{
		private:
		protected:
			/**
			 * Mutex for access locking
			 */
			CSpinLock	m_SpinLock;
			/**
			 * Flag for detection, if driver is already registered.
			 * If value == 0, not registered
			 */
			int	m_iRegistered;

			/**
			 * Minimal PLC address for inputs to which is device registered
			 */
			int	m_iMinAddrIn;
			/**
			 * Maximal PLC address for inputs to which is device registered
			 */
			int	m_iMaxAddrIn;
			/**
			 * Minimal PLC address for outputs to which is device registered
			 */
			int	m_iMinAddrOut;
			/**
			 * Maximal PLC address for outputs to which is device registered
			 */
			int	m_iMaxAddrOut;

			/**
			 * Recalculation of given address - simple moves address
			 * for m_MinAddr bytes down
			 *
			 * @param iInputAddr	If == 0, address is for output, else it's for input
			 * @param iByteAddr	Address given by device manager (logical PLC address)
			 *
			 * @return Recalculated address
			 */
			virtual int RecalculateAddress(int iValueSize, int iInputAddr, int iByteAddr)
			{
				int iRetv;
				
				if (iInputAddr)
				{
					if ((iValueSize + iByteAddr - 1) > m_iMaxAddrIn)
						return -1;
					
					iRetv = iByteAddr - m_iMinAddrIn;
				}
				else
				{
					if ((iValueSize + iByteAddr - 1) > m_iMaxAddrOut)
						return -1;
							
					iRetv = iByteAddr - m_iMinAddrOut;
				}
				
				return iRetv;
			};
		public:
			CDeviceDriver(void)
			{
				m_iRegistered = 0;
			};

			/**
			 * Class destructor - unregisters driver
			 */
			virtual ~CDeviceDriver(void)
			{
				m_SpinLock.Lock();
				
				// driver is automatically unregistered in destructor
				if (m_iRegistered)
				{
					g_KernelPLCDeviceManager.UnregisterDriver(this);
					m_iRegistered = 0;
				}

				m_SpinLock.Unlock();
			};
			
			/**
			 * Method for registration of driver. Must be called after driver
			 * initialization, when ready to server requests.
			 *
			 * @param iMinAddress	Minimal address which is served by device
			 * @param iMaxAddress	Maximal address which is server by device
			 */
			void	RegisterDriver(int iMinAddressIn, int iMaxAddressIn, int iMinAddressOut, int iMaxAddressOut)
			{
				if (m_iRegistered)
				{
					rtl_printf("Device is already registered!\n");
					return;
				}

				if (g_KernelPLCDeviceManager.RegisterDriver(this, iMinAddressIn, iMaxAddressIn, iMinAddressOut, iMaxAddressOut) != 1)
				{
					rtl_printf("Registration of driver failed!\n");
					return;
				}

				m_iMinAddrIn = iMinAddressIn;
				m_iMaxAddrIn = iMaxAddressIn;
				
				m_iMinAddrOut = iMinAddressOut;
				m_iMaxAddrOut = iMaxAddressOut;
				
				m_iRegistered = 1;
			};

			/**
			 * Virtual method for implementing Ixx addressing (reading
			 * value from device)
			 *
			 * @param Value		Target variable for value storage
			 * @param iValueSize	Size of target variable
			 * @param iInputAddr	If == 0, address is for output, else it's for input
			 * @param iByteAddress	Byte possition for address
			 * @param iBitAddress	Bit possition for address. If value == -1, then byte addressing
			 * 			is requested
			 *
			 * @return 1 if value is successfully returned, 0 if an error occured
			 */
			virtual int ReadAddress(void *Value, int iValueSize, int iInputAddr, int iByteAddress, int iBitAddress) = 0;
			
			/**
			 * Virtual method for implementation of value writing into device
			 *
			 * @param Value
			 * @param iValueSize
			 * @param iInputAddr	If == 0, address is for output, else it's for input
			 * @param iByteAddress
			 * @param iBitAddress
			 */
			virtual int SetAddress(void *Value, int iValueSize, int iInputAddr, int iByteAddress, int iBitAddress) = 0;

			/**
			 * Returns identification string for device driver
			 */
			virtual const char* DriverInfo(void) = 0;
	};
#endif

