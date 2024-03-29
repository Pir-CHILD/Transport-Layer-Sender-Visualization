#include "tool.h"

int split(std::string &line, std::string &pid, std::string &state, std::string &sk_add, std::string &timestamp)
{
    std::string::size_type state_pos, sk_add_pos, timestamp_pos;
    state_pos = line.find(" ", 0);
    if (state_pos != std::string::npos)
    {
        sk_add_pos = line.find(" ", state_pos + 1);
        if (sk_add_pos != std::string::npos)
        {
            timestamp_pos = line.find(" ", sk_add_pos + 1);
            if (timestamp_pos != std::string::npos)
            {
                pid = line.substr(0, state_pos);
                state = line.substr(state_pos + 1, sk_add_pos - state_pos - 1);
                sk_add = line.substr(sk_add_pos + 1, timestamp_pos - sk_add_pos - 1);
                timestamp = line.substr(timestamp_pos + 1);
                return 0; // success
            }
        }
    }
    return 1; // fail
}

void clear_strings(std::string &line, std::string &pid, std::string &state, std::string &sk_add, std::string &timestamp)
{
    line.clear();
    pid.clear();
    state.clear();
    sk_add.clear();
    timestamp.clear();
}

int state_2_num(std::string &state)
{
    if (!state.compare("ca_state=0x0"))
        return 0;
    else if (!state.compare("ca_state=0x1"))
        return 1;
    else if (!state.compare("ca_state=0x2"))
        return 2;
    else if (!state.compare("ca_state=0x3"))
        return 3;
    else if (!state.compare("ca_state=0x4"))
        return 4;
    else
        return 5; // fail
}

std::string convert_timestamp(long long timestamp)
{
    // get a precise timestamp as a string
    const std::chrono::duration<long long, std::milli> dur(timestamp);
    const std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp(dur);
    const std::time_t now = std::chrono::system_clock::to_time_t(tp);
    const std::chrono::duration<long long, std::milli> now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()) % 1000;

    std::stringstream s;
    s << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << '.' << std::setfill('0') << std::setw(3) << now_ms.count();

    std::string time_ms = s.str();

    return time_ms;
}

SockData *extract_data(const char *file_name)
{
    std::ifstream infile;
    infile.open(file_name, std::ios::in);
    if (!infile.is_open())
    {
        std::cerr << "Open failed!" << std::endl;
        exit(1);
    }

    bool flag = false;
    SockData *sock_data;
    std::streamoff offset = 0;
    std::string line, pid_t, state_t, sk_add_t, timestamp_t, pid_target;

    getline(infile, pid_target);
    offset = sizeof(pid_target);

    while (getline(infile, line))
    {
        split(line, pid_t, state_t, sk_add_t, timestamp_t);
        if (!flag)
        {
            if (!pid_target.compare(pid_t))
            {
                flag = true;
                sock_data = new SockData(sk_add_t);
                infile.seekg(offset, std::ios::beg); // return to 2nd line
            }
        }
        else
        {
            if (!sock_data->get_sk_add().compare(sk_add_t))
            {
                StateChangeInfoMsItem t;
                t.pid = std::stoi(pid_t);
                t.state = state_2_num(state_t);
                t.timestamp = std::stoll(timestamp_t, NULL, 10);
                sock_data->state_change_info.push_back(t);
            }
        }
        clear_strings(line, pid_t, state_t, sk_add_t, timestamp_t);
    }
    return sock_data;
}

void deal_data(SockData *sock_data)
{
    for (std::vector<StateChangeInfoMsItem>::iterator it = sock_data->state_change_info.begin(); it != sock_data->state_change_info.end(); it++)
    {
        sock_data->state_count[it->state].count++;
        sock_data->timestamp_record.push_back(it->timestamp);
    }
}

void cmd_get_sk_add(std::string &sk_add, const char *dest_ip)
{
    char cmd_1[] = "stap-4.8 /home/test/Transport-Layer-Sender-Visualization/script/systemtap/get_sock_add.stp ";
    strcat(cmd_1, dest_ip);
    const char *cmd = cmd_1;
    FILE *fp = popen(cmd, "r");
    assert(fp != NULL);

    char buf[100];
    // ffff926719db88c0        47.94.104.34
    // 二者之间是 \t 不是空格!!!
    if (fgets(buf, 100, fp) != NULL)
    {
        char token[20] = "";
        for (int i = 0; i < 100 && buf[i] != ' ' && buf[i] != '\t'; i++)
            token[i] = buf[i];
        sk_add = token;
    }
    std::cout << sk_add << std::endl;
    pclose(fp);
}

void cmd_get_state_info(SockData *sock_data)
{
    DB *db = new DB{"101.43.161.79", "zq", "zaq.1234"};
    const char *cmd = "stap-4.8 /home/test/Transport-Layer-Sender-Visualization/script/systemtap/get_state_info.stp";
    FILE *fp = popen(cmd, "r");
    assert(fp != NULL);

    char buf[100];
    std::string line, pid_t, state_t, sk_add_t, timestamp_t;
    while (fgets(buf, 100, fp) != NULL)
    {
        line = buf;
        // std::cout << line;

        split(line, pid_t, state_t, sk_add_t, timestamp_t);
        sk_add_t = sk_add_t.substr(5); // remove prefix "sk=0x"
        // std::cout << sock_data->get_sk_add() << " " << sk_add_t << std::endl;
        if (!sock_data->get_sk_add().compare(sk_add_t))
        {
            StateChangeInfoMsItem t;
            t.pid = std::stoi(pid_t);
            t.state = state_2_num(state_t);
            t.timestamp = std::stoll(timestamp_t, NULL, 10);
            sock_data->state_change_info.push_back(t);
        }
        clear_strings(line, pid_t, state_t, sk_add_t, timestamp_t);

        if (sock_data->state_change_info.size() >= 10)
        {
            db->send_change_info_ms(sock_data, "cubic");
            sock_data->state_change_info.clear();
        }
    }
    // for (std::vector<StateChangeInfoMsItem>::iterator it = sock_data->state_change_info.begin(); it != sock_data->state_change_info.end(); it++)
    // {
    //     std::cout << it->pid << " " << it->state << " " << it->timestamp << std::endl;
    // }
}

void cmd_get_state_info(SockData *sock_data, const char *test_info)
{
    DB *db = new DB{"101.43.161.79", "zq", "zaq.1234"};
    const char *cmd = "stap-4.8 /home/test/Transport-Layer-Sender-Visualization/script/systemtap/get_state_info.stp";
    FILE *fp = popen(cmd, "r");
    assert(fp != NULL);

    char buf[100];
    std::string line, pid_t, state_t, sk_add_t, timestamp_t;
    while (fgets(buf, 100, fp) != NULL)
    {
        line = buf;
        // std::cout << line;

        split(line, pid_t, state_t, sk_add_t, timestamp_t);
        sk_add_t = sk_add_t.substr(5); // remove prefix "sk=0x"

        if (!sock_data->get_sk_add().compare(sk_add_t))
        {
            StateChangeInfoMsItem t;
            t.pid = std::stoi(pid_t);
            t.state = state_2_num(state_t);
            t.timestamp = std::stoll(timestamp_t, NULL, 10);
            sock_data->state_change_info.push_back(t);
        }
        clear_strings(line, pid_t, state_t, sk_add_t, timestamp_t);

        if (sock_data->state_change_info.size() >= 10)
        {
            db->send_change_info_ms(sock_data, "cubic", test_info);
            sock_data->state_change_info.clear();
        }
    }
    // for (std::vector<StateChangeInfoMsItem>::iterator it = sock_data->state_change_info.begin(); it != sock_data->state_change_info.end(); it++)
    // {
    //     std::cout << it->pid << " " << it->state << " " << it->timestamp << std::endl;
    // }
}