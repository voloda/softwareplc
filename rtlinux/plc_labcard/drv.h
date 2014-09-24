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
 *  Version: $Revision: 1.3 $
 */

#ifndef _PLC_LABCARD_DRV_H_
	#define _PLC_LABCARD_DRV_H_


	#ifdef __cplusplus

	#include <rtl_cpp.h>

	extern "C"
	{
	#else
		#include <linux/kernel.h>
	#endif
		void	*FindPCI1750Card(void);
		u32	GetPCI1750BaseAddr(void *pDevice);
		void	ReleasePCI1750Card(void *pDevice);
	#ifdef __cplusplus
	};
		#include <cdevicedriver.h>

		/**
		 * Driver for I/O card:
		 * Advantech PC-LabCard series (PCI-1750)
		 *
		 * Driver allowes access to all digital I/O
		 * on addresses BASE, BASE + 1 addresses as
		 * single bit, byte or word.
		 * 
		 * After driver intialization, all outputs are
		 * set to zero.
		 *
		 * *******************************************************
		 * Card addressing:
		 * 
		 * BASE + addr	Read		Write
		 * 0		IDI0-7		IDO0-7
		 * 1		IDI8-15		IDO8-15
		 * 2-23		Reserved	Reserved
		 * 24		Counter 0	Counter 0
		 * 25		Counter 1	Counter 1
		 * 26		Counter 2	Counter 2
		 * 27				Counters control register
		 * 28-31	Reserved	Reserver
		 * 32		Interrupt Stat.	Interrupt control register
		 * *******************************************************
		 * 
		 * @version $Revision: 1.3 $
		 * @author Voloda <Vladimir.Kloz@dtg.cz>
		 */
		class CLabCardDriver: public CDeviceDriver
		{
			private:
				/**
				 * Internal buffer for storage of
				 * complete current state of outputs
				 */
				unsigned char	m_OutBuff[2];

				/**
				 * Structure with informations about
				 * found card. If == NULL, no card is
				 * initialized
				 */
				void	*m_pDevice;
				/**
				 * Base card address (as found from PCI BIOS)
				 * Access to I/O is on addresses BASE + 0,
				 * BASE + 1
				 */
				u32	m_BaseAddr;
			public:
				/**
				 * Class constructor
				 */
				CLabCardDriver(void);
				/**
				 * Class destructor
				 */
				~CLabCardDriver(void);
				/**
				 * Search PCI bus for card
				 *
				 * @return	0 - no card found, > 0, card found and initialized
				 */
				int FindCard(void);
				int ReadAddress(void *Value, int iValueSize, int iInputAddr, int iByteAddr, int iBitAddr);
				int SetAddress(void *Value, int iValueSize, int iInputAddr, int iByteAddr, int iBitAddr);
				const char *DriverInfo(void);
		};
	#endif

#endif
