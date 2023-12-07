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

    connect(manager,SIGNAL(finished(QNetworkReply *)),this,SLOT(databack(QNetworkReply *)));
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

    //network
    manager = new QNetworkAccessManager;
    QString version = QSslSocket::sslLibraryBuildVersionString();//"OpenSSL 1.1.1d  10 Sep 2019"
    bool support = QSslSocket::supportsSsl();

    qDebug() << manager->supportedSchemes()<<"version:"<<version<<"support:"<<support<<endl;
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

            /* ý���б����Ӹ��� */
           if(OnSetPlaylist(info.filePath))
           {
               /* ���ӵ����������ﴢ�� */
               mediaObjectInfo.append(info);
               /* ���Ӹ����������б� */
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
    QStringList fileNames;//�Ƿ�����ʹ�÷����飿������


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

        /* ý���б����Ӹ��� */
       if(OnSetPlaylist(info.filePath))
       {
           /* ���ӵ����������ﴢ�� */
           mediaObjectInfo.append(info);
           /* ���Ӹ����������б� */
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

            /* ý���б����Ӹ��� */
           if(OnSetPlaylist(info.filePath))
           {
               /* ���ӵ����������ﴢ�� */
               mediaObjectInfo.append(info);
               /* ���Ӹ����������б� */
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
    if(count == 0)//�б�Ϊ��
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
            /* ý�岥�� */
            m_Player->play();
            /*���ð�ťͼ��*/
            ui->ButPlay->setFlat(true);
            ui->ButPlay->setIcon(icon_pauseButton);
            qDebug()<<"\nseq:"<<m_Playerlist->currentIndex()<<endl;

            break;

        case QMediaPlayer::PlayingState:
            /* ý����ͣ */
            m_Player->pause();
            /*���ð�ťͼ��*/
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
    if(count == 0)//�б�Ϊ��
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

void MainWindow::OnStateChange() //�����⣬��Ҫ�޸Ĳ��ŷ�ʽ�����߲˵��б�����
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

void MainWindow::databack(QNetworkReply *reply)
{
    searchInfo=reply->readAll();
    QJsonParseError err;               //错误信息对象
    QJsonDocument json_recv = QJsonDocument::fromJson(searchInfo,&err);//将json文本转换为 json 文件对象
    if(err.error != QJsonParseError::NoError)             //判断是否符合语法
    {
        qDebug() <<"搜索歌曲Json获取格式错误"<< err.errorString();
        return;
    }
    QJsonObject totalObject = json_recv.object();
    QStringList keys = totalObject.keys();    // 列出json里所有的key
    if(keys.contains("result"))                 //如果有结果
    {       //在 json 文本中 {}花括号里面是QJsonObject对象, []方括号里面是QJsonArray

        QJsonObject resultObject = totalObject["result"].toObject();     //就将带 result 的内容提取后转换为对象
        QStringList resultKeys = resultObject.keys();      //保存所有key
        if(resultKeys.contains("songs"))                    //如果 key 为songs ,代表找到了歌曲
        {
            QJsonArray array = resultObject["songs"].toArray();
            for(auto i : array)                   //开始获取歌曲中的信息
            {
                QJsonObject object = i.toObject();
                musicId = object["id"].toInt();                         // 音乐id
                musicDuration = object["duration"].toInt();             // 音乐长度
                musicName = object["name"].toString();                  // 音乐名
                mvId = object["mvid"].toInt();                          // mvid
                QStringList artistsKeys = object.keys();
                if(artistsKeys.contains("artists"))                //如果result中包含了 artists
                {
                    QJsonArray artistsArray = object["artists"].toArray();   //将 artist 的内容提取后保存
                    for(auto j : artistsArray)
                    {
                        QJsonObject object2 = j.toObject();
                        singerName = object2["name"].toString();         // 歌手名
                    }
                }
                if(artistsKeys.contains("album"))                //包含了专辑
                {
                    QJsonObject albumObjct = object["album"].toObject();
                    albumName = albumObjct["name"].toString();            // 专辑名
                }
                qDebug()<<"音乐ID："<<QString::number(musicId)<<"音乐名："<<musicName<<"歌手名："<<singerName
                       <<"总时长："<<QString::number(musicDuration)<<"专辑名："<<albumName<<endl;

                QString url;
                url=QString("https://music.163.com/song/media/outer/url?id=%0").arg(musicId);

                MediaObjectInfo info;
                info.fileName = musicName + "-" + singerName + ".mp3";
                qDebug()<<"filename:"<<info.fileName<<endl;
                info.filePath = url + ".mp3";
                qDebug()<<"filepath:"<<info.filePath<<endl;

                /* ý���б����Ӹ��� */
               if(m_Playerlist->addMedia(QUrl(url)))
               {
                   /* ���ӵ����������ﴢ�� */
                   mediaObjectInfo.append(info);
                   /* ���Ӹ����������б� */
                   ui->Playlist->addItem(info.fileName);
               } else
               {
                   qDebug()<< m_Playerlist->errorString().toUtf8().data()  << endl;
                   qDebug()<< " Error number:" << m_Playerlist->error() << endl;

               }

               DownloadTool* dT;
               dT = new DownloadTool(info.filePath, QApplication::applicationDirPath() + "/download", info.fileName);
               dT->startDownload();

                /*
                     ui->music_m->appendPlainText("音乐ID："+QString::number(musicId));
                     ui->music_m->appendPlainText("音乐名："+musicName);
                     ui->music_m->appendPlainText("歌手名："+singerName);
                     ui->music_m->appendPlainText("总时长："+QString::number(musicDuration));
                     ui->music_m->appendPlainText("专辑名："+albumName);*/
            }
        }
    }


    //playlist->addMedia(QUrl(url));                     //添加返回的音乐到播放列表中
    //ui->plainTextEdit->appendPlainText(musicName); //用于显示
}

void MainWindow::on_Playlist_itemDoubleClicked(QListWidgetItem *item)
{
    m_Player->stop();
    m_Playerlist->setCurrentIndex(ui->Playlist->row(item));
    m_Player->play();
}

void MainWindow::on_pushButton_search_clicked()
{
    QString str,s;
    s = ui->lineEdit->text();
    str = "http://music.163.com/api/search/get/web?csrf_token=hlpretag=&hlposttag=&s={"+s+"}&type=1&offset=0&total=true&limit=1";
    QNetworkRequest request;          //定义一个请求对象
    request.setUrl(str);      //将请求格式设置给请求对象
    manager->get(request);              //请求
}
