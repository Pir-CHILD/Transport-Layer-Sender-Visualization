#include "sock_data.h"

const char *SockData::state_machine[5] = {"Open     ", "Disorder ", "CWR      ", "Recovery ", "Loss     "};

SockData::SockData(std::string s)
{
    sk_add = s;
    last_time_info.last_state = -1;
    last_time_info.last_time = 0;
    for (int i = 0; i < 5; i++)
    {
        state_transfer[i].state_name = state_machine[i];
    }
    for (int i = 0; i < 5; i++)
    {
        state_count[i].count = 0;
        state_count[i].state_name = state_machine[i];
    }
}

SockData::~SockData()
{
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < state_transfer[i].state_transfer_min_cnt.size(); j++)
        {
            delete state_transfer[i].state_transfer_min_cnt[j];
        }
        state_transfer[i].state_transfer_min_cnt.clear();
    }
    timestamp_record.clear();
    state_change_info.clear();
}