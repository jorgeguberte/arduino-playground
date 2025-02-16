#include <Wire.h>

const int MPU_ADDR = 0x68;
float accel_x, accel_y, accel_z;
float gyro_x, gyro_y, gyro_z;
float temp;

// Calibration offsets
float accel_offset_x, accel_offset_y, accel_offset_z;
float gyro_offset_x, gyro_offset_y, gyro_offset_z;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  // Initialize MPU-6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);  // PWR_MGMT_1
  Wire.write(0);     // Wake up
  Wire.endTransmission(true);

  // Configure sensor ranges
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1C);  // ACCEL_CONFIG
  Wire.write(0x00);  // ±2g
  Wire.endTransmission(true);

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1B);  // GYRO_CONFIG
  Wire.write(0x00);  // ±250°/s
  Wire.endTransmission(true);

  // Configure digital low-pass filter
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1A);  // CONFIG
  Wire.write(0x05);  // ~5Hz bandwidth
  Wire.endTransmission(true);

  delay(100);
  calibrateSensor();
  delay(100);
}

void calibrateSensor() {
  const int samples = 500;
  float ax = 0, ay = 0, az = 0;
  float gx = 0, gy = 0, gz = 0;

  for(int i = 0; i < samples; i++) {
    readRawData();
    ax += accel_x;
    ay += accel_y;
    az += accel_z;
    gx += gyro_x;
    gy += gyro_y;
    gz += gyro_z;
    delay(5);
  }

  accel_offset_x = ax / samples;
  accel_offset_y = ay / samples;
  accel_offset_z = (az / samples) - 1.0;  // Remove 1g from Z-axis

  gyro_offset_x = gx / samples;
  gyro_offset_y = gy / samples;
  gyro_offset_z = gz / samples;
}

void readRawData() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  if(Wire.endTransmission(false) != 0) {
    Serial.println("I2C Error");
    return;
  }

  if(Wire.requestFrom(MPU_ADDR, 14, true) != 14) {
    Serial.println("Incomplete I2C Data");
    return;
  }

  // Read and convert raw sensor data
  accel_x = (Wire.read() << 8 | Wire.read()) / 16384.0;
  accel_y = (Wire.read() << 8 | Wire.read()) / 16384.0;
  accel_z = (Wire.read() << 8 | Wire.read()) / 16384.0;
  
  temp = (Wire.read() << 8 | Wire.read()) / 340.0 + 36.53;
  
  gyro_x = (Wire.read() << 8 | Wire.read()) / 131.0;
  gyro_y = (Wire.read() << 8 | Wire.read()) / 131.0;
  gyro_z = (Wire.read() << 8 | Wire.read()) / 131.0;
}

void loop() {
  readRawData();

  // Apply calibration offsets
  accel_x -= accel_offset_x;
  accel_y -= accel_offset_y;
  accel_z -= accel_offset_z;
  
  gyro_x -= gyro_offset_x;
  gyro_y -= gyro_offset_y;
  gyro_z -= gyro_offset_z;

  // Print data
  Serial.print("AcX: "); Serial.print(accel_x, 2);
  Serial.print(" | AcY: "); Serial.print(accel_y, 2);
  Serial.print(" | AcZ: "); Serial.print(accel_z, 2);
  
  Serial.print(" | Tmp: "); Serial.print(temp, 2);
  
  Serial.print(" | GyX: "); Serial.print(gyro_x, 2);
  Serial.print(" | GyY: "); Serial.print(gyro_y, 2);
  Serial.print(" | GyZ: "); Serial.print(gyro_z, 2);
  
  Serial.println();
  delay(200);
}