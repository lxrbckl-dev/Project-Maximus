#pragma once

#include "i_data_source.h"
#include <QTimer>

class SyntheticDataSource : public IDataSource {
    Q_OBJECT

public:
    explicit SyntheticDataSource(QObject *parent = nullptr);

public slots:
    void start() override;
    void stop()  override;

private slots:
    void tick();

private:
    // ---------- helpers ----------
    double lerp(double a, double b, double t) const { return a + (b - a) * t; }
    double clamp(double v, double lo, double hi) const {
        return v < lo ? lo : (v > hi ? hi : v);
    }

    // ---------- infrastructure ----------
    QTimer  m_timer;
    qint64  m_tickCount   = 0;  // total ticks since start

    // ---------- RPM drive event state ----------
    // States: 0 = idle, 1 = accelerating, 2 = holding, 3 = decelerating
    int     m_driveState      = 0;
    qint64  m_driveStateEnd   = 0;   // tick at which state expires
    double  m_targetRpm       = 800.0;
    double  m_driveBaseRpm    = 800.0; // RPM at event start

    // ---------- smoothed outputs ----------
    double  m_smoothRpm       = 800.0;
    double  m_smoothSpeed     = 0.0;
    double  m_smoothThrottle  = 0.0;

    // ---------- coolant warm-up ----------
    double  m_coolantTemp     = 70.0;

    // ---------- fuel ----------
    double  m_fuelLevel       = 1.0;

    // Tick intervals (at 50 Hz = 20 ms/tick)
    static constexpr int kHz             = 50;
    // Warm-up to 195°F over 3 min
    static constexpr int kWarmupTicks    = 3 * 60 * kHz; // 9000

    // Idle RPM and jitter
    static constexpr double kIdleRpm     = 800.0;
    static constexpr double kIdleJitter  = 50.0;

    // Fuel drain: ~10% per hour at idle
    // rate per tick = 0.10 / (3600 * kHz)
    static constexpr double kFuelDrain   = 0.10 / (3600.0 * kHz);
};
