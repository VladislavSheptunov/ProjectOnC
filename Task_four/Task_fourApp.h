//---------------------------------------------------------------------------
//
// Name:        Task_fourApp.h
// Author:      Andrey
// Created:     13.01.2018 13:26:46
// Description: 
//
//---------------------------------------------------------------------------

#ifndef __TASK_FOURFRMApp_h__
#define __TASK_FOURFRMApp_h__

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#else
	#include <wx/wxprec.h>
#endif

class Task_fourFrmApp : public wxApp
{
	public:
		bool OnInit();
		int OnExit();
};

#endif
