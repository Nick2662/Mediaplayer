#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtMultimedia>
#include <QMediaPlaylist>
#include <QVideoWidget>
#include <QDir>
#include <QListWidgetItem>

#include <Main.h>
#include <mediaobject.h>
#include <Database.h>
#include "DownloadTool.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
   Ui::MainWindow *ui;

   /*mediaInfo*/
   QVector<MediaObjectInfo> mediaObjectInfo;

   /*DataBase*/
   DataBase *db;

   /*mediaplayer, to play the meida*/
   QMediaPlayer *m_Player;
   /*mediaplaylist*/
   QMediaPlaylist *m_Playerlist;
   /*videoplayer widdget*/
   QVideoWidget *m_PlayerWidget;
   
   /*mediaplayer initialize*/
   void mediaPlayerInit();
   void iconInit();

public:
   int playlistIndex = -1;

public slots:
   void OnSetMediaFile(void);//choose media file
   void OnScanMedia();
   void OnAddMedia();
   bool OnSetPlaylist(QString fileNames);//add to playlist

   void OnPrevious(void);
   void OnPlay(void);//play the media
   void OnPause(void);
   void OnStop(void);
   void OnNext(void);

   void OnSlider(void);
   void GetDuration(void);
   void SliderChangde(void);

   void OnStateChange();

   void databack(QNetworkReply *reply);
private slots:
   void on_Playlist_itemDoubleClicked(QListWidgetItem *item);
   void on_pushButton_search_clicked();

private: //使用api
   QNetworkAccessManager* manager;
   QByteArray searchInfo;
   int musicId,musicDuration,mvId;
   QString musicName,singerName,albumName;

};

#endif // MAINWINDOW_H
