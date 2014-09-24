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

#include <stdio.h>
#include "include/plc2cpp_globals.h"

using namespace std;

void ParsePathString(vector<string> &vsTarget, const string sPath, const string sDelim)
{
	int	iPos, iPosOld = 0;

	while((iPos = sPath.find(sDelim, iPosOld)) !=  -1)
	{
		vsTarget.push_back(sPath.substr(iPosOld, (iPos - iPosOld)));
		iPosOld = iPos + 1;
	}
	
	if (sPath.substr(iPosOld).length() > 0)
		vsTarget.push_back(sPath.substr(iPosOld));
}

void GetRealSourcePath(string &sSourceFile, const vector<string> &vsPaths)
{
	int	iSize;

	iSize = vsPaths.size();

	for(int i = -1; i < iSize; i++)
	{
		string	sTestPath;
		FILE	*fTest;
		
		if (i == -1)
			sTestPath = sSourceFile;
		else
			sTestPath = vsPaths[i] + "/" + sSourceFile;

		fTest = fopen(sTestPath.c_str(), "r");

		if (fTest)
		{
			sSourceFile = sTestPath;
			fclose(fTest);
			return;
		}
	}
}

void WriteListOfGeneratedFiles(const string sTargetFile)
{
	ofstream	fFileList;

	fFileList.open(sTargetFile.c_str());

	for (int i = 0; i < vsGeneratedFiles.size(); i++)
		fFileList << vsGeneratedFiles[i] << "\n";
}

void AddDependFile(const string sFileName)
{
	for (int i = 0; i < vsParseFiles.size(); i++)
	{
		if (vsParseFiles[i] == sFileName)
			return;
	}
	
	vsParseFiles.push_back(sFileName);
}

