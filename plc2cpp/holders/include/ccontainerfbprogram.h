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
 *  Version: $Revision: 1.3 $
 */

#ifndef _CCONTAINERFBPROGRAM_H_
	#define _CCONTAINERFBPROGRAM_H_

	#include <string>
	
	using namespace std;

	#include "plc2cpp_globals.h"

	#include "ccontainerbase.h"
	#include "ccontainercontainer.h"
	#include "ccontainervarblock.h"
	#include "ccontaineril.h"
	#include "ccontainerincludes.h"

	/*
	 * Class implementing code container for PLC
	 * function block or program source
	 * Class generates C++ source from PLC source
	 * divided into header file with function class
	 * and class implementation consisting from
	 * constructor and function implementation in C++
	 */
	class CContainerFBProgram: public CContainerBase
	{
		private:
			CContainerContainer<CContainerVarBlock>	*m_pVars;
			CContainerContainer<CContainerIL>	*m_pIL;
			CContainerIncludes			*m_pIncludes;
			ProgramBlocks				m_BlockType;
		protected:
		public:
			CContainerFBProgram(char *pName, ProgramBlocks BlockType, int iLineNumber = -1);
			~CContainerFBProgram(void);
			CContainerFBProgram& operator =(CContainerContainer<CContainerVarBlock> *pVars);
			CContainerFBProgram& operator =(CContainerContainer<CContainerIL> *pIL);
			CContainerFBProgram& operator =(CContainerIncludes *pIncludes);

			/*
			 * Generates particulary code for function
			 *  iCodePart == 0: Function class declaration (header)
			 *  iCodePart == 1: Function class implementation (code)
			 */
			void GenerateCode(int iCodePart = 0);
	};
#endif

