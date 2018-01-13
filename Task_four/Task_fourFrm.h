///-----------------------------------------------------------------
///
/// @file      Task_fourFrm.h
/// @author    Andrey
/// Created:   13.01.2018 13:26:46
/// @section   DESCRIPTION
///            Task_fourFrm class declaration
///
///------------------------------------------------------------------

#ifndef __TASK_FOURFRM_H__
#define __TASK_FOURFRM_H__

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
	#include <wx/frame.h>
#else
	#include <wx/wxprec.h>
#endif

//Do not add custom headers between 
//Header Include Start and Header Include End.
//wxDev-C++ designer will remove them. Add custom headers after the block.
////Header Include Start
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
////Header Include End

////Dialog Style Start
#undef Task_fourFrm_STYLE
#define Task_fourFrm_STYLE wxCAPTION | wxSYSTEM_MENU | wxMINIMIZE_BOX | wxCLOSE_BOX
////Dialog Style End

#include "string.h"
#include <math.h>

class Task_fourFrm : public wxFrame
{
	private:
		DECLARE_EVENT_TABLE();
		
	    double  Rc,                // Type cement
                R_ten,             // Strength 10 day 
                R_twenty_eight,    // Strength 28 day
                CW,                // Ц/В           
                C,                 // Ц
                W,                 // Water
                coeff_a,           // Coefficient A
                strength_concrete; // Прочность бетона
                
	public:
		Task_fourFrm(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Task_four"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = Task_fourFrm_STYLE);
		virtual ~Task_fourFrm();
		void Task_fourFrmActivate(wxActivateEvent& event);
		void WxButton1Click(wxCommandEvent& event);
		void WxEdit1Updated(wxCommandEvent& event);
		void SetClick(wxCommandEvent& event);
		void WxEdit1Updated0(wxCommandEvent& event);
		
	private:
		//Do not add custom control declarations between
		//GUI Control Declaration Start and GUI Control Declaration End.
		//wxDev-C++ will remove them. Add custom code after the block.
		////GUI Control Declaration Start
		wxTextCtrl *WxEdit4;
		wxStaticText *WxStaticText4;
		wxStaticText *WxStaticText3;
		wxStaticText *WxStaticText2;
		wxStaticText *WxStaticText1;
		wxTextCtrl *WxEdit3;
		wxTextCtrl *WxEdit2;
		wxTextCtrl *WxEdit1;
		wxButton *Set;
		////GUI Control Declaration End
		
	private:
		//Note: if you receive any error with these enum IDs, then you need to
		//change your old form code that are based on the #define control IDs.
		//#defines may replace a numeric value for the enum names.
		//Try copy and pasting the below block in your old form header files.
		enum
		{
			////GUI Enum Control ID Start
			ID_WXEDIT4 = 1012,
			ID_WXSTATICTEXT4 = 1010,
			ID_WXSTATICTEXT3 = 1009,
			ID_WXSTATICTEXT2 = 1008,
			ID_WXSTATICTEXT1 = 1007,
			ID_WXEDIT3 = 1006,
			ID_WXEDIT2 = 1005,
			ID_WXEDIT1 = 1004,
			ID_SET = 1001,
			////GUI Enum Control ID End
			ID_DUMMY_VALUE_ //don't remove this value unless you have other enum values
		};
		
	private:
		void OnClose(wxCloseEvent& event);
		void CreateGUIControls();
};

#endif
