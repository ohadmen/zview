#include <QtWidgets/QApplication>
#include "QtCore/QStringList"
#include "zview/common/params.h"
#include "zview/gui/main_window.h"
QStringList getArgs(int argc, char **argv)
{
    QStringList list;
    for (int i = 1; i != argc; ++i)
        list.push_back(argv[i]);
    
    return list;
}

int main(int argc, char **argv)
{
    
    QApplication app(argc, argv);

    app.setApplicationName("Zview");
    app.setApplicationVersion(Params::VERSION.toQString());

    auto list = getArgs(argc,argv);
    
    MainWindow win;
    win.readFileList(list);
    win.show();
    return app.exec();
}