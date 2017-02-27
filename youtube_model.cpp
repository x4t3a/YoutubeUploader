#include "youtube_model.h"

//===================================================================================================================//

#include <QtWidgets>
#include <QtNetwork>
#include <QUrl>
#include <QMessageBox>

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
    , youtube{ QString{} } // TODO: pass path to json
    , status_label{ new QLabel{ tr("Please wait for authorization") } }
    , file_path{}
    , choose_file_button{ new QPushButton{ tr("&Choose video to upload") } }
    , upload_button{ new QPushButton{ tr("&Upload") } }
    , progress_dialog{ nullptr }
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
    if (this->progress_dialog) { return; }

    this->reply = this->youtube.uploadVideo(QString{ this->file_path });
    this->progress_dialog = new ProgressDialog(this->file_path, this);
    connect(this->reply, &QNetworkReply::finished, this, &YoutubeModel::uploadFinished);

    //progress_dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(progress_dialog, &QProgressDialog::canceled, this, &YoutubeModel::cancelUpload);
    connect(reply, &QNetworkReply::uploadProgress, progress_dialog, &ProgressDialog::networkReplyProgress);
    //progress_dialog->show();

    status_label->setText(tr("Uploading %1...").arg(this->file_path));
}

//===================================================================================================================//

void
YoutubeModel::authorized(void)
{
    this->choose_file_button->setEnabled(true);
    this->choose_file_button->setDefault(true);
    this->status_label->setText(QString{ "Now you can choose a video to upload." });
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
        this->status_label->setText(QString{ "Press 'Upload' to proceed." });
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
    auto dialog{ new QDialog{} };
    auto layout{ new QVBoxLayout{} };
    auto line{ new QLineEdit{ QString{ "Something's gone wrong" } } };
    auto button = new QPushButton{ QString{ "Copy to clipboard" } };

    line->setReadOnly(true);
    layout->addWidget(line);

    if (reply->error() == QNetworkReply::NoError)
    {
        if (reply->open(QIODevice::ReadOnly))
        {
            auto obj = QJsonDocument::fromJson(reply->readAll()).object();
            auto id = obj.value("id");
            if (QJsonValue::Undefined != id)
            {
                line->setText(QString{ "youtube.com/watch?v=" } + id.toString());
                layout->addWidget(button);
                connect(button, &QPushButton::pressed, [=]() { line->selectAll(); line->copy(); });
            }
        }
    }

    delete this->progress_dialog; this->progress_dialog = nullptr;

    dialog->setLayout(layout);
    dialog->show();
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
