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

#ifndef _CMeasureVector_H_
	#define _CMeasureVector_H_

	#include <wx/thread.h>

	#include <vector>

	using namespace std;

	template <class StoreType> class CMeasureVector:public vector<StoreType>
	{
		private:
			int	m_iSize;
			wxMutex	m_Mutex;
		protected:
		public:
			CMeasureVector(void)
			{
				m_iSize = 0;
			};
			
			void Lock(void)
			{
				m_Mutex.Lock();
			};

			void Unlock(void)
			{
				m_Mutex.Unlock();
			};
			/**
			 * Sets number of items, which can
			 * be stored in vector. If number of
			 * items is reached, they are overwrited
			 * from begin
			 *
			 * @param iNewSize New size of vector
			 */
			void SetSize(int iNewSize)
			{
				m_iSize = iNewSize;
			}
			
			void AddValue(StoreType Value)
			{
				if (m_iSize == 0)
					return;

				this->push_back(Value);

				while(size() > m_iSize)
				{
					erase(begin());

					if (size() == 0)
						break;
				}
			};

			
	};
	
#endif
