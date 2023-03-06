#include "sock_data.h"
#include <ctime>
#include <chrono>
#include <iomanip>

int split(std::string &line, int &pid, std::string &state, std::string &sk_add, long long &timestamp);

void clear_strings(std::string &line, std::string &state, std::string &sk_add);

int state_2_num(std::string &state);

std::string &convert_timestamp(long long timestamp);