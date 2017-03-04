/**
 * LED Wristwatch Host Software
 *
 * Kevin Cuzner
 */

#pragma once

#include <string>
#include <vector>
#include <hidapi.h>

struct WristwatchReport
{
    uint8_t data[8]; //TODO: Replace this with something nice
};

class Wristwatch
{
public:
    Wristwatch(const std::string &path, bool blocking = false);
    ~Wristwatch();

    /**
     * Gets a report from the device
     */
    bool getReport(WristwatchReport &report);

    /**
     * Sends a report to the device
     */
    void sendReport(const WristwatchReport &report);

    /**
     * Finds all currently valid LED Wristwatch device paths
     */
    static std::vector<std::string> findDevicePaths();

private:
    void throwHid();

    hid_device *m_dev;
};
