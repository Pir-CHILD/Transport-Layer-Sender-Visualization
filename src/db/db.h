#ifndef _DB_H
#define _DB_H
#include <mysql/mysql.h>
#include "../tool/tool.h"
#include "../sock_data/sock_data.h"

class DB
{
public:
    DB();
    DB(const char *h, const char *u, const char *p);
    ~DB();

    int connect_db(const char *d);
    int send_count_info(SockData *sock_data, const char *cc_info, const char *test_info);
    int send_change_info_ms(SockData *sock_data, const char *cc_info);
    int send_change_info_ms(SockData *sock_data, const char *cc_info, const char *test_info);

private:
    MYSQL *mysql;
    char *host; // need new some space before assignment
    char *user;
    char *passwd;
    char *db;
    int port;
};
#endif