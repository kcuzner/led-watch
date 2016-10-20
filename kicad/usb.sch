EESchema Schematic File Version 2
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
$Descr A4 11693 8268
encoding utf-8
Sheet 5 6
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L USB_OTG P3
U 1 1 57FA80BD
P 3750 3300
F 0 "P3" H 4075 3175 50  0000 C CNN
F 1 "USB_OTG" H 3750 3500 50  0000 C CNN
F 2 "simple-led-watch:AMP-10104110-USB" V 3700 3200 50  0001 C CNN
F 3 "" V 3700 3200 50  0000 C CNN
F 4 "609-4052-1-ND" H 3750 3300 60  0001 C CNN "Part No."
	1    3750 3300
	0    -1   1    0   
$EndComp
$Comp
L GND #PWR033
U 1 1 57FA8252
P 4200 3650
F 0 "#PWR033" H 4200 3400 50  0001 C CNN
F 1 "GND" H 4200 3500 50  0000 C CNN
F 2 "" H 4200 3650 50  0000 C CNN
F 3 "" H 4200 3650 50  0000 C CNN
	1    4200 3650
	1    0    0    -1  
$EndComp
NoConn ~ 4050 3400
Text HLabel 9300 3100 2    60   Input ~ 0
VUSB
$Comp
L USBLC6-2 U5
U 1 1 57FA840B
P 6300 3900
F 0 "U5" H 6300 3450 60  0000 C CNN
F 1 "USBLC6-2" H 6300 4350 60  0000 C CNN
F 2 "simple-led-watch:SOT-666" H 6300 3900 60  0001 C CNN
F 3 "" H 6300 3900 60  0001 C CNN
F 4 "497-5026-1-ND" H 6300 3900 60  0001 C CNN "Part No."
	1    6300 3900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR034
U 1 1 57FA84E6
P 5700 4400
F 0 "#PWR034" H 5700 4150 50  0001 C CNN
F 1 "GND" H 5700 4250 50  0000 C CNN
F 2 "" H 5700 4400 50  0000 C CNN
F 3 "" H 5700 4400 50  0000 C CNN
	1    5700 4400
	1    0    0    -1  
$EndComp
Text HLabel 8000 4000 2    60   Input ~ 0
USB_D+
Text HLabel 8000 3800 2    60   Input ~ 0
USB_D-
$Comp
L R R36
U 1 1 5805F39C
P 3400 4050
F 0 "R36" V 3480 4050 50  0000 C CNN
F 1 "ANY" V 3400 4050 50  0000 C CNN
F 2 "Resistors_SMD:R_0402" V 3330 4050 50  0001 C CNN
F 3 "" H 3400 4050 50  0000 C CNN
	1    3400 4050
	1    0    0    -1  
$EndComp
$Comp
L C C27
U 1 1 5805F3E3
P 3650 4050
F 0 "C27" H 3675 4150 50  0000 L CNN
F 1 "ANY" H 3675 3950 50  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 3688 3900 50  0001 C CNN
F 3 "" H 3650 4050 50  0000 C CNN
	1    3650 4050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR035
U 1 1 5805F4FD
P 3650 4400
F 0 "#PWR035" H 3650 4150 50  0001 C CNN
F 1 "GND" H 3650 4250 50  0000 C CNN
F 2 "" H 3650 4400 50  0000 C CNN
F 3 "" H 3650 4400 50  0000 C CNN
	1    3650 4400
	1    0    0    -1  
$EndComp
Text Notes 3900 4100 0    50   ~ 0
Populate 0402 as needed
Text Label 4600 3200 0    50   ~ 0
USB_C_D-
Text Label 4600 3300 0    50   ~ 0
USB_C_D+
Text Label 3650 3800 0    50   ~ 0
USB_SHIELD
Wire Wire Line
	4050 3500 4200 3500
Wire Wire Line
	4200 3500 4200 3650
Wire Wire Line
	4050 3100 9300 3100
Wire Wire Line
	5800 3600 5700 3600
Wire Wire Line
	5700 3600 5700 3100
Connection ~ 5700 3100
Wire Wire Line
	5700 4400 5700 4200
Wire Wire Line
	5700 4200 5800 4200
Wire Wire Line
	4050 3200 5600 3200
Wire Wire Line
	4050 3300 5500 3300
Wire Wire Line
	6800 3800 8000 3800
Wire Wire Line
	8000 4000 6800 4000
Wire Wire Line
	3650 3700 3650 3900
Wire Wire Line
	3650 3800 3400 3800
Wire Wire Line
	3400 3800 3400 3900
Connection ~ 3650 3800
Wire Wire Line
	3400 4200 3400 4300
Wire Wire Line
	3400 4300 3650 4300
Wire Wire Line
	3650 4200 3650 4400
Connection ~ 3650 4300
Wire Wire Line
	5500 3300 5500 4000
Wire Wire Line
	5500 4000 5800 4000
Wire Wire Line
	5600 3200 5600 3800
Wire Wire Line
	5600 3800 5800 3800
$EndSCHEMATC
