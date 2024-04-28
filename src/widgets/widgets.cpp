#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QProcess>

#include "mainWindow.h"
#include "widgets.h"

int _showApp(int argc, char **argv)
{
    QApplication app(argc, argv);

    QSettings settings(app.applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    if(settings.value("Locale", -1) == -1)  {
        settings.setValue("Locale", "en_US");
    }

    QString lang = settings.value("Locale", "en_US").toString();
    QString translationFile = app.applicationDirPath() + "/ts/" + lang + ".qm";

    QTranslator translator;
    if(translator.load(translationFile)) {
        app.installTranslator(&translator);
        qDebug() << "Language" << lang;
    } else {
        qDebug() << "No translation file:" << translationFile;
    }

    MainWindow window;
    window.showMaximized();

    return app.exec();
}

int showApp(int argc, char **argv)
{
    int exitCode = 0;

    exitCode = _showApp(argc, argv);
    if(exitCode == MainWindow::EXIT_CODE_REBOOT) {
        QStringList args;
        for(size_t i = 1; i <= argc; ++i) {
            args.push_back(QString::fromUtf8(argv[i]));
        }
        QProcess::startDetached(argv[0], args);
    }

    return exitCode;
}
