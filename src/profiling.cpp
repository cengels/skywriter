#include "profiling.h"

#include <QHash>
#include <QDebug>
#include <QDateTime>

namespace {
    QHash<const QString, qint64> timeMap;
    QHash<const QString, int> countMap;
    QDebug debug() {
        QDebug debug = qDebug();

        debug.noquote();

        return debug;
    }
}

void profiling::start(const QString& name)
{
    if (timeMap.contains(name)) {
        auto value = timeMap.value(name);

        if (value <= 0) {
            // was paused
            timeMap.insert(name, QDateTime::currentMSecsSinceEpoch() + timeMap.value(name));
        }

        return;
    }

    debug() << "[" + name + "]" << "Starting profiling.";

    timeMap.insert(name, QDateTime::currentMSecsSinceEpoch());
}

void profiling::mark(const QString& name)
{
    if (!timeMap.contains(name)) {
        debug() << "Can't mark: profiler" << name << "is not started.";
        return;
    }

    auto value = timeMap.value(name);
    auto time = value > 0 ? QDateTime::currentMSecsSinceEpoch() - value : -value;

    debug() << "[" + name + "]" << time << "ms elapsed.";
}

void profiling::stop(const QString& name)
{
    if (!timeMap.contains(name)) {
        debug() << "Can't stop: profiler" << name << "is not started.";
        return;
    }

    auto value = timeMap.value(name);
    auto time = value > 0 ? QDateTime::currentMSecsSinceEpoch() - value : -value;

    debug() << "[" + name + "]" << time << "ms elapsed. Stop.";

    timeMap.remove(name);
}

void profiling::pause(const QString& name)
{
    if (!timeMap.contains(name)) {
        debug() << "Can't pause: profiler" << name << "is not started.";
        return;
    }

    // Saves the currently elapsed time, thus temporarily changing the meaning of the time map's value field.
    // We're saving it as a negative number to be able to identify it later.
    qint64 msecs = -(QDateTime::currentMSecsSinceEpoch() - timeMap.value(name));
    timeMap.insert(name, msecs);
}

void profiling::count(const QString& name)
{
    if (!countMap.contains(name)) {
        countMap.insert(name, 1);
    } else {
        countMap.insert(name, countMap.value(name) + 1);
    }
}

void profiling::countEnd(const QString& name)
{
    if (!countMap.contains(name)) {
        debug() << "Count" << name << "was called a total of 0 times.";
    } else {
        debug() << "Count" << name << "was called a total of " << countMap.value(name) << " times.";
        countMap.remove(name);
    }
}
