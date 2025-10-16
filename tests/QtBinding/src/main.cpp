#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <MSG/QtWindow.hpp>
#include <QtBindingTestItem.hpp>

using namespace Msg;

int main(int argc, char** argv)
{
    QGuiApplication app(argc, argv);
    QtWindow::RegisterQMLType();
    QtBindingTestItem::RegisterQMLType();
    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/main.qml"));

    return app.exec();
}
