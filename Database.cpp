#include "Database.h"
#include <QMessageBox>
#include <QDebug>

DataBase::DataBase()
{

}

bool DataBase::connectDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(HostName);  //连接本地主机
    db.setPort(Port);
    db.setDatabaseName(DBName);
    db.setUserName(UserName);
    db.setPassword(password);
    bool ok = db.open();

    //输出可用数据库
    qDebug()<<"available drivers:";
    QStringList drivers = QSqlDatabase::drivers();
    foreach(QString driver, drivers)
        qDebug()<<driver;


    if (ok){
        //QMessageBox::information(this, "infor", "link success");
        qDebug()<<"coonnect ok!"<<endl;
        return 1;
    }
    else {
        qDebug()<<"coonnect fail!"<<endl;
        //QMessageBox::information(this, "infor", "link failed");
        //qDebug()<<"error open database because"<<db.lastError().text();
        return 0;
    }
}
