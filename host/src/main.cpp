/**
 * LED Wristwatch Host Software
 *
 * Kevin Cuzner
 */

#include <hidapi.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <chrono>
#include <ctime>

#include <gtkmm/application.h>

#include "Wristwatch.h"
#include "MainWindow.h"

using namespace std;

int main(int argc, char **argv)
{
    WristwatchReport report;

    auto paths = Wristwatch::findDevicePaths();

    auto it = paths.cbegin();
    if (it == paths.cend())
    {
        cout << "No devices found" << endl;
        exit(EXIT_FAILURE);
    }

    auto watch = Wristwatch(*it, true);
    auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto now = localtime(&t);
    report.data[0] = now->tm_year - 100;
    report.data[1] = now->tm_mon + 1;
    report.data[2] = now->tm_mday;
    report.data[3] = now->tm_hour;
    report.data[4] = now->tm_min;
    report.data[5] = now->tm_sec;
    watch.sendReport(report);

    //auto app = Gtk::Application::create(argc, argv, "com.kevincuzner.wristwatch");

    //MainWindow mainWindow;

    return 0;// app->run(mainWindow);
}
