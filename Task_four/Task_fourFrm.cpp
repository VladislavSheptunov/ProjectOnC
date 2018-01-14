///-----------------------------------------------------------------
///
/// @file      Task_fourFrm.cpp
/// @author    Andrey
/// Created:   13.01.2018 13:26:46
/// @section   DESCRIPTION
///            Task_fourFrm class implementation
///
///------------------------------------------------------------------

#include "Task_fourFrm.h"

//Do not add custom headers between
//Header Include Start and Header Include End
//wxDev-C++ designer will remove them
////Header Include Start
////Header Include End

//----------------------------------------------------------------------------
// Task_fourFrm
//----------------------------------------------------------------------------
//Add Custom Events only in the appropriate block.
//Code added in other places will be removed by wxDev-C++
////Event Table Start
BEGIN_EVENT_TABLE(Task_fourFrm,wxFrame)
	////Manual Code Start
	////Manual Code End
	
	EVT_CLOSE(Task_fourFrm::OnClose)
	EVT_ACTIVATE(Task_fourFrm::Task_fourFrmActivate)
	
	EVT_TEXT(ID_WXEDIT1,Task_fourFrm::WxEdit1Updated0)
	EVT_BUTTON(ID_SET,Task_fourFrm::WxButton1Click)
END_EVENT_TABLE()
////Event Table End

Task_fourFrm::Task_fourFrm(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxFrame(parent, id, title, position, size, style)
{
	CreateGUIControls();
}

Task_fourFrm::~Task_fourFrm()
{
}

void Task_fourFrm::CreateGUIControls()
{
	//Do not add custom code between
	//GUI Items Creation Start and GUI Items Creation End
	//wxDev-C++ designer will remove them.
	//Add the custom code before or after the blocks
	////GUI Items Creation Start

	wxInitAllImageHandlers();   //Initialize graphic format handlers

	WxStaticBitmap1 = new wxStaticBitmap(this, ID_WXSTATICBITMAP1, wxNullBitmap, wxPoint(8, 8), wxSize(586, 499) );

	WxEdit4 = new wxTextCtrl(this, ID_WXEDIT4, _("Result"), wxPoint(610, 263), wxSize(121, 32), 0, wxDefaultValidator, _("WxEdit4"));

	WxStaticText4 = new wxStaticText(this, ID_WXSTATICTEXT4, _("Coefficient A"), wxPoint(618, 186), wxDefaultSize, 0, _("WxStaticText4"));

	WxStaticText3 = new wxStaticText(this, ID_WXSTATICTEXT3, _("Brand of cement"), wxPoint(614, 123), wxDefaultSize, 0, _("WxStaticText3"));

	WxStaticText2 = new wxStaticText(this, ID_WXSTATICTEXT2, _("Strength of concrete"), wxPoint(610, 62), wxDefaultSize, 0, _("WxStaticText2"));

	WxStaticText1 = new wxStaticText(this, ID_WXSTATICTEXT1, _("Enter value"), wxPoint(613, 23), wxDefaultSize, 0, _("WxStaticText1"));

	WxEdit3 = new wxTextCtrl(this, ID_WXEDIT3, _(""), wxPoint(609, 213), wxSize(121, 31), 0, wxDefaultValidator, _("WxEdit3"));

	WxEdit2 = new wxTextCtrl(this, ID_WXEDIT2, _(""), wxPoint(610, 148), wxSize(121, 31), 0, wxDefaultValidator, _("WxEdit2"));

	WxEdit1 = new wxTextCtrl(this, ID_WXEDIT1, _(""), wxPoint(611, 89), wxSize(121, 29), 0, wxDefaultValidator, _("WxEdit1"));

	Set = new wxButton(this, ID_SET, _("Calculation"), wxPoint(611, 309), wxSize(120, 40), 0, wxDefaultValidator, _("Set"));

	SetTitle(_("Task_four"));
	SetIcon(wxNullIcon);
	SetSize(13,-16,798,600);
	Center();
	
	////GUI Items Creation End
}

void Task_fourFrm::OnClose(wxCloseEvent& event)
{
	Destroy();
}

/*
 * Task_fourFrmActivate
 */
void Task_fourFrm::Task_fourFrmActivate(wxActivateEvent& event)
{
	// insert your code here
}

/*
 * WxButton1Click
 */
void Task_fourFrm::WxButton1Click(wxCommandEvent& event)
{	
	wxString brand = WxEdit2->GetValue();
	R_ten = 15;
	W  = 180.0;
	bool flag = true;
	
	double cement[150];
	double water[150];
	
	for (int i = 0; i <= 50; i++)
	   water[i] = i + 150;
    	
	if(WxEdit3->GetValue().ToDouble(&coeff_a) == false || (coeff_a != 0.55 && coeff_a != 0.65 && coeff_a != 0.6)){
        WxEdit4->SetValue("Invalid parameters!");
        flag = false;
    }
    
    if(WxEdit1->GetValue().ToDouble(&strength_concrete) == false){
        WxEdit4->SetValue("Invalid parameters!");
        flag = false;
    }

    if (brand.compare("M300") == 0){
        Rc = 30.0;
    }else if (brand.compare("M400") == 0){
           Rc = 40.0;
    }else if (brand.compare("M500") == 0){
        Rc = 50.0;
    } else {
        WxEdit4->SetValue("Invalid parameters!");
        flag = false;
    }
    
    if (flag){
        R_twenty_eight = R_ten * (log(28) / log(10));
        
        // Формула Скрамтаема 
        CW = ((R_twenty_eight * 10) / (coeff_a * Rc * 10)) + 0.5;
                
        C = W * CW;
        //WxEdit4->SetValue(wxString::Format(wxT("%lf"),C));
        WxEdit4->SetValue("Value is OK!");
        
        	
	wxClientDC dc(WxStaticBitmap1);
        
    dc.SetPen(wxPen(*wxBLACK, 2, wxSOLID));
        
    dc.DrawRectangle(40, 40, 500, 300);
                
    dc.SetPen(wxPen(*wxBLACK, 1, wxSOLID));
        
    wxFont font(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL);
    dc.SetFont(font);
    dc.SetTextForeground(*wxBLACK);
    dc.SetBackgroundMode(wxTRANSPARENT);
    
   for(int i = 1; i <= 5; i++){
       dc.DrawLine(40, 40 + (i * 50), 535, 40 + (i * 50));
       dc.DrawText(wxString::Format(_("%i"),200 - i*10 ), 10, 40 + (i * 50));
    
    }
    for(int i = 0; i <= 10; i++){
        dc.DrawLine(40 + (i * 50), 40, 40 + (i * 50),300);
        //dc.DrawText(wxString::Format(_("%i"),0 +  ), 10, 40 + (i * 50));
    }
    
    for(int i = 0; i <= 50; i++) {
        cement[i] = water[i] * CW;
        dc.DrawCircle(40 + ((int)water[i] - (int)water[0]) * 50, 40 + ((int)cement[i] - (int)cement[0]), 5);

    }
    }    
}

/*
 * WxEdit1Updated
 */
void Task_fourFrm::WxEdit1Updated(wxCommandEvent& event)
{
	// insert your code here
}

/*
 * SetClick
 */
void Task_fourFrm::SetClick(wxCommandEvent& event)
{
	// insert your code here
}

/*
 * WxEdit1Updated0
 */
void Task_fourFrm::WxEdit1Updated0(wxCommandEvent& event)
{
	// insert your code here
}
