#include <string>
#include <boost/asio.hpp>
#include <vector>
#include "comport_setting.hpp"
#include "serial_utils.hpp"
#include "modbus_utils.hpp"
#include <fstream>


#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <dirent.h>
#include <regex>
#endif

using namespace std;
using namespace boost::asio;

serial_port ComportSettingsDialog::InitialSerial(io_service& io, const string& port_name)
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

modbus_t* ComportSettingsDialog::InitialModbus(const char* modbus_port) {
    modbus_t* ctx = initialize_modbus(modbus_port);
    return ctx;
}

vector<string> ComportSettingsDialog::FetchAvailablePorts() {
    vector<string> ports;

#ifdef _WIN32
    char portName[10];
    for (int i = 1; i <= 256; ++i) {
        snprintf(portName, sizeof(portName), "COM%d", i);
        HANDLE hPort = CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (hPort != INVALID_HANDLE_VALUE) {
            ports.push_back(portName);
            CloseHandle(hPort);
        }
    }
#endif

#ifdef __APPLE__
    DIR* dir = opendir("/dev");
    if (dir) {
        struct dirent* entry;
        regex serialRegex("^tty\\..*"); // พอร์ต serial บน macOS มักจะขึ้นต้นด้วย "cu."
        while ((entry = readdir(dir)) != NULL) {
            if (regex_match(entry->d_name, serialRegex)) {
                ports.push_back("/dev/" + string(entry->d_name));
            }
        }
        closedir(dir);
    }
#endif

    return ports;
}

// Other includes and namespace remain unchanged

void ComportSettingsDialog::SaveSelectedPorts() {
    ofstream outFile("selected_ports.txt");
    outFile << selectedBleAgentPort << endl;
    outFile << selectedModbusPort << endl;
    outFile << selectedPowerSupplyPort << endl;
    outFile.close();
}

void ComportSettingsDialog::LoadSelectedPorts() {
    ifstream inFile("selected_ports.txt");
    if (inFile) {
        getline(inFile, selectedBleAgentPort);
        getline(inFile, selectedModbusPort);
        getline(inFile, selectedPowerSupplyPort);
        inFile.close();
    }
}

ComportSettingsDialog::ComportSettingsDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "Comport Settings", wxDefaultPosition, wxSize(500, 200), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    LoadSelectedPorts();

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer* gridSizer = new wxFlexGridSizer(3, 3, 10, 10);

    auto availablePorts = FetchAvailablePorts();
    wxArrayString baudRates;
    baudRates.Add("9600");
    baudRates.Add("19200");
    baudRates.Add("38400");
    baudRates.Add("57600");
    baudRates.Add("115200");

    // BLEAgent Label และ Choice
    gridSizer->Add(new wxStaticText(this, wxID_ANY, "BLEAgent:"), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL);
    wxChoice* bleAgentChoice = new wxChoice(this, wxID_ANY);
    for (const auto& port : availablePorts) {
        bleAgentChoice->Append(port);
    }
    bleAgentChoice->SetStringSelection(selectedBleAgentPort);
    gridSizer->Add(bleAgentChoice, 1, wxALIGN_CENTER_HORIZONTAL);
    gridSizer->AddSpacer(1); // Empty column for BLEAgent row

    // Modbus Label และ Choice
    gridSizer->Add(new wxStaticText(this, wxID_ANY, "Modbus:"), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL);
    wxChoice* modbusChoice = new wxChoice(this, wxID_ANY);
    for (const auto& port : availablePorts) {
        modbusChoice->Append(port);
    }
    modbusChoice->SetStringSelection(selectedModbusPort);
    gridSizer->Add(modbusChoice, 1, wxALIGN_CENTER_HORIZONTAL);

    wxChoice* modbusBaudChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, baudRates);
    modbusBaudChoice->SetStringSelection(selectedModbusBaudRate);
    gridSizer->Add(modbusBaudChoice, 1, wxALIGN_CENTER_HORIZONTAL);

    // Power Supply Label และ Choice
    gridSizer->Add(new wxStaticText(this, wxID_ANY, "Power Supply:"), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL);
    wxChoice* powerSupplyChoice = new wxChoice(this, wxID_ANY);
    for (const auto& port : availablePorts) {
        powerSupplyChoice->Append(port);
    }
    powerSupplyChoice->SetStringSelection(selectedPowerSupplyPort);
    gridSizer->Add(powerSupplyChoice, 1, wxALIGN_CENTER_HORIZONTAL);

    wxChoice* powerSupplyBaudChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, baudRates);
    powerSupplyBaudChoice->SetStringSelection(selectedPowerSupplyBaudRate);
    gridSizer->Add(powerSupplyBaudChoice, 1, wxALIGN_CENTER_HORIZONTAL);

    mainSizer->Add(gridSizer, 1, wxALL | wxALIGN_CENTER, 10);

    wxButton* okButton = new wxButton(this, wxID_OK, "OK");
    mainSizer->Add(okButton, 0, wxALL | wxALIGN_CENTER, 10);

    okButton->Bind(wxEVT_BUTTON, [this, bleAgentChoice, modbusChoice, powerSupplyChoice, modbusBaudChoice, powerSupplyBaudChoice](wxCommandEvent& event) {
        selectedBleAgentPort = bleAgentChoice->GetStringSelection().ToStdString();
        selectedModbusPort = modbusChoice->GetStringSelection().ToStdString();
        selectedPowerSupplyPort = powerSupplyChoice->GetStringSelection().ToStdString();
        selectedModbusBaudRate = stoi(modbusBaudChoice->GetStringSelection().ToStdString());
        selectedPowerSupplyBaudRate = stoi(powerSupplyBaudChoice->GetStringSelection().ToStdString());

        SaveSelectedPorts();

        io_service io;
        serial_port serial = InitialSerial(io, selectedPowerSupplyPort.c_str());
        modbus_t* modbusContext = InitialModbus(selectedModbusPort.c_str());

#ifdef _WIN32

        string cmd_modbus = "mode " + selectedModbusPort + ": baud=" + selectedModbusBaudRate;
        system(cmd_modbus.c_str());

        string cmd_power_supply = "mode " + selectedPowerSupplyPort + ": baud=" + selectedPowerSupplyBaudRate;
        system(cmd_power_supply.c_str());
#endif 

        wxLogMessage("BLEAgent Port: %s", selectedBleAgentPort);
        wxLogMessage("Modbus Port: %s", selectedModbusPort);
        wxLogMessage("Power Supply Port: %s", selectedPowerSupplyPort);

        if (modbusContext) {
            wxLogMessage("Modbus and Serial initialized successfully.");
        }
        else {
            wxLogError("Failed to initialize Modbus.");
        }

        if (modbusContext) {
            modbus_close(modbusContext);
            modbus_free(modbusContext);
        }

        this->EndModal(wxID_OK);
        });

    SetSizer(mainSizer);
    Layout();
}
