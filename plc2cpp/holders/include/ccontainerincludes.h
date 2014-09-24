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
 *  Version: $Revision: 1.2 $
 */

#ifndef _PLC2CPP_CONTAINERINCLUDES_H_
	#define _PLC2CPP_CONTAINERINCLUDES_H_

	#include <vector>
	#include <string>

	#include "plc2cpp_globals.h"
	#include "ccontainerbase.h"

	using namespace std;

	class CContainerIncludes: public CContainerBase
	{
		private:
			vector<string>	m_Includes;

			int IncludeExists(const char *pInclude);
		protected:
		public:
			CContainerIncludes(void);
			virtual ~CContainerIncludes(void);

			CContainerIncludes& operator +=(const char *pInclude);

			void GenerateCode(void);
	};
	
#endif
