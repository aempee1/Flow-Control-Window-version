#ifndef MAINWINDOW_PROPERTIES_HPP
#define MAINWINDOW_PROPERTIES_HPP

#include <wx/wx.h>
#include "manual_calibrate.hpp"
#include "comport_setting.hpp"

// สร้างคลาสหลักสำหรับหน้าต่าง
class MyFrame : public wxFrame {
public:
    MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size);

private:
    void SetupMainMenu();                // ฟังก์ชันสำหรับสร้างเมนู
    void OnAboutSoftware(wxCommandEvent& event); // ฟังก์ชันสำหรับจัดการคลิกเมนู About
    void OnComportSettings(wxCommandEvent& event); // ฟังก์ชันสำหรับจัดการคลิกเมนู Comport Settings
    void OnManualFlowsystem(wxCommandEvent& event); //
    wxDECLARE_EVENT_TABLE();             // ประกาศ Event Table
};


#endif // MAINWINDOW_PROPERTIES_HPP