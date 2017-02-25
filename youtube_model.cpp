#include "youtube_model.h"

//===================================================================================================================//

#include <QtWidgets>
#include <QtNetwork>
#include <QUrl>

//===================================================================================================================//

ProgressDialog::ProgressDialog(const QString& url, QWidget* parent)
    : QProgressDialog(parent)
{
    setWindowTitle(tr("Upload Progress"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setLabelText(tr("Uploading %1.").arg(url));
    setMinimum(0);
    setValue(0);
    setMinimumDuration(0);
}

//===================================================================================================================//

void ProgressDialog::networkReplyProgress(qint64 bytesRead, qint64 totalBytes)
{
    setMaximum(totalBytes);
    setValue(bytesRead);
}

//===================================================================================================================//

YoutubeModel::YoutubeModel(QWidget* parent)
    : QDialog{ parent }
    , youtube{ QString{} }
    , status_label{ new QLabel{ tr("Please wait for authorization") } }
    , file_path{}
    , choose_file_button{ new QPushButton{ tr("&Choose video to upload") } }
    , upload_button{ new QPushButton{ tr("&Upload") } }
    , reply{ nullptr }
    , link_line( new QLineEdit{} )
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Youtube uploader"));

    this->youtube.grant();
    connect(&this->youtube, &YoutubeWrapper::authenticated, this, &YoutubeModel::authorized);

    auto main_layout{ new QVBoxLayout{ this } };

    status_label->setWordWrap(true);
    main_layout->addWidget(status_label);
    choose_file_button->setEnabled(false);
    upload_button->setEnabled(false);
    connect(upload_button, &QAbstractButton::clicked, this, &YoutubeModel::uploadVideo);
    connect(choose_file_button , &QAbstractButton::clicked, this, &YoutubeModel::chooseVideo);
    auto button_box{ new QDialogButtonBox{} };
    button_box->addButton(choose_file_button, QDialogButtonBox::ActionRole);
    button_box->addButton(upload_button, QDialogButtonBox::ActionRole);
    main_layout->addWidget(button_box);
    this->setAcceptDrops(true);
}

//===================================================================================================================//

void
YoutubeModel::uploadVideo(void)
{
    this->reply = this->youtube.uploadVideo(QString{ this->file_path });
    connect(this->reply, &QNetworkReply::finished, this, &YoutubeModel::uploadFinished);

    ProgressDialog *progressDialog = new ProgressDialog(this->file_path, this);
    progressDialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(progressDialog, &QProgressDialog::canceled, this, &YoutubeModel::cancelUpload);
    connect(reply, &QNetworkReply::downloadProgress, progressDialog, &ProgressDialog::networkReplyProgress);
    connect(reply, &QNetworkReply::finished, progressDialog, &ProgressDialog::hide);
    progressDialog->show();

    status_label->setText(tr("Uploading %1...").arg(this->file_path));
}

//===================================================================================================================//

void
YoutubeModel::authorized(void)
{
    this->choose_file_button->setEnabled(true);
    this->choose_file_button->setDefault(true);
}

//===================================================================================================================//

void
YoutubeModel::chooseVideo(void)
{
    const QString filters{
        "Youtube supported types(*.mov *.mpeg4 *.mp4 *.avi *.wmv *.mpegps *.flv *.3gpp "
        "*.webm *.MOV *.MPEG4 *.MP4 *.AVI *.WMV *.MPEGPS *.FLV *.3GPP *.WEBM);;All types(*)"
    };

    this->file_path = QFileDialog::getOpenFileName(this, tr("Choose Video"), "~", filters);
    if (this->file_path.length())
    {
        this->upload_button->setEnabled(true);
        this->upload_button->setDefault(true);
    }
}

//===================================================================================================================//

void
YoutubeModel::cancelUpload(void)
{
    status_label->setText(tr("Download canceled."));
    reply->abort();
    this->enableControls();
}

//===================================================================================================================//

void
YoutubeModel::uploadFinished(void)
{
    // TODO: display youtube URL
    qInfo() << "Upload finished: " << static_cast<int>(reply->error());
    if (reply->error() == QNetworkReply::NoError)
    {
        qInfo() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString().toLatin1();
        if (reply->open(QIODevice::ReadOnly))
        {
            qInfo() << QString(reply->readAll()).toLatin1();
        }
        else
        {
            qInfo() << "cant open reply";
        }
    }
    else
    {
        qInfo() << "not open";
    }
}

//===================================================================================================================//

void
YoutubeModel::enableControls(void)
{
    upload_button->setEnabled(true);
}

//===================================================================================================================//

void
YoutubeModel::disableControls(void)
{
    upload_button->setEnabled(false);
}

//===================================================================================================================//
