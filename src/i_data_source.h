#pragma once

#include <QObject>
#include <QtTypes>

struct VehicleFrame {
    double   engineRpm       = 0.0;
    double   vehicleSpeedMph = 0.0;
    double   coolantTempF    = 70.0;
    double   throttlePosition = 0.0; // 0.0–1.0
    double   fuelLevel        = 1.0; // 0.0–1.0
    qint64   timestampMs      = 0;
};
Q_DECLARE_METATYPE(VehicleFrame)

class IDataSource : public QObject {
    Q_OBJECT

public:
    explicit IDataSource(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~IDataSource() = default;

public slots:
    virtual void start() = 0;
    virtual void stop()  = 0;

signals:
    void frameReceived(const VehicleFrame &frame);
};
