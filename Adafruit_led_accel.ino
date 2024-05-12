#include <Adafruit_NeoPixel.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Scheduler.h>
//
// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN 7
Adafruit_MPU6050 mpu;
// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 72
bool fast = false;
long currentColor = 0;
// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  Serial.println("Started");
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.


  strip.begin();             // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();              // Turn OFF all pixels ASAP
  strip.setBrightness(255);  // Set BRIGHTNESS to about 1/5 (max = 255)
  Scheduler.startLoop(showLights);

  // accelerometer
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(1);
    }
  }
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  mpu.setMotionDetectionThreshold(1);  //1
  mpu.setMotionDetectionDuration(20);  //20
  //mpu.setInterruptPinLatch(true);      // Keep it latched.  Will turn off when reinitialized.
  //mpu.setInterruptPinPolarity(true);
  // mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  //mpu.setMotionInterrupt(true);

  Serial.println("");
  delay(100);
  Scheduler.startLoop(detect);
}

void loop() {}

void showLights() {
  int speed = 25;
  if (fast == true) {
    // colorWipe(strip.Color(255, 0, 255), speed);  // Red
    rainbow2(5);
    //theaterChaseRainbow(speed);
  } else {
    rainbow(speed);  // Flowing rainbow cycle along the whole strip
  }
}

void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < strip.numPixels(); i++) {  // For each pixel in strip...
    if (false == fast) {
      break;
    }
    strip.setPixelColor(i, color);  //  Set pixel's color (in RAM)
    strip.show();                   //  Update strip to match
    delay(wait);                    //  Pause for a moment
  }
}


void detect() {
 // if (mpu.getMotionInterruptStatus()) {
    /* Get new sensor events with the readings */
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    /* Print out the values */
    // Serial.print("AccelX:");
    // Serial.print(a.acceleration.x);
    // Serial.print(",");
    // Serial.print("AccelY:");
    // Serial.print(a.acceleration.y);
    // Serial.print(",");
    // Serial.print("AccelZ:");
    // Serial.print(a.acceleration.z);
    // Serial.print(", ");
    Serial.print("GyroX:");
    Serial.print(g.gyro.x);
    Serial.print(",");
    Serial.print("GyroY:");
    Serial.print(g.gyro.y);
    Serial.print(",");
    Serial.print("GyroZ:");
    Serial.print(g.gyro.z);

    float gyroX = abs(g.gyro.x);
    float gyroY = abs(g.gyro.y);
    float threshold = 1;

    if (gyroX > threshold || gyroY > threshold) {
      Serial.print("     fast");
      fast = true;
    } else {
      fast = false;
      Serial.print("     slow");
    }
    Serial.println("");
    Serial.println("");
//  }
  delay(1);
}


// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this loop:
  for (long firstPixelHue = currentColor; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
    if (currentColor != 0) {
   //   firstPixelHue = currentColor;
      currentColor = 0;
    }
    if (fast) {
      if (firstPixelHue != 0) {
        currentColor = firstPixelHue;
      }
      break;
    }
    // strip.rainbow() can take a single argument (first pixel hue) or
    // optionally a few extras: number of rainbow repetitions (default 1),
    // saturation and value (brightness) (both 0-255, similar to the
    // ColorHSV() function, default 255), and a true/false flag for whether
    // to apply gamma correction to provide 'truer' colors (default true).
    strip.rainbow(firstPixelHue);
    // Above line is equivalent to:
    // strip.rainbow(firstPixelHue, 1, 255, 255, true);
    strip.show();  // Update strip with new contents
    delay(wait);   // Pause for a moment
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow2(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this loop:
  for (long firstPixelHue = currentColor; firstPixelHue < 5 * 65536; firstPixelHue += 1024) {
    if (currentColor != 0) {
     // firstPixelHue = currentColor;
      currentColor = 0;
    }
    if (fast == false) {
      if (firstPixelHue != 0) {
        currentColor = firstPixelHue;
      }
      break;
    }
    // strip.rainbow() can take a single argument (first pixel hue) or
    // optionally a few extras: number of rainbow repetitions (default 1),
    // saturation and value (brightness) (both 0-255, similar to the
    // ColorHSV() function, default 255), and a true/false flag for whether
    // to apply gamma correction to provide 'truer' colors (default true).
    strip.rainbow(firstPixelHue);
    // Above line is equivalent to:
    // strip.rainbow(firstPixelHue, 1, 255, 255, true);
    strip.show();  // Update strip with new contents
    delay(wait);   // Pause for a moment
  }
}


// // Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;           // First pixel starts at red (hue 0)
  for (int a = 0; a < 30; a++) {   // Repeat 30 times...
    for (int b = 0; b < 3; b++) {  //  'b' counts from 0 to 2...
      if (false == fast) {
        break;
      }
      strip.clear();  //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for (int c = b; c < strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int hue = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue));  // hue -> RGB
        strip.setPixelColor(c, color);                        // Set pixel 'c' to value 'color'
      }
      strip.show();                 // Update strip with new contents
      delay(wait);                  // Pause for a moment
      firstPixelHue += 65536 / 90;  // One cycle of color wheel over 90 frames
    }
  }
}
