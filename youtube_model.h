#ifndef YOUTUBE_MODEL_H
#   define YOUTUBE_MODEL_H

//===================================================================================================================//

#   include "youtube_wrapper.h"

//===================================================================================================================//

#   include <QProgressDialog>
#   include <QNetworkAccessManager>
#   include <QUrl>


//===================================================================================================================//

class QFile;
class QLabel;
class QLineEdit;
class QPushButton;
class QSslError;
class QAuthenticator;
class QNetworkReply;
class QCheckBox;

//===================================================================================================================//

class ProgressDialog : public QProgressDialog {
    Q_OBJECT

public:
    explicit ProgressDialog(const QString& url, QWidget* parent = Q_NULLPTR);

public slots:
    void networkReplyProgress(qint64 bytesRead, qint64 totalBytes);

};

//===================================================================================================================//

class YoutubeModel : public QDialog {
    Q_OBJECT

public:
    explicit YoutubeModel(QWidget* parent = nullptr);

protected:
    void dropEvent(QDropEvent*) { qInfo() << "drop it!"; }

private slots:
    void uploadVideo(void);
    void chooseVideo(void);
    void authorized(void);
    void uploadFinished(void);
    void cancelUpload(void);
    void enableControls(void);
    void disableControls(void);

private:
    YoutubeWrapper  youtube;
    QLabel*         status_label;
    QString         file_path;
    QPushButton*    choose_file_button;
    QPushButton*    upload_button;
    ProgressDialog* progress_dialog;
    QNetworkReply*  reply;
    QLineEdit*      link_line;

}; // YoutubeModel

//===================================================================================================================//

#endif // YOUTUBE_MODEL_H
