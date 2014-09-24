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

#include "include/ccontainertask.h"

CContainerTask :: CContainerTask(const char *pTaskName)
{
	m_iPriority = 50;
	m_llInterval = 0;
	m_sBlockName = pTaskName;
	str2lower(m_sBlockName);
}

CContainerTask :: ~CContainerTask(void)
{
}

void CContainerTask :: SetInterval(long long llInterval)
{
	m_llInterval = llInterval;
}

void CContainerTask :: SetPriority(int iPriority)
{
	m_iPriority = iPriority;
}

void CContainerTask :: SetSingleInputVariable(const char *pSingleInput)
{

	m_sSingleInput = "g_";
	m_sSingleInput += pSingleInput;

	str2lower(m_sSingleInput);
}

void CContainerTask :: SetSingleInputLocation(const char *pSingleInput)
{
	m_sSingleInput = "CVariable<BOOL>(\"";
	m_sSingleInput += pSingleInput;
	m_sSingleInput += "\", 0)";
}

const char* CContainerTask :: GetSingleInput(void)
{
	return(m_sSingleInput.c_str());
}

int CContainerTask :: GetPriority(void)
{
	return(m_iPriority);
}

long long CContainerTask :: GetInterval(void)
{
	return(m_llInterval);
}

