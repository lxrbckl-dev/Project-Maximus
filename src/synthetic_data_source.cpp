#include "synthetic_data_source.h"

#include <QDateTime>
#include <QtMath>
#include <cstdlib>  // rand / srand
#include <ctime>

// ─── tiny deterministic PRNG (avoids seeding std::mt19937 in a header) ───────

static double randF(double lo, double hi)
{
    // Returns a uniform float in [lo, hi).
    // Not cryptographic quality — fine for simulation.
    static bool seeded = false;
    if (!seeded) { std::srand(static_cast<unsigned>(std::time(nullptr))); seeded = true; }
    double r = static_cast<double>(std::rand()) / (static_cast<double>(RAND_MAX) + 1.0);
    return lo + r * (hi - lo);
}

// ─────────────────────────────────────────────────────────────────────────────

SyntheticDataSource::SyntheticDataSource(QObject *parent)
    : IDataSource(parent)
{
    m_timer.setInterval(1000 / kHz); // 20 ms
    connect(&m_timer, &QTimer::timeout, this, &SyntheticDataSource::tick);
}

void SyntheticDataSource::start()
{
    m_timer.start();
}

void SyntheticDataSource::stop()
{
    m_timer.stop();
}

// ─── tick ────────────────────────────────────────────────────────────────────

void SyntheticDataSource::tick()
{
    ++m_tickCount;

    // ── Drive-event state machine ─────────────────────────────────────────

    if (m_tickCount >= m_driveStateEnd) {
        // Transition to next state
        switch (m_driveState) {
        case 0: // idle → begin acceleration
        {
            m_driveBaseRpm  = m_smoothRpm;
            m_targetRpm     = randF(2500.0, 6000.0);
            // Acceleration duration: proportional to delta RPM, ~1–3 s
            double delta    = m_targetRpm - m_driveBaseRpm;
            int accelTicks  = static_cast<int>(clamp(delta / 40.0, 50, 150)); // 1–3 s
            m_driveState    = 1;
            m_driveStateEnd = m_tickCount + accelTicks;
            break;
        }
        case 1: // accelerating → hold
        {
            m_driveBaseRpm  = m_smoothRpm;
            int holdTicks   = static_cast<int>(randF(50, 150)); // 1–3 s
            m_driveState    = 2;
            m_driveStateEnd = m_tickCount + holdTicks;
            break;
        }
        case 2: // holding → decelerate
        {
            m_driveBaseRpm  = m_smoothRpm;
            m_targetRpm     = kIdleRpm;
            double delta    = m_driveBaseRpm - kIdleRpm;
            int decelTicks  = static_cast<int>(clamp(delta / 35.0, 50, 200)); // 1–4 s
            m_driveState    = 3;
            m_driveStateEnd = m_tickCount + decelTicks;
            break;
        }
        case 3: // decelerated → idle pause
        {
            // Idle pause: 8–12 s
            int idleTicks   = static_cast<int>(randF(400, 600));
            m_driveState    = 0;
            m_driveStateEnd = m_tickCount + idleTicks;
            break;
        }
        default:
            m_driveState    = 0;
            m_driveStateEnd = m_tickCount + 400;
        }
    }

    // ── Compute instantaneous target RPM ─────────────────────────────────

    double instantRpm = m_smoothRpm;
    switch (m_driveState) {
    case 1: // accelerating
    {
        // Linear interpolation from driveBaseRpm to targetRpm over the state duration
        qint64 stateDuration = m_driveStateEnd - (m_tickCount - 1);
        double progress = 1.0 - static_cast<double>(m_driveStateEnd - m_tickCount)
                                / static_cast<double>(stateDuration > 0 ? stateDuration : 1);
        progress = clamp(progress, 0.0, 1.0);
        instantRpm = lerp(m_driveBaseRpm, m_targetRpm, progress);
        break;
    }
    case 2: // holding — small jitter around target
        instantRpm = m_targetRpm + randF(-30.0, 30.0);
        break;
    case 3: // decelerating
    {
        qint64 stateDuration = m_driveStateEnd - (m_tickCount - 1);
        double progress = 1.0 - static_cast<double>(m_driveStateEnd - m_tickCount)
                                / static_cast<double>(stateDuration > 0 ? stateDuration : 1);
        progress = clamp(progress, 0.0, 1.0);
        instantRpm = lerp(m_driveBaseRpm, kIdleRpm, progress);
        break;
    }
    default: // idle
        instantRpm = kIdleRpm + randF(-kIdleJitter, kIdleJitter);
    }

    // ── Smooth RPM (LP filter, alpha = 0.15) ─────────────────────────────
    constexpr double kRpmAlpha = 0.15;
    m_smoothRpm = lerp(m_smoothRpm, instantRpm, kRpmAlpha);
    m_smoothRpm = clamp(m_smoothRpm, 0.0, 8000.0);

    // ── Speed: correlated with RPM, smoothed independently ───────────────
    double targetSpeed = clamp((m_smoothRpm - 800.0) / 70.0, 0.0, 180.0);
    constexpr double kSpeedAlpha = 0.05;
    m_smoothSpeed = lerp(m_smoothSpeed, targetSpeed, kSpeedAlpha);
    m_smoothSpeed = clamp(m_smoothSpeed, 0.0, 180.0);

    // ── Throttle: derived from RPM derivative ─────────────────────────────
    // Approximate RPM derivative as (instantRpm - smoothRpm) — positive when climbing.
    double rpmDelta = instantRpm - m_smoothRpm;
    double targetThrottle;
    if (rpmDelta > 100.0) {
        // Climbing — high throttle
        targetThrottle = randF(0.60, 1.00);
    } else if (rpmDelta < -100.0) {
        // Decelerating — near zero throttle
        targetThrottle = randF(0.00, 0.05);
    } else {
        // Steady cruise — light load
        targetThrottle = randF(0.15, 0.30);
    }
    constexpr double kThrottleAlpha = 0.12;
    m_smoothThrottle = lerp(m_smoothThrottle, targetThrottle, kThrottleAlpha);
    m_smoothThrottle = clamp(m_smoothThrottle, 0.0, 1.0);

    // ── Coolant temp warm-up ──────────────────────────────────────────────
    if (m_tickCount <= kWarmupTicks) {
        // Linear ramp from 70°F to 195°F
        double progress = static_cast<double>(m_tickCount) / kWarmupTicks;
        m_coolantTemp   = lerp(70.0, 195.0, progress);
    } else {
        // Oscillate ±3°F around 195°F
        double phase = static_cast<double>(m_tickCount) * 0.002; // slow wave
        m_coolantTemp = 195.0 + 3.0 * qSin(phase);
    }

    // ── Fuel drain ────────────────────────────────────────────────────────
    m_fuelLevel = clamp(m_fuelLevel - kFuelDrain, 0.0, 1.0);

    // ── Emit frame ────────────────────────────────────────────────────────
    VehicleFrame frame;
    frame.engineRpm        = m_smoothRpm;
    frame.vehicleSpeedMph  = m_smoothSpeed;
    frame.coolantTempF     = m_coolantTemp;
    frame.throttlePosition = m_smoothThrottle;
    frame.fuelLevel        = m_fuelLevel;
    frame.timestampMs      = QDateTime::currentMSecsSinceEpoch();

    emit frameReceived(frame);
}
