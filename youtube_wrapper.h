#ifndef YOUTUBE_WRAPPER_H
#   define YOUTUBE_WRAPPER_H

//===================================================================================================================//

#   include <QtCore>
#   include <QtNetwork>
#   include <QOAuth2AuthorizationCodeFlow>

//===================================================================================================================//

class YoutubeWrapper : public QObject {
    Q_OBJECT

public:
    YoutubeWrapper(QObject* parent = nullptr);
    YoutubeWrapper(const QString& clientIdentifier, QObject* parent = nullptr);

    QNetworkReply* uploadVideo(QString video_path);

    bool isPermanent() const;
    void setPermanent(bool value);

public slots:
    void grant();

signals:
    void authenticated();

private:
    QOAuth2AuthorizationCodeFlow youtube;
    bool permanent = false;

}; // YoutubeWrapper

//===================================================================================================================//

#endif // YOUTUBE_WRAPPER_H
