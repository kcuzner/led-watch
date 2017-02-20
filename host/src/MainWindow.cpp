/**
 * LED Wristwatch Host Software
 *
 * Kevin Cuzner
 */

#include "MainWindow.h"
#include "Wristwatch.h"

#include <gtkmm/button.h>
#include <gtkmm/hvbox.h>
#include <gtkmm/menubar.h>
#include <gtkmm/menu.h>
#include <gtkmm/menuitem.h>
#include <gtkmm/statusbar.h>

MainWindow::MainWindow()
{
    set_type_hint(Gdk::WINDOW_TYPE_HINT_DIALOG); //encourages i3wm to work as I expect with this program (I love i3)

    set_title("LED Wristwatch");
    set_default_size(400, 200);

    auto vbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    add(*vbox);

    //Create main menu
    auto menubar = Gtk::manage(new Gtk::MenuBar());
    vbox->pack_start(*menubar, Gtk::PACK_SHRINK);

    auto fileMenu = Gtk::manage(new Gtk::Menu());
    auto fileMi = Gtk::manage(new Gtk::MenuItem("File"));
    auto quitMi = Gtk::manage(new Gtk::MenuItem("Quit"));
    fileMi->set_submenu(*fileMenu);
    fileMenu->append(*quitMi);
    menubar->append(*fileMi);
    quitMi->signal_activate().connect(sigc::mem_fun(this, &MainWindow::on_quit_activated));

    //Create status bar
    auto statusBar = Gtk::manage(new Gtk::Statusbar());
    statusBar->push("Disconnected");
    vbox->pack_end(*statusBar, Gtk::PACK_SHRINK);

    //Create main content
    //auto btn = Gtk::manage(new Gtk::Button("asdf"));
    //vbox->pack_start(*btn);

    show_all();
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_quit_activated()
{
    //hide this window to quit the application
    hide();
}

