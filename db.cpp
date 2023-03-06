#include "db.h"

DB::DB()
{
    strcpy(host, "localhost");
    strcpy(user, "root");
    strcpy(passwd, "zaq.1234");
    port = 3306;
    if (mysql_library_init(0, NULL, NULL))
    {
        fprintf(stderr, "could not initialize MySQL client library\n");
        exit(1);
    }
    mysql = mysql_init(NULL);
    if (mysql == NULL)
    {
        fprintf(stderr, "could not initialize mysql\n");
        exit(1);
    }
}

DB::DB(const char *h, const char *u, const char *p)
{
    strcpy(host, h);
    strcpy(user, u);
    strcpy(passwd, p);
    port = 3306;
    if (mysql_library_init(0, NULL, NULL))
    {
        fprintf(stderr, "could not initialize MySQL client library\n");
        exit(1);
    }
    mysql = mysql_init(NULL);
    if (mysql == NULL)
    {
        fprintf(stderr, "could not initialize mysql\n");
        exit(1);
    }
}

DB::~DB()
{
    mysql_close(mysql);
    mysql_library_end();
}

int DB::connect_db(const char *d)
{
    strcpy(db, d);
    mysql = mysql_real_connect(mysql, host, user, passwd, db, port, NULL, 0);
    if (mysql == NULL)
    {
        fprintf(stderr, "disconnect\n");
        return 1;
    }
    else
    {
        return 0;
    }
}

int DB::send_count_info(SockDate *sock_data, const char *cc_info)
{
    const char *d = "count_info";
    std::string table_name = strcmp(cc_info, "cubic") == 0
                                 ? "cubic_count_info"
                                 : "bbr_count_info";
    if (!connect_db(d)) // connect succeed
    {
        int query_res = -1;

        std::string CREATE_STATEMENT =
            "CREATE TABLE if not exists " + table_name + " (begin_time VARCHAR(30) NOT NULL PRIMARY KEY, Open INT, Disorder INT, CWR INT, Recovery INT, Loss INT, read_time DATETIME(3))";
        query_res = mysql_query(mysql, CREATE_STATEMENT.c_str());
        if (query_res)
        {
            std::cerr << mysql_error(mysql) << std::endl;
            return 1;
        }

        std::string INSERT_STATEMENT =
            "INSERT INTO " + table_name + " (begin_time, Open, Disorder, CWR, Recovery, Loss, read_time) VALUES (" + std::to_string(sock_data->timestamp_record[0]) + ", ";
        for (int i = 0; i < 5; i++)
            INSERT_STATEMENT += (std::to_string(sock_data->state_count[i].count) + ", ");
        INSERT_STATEMENT += (convert_timestamp(sock_data->timestamp_record[0]) + ")");
        query_res = mysql_query(mysql, INSERT_STATEMENT.c_str());
        if (query_res)
        {
            std::cerr << mysql_error(mysql) << std::endl;
            return 1;
        }

        return 0;
    }
    return 1; // fail
}

int DB::send_change_info_ms(SockDate *sock_data, const char *cc_info, const char *test_info)
{
    const char *d = "change_info";
    std::string table_name = strcmp(cc_info, "cubic") == 0
                                 ? "cubic_change_info"
                                 : "bbr_change_info";
    if (!connect_db(d))
    {
        int query_res = -1;

        std::string CREATE_STATEMENT =
            "CREATE TABLE if not exists " + table_name + " (begin_time VARCHAR(30) NOT NULL, pid INT, state INT NOT NULL, read_time DATETIME(3), test_info VARCHAR(50), PRIMARY KEY (begin_time, state))";
        query_res = mysql_query(mysql, CREATE_STATEMENT.c_str());
        if (query_res)
        {
            std::cerr << mysql_error(mysql) << std::endl;
            return 1;
        }

        for (std::vector<StateChangeInfoMsItem>::iterator it = sock_data->state_change_info.begin(); it != sock_data->state_change_info.end(); it++)
        {
            std::string INSERT_STATEMENT =
                "INSERT INTO " + table_name + " (begin_time, pid, state, read_time, test_info) VALUES (" + std::to_string(it->timestamp) + ", " + std::to_string(it->pid) + ", " + std::to_string(it->state) + ", " + convert_timestamp(it->timestamp) + ", " + test_info + ")";
            query_res = mysql_query(mysql, INSERT_STATEMENT.c_str());
            if (query_res)
            {
                std::cerr << mysql_error(mysql) << std::endl;
                return 1;
            }
        }
        return 0;
    }
    return 1;
}

int DB::send_transfer_info_min(SockDate *sock_data, const char *cc_info)
{
    // TODO
}