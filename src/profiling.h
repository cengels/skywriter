#ifndef PROFILING_H
#define PROFILING_H

#include <QString>


namespace profiling
{
    //! Starts a new profiler.
    void start(const QString& name);
    //! Prints the currently elapsed time for the given profiler to the debug output.
    void mark(const QString& name);
    //! Stops the profiler and prints the total elapsed time for the given profiler to the debug output.
    void stop(const QString& name);
};

#endif // PROFILING_H
