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
 *  Version: $Revision: 1.10 $
 */

#include <string>
#include <iostream>

using namespace std;

#include "plc2cpp_globals.h"

#include "include/ccontainerfunction.h"

CContainerFunction :: CContainerFunction(char *pName, int iLineNumber)
	: CContainerBase(iLineNumber)
{
	m_pVars = NULL;

	m_sBlockName = string("plcf_") + pName;
	str2lower(m_sBlockName);

	m_sRetVarName = pName;
	str2lower(m_sRetVarName);

	if (m_sBlockName == sProjectName)
	{
		ShowError(string("Name of function can't be same as name of project(") + sProjectName + string(")\n"));
		iExitCode = 1;
	}

	m_pIL = NULL;
}

CContainerFunction :: ~CContainerFunction(void)
{
	if (m_pVars)
		delete m_pVars;

	if (m_pIL)
		delete m_pIL;
}

CContainerFunction& CContainerFunction :: operator =(CContainerContainer<CContainerVarBlock> *pVars)
{
	m_pVars = pVars;
	return(*this);
}

CContainerFunction& CContainerFunction :: operator =(CContainerContainer<CContainerIL> *pIL)
{
	m_pIL = pIL;
	return(*this);
}

CContainerFunction& CContainerFunction :: operator =(CContainerIncludes *pIncludes)
{
	m_pIncludes = pIncludes;
	return(*this);
}

void CContainerFunction :: SetReturnDataType(const char *pDataType)
{
	m_sDataType = pDataType;
}

/*
 * Generates particulary code for function
 *  iCodePart == 0: Function class declaration (header)
 *  iCodePart == 1: Function class implementation (code)
 */
void CContainerFunction :: GenerateCode(int iCodePart)
{
				 
	if (iCodePart == 0)
	{
		this->OpenSourceFile(0);

		if (this->m_pIncludes)
			m_pIncludes->GenerateCode();

		CurrentOutput << "/* Function class(" << m_sBlockName.c_str() << ") */\n\n";

		CurrentOutput << "class " << m_sBlockName.c_str() << "\n{\n";
		CurrentOutput << "\tprivate:\n";

		CurrentOutput << "\t\tCVariable<" << m_sDataType.c_str() << "> m_" << m_sRetVarName.c_str() << ";\n";

		if (m_pVars != NULL)
		{
			for(int i = 0; i < m_pVars->size(); i++)
			{
				(*m_pVars)[i]->GenerateCode(1);
			}
		}
		else
		{
			cerr << "Warning: No variables defined in function: " << m_sBlockName.c_str() << "\n";
		}
	
		CurrentOutput << "\tpublic:\n";

		if (m_pVars != NULL)
		{
			for(int i = 0; i < m_pVars->size(); i++)
			{
				(*m_pVars)[i]->GenerateCode(2);
			}
		}
		
		CurrentOutput << "\n\t\t" << m_sBlockName.c_str() << "(void);\n";
		CurrentOutput << "\t\tvirtual ~" << m_sBlockName.c_str() << "(void);\n";
		CurrentOutput << "\t\tvoid Function(void);\n";
		CurrentOutput << "\t\tconst CVariable<" << m_sDataType.c_str() << ">& GetResult(void);\n";
		CurrentOutput << "};\n";

		this->WriteHeaderEnd();
	}
	else
	{
		this->OpenSourceFile(1);

		CurrentOutput << "#define RETURN()\t\treturn;\n\n"; 
		// generate contructor
		CurrentOutput << m_sBlockName.c_str() << " :: " << m_sBlockName.c_str() << "(void)\n{\n";

		if (m_pVars != NULL)
		{
			for (int i = 0; i < m_pVars->size(); i++)
			{
				(*m_pVars)[i]->GenerateCode(3);
			}
		}

		CurrentOutput << "\n}\n\n";

		// generate destructor
		CurrentOutput << m_sBlockName.c_str() << " :: ~" << m_sBlockName.c_str() << "(void)\n{\n";

		CurrentOutput << "\n}\n";
		CurrentOutput << "const CVariable<" << m_sDataType.c_str() << "> &" << m_sBlockName.c_str() << " :: GetResult(void)\n{\n";

		CurrentOutput << "\treturn(this->m_" << m_sRetVarName.c_str() << ");\n";
		CurrentOutput << "\n}\n";

		CurrentOutput << "\nvoid " << m_sBlockName.c_str() << " :: Function(void)\n{\n";
		CurrentOutput << "\tCResult result;\n\n";

		if (m_pIL)
		{
			for (int i = 0; i < m_pIL->size(); i++)
			{
				(*m_pIL)[i]->GenerateCode();
			}
		}
		else
		{
			cerr << "Warning: Function have no code: " << m_sBlockName.c_str() << "\n";
		}

		CurrentOutput << "\n}\n";
	}

	CurrentOutput.close();
}

