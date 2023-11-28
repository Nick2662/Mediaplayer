#ifndef DATABASE_H
#define DATABASE_H

#include <Main.h>
#include <QSqlDatabase>

#define HostName "127.0.0.1"
#define Port 3306
#define DBName "Test"
#define UserName "root"
#define password "root"



class DataBase
{
public:
    DataBase();
    ~ DataBase();
public:
    bool connectDatabase();

};

#endif // DATABASE_H
