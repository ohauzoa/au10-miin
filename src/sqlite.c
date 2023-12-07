#include "sqlite.h"

#include "../sqlite/sqlite3.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void close_db(sqlite3* db)
{
    if (db)
    {
        sqlite3_close(db);
        db = NULL;
    }
}

sqlite3* open_db()
{
    sqlite3 *db = NULL;

    if (sqlite3_open("test.db", &db) != SQLITE_OK)
    {
        printf("Can't open database: %s\r\n", sqlite3_errmsg(db));
        if (db) close_db(db);
        return NULL;
    }

    return db;
}

int create_table(sqlite3* db)
{
    char *zErrMsg = 0;
    char *create_query = "CREATE TABLE FRUITS_KEEPER (FRUIT_TYPE TEXT PRIMARY KEY NOT NULL, PRICE TEXT NOT NULL, STOCKS TEXT NOT NULL);";

    if (!db) return -1;

    if (sqlite3_exec(db, create_query, NULL, NULL, &zErrMsg) != SQLITE_OK)
    {
        printf("[create_table] CREATE: %S\r\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }

    printf("[create_table] done \r\n");


    return 0;
}


int put_db(sqlite3* db, char *kinds, int price, int stocks)
{
    char *zErrMsg = 0;
    char query[128] = { 0, };

    if (!db || !kinds || !price || !stocks) return -1;

    snprintf(query, 128, "INSERT INTO FRUITS_KEEPER (FRUIT_TYPE,PRICE,STOCKS) VALUES ('%s', '%d', '%d');", kinds, price, stocks);

    if (sqlite3_exec(db, query, NULL, 0, &zErrMsg) != SQLITE_OK)
    {
        printf("[put_db] INSERT : %S\r\n", zErrMsg);
        sqlite3_free(zErrMsg);
    return -1;
    }

    return 0;
}

int update_db(sqlite3* db, char *kinds, int price, int stock)
{
    char *zErrMsg = 0;
    char query[128] = { 0, };

    if (!db || !kinds) return -1;

    snprintf(query, 128, "UPDATE FRUITS_KEEPER SET PRICE = '%d' WHERE FRUIT_TYPE ='%s';\0", price, kinds);

    if (sqlite3_exec(db, query, NULL, 0, &zErrMsg) != SQLITE_OK)
    {
        printf("[update_db] UPDATE : %S\r\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }

    snprintf(query, 128, "UPDATE FRUITS_KEEPER SET STOCKS = '%d' WHERE FRUIT_TYPE ='%s';\0", stock, kinds);

    if (sqlite3_exec(db, query, NULL, 0, &zErrMsg) != SQLITE_OK)
    {
        printf("[update_db] UPDATE : %S\r\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }

    return 0;
}

typedef struct _search_cb_data_t
{
    char kinds[32];
    int found;
    int price;
    int stock;
}search_cb_data_t;


static int search_cb(void *data, int argc, char **argv, char **azColName)
{
    search_cb_data_t *user_data = (search_cb_data_t*)data;

    if (user_data)
    {
        if (argc == 3 && strcmp(azColName[0], "FRUIT_TYPE") == 0
        && argv[0] && strcmp(argv[0], user_data->kinds) == 0
        && strcmp(azColName[1], "PRICE") == 0
        && strcmp(azColName[2], "STOCKS") == 0)
        {
            user_data->found = 1;
            if (argv[1]) user_data->price = atoi(argv[1]);
            if (argv[2]) user_data->stock = atoi(argv[2]);
        }
    }

    return 0;
}


int search_db(sqlite3* db, char *kinds, int *price, int *stock)
{
    char *zErrMsg = 0;
    search_cb_data_t user_data = { 0, };
    char query[128] = { 0, };

    if (!db || !kinds) return -1;

    memset(&user_data, 0, sizeof(search_cb_data_t));

    snprintf(user_data.kinds, 32, "%s", kinds);
    snprintf(query, 128, "SELECT * FROM FRUITS_KEEPER WHERE FRUIT_TYPE LIKE '%s';", kinds);

    if (sqlite3_exec(db, query, search_cb, (void*)&user_data, &zErrMsg) != SQLITE_OK)
    {
        // not found...
        printf("[search_db] SELECT : %S\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }

    if (user_data.found)
    {
        if (price) *price = user_data.price;
        if (stock) *stock = user_data.stock;
        return 0;
    }

    return -1;
}

int test_sqlite()
{
    int price = 0;
    int stock = 0;

    sqlite3 *db = open_db();

//    create_table(db);

    printf("put db\r\n");

    if (search_db(db, "apple", NULL, NULL) != 0) put_db(db, "apple", 1500, 100);
    if (search_db(db, "orange", NULL, NULL) != 0) put_db(db, "orange", 1000, 200);
    if (search_db(db, "watermelon", NULL, NULL) != 0) put_db(db, "watermelon", 9000, 80);
    if (search_db(db, "peach", NULL, NULL) != 0) put_db(db, "peach", 1200, 200);


    search_db(db, "apple", &price, &stock);
    printf("apple price %d stock %d\r\n", price, stock);
    search_db(db, "orange", &price, &stock);
    printf("orange price %d stock %d\r\n", price, stock);
    search_db(db, "watermelon", &price, &stock);
    printf("watermelon price %d stock %d\r\n", price, stock);
    search_db(db, "peach", &price, &stock);
    printf("peach price %d stock %d\r\n", price, stock);

    /*
    */
    printf("update db\r\n");

    update_db(db, "apple", 1500, 50);
    update_db(db, "orange", 900, 200);
    update_db(db, "watermelon", 9000, 100);
    update_db(db, "peach", 1200, 100);

    search_db(db, "apple", &price, &stock);
    printf("apple price %d stock %d\r\n", price, stock);
    search_db(db, "orange", &price, &stock);
    printf("orange price %d stock %d\r\n", price, stock);
    search_db(db, "watermelon", &price, &stock);
    printf("watermelon price %d stock %d\r\n", price, stock);
    search_db(db, "peach", &price, &stock);
    printf("peach price %d stock %d\r\n", price, stock);

    close_db(db);
}