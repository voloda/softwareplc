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

#include <wx/pen.h>
#include <wx/dcmemory.h>
#include <wx/bitmap.h>
#include <wx/msgdlg.h>
#include <wx/thread.h>

#include "include/visualisationcanvas.h"
#include "include/measure.h"

#define ROT_MAX	110
#define ROT_MIN	-110

BEGIN_EVENT_TABLE(CVisualisationCanvas, wxPanel)
	EVT_SIZE( CVisualisationCanvas :: OnSize)
	EVT_TIMER(-1, CVisualisationCanvas :: OnTimer)
END_EVENT_TABLE()

extern CMeasureThread *MeasureThread;

CVisualisationCanvas :: CVisualisationCanvas(wxFrame *pParent, const wxPoint &coord, const wxSize &size)
	: wxPanel(pParent, -1, coord, size), m_RedrawTimer(this)
{
	m_pParent = pParent;
	
	MeasureThread->SetVectors(&m_CurrentValue, &m_WantedValue); //, &m_CurrentValue);

	// SetBackgroundColour(wxColor(0, 0, 0));

	m_RedrawTimer.Start(50);
}

CVisualisationCanvas :: ~CVisualisationCanvas()
{
	m_RedrawTimer.Stop();

	MeasureThread->SetVectors();
}


void CVisualisationCanvas :: DrawAxes(wxDC &dc)
{
	wxSize	MySize;
	wxString	sDisplay;
	double		dPosun, dConst;
       	int		iPos;
	int		iDrawSubAxes[] = {100, 75, 50, 25, -25, -50, -75, -100};

	wxPen	AxesPen(wxColour(255, 255, 255), 1, wxSOLID);
	
	MySize = this->GetSize();
	
	dc.SetPen(AxesPen);
	
	dPosun = MySize.GetHeight() / 2;

	dc.DrawLine(29, (int) dPosun, MySize.GetWidth(), (int)dPosun);


	dc.DrawLine(30, 0, 30, MySize.GetHeight());
	
	dc.SetTextBackground(wxColor(0, 0, 0));
	dc.SetTextForeground(wxColor(255, 255, 255));
	
	AxesPen.SetStyle(wxDOT);
	
	dc.SetPen(AxesPen);
	
	dConst = dPosun / (double)ROT_MAX;

	for (int i = 0; i < sizeof(iDrawSubAxes)/sizeof(int); i++)
	{
		char pBuff[50];
		
		iPos = (int)(dPosun - iDrawSubAxes[i] * dConst);
	
		dc.DrawLine(29, iPos, MySize.GetWidth(), iPos);

		snprintf(pBuff, 50, "%d", iDrawSubAxes[i]);
		
		WriteText(dc, pBuff, iPos, 27);
	}
	
	WriteText(dc, "0", (int) dPosun, 27);
}

void CVisualisationCanvas :: WriteText(wxDC &dc, const wxString &sText, int iYMiddle, int iXMax)
{
	wxCoord	TextWidth, TextHeight;

	dc.GetTextExtent(sText, &TextWidth, &TextHeight);

	dc.DrawText(sText, iXMax - TextWidth, iYMiddle - (TextHeight / 2));
}

void CVisualisationCanvas :: OnSize(wxSizeEvent& event)
{
	wxSize	NewSize;
	
	NewSize = event.GetSize();
	
	m_WantedValue.Lock();
	m_WantedValue.SetSize(NewSize.GetWidth());
	m_WantedValue.Unlock();

	m_CurrentValue.Lock();
	m_CurrentValue.SetSize(NewSize.GetWidth());
	m_CurrentValue.Unlock();

}

void CVisualisationCanvas :: OnTimer(wxTimerEvent& event)
{
	m_RedrawTimer.Stop();

	wxSize		PanelSize = this->GetSize();
	wxBitmap	Bitmap(PanelSize.GetWidth(), PanelSize.GetHeight());
	wxMemoryDC	dc;
	wxClientDC	dcPanel(this);
	int		iSize, iSizeWanted;
	wxBrush		bkBrush(wxColor(0, 0, 0), 1);
	char		pBuff[100];
	
	double	dConst;
	int	iPosun;

	dc.SelectObject(Bitmap);
	dc.SetBackground(bkBrush);
	dc.Clear();

	m_DrawLock.Lock();
	
	dc.BeginDrawing();
	
	DrawAxes(dc);

	m_WantedValue.Lock();
	m_CurrentValue.Lock();
	
	iSize = m_CurrentValue.size();
	iSizeWanted = m_WantedValue.size();

	iPosun = PanelSize.GetHeight() / 2;
	dConst = (double)iPosun / (double)ROT_MAX;

	for (int i = 1; i < iSize; i++)
	{
		
		int	RecalcYOld, RecalcY;
		
		if (iSizeWanted >= i)
		{
			RecalcYOld = iPosun - (int)(m_WantedValue[i - 1] * dConst);
			RecalcY = iPosun - (int)(m_WantedValue[i] * dConst);
			
			dc.SetPen(*wxRED_PEN);
			dc.DrawLine(i - 1, RecalcYOld, i, RecalcY);
		}
		
		RecalcYOld = iPosun - (int)(m_CurrentValue[i - 1] * dConst);
		RecalcY = iPosun - (int)(m_CurrentValue[i] * dConst);

		dc.SetPen(*wxGREEN_PEN);
		dc.DrawLine(i - 1, RecalcYOld, i, RecalcY);
	}

	snprintf(pBuff, 100, "Aktualni otacky/s: %d", m_CurrentValue[m_CurrentValue.size() - 1]);

	WriteText(dc, pBuff, 10, PanelSize.GetWidth() - 5);
			
	m_CurrentValue.Unlock();
	m_WantedValue.Unlock();

	dc.EndDrawing();

	PanelSize = this->GetSize();
	
	dcPanel.Blit(0, 0, PanelSize.GetWidth(), PanelSize.GetHeight(), &dc, 0, 0);

	m_DrawLock.Unlock();

	m_RedrawTimer.Start(50);
}

