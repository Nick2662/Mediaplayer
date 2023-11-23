#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtMultimedia>
#include <QMediaPlaylist>
#include <QVideoWidget>
#include <QDir>
#include <QListWidgetItem>

#include <mediaobject.h>

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
private slots:
   void on_Playlist_itemDoubleClicked(QListWidgetItem *item);
};

#endif // MAINWINDOW_H
