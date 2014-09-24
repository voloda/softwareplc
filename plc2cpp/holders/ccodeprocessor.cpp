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
#include <fstream>

using namespace std;

#include "plc2cpp_globals.h"

#include "include/ccodeprocessor.h"
#include "include/ccontainercontainer.h"
#include "include/ccontainervarblock.h"
#include "include/ccontainerfunction.h"
#include "include/ccontainerfbprogram.h"
#include "include/ccontaineril.h"
#include "include/ccontainerincludes.h"
#include "include/ccontainerconfiguration.h"
#include "include/ccontainertask.h"
#include "include/ccontainerpcall.h"

/*
 * Temporary variables uses when parsing
 * source code of PLC program
 */
CContainerVariable	*pContainerVariable = NULL;
CContainerVarBlock	*pVariables = NULL;
CContainerVarBlock	*pVariablesExt = NULL;
CContainerVarBlock	*pVariablesGlobal = NULL;
CContainerVarBlock	*pVariablesIO = NULL;
CContainerFunction	*pFunction = NULL;
CContainerIL		*pIL = NULL;
CContainerIncludes	*pIncludes = NULL;
CContainerTask		*pTask = NULL;
CContainerPCall		*pPCall = NULL;
CContainerConfiguration	*pConfiguration = NULL;
CContainerFBProgram 	*pFBProg = NULL;

CContainerContainer<CContainerFunction> *pFunctions = NULL;
CContainerContainer<CContainerFBProgram>	*pFBs = NULL;
CContainerContainer<CContainerVarBlock> *pVarBlocks = NULL;
CContainerContainer<CContainerIL>	*pILBlock = NULL;
CContainerContainer<CContainerTask>	*pTasks = NULL;
CContainerContainer<CContainerPCall>	*pPCalls = NULL;

VarBlocks		CurrentVarBlockType = EVarUndefined;
/*
 * Declaration of current output stream
 * for generated source
 */
ofstream	CurrentOutput;

int isVariableLocal(const char *pVarName)
{
	int iMax;

	if (pVarBlocks == NULL)
		return 0;
	
	iMax = pVarBlocks->size();
	
	for (int i = 0; i < iMax; i++)
	{
		if ((*pVarBlocks)[i]->FindVariable(pVarName) >= 0)
			return 1;
	}
	
	return 0;
}

void NewUseContainerVariable(void)
{
	if (pContainerVariable == NULL)
	{
		pContainerVariable = new CContainerVariable;
		pContainerVariable->SetProgramBlock(CurrentCodeBlockType);
	}
}

void FinishUseContainerVariable(void)
{
	if (pContainerVariable == NULL)
		return;
	
	NewUseContainerContainerVars();

	pContainerVariable->SetLine(iLineCount);
	pContainerVariable->SetVarBlockType(CurrentVarBlockType);

	(*pVariables) += pContainerVariable;

	pContainerVariable = NULL;
}

void NewUseContainerIncludes(void)
{
	if (pIncludes == NULL)
		pIncludes = new CContainerIncludes;
}

void NewUseContainerContainerVars(void)
{
	if (pVariables == NULL)
		pVariables = new CContainerVarBlock;
}

void FinishUseContainerContainerVars(void)
{
	if (pVariables == NULL)
		return;
	
	pVariables->SetCodeBlockType(CurrentCodeBlockType);

	switch(CurrentVarBlockType)
	{
		case EVarInput:
			pVariables->SetBlockType(EVarInput);
			break;
		case EVarOutput:
			pVariables->SetBlockType(EVarOutput);
			break;
		case EVarLocal:
			pVariables->SetBlockType(EVarLocal);
			break;
		case EVarIO:
			pVariables->SetBlockType(EVarIO);
			pVariablesIO = pVariables;
			break;
		case EVarExternal:
			pVariables->SetBlockType(EVarExternal);
			pVariablesExt = pVariables;
			pVariables = NULL;
			return;
		case EVarGlobal:
			pVariables->SetBlockType(EVarGlobal);

			if (pVariablesGlobal == NULL)
				pVariablesGlobal = pVariables;
			else
				(*pVariablesGlobal) += pVariables;

			pVariables = NULL;
			return;
	}

	if (pVarBlocks == NULL)
		pVarBlocks = new CContainerContainer<CContainerVarBlock>;

	(*pVarBlocks) += pVariables;

	pVariables = NULL;

}

void NewUseContainerBlock(ProgramBlocks BlockType, char *pName)
{
	switch(BlockType)
	{
		case EFunction:
			if (pFunction == NULL)
			{
				pFunction = new CContainerFunction(pName, iLineCount);
			}
			break;
		case EFunctionBlock:
		case EProgram:
			if (pFBProg == NULL)
			{
				pFBProg = new CContainerFBProgram(pName, BlockType, iLineCount);
			}
			break;

	}
}

void FinishUseContainerBlock(ProgramBlocks BlockType, char *pName)
{
	
	switch(BlockType)
	{
		case EFunction:
			if (pFunctions == NULL)
				pFunctions = new CContainerContainer<CContainerFunction>;
			if (pFunction == NULL)
				pFunction = new CContainerFunction(pName);

			(*pFunction) = pVarBlocks;
			(*pFunction) = pILBlock;
			(*pFunction) = pIncludes;

			(*pFunctions) += pFunction;

			pFunction = NULL;
			pVarBlocks = NULL;
			pILBlock = NULL;
			pIncludes = NULL;

			break;
		case EFunctionBlock:
		case EProgram:
			
			if (pFBs == NULL)
				pFBs = new CContainerContainer<CContainerFBProgram>;

			if (pFBProg == NULL)
				pFBProg = new CContainerFBProgram(pName, BlockType);

			(*pFBProg) = pVarBlocks;
			(*pFBProg) = pILBlock;
			(*pFBProg) = pIncludes;

			(*pFBs) += pFBProg;

			pVarBlocks = NULL;
			pILBlock = NULL;
			pIncludes = NULL;
			pFBProg = NULL;

			break;
		case EConfiguration:
			if (pConfiguration == NULL)
				pConfiguration = new CContainerConfiguration(pName);
			else
			{
				cerr << "Warning: second usage of configuration. Declaration ignored: " << pName << "\n";
				return;
			}

			(*pConfiguration) += pVariablesGlobal;
			(*pConfiguration) += pTasks;
			(*pConfiguration) += pPCalls;
			(*pConfiguration) += pIncludes;

			pIncludes = NULL;

			break;
	}

	DELETE(pVariablesExt);
	pVariablesIO = NULL;
}

void FinishUseContainerIL(void)
{
	if (pIL == NULL)
		return;
	
	if (pILBlock == NULL)
		pILBlock = new CContainerContainer<CContainerIL>;
	
	pIL->SetLine(iLineCount);

	(*pILBlock) += pIL;

	pIL = NULL;

}

void NewUseContainerIL(void)
{
	if (pIL != NULL)
		return;

	pIL = new CContainerIL;
}

void GenerateCode(void)
{
	if (pFunctions)
	{
		pFunctions->GenerateCode();
		pFunctions->GenerateCode(1);
	}

	if (pFBs)
	{
		pFBs->GenerateCode();
		pFBs->GenerateCode(1);
	}

	if (pConfiguration)
	{
		pConfiguration->GenerateCode(0);
		pConfiguration->GenerateCode(1);
	}
}

/*
 * Function for conversion of string time
 * into numeric time storage. Input pTime
 * sample: t#1ms
 */
long long ConvertString2Time(const char *pTime)
{
	const char *pModif[] = {"d", "h", "m", "s", "x"};
	const long long llMultip[] = {24*3600000000000, 3600000000000, 60000000000, 1000000000,1000000};
	string	sWorking;
	char	*pPos;
	int	iIndex, iIndexOld;
	long long llRetv = 0;

	if (pTime == NULL)
		return 0xffffffff;

	pPos = strstr(pTime, "#") + 1;

	if (pPos == NULL)
		return 0xffffffff;

	sWorking = pPos;
	
	// replace miliseconds with x for easily parsing of time
	iIndex = sWorking.find("ms");

	if (iIndex != -1)
		sWorking.replace(iIndex, 2, "x");

	iIndexOld = 0;
	
	for (int i = 0; i < 5; i++)
	{
		double dVal;
		
		if ((iIndex = sWorking.find(pModif[i])) != -1)
		{
			dVal = strtod(sWorking.substr(iIndexOld, iIndex - iIndexOld).c_str(), NULL);
			llRetv += (long long)(((double)llMultip[i]) * dVal);
			iIndexOld = iIndex + 1;
		}
	}

	return llRetv;
}

void NewUseContainerTask(char *&pTaskName)
{
	if (pTask == NULL)
		pTask = new CContainerTask(pTaskName);

	FREE(pTaskName);
}

void FinishUseContainerTask(void)
{
	if (pTask == NULL)
		return;

	if (pTasks == NULL)
		pTasks = new CContainerContainer<CContainerTask>;

	(*pTasks) += pTask;

	pTask = NULL;
}

void NewUseContainerPCall(char *&pCall)
{
	if (pPCall == NULL)
		pPCall = new CContainerPCall(pCall);

	FREE(pCall);
}

/*
 * Finish usage of current container
 * for program call.
 * Add container to list of program calls
 */
void FinishUseContainerPCall(void)
{
	if (pPCall == NULL)
		return;

	if (pPCalls == NULL)
		pPCalls = new CContainerContainer<CContainerPCall>;

	(*pPCalls) += pPCall;
	
	pPCall = NULL;
	
}

