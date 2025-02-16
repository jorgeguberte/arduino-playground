# Arduino Playground
This is a series of experiments with Arduino Uno.

Schematics and videos coming soon™.


## Accelerometer (MPU6050)
Connect the MPU6050 to the Arduino using the following connections:

Conecte o MPU6050 ao Arduino usando as seguintes conexões:

| MPU6050 | Arduino |
|---------|---------|
| VCC     | 5V      |
| GND     | GND     |
| SCL     | A5      |
| SDA     | A4      |i

Use a 100nF ceramic capacitor (104) between VCC and GND to filter out noise.

Use um capacitor cerâmico de 100nF (104) entre VCC e GND para filtrar o ruído.



## Joystick + OLED display

Connect the Arduino 5V and GND to the breadboard power rail.
Conecte o 5V e o GND do Arduino ao trilho de alimentação da protoboard.

Connect the OLED display to the Arduino using the following connections:
Conecte o display OLED ao Arduino usando as seguintes conexões:

| OLED | Arduino    |
|------|------------|
| VCC  | 5V         |
| GND  | GND        |
| SDA  | A4         |
| SCL  | A5         |

Connect the joystick to the Arduino using the following connections:
Conecte o joystick ao Arduino usando as seguintes conexões:

| Joystick | Arduino    |
|----------|------------|
| VCC      | 5V         |
| GND      | GND        |
| VRX      | A0         |
| VRY      | A1         |
| SW       | 2          |

A square object is displayed on the OLED screen. The object can be moved around the screen using the joystick.

Um quadrado é exibido na tela OLED. O objeto pode ser movido pela tela usando o joystick.





## Block Destroyer
Same connections as the Joystick + OLED display.

It's a simple block destroyer game; press the joystick button to shoot the blocks.

## 3D Cube
Same connections as the Joystick + OLED display.

It's a 3D cube that responds to the joystick movement around the X and Y axes.


## Tic Tac Toe - Single Player
Same connections as the Joystick + OLED display.

It's a Tic Tac Toe game that uses circles and squares; move the joystick to select the cell and press the joystick button to place your mark.








