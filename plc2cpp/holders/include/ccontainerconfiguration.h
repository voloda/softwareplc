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

#ifndef _CCONTAINERCONFIGURATION_H_
	#define _CCONTAINERCONFIGURATION_H_

	#include "plc2cpp_globals.h"
	#include "ccontainerbase.h"
	#include "ccontainervarblock.h"
	#include "ccontainercontainer.h"
	#include "ccontainerpcall.h"
	#include "ccontainertask.h"
	#include "ccontainerincludes.h"

	class CContainerConfiguration : public CContainerBase
	{
		private:
			CContainerVarBlock	*m_pGlobalVars;
			CContainerContainer<CContainerTask>	*m_pTasks;
			CContainerContainer<CContainerPCall>	*m_pPCalls;
			CContainerIncludes			*m_pIncludes;
		protected:
		public:
			CContainerConfiguration(char *pName);
			~CContainerConfiguration(void);
			void GenerateCode(int iType);
			CContainerConfiguration& operator +=(CContainerVarBlock *pBlock);
			CContainerConfiguration& operator +=(CContainerContainer<CContainerPCall> *pPCalls);
			CContainerConfiguration& operator +=(CContainerContainer<CContainerTask> *pTasks);
			CContainerConfiguration& operator +=(CContainerIncludes *pIncludes);
	};

#endif
