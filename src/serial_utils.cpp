#include "serial_utils.hpp"
#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <chrono>
#include <cstdio>

using namespace std;
using namespace boost::asio;

void send_scpi_command(serial_port& serial, const string& command, string& response, bool expect_response ) {
    if (!serial.is_open()) {
        cerr << "Serial port is not open!" << endl;
        return;
    }

    // ส่งคำสั่งไปยัง Serial Port
    write(serial, buffer(command + "\n"));

    // ถ้าคำสั่งไม่ต้องการ response ให้หยุดที่นี่
    if (!expect_response) {
        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
        //cout << "Command sent: " << command << endl;
        return;
    }

    // ถ้าคำสั่งต้องการ response
    boost::asio::streambuf buffer; //กำหนด namespace แบบเต็มเพื่อไม่ให้ compiler กำกวมกับการเลือกใช้ * ให้ใช้ class streambuf ของ boost *
    read_until(serial, buffer, '\n');

    // แปลงข้อมูลที่อ่านได้จาก buffer เป็น string
    istream input_stream(&buffer);
    getline(input_stream, response);

    // เช็คว่ามีข้อมูลหรือไม่
    if (!response.empty()) {
        //cout << "Received response: " << response << endl;
    } else {
        cerr << "No data received or read error" << endl;
    }
}

void set_voltage(serial_port& serial, double voltage) {
    string command = "VOLT " + to_string(voltage);
    string response;
    send_scpi_command(serial, command, response , false);
    cout << "Set Voltage to: " << voltage << " V" << endl;
}

void set_current(serial_port& serial, double current) {
    string command = "CURR " + to_string(current);
    string response;
    send_scpi_command(serial, command, response , false);
    cout << "Set Current to: " << current << " A" << endl;
}