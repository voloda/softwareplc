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

#include "include/ccontainervarblock.h"

void CContainerVarBlock :: SetCodeBlockType(ProgramBlocks BlockType)
{
	m_ProgramType = BlockType;
}
/*
 * Set type of actual variable block
 */
void CContainerVarBlock :: SetBlockType(VarBlocks BlockType)
{
	m_Type = BlockType;
}
/*
 * Get type of actual variable block
 */
VarBlocks CContainerVarBlock :: GetBlockType(VarBlocks BlockType)
{
	return m_Type;
}
/*
 * Generation of C++ source code
 * iStage values are:
 *   1 - Declaration stage for variables, for global variables it's declaration
 *       of variable as extern in header
 *   2 - Declaration stage for parameters
 *   3 - Value initialization stage (class constructor) or for global variables
 *       declaration of variable in module base file
 */
void CContainerVarBlock :: GenerateCode(int iStage)
{
	vector<CContainerVariable *>::iterator	Iter;
	int i;

	for (i = 0, Iter = m_Records.begin(); Iter < m_Records.end(); Iter++, i++)
	{
		CContainerVariable *pData;

		pData = *Iter;
		
		switch(iStage)
		{
			case 1:
				if (m_Type != EVarGlobal)
					pData->GenerateClassDeclarationCode();
				else
					pData->GenerateGlobalHeaderCode();

				break;
			case 2:
				if ((m_Type == EVarInput) || (m_Type == EVarOutput) || (m_Type == EVarIO))
					pData->GenerateClassPropertyCode(i);
				break;
			case 3:
				switch(m_Type)
				{
					case EVarInput:
						pData->GenerateInitializationCode(i);
						break;
					case EVarOutput:
						pData->GenerateInitializationCode();
						break;
					case EVarIO:
						pData->GenerateInitializationCode();
						break;
					case EVarLocal:
						pData->GenerateInitializationCode();
						break;
					case EVarGlobal:
						pData->GenerateGlobalDeclarationCode();
						break;
				}
				break;
			case 4:
				if (m_Type == EVarIO)
					pData->GenerateExitCallCode();
				break;
		}
	}
}

/*
 * Class constructor
 */
CContainerVarBlock :: CContainerVarBlock(void)
{
	m_Type = EVarUndefined;
}

/*
 * Class destructor
 */
CContainerVarBlock :: ~CContainerVarBlock(void)
{
}

int CContainerVarBlock :: FindVariable(const char *pVarName)
{
	int iMax;

	iMax = m_Records.end() - m_Records.begin();

	for (int i = 0; i < iMax; i++)
	{
		if ((*m_Records[i]) == pVarName)
			return i;
	}	

	return -1;
}


