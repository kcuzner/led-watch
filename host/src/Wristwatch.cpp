#include "Wristwatch.h"

#include <stdexcept>
#include <cwchar>
#include <cstring>
#include <codecvt>
#include <locale>

#include <iostream>

using namespace std;

Wristwatch::Wristwatch(const string &path)
{
    m_dev = hid_open_path(path.c_str());
    if (!m_dev)
        throw new runtime_error("Unable to open device");

    hid_set_nonblocking(m_dev, 1);
}

Wristwatch::~Wristwatch()
{
    hid_close(m_dev);
}

bool Wristwatch::getReport(WristwatchReport &report)
{
    unsigned char data[sizeof(report)];
    int res = hid_read(m_dev, data, sizeof(data));
    if (res < 0)
    {
        auto error = wstring(hid_error(m_dev));
        using convert_type = std::codecvt_utf8<wchar_t>;
        wstring_convert<convert_type, wchar_t> converter;
        throw new runtime_error(converter.to_bytes(error));
    }
    else if (res)
    {
        memcpy(&report, data, sizeof(report));
        return true;
    }
    else
    {
        return false;
    }
}

vector<string> Wristwatch::findDevicePaths()
{
    vector<string> paths;
    struct hid_device_info *devs, *cur_dev;

    devs = hid_enumerate(0x16c0, 0x05dc);
    cur_dev = devs;
    while (cur_dev)
    {
        if (!wcscmp(cur_dev->manufacturer_string, L"kevincuzner.com") && !wcscmp(cur_dev->product_string, L"LED Wristwatch"))
            paths.push_back(cur_dev->path);
        cur_dev = cur_dev->next;
    }

    return paths;
}

