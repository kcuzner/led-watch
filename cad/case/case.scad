// Watch case, tuned for the MP Select Mini 3D Printer
// Material: ABS
// Nozzle: 0.2mm
// Layer height: <0.1mm

// We use various dimensions, depending on where I pulled them from (PCB CAD is inches, physical measurements are mm)
inches = 25.4;
mm = 1;
degrees = 1;

//Set up for higher resolution circles
$fn = 360;

//Variables

pad_manifold = 0.1 * mm; //padding for maintaining a manifold (avoiding zero-width shapes)

case_radius = 18.2 * mm;
case_width = 27 * mm;
case_length = 44 * mm;
case_height = 9 * mm;

pcb_radius = 32.7 * mm / 2;
pcb_height = 1.6 * mm; //height of the actual circuit board, no parts
pcb_padding = 0.2 * mm; //tuning parameter for pcb fit
pcb_usb_length = 11.65 * mm;
pcb_usb_width = 32.95 * mm - pcb_radius * 2;

lid_height = 2.5 * mm;
lid_tab_width = 1.5 * mm;
lid_tab_length = (case_length - (26 * mm)) / 2;
lid_tab_radius = 0.5 * mm;
lid_tab_padding = 0.3 * mm; //tuning parameter for lid fit (on case)
lid_height_padding = 0.1 * mm; //tuning parameter for lid fit (on lid)
lid_chamfer_size = 1 * mm;
lid_pcb_retention = 0.5 * mm;
lid_window_radius = pcb_radius - 1 * mm;
lid_window_height = 0.3 * mm;

chamfer_size = sqrt(2 * pow(lid_height, 2)) * mm;

lip_height = 1 * mm;
lip_radius = pcb_radius + abs(case_radius - pcb_radius) * 0.5;
lip_padding = 0.3 * mm; //tuning parameter for lid fit (on lid)

comp_height = 3 * mm;
comp_usb_length = 8.5 * mm;
comp_usb_height = 3.5 * mm; //height of usb port space
comp_usb_height_offset = -0.5 * mm; //offset of USB port space beneath PCB
comp_keepout_length = 1 * inches; //area beneath the PCB for component keepout
comp_keepout_width = 1.145 * inches; //area benath the PCB for component keepout
comp_keepout_length2 = 0.2 * inches;
comp_keepout_width_offset = 0.15 * inches;
comp_prog_x = 0.2 * inches;
comp_prog_y = -0.34 * inches;
comp_prog_width = 3.8 * mm;
comp_prog_length = 6.4 * mm;

battery_width = 15.2 * mm;
battery_length = 35.7 * mm;
battery_height = 3 * mm;
battery_padding = 0.2 * mm;

band_width = 20 * mm;
band_height = case_height - lid_height - 2 * mm;
band_depth = 3.9 * mm;

bar_radius = 1 * mm / 2;
bar_end_offset = 0.85 * mm;
bar_height_offset = 1 * mm;
bar_hole_depth = 1.5 * mm;

button_width = 3.5 * mm;
button_thickness = 1 * mm;
button_height = 2 * mm;
button_height_offset = -1 * mm;
button_cap_thickness = 0.5 * mm;
button_cap_radius = 0.5 * mm;
button_angles = [30, 150, -150, -30] * degrees;

//Base shape for the case
module CaseBase() {
    union() {
        translate([0, 0, case_height / 2]) {
            cube(size = [case_width, case_length, case_height], center = true);
            cylinder(h = case_height, r = case_radius, center = true);
        }
    }
}

//Subtractive solid for the space occupied by the case lid with chamfers
module CaseLidInset() {
    translate([0, 0, lid_height / 2 + pad_manifold / 2]) {
        difference () {
            //cut out for the vertical tab surfaces
            cube(size = [case_width - lid_tab_width * 2 + lid_tab_padding * 2, case_length + pad_manifold, lid_height + pad_manifold], center = true);
            //horizontal tab sufaces (rendered as cylinders)
            union() {
                translate([case_width/2 - lid_tab_width + lid_tab_padding, case_length / 2 + pad_manifold, lid_height / 2 - lid_tab_radius]) {
                    rotate([90, 0, 0]) {
                        cylinder(h = case_length + pad_manifold * 2, r = lid_tab_radius);
                    }
                }
                translate([-case_width/2 + lid_tab_width - lid_tab_padding, case_length / 2 + pad_manifold, lid_height / 2 - lid_tab_radius]) {
                    rotate([90, 0, 0]) {
                        cylinder(h = case_length + pad_manifold * 2, r = lid_tab_radius);
                    }
                }
            }
        }
        //cut across watch face
        cylinder(h = lid_height + pad_manifold, r = lip_radius, center = true);
        translate([0, 0, lid_height / 2 - (lid_height - lip_height)/2])
            cylinder(h = lid_height - lip_height + pad_manifold, r = case_radius + lid_tab_padding, center = true);
        //remove any remaining stubs from the tabs (if the distance from lip top to case top is less than then tab height, there will be a spike left over)
        intersection() {
            cube(size = [case_width - lid_tab_width * 2 + lid_tab_padding * 2, case_length + pad_manifold, lid_height + pad_manifold], center = true);
            cylinder(h = lid_height + pad_manifold, r = case_radius + lid_tab_padding, center = true);
        }
    }
    //chamfer 1
    translate([0, -case_length/2, lid_height]) {
        rotate([45, 0, 0]) {
            cube([case_width + pad_manifold * 2, chamfer_size, chamfer_size], center = true);
        }
    }
    //chamfer 2
    translate([0, case_length/2, lid_height]) {
        rotate([45, 0, 0]) {
            cube([case_width + pad_manifold * 2, chamfer_size, chamfer_size], center = true);
        }
    }
}

//Subtractive solid for the space occupied by the PCB itself
module PCBInset() {
    union() {
        translate([0, 0, pcb_height / 2 + pad_manifold / 2]) {
            cylinder(h = pcb_height + pad_manifold, r = pcb_radius + pcb_padding, center = true);
            translate([0, -(pcb_usb_length + pcb_padding) / 2, -pcb_height / 2 - pad_manifold / 2]) {
                cube(size = [pcb_usb_width + pcb_radius + pcb_padding, pcb_usb_length + pcb_padding, pcb_height + pad_manifold], center = false);
            }
        }
    }
}

//Subtractive solid for the space beneath the PCB occupied by components, including USB, programming header, and supports
module ComponentInset() {
    translate([0, 0, -comp_height]) {
        //main components
        intersection() {
            union () {
                //general component height
                cylinder(h = comp_height + pad_manifold, r = pcb_radius);
                //usb
                translate([0, -comp_usb_length / 2, 0]) {
                    cube(size = [case_radius + pad_manifold, comp_usb_length, comp_usb_height]);
                }
            }
            //board support keepout
            union () {
                translate([comp_keepout_width_offset, 0, 0])
                    cube(size = [comp_keepout_width, comp_keepout_length, comp_height + comp_usb_height], center = true);
                translate([0, comp_keepout_length/2 - comp_keepout_length2, 0])
                    cube(size = [pcb_radius * 2, comp_keepout_length / 2 - comp_keepout_length2 / 2, comp_height + comp_usb_height], center = true);
                translate([0, -comp_keepout_length/2 + comp_keepout_length2, 0])
                    cube(size = [pcb_radius * 2, comp_keepout_length / 2 - comp_keepout_length2 / 2, comp_height + comp_usb_height], center = true);
            }
        }
        //programming header
        translate([comp_prog_x, comp_prog_y, -case_height / 2 + comp_height]) {
            cube(size = [comp_prog_width, comp_prog_length, case_height], center = true);
        }
    }
}

//Subtractive solid for the space beneath the PCB occupied by the battery, including space for inserting the battery
module BatteryInset() {
    battery_offset = -(case_width - (2 * lid_tab_width)) / 2;
    translate([battery_offset, -(battery_length + battery_padding * 2) / 2, -battery_height - battery_padding - comp_height]) {
        cube(size = [battery_width + battery_padding * 2, battery_length + battery_padding * 2, battery_height + battery_padding * 2 + comp_height + pad_manifold], center = false);
    }
}

//Subtractive solid for the watch band slots
module WatchBandInset() {
    translate([0, -case_length / 2 + band_depth / 2 - pad_manifold, band_height / 2 - pad_manifold]) {
        cube(size = [band_width, band_depth, band_height], center = true);
    }
}

//Subtractive solid for the spring bar holes
module SpringBarInset() {
    translate([0, -case_length / 2 + bar_end_offset, bar_height_offset]) {
        rotate([0, 90, 0]) {
            cylinder(h = band_width + 2 * bar_hole_depth, r = bar_radius, center = true);
        }
    }
}

//Subtractive solid for the buttons
module ButtonInset() {
    button_padding = pad_manifold + pcb_radius - cos(asin(button_width / (2 * pcb_radius))) * pcb_radius; //distance to edge of PCB from the far corner of the button
    button_depth = case_radius - pcb_radius + pad_manifold;
    for (angle = button_angles) {
        rotate([0, 0, angle]) {
            translate([pcb_radius + button_thickness / 2 - button_padding/2, 0, button_height / 2 + button_height_offset]) {
                translate([0, 0, (button_height_offset + pcb_height)/2 + pad_manifold/2])
                    cube(size = [button_thickness + button_padding, button_width, button_height + (button_height_offset + pcb_height) + pad_manifold], center = true);
                hull() {
                    translate([button_thickness / 2, 0, ])
                        cube(size = [button_thickness + button_cap_thickness, button_width, button_height], center = true);
                    rotate([0, 90, 0])
                        cylinder(h = button_thickness + button_cap_thickness, r = button_cap_radius);
                }
                rotate([0, 90, 0])
                    cylinder(h = button_depth + pad_manifold, r = button_cap_radius);
            }
        }
    }
}

//Base solid for the lid component
module LidBase() {
    difference() {
        translate([0, 0, lid_height / 2 + lid_height_padding / 2]) {
            union () {
                //chamfered center circle
                hull() {
                    cylinder(h = lid_height - lid_height_padding, r = case_radius - lid_chamfer_size / 2, center = true);
                    translate([0, 0, -lid_chamfer_size/2])
                        cylinder(h = lid_height - lid_chamfer_size - lid_height_padding, r = case_radius, center = true);
                }
                //edge chamfered rectangle for tabs
                hull() {
                    translate([0, 0, lid_height / 2 - lid_tab_radius - lid_height_padding / 2])
                        cube(size = [case_width - lid_tab_width * 2 - lid_tab_radius * 4, case_length, lid_tab_radius * 2], center = true);
                    cube(size = [case_width - lid_tab_width * 2, case_length, max(lid_height - lid_tab_radius * 4 - lid_height_padding, pad_manifold)], center = true);
                    translate([0, 0, -lid_height / 2  + lid_tab_radius + lid_height_padding / 2])
                        cube(size = [case_width - lid_tab_width * 2 - lid_tab_radius * 4, case_length, lid_tab_radius * 2], center = true);
                }
            }
        }
        //chamfer 1
        translate([0, -case_length/2, lid_height]) {
            rotate([45, 0, 0]) {
                cube([case_width + pad_manifold * 2, chamfer_size, chamfer_size], center = true);
            }
        }
        //chamfer 2
        translate([0, case_length/2, lid_height]) {
            rotate([45, 0, 0]) {
                cube([case_width + pad_manifold * 2, chamfer_size, chamfer_size], center = true);
            }
        }
    }
}

//Subtractive solid for the window in the lid
module LidWindowInset() {
    translate([0, 0, lid_height / 2]) {
        cylinder(h = lid_height + pad_manifold, r = lid_window_radius, center = true);
        translate([0, 0, -lid_window_height])
            cylinder(h = lid_height - lid_window_height + pad_manifold, r = pcb_radius - lid_pcb_retention, center = true);
    }
}

//Subtractive solid for the lid lip
module LidLipInset() {
    difference() {
        cylinder(h = lip_height + lid_height_padding, r = case_radius + pad_manifold);
        cylinder(h = lip_height + lid_height_padding, r = lip_radius - lip_padding);
        translate([0, 0, (lid_height + pad_manifold) / 2])
            cube(size = [case_width - lid_tab_width * 2, case_length, lid_height + pad_manifold], center = true);
    }
}

module Body() { // `make` me
    difference() {
        CaseBase();
        translate([0, 0, case_height - lid_height]) {
            CaseLidInset();
            BatteryInset();
        }
        translate([0, 0, case_height - lid_height - pcb_height]) {
            PCBInset();
            ComponentInset();
            ButtonInset();
        }
        translate([0, 0, (case_height - lid_height) / 2, 0]) {
            WatchBandInset();
            mirror([0, 1, 0]) {
                WatchBandInset();
            }
        }
        translate([0, 0, 0]) {
            SpringBarInset();
            mirror([0, 1, 0]) {
                SpringBarInset();
            }
        }
    }
}

module Lid() { // `make` me
    difference() {
        LidBase();
        LidWindowInset();
        LidLipInset();
    }
}

Body();
translate([0, 0, case_height - lid_height]) {
    Lid();
}

