#pragma once

#include <QObject>
#include "i_data_source.h"

class VehicleDataModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(double engineRpm        READ engineRpm        NOTIFY engineRpmChanged)
    Q_PROPERTY(double vehicleSpeedMph  READ vehicleSpeedMph  NOTIFY vehicleSpeedMphChanged)
    Q_PROPERTY(double coolantTempF     READ coolantTempF     NOTIFY coolantTempFChanged)
    Q_PROPERTY(double throttlePosition READ throttlePosition NOTIFY throttlePositionChanged)
    Q_PROPERTY(double fuelLevel        READ fuelLevel        NOTIFY fuelLevelChanged)

public:
    explicit VehicleDataModel(QObject *parent = nullptr);

    double engineRpm()        const { return m_engineRpm; }
    double vehicleSpeedMph()  const { return m_vehicleSpeedMph; }
    double coolantTempF()     const { return m_coolantTempF; }
    double throttlePosition() const { return m_throttlePosition; }
    double fuelLevel()        const { return m_fuelLevel; }

public slots:
    void updateFromFrame(const VehicleFrame &frame);

signals:
    void engineRpmChanged();
    void vehicleSpeedMphChanged();
    void coolantTempFChanged();
    void throttlePositionChanged();
    void fuelLevelChanged();

private:
    double m_engineRpm        = 800.0;
    double m_vehicleSpeedMph  = 0.0;
    double m_coolantTempF     = 70.0;
    double m_throttlePosition = 0.0;
    double m_fuelLevel        = 1.0;
};
