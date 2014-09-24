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


	#define BASE_ADDR	0x378
	#define LPT_IRQ		7

	#ifdef __cplusplus

	#include <rtl_cpp.h>

	extern "C"
	{
	#else
		#include <linux/kernel.h>
	#endif
		void PLCReleaseLPT(void);
		int PLCRegisterLPT(void);
	#ifdef __cplusplus
	};
		#include <cdevicedriver.h>

		/**
		 * Driver for paralel port at base address 0x378
		 *
		 * Support output addresing for range of 1 byte
		 *
		 * @version $Revision: 1.3 $
		 * @author Voloda <Vladimir.Kloz@dtg.cz>
		 */
		class CLPTDriver: public CDeviceDriver
		{
			private:
				/**
				 * Internal buffer for storage of
				 * complete current state of outputs
				 */
				unsigned char	m_OutBuff;
				unsigned char	m_InBuff[2];
				u32		m_BaseAddr;
				unsigned int	m_iIRQ;
				
				/**
				 * Interrupt handler for parallel port.
				 * Set's 1.0  bit of inputs to logical
				 * 1, if interrupt is requested.
				 */
				static unsigned int InterruptHandler(unsigned int irq, struct pt_regs *regs);
			public:
				/**
				 * Class constructor
				 */
				CLPTDriver(void);
				/**
				 * Class destructor
				 */
				~CLPTDriver(void);
				
				int FindLPT(void);
				int ReadAddress(void *Value, int iValueSize, int iInputAddr, int iByteAddr, int iBitAddr);
				int SetAddress(void *Value, int iValueSize, int iInputAddr, int iByteAddr, int iBitAddr);
				const char *DriverInfo(void);
		};
	#endif

#endif
