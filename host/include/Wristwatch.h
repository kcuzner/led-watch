/**
 * LED Wristwatch Host Software
 *
 * Kevin Cuzner
 */

#pragma once

#include <string>
#include <vector>
#include <hidapi.h>

class Wristwatch
{
    public:
        Wristwatch(const std::string &path);
        ~Wristwatch();

        void read();

        static std::vector<std::string> findDevicePaths();

    private:
        hid_device *m_dev;
};

