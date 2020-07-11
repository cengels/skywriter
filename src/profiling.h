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

    //! Increments the count for the given profiler name.
    void count(const QString& name);
    //! Prints the total count for the given profiler name to the debug output
    //! and clears the count.
    void countEnd(const QString& name);
};

#endif // PROFILING_H
