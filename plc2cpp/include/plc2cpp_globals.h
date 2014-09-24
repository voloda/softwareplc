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
 *  Version: $Revision: 1.10 $
 */

#ifndef _PLC2CPP_GLOBALS_H_
	#define _PLC2CPP_GLOBALS_H_

	#include <ctype.h>
	#include <string>
	#include <fstream>
	#include <vector>

	using namespace std;

	/*
	 * Actual active output stream for storage
	 * of generated sources
	 */
	extern ofstream	CurrentOutput;

	/*
	 * Name of target directory where generated
	 * sources are stored
	 */
	extern char *pTargetDirectory;

	/*
	 * Name of PLC project 
	 */
	extern string sProjectName;

	/*
	 * Global variable containing name of current
	 * processed source file. If NULL stdin is processed
	 */
	extern string sActualFile;

	/*
	 * Global variable containing current line
	 * number of processed file
	 */
	extern int iLineCount;

	/*
	 * Exit code for program execution. If non zero
	 * value, an error occured when proceeding source
	 * file.
	 */
	extern int iExitCode;
        /*
	 * Enumerated datatype for setting type
	 * of current parsed program block
	 */
        typedef enum ProgramBlocks
        {
		EFunction,
		EFunctionBlock,
		EProgram,
		EResource,
		EConfiguration
        };

	/*
	 * Global variable containing current parsed
	 * program block type
	 */
	extern ProgramBlocks CurrentCodeBlockType;

        /*
	 * Enumerated datatype for setting type
	 * of current parsed variable block
	 */
	typedef enum VarBlocks
	{
		EVarInput,
		EVarOutput,
		EVarLocal,
		EVarIO,
		EVarGlobal,
		EVarExternal,
		EVarUndefined
	};

	/*
	 * Safe replacement for delete call when
	 * destroying dynamically created objects
	 */
	template<class Type> void DELETE(Type &pVar)
	{
		if (pVar != NULL)
		{
			delete pVar;
			pVar = NULL;
		}
	}

	/*
	 * Safe replacement for free() function when
	 * deallocationg dynamic memory
	 */
	template<class Type> void FREE(Type &pVar)
	{
		if (pVar != NULL)
		{
			free(pVar);
			pVar = NULL;
		}
	}

	/**
	 * Converts specified string to lower case letters
	 *
	 * @param	sConverted	String for conversion to lower case
	 */
	static inline void str2lower(string &sConverted)
	{
		int	iSize;

		iSize = sConverted.length();
		
		for(int i = 0; i < iSize; i++)
		{
			sConverted[i] = tolower(sConverted[i]);
		}
		
	}

	/**
	 * Strips underlines from given null terminated string
	 *
	 * @param pString	String to strip underlines
	 */

	static inline void StripUnderlines(char *pString)
	{
		int	iSize;
		char	*pWritePos;
		
		// include terminating null
		iSize = strlen(pString) + 1;

		pWritePos = pString;

		for (int i = 0; i < iSize; i++)
		{
			if (pString[i] != '_')
			{
				(*pWritePos) = pString[i];
				pWritePos++;
			}
		}
		
	}

	/**
	 * Parses string with library directories
	 * and values add's into vector
	 *
	 * @param	vsTarget	Vector with library directories
	 * @param	sPath	String delimited with sDelim which is parsed
	 * 			into vsTarget
	 * @param	sDelim	Delimiter for records
	 */
	void ParsePathString(vector<string> &vsTarget, const string sPath, const string sDelim = ":");

	/**
	 * Find's path where source file is located. At first search source
	 * without directory, then searches library directories
	 *
	 * @param	sFile	On input source file as given on command line,
	 * 			on output contains modified path with added
	 * 			library directory
	 * @param	vsLibdDirs	Vector with library directories
	 */
	void GetRealSourcePath(string &sFile, const vector<string> &vsLibDirs);

	/**
	 * Write's list of generated C++ files and headers
	 * from given PLC source
	 * @param	sTargetFile	Name of file, which will contain list
	 * 				of generated files
	 */
	void WriteListOfGeneratedFiles(const string sTargetFile);
	
	/**
	 * Add's new file to list of files to parse, if it already
	 * isn't there
	 *
	 * @param	sFileName	Name of file to add to list
	 */
	void AddDependFile(const string sFileName);

	extern long long	llSinglePeriod;
	extern long long	llNoTaskPeriod;
	extern vector<string>	vsGeneratedFiles;
	extern vector<string>	vsParseFiles;
#endif

