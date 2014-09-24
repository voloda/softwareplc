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
 *  Version: $Revision: 1.5 $
 */

#ifndef _CCONTAINERCONTAINER_H
	#define _CCONTAINERCONTAINER_H
	
	#include <vector>
	
	using namespace std;
	
	/*
	 * Base class implementing container
	 * for lists of container - list of
	 * single variables, list of variables
	 * blocks etc.
	 */
	template<class StoreType> class CContainerContainer
	{
		private:
		protected:
			vector<StoreType *>	m_Records;
		public:
			CContainerContainer(void)
			{
			};

			/*
			 * Class destructor frees all memory
			 * allocated in vector of records
			 */
			virtual ~CContainerContainer(void)
			{
				vector<StoreType *>::iterator Iter;
				
				for (Iter = m_Records.begin(); m_Records.size() > 0; Iter = m_Records.begin())
				{
					delete *Iter;

					m_Records.erase(Iter);
				}

			};

			CContainerContainer<StoreType>& operator +=(StoreType *pType)
			{
				m_Records.push_back(pType);
				return(*this);
			};

			CContainerContainer<StoreType>& operator +=(CContainerContainer<StoreType> *pBlock)
			{
				if (pBlock == NULL)
					return(*this);

				for(int i = 0; i < pBlock->size(); i++)
					(*this) += pBlock->m_Records[i];

				return(*this);
			}

			StoreType * operator [](int iItem)
			{
				return m_Records[iItem];
			};

			StoreType * operator [](const char *pBlockName)
			{
				for (int i = 0; i < this->size(); i++)
				{
					if (strcmp(m_Records[i]->GetBlockName(), pBlockName) == 0)
						return(m_Records[i]);
				}

				return NULL;
			};
			
			int size(void)
			{
				return m_Records.end() - m_Records.begin();
			}

			void GenerateCode(int iStage = 0)
			{
				vector<StoreType *>::iterator	Iter;

				for (Iter = m_Records.begin(); Iter < m_Records.end(); Iter++)
				{
					(*Iter)->GenerateCode(iStage);
				}
			}
	};

#endif

