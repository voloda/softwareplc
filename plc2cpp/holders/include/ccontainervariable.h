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

#ifndef _CCONTAINERVARIABLE_H_
	#define _CCONTAINERVARIABLE_H_

	#include <stdlib.h>
	#include <string>

	#include "ccontainerbase.h"

	using namespace std;

	/*
	 * Class implementing container for single
	 * parsed variable. Container generates code
	 * specific for variable declaration and
	 * variable initialization depending on
	 * variable location
	 */
	class CContainerVariable:public CContainerBase
	{
		private:
			/*
			 * Data type of variable
			 */
			char	*m_pDataType;
			/*
			 * Name of variable
			 */
			string	m_sName;
			/*
			 * Initial value of variable
			 */
			char	*m_pInitialValue;
			/*
			 * Explicitly given location
			 * of variable in PLC internal memory
			 */
			char	*m_pMemLocation;

			/*
			 * If == 1 - variable is function block
			 * refernce. If == 0 variable is standard
			 * data type
			 */
			int	m_iFunctionBlock;
		
			/*
			 * If != 0, variable is declared as constant
			 */
			int	m_iConstant;

			ProgramBlocks	m_Block;

			inline void AllocateInitialValue(int iSize);

			VarBlocks	m_Type;
		protected:
		public:
			/*
			 * Class constructor
			 */
			CContainerVariable(void);
			/*
			 * Class destructor
			 */
			virtual ~CContainerVariable(void);
			/*
			 * Method for declaration of variable
			 * adress (location in memory)
			 */
			void SetLocation(char *pLocation = NULL, int iAutoAlloc = 0);
			/*
			 * Method for declaration of variable
			 * name
			 */
			void SetName(char *pName = NULL, int iAutoAlloc = 0);
			/*
			 * Method for declaration of variable
			 * datatype
			 */
			void SetDataType(const char *pDataType);
			/*
			 * Method for declaration of FB class
			 * name datatype
			 */
			void SetFBDataType(char *pDataType, int iAutoAlloc = 0);
			/*
			 * Method for setting of string initial
			 * value of variable
			 */
			void SetInitialValue(char *pValue, int iAutoAlloc = 0);
			/*
			 * Method for setting of integer initial
			 * value for variable
			 */
			void SetInitialValue(int iValue);
			/*
			 * Method for setting of double initial
			 * value for variable
			 */
			void SetInitialValue(double dValue);
			/*
			 * Method for generating variable
			 * initialization code (code in constructor
			 * of class implementing functions, function
			 * blocks etc.)
			 */
			void GenerateInitializationCode(int iOrder = 0);
			/*
			 * Method for generating variable
			 * declarations (code for private
			 * variable declarations in classes for
			 * functions, function blocks etc.)
			 */
			void GenerateClassDeclarationCode(void);
			/*
			 * Method for generating Property
			 * declarations code
			 */
			void SetProgramBlock(ProgramBlocks BlockType);
			void GenerateClassPropertyCode(int iOrder = 0);

			void SetConstant(void);

			int operator ==(const char *pVarName);

			void GenerateGlobalDeclarationCode(void);
			void GenerateGlobalHeaderCode(void);
			void GenerateExitCallCode(void);

			void SetVarBlockType(VarBlocks Type);
	};

#endif

