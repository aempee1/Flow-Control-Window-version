#ifndef COMPORT_SETTING_HPP
#define COMPORT_SETTING_HPP

#include <wx/wx.h>         // สำหรับ wxWidget
#include <wx/choice.h>
#include <vector>
#include <string>
#include <boost/asio.hpp>
#include "modbus_utils.hpp"
#include "serial_utils.hpp"

using namespace std;
using namespace boost::asio;


class ComportSettingsDialog : public wxDialog {
public:
    ComportSettingsDialog(wxWindow *parent);

private:
    vector<string> FetchAvailablePorts(); // ฟังก์ชันสำหรับดึงพอร์ตที่สามารถใช้ได้
    serial_port InitialSerial(io_service& io, const string& port_name);
    modbus_t* InitialModbus(const char* modbus_port);

    void SaveSelectedPorts();
    void LoadSelectedPorts();

    // ตัวแปรสำหรับเก็บพอร์ตที่เลือก
    string selectedBleAgentPort;
    string selectedModbusPort;
    string selectedPowerSupplyPort;
    string selectedModbusBaudRate;
    string selectedPowerSupplyBaudRate;

};

#endif // COMPORT_SETTINGS_HPP