//---------------------------------------------------------------------------
//
// Name:        Task_fourApp.cpp
// Author:      Andrey
// Created:     13.01.2018 13:26:46
// Description: 
//
//---------------------------------------------------------------------------

#include "Task_fourApp.h"
#include "Task_fourFrm.h"

IMPLEMENT_APP(Task_fourFrmApp)

bool Task_fourFrmApp::OnInit()
{
    Task_fourFrm* frame = new Task_fourFrm(NULL);
    SetTopWindow(frame);
    frame->Show();
    return true;
}
 
int Task_fourFrmApp::OnExit()
{
	return 0;
}
