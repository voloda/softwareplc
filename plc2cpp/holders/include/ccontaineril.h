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
 *  Version: $Revision: 1.9 $
 */

#ifndef _CCONTAINERIL_H_
	#define _CCONTAINERIL_H_

	#include <string>
	
	using namespace std;

	#include "ccontainerbase.h"

	#include "ccontainercontainer.h"
	/*
	 * Container for single IL language
	 * instruction
	 */
	class CContainerIL: public CContainerBase
	{
		private:
			string	m_sRight;
			string	m_sCode;
			string	m_sLabel;
			int	m_iCounter;
			int	m_iLocationCount;
			CContainerContainer<string>	m_FBParms;
			CContainerContainer<string>	m_FParms;
		protected:
		public:
			CContainerIL(void);
			virtual ~CContainerIL(void);
			void RightVariable(char *pVar, int iAutoFree = 1);
			void RightLocation(char *pLocation, int iAutoFree = 1);
			void RightValue(const double dValue);
			void RightValue(const int lValue);
			void RightValue(const long long iValue);
			void RightOperand(char *pOperand, int iAutoFree = 1);
			void AddFBParm(char *pParmName, int iAutoFree = 1);
			void AddFBName(char *pFBName, int iAutoFree = 1);
			void AddFunctionParm(void);
			void AddFunctionName(char *pFunctionName, int iAutoFree = 1);
			void Label(char *pLabel, int iAutoFree = 1);
			void GenerateCode(void);
			void LD(void);
			void LD_NEG(void);
			void ST(void);
			void ST_NEG(void);
			void S(void);
			void R(void);
			void AND(void);
			void AND_NEG(void);
			void OR(void);
			void OR_NEG(void);
			void XOR(void);
			void XOR_NEG(void);
			void ADD(void);
			void SUB(void);
			void MUL(void);
			void DIV(void);
			void GT(void);
			void GE(void);
			void EQ(void);
			void NE(void);
			void LE(void);
			void LT(void);
			void JMP(void);
			void JMPC(void);
			void JMPCN(void);
			void CAL(void);
			void CALC(void);
			void CALCN(void);
			void RET(void);
			void RETC(void);
			void RETCN(void);
	};
#endif

