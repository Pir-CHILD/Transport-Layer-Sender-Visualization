#ifndef _SOCK_DATA_H
#define _SOCK_DATA_H
#include <cstring>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <exception>

struct LastTimeInfo
{
    int last_state;
    long long last_time;
};

struct StateChangeInfoMsItem
{
    int pid;
    int state;
    long long timestamp;
};

struct StateTransferMinCount
{
    std::string state_name;
    std::vector<int *> state_transfer_min_cnt; // 1st min[Open 2 CWR]
};

struct StateCount
{
    std::string state_name;
    int count;
};

class SockData
{
public:
    StateCount state_count[5];
    static const char *state_machine[5];
    StateTransferMinCount state_transfer[5];
    std::vector<long long> timestamp_record;
    std::vector<StateChangeInfoMsItem> state_change_info;

    SockData(std::string s);
    ~SockData();

    std::string get_sk_add() const { return sk_add; }
    LastTimeInfo get_last_time_info() const { return last_time_info; }
    void update_last_time_info(int state_t, long long timestamp_t)
    {
        last_time_info.last_state = state_t;
        last_time_info.last_time = timestamp_t;
    }

private:
    std::string sk_add;
    LastTimeInfo last_time_info;
};
#endif