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
        debug() << "Profiler" << name << "already started. Not starting another one.";
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

    debug() << "[" + name + "]" << QDateTime::currentMSecsSinceEpoch() - timeMap.value(name) << "ms elapsed.";
}

void profiling::stop(const QString& name)
{
    if (!timeMap.contains(name)) {
        debug() << "Can't stop: profiler" << name << "is not started.";
        return;
    }

    debug() << "[" + name + "]" << QDateTime::currentMSecsSinceEpoch() - timeMap.value(name) << "ms elapsed. Stop.";

    timeMap.remove(name);
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
