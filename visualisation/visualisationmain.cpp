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
 *  Version: $Revision: 1.3 $
 */

#include "include/visualisationmain.h"
#include <wx/msgdlg.h>
#include <wx/spinctrl.h>
#include <wx/dcscreen.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>

#include "include/measure.h"

CMeasureThread	*MeasureThread = NULL;

BEGIN_EVENT_TABLE(CVisualisationMain, wxFrame)
	EVT_SPINCTRL(IDSP_ROTATION_PER_SEC, CVisualisationMain :: OnSetRotation)
	EVT_SIZE(CVisualisationMain :: OnSize)
END_EVENT_TABLE()

CVisualisationMain :: CVisualisationMain(void) : wxFrame(NULL, ID_VISUALISATION_MAIN_WINDOW, "PLC Sample visualisation", wxPoint(0, 0), wxSize(300, 400))
{
	new wxStaticText(this, -1, "Pozadovane otacky/s:", wxPoint(5, 10));

	m_RotPerSec = new wxSpinCtrl(this, IDSP_ROTATION_PER_SEC, "0", wxPoint(130, 5), wxSize(70, 30), wxSP_ARROW_KEYS, -1000, 1000);

	MeasureThread = new CMeasureThread;

	MeasureThread->Create();
	MeasureThread->Run();
	MeasureThread->SetRotations(0);

	m_Canvas = new CVisualisationCanvas(this, wxPoint(0, 40), wxSize(300, 360));

}

CVisualisationMain :: ~CVisualisationMain(void)
{
	m_Canvas = NULL;
	m_RotPerSec = NULL;
}

void CVisualisationMain :: OnSetRotation(wxSpinEvent &event)
{
	
 	switch (event.GetId())
	{
		case IDSP_ROTATION_PER_SEC:
			if (m_RotPerSec != NULL)
				MeasureThread->SetRotations(m_RotPerSec->GetValue());
			break;
	}
		
}

void CVisualisationMain :: OnSize(wxSizeEvent &event)
{

	if (m_Canvas != NULL)
	{
		wxSize	NewSize;

		NewSize = event.GetSize();
	
		NewSize.SetHeight(NewSize.GetHeight() - 40);

		m_Canvas->SetSize(NewSize);
	}
}

