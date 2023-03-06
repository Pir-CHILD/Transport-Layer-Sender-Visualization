#ifndef _TOOL_H
#define _TOOL_H
#include "sock_data.h"
#include <ctime>
#include <chrono>
#include <iomanip>
#include <fstream>

int split(std::string &line, std::string &pid, std::string &state, std::string &sk_add, std::string &timestamp);

void clear_strings(std::string &line, std::string &pid, std::string &state, std::string &sk_add, std::string &timestamp);

int state_2_num(std::string &state);

std::string convert_timestamp(long long timestamp);

SockData *extract_data(const char *file_name);

void deal_data(SockData *sock_data);
#endif