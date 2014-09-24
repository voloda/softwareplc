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

#include "include/ccodeprocessor.h"
#include "include/ccontainerconfiguration.h"

CContainerConfiguration :: CContainerConfiguration(char *pName)
{
	m_pGlobalVars = NULL;
	m_pPCalls = NULL;
	m_pTasks = NULL;
	m_pIncludes = NULL;
	m_sBlockName = pName;
}

CContainerConfiguration :: ~CContainerConfiguration(void)
{
	DELETE(m_pIncludes);
	DELETE(m_pTasks);
	DELETE(m_pPCalls);
	DELETE(m_pGlobalVars);
}

CContainerConfiguration& CContainerConfiguration :: operator +=(CContainerVarBlock *pBlock)
{
	m_pGlobalVars = pBlock;

	return(*this);
}

CContainerConfiguration& CContainerConfiguration :: operator +=(CContainerContainer<CContainerPCall> *pPCalls)
{
	m_pPCalls = pPCalls;

	return(*this);
}

CContainerConfiguration& CContainerConfiguration :: operator +=(CContainerContainer<CContainerTask> *pTasks)
{
	m_pTasks = pTasks;
	
	return(*this);
}

CContainerConfiguration& CContainerConfiguration :: operator +=(CContainerIncludes *pIncludes)
{
	m_pIncludes = pIncludes;

	return(*this);
}

void CContainerConfiguration :: GenerateCode(int iType)
{
	if (iType == 0)
	{
		this->OpenSourceFile(0);

		if (m_pIncludes != NULL)
		{
			m_pIncludes->GenerateCode();
		}
		
		if (pVariablesGlobal)
			pVariablesGlobal->GenerateCode(1);
		
		this->WriteHeaderEnd();
	}
	else
	{
		this->OpenSourceFile(1);

		CurrentOutput << "#include <rtl_cpp.h>\n";

		CurrentOutput << "CMemory<PLC_MEMORY_SIZE> g_KernelPLCMemory;\n";

		if (pVariablesGlobal)
			pVariablesGlobal->GenerateCode(3);

		if (m_pPCalls != NULL)
		{
			// generate function for program call
			m_pPCalls->GenerateCode(0);

			CurrentOutput << "#define THREADS_COUNT " << m_pPCalls->size() << "\n";
			CurrentOutput << "static pthread_t threads[THREADS_COUNT];\n";

			CurrentOutput << "extern \"C\" {\n";
			// generate module initialization routine
			// ---
			CurrentOutput << "\n\nint init_module(void)\n{\n";
			CurrentOutput << "\tint iRetv = 0, iCount = 0;\n\n\t__do_global_ctors_aux();\n\n";
			// generate code for task thread
			// initializations (module initialization)
			m_pPCalls->GenerateCode(1);

			CurrentOutput << "\n\treturn iRetv;\n}\n";
			// ---
			
			// generate module cleanup routine
			// ---
			CurrentOutput << "\n\nvoid cleanup_module(void)\n{\n";
			CurrentOutput << "\tfor (int i = 0; i < THREADS_COUNT; i++)\n\t{\n";
			CurrentOutput << "\t\tpthread_delete_np(threads[i]);\n\t}\n\n";

			CurrentOutput << "\t__do_global_dtors_aux();\n}\n\n";
			
			CurrentOutput << "}\n";
			CurrentOutput << "MODULE_LICENSE(\"GPL\");\n\n";
			// ---
		}
			
	}

	CurrentOutput.close();
}

