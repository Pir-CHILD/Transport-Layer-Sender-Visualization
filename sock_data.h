#include <cstring>
#include <vector>
#include <string>

struct LastTimeInfo
{
    int last_state;
    long long last_time;
};

struct StateChangeInfoItem 
{
    int pid;
    int state;
    long long timestamp;
};

struct StateTransferItem
{
    std::string state_name;
    std::vector<int *> state_transfer_min_cnt; // 1st min[Open 2 CWR]
};

struct StateCount
{
    std::string state_name;
    int count;
};


class SockDate {
    public:
        StateCount state_count[5];
        static const char *state_machine[5];
        StateTransferItem state_transfer[5];
        std::vector<StateChangeInfoItem> state_change_info;
        
        SockDate(std::string s);
        ~SockDate();

        std::string get_sk_add() const {return sk_add;}
        LastTimeInfo get_last_time_info() const {return last_time_info;}
    private:
        std::string sk_add;
        LastTimeInfo last_time_info;
};