/**
 * LED Wristwatch Host Software
 *
 * Kevin Cuzner
 */

#include "WristwatchPoller.h"

WristwatchPoller::WristwatchPoller(std::function<void(const WristwatchReport &)> fn)
        : m_callback(fn)
{
}

void WristwatchPoller::poll()
{
    WristwatchReport report;

    reset();

    try
    {
        auto watch = Wristwatch(path);
        while (!getStop())
        {
            if (watch.getReport(report))
                callback(report);
        }
    }
    catch (...)
    {
        return false;
    }

    return true;
}

void WristwatchPoller::stop()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_stop = true;
}

bool WristwatchPoller::getStop()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_stop;
}

void WristwatchPoller::reset()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_stop = false;
}

