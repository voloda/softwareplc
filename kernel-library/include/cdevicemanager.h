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

#ifndef _CDEVICEMANAGER_H_
	#define _CDEVICEMANAGER_H_

	#ifdef __RTL__
		#include <rtl_cpp.h>
	#else
		#include <stdio.h>
		
		#undef rtl_printf
		#define rtl_printf	printf
		
	#endif

	#include "cmutex.h"

	class CDeviceDriver;

	/**
	 * Structure for storage of informations about registered
	 * PLC I/O drivers.
	 */
	struct PLCDriverRegistration
	{
		/**
		 * Pointer to instance of device driver, if == NULL,
		 * all others values are not valid!
		 */
		CDeviceDriver	*pDriver;
		/**
		 * Lower bound for address of inputs, must be -1
		 * if no inputs are supported
		 */
		int		iMinAddrIn;
		/**
		 * Upper bound for address of inputs, must be -1
		 * if no inputs are supported
		 */
		int		iMaxAddrIn;
		/**
		 * Lower bound for address of outputs, must be -1,
		 * if no outputs are supported
		 */
		int		iMinAddrOut;
		/**
		 * Upper bound for address of outputs, must be -1
		 * if no outputs are supported
		 */
		int		iMaxAddrOut;
	};

	/**
	 * Class for managing I/O cards access
	 * Every driver, which should be available
	 * for PLC programs as input (Ixx) address
	 * or as output (Qxx) must register here.
	 *
	 * @param	iMaxDrivers	Maximum number of drivers, which should register
	 * @version	$Revision: 1.5 $
	 * @author	Voloda <Vladimir.Kloz@dtg.cz>
	 */
	template<int iMaxDrivers> class CDeviceManager
	{
		private:
			/**
			 * Mutex for locking operations with array
			 * m_RegisteredDrivers
			 */
			CMutex	m_Mutex;
			/**
			 * Array of structures for storage of informations
			 * about available registered drivers
			 */
			struct PLCDriverRegistration	m_RegisteredDrivers[iMaxDrivers];

			/**
			 * Function finds index in instance array m_RegisteredDrivers
			 * for given address and type.
			 *
			 * @param iInputAddress	If == 0, output address is given, else input address
			 * @param iByteAddr	Given byte address
			 *
			 * @return Index of driver for given address or -1 if no driver found
			 */
			inline int FindDriver(int iInputAddress, int iByteAddr)
			{
				if (iInputAddress)
				{
					for (int i = 0; i < iMaxDrivers; i++)
					{
						if (m_RegisteredDrivers[i].pDriver == NULL)
							continue;
						
						if ((m_RegisteredDrivers[i].iMinAddrIn <= iByteAddr) && (m_RegisteredDrivers[i].iMaxAddrIn >= iByteAddr))
							return i;
					}
				}
				else
				{
					for (int i = 0; i < iMaxDrivers; i++)
					{
						if (m_RegisteredDrivers[i].pDriver == NULL)
							continue;

						if ((m_RegisteredDrivers[i].iMinAddrOut <= iByteAddr) && (m_RegisteredDrivers[i].iMaxAddrOut >= iByteAddr))
							return i;
					}
				}

				return -1;
			};
		public:
			/**
			 * Class constructor - initializes internal
			 * structure with informations about drivers
			 */
			CDeviceManager(void)
			{
				
				for (int i = 0; i < iMaxDrivers; i++)
				{
					m_RegisteredDrivers[i].pDriver = NULL;
				}

			};
			
			~CDeviceManager(void)
			{
			};

			/**
			 * Registration for new I/O card driver
			 *
			 * @param pDriver	Pointer to existing instance of driver class
			 * @param iMinAddr	Minimal address, which is driver 
			 * @param iMaxAddr	Maximal address, which is driver
			 *
			 * @return	1 if driver is successfully registered, 0 when an error
			 *		occured
			 */
			int RegisterDriver(CDeviceDriver *pDriver, int iMinAddrIn, int iMaxAddrIn, int iMinAddrOut, int iMaxAddrOut)
			{
				m_Mutex.Lock();
				
				if ((FindDriver(1, iMinAddrIn) != -1) || (FindDriver(0, iMinAddrOut) != -1))
				{
					m_Mutex.Unlock();
					return 0;
				}
				for (int i = 0; i < iMaxDrivers; i++)
				{
					if (m_RegisteredDrivers[i].pDriver == NULL)
					{
						rtl_printf("New PLC driver registered: %s\n", pDriver->DriverInfo());

						m_RegisteredDrivers[i].pDriver		= pDriver;
						m_RegisteredDrivers[i].iMinAddrIn	= iMinAddrIn;
						m_RegisteredDrivers[i].iMaxAddrIn	= iMaxAddrIn;
						m_RegisteredDrivers[i].iMinAddrOut	= iMinAddrOut;
						m_RegisteredDrivers[i].iMaxAddrOut	= iMaxAddrOut;

						m_Mutex.Unlock();

						return 1;
					}
				}

				m_Mutex.Unlock();
				// no free entry for driver registration
				return 0;
			};

			/**
			 * Unregister I/O card driver
			 *
			 * @param pDriver	Pointer to instance of driver class which should
			 * 			be unregistered
			 *
			 * @return	1 - driver is unregistered, 0 when an error occured
			 */
			int UnregisterDriver(CDeviceDriver *pDriver)
			{
				m_Mutex.Lock();
				
				for (int i = 0; i < iMaxDrivers; i++)
				{
					if (m_RegisteredDrivers[i].pDriver == pDriver)
					{
						m_RegisteredDrivers[i].pDriver = NULL;

						m_Mutex.Unlock();

						return 1;
					}
				}

				m_Mutex.Unlock();

				return 0;
			};

			template<class DataType> void GetValue(DataType &Value, int iInputAddr, int iByteAddr, int iBitAddr)
			{
				int iIndex;

				m_Mutex.Lock();
				
				iIndex = FindDriver(iInputAddr, iByteAddr);

				if (iIndex == -1)
				{
					m_Mutex.Unlock();
					
					rtl_printf("Driver for address %d not found!\n", iByteAddr);
					return;
				}

				if (m_RegisteredDrivers[iIndex].pDriver->ReadAddress((void *)&Value, sizeof(DataType), iInputAddr, iByteAddr, iBitAddr) == 0)
					rtl_printf("Driver read address (%c)%d failed (DriverID: %s)!\n", (iInputAddr) ? 'I' : 'O', iByteAddr, m_RegisteredDrivers[iIndex].pDriver->DriverInfo());
				m_Mutex.Unlock();
			};

			template<class DataType> void SetValue(DataType Value, int iInputAddr, int iByteAddr, int iBitAddr)
			{
				int iIndex;

				m_Mutex.Lock();
				
				iIndex = FindDriver(iInputAddr, iByteAddr);

				if (iIndex == -1)
				{
					m_Mutex.Unlock();
					
					rtl_printf("Driver for address %d not found!\n", iByteAddr);
					return;
				}

				if (m_RegisteredDrivers[iIndex].pDriver->SetAddress((void *)&Value, sizeof(DataType), iInputAddr, iByteAddr, iBitAddr) == 0)
					rtl_printf("Driver write address (%c)%d failed (DriverID: %s)!\n", (iInputAddr) ? 'I' : 'O', iByteAddr, m_RegisteredDrivers[iIndex].pDriver->DriverInfo());
				m_Mutex.Unlock();
			}
	};

	#include "cdevicedriver.h"
#endif

