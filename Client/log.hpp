/* Log function overload signatures. */
#pragma once

#include <string>
#include <iostream>

void cli_log(std::string name, int value);
void cli_log(std::string name, std::string value);
void cli_log(std::string value);
void cli_log(std::string name, char* value);