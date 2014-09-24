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
 *  Version: $Revision: 1.4 $
 */

#ifndef _CCONTAINERPCALL_H_
	#define _CCONTAINERPCALL_H_

	#include <string>

	using namespace std;

	#include "ccontainerbase.h"

	/*
	 * Container for storage of program
	 * call with or without task association
	 */
	class CContainerPCall : public CContainerBase
	{
		private:
			string	m_sTaskName;	// associated task
			string	m_sCode;	// generated code
			string	m_sProgramName;	// called program name
		protected:
		public:
			CContainerPCall(const char *pCallName);
			~CContainerPCall(void);
			void SetTaskName(const char *pTaskName);
			void SetProgramName(const char *pProgramName);
			/*
			 * Add parameter name
			 */
			void AddParmName(const char *pParmName);
			/*
			 * Add location address as parameter value
			 */
			void AddParmLocation(const char *pParmLocation);
			/*
			 * Add !GLOBAL! variable name as parameter value
			 */
			void AddParmVariable(const char *pParmVariable);

			/*
			 * Generates code from given informations
			 */
			void GenerateCode(int iStage);
	};
#endif

