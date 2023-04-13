#include "./sock_data/sock_data.h"
#include "./tool/tool.h"
// #include "db.h"

int main(int argc, char *argv[])
{
    // if (argc < 3)
    // {
    //     std::cerr << "Missing Parameters!" << std::endl;
    //     return 1;
    // }
    // else
    // {
    std::string sk_add = "";
    // const char *cc_info = argv[1]; // cubic or bbr
    // const char *file_name = argv[2]; // test.txt

    // SockData *sock_data = extract_data(file_name);
    // deal_data(sock_data);
    const char *dest_ip = argc == 2 ? argv[1] : "101.43.161.79";
    cmd_get_sk_add(sk_add, dest_ip);
    SockData *sock_data = new SockData(sk_add);
    cmd_get_state_info(sock_data);

    delete sock_data;

    // DB *db = new DB{"localhost", "root", "zaq.1234"};
    // db->send_count_info(sock_data, cc_info, file_name);
    // db->send_change_info_ms(sock_data, cc_info);
    // db->send_transfer_info_min(sock_data, cc_info, file_name);
    // }
    return 0;
}