EESchema Schematic File Version 2
LIBS:simple-led-watch-rescue
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:simple-led-watch
LIBS:simple-led-watch-cache
EELAYER 25 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 1 6
Title "LED Watch, Simplified"
Date ""
Rev ""
Comp "Kevin Cuzner"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Sheet
S 2300 1400 1500 1800
U 57FA8083
F0 "Power Supply" 60
F1 "power-supply.sch" 60
F2 "VUSB" I L 2300 3100 60 
F3 "CHG_STAT" I R 3800 2500 60 
F4 "~USB_PRES" I R 3800 2600 60 
$EndSheet
$Sheet
S 4200 2200 3800 3000
U 57FA808A
F0 "Control" 60
F1 "control.sch" 60
F2 "USB_D+" I L 4200 3600 60 
F3 "USB_D-" I L 4200 3700 60 
F4 "~USB_PRES" I L 4200 2600 60 
F5 "CHG_STAT" I L 4200 2500 60 
F6 "BTN[0..3]" I L 4200 4500 60 
F7 "LED_A[0..5]" I R 8000 2400 50 
F8 "LED_C_MUX[0..3]" I R 8000 2500 50 
F9 "~LED_C_EN" I R 8000 2600 50 
$EndSheet
$Sheet
S 2300 4300 1500 900 
U 57FA8093
F0 "Input" 60
F1 "input.sch" 60
F2 "BTN[0..3]" I R 3800 4500 60 
$EndSheet
$Sheet
S 700  2900 1200 1100
U 57FA8096
F0 "USB" 60
F1 "usb.sch" 60
F2 "VUSB" I R 1900 3100 60 
F3 "USB_D+" I R 1900 3600 60 
F4 "USB_D-" I R 1900 3700 60 
$EndSheet
$Sheet
S 8400 2200 1500 3000
U 57FA809B
F0 "LEDs" 60
F1 "leds.sch" 60
F2 "LED_A[0..5]" I L 8400 2400 50 
F3 "LED_C_MUX[0..3]" I L 8400 2500 50 
F4 "~LED_C_EN" I L 8400 2600 50 
$EndSheet
Wire Wire Line
	1900 3100 2300 3100
Wire Wire Line
	4200 3600 1900 3600
Wire Wire Line
	1900 3700 4200 3700
Wire Wire Line
	4200 2500 3800 2500
Wire Wire Line
	3800 2600 4200 2600
Wire Bus Line
	3800 4500 4200 4500
Wire Bus Line
	8000 2400 8400 2400
Wire Bus Line
	8000 2500 8400 2500
Wire Wire Line
	8000 2600 8400 2600
$EndSCHEMATC
