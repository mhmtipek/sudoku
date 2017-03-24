#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>

#include "gamecontrolproxy.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QScopedPointer<QTranslator> translator;
    if (QLocale::system().language() == QLocale::Turkish) {
        translator.reset(new QTranslator);
        if (!translator->load(":/sudoku_tr.qm"))
            qDebug() << "Failed to load translation";
        else
            app.installTranslator(translator.data());
    }

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("GameControl", &GameControlProxy::instance());
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
