#include "manual_calibrate.hpp"
#include "serial_utils.hpp"
#include <fstream>
#include <string>
#include <tuple>
#include <stdexcept>
#include <iostream>

using namespace std;
using namespace boost::asio;

// เพิ่มค่าเริ่มต้นของ PID Controller 
const double Kp = 0.04;
const double Ki = 0.01;
const double Kd = 0.04;

double integral = 0.0;
double previousError = 0.0;
double pidOutput = 10;
//-----------------------------
io_service io ;

double ManualCalibrationDialog::calculatePID(double setpointValue, double currentValue) {
    double errorValue = setpointValue - currentValue;
    integral += errorValue;
    double derivativeValue = errorValue - previousError;
    previousError = errorValue;

    double PID_output = (Kp * errorValue) + (Ki * integral) + (Kd * derivativeValue);
    return PID_output;
}

serial_port ManualCalibrationDialog::InitialSerial(io_service& io, const string& port_name)
{
    serial_port serial(io, port_name);
    serial.set_option(serial_port_base::baud_rate(9600));
    serial.set_option(serial_port_base::character_size(8));
    serial.set_option(serial_port_base::parity(serial_port_base::parity::none));
    serial.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
    serial.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));

    if (!serial.is_open())
    {
        cerr << "Failed to open serial port!" << endl;
        throw runtime_error("Failed to open serial port");
    }
    cout << "Successfully initialized Serial on port: " << port_name << endl;
    return serial;
}

modbus_t* ManualCalibrationDialog::InitialModbus(const char* modbus_port) {
    modbus_t* ctx = initialize_modbus(modbus_port);
    return ctx;
}

tuple<string, string, string> ReadPortsFromFile(const string& fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        throw runtime_error("Unable to open the port configuration file."); // โยนข้อผิดพลาดหากเปิดไฟล์ไม่ได้
    }

    string bluetoothPort, modbusPort, serialPort;
    getline(file, bluetoothPort);
    getline(file, modbusPort);
    getline(file, serialPort);

    if (bluetoothPort.empty() || modbusPort.empty() || serialPort.empty()) {
        throw runtime_error("The port configuration file must contain at least 3 lines."); // โยนข้อผิดพลาดหากข้อมูลไม่ครบ
    }

    return make_tuple(bluetoothPort, modbusPort, serialPort); // คืนค่าพอร์ตทั้งหมดในรูปแบบ tuple
}

bool ManualCalibrationDialog::CheckAndLoadPorts(const string& fileName, vector<string>& ports) {
    cout << "Attempting to open the file: " << fileName << endl;  // เพิ่ม log message
    ifstream file(fileName);
    if (!file.is_open()) {
        wxMessageBox("Unable to open the file selected_ports.txt", "Error", wxOK | wxICON_ERROR, this);
        cerr << "Failed to open file: " << fileName << endl;  // เพิ่ม log message
        return false;
    }

    cout << "File opened successfully: " << fileName << endl;  // เพิ่ม log message

    string port;
    while (getline(file, port)) {
        if (!port.empty()) {
            ports.push_back(port);
        }
    }

    if (ports.empty()) {
        wxMessageBox("You don't have selected ports", "Warning", wxOK | wxICON_WARNING, this);
        cerr << "No ports selected in the file." << endl;  // เพิ่ม log message
        return false;
    }

    cout << "Ports loaded successfully." << endl;  // เพิ่ม log message
    return true;
}


ManualCalibrationDialog::ManualCalibrationDialog(wxWindow *parent)
    : wxDialog(parent, wxID_ANY, "Manual Calibration", wxDefaultPosition, wxSize(400, 300), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
      setpoint(0), timer(new wxTimer(this)) , modbusCtx(nullptr) , serialCtx(io)  // กำหนดค่าเริ่มต้นให้ timer
{
    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer *gridSizer = new wxFlexGridSizer(4, 3, 10, 10);

    // Set Flow Row
    gridSizer->Add(new wxStaticText(this, wxID_ANY, "Set Flow:"), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    setFlowInput = new wxTextCtrl(this, wxID_ANY);
    gridSizer->Add(setFlowInput, 1, wxEXPAND);

    wxButton *setButton = new wxButton(this, wxID_ANY, "Set");
    gridSizer->Add(setButton, 0, wxALIGN_CENTER);

    // Bind event handler ให้กับปุ่ม set
    setButton->Bind(wxEVT_BUTTON, &ManualCalibrationDialog::OnSetButtonClick, this);

    // Ref. Flow Row
    gridSizer->Add(new wxStaticText(this, wxID_ANY, "Ref. Flow:"), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    refFlowInput = new wxTextCtrl(this, wxID_ANY);
    refFlowInput->SetEditable(false); // Make the field read-only
    gridSizer->Add(refFlowInput, 1, wxEXPAND);
    gridSizer->Add(new wxStaticText(this, wxID_ANY, "m3/h"), 0, wxALIGN_CENTER_VERTICAL);

    // Act. Flow Row
    gridSizer->Add(new wxStaticText(this, wxID_ANY, "Act. Flow:"), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    actFlowInput = new wxTextCtrl(this, wxID_ANY);
    actFlowInput->SetEditable(false); // Make the field read-only
    gridSizer->Add(actFlowInput, 1, wxEXPAND);
    gridSizer->Add(new wxStaticText(this, wxID_ANY, "m3/h"), 0, wxALIGN_CENTER_VERTICAL);

    // Error Row
    gridSizer->Add(new wxStaticText(this, wxID_ANY, "Error:"), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    errorInput = new wxTextCtrl(this, wxID_ANY);
    errorInput->SetEditable(false);
    gridSizer->Add(errorInput, 1, wxEXPAND);
    gridSizer->Add(new wxStaticText(this, wxID_ANY, "%"), 0, wxALIGN_CENTER_VERTICAL);

    // Center Grid
    wxBoxSizer *gridCenterSizer = new wxBoxSizer(wxHORIZONTAL);
    gridCenterSizer->Add(gridSizer, 0, wxALIGN_CENTER);

    mainSizer->Add(gridCenterSizer, 1, wxALL | wxALIGN_CENTER, 10);

    // Done Button
    wxButton *doneButton = new wxButton(this, wxID_OK, "Done");
    mainSizer->Add(doneButton, 0, wxALL | wxALIGN_CENTER, 10);

    SetSizer(mainSizer);
    Layout();
    Center();

    srand(std::time(nullptr));

    // Load ports from file
    vector<string> selectedPorts;
    if (!CheckAndLoadPorts("selected_ports.txt", selectedPorts)) {
        return; // หากไม่มีพอร์ตที่เลือกให้หยุดการทำงานของ dialog
    }

    if (selectedPorts.size() < 2) {
        wxMessageBox("Selected ports are insufficient.", "Error", wxOK | wxICON_ERROR, this);
        return;
    }

    // Initialize Modbus
    modbusCtx = InitialModbus(selectedPorts[1].c_str());
    if (!modbusCtx) {
        wxMessageBox("Failed to initialize Modbus.", "Error", wxOK | wxICON_ERROR, this);
        return;
    }

    // Initialize Serial Port (เรียกใช้ฟังก์ชัน init_serial_port)
    serialCtx = InitialSerial(io, selectedPorts[2].c_str());

    cout << "Modbus and Serial port are initialized successfully." << std::endl;

}

ManualCalibrationDialog::~ManualCalibrationDialog() {
    if (timer->IsRunning()) {
        timer->Stop();
    }
    if (modbusCtx) {
        modbus_close(modbusCtx);
        modbus_free(modbusCtx);
    }
    if (serialCtx.is_open()) {
        serialCtx.close();
    }
    delete timer;
}

void ManualCalibrationDialog::OnSetButtonClick(wxCommandEvent &event) {
    wxString inputStr = setFlowInput->GetValue();
    long value;
    if (inputStr.ToLong(&value)) {
        setpoint = static_cast<int>(value);

        // เริ่มทำงาน timer สำหรับสุ่มค่า
        if (!timer->IsRunning()) {
            timer->Start(2000); // สุ่มทุก 1 วินาที
        }
    } else {
        wxMessageBox("Invalid input. Please enter an integer value.", "Error", wxOK | wxICON_ERROR, this);
    }
}

void ManualCalibrationDialog::OnTimer(wxTimerEvent &event) {
    uint16_t refFlow[4] ;
    int rc ;
    do{
        rc = modbus_read_registers(modbusCtx, /*register address=*/6, /*num registers=*/2, refFlow);
        if (rc == -1) {
        return;
    }
    }while(rc == -1);
    float refFlowValue ;
    memcpy(&refFlowValue , refFlow, sizeof(refFlowValue));
    // อัปเดตค่า refFlow
    refFlowInput->SetValue(wxString::Format("%.2f", refFlowValue));
    // คำนวณค่า Act. Flow โดยใช้ PID
    pidOutput += calculatePID(setpoint, refFlowValue);
    cout << "PID output from calculate " <<pidOutput << endl;
    if(pidOutput < 10){
        pidOutput = 10 ;
    }else if(pidOutput > 40){
        pidOutput = 40 ;
    }
    set_voltage(serialCtx,pidOutput);

    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // หน่วงเวลา 500 ms

    // อัปเดตค่าที่คำนวณได้
    //actFlowInput->SetValue(wxString::Format("%.2f", pidOutput));

    // คำนวณ Error
    double errorValue_percentage = 100 - (refFlowValue*100)/setpoint;
    errorInput->SetValue(wxString::Format("%.2f", errorValue_percentage));

}

// Bind Event Table
wxBEGIN_EVENT_TABLE(ManualCalibrationDialog, wxDialog)
    EVT_TIMER(wxID_ANY, ManualCalibrationDialog::OnTimer)
wxEND_EVENT_TABLE()
