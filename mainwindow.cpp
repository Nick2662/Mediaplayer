#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <qdebug.h>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mediaPlayerInit();
    OnScanMedia();
    iconInit();

    connect(ui->ButLoad, SIGNAL(clicked()), this, SLOT(OnAddMedia()));
    connect(ui->ButMenu, SIGNAL(clicked()), this, SLOT(OnScanMedia()));
    connect(ui->ButPre, SIGNAL(clicked()), this, SLOT(OnPrevious()));
    connect(ui->ButPlay, SIGNAL(clicked()), this, SLOT(OnPlay()));
    //connect(ui->ButPause, SIGNAL(clicked()), this, SLOT(OnPause()));
    connect(ui->ButStop, SIGNAL(clicked()), this, SLOT(OnStop()));
    connect(ui->ButNext, SIGNAL(clicked()), this, SLOT(OnNext()));

    //listwidget clicked
    connect(ui->Playlist, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(on_Playlist_itemDoubleClicked(QListWidgetItem *item)));

    //slider change
    //way1:connect(ui->SliderTime, &QSlider::valueChanged, this, &MainWindow::GetDuration);
    connect(m_Player, SIGNAL(durationChanged(qint64)), this, SLOT(GetDuration()));
    connect(m_Player, SIGNAL(positionChanged(qint64)), this, SLOT(OnSlider()));
    connect(ui->SliderTime, SIGNAL(valueChanged(int)), this, SLOT(SliderChangde()));

    //autoplay next media
    connect(m_Player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(OnStateChange()));
    //connect(m_Playerlist, SIGNAL(currentIndexChanged(int)), this, SLOT(OnStateChange())); //currentIndexChanged()&currentMediaChanged() emit when media_in_playlist changed
    connect(m_Player, &QMediaPlayer::stateChanged, [](QMediaPlayer::State newState){
        qDebug() << newState;
    });//test when mp3/mp4 stoped play the QMediaPlayer::State
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mediaPlayerInit()
{
    m_Player = new QMediaPlayer;
    m_Playerlist = new QMediaPlaylist;
    m_Player->setPlaylist(m_Playerlist);
    m_Playerlist->setPlaybackMode(QMediaPlaylist::Loop);

    //play video
    m_PlayerWidget = new QVideoWidget;
    m_Player->setVideoOutput(m_PlayerWidget);
    ui->VideoOutput->addWidget(m_PlayerWidget);

    //Database
    db = new DataBase;
    db->connectDatabase();
}

void MainWindow::iconInit()
{
    QIcon icon_playButton(":/icon/play.png");
    ui->ButPlay->setIcon(icon_playButton);
}

void MainWindow::OnSetMediaFile()//choose media file
{
    //choose folder to import media resource
    QDir mediaResDir("C:/Qt/Reposition/MediaPlayer/MusicRes");
    qDebug()<<"appDir:"<<QCoreApplication::applicationDirPath()<<endl;

    if(mediaResDir.exists())
    {
        qDebug()<<"dir exist"<<endl;
        QStringList filter;
        filter<<"*.mp4"<<"*.mp3";
        QFileInfoList mediaInfos = mediaResDir.entryInfoList(filter, QDir::Files);
        qDebug()<<"infolist count:"<<mediaInfos.count()<<endl;
        for(int i = 0; i < mediaInfos.count(); i++)
        {
            MediaObjectInfo info;
            info.fileName = QString::fromUtf8(mediaInfos.at(i).fileName().toUtf8().data());
            qDebug()<<"filename:"<<info.fileName<<endl;
            info.filePath = QString::fromUtf8(mediaInfos.at(i).filePath().toUtf8().data());
            qDebug()<<"filepath:"<<info.filePath<<endl;

            /* 媒体列表添加歌曲 */
           if(OnSetPlaylist(info.filePath))
           {
               /* 添加到容器数组里储存 */
               mediaObjectInfo.append(info);
               /* 添加歌曲名字至列表 */
               ui->Playlist->addItem(info.fileName);
           } else
           {
               qDebug()<< m_Playerlist->errorString().toUtf8().data()  << endl;
               qDebug()<< " Error number:" << m_Playerlist->error() << endl;

           }
        }

    }else
    {
        qDebug()<<"mediaResDir:"<<mediaResDir.absolutePath();
    }


    /* //use dialog to import media resource
    //QFileDialog dialog(this);
    QFileDialog dialog;
    dialog.setDirectory("C:/Qt/Reposition/MediaPlayer/MusicRes");
    dialog.setFileMode(QFileDialog::AnyFile);
    //qDebug()<<"existDir1:"<<dialog.getExistingDirectory()<<"\n"<<" existDirUrl1: "<<dialog.getExistingDirectoryUrl();
    QStringList fileNames;


    if (dialog.exec())
        fileNames = dialog.selectedFiles();

    if(!fileNames.empty())
    {
        qDebug()<<"fileName:"<<fileNames[0];
        //qDebug()<<"existDir2:"<<dialog.getExistingDirectory()<<"\n existDirUrl2: "<<dialog.getExistingDirectoryUrl();
        OnSetPlaylist(fileNames);

    }*/

}

void MainWindow::OnAddMedia()
{
    //use dialog to import media resource
    QFileDialog dialog(this);
    //QFileDialog dialog;
    //dialog.setDirectory("C:/Qt/Reposition/MediaPlayer/MusicRes");
    dialog.setFileMode(QFileDialog::AnyFile);
    //qDebug()<<"existDir1:"<<dialog.getExistingDirectory()<<"\n"<<" existDirUrl1: "<<dialog.getExistingDirectoryUrl();
    QStringList fileNames;//是否可以使用非数组？？？？


    if (dialog.exec())
    {
        fileNames = dialog.selectedFiles();
        qDebug()<<"select file path:"<<fileNames[0];
    }


    if(!fileNames.empty())
    {
        QFileInfo file1(fileNames[0]);

        MediaObjectInfo info;
        info.fileName = file1.fileName();
        qDebug()<<"filename:"<<info.fileName<<endl;
        info.filePath = file1.absoluteFilePath();
        qDebug()<<"filepath:"<<info.filePath<<endl;

        /* 媒体列表添加歌曲 */
       if(OnSetPlaylist(info.filePath))
       {
           /* 添加到容器数组里储存 */
           mediaObjectInfo.append(info);
           /* 添加歌曲名字至列表 */
           ui->Playlist->addItem(info.fileName);
       } else
       {
           qDebug()<< m_Playerlist->errorString().toUtf8().data()  << endl;
           qDebug()<< " Error number:" << m_Playerlist->error() << endl;

       }
        //qDebug()<<"existDir2:"<<dialog.getExistingDirectory()<<"\n existDirUrl2: "<<dialog.getExistingDirectoryUrl();
        //OnSetPlaylist(fileNames);

    }
}

void MainWindow::OnScanMedia()
{
    //choose folder to import media resource
    QDir mediaResDir("C:/Qt/Reposition/MediaPlayer/MusicRes");
    qDebug()<<"appDir:"<<QCoreApplication::applicationDirPath()<<endl;

    if(mediaResDir.exists())
    {
        qDebug()<<"dir exist"<<endl;
        QStringList filter;
        filter<<"*.mp4"<<"*.mp3";
        QFileInfoList mediaInfos = mediaResDir.entryInfoList(filter, QDir::Files);
        qDebug()<<"infolist count:"<<mediaInfos.count()<<endl;
        for(int i = 0; i < mediaInfos.count(); i++)
        {
            MediaObjectInfo info;
            info.fileName = QString::fromUtf8(mediaInfos.at(i).fileName().toUtf8().data());
            qDebug()<<"filename:"<<info.fileName<<"\nseq:"<<m_Playerlist->currentIndex()<<endl;
            info.filePath = QString::fromUtf8(mediaInfos.at(i).filePath().toUtf8().data());
            qDebug()<<"filepath:"<<info.filePath<<endl;

            /* 媒体列表添加歌曲 */
           if(OnSetPlaylist(info.filePath))
           {
               /* 添加到容器数组里储存 */
               mediaObjectInfo.append(info);
               /* 添加歌曲名字至列表 */
               ui->Playlist->addItem(info.fileName);
           } else
           {
               qDebug()<< m_Playerlist->errorString().toUtf8().data()  << endl;
               qDebug()<< " Error number:" << m_Playerlist->error() << endl;

           }
        }

    }else
    {
        qDebug()<<"mediaResDir:"<<mediaResDir.absolutePath();
    }
    m_Playerlist->setCurrentIndex(0);
    //m_Player->play();
    qDebug()<<"before play state:"<<m_Player->state()<<"currentIndex:"<<m_Playerlist->currentIndex()<<endl;
}

bool MainWindow::OnSetPlaylist(QString filepath)
{
    return m_Playerlist->addMedia(QUrl::fromLocalFile(filepath));
    //ui->Playlist->addItems(m_Playerlist);
    /*
    if(m_Playerlist->isEmpty())
    {
        m_Playerlist->addMedia(QUrl::fromLocalFile(fileNames[0]));
        //m_Playerlist->setCurrentIndex(0);
    }

    m_Playerlist->addMedia(QUrl::fromLocalFile(fileNames[0]));
    qDebug()<<"currentIndex:"<<m_Playerlist->currentIndex();
    m_Playerlist->currentMedia();*/
}

void MainWindow::OnPrevious()
{
    m_Player->stop();
    int count = m_Playerlist->mediaCount();
    if(count == 0)//列表为空
    {
        return;
    }
    
    m_Playerlist->previous();
    m_Player->play();
}

void MainWindow::OnPlay()
{
    /*
    qDebug()<<"state:"<<m_Player->state()<<"index_beforePlay:"<<m_Playerlist->currentIndex();
    m_Player->play();
    index = m_Playerlist->currentIndex();
    qDebug()<<"state:"<<m_Player->state()<<"index_afterPlay:"<<m_Playerlist->currentIndex()<<"index:"<<index;
    */

    int state = m_Player->state();
    QIcon icon_playButton(":/icon/play.png");
    QIcon icon_pauseButton(":/icon/pause.png");
    switch (state) {
        case QMediaPlayer::StoppedState:
            /* 媒体播放 */
            m_Player->play();
            /*设置按钮图标*/
            ui->ButPlay->setFlat(true);
            ui->ButPlay->setIcon(icon_pauseButton);
            qDebug()<<"\nseq:"<<m_Playerlist->currentIndex()<<endl;

            break;

        case QMediaPlayer::PlayingState:
            /* 媒体暂停 */
            m_Player->pause();
            /*设置按钮图标*/
            //QIcon icon_cdButton(":/images/cd.png");
            ui->ButPlay->setIcon(icon_playButton);
            break;

        case QMediaPlayer::PausedState:
            m_Player->play();

            ui->ButPlay->setIcon(icon_pauseButton);
            qDebug()<<"\nseq:"<<m_Playerlist->currentIndex()<<endl;
            break;
    }
}

void MainWindow::OnPause()
{
    m_Player->pause();
}

void MainWindow::OnStop()
{
    m_Player->stop();

    QIcon icon_playButton(":/icon/play.png");
    ui->ButPlay->setIcon(icon_playButton);
}

void MainWindow::OnNext()
{
    m_Player->stop();
    int count = m_Playerlist->mediaCount();
    if(count == 0)//列表为空
    {
        return;
    }

    m_Playerlist->next();
    m_Player->play();
}

void MainWindow::OnSlider()
{
    ui->SliderTime->setValue(m_Player->position());
}

void MainWindow::GetDuration()
{
    int time = m_Player->duration();
    ui->SliderTime->setMaximum(time);
}

void MainWindow::SliderChangde()
{
    int pos = ui->SliderTime->value();
    m_Player->setPosition(pos);
}

void MainWindow::OnStateChange() //有问题，需要修改播放方式，更具菜单列表播放
{
    qDebug()<<"new state:"<<m_Player->state();
    if((m_Player->state() == QMediaPlayer::StoppedState)&&(playlistIndex != m_Playerlist->currentIndex()) )
    {
        //qDebug()<<"index1:"<<playlistIndex;
        playlistIndex = m_Playerlist->currentIndex();
        //qDebug()<<"index2:"<<playlistIndex;
        m_Player->play();
        //qDebug()<<"index3:"<<m_Playerlist->currentIndex();
    }

    /*  //this part is to confirm signal-playlist:currentindexchanged
    if(m_Player->state() == QMediaPlayer::StoppedState)
    {
        qDebug()<<"currentindex1:"<<m_Playerlist->currentIndex();
        //m_Player->stateChanged(QMediaPlayer::PlayingState); //cannot use this way
        m_Player->play();
    }

    qDebug()<<"currentindex2:"<<m_Playerlist->currentIndex();
    //m_Player->stateChanged(QMediaPlayer::PlayingState); //cannot use this way
    m_Player->play();*/


}

void MainWindow::on_Playlist_itemDoubleClicked(QListWidgetItem *item)
{
    m_Player->stop();
    m_Playerlist->setCurrentIndex(ui->Playlist->row(item));
    m_Player->play();
}
