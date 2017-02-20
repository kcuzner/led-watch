/**
 * LED Wristwatch Host Software
 *
 * Kevin Cuzner
 */

#include <hidapi.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <gtkmm/application.h>

#include "Wristwatch.h"
#include "MainWindow.h"

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

    auto app = Gtk::Application::create(argc, argv, "com.kevincuzner.wristwatch");

    MainWindow mainWindow;

    return app->run(mainWindow);
}
