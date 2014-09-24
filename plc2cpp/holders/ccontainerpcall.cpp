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
 *  Version: $Revision: 1.11 $
 */

#include <iostream>

#include "plc2cpp_globals.h"
#include "include/ccontainerpcall.h"
#include "include/ccodeprocessor.h"

using namespace std;

CContainerPCall :: CContainerPCall(const char *pCallName)
{
	m_sBlockName = pCallName;

	str2lower(m_sBlockName);
}

CContainerPCall :: ~CContainerPCall(void)
{
}

void CContainerPCall :: SetTaskName(const char *pTaskName)
{
	m_sTaskName = pTaskName;
	str2lower(m_sTaskName);
}

void CContainerPCall :: SetProgramName(const char *pProgramName)
{
	m_sProgramName = "gprog_plcp_";
	m_sProgramName += pProgramName;

	str2lower(m_sProgramName);
}

void CContainerPCall :: AddParmName(const char *pParmName)
{
	m_sCode += "\t";
	m_sCode += m_sProgramName;
	m_sCode += ".";
	m_sCode += pParmName;
	m_sCode += " = ";

	str2lower(m_sCode);
}

void CContainerPCall :: AddParmLocation(const char *pParmLocation)
{
	m_sCode += "CVariable<BIT>(\"";
	m_sCode += pParmLocation;
	m_sCode += "\");\n";
}

void CContainerPCall :: AddParmVariable(const char *pParmVariable)
{
	string	sLower;

	sLower = pParmVariable;
	str2lower(sLower);

	m_sCode += "g_";
	m_sCode += sLower;
	m_sCode += ";\n";
}

void CContainerPCall :: GenerateCode(int iCodePart)
{
	CContainerTask *pTask = NULL;

	switch(iCodePart)
	{
		// generates program call code
		case 0:

			this->GenerateLineInfo();

			CurrentOutput << "/*\n * RT-Linux thread entry for program block: " << m_sBlockName.c_str() << "\n */\n";
			CurrentOutput << "static void *PCall" << m_sBlockName.c_str() << "(void *pParms)\n{\n";


			if (pTasks)
				pTask = (*pTasks)[m_sTaskName.c_str()];

			CurrentOutput << "\tstruct sched_param\tp;\n\n";

			if (pTask == NULL)
			{
				cerr << "Warning: Nonexisting task name associated to program block: " << m_sBlockName.c_str() << ", using period " << llNoTaskPeriod << "ns, and lowest priority\n";
	
				CurrentOutput << "\tp.sched_priority = 50;\n";
				CurrentOutput << "\tpthread_setschedparam(pthread_self(), SCHED_FIFO, &p);\n";
				CurrentOutput << "\tpthread_make_periodic_np(pthread_self(), gethrtime(), " << llNoTaskPeriod << ");\n\n";
				CurrentOutput << "\twhile(1)\n\t{\n\t\tpthread_wait_np();\n";
				CurrentOutput << "\t\t" << m_sProgramName.c_str() << ".Function();\n\t}\n";

				iExitCode = 1;
			}
			else
			{
				string	sSingle;
				int	iREdge;
				
				sSingle = pTask->GetSingleInput();

				iREdge = ((pTask->GetInterval() == 0) && (sSingle.length() > 0));

				if (iREdge)
					CurrentOutput << "\tint iREdgeDetection;\n\n\tiREdgeDetection = " << sSingle.c_str() << ";\n\n";
				CurrentOutput << "\tp.sched_priority = " << pTask->GetPriority() << ";\n";
				CurrentOutput << "\tpthread_setschedparam(pthread_self(), SCHED_FIFO, &p);\n";

				CurrentOutput << "\tpthread_make_periodic_np(pthread_self(), gethrtime(), " << ((iREdge) ? llSinglePeriod : (pTask->GetInterval())) << ");\n\n";

				CurrentOutput << "\twhile(1)\n\t{\n\t\tpthread_wait_np();\n";

				if (sSingle.length() > 0)
				{
					if (iREdge)
						CurrentOutput << "\t\t" << "if ((iREdgeDetection == 0) && (" << sSingle.c_str() << " != 0))\n\t\t{\n";
					else
						CurrentOutput << "\t\t" << "if (" << sSingle.c_str() << " != 0)\n\t\t{\n";
				}

				CurrentOutput << m_sCode.c_str();
				CurrentOutput << "\t\t\t" << m_sProgramName.c_str() << ".Function();\n";
				
				if (sSingle.length() > 0)
				{
					CurrentOutput << "\t\t}\n";
				
					if (iREdge)
						CurrentOutput << "\t\tiREdgeDetection = " << sSingle.c_str() << ";\n";
				}

				CurrentOutput << "\t}\n";
				CurrentOutput << "\treturn NULL;\n";
			}
			CurrentOutput << "}\n\n";
			break;
		// generates program call thread initialization
		case 1:
			CurrentOutput << "\tiRetv += pthread_create(&threads[iCount++], &attr, PCall" << m_sBlockName.c_str() << ", NULL);\n";
			break;
	}
}

