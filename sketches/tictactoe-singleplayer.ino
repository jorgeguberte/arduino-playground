#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ===== Display Settings =====
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ===== Joystick Pins =====
const int VRX = A0;    // Joystick X-axis
const int VRY = A1;    // Joystick Y-axis
const int SW  = 2;     // Joystick button (using INPUT_PULLUP)

// Joystick dead zone thresholds for edge detection
const int deadLower = 450;
const int deadUpper = 550;

// ===== Game State Management =====
enum GameState {
  STATE_SPLASH,
  STATE_GAME,
  STATE_GAME_OVER
};
GameState currentState = STATE_SPLASH;

// ===== Tic Tac Toe Board =====
// Board cells: ' ' = empty, 'C' = circle, 'S' = square
char board[3][3];
int moveCount = 0;  // Even: circle; Odd: square

// Cursor (selected cell) position (starting at center)
int cursorRow = 1;
int cursorCol = 1;

// Flag for edge detection of joystick movement
bool joystickMoved = false;

// Button debounce variables
unsigned long lastDebounceTime = 0;
const int debounceDelay = 50;

// Function Prototypes
void showSplashScreen();
void newGame();
void drawGame();
void updateCursor();
char checkWin();
void showGameOverScreen(char result);

void setup() {
  Serial.begin(115200);
  pinMode(VRX, INPUT);
  pinMode(VRY, INPUT);
  pinMode(SW, INPUT_PULLUP); // Button pressed = LOW

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED initialization failed");
    while (true);
  }
  display.clearDisplay();
  currentState = STATE_SPLASH;
  showSplashScreen();
}

void loop() {
  switch (currentState) {
    case STATE_SPLASH:
      // Wait for button press to start new game
      if (digitalRead(SW) == LOW && (millis() - lastDebounceTime) > debounceDelay) {
        lastDebounceTime = millis();
        newGame();
        currentState = STATE_GAME;
      }
      break;

    case STATE_GAME:
      updateCursor();
      // Check if the button is pressed to place a move
      if (digitalRead(SW) == LOW && (millis() - lastDebounceTime) > debounceDelay) {
        lastDebounceTime = millis();
        if (board[cursorRow][cursorCol] == ' ') {
          board[cursorRow][cursorCol] = (moveCount % 2 == 0) ? 'C' : 'S';
          moveCount++;
          drawGame();

          // Check win condition after move
          char result = checkWin();
          if (result != ' ') {
            currentState = STATE_GAME_OVER;
            showGameOverScreen(result);
          }
        }
      }
      break;

    case STATE_GAME_OVER:
      delay(3000);
      currentState = STATE_SPLASH;
      showSplashScreen();
      break;
  }
  delay(50);
}

// Display a splash screen with title and instruction.
void showSplashScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.println("Tic Tac Toe");
  display.setTextSize(1);
  display.setCursor(20, 40);
  display.println("Press button to start");
  display.display();
}

// Initialize a new game.
void newGame() {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      board[i][j] = ' ';
    }
  }
  moveCount = 0;
  cursorRow = 1;
  cursorCol = 1;
  drawGame();
}

// Draw the Tic Tac Toe grid, moves, and highlight the current cursor.
void drawGame() {
  display.clearDisplay();

  // Draw grid lines
  display.drawLine(43, 0, 43, SCREEN_HEIGHT, SSD1306_WHITE);
  display.drawLine(86, 0, 86, SCREEN_HEIGHT, SSD1306_WHITE); // Vertical lines
  display.drawLine(0, 21, SCREEN_WIDTH, 21, SSD1306_WHITE);
  display.drawLine(0, 42, SCREEN_WIDTH, 42, SSD1306_WHITE);  // Horizontal lines

  // Draw moves on the board
  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {
      int cellX = col * 43;
      int cellY = row * 21;
      int centerX = cellX + 21;
      int centerY = cellY + 10;
      if (board[row][col] == 'C') {
        display.drawCircle(centerX, centerY, 8, SSD1306_WHITE);
      } else if (board[row][col] == 'S') {
        display.drawRect(centerX - 8, centerY - 8, 16, 16, SSD1306_WHITE);
      }
    }
  }

  // Highlight the selected cell
  int highlightX = cursorCol * 43;
  int highlightY = cursorRow * 21;
  display.drawRect(highlightX, highlightY, 43, 21, SSD1306_WHITE);

  display.display();
}

// Update the cursor based on joystick readings with edge-detection.
void updateCursor() {
  int xVal = analogRead(VRX);
  int yVal = analogRead(VRY);
  
  // If joystick is in the dead zone, reset the edge-detect flag.
  if (xVal >= deadLower && xVal <= deadUpper && yVal >= deadLower && yVal <= deadUpper) {
    joystickMoved = false;
    return;
  }
  
  // If a move hasn't been processed yet, update the cursor.
  if (!joystickMoved) {
    if (xVal < deadLower && cursorCol > 0) {
      cursorCol--;
    } else if (xVal > deadUpper && cursorCol < 2) {
      cursorCol++;
    }
    if (yVal < deadLower && cursorRow > 0) {
      cursorRow--;
    } else if (yVal > deadUpper && cursorRow < 2) {
      cursorRow++;
    }
    drawGame();
    joystickMoved = true;
  }
}

// Check for win or tie conditions.
// Returns 'C' if circles win, 'S' if squares win, 'T' for tie, or ' ' if the game continues.
char checkWin() {
  // Check rows and columns.
  for (int i = 0; i < 3; i++) {
    if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2])
      return board[i][0];
    if (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i])
      return board[0][i];
  }
  // Check diagonals.
  if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2])
    return board[0][0];
  if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0])
    return board[0][2];

  // Check for a tie.
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (board[i][j] == ' ')
        return ' '; // Game continues.
    }
  }
  return 'T'; // Tie if no empty cell.
}

// Display the game over screen with the result.
void showGameOverScreen(char result) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 10);
  if (result == 'T') {
    display.println("Tie Game!");
  } else if (result == 'C') {
    display.println("Circle Win!");
  } else if (result == 'S') {
    display.println("Square Win!");
  }
  display.setTextSize(1);
  display.setCursor(10, 40);
  display.println("Returning to menu...");
  display.display();
}