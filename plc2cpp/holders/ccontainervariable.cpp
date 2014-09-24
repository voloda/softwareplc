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
 *  Version: $Revision: 1.15 $
 */

#include <iostream>
#include <string.h>
#include <stdio.h>

#include "include/ccontainervariable.h"

CContainerVariable :: CContainerVariable(void)
{
	m_pInitialValue	= NULL;
	m_pMemLocation	= NULL;
	m_pDataType	= NULL;

	m_iFunctionBlock = 0;
	m_iConstant = 0;

	m_Type = EVarUndefined;
}

CContainerVariable :: ~CContainerVariable(void)
{
	if (m_pInitialValue != NULL)
		free(m_pInitialValue);
	
	if (m_pMemLocation != NULL)
		free(m_pMemLocation);

	if (m_pDataType != NULL)
		free(m_pDataType);
}

void CContainerVariable :: SetLocation(char *pLocation, int iAutoAlloc)
{
	if (iAutoAlloc)
		m_pMemLocation = strdup(pLocation);
	else
		m_pMemLocation = pLocation;
}

void CContainerVariable :: SetName(char *pName, int iAutoAlloc)
{
	m_sName = pName;
	
	str2lower(m_sName);
	
	if (iAutoAlloc)
		free(pName);
}

void CContainerVariable :: SetInitialValue(char *pValue, int iAutoAlloc)
{
	if (iAutoAlloc)
		m_pInitialValue = strdup(pValue);
	else
		m_pInitialValue = pValue;
}

void CContainerVariable :: AllocateInitialValue(int iSize)
{
	if (m_pInitialValue)
		m_pInitialValue = (char *) realloc(m_pInitialValue, iSize);
	else
		m_pInitialValue = (char *) malloc(iSize);
}

void CContainerVariable :: SetInitialValue(int iValue)
{
	AllocateInitialValue(10);

	snprintf(m_pInitialValue, 10, "%d", iValue);
}

void CContainerVariable :: SetInitialValue(double iValue)
{
	AllocateInitialValue(30);

	snprintf(m_pInitialValue, 30, "%e", iValue);
}

void CContainerVariable :: SetDataType(const char *pDataType)
{
	m_pDataType = strdup(pDataType);

	m_iFunctionBlock = 0;
}

void CContainerVariable :: SetFBDataType(char *pDataType, int iAutoAlloc)
{
	string	sLower;

	sLower = string("plcfb_") + pDataType;
	str2lower(sLower);
	
	if (iAutoAlloc)
		m_pDataType = strdup(sLower.c_str());
	else
	{
		m_pDataType = strdup(sLower.c_str());
		free(pDataType);
	}

	m_iFunctionBlock = 1;	
}

void CContainerVariable :: SetProgramBlock(ProgramBlocks BlockType)
{
	m_Block = BlockType;
}

void CContainerVariable :: GenerateInitializationCode(int iOrder)
{
	if (m_iConstant)
		return;
	
	CurrentOutput << GenerateLineInfo();
	

	if (m_pInitialValue)
	{
		CurrentOutput << "\tm_" << m_sName.c_str() << " = CVariable<" << m_pDataType << "> ((" << m_pDataType << ") " << m_pInitialValue ;
		
		if (m_pMemLocation)
			CurrentOutput << ", \"" << m_pMemLocation << "\"";

		CurrentOutput << ");\n";
	}
	else if (m_pMemLocation)
	{
			CurrentOutput << "\tm_" << m_sName.c_str() << " = CVariable<" << m_pDataType << ">(\"" << m_pMemLocation << "\");\n";
	}


	if ((m_Type != EVarLocal) && (m_Type != EVarIO))
	{
		if ((m_Block == EFunction) && (m_Type == EVarInput))
			CurrentOutput << "\tparm" << iOrder;
		else
			CurrentOutput << "\t" <<  m_sName.c_str();
		
		CurrentOutput << " = CProperty";

		switch(m_Type)
		{
			case EVarInput:
				CurrentOutput << "In";
				break;
			case EVarOutput:
				CurrentOutput << "Out";
				break;
		}

		CurrentOutput << "<" << m_pDataType << "> (&m_" << m_sName.c_str() << ");\n";
	}

	CurrentOutput << "\n";
}

void CContainerVariable :: GenerateClassDeclarationCode(void)
{
	CurrentOutput << GenerateLineInfo();

	if ((m_sName.length()) && (m_Type != EVarIO))
	{
		if (m_iConstant)
		{
			if (m_iFunctionBlock)
			{
				ShowError("Declaration of function block variable as constant ignored\n");
			}
			else
			{
				CurrentOutput << "const static " << m_pDataType << " m_" << m_sName.c_str();

				if (m_pInitialValue)
				{
					CurrentOutput << " = " << m_pInitialValue;
				}
				else
				{
					ShowError("Constant variable without initialization value\n");
				}
				
				CurrentOutput << ";\n";
			}
				
		}
		else
		{
			if (m_iFunctionBlock == 0)
				CurrentOutput << "\t\tCVariable<" << m_pDataType << "> m_" << m_sName.c_str() << ";\n";
			else
				CurrentOutput << "\t\t" << m_pDataType << " m_" << m_sName.c_str() << ";\n";
		}
	}
}

void CContainerVariable :: GenerateClassPropertyCode(int iOrder)
{
	CurrentOutput << GenerateLineInfo();
	
	if (m_sName.length() == 0)
		return;

	switch(m_Block)
	{
		case EFunction:
			CurrentOutput << "\t\tCPropertyIn<" << m_pDataType << "> parm" << iOrder << ";\n";
			break;
		case EFunctionBlock:
		case EProgram:
			CurrentOutput << "\t\tCProperty";

			switch(m_Type)
			{
				case EVarInput:
					CurrentOutput << "In";
					break;
				case EVarOutput:
					CurrentOutput << "Out";
					break;
				case EVarIO:
					CurrentOutput << "IO";
					break;
			}
			CurrentOutput << "<" << m_pDataType << "> " << m_sName.c_str() << ";\n";
			break;
		default:
			cerr << "Warning: Undefined program block type\n";
			break;
	}
}


int CContainerVariable :: operator ==(const char *pVarName)
{
	string	sVarName;

	sVarName = pVarName;
	str2lower(sVarName);
	
	return(m_sName == sVarName);
}

void CContainerVariable :: GenerateGlobalDeclarationCode(void)
{
	CurrentOutput << GenerateLineInfo();

	if (m_iConstant)
		CurrentOutput << "const ";

	if (!m_iFunctionBlock)
		CurrentOutput << "CVariable<" << m_pDataType << ">";
	else
		CurrentOutput << "\t" << m_pDataType;
	
	CurrentOutput << " g_" << m_sName.c_str();
	 
	if (m_pInitialValue)
	{
		CurrentOutput << " = CVariable<" << m_pDataType << ">((" << m_pDataType << ") " << m_pInitialValue;
		       
		if (m_pMemLocation)
			CurrentOutput << ", \"" << m_pMemLocation << "\"";
		
		CurrentOutput << ")";
	}
	else if (m_pMemLocation)
	{
		CurrentOutput << " = CVariable<" << m_pDataType << ">(\"" << m_pMemLocation << "\")";
	}

	CurrentOutput << ";\n";
}

void CContainerVariable :: GenerateGlobalHeaderCode(void)
{
	CurrentOutput << GenerateLineInfo() << "\textern ";

	if (m_iConstant)
		CurrentOutput << "const ";

	if (!m_iFunctionBlock)
		CurrentOutput << "CVariable<" << m_pDataType << ">";
	else
		CurrentOutput << m_pDataType;
	
	CurrentOutput << " g_" << m_sName.c_str() << ";\n";	
}

void CContainerVariable :: SetConstant(void)
{
	m_iConstant = 1;
}

void CContainerVariable :: SetVarBlockType(VarBlocks Type)
{
	m_Type = Type;
}

void CContainerVariable :: GenerateExitCallCode(void)
{
	if (m_Type != EVarIO)
		return;

	CurrentOutput << "\t" << m_sName.c_str() << ".CallExit();\n";
}

