#include "Wristwatch.h"

#include <stdexcept>
#include <cwchar>

#include <iostream>

using namespace std;

Wristwatch::Wristwatch(const string &path)
{
    m_dev = hid_open_path(path.c_str());
    if (!m_dev)
        throw new runtime_error("Unable to open device");
}

Wristwatch::~Wristwatch()
{
    hid_close(m_dev);
}

void Wristwatch::read()
{
    unsigned char data[255];
    int res = hid_read_timeout(m_dev, data, sizeof(data), 1000);
    if (!res || res == -1)
    {
        cout << "Unable to read device" << endl;
    }
    else
    {
        cout << "Read " << res << " characters" << endl;
        if (res)
            printf("0x%02x\n", data[0]);
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

