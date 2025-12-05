/* Log function overload signatures. */
#pragma once

#include <string>
#include <iostream>

void srv_log(std::string name, int value);
void srv_log(std::string name, std::string value);
void srv_log(std::string value);
void srv_log(std::string name, char* value);