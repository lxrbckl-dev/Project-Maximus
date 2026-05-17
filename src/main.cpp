#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "vehicle_data.h"
#include "synthetic_data_source.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Data layer
    auto *model  = new VehicleDataModel(&app);
    auto *source = new SyntheticDataSource(&app);

    // Wire data source → model
    QObject::connect(source, &IDataSource::frameReceived,
                     model,  &VehicleDataModel::updateFromFrame);

    // QML engine
    QQmlApplicationEngine engine;

    // Expose the model as a context property so QML can bind to it directly
    engine.rootContext()->setContextProperty("vehicleData", model);

    const QUrl url(QStringLiteral("qrc:/Maximus/qml/Main.qml"));
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed,
        &app,    &QGuiApplication::quit,
        Qt::QueuedConnection
    );
    engine.load(url);

    if (engine.rootObjects().isEmpty())
        return -1;

    // Start the 50 Hz data feed
    source->start();

    return app.exec();
}
