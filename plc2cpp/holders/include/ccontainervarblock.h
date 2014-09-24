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
 *  Version: $Revision: 1.6 $
 */

#ifndef _CCONTAINERVARBLOCK_H_
	#define _CCONTAINERVARBLOCK_H_

	#include "ccontainercontainer.h"
	#include "ccontainervariable.h"

	/*
	 * Class implementing container for storage
	 * for list of variables
	 */
	class CContainerVarBlock:public CContainerContainer<CContainerVariable>
	{
		private:
			VarBlocks	m_Type;
			ProgramBlocks	m_ProgramType;
		protected:
		public:
			void SetCodeBlockType(ProgramBlocks BlockType);
			/*
			 * Set type of actual variable block
			 */
			void SetBlockType(VarBlocks BlockType);
			/*
			 * Get type of actual variable block
			 */
			VarBlocks	GetBlockType(VarBlocks BlockType);
			/*
			 * Generation of C++ source code
			 * iStage values are:
			 *   1 - Declaration stage for variables
			 *   2 - Declaration stage for parameters
			 *   3 - Value initialization stage (class constructor)
			 */
			void GenerateCode(int iStage);
			/*
			 * Class constructor
			 */
			CContainerVarBlock(void);
			/*
			 * Class destructor
			 */
			virtual ~CContainerVarBlock(void);

			/*
			 *  Find variable by name and return 
			 *  internal storage index
			 *  If not found, return -1
			 */
			int FindVariable(const char *pVarName);

	};
	
#endif
