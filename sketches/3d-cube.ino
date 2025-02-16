#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Joystick pins
const int VRX = A0;
const int VRY = A1;
const int SW = 2;

// 3D Cube parameters
float angleX = 0;
float angleY = 0;
float angleZ = 0;
float rotationSpeed = 0.05;

// Cube vertices (centered at origin)
float cube[8][3] = {
  {-10, -10, -10},
  {10, -10, -10},
  {10, 10, -10},
  {-10, 10, -10},
  {-10, -10, 10},
  {10, -10, 10},
  {10, 10, 10},
  {-10, 10, 10}
};

// Cube edges
int edges[12][2] = {
  {0,1}, {1,2}, {2,3}, {3,0},
  {4,5}, {5,6}, {6,7}, {7,4},
  {0,4}, {1,5}, {2,6}, {3,7}
};

void setup() {
  Serial.begin(115200);
  pinMode(SW, INPUT_PULLUP);
  
  Serial.println("Initializing display...");
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED failed");
    while(1);
  }
  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  Serial.println("Ready!");
}

void loop() {
  // Debug output
  Serial.println("------ New Frame ------");
  Serial.print("VRX: "); Serial.print(analogRead(VRX));
  Serial.print(" VRY: "); Serial.println(analogRead(VRY));
  
  processInput();
  drawCube();
  delay(50);
}

void processInput() {
  int xVal = analogRead(VRX);
  int yVal = analogRead(VRY);
  
  // X-axis rotation
  if(yVal < 300) angleX += rotationSpeed;
  if(yVal > 700) angleX -= rotationSpeed;
  
  // Y-axis rotation
  if(xVal < 300) angleY += rotationSpeed;
  if(xVal > 700) angleY -= rotationSpeed;
}

void drawCube() {
  display.clearDisplay();
  
  float projected[8][2];
  
  // Project 3D points to 2D
  for(int i=0; i<8; i++) {
    // Rotate point
    float x = cube[i][0];
    float y = cube[i][1];
    float z = cube[i][2];
    
    // Rotation matrices
    // X-axis rotation
    float tempY = y*cos(angleX) - z*sin(angleX);
    float tempZ = y*sin(angleX) + z*cos(angleX);
    y = tempY;
    z = tempZ;
    
    // Y-axis rotation
    float tempX = x*cos(angleY) - z*sin(angleY);
    tempZ = x*sin(angleY) + z*cos(angleY);
    x = tempX;
    z = tempZ;
    
    // Project to 2D
    float scale = 150/(150 + z); // Simple perspective
    projected[i][0] = x * scale + SCREEN_WIDTH/2;
    projected[i][1] = y * scale + SCREEN_HEIGHT/2;
    
    Serial.print("Point "); Serial.print(i);
    Serial.print(": "); Serial.print(projected[i][0]);
    Serial.print(", "); Serial.println(projected[i][1]);
  }
  
  // Draw edges
  for(int i=0; i<12; i++) {
    int start = edges[i][0];
    int end = edges[i][1];
    
    display.drawLine(
      projected[start][0], projected[start][1],
      projected[end][0], projected[end][1],
      SSD1306_WHITE
    );
  }
  
  display.display();
}