//---------------------------------------------------------------------------
//
// Name:        wxJBupGUIApp.h
// Author:      Josh Bosley
// Created:     4/1/2014 6:29:28 PM
// Description: 
//
//---------------------------------------------------------------------------

#ifndef __JBUPApp_h__
#define __JBUPApp_h__

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#else
	#include <wx/wxprec.h>
#endif

class JbupApp : public wxApp
{
	public:
		bool OnInit();
		int OnExit();
};

#endif
