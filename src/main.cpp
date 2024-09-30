#include <Arduino.h>
const int PWM_CHANNEL1 = 0;    // ESP32 has 16 channels which can generate 16 independent waveforms
const int PWM_CHANNEL2 = 1;    // ESP32 has 16 channels which can generate 16 independent waveforms
const int PWM_FREQ = 5000;     // Recall that Arduino Uno is ~490 Hz. Official ESP32 example uses 5,000Hz
const int PWM_RESOLUTION = 8; // We'll use same resolution as Uno (8 bits, 0-255) but ESP32 can go up to 16 bits 

// The max duty cycle value based on PWM resolution (will be 255 if resolution is 8 bits)
const int MAX_DUTY_CYCLE = (int)(pow(2, PWM_RESOLUTION) - 1); 
const int MIN_DUTY_CYCLE = (int)(0); 
const int HALF_DUTY_CYCLE = (int)(MAX_DUTY_CYCLE/2); 

// Lüfter mit Drehrichtungsumkehr. 0% - 45% PWM über Stege blasend, 55% - 100% über Stege saugend.

// The pin numbering on the Huzzah32 is a bit strange so always helps to consult the pin diagram
// See pin diagram here: https://makeabilitylab.github.io/physcomp/esp32/
const int LED1_OUTPUT_PIN = 16;
const int FAN1_OUTPUT_PIN = 17;
const int LED2_OUTPUT_PIN = 18;
const int FAN2_OUTPUT_PIN = 19;

const int RAMP_DELAY_MS = 20;  // delay between fade increments
const int TARGET_SPEED_DELAY_MS = 5000;

void setup() {

  Serial.begin(9600);
  Serial.println("Starting...");

  // Sets up a channel (0-15), a PWM duty cycle frequency, and a PWM resolution (1 - 16 bits) 
  // ledcSetup(uint8_t channel, double freq, uint8_t resolution_bits);
  ledcSetup(PWM_CHANNEL1, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(PWM_CHANNEL2, PWM_FREQ, PWM_RESOLUTION);

  // ledcAttachPin(uint8_t pin, uint8_t channel);
  ledcAttachPin(LED1_OUTPUT_PIN, PWM_CHANNEL1);
  ledcAttachPin(LED2_OUTPUT_PIN, PWM_CHANNEL2);
  ledcAttachPin(FAN1_OUTPUT_PIN, PWM_CHANNEL1);
  ledcAttachPin(FAN2_OUTPUT_PIN, PWM_CHANNEL2);
}

// set relativ speed: -100 to 100
void setSpeed(int channel1, int channel2, int relativspeed = 0) {
  int dutyCycle = int(float(relativspeed+100)/200.0*MAX_DUTY_CYCLE);
  int dutyCycleReverse = int(float(-1*relativspeed+100)/200.0*MAX_DUTY_CYCLE);;
  ledcWrite(channel1, dutyCycle);
  ledcWrite(channel2, dutyCycleReverse);
  Serial.print("Relativ: ");
  Serial.print(relativspeed);
  Serial.print(" duty1: ");
  Serial.print(dutyCycle);
  Serial.print(" duty2: ");
  Serial.println(dutyCycleReverse);
}

void loop() {

  // fade up PWM on given channel

  for(int fade = 0; fade <= 100; fade++){   
    setSpeed(PWM_CHANNEL1, PWM_CHANNEL2, fade);
    delay(RAMP_DELAY_MS);
  }

  delay(TARGET_SPEED_DELAY_MS);

  for(int fade = 100; fade >= -100; fade--){   
    setSpeed(PWM_CHANNEL1, PWM_CHANNEL2, fade);
    delay(RAMP_DELAY_MS);
  }

  delay(TARGET_SPEED_DELAY_MS);

  for(int fade = -100; fade <= 0; fade++){   
    setSpeed(PWM_CHANNEL1, PWM_CHANNEL2, fade);
    delay(RAMP_DELAY_MS);
  }
}
