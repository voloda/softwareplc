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

#define __CPP_INIT__

#include <rtl_cpp.h>
#include "data_types.h"
#include "cdevicedriver.h"

/**
 * Simple driver for tracing values of
 * variables.
 * When storage of new value is performed,
 * new value is showed.
 * When reading current value on given
 * address, current value is showed.
 *
 * This may be used as simple example how
 * to create new PLC I/O driver.
 *
 * @version $Revision: 1.3 $
 * @author Voloda <Vladimir.Kloz@dtg.cz>
 */
class CTraceDriver: public CDeviceDriver
{
	private:
		/**
		 * Internal buffer for emulating inputs
		 */
		unsigned char	m_InBuff[16];
		/**
		 * Internal buffer for emulating outputs
		 */
		unsigned char	m_OutBuff[16];

	public:
		int ReadAddress(void *Value, int iValueSize, int iInputAddr, int iByteAddr, int iBitAddr)
		{
			iByteAddr = RecalculateAddress(iValueSize, iInputAddr, iByteAddr);

			if (iByteAddr == -1)
				return 0;

			m_SpinLock.Lock();
			
			memcpy((void *)Value, (iValueSize) ? (void *)(m_InBuff + iByteAddr ): (void *) (m_OutBuff + iByteAddr), iValueSize);

			switch (iValueSize)
			{
				case 1:
					rtl_printf("Current value (%d.%d): %d\n", iByteAddr, iBitAddr, (BYTE) *(m_InBuff + iByteAddr));
					break;
				case 2:
					rtl_printf("Current value (%d.%d): %d\n", iByteAddr, iBitAddr, (WORD) *(m_InBuff + iByteAddr));
					break;
				case 4:
					rtl_printf("Current value (%d.%d): %d\n", iByteAddr, iBitAddr, (DWORD) *(m_InBuff + iByteAddr));
					break;
				default:
					break;

			}
			
			m_SpinLock.Unlock();
			
			return 1;
		};

		int SetAddress(void *Value, int iValueSize, int iInputAddr, int iByteAddr, int iBitAddr)
		{
			iByteAddr = RecalculateAddress(iValueSize, iInputAddr, iByteAddr);
			
			if (iByteAddr == -1)
				return 0;

			m_SpinLock.Lock();
			
			memcpy((iValueSize) ? (void *)(m_InBuff + iByteAddr): (void *) (m_OutBuff + iByteAddr), (void *)Value, iValueSize);
			switch (iValueSize)
			{
				case 1:
					rtl_printf("New value (%d.%d): %d\n", iByteAddr, iBitAddr, (BYTE) *(m_InBuff + iByteAddr));
					break;
				case 2:
					rtl_printf("New value (%d.%d): %d\n", iByteAddr, iBitAddr, (WORD) *(m_InBuff + iByteAddr));
					break;
				case 4:
					rtl_printf("New value (%d.%d): %d\n", iByteAddr, iBitAddr, (DWORD) *(m_InBuff + iByteAddr));
					break;
				default:
					break;

			}

			m_SpinLock.Unlock();

			return 1;
		};

		const char *DriverInfo(void)
		{
			return "Trace testing driver";
		}
};

static CTraceDriver	plcTraceDriver;

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
	
	plcTraceDriver.RegisterDriver(iladdr, iladdr + 15, oladdr, oladdr + 15);

	return 0;
}

/*
 * Module cleanup - deletes all initialized threads
 */
void cleanup_module(void) {

	__do_global_dtors_aux();
}

MODULE_LICENSE("GPL");

