/* Log function overload signatures. */
#pragma once

#include <string>
#include <iostream>

void log(std::string name, int value);
void log(std::string name, std::string value);
void log(std::string value);
void log(std::string name, char* value);