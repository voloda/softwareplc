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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#include "include/measure.h"
#include "include/plc_uspace.h"

CMeasureThread :: CMeasureThread(CMeasureVector<int> *pProcessVariable, CMeasureVector<int> *pSetPoint)
	: wxThread()
{
	m_pProcessVariable = pProcessVariable;
	m_pSetPoint = pSetPoint;

	memset(m_Inputs, 0, 64);
}

void CMeasureThread :: SetVectors(CMeasureVector<int> *pProcessVariable, CMeasureVector<int> *pSetPoint)
{
	m_pProcessVariable = pProcessVariable;
	m_pSetPoint = pSetPoint;

}

void CMeasureThread :: SetRotations(int iRotations)
{
	wxMutexLocker	lock(m_LockInputs);
	int		iSetInputs;
	
	iSetInputs = open("/dev/rtf1", O_WRONLY);

	if (iSetInputs == -1)
		return;

	*((int *)m_Inputs) = iRotations;

	write(iSetInputs, m_Inputs, 64);

	close(iSetInputs);
}

void CMeasureThread :: ResetDriver(void)
{
	struct PLCUserSpaceControl	Control;

	Control.Command = PLC_RESET;
	write(m_iControl, &Control, sizeof(Control));
	
	Control.Command = PLC_SEND_SET_TIME;
	Control.iTime = 50;

	write(m_iControl, &Control, sizeof(Control));
}

void *CMeasureThread :: Entry()
{
	int	iInput;
	struct PLCUserSpaceControl	Control;
	
	m_iControl = open("/dev/rtf0", O_WRONLY);
	iInput = open("/dev/rtf2", O_RDONLY);

	if ((m_iControl == -1) || (m_iControl == -1))
		return 0;

	ResetDriver();
	
	while(1)
	{
		char pBuff[65];

		if (ReadData(iInput, pBuff) == -1)
			break;
	
		if (TestDestroy())
			break;

		if ((m_pSetPoint == NULL) || (m_pProcessVariable == NULL))
		{
			continue;
		}

		if (pBuff[0] == 'I')
		{
			wxMutexLocker	lock(m_LockInputs);

			m_pSetPoint->Lock();
			m_pSetPoint->AddValue(*((int *) (pBuff + 1)));
			m_pSetPoint->Unlock();

			memcpy(m_Inputs, pBuff + 1, 64);
		}
		else if (pBuff[0] == 'O')
		{
			int	iValue;
			
			iValue = *(int *)(pBuff + 1);

			m_pProcessVariable->Lock();
			m_pProcessVariable->AddValue(iValue);
			m_pProcessVariable->Unlock();

			// cout << "Value: " << iValue << "\n";
		}
		else
			ResetDriver();
	}

	Control.Command = PLC_SEND_STOP;
	
	write(m_iControl, &Control, sizeof(Control));
	
	close(m_iControl);
	close(iInput);
	
	return 0;
}

int CMeasureThread :: ReadData(int iInput, char pBuff[])
{
	int	iSize = 0;
	
	while(iSize != 65)
	{
		int iSizeTmp;

		iSizeTmp = read(iInput, pBuff + iSize, 65 - iSize);

		if (iSizeTmp == -1)
			return -1;

		iSize += iSizeTmp;
	}

	return 1;
}

