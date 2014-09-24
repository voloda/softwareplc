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

#define __CPP_INIT__

#include <rtl_cpp.h>
#include <rtl_fifo.h>

#include "userspace.h"
#include "plc_uspace.h"

static CUserSpaceDriver	plcUserSpaceDriver;

CUserSpaceDriver :: ~CUserSpaceDriver()
{
	rtf_destroy(m_iFirstFIFO);
	rtf_destroy(m_iFirstFIFO + 1);
	rtf_destroy(m_iFirstFIFO + 2);

	pthread_delete_np(m_SendThread);
}

int CUserSpaceDriver :: ReadAddress(void *Value, int iValueSize, int iInputAddr, int iByteAddr, int iBitAddr)
{
	iByteAddr = RecalculateAddress(iValueSize, iInputAddr, iByteAddr);

	if (iByteAddr == -1)
		return 0;

	m_SpinLock.Lock();
	
	memcpy((void *)Value, (iInputAddr) ? (void *)(m_InBuff + iByteAddr ): (void *) (m_OutBuff + iByteAddr), iValueSize);

	m_SpinLock.Unlock();
	
	return 1;
}

int CUserSpaceDriver :: SetAddress(void *Value, int iValueSize, int iInputAddr, int iByteAddr, int iBitAddr)
{
	if (iInputAddr)
		return 0;

	iByteAddr = RecalculateAddress(iValueSize, iInputAddr, iByteAddr);
	
	if (iByteAddr == -1)
		return 0;

	m_SpinLock.Lock();
	
	memcpy((void *) (m_OutBuff + iByteAddr), (void *)Value, iValueSize);

	m_SpinLock.Unlock();

	return 1;
}

const char *CUserSpaceDriver :: DriverInfo(void)
{
	return "PLC<->RT-Linux user-space communication  driver";
}

int CUserSpaceDriver :: InitializeDriverFIFO(int iFirstFIFO)
{
	// First FIFO is used for controling function
	// of driver
	rtf_create(iFirstFIFO, 1000);	// First FIFO is used for control
	// Second FIFO is used for setting values of
	// inputs from user-space
	rtf_create(iFirstFIFO + 1, 10 * PLC_USPACE_MEMORY);	
	// Third FIFO is used for reading current values
	// of I/O from user-space
	rtf_create(iFirstFIFO + 2, 2 * 10 * PLC_USPACE_MEMORY); 

	rtf_create_handler(iFirstFIFO, CUserSpaceDriver :: FIFOControlHandler);
	rtf_create_handler(iFirstFIFO + 1, CUserSpaceDriver :: FIFOHandler);

	m_iFirstFIFO = iFirstFIFO;

	pthread_create(&m_SendThread, NULL, CUserSpaceDriver :: SendThreadCode, this);

	return 1;
}

void *CUserSpaceDriver :: SendThreadCode(void *t)
{
	CUserSpaceDriver *pDriver;
	
	if (t == NULL)
		return NULL;

	pDriver = (CUserSpaceDriver *) t;

	pthread_suspend_np(pthread_self());

	while(1)
	{
		char	cID;
		
		pDriver->m_SpinLock.Lock();

		cID = 'I';
		
		rtf_put(plcUserSpaceDriver.m_iFirstFIFO + 2, &cID, 1);
		rtf_put(plcUserSpaceDriver.m_iFirstFIFO + 2, plcUserSpaceDriver.m_InBuff, PLC_USPACE_MEMORY);

		cID = 'O';
		rtf_put(plcUserSpaceDriver.m_iFirstFIFO + 2, &cID, 1);
		rtf_put(plcUserSpaceDriver.m_iFirstFIFO + 2, plcUserSpaceDriver.m_OutBuff, PLC_USPACE_MEMORY);
		
		pDriver->m_SpinLock.Unlock();

		pthread_wait_np();
	}
	
	return NULL;
}

int CUserSpaceDriver :: FIFOHandler(unsigned int iFIFO)
{
	int	iErr;
	char	pBuff[PLC_USPACE_MEMORY];
	
	while((iErr = rtf_get(iFIFO, pBuff, PLC_USPACE_MEMORY)) == PLC_USPACE_MEMORY)
	{
		plcUserSpaceDriver.m_SpinLock.Lock();
		
		memcpy(plcUserSpaceDriver.m_InBuff, pBuff, PLC_USPACE_MEMORY);
		
		plcUserSpaceDriver.m_SpinLock.Unlock();
	}
	
	if (iErr != 0)
		return -EINVAL;
	
	return 0;
}

int CUserSpaceDriver :: FIFOControlHandler(unsigned int iFIFO)
{
	int	iErr;
	struct	PLCUserSpaceControl Control;
	
	while((iErr = rtf_get(iFIFO, &Control, sizeof(struct PLCUserSpaceControl))) == sizeof(struct PLCUserSpaceControl))
	{
		switch(Control.Command)
		{
			case PLC_SEND_SET_TIME:
				hrtime_t Time;
				
				Time = (hrtime_t)Control.iTime * (hrtime_t) 1000000;
				
				pthread_wakeup_np(plcUserSpaceDriver.m_SendThread);
				pthread_make_periodic_np(plcUserSpaceDriver.m_SendThread, gethrtime(), Time);

				break;
			case PLC_SEND_STOP:
				pthread_suspend_np(plcUserSpaceDriver.m_SendThread);
				break;
			case PLC_RESET:
				pthread_suspend_np(plcUserSpaceDriver.m_SendThread);
				rtf_flush(plcUserSpaceDriver.m_iFirstFIFO + 1);
				rtf_flush(plcUserSpaceDriver.m_iFirstFIFO + 2);
				break;
		}
	}		
	
	if (iErr != 0)
		return -EINVAL;

	return 0;
}

MODULE_PARM(iladdr, "i");
MODULE_PARM(oladdr, "i");
MODULE_PARM(FirstFIFO, "i");
MODULE_PARM_DESC(iladdr, "Input lowest registration address");
MODULE_PARM_DESC(oladdr, "Output lowest registration address");
MODULE_PARM_DESC(FirstFIFO, "First FIFO used for communication");
		
static int iladdr = 0;
static int oladdr = 0;
static unsigned int FirstFIFO = 0;

/*
 * Module initialization
 */
int init_module(void) {
	
	__do_global_ctors_aux();
	
	plcUserSpaceDriver.RegisterDriver(iladdr, iladdr + PLC_USPACE_MEMORY, oladdr, oladdr + PLC_USPACE_MEMORY);

	plcUserSpaceDriver.InitializeDriverFIFO(FirstFIFO);

	return 0;
}

/*
 * Module cleanup - deletes all initialized threads
 */
void cleanup_module(void) {

	__do_global_dtors_aux();
}

MODULE_LICENSE("GPL");

