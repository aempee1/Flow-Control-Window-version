#include <wx/wx.h>
#include "mainwindow_properties.hpp"
#include <wx/aboutdlg.h>  // สำหรับ wxAboutDialogInfo
#include <wx/button.h>
#include <wx/image.h> // จำเป็นสำหรับ wxInitAllImageHandlers()
#include <wx/sizer.h>


MyFrame::MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    SetupMainMenu(); // เรียกใช้ฟังก์ชันสร้างเมนู

    this->SetMinSize(this->FromDIP(wxSize(200, 200)));
    this->SetSize(this->FromDIP(wxSize(500, 200)));

    this->Center();
}

void MyFrame::OnComportSettings(wxCommandEvent& WXUNUSED(event)) {
    ComportSettingsDialog dialog(this);
    dialog.ShowModal();
}

//----------------------------------------------------------------
void MyFrame::OnManualFlowsystem(wxCommandEvent& WXUNUSED(event)) {
    ManualCalibrationDialog dialog(this);
    dialog.ShowModal();
}

void MyFrame::SetupMainMenu() {
    wxMenuBar *menuBar = new wxMenuBar();

    // เมนู Setting
    wxMenu *settingMenu = new wxMenu();
    settingMenu->Append(1002, "Comport Settings");
    settingMenu->Append(wxID_ANY, "File Settings");

    // เมนู Production
    wxMenu *productionMenu = new wxMenu();
    productionMenu->Append(1003, "Manual Flow System");
    productionMenu->Append(wxID_ANY, "Auto Flow System");

    // เมนู R&D Test
    wxMenu *R_D_testMenu = new wxMenu();

    // เมนู Help
    wxMenu *helpMenu = new wxMenu();

    // เมนู About
    wxMenu *aboutMenu = new wxMenu();
    aboutMenu->Append(1001, "About Software");

    // เพิ่มเมนูลงใน MenuBar
    menuBar->Append(settingMenu, "&Setting");
    menuBar->Append(productionMenu, "&Production");
    menuBar->Append(R_D_testMenu, "&RnD Test");
    menuBar->Append(helpMenu, "&Help");
    menuBar->Append(aboutMenu, "&About");

    // ตั้งค่า MenuBar ให้กับหน้าต่าง
    SetMenuBar(menuBar);
}

//----------------------------------------------------------------
// ฟังก์ชันจัดการคลิกเมนู About
void MyFrame::OnAboutSoftware(wxCommandEvent& WXUNUSED(event)) {
    wxInitAllImageHandlers(); // เรียกใช้งาน Image Handlers
    wxDialog *aboutDialog = new wxDialog(this, wxID_ANY, "About", wxDefaultPosition, wxSize(350, 400), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

    // โหลดรูปภาพและปรับขนาด
    wxImage image(wxT("media/SutoLogo.png"), wxBITMAP_TYPE_PNG);
    if (!image.IsOk()) {
        wxMessageBox("Failed to load image!", "Error", wxICON_ERROR);
        return;
    }
    image.Rescale(100, 100); // ปรับขนาดรูปภาพให้เป็น 100x100 พิกเซล

    // แปลง wxImage เป็น wxBitmap
    wxBitmap resizedBitmap(image);
    wxStaticBitmap *imageCtrl = new wxStaticBitmap(aboutDialog, wxID_ANY, resizedBitmap);
    mainSizer->Add(imageCtrl, 0, wxALL | wxALIGN_CENTER, 10);


    // สร้าง GridSizer สำหรับข้อมูล 2 คอลัมน์และ 5 แถว
    wxGridSizer *gridSizer = new wxGridSizer(5, 2, 5, 0); // 5 แถว, 2 คอลัมน์, ระยะห่างระหว่างแถวและคอลัมน์

    // เพิ่มข้อมูลใน GridSizer
    gridSizer->Add(new wxStaticText(aboutDialog, wxID_ANY, "Software Name :"), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    gridSizer->Add(new wxStaticText(aboutDialog, wxID_ANY, "Flow Control"), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

    gridSizer->Add(new wxStaticText(aboutDialog, wxID_ANY, "Version :"), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    gridSizer->Add(new wxStaticText(aboutDialog, wxID_ANY, "1.0.0"), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

    gridSizer->Add(new wxStaticText(aboutDialog, wxID_ANY, "Release Date :"), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    gridSizer->Add(new wxStaticText(aboutDialog, wxID_ANY, "01 Jan 2025"), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

    gridSizer->Add(new wxStaticText(aboutDialog, wxID_ANY, "Author :"), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    gridSizer->Add(new wxStaticText(aboutDialog, wxID_ANY, "M"), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

    gridSizer->Add(new wxStaticText(aboutDialog, wxID_ANY, "Email :"), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    gridSizer->Add(new wxStaticText(aboutDialog, wxID_ANY, "khamae19499@gmail.com"), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

    // เพิ่ม GridSizer ลงใน MainSizer
    mainSizer->Add(gridSizer, 1, wxALL | wxEXPAND, 10);

    // เพิ่มปุ่ม OK
    wxButton *okButton = new wxButton(aboutDialog, wxID_OK, "OK");
    mainSizer->Add(okButton, 0, wxALL | wxALIGN_CENTER, 10);

    aboutDialog->SetSizer(mainSizer);
    aboutDialog->Layout();
    aboutDialog->ShowModal();
    aboutDialog->Destroy();
}

// เริ่มต้นตาราง Event
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(1001, MyFrame::OnAboutSoftware) // เชื่อมเมนู About กับฟังก์ชัน OnAboutSoftware
    EVT_MENU(1002, MyFrame::OnComportSettings)
    EVT_MENU(1003, MyFrame::OnManualFlowsystem) 
wxEND_EVENT_TABLE()
