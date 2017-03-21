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

case_radius = 18 * mm;
case_width = 25 * mm;
case_length = 42 * mm;
case_height = 9 * mm;

lid_height = 3 * mm;
lid_tab_width = 1.1875 * mm;
lid_tab_length = (case_length - (26 * mm)) / 2;
lid_tab_radius = 0.5 * mm;
lid_tab_padding = 0.3 * mm; //tuning parameter for lid fit

chamfer_size = sqrt(2 * pow(lid_height, 2)) * mm;

pcb_radius = 32.7 * mm / 2;
pcb_height = 1.6 * mm; //height of the actual circuit board, no parts
pcb_padding = 0.2 * mm; //tuning parameter for pcb fit
pcb_usb_length = 11.65 * mm;
pcb_usb_width = 32.95 * mm - pcb_radius * 2;

comp_height = 2.5 * mm;
comp_usb_length = 8.5 * mm;
comp_usb_height = 3 * mm; //height of usb port space
comp_usb_height_offset = -0.5 * mm; //offset of USB port space beneath PCB
comp_keepout_length = 1 * inches; //area beneath the PCB for component keepout
comp_keepout_width = 1.145 * inches; //area benath the PCB for component keepout
comp_keepout_width_offset = 0.15 * inches;
comp_prog_x = 0.2 * inches;
comp_prog_y = -0.34 * inches;
comp_prog_width = 3.8 * mm;
comp_prog_length = 6.4 * mm;

battery_width = 15.2 * mm;
battery_length = 35.7 * mm;
battery_height = 2.5 * mm;
battery_padding = 0.2 * mm;

band_width = 20 * mm;
band_height = 2 * mm;
band_angle = 45 * degrees;
band_center_offset = 0.7 * mm;

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
        //cut across watch face (the cube may be redundant)
        cube(size = [case_radius * 2 + pad_manifold, case_length - (lid_tab_length * 2) + lid_tab_padding * 2, lid_height + pad_manifold], center = true);
        cylinder(h = lid_height + pad_manifold, r = case_radius + lid_tab_padding, center = true);
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
            translate([comp_keepout_width_offset, 0, 0]) {
                union () {
                    cube(size = [comp_keepout_width, comp_keepout_length, comp_height + comp_usb_height], center = true);
                }
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
    translate([0, -case_length / 2, 0]) {
        rotate([-band_angle, 0, 0]) {
            cube(size = [band_width, case_height * 2, band_height], center = true);
        }
    }
}

difference() {
    CaseBase();
    translate([0, 0, case_height - lid_height]) {
        CaseLidInset();
        BatteryInset();
    }
    translate([0, 0, case_height - lid_height - pcb_height]) {
        PCBInset();
        ComponentInset();
    }
    translate([0, 0, (case_height - lid_height) / 2 - band_center_offset]) {
        WatchBandInset();
        mirror([0, 1, 0]) {
            WatchBandInset();
        }
    }
}

