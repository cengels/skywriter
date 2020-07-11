#include "profiling.h"

#include <QHash>
#include <QDebug>
#include <QDateTime>

namespace {
    QHash<const QString, qint64> map;
    QDebug debug() {
        QDebug debug = qDebug();

        debug.noquote();

        return debug;
    }
}

void profiling::start(const QString& name)
{
    if (map.contains(name)) {
        debug() << "Profiler" << name << "already started. Not starting another one.";
        return;
    }

    debug() << "[" + name + "]" << "Starting profiling.";

    map.insert(name, QDateTime::currentMSecsSinceEpoch());
}

void profiling::mark(const QString& name)
{
    if (!map.contains(name)) {
        debug() << "Can't mark: profiler" << name << "is not started.";
        return;
    }

    debug() << "[" + name + "]" << QDateTime::currentMSecsSinceEpoch() - map.value(name) << "ms elapsed.";
}

void profiling::stop(const QString& name)
{
    if (!map.contains(name)) {
        debug() << "Can't stop: profiler" << name << "is not started.";
        return;
    }

    debug() << "[" + name + "]" << QDateTime::currentMSecsSinceEpoch() - map.value(name) << "ms elapsed. Stop.";

    map.remove(name);
}
