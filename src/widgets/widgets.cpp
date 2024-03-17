#include <QApplication>

#include "MainWindow.h"
#include "widgets.h"

MainWindow *window;

int showApp(int argc, char **argv)
{
    if(!window) {
        QApplication app(argc, argv);
        window = new MainWindow();
        window->show();
        return app.exec();
    } else {
        window->show();
        return 0;
    }
}
