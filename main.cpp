#include "youtube_wrapper.h"
#include "youtube_model.h"

//===================================================================================================================//

#include <QtCore>
#include <QtWidgets>

//===================================================================================================================//

int
main(int argc, char* argv[])
{
    QApplication app{ argc, argv };
    //QCommandLineParser parser{};
    //const QCommandLineOption creds_file{
    //    QStringList() << "j" << "json", "Specifies path to .json file with app credentials", "json"
    //};
    //
    //parser.addOptions({ creds_file });
    //parser.process(app);
    //if (parser.isSet(creds_file))
    YoutubeModel model{}; // { parser.value(creds_file) };

    const QRect avail_sz = QApplication::desktop()->availableGeometry(&model);
    model.resize(avail_sz.width() / 4, avail_sz.height() / 4);
    model.move((avail_sz.width() - model.width()) / 2,
               (avail_sz.height() - model.height()) / 2);

    model.show();
    return app.exec();
}

//===================================================================================================================//
