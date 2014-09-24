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

CLabCardDriver :: CLabCardDriver(void)
{
	m_pDevice = NULL;
	m_BaseAddr = 0;
	m_OutBuff[0] = 0;
	m_OutBuff[1] = 0;
}

CLabCardDriver :: ~CLabCardDriver(void)
{
	if (m_pDevice != NULL)
	{
		ReleasePCI1750Card(m_pDevice);
		m_pDevice = NULL;
	}
}

int CLabCardDriver :: FindCard(void)
{
	m_pDevice = FindPCI1750Card();

	if (m_pDevice != NULL)
	{
		m_BaseAddr = GetPCI1750BaseAddr(m_pDevice);
		// initialize outputs to zeros
		outw(*(WORD *)m_OutBuff, m_BaseAddr);
		
		rtl_printf("PLC Advantech PC-LabCard PCI-1750 found (base address: 0x%X)\n", m_BaseAddr);
	}
	else
	{
		rtl_printf("PLC Advantech PC-LabCard PCI-1750 not found\n");
	}

	return (m_pDevice != NULL);
}

int CLabCardDriver :: ReadAddress(void *Value, int iValueSize, int iInputAddr, int iByteAddr, int iBitAddr)
{
	if (m_BaseAddr == 0)
		return 0;

	iByteAddr = RecalculateAddress(iValueSize, iInputAddr, iByteAddr);

	if (iByteAddr == -1)
		return 0;

	m_Mutex.Lock();
	
	switch (iValueSize)
	{
		case 1:
			* ((BYTE *)Value) = (iInputAddr) ? inb(m_BaseAddr + iInputAddr) : m_OutBuff[iInputAddr];
			break;
		case 2:
			* ((WORD *)Value) = (iInputAddr) ? inw(m_BaseAddr) : m_OutBuff[iInputAddr];
			break;
	}
	
	m_Mutex.Unlock();

	if (iBitAddr != -1)
	{
		*((BYTE *)Value) = READ_BIT(*(BYTE *)Value, iBitAddr);
	}
	
	return 1;
}

int CLabCardDriver :: SetAddress(void *Value, int iValueSize, int iInputAddr, int iByteAddr, int iBitAddr)
{
	// Can't write into input location
	if ((iInputAddr) || (m_BaseAddr == 0))
		return 0;

	iByteAddr = RecalculateAddress(iValueSize, iInputAddr, iByteAddr);
	
	if (iByteAddr == -1)
		return 0;

	m_Mutex.Lock();
	
	// I'll prepare new value of output in class buffer
	if (iBitAddr != -1)
	{
		m_OutBuff[iInputAddr] = WRITE_BIT((BYTE)m_OutBuff[iInputAddr], (*(BYTE *) Value != 0), iBitAddr);
	}

	switch (iValueSize)
	{
		case 1:
			outb((BYTE)m_OutBuff[iInputAddr], m_BaseAddr + iInputAddr);

			break;
		case 2:
			// for word values, only BASE card address is
			// allowed
			outw(*(WORD *)m_OutBuff, m_BaseAddr);
			break;
	}

	m_Mutex.Unlock();

	return 1;
}

const char *CLabCardDriver :: DriverInfo(void)
{
	return "Advantech PC-LabCard PCI-1750";
}


