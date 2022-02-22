#include <qapplication.h>
#include <qlocale.h>

#include "Paint.h"


int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationDomain("https://github.com/software-made-easy/Paint++");
    a.setApplicationVersion(APP_VERSION);
    a.setWindowIcon(QIcon(":/icons/Icon"));

    MainWindow w;
    w.show();
    return a.exec();
}
