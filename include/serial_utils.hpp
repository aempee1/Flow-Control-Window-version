#ifndef SERIAL_UTILS_HPP
#define SERIAL_UTILS_HPP

#include <string>
#include <boost/asio.hpp>
#include <vector>

using namespace std;
using namespace boost::asio;

void send_scpi_command(serial_port& serial, const string& command, string& response , bool expect_response = true);
void set_voltage(serial_port& serial, double voltage);
void set_current(serial_port& serial, double current);



#endif // SERIAL_UTILS_HPP