/**
 * LED Wristwatch Host Software
 *
 * Kevin Cuzner
 */

#pragma once

#include <string>
#include <functional>
#include <mutex>

#include "Wristwatch.h"

/**
 * Polls a wristwatch device for reports
 */
class WristwatchPoller
{
public:
    /**
     * Creates a new WristwatchPoller
     *
     * callbacl: Callback function for when a new report is received
     */
    WristwatchPoller(std::function<void(const WristwatchReport &)> callback);

    /**
     * Continuously polls the first found wristwatch device at the passed path,
     * calling the callback when a new report is received.
     */
    void poll();

    void stop();

protected:
    void reset();
    bool getStop();

private:
    void tick();
    bool tryConnect();

    std::mutex m_mutex;
    std::function<void(const WristwatchReport &)> m_callback;
    bool m_stop;
};

