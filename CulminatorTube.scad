// Tube Stuff
barrel_depth = 13.9;
barrel_length = 15;
barrel_diameter = 24.25-.75;
tube_thickness = 2.5;


// Lense Stuff
culminator_lense_diameter = 29.9-.75;
focal_length = 14.6;
led_height = 4.5;
lense_lip_depth = 2;


 
module culminator_tube(){
	// Do some maths
	tube_height  = (barrel_length - barrel_depth) + focal_length + led_height;
	difference(){
		difference(){
			cylinder(h = tube_height, r2 = (culminator_lense_diameter/2 + tube_thickness), r1 = (barrel_diameter + tube_thickness)/2);
			cylinder(h = tube_height, r = barrel_diameter/2);
		}
		translate([0,0,tube_height - lense_lip_depth]) cylinder(h = 2.2, r = culminator_lense_diameter/2);
	}
}

culminator_tube();