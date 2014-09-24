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

#ifdef __KERNEL__
	/*
	 * I need only declaration of macro's for
	 * port I/O access (inb,inw,outb,outw). Else I can't compile
	 * this in C++
	 */
	#undef __KERNEL__
	#include <asm/io.h>
	#define __KERNEL__
#else
	#include <asm/io.h>
#endif

#include "data_types.h"
#include "drv.h"

static CLPTDriver *pDriver = NULL;

CLPTDriver :: CLPTDriver(void)
{
	m_BaseAddr = 0;
	m_OutBuff = 0;

	pDriver = this;
}

CLPTDriver :: ~CLPTDriver(void)
{
	if (m_BaseAddr != 0)
	{
		outb(0, m_BaseAddr);
		PLCReleaseLPT();
	}

	if (m_iIRQ != 0)
	{
		rtl_free_irq(m_iIRQ);
		
		outb(inb(m_BaseAddr + 0x2) & 0xef, m_BaseAddr + 0x2);
	}

	pDriver = NULL;
}

unsigned int CLPTDriver :: InterruptHandler(unsigned int irq, struct pt_regs *regs)
{
	if (pDriver != NULL)
	{
		pDriver->m_SpinLock.Lock();
		pDriver->m_InBuff[1] = WRITE_BIT(pDriver->m_InBuff[1], 1, 0);
		pDriver->m_SpinLock.Unlock();
	}
	
	rtl_hard_enable_irq(irq);

	return 0;
}

int CLPTDriver :: FindLPT(void)
{
	if (PLCRegisterLPT())
	{
		m_BaseAddr = BASE_ADDR;
		outb(m_OutBuff, m_BaseAddr);

		
		rtl_request_irq(LPT_IRQ, CLPTDriver :: InterruptHandler);
		rtl_hard_enable_irq(LPT_IRQ);

		outb(inb(m_BaseAddr + 0x2) | 0x10, m_BaseAddr + 0x2);

		m_iIRQ = LPT_IRQ;
		
		return 1;
	}

	return 0;
}

int CLPTDriver :: ReadAddress(void *Value, int iValueSize, int iInputAddr, int iByteAddr, int iBitAddr)
{
	if (m_BaseAddr == 0)
		return 0;

	iByteAddr = RecalculateAddress(iValueSize, iInputAddr, iByteAddr);

	if (iByteAddr == -1)
		return 0;

	m_SpinLock.Lock();
	
	if ((iInputAddr) && (iByteAddr == 0))
		m_InBuff[0] = inb(m_BaseAddr + 1);

	switch (iValueSize)
	{
		case 1:
			 *((BYTE *)Value) = (iInputAddr) ? m_InBuff[iByteAddr] : m_OutBuff;
			break;
	}

	if (iBitAddr != -1)
	{
		*((BYTE *)Value) = (READ_BIT(*(BYTE *)Value, iBitAddr) == 0) ? 0: 0xff;

		if ((iInputAddr) && (iByteAddr = 1) && (iBitAddr == 0))
			m_InBuff[1] = WRITE_BIT(m_InBuff[1], 0, 0);
	}
	
	m_SpinLock.Unlock();

	return 1;
}

int CLPTDriver :: SetAddress(void *Value, int iValueSize, int iInputAddr, int iByteAddr, int iBitAddr)
{
	// Can't write into input location
	if ((iInputAddr) || (m_BaseAddr == 0))
		return 0;

	iByteAddr = RecalculateAddress(iValueSize, iInputAddr, iByteAddr);
	
	if (iByteAddr == -1)
		return 0;

	m_SpinLock.Lock();
	
	// I'll prepare new value of output in class buffer
	if (iBitAddr != -1)
	{
		m_OutBuff = WRITE_BIT((BYTE)m_OutBuff, ((*(BYTE *) Value) != 0), iBitAddr);
	}
	else
	{
		m_OutBuff = *(BYTE *) Value;
	}

	switch (iValueSize)
	{
		case 1:
			outb((BYTE)m_OutBuff, m_BaseAddr + iByteAddr);

			break;
	}

	m_SpinLock.Unlock();

	return 1;
}

const char *CLPTDriver :: DriverInfo(void)
{
	return "Parallel port driver for PLC";
}


