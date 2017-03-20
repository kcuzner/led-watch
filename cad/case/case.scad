inches = 25.4;
mm = 1;

pcb_diameter = 1.29 * inches;
pcb_thickness = 0.0625 * inches;
pcb_clearence_upper = 1.5 * mm;
pcb_clearence_lower = 2.7 * mm;
pcb_tab_edge_width = 0.045 * inches;
pcb_tab_edge_height = 0.43 * inches;
pcb_tab_clearence_upper = 0 * mm;
pcb_tab_clearence_lower = 2.6 * mm;

pcb_buffer = 0.2 * mm;

union()
{
    //main pcb thickness and keepout
    color([1, 0, 0]) translate([0, 0, pcb_clearence_upper/2+pcb_thickness/2]) cylinder(r=pcb_diameter/2, h=pcb_clearence_upper, $fn=128, center=true);
    color([0, 1, 0]) cylinder(r=pcb_diameter/2, h=pcb_thickness, $fn=128, center=true);
    color([1, 0, 0]) translate([0, 0, -pcb_clearence_lower/2-pcb_thickness/2]) cylinder(r=pcb_diameter/2, h=pcb_clearence_lower, $fn=128, center=true);
    //pcb tab thickness and keepout
    translate([pcb_diameter/2-pcb_tab_edge_width/2, 0, 0]) union()
    {
        color([1, 0, 0]) translate([0, 0, pcb_tab_clearence_upper/2+pcb_thickness/2]) cube(size=[pcb_tab_edge_width, pcb_tab_edge_height, pcb_tab_clearence_upper], center=true);
        color([0, 1, 0]) cube(size=[pcb_tab_edge_width, pcb_tab_edge_height, pcb_thickness], center=true);
        color([1, 0, 0]) translate([0, 0, -pcb_tab_clearence_lower/2-pcb_thickness/2]) cube(size=[pcb_tab_edge_width, pcb_tab_edge_height, pcb_tab_clearence_lower], center=true);
    }
}


