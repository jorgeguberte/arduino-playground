#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pins
const int VRX = A0;
const int VRY = A1;
const int SW = 2;

// Game constants
#define PLAYER_SIZE 8
#define ENEMY_SIZE 6
#define BULLET_SPEED 3
#define ENEMY_ROWS 3
#define ENEMY_COLS 5
#define ENEMY_SPACING 20

// Game state
int playerX = SCREEN_WIDTH/2;
int playerSpeed = 3;
int bulletX = -1;
int bulletY = -1;
bool gameRunning = true;
unsigned long lastEnemyMove = 0;
int enemyDirection = 1;
int score = 0;

struct Enemy {
  int x;
  int y;
  bool active;
};
Enemy enemies[ENEMY_ROWS][ENEMY_COLS];

void setup() {
  Serial.begin(9600);
  pinMode(SW, INPUT_PULLUP);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED failed"));
    for(;;);
  }
  
  initializeGame();
}

void initializeGame() {
  // Initialize enemies grid
  for(int row = 0; row < ENEMY_ROWS; row++) {
    for(int col = 0; col < ENEMY_COLS; col++) {
      enemies[row][col] = {
        col * ENEMY_SPACING + 10,
        row * 15 + 10,
        true
      };
    }
  }
}

void loop() {
  if(gameRunning) {
    readInput();
    updateGame();
  }
  drawScreen();
  delay(33); // ~30 FPS
}

void readInput() {
  // Horizontal movement
  int joyX = analogRead(VRX);
  if(joyX < 300) playerX -= playerSpeed;
  if(joyX > 700) playerX += playerSpeed;
  playerX = constrain(playerX, 0, SCREEN_WIDTH - PLAYER_SIZE);

  // Shooting
  if(digitalRead(SW) == LOW && bulletY < 0) {
    bulletX = playerX + PLAYER_SIZE/2;
    bulletY = SCREEN_HEIGHT - PLAYER_SIZE;
  }
}

void updateGame() {
  // Move bullet
  if(bulletY > 0) {
    bulletY -= BULLET_SPEED;
    checkCollisions();
  }

  // Enemy movement
  if(millis() - lastEnemyMove > 1000) {
    moveEnemies();
    lastEnemyMove = millis();
  }
}

void moveEnemies() {
  // Move all active enemies
  for(int row = 0; row < ENEMY_ROWS; row++) {
    for(int col = 0; col < ENEMY_COLS; col++) {
      if(enemies[row][col].active) {
        enemies[row][col].x += 5 * enemyDirection;
        
        // Reverse direction if hitting edge
        if(enemies[row][col].x <= 0 || enemies[row][col].x >= SCREEN_WIDTH - ENEMY_SIZE) {
          enemyDirection *= -1;
          return;
        }
      }
    }
  }
}

void checkCollisions() {
  for(int row = 0; row < ENEMY_ROWS; row++) {
    for(int col = 0; col < ENEMY_COLS; col++) {
      if(enemies[row][col].active &&
         bulletX >= enemies[row][col].x &&
         bulletX <= enemies[row][col].x + ENEMY_SIZE &&
         bulletY <= enemies[row][col].y + ENEMY_SIZE) {
        enemies[row][col].active = false;
        bulletY = -1;
        score += 10;
        checkGameWin();
        return;
      }
    }
  }
}

void checkGameWin() {
  for(int row = 0; row < ENEMY_ROWS; row++) {
    for(int col = 0; col < ENEMY_COLS; col++) {
      if(enemies[row][col].active) return;
    }
  }
  gameRunning = false;
}

void drawScreen() {
  display.clearDisplay();
  
  // Draw player
  display.fillRect(playerX, SCREEN_HEIGHT - PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE, SSD1306_WHITE);
  
  // Draw bullet
  if(bulletY > 0) {
    display.drawPixel(bulletX, bulletY, SSD1306_WHITE);
  }
  
  // Draw enemies
  for(int row = 0; row < ENEMY_ROWS; row++) {
    for(int col = 0; col < ENEMY_COLS; col++) {
      if(enemies[row][col].active) {
        display.drawRect(
          enemies[row][col].x,
          enemies[row][col].y,
          ENEMY_SIZE,
          ENEMY_SIZE,
          SSD1306_WHITE
        );
      }
    }
  }
  
  // Draw score
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print("Score: ");
  display.print(score);
  
  if(!gameRunning) {
    display.setCursor(SCREEN_WIDTH/2 - 30, SCREEN_HEIGHT/2);
    display.print(score == ENEMY_ROWS*ENEMY_COLS*10 ? "YOU WIN!" : "GAME OVER");
  }
  
  display.display();
}