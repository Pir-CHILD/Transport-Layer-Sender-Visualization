#ifndef _TOOL_H
#define _TOOL_H
#include <ctime>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <assert.h>
#include <sstream>
#include "../db/db.h"
#include "../sock_data/sock_data.h"

int split(std::string &line, std::string &pid, std::string &state, std::string &sk_add, std::string &timestamp);

void clear_strings(std::string &line, std::string &pid, std::string &state, std::string &sk_add, std::string &timestamp);

int state_2_num(std::string &state);

std::string convert_timestamp(long long timestamp);

SockData *extract_data(const char *file_name);

void deal_data(SockData *sock_data);

void cmd_get_sk_add(std::string &sk_add, const char *dest_ip);

void cmd_get_state_info(SockData *sock_data);
void cmd_get_state_info(SockData *sock_data, const char *test_info);
#endif