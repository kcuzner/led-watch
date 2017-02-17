/**
 * LED Wristwatch Host Software
 *
 * Kevin Cuzner
 */

#include <hidapi.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "Wristwatch.h"

using namespace std;

int main(int argc, char **argv)
{
    auto paths = Wristwatch::findDevicePaths();

    auto it = paths.cbegin();
    if (it == paths.cend())
    {
        cout << "No devices found" << endl;
        exit(EXIT_FAILURE);
    }

    Wristwatch watch(*it);

    watch.read();

    return 0;
}
