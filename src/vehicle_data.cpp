#include "vehicle_data.h"

VehicleDataModel::VehicleDataModel(QObject *parent)
    : QObject(parent)
{
}

void VehicleDataModel::updateFromFrame(const VehicleFrame &frame)
{
    if (m_engineRpm != frame.engineRpm) {
        m_engineRpm = frame.engineRpm;
        emit engineRpmChanged();
    }
    if (m_vehicleSpeedMph != frame.vehicleSpeedMph) {
        m_vehicleSpeedMph = frame.vehicleSpeedMph;
        emit vehicleSpeedMphChanged();
    }
    if (m_coolantTempF != frame.coolantTempF) {
        m_coolantTempF = frame.coolantTempF;
        emit coolantTempFChanged();
    }
    if (m_throttlePosition != frame.throttlePosition) {
        m_throttlePosition = frame.throttlePosition;
        emit throttlePositionChanged();
    }
    if (m_fuelLevel != frame.fuelLevel) {
        m_fuelLevel = frame.fuelLevel;
        emit fuelLevelChanged();
    }
}
