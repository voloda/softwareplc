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

#ifndef _CCODEPROCESSOR_H_
	#define _CCODEPROCESSOR_H_

	#include "ccontainervariable.h"
	#include "ccontainervarblock.h"
	#include "ccontaineril.h"
	#include "ccontainerincludes.h"
	#include "ccontainertask.h"
	#include "ccontainerpcall.h"
	#include "ccontainerfunction.h"
	#include "ccontainercontainer.h"

	extern CContainerVariable	*pContainerVariable;
	extern CContainerIL		*pIL;
	extern CContainerIncludes	*pIncludes;
	extern CContainerVarBlock	*pVariablesExt;
	extern CContainerVarBlock	*pVariablesGlobal;
	extern CContainerVarBlock	*pVariablesIO;
	extern CContainerTask		*pTask;
	extern CContainerPCall		*pPCall;
	extern CContainerFunction	*pFunction;
	extern CContainerContainer<CContainerTask> *pTasks;

	extern VarBlocks		CurrentVarBlockType;

	void FinishUseContainerVariable(void);
	void NewUseContainerVariable(void);

	void NewUseContainerContainerVars(void);
	void FinishUseContainerContainerVars(void);

	void NewUseContainerBlock(ProgramBlocks BlockType, char *pName);
	void FinishUseContainerBlock(ProgramBlocks BlockType, char *pName);

	void GenerateCode(void);

	void NewUseContainerIL(void);
	void FinishUseContainerIL(void);

	void NewUseContainerIncludes(void);

	int isVariableLocal(const char *pVarName);

	void NewUseContainerTask(char *&pTaskName);
	void FinishUseContainerTask(void);

	void NewUseContainerPCall(char *&pCall);
	void FinishUseContainerPCall(void);
	
	
	/* Converts PLC string time format into unsigned long storage
	 * for PLC
	 */
	long long ConvertString2Time(const char *pTime);

#endif

