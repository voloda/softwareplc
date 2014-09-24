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
 *  Version: $Revision: 1.19 $
 */

#include <stdio.h>

#include "plc2cpp_globals.h"

#include "include/ccontaineril.h"
#include "include/ccodeprocessor.h"

CContainerIL :: CContainerIL(void)
{
	m_iCounter = 0;	// initialize counter
	m_iLocationCount = 0;
}

CContainerIL :: ~CContainerIL(void)
{
}

void CContainerIL :: GenerateCode(void)
{
	CurrentOutput << GenerateLineInfo();
	// insert label if exists
	if (m_sLabel.length() > 0)
		CurrentOutput << m_sLabel.c_str() << ":\n";

	CurrentOutput << m_sCode.c_str();
}

void CContainerIL :: RightVariable(char *pVariable, int iAutoFree)
{
	if ((pVariablesExt != NULL) && (pVariablesExt->FindVariable(pVariable) >= 0))
	{
		m_sRight = "g_";
	}
	else if ((pVariablesIO == NULL) || ((pVariablesIO != NULL) && (pVariablesIO->FindVariable(pVariable) < 0)))
		m_sRight = "m_";

	m_sRight += pVariable;

	if (iAutoFree)
		free(pVariable);

	str2lower(m_sRight);
}

void CContainerIL :: RightLocation(char *pLocation, int iAutoFree)
{
	string	sType;
	char	pBuff[20];
	
	snprintf(pBuff, 20, "tmp%d", m_iLocationCount++);

	m_sCode += "\tCVariable<";

	if (strstr(pLocation, "B"))
		sType = "BYTE";
	else if (strstr(pLocation, "W"))
		sType = "WORD";
	else if (strstr(pLocation, "D"))
		sType = "DWORD";
	else if (strstr(pLocation, "L"))
		sType = "LDWORD";
	else
		sType = "BIT";

	m_sCode += sType;
	m_sCode += "> ";
	m_sCode += pBuff;
       	m_sCode	+= " = CVariable<";
	m_sCode += sType;
	m_sCode += ">(\"";
	m_sCode += pLocation;
	m_sCode += "\", 0);\n";

	m_sRight += pBuff;
	
	if (iAutoFree)
		free(pLocation);
}

void CContainerIL :: RightValue(const double dValue)
{
	char	pBuff[30];

	snprintf(pBuff, 30, "%e", dValue);

	m_sRight = pBuff;
}

void CContainerIL :: RightOperand(char *pOperand, int iAutoFree)
{
	m_sRight = pOperand;

	str2lower(m_sRight);

	if (iAutoFree)
		free(pOperand);
}

void CContainerIL :: RightValue(const int lValue)
{
	char	pBuff[30];

	snprintf(pBuff, 30, "%d", lValue);

	m_sRight = pBuff;
}

void CContainerIL :: Label(char *pLabel, int iAutoFree)
{
	m_sLabel = pLabel;

	str2lower(m_sLabel);

	if (iAutoFree)
		free(pLabel);
}

void CContainerIL :: AddFBParm(char *pParmName, int iAutoFree)
{
	string	*sNewParm;
	
	sNewParm = new string;
	
	(*sNewParm) = pParmName;
	(*sNewParm) += " = ";
	(*sNewParm) += m_sRight;
	(*sNewParm) += ";\n";

	str2lower(*sNewParm);

	m_sRight = "";

	m_FBParms += sNewParm;
		
	if (iAutoFree)
		free(pParmName);
}

void CContainerIL :: AddFBName(char *pFBName, int iAutoFree)
{
	// if function block name is not
	// local or global variable, it's
	// call for programm block which
	// is declared as global kernel symbol
	// with prefix gprog_
	// also add include for program block
	if (isVariableLocal(pFBName))
		m_sRight = "m_";
	else if ((pVariablesExt != NULL) && (pVariablesExt->FindVariable(pFBName) >= 0))
		m_sRight = "g_";
	else
	{
		string	sNewInclude;

		sNewInclude = pFBName;
		sNewInclude += ".h";

		NewUseContainerIncludes();
		(*pIncludes) += sNewInclude.c_str();
		m_sRight = "gprog_plcp_";
	}

	m_sRight += pFBName;

	str2lower(m_sRight);

	if (iAutoFree)
		free(pFBName);
}

void CContainerIL :: AddFunctionName(char *pFunctionName, int iAutoFree)
{
	string	sCode;

	sCode = "\t{\n\t" + m_sCode + "\t\t";
	sCode += string("plcf_") + string(pFunctionName);
	sCode += "\tf_function;\n\t\t";
	sCode += "result.GetValue(f_function.parm0);\n";
	
	for (int i = 0; i < m_FParms.size(); i++)
	{
		sCode += "\t\tf_function";
		sCode += (*m_FParms[i]);
	}
	
	sCode += "\t\tf_function.Function();\n";
	sCode += "\t\tresult = f_function.GetResult();\n";
	sCode += "\t}\n\n";

	m_sCode = sCode;
}

void CContainerIL :: AddFunctionParm(void)
{
	string	*sParm;
	char	pBuff[10];
	
	if (m_sRight.length() == 0)
		return;

	m_iCounter++;
	
	sParm = new string;

	snprintf(pBuff, 10, "%d", m_iCounter);

	(*sParm) = ".parm";
	(*sParm) += pBuff;
	(*sParm) += " = ";
	(*sParm) += m_sRight;
	(*sParm) += ";\n";

	m_sRight = "";

	m_FParms += sParm;
}

void CContainerIL :: LD(void)
{
	m_sCode = "\t{\n" + m_sCode;
	m_sCode += "\tresult = ";
	m_sCode += m_sRight;
	m_sCode += ";\n\t}\n";
}

void CContainerIL :: LD_NEG(void)
{
	m_sCode = "\t{\n" + m_sCode;
	m_sCode += "\tresult = ";
	m_sCode += "~(" + m_sRight + ")";
	m_sCode += ";\n\t}\n";
}

void CContainerIL :: ST(void)
{
	m_sCode = "\t{\n" + m_sCode;
	m_sCode += "\tresult.GetValue(";
	m_sCode += m_sRight;
	m_sCode += ");\n\t}\n";
}

void CContainerIL :: ST_NEG(void)
{
	m_sCode = "\t{\n" + m_sCode;
	m_sCode += "\tresult.GetValueNeg(";
	m_sCode += m_sRight;
	m_sCode += ");\n\t}\n";
}

void CContainerIL :: S(void)
{
	m_sCode = "\tif (result == true) {\n" + m_sCode;
	m_sCode += m_sRight;
	m_sCode += " = 1;\n\t}\n";
}

void CContainerIL :: R(void)
{
	m_sCode = "\tif (result == true) {\n" + m_sCode;
	m_sCode += m_sRight;
	m_sCode += " = 0;\n\t}\n";
}

void CContainerIL :: AND(void)
{
	m_sCode = "\t{\n" + m_sCode;
	m_sCode += "result = result & ";
	m_sCode += m_sRight;
	m_sCode += ";\n\t}\n";
}

void CContainerIL :: AND_NEG(void)
{
	m_sCode = "\t{\n" + m_sCode;
	m_sCode += "\tresult = result & ~(";
	m_sCode += m_sRight;
	m_sCode += ");\n\t}\n";
}

void CContainerIL :: OR(void)
{
	m_sCode = "\t{\n" + m_sCode;
	m_sCode += "\tresult = result | ";
	m_sCode += m_sRight;
	m_sCode += ";\n\t}\n";
}

void CContainerIL :: OR_NEG(void)
{
	m_sCode = "\t{\n" + m_sCode;
	m_sCode += "\tresult = result | ~(";
	m_sCode += m_sRight;
	m_sCode += ");\n\t}\n";
}

void CContainerIL :: XOR(void)
{
	m_sCode = "\t{\n" + m_sCode;
	m_sCode += "\tresult = result ^ ";
	m_sCode += m_sRight;
	m_sCode += ";\n\t}\n";
}

void CContainerIL :: XOR_NEG(void)
{
	m_sCode = "\t{\n" + m_sCode;
	m_sCode += "\tresult = result ^ ~(";
	m_sCode += m_sRight;
	m_sCode += ");\n\t}\n";
}

void CContainerIL :: ADD(void)
{
	m_sCode = "\t{\n" + m_sCode;
	m_sCode += "\tresult = result + ";
	m_sCode += m_sRight;
	m_sCode += ";\n\t}\n";
}

void CContainerIL :: SUB(void)
{
	m_sCode = "\t{\n" + m_sCode;
	m_sCode += "\tresult = result - ";
	m_sCode += m_sRight;
	m_sCode += ";\n\t}\n";
}

void CContainerIL :: MUL(void)
{
	m_sCode = "\t{\n" + m_sCode;
	m_sCode += "\tresult = result * ";
	m_sCode += m_sRight;
	m_sCode += ";\n\t}\n";
}

void CContainerIL :: DIV(void)
{
	m_sCode = "\t{\n" + m_sCode;
	m_sCode += "\tresult = result / ";
	m_sCode += m_sRight;
	m_sCode += ";\n\t}\n";
}

void CContainerIL :: GT(void)
{
	m_sCode = "\t{\n" + m_sCode;
	m_sCode += "\tresult = (result > ";
	m_sCode += m_sRight;
	m_sCode	+= ");\n\t}\n";	
}

void CContainerIL :: GE(void)
{
	m_sCode = "\t{\n" + m_sCode;
	m_sCode += "\tresult = (result >= ";
	m_sCode += m_sRight;
	m_sCode	+= ");\n\t}\n";
}

void CContainerIL :: EQ(void)
{
	m_sCode = "\t{\n" + m_sCode;
	m_sCode += "\tresult = (result == ";
	m_sCode += m_sRight;
	m_sCode	+= ");\n\t}\n";
}

void CContainerIL :: NE(void)
{
	m_sCode = "\t{\n" + m_sCode;
	m_sCode += "\tresult = (result != ";
	m_sCode += m_sRight;
	m_sCode	+= ");\n\t}\n";
}

void CContainerIL :: LE(void)
{
	m_sCode = "\t{\n" + m_sCode;
	m_sCode += "\tresult = (result <= ";
	m_sCode += m_sRight;
	m_sCode	+= ");\n\t}\n";
}

void CContainerIL :: LT(void)
{
	m_sCode = "\t{\n" + m_sCode;
	m_sCode += "\tresult = (result < ";
	m_sCode += m_sRight;
	m_sCode	+= ");\n\t}\n";
}

void CContainerIL :: JMP(void)
{
	m_sCode += "\t{\n\t\tgoto ";
	m_sCode += m_sRight;
	m_sCode	+= ";\n\t}\n";
}

void CContainerIL :: JMPC(void)
{
	m_sCode = "\n\tif (result != 0)\n";
	this->JMP();
}

void CContainerIL :: JMPCN(void)
{
	m_sCode = "\n\tif (result == 0)\n";
	this->JMP();
}

void CContainerIL :: CAL(void)
{
	m_sCode = "\t{\n" + m_sCode;
	for(int i = 0; i < m_FBParms.size(); i++)
	{
		m_sCode += "\t\t";
		m_sCode += m_sRight;
		m_sCode += ".";
		m_sCode += (*m_FBParms[i]);
	}

	m_sCode += "\t\t";
	m_sCode += m_sRight;
	m_sCode	+= ".Function();\n\t}\n\n";
}

void CContainerIL :: CALC(void)
{
	m_sCode += "\tif (result != 0)\n";
	this->CAL();
}

void CContainerIL :: CALCN(void)
{
	m_sCode += "\tif (result == 0)\n";
	this->CAL();
}

void CContainerIL :: RET(void)
{
	// use macro declared in function (returns value)
	// or function block
	m_sCode = "\n\tRETURN();\n";
}

void CContainerIL :: RETC(void)
{
	m_sCode = "\n\tif (result != 0)\n\t\tRETURN();\n";
}

void CContainerIL :: RETCN(void)
{
	m_sCode = "\n\tif (result == 0)\n\t\tRETURN();\n";
}

void CContainerIL :: RightValue(const long long iValue)
{
	char	pBuff[30];

	snprintf(pBuff, 30, "%ll", iValue);

	m_sRight = pBuff;
}

