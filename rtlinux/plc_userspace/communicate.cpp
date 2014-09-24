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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "plc_uspace.h"

int ReadData(int iInput, char pBuff[])
{
	int	iSize;

	iSize = 0;
		
	while(iSize != 65)
	{
		int iSizeTmp;
		
		iSizeTmp = read(iInput, pBuff + iSize, 65 - iSize);

		if (iSizeTmp == -1)
			return (-1);

		iSize += iSizeTmp;
	}

	return 1;
}

void Posli(void)
{
	char	pData[64];
	int	iF;
	
	memset(pData, 'F', 64);

	iF = open("/dev/rtf1", O_WRONLY);

	write(iF, pData, 64);

	close(iF);
}

int main(int argc, char *argv[])
{
	int	iControl, iInput, iCall = 0;
	struct	PLCUserSpaceControl	Control;
	
	Posli();

	iControl = open("/dev/rtf0", O_WRONLY);
	iInput = open("/dev/rtf2", O_RDONLY);

	Control.Command = PLC_RESET;
	
	write(iControl, &Control, sizeof(Control));

	Control.Command = PLC_SEND_SET_TIME;
	Control.iTime = 1000;
	
	write(iControl, &Control, sizeof(Control));

	while(1)
	{
		char pBuff[65];

		iCall++;
		
	
		if (ReadData(iInput, pBuff) == -1)
			return -1;

		if (pBuff[0] == 'I')
			printf("\nValues of inputs (call: %d):\n", iCall);
		else
			printf("\nValues of outputs\n");

		for(int i = 0; i < 64; i++)
			printf("%02X ", (unsigned int)(unsigned char) pBuff[i + 1]);

		printf("\n");

	}
	return 0;
}
