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

#ifndef _CMeasure_H_
	#define _CMeasure_H_

	#include <wx/thread.h>
	#include <wx/panel.h>

	#include "measurevector.h"

	class CMeasureThread: public wxThread
	{
		private:
			int ReadData(int iInput, char pBuff[]);
			CMeasureVector<int>	*m_pProcessVariable;
			CMeasureVector<int>	*m_pSetPoint;
			unsigned char		m_Inputs[64];
			wxMutex			m_LockInputs;

			void			ResetDriver(void);
			int			m_iControl;
		public:
			CMeasureThread(CMeasureVector<int> *pProcessVariable = NULL, CMeasureVector<int> *pSetPoint = NULL);
			void SetVectors(CMeasureVector<int> *pProcessVariable = NULL, CMeasureVector<int> *pSetPoint = NULL);
			void SetRotations(int iRotations);
			virtual void *Entry();
	};
#endif

