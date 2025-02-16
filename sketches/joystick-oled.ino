/*
* Display a controllable object on an OLED screen using a joystick.
* The object is a 5x5 square that can be moved around the screen using the joystick.
* The joystick is connected to the Arduino via the following pins:
* - VRX: A0
* - VRY: A1
* - SW: 2
* - VCC: 5V (via board rail)
* - GND: GND (via board rail)
*
* The OLED screen is connected to the Arduino via the following pins:
* - SDA: A4
* - SCL: A5
* - VCC: 5V (via board rail)
* - GND: GND (via board rail)
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int VRX = A0;
const int VRY = A1;
const int SW = 2;

int read_VRX, read_VRY, read_button;
int x = SCREEN_WIDTH / 2;  // Start at center
int y = SCREEN_HEIGHT / 2;

void setup() {
  Serial.begin(9600);
  pinMode(VRX, INPUT);
  pinMode(VRY, INPUT);
  pinMode(SW, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Ensure address is 0x3C
    Serial.println(F("OLED initialization failed"));
    for (;;);
  }

  display.clearDisplay();
}

void loop() {
  read_VRX = analogRead(VRX);
  read_VRY = analogRead(VRY);
  read_button = digitalRead(SW);

  // Map joystick values (0-1023) to screen size (0-127 for x, 0-63 for y)
  x = map(read_VRX, 0, 1023, 0, SCREEN_WIDTH - 1);
  y = map(read_VRY, 0, 1023, 0, SCREEN_HEIGHT - 1);

  // Display the object
  display.clearDisplay();
  display.fillRect(x, y, 5, 5, SSD1306_WHITE);  // Draw a 5x5 square at (x, y)
  display.display();

  Serial.print("X: ");
  Serial.print(x);
  Serial.print("\tY: ");
  Serial.print(y);
  Serial.print("\tButton: ");
  Serial.println(read_button);

  delay(50);
}
