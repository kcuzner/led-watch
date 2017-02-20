/**
 * LED Wristwatch Host Software
 *
 * Kevin Cuzner
 */

#pragma once

#include <gtkmm/window.h>

class MainWindow : public Gtk::Window
{
public:
    MainWindow();
    ~MainWindow();

private:
    void on_quit_activated();
};

