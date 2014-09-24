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
 *  Version: $Revision: 1.7 $
 */


#include <string>
#include <iostream>

using namespace std;

#include "plc2cpp_globals.h"
#include "include/ccodeprocessor.h"
#include "include/ccontainerfbprogram.h"

CContainerFBProgram :: CContainerFBProgram(char *pName, ProgramBlocks BlockType, int iLineNumber) 
	: CContainerBase(iLineNumber)
{
	m_pVars = NULL;
	
	m_sBlockName = pName;
	str2lower(m_sBlockName);
	
	switch(BlockType)
	{
		case EFunctionBlock:
			m_sBlockName = "plcfb_" + m_sBlockName;
			break;
		case EProgram:
			m_sBlockName = "plcp_" + m_sBlockName;
			break;
	}
	
	if (m_sBlockName == sProjectName)
	{
		ShowError(string("Name of block can't be same as name of project (") + m_sBlockName + string(")\n"));
	}
	
	m_pIL = NULL;
	m_pIncludes = NULL;

	m_BlockType = BlockType;
}

CContainerFBProgram :: ~CContainerFBProgram(void)
{
	DELETE(m_pVars);

	DELETE(m_pIL);

	DELETE(m_pIncludes);
}

CContainerFBProgram& CContainerFBProgram :: operator =(CContainerContainer<CContainerVarBlock> *pVars)
{
	m_pVars = pVars;
	return(*this);
}

CContainerFBProgram& CContainerFBProgram :: operator =(CContainerContainer<CContainerIL> *pIL)
{
	m_pIL = pIL;
	return(*this);
}

CContainerFBProgram& CContainerFBProgram :: operator =(CContainerIncludes *pIncludes)
{
	m_pIncludes = pIncludes;
	return(*this);
}

/*
 * Generates particulary code for function
 *  iCodePart == 0: Function class declaration (header)
 *  iCodePart == 1: Function class implementation (code)
 */
void CContainerFBProgram :: GenerateCode(int iCodePart)
{
	if (iCodePart == 0)
	{
		this->OpenSourceFile(0);

		if (m_pIncludes)
			m_pIncludes->GenerateCode();
	
		if (m_BlockType == EFunctionBlock)
			CurrentOutput << "/* Function block";
		else
			CurrentOutput << "/* Program block";

		CurrentOutput << " class(" << m_sBlockName.c_str() << ") */\n\n";
		CurrentOutput << "class " << m_sBlockName.c_str() << "\n{\n";
		CurrentOutput << "\tprivate:\n";

		if (m_pVars != NULL)
		{
			for(int i = 0; i < m_pVars->size(); i++)
			{
				(*m_pVars)[i]->GenerateCode(1);
			}
		}
		else
		{
			cerr << "Warning: No variables localy defined in " << ((m_BlockType == EFunctionBlock) ? "function block" : "program") << ": " << m_sBlockName.c_str() << "\n";
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
		CurrentOutput << "\t\tvoid ExitCall(void);\n";
		CurrentOutput << "};\n";

		// declaration for external usage of program block
		if (m_BlockType == EProgram)
			CurrentOutput << "extern " << m_sBlockName.c_str() << "\tgprog_" << m_sBlockName.c_str() << ";\n\n";

		this->WriteHeaderEnd();

	}
	else
	{
		this->OpenSourceFile(1);

		CurrentOutput << "#define RETURN()\t\t{ExitCall();return;}\n\n";
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
		
		CurrentOutput << "void " << m_sBlockName.c_str() << " :: ExitCall(void)\n{\n";

		if (m_pVars != NULL)
		{
			for (int i = 0; i < m_pVars->size(); i++)
				(*m_pVars)[i]->GenerateCode(4);
		}
		
		CurrentOutput << "}\n";
		// generate destructor
		CurrentOutput << m_sBlockName.c_str() << " :: ~" << m_sBlockName.c_str() << "(void)\n{\n";

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
			cerr << "Warning: " << ((m_BlockType == EFunctionBlock) ? "Function block" : "Program") << " have no code: " << m_sBlockName.c_str() << "\n";
		}

		CurrentOutput << "\tRETURN();\n}\n";

		// global accessible instance of program block
		if (m_BlockType == EProgram)
			CurrentOutput << m_sBlockName.c_str() << "\tgprog_" << m_sBlockName.c_str() << ";\n";
	}

	CurrentOutput.close();
}

