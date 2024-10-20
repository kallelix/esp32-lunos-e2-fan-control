// Papst Product Data Sheet 4412 FGMPR: https://www.farnell.com/datasheets/2634798.pdf
// Lüfter mit Drehrichtungsumkehr. 0% - 45% PWM über Stege blasend, 55% - 100% über Stege saugend.

const int PWM_FREQ = 2000;     // Official ESP32 example uses 5,000Hz as default
const int PWM_RESOLUTION = 8; // resolution: 8 bits, 0-255

// pinouts
const int LED1_OUTPUT_PIN = 16;
const int LED2_OUTPUT_PIN = 17;
const int FAN1_LIVING_ROOM_OUTPUT_PIN = 18;
const int FAN2_LIVING_ROOM_OUTPUT_PIN = 19;
