#include "./sock_data/sock_data.h"
#include "./tool/tool.h"
#include "./3rdparty/CLI11.hpp"
// #include "db.h"

int main(int argc, char *argv[])
{
    /* CLI init */
    CLI::App app;

    std::string server_ip = "101.43.161.79";
    app.add_option("--sIP", server_ip, "server IP");

    std::string info = "0";
    app.add_option("-i,--info", info, "test info");

    CLI11_PARSE(app, argc, argv);

    std::string sk_add = "";

    // SockData *sock_data = extract_data(file_name);
    // deal_data(sock_data);
    const char *dest_ip = server_ip.c_str();
    const char *test_info = info.c_str();
    cmd_get_sk_add(sk_add, dest_ip);
    SockData *sock_data = new SockData(sk_add);
    cmd_get_state_info(sock_data, test_info);

    delete sock_data;

    // DB *db = new DB{"localhost", "root", "zaq.1234"};
    // db->send_count_info(sock_data, cc_info, file_name);
    // db->send_change_info_ms(sock_data, cc_info);
    // db->send_transfer_info_min(sock_data, cc_info, file_name);
    // }
    return 0;
}