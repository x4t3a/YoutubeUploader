#include "youtube_wrapper.h"

//===================================================================================================================//

#include <QtGui>
#include <QtCore>
#include <QtNetworkAuth>

//===================================================================================================================//

namespace {
    // List of scopes, separated by spaces. See: https://developers.google.com/identity/protocols/googlescopes
    // For example, to upload videos & manage your youtube account you would need:
    //     "https://www.googleapis.com/auth/youtube.upload https://www.googleapis.com/auth/youtube"
    const QString scope{ "https://www.googleapis.com/auth/youtube.upload https://www.googleapis.com/auth/youtube" };

    // https://developers.google.com/youtube/v3/guides/auth/devices
    const QUrl auth_url{ "https://accounts.google.com/o/oauth2/device/code" };
    const QUrl token_url{ "https://www.googleapis.com/oauth2/v4/token" };

    // https://developers.google.com/youtube/v3/docs/videos/insert
    const QUrl youtube_videos_insert{ "https://www.googleapis.com/upload/youtube/v3/videos?part=snippet" };

    // For the Authorization header:
    const QString bearer_format = QStringLiteral("Bearer %1");
}

//===================================================================================================================//

YoutubeWrapper::YoutubeWrapper(QObject* parent)
    : QObject{ parent }
{
    auto reply_handler{ new QOAuthHttpServerReplyHandler(10011, this) };
    youtube.setReplyHandler(reply_handler);
    youtube.setAuthorizationUrl(auth_url);
    youtube.setAccessTokenUrl(token_url);
    youtube.setScope(scope);

    connect(&youtube, &QOAuth2AuthorizationCodeFlow::statusChanged,
            [=] (QAbstractOAuth::Status status)
            {
                if ((QAbstractOAuth::Status::Granted == status))
                { emit authenticated(); }
            });

    youtube.setModifyParametersFunction(
        [&] (QAbstractOAuth::Stage stage, QVariantMap* parameters)
        {
            if ((QAbstractOAuth::Stage::RequestingAuthorization == stage) && isPermanent())
            { parameters->insert("duration", "permanent"); }
        });

    connect(&youtube, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, &QDesktopServices::openUrl);
}

//===================================================================================================================//

YoutubeWrapper::YoutubeWrapper(const QString& auth_file, QObject* parent)
    : YoutubeWrapper{ parent }
{
    Q_UNUSED(auth_file);
    youtube.setClientIdentifier(QString{ tr("848879698134-0m0eu67k9jlnu5p9rss1jlla530ossrh.apps.googleusercontent.com") });
    youtube.setAuthorizationUrl(QString{ tr("https://accounts.google.com/o/oauth2/auth") });
    youtube.setAccessTokenUrl(QString{ tr("https://accounts.google.com/o/oauth2/token") });
    youtube.setClientIdentifierSharedKey(QString{ tr("hYYsySJ3fqryHfZK4BOYjkdj") });
}

//===================================================================================================================//

QNetworkReply*
YoutubeWrapper::uploadVideo(QString video_path)
{
    QNetworkRequest req{ youtube_videos_insert };
    req.setRawHeader("Authorization", bearer_format.arg(youtube.token()).toUtf8());

    auto multi_part = new QHttpMultiPart{ QHttpMultiPart::MixedType };
    auto video_part = new QHttpPart{};

    auto video = new QFile{ video_path };
    if (!video->open(QIODevice::ReadOnly))
    {
        qDebug() << __FUNCTION__ << " wasn't able to open video file\n";
        return nullptr;
    }

    video_part->setHeader(QNetworkRequest::ContentTypeHeader, QVariant("video/*"));
    video_part->setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("Slug"));
    video_part->setBodyDevice(video);
    multi_part->append(*video_part);

    return youtube.networkAccessManager()->post(req, multi_part);
}

//===================================================================================================================//

bool
YoutubeWrapper::isPermanent(void) const
{
    return permanent;
}

//===================================================================================================================//

void
YoutubeWrapper::setPermanent(bool value)
{
    permanent = value;
}

//===================================================================================================================//

void
YoutubeWrapper::grant()
{
    youtube.grant();
}

//===================================================================================================================//
