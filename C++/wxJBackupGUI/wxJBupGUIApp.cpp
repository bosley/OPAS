//---------------------------------------------------------------------------
//
// Name:        wxJBupGUIApp.cpp
// Author:      Josh Bosley
// Created:     4/1/2014 6:29:28 PM
// Description: 
//
//---------------------------------------------------------------------------
// Job information
#define JOB_LIST "JConfig.txt"

#include "wxJBupGUIApp.h"
#include "wxJBupGUIFrm.h"

IMPLEMENT_APP(JbupApp)

bool JbupApp::OnInit()
{
    Jbup* frame = new Jbup(NULL);
    SetTopWindow(frame);
    
    // Set Background and show
    frame->SetBackgroundColour(wxColour(wxT("OFFWHITE")));
    frame->Show();
    return true;
}
 
int JbupApp::OnExit()
{
	return 0;
}
