/* Log function overload signatures. */
#pragma once
#include <string>
void log_data(std::string & name, int value);
void log_data(std::string & name, std::string & value);
void log_data(std::string & value);