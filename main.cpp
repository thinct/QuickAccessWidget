#include "QuickAccessWidget/QuickAccessWidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QuickAccessWidget w;
    w.show();

    return a.exec();
}
