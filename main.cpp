#include <Encoder.h>       // Rotary encoder library
#include <Wire.h>          // I2C library (for MPU6050)
#include <MPU6050.h>       // MPU6050 library
#include <Ultrasonic.h>    // Ultrasonic sensor library (for HC-SR04)
#include <EEPROM.h>        // EEPROM library

// ======================
// Hardware Pin Assignments
// ======================
// Rotary Encoder pins
const int ENC_PIN_A = 2;
const int ENC_PIN_B = 3;

// Motor driver pins (assuming dual H-bridge for two motors)
const int ENA = 11;   // Left motor PWM
const int IN1 = 5;    // Left motor direction
const int IN2 = 4;
const int ENB = 10;   // Right motor PWM
const int IN3 = 6;    // Right motor direction
const int IN4 = 7;

// Ultrasonic sensor pins (3 sensors: front, left, right)
const int TRIG_FRONT = 8;
const int ECHO_FRONT = 9;
const int TRIG_LEFT  = A0;
const int ECHO_LEFT  = A1;
const int TRIG_RIGHT = A2;
const int ECHO_RIGHT = A3;

// ======================
// Maze & Competition Parameters (from Rule Book [])
// ======================
const int MAZE_SIZE = 10;   // 10 x 10 grid
const int CELL_SIZE_CM = 25;  // Each cell is 25cm x 25cm

// Define starting and goal positions.
// For example: starting at bottom-left (0,0) and goal at top-right (9,9)
const int START_X = 0;
const int START_Y = 0;
const int GOAL_X  = 9;
const int GOAL_Y  = 9;

// Maximum distance value for flood-fill propagation
const int MAX_DISTANCE = 255;
// Threshold (in cm) to consider a wall detected by an ultrasonic sensor
const int WALL_DISTANCE_CM = 15;  // Adjust as needed

// Encoder threshold for one cell travel (calibrate for 25cm per cell)
int cellDistanceTicks = 200; // Example value; adjust based on your setup

// ======================
// Global Variables for Maze Navigation
// ======================
int currentDirection = 0;  // Orientation: 0 = North, 1 = East, 2 = South, 3 = West
int posX = START_X, posY = START_Y;  // Robot's current cell position

// Maze mapping arrays
uint8_t distanceGrid[MAZE_SIZE][MAZE_SIZE];   // Flood-fill distances
uint8_t wallsGrid[MAZE_SIZE][MAZE_SIZE];       // Wall information bitfield per cell
// Wall bits: bit0 = North, bit1 = East, bit2 = South, bit3 = West

// ======================
// Sensor & Actuator Objects
// ======================
Encoder enc(ENC_PIN_A, ENC_PIN_B);
MPU6050 mpu;
float gyroZoffset = 0;
const float GYRO_LSB_DEG = 131.0;  // For ±250°/s

// Three ultrasonic sensors
Ultrasonic frontUS(TRIG_FRONT, ECHO_FRONT);
Ultrasonic leftUS(TRIG_LEFT, ECHO_LEFT);
Ultrasonic rightUS(TRIG_RIGHT, ECHO_RIGHT);

// EEPROM storage addresses
const int EEPROM_FLAG_ADDR = 0;              // Flag address to indicate stored maze data
const int EEPROM_MAZE_START_ADDR = 1;          // Maze data start address

// ======================
// Function: scanWalls()
// Reads the three ultrasonic sensors and updates wall information
// for the current cell. Since no rear sensor is provided, we assume no wall.
void scanWalls() {
  unsigned int dFront = frontUS.Ranging(CM);
  unsigned int dLeft  = leftUS.Ranging(CM);
  unsigned int dRight = rightUS.Ranging(CM);
  // For the rear, assume no wall (simulate with a large distance)
  unsigned int dBack = 100;
  
  uint8_t wallBits = 0;
  // Map sensor readings to absolute walls based on current orientation:
  // 0 = North, 1 = East, 2 = South, 3 = West
  switch(currentDirection) {
    case 0: // Facing North
      if(dFront < WALL_DISTANCE_CM) wallBits |= 0x01;  // North wall
      if(dRight < WALL_DISTANCE_CM) wallBits |= 0x02;  // East wall
      if(dBack  < WALL_DISTANCE_CM)  wallBits |= 0x04;  // South wall (not measured)
      if(dLeft  < WALL_DISTANCE_CM) wallBits |= 0x08;  // West wall
      break;
    case 1: // Facing East
      if(dFront < WALL_DISTANCE_CM) wallBits |= 0x02;  // East wall
      if(dRight < WALL_DISTANCE_CM) wallBits |= 0x04;  // South wall
      if(dBack  < WALL_DISTANCE_CM) wallBits |= 0x08;  // West wall
      if(dLeft  < WALL_DISTANCE_CM) wallBits |= 0x01;  // North wall
      break;
    case 2: // Facing South
      if(dFront < WALL_DISTANCE_CM) wallBits |= 0x04;  // South wall
      if(dRight < WALL_DISTANCE_CM) wallBits |= 0x08;  // West wall
      if(dBack  < WALL_DISTANCE_CM) wallBits |= 0x01;  // North wall
      if(dLeft  < WALL_DISTANCE_CM) wallBits |= 0x02;  // East wall
      break;
    case 3: // Facing West
      if(dFront < WALL_DISTANCE_CM) wallBits |= 0x08;  // West wall
      if(dRight < WALL_DISTANCE_CM) wallBits |= 0x01;  // North wall
      if(dBack  < WALL_DISTANCE_CM) wallBits |= 0x02;  // East wall
      if(dLeft  < WALL_DISTANCE_CM) wallBits |= 0x04;  // South wall
      break;
  }
  // Merge new wall data with any previously recorded info
  wallsGrid[posY][posX] |= wallBits;
  // Update neighboring cells reciprocally:
  if((wallBits & 0x01) && posY < MAZE_SIZE-1) wallsGrid[posY+1][posX] |= 0x04; // North wall => neighbor's South
  if((wallBits & 0x02) && posX < MAZE_SIZE-1) wallsGrid[posY][posX+1] |= 0x08; // East wall  => neighbor's West
  if((wallBits & 0x04) && posY > 0)          wallsGrid[posY-1][posX] |= 0x01; // South wall => neighbor's North
  if((wallBits & 0x08) && posX > 0)          wallsGrid[posY][posX-1] |= 0x02; // West wall  => neighbor's East
}

// ======================
// Function: computeDistances()
// Implements the flood-fill algorithm to update the distance grid from the goal.
void computeDistances(int targetX, int targetY) {
  // Initialize all cells to MAX_DISTANCE
  for (int y = 0; y < MAZE_SIZE; y++) {
    for (int x = 0; x < MAZE_SIZE; x++) {
      distanceGrid[y][x] = MAX_DISTANCE;
    }
  }
  distanceGrid[targetY][targetX] = 0;
  bool updated = true;
  while (updated) {
    updated = false;
    for (int y = 0; y < MAZE_SIZE; y++) {
      for (int x = 0; x < MAZE_SIZE; x++) {
        if (x == targetX && y == targetY) continue;
        uint8_t minNeighbor = MAX_DISTANCE;
        // Check neighbors (if no wall between current cell and neighbor)
        // North neighbor:
        if (!(wallsGrid[y][x] & 0x01) && y < MAZE_SIZE - 1) {
          if (distanceGrid[y+1][x] < minNeighbor) minNeighbor = distanceGrid[y+1][x];
        }
        // East neighbor:
        if (!(wallsGrid[y][x] & 0x02) && x < MAZE_SIZE - 1) {
          if (distanceGrid[y][x+1] < minNeighbor) minNeighbor = distanceGrid[y][x+1];
        }
        // South neighbor:
        if (!(wallsGrid[y][x] & 0x04) && y > 0) {
          if (distanceGrid[y-1][x] < minNeighbor) minNeighbor = distanceGrid[y-1][x];
        }
        // West neighbor:
        if (!(wallsGrid[y][x] & 0x08) && x > 0) {
          if (distanceGrid[y][x-1] < minNeighbor) minNeighbor = distanceGrid[y][x-1];
        }
        if (minNeighbor != MAX_DISTANCE && distanceGrid[y][x] != minNeighbor + 1) {
          distanceGrid[y][x] = minNeighbor + 1;
          updated = true;
        }
      }
    }
  }
}

// ======================
// Motor Control Functions
// ======================
void driveStop() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void driveForward(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void driveTurnLeft(int speed) {
  // In-place left turn: left motor backward, right motor forward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void driveTurnRight(int speed) {
  // In-place right turn: left motor forward, right motor backward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

// ======================
// Function: turnByDegrees()
// Uses the MPU6050 to perform a precise 90° turn. Positive degrees for right, negative for left.
void turnByDegrees(float degrees) {
  float target = fabs(degrees);
  // Start the appropriate turn
  if (degrees > 0) {
    driveTurnRight(baseSpeed);
  } else if (degrees < 0) {
    driveTurnLeft(baseSpeed);
  } else return;

  float accumulated = 0.0;
  unsigned long lastTime = micros();
  while (accumulated < target) {
    int16_t gz;
    mpu.getRotation(NULL, NULL, &gz);
    float rate = fabs(gz - gyroZoffset) / GYRO_LSB_DEG; // deg/sec
    unsigned long now = micros();
    float dt = (now - lastTime) / 1000000.0;
    lastTime = now;
    accumulated += rate * dt;
  }
  driveStop();
  // Update current orientation
  if (degrees > 0)
    currentDirection = (currentDirection + 1) % 4;
  else if (degrees < 0)
    currentDirection = (currentDirection + 3) % 4;
}

// ======================
// Function: moveForwardOneCell()
// Drives forward exactly one cell (25 cm) using encoder feedback.
void moveForwardOneCell(int speed) {
  enc.write(0);
  driveForward(speed);
  while (abs(enc.read()) < cellDistanceTicks) {
    // Optionally add gyro corrections here if needed.
  }
  driveStop();
  // Update cell position based on current direction:
  if (currentDirection == 0) posY += 1;       // North
  else if (currentDirection == 1) posX += 1;  // East
  else if (currentDirection == 2) posY -= 1;  // South
  else if (currentDirection == 3) posX -= 1;  // West
}

// ======================
// Function: decideAndMove()
// Chooses the next move using the flood-fill distances,
// preferring forward motion over turns when distances tie.
void decideAndMove(bool fastRun = false) {
  uint8_t currDist = distanceGrid[posY][posX];
  uint8_t distForward = MAX_DISTANCE, distLeft = MAX_DISTANCE;
  uint8_t distRight = MAX_DISTANCE, distBack = MAX_DISTANCE;
  
  // Check each neighbor based on currentDirection
  // For simplicity, the mapping is done using the current cell's wall bits
  // and the corresponding neighbor's distance value.
  // Here we assume that if a wall exists, that direction is not available.
  if (!(wallsGrid[posY][posX] & 0x01) && posY < MAZE_SIZE - 1) { // North neighbor available
    uint8_t d = distanceGrid[posY+1][posX];
    switch(currentDirection) {
      case 0: distForward = d; break;
      case 1: distLeft    = d; break;
      case 2: distBack    = d; break;
      case 3: distRight   = d; break;
    }
  }
  if (!(wallsGrid[posY][posX] & 0x02) && posX < MAZE_SIZE - 1) { // East neighbor
    uint8_t d = distanceGrid[posY][posX+1];
    switch(currentDirection) {
      case 0: distRight   = d; break;
      case 1: distForward = d; break;
      case 2: distLeft    = d; break;
      case 3: distBack    = d; break;
    }
  }
  if (!(wallsGrid[posY][posX] & 0x04) && posY > 0) { // South neighbor
    uint8_t d = distanceGrid[posY-1][posX];
    switch(currentDirection) {
      case 0: distBack    = d; break;
      case 1: distRight   = d; break;
      case 2: distForward = d; break;
      case 3: distLeft    = d; break;
    }
  }
  if (!(wallsGrid[posY][posX] & 0x08) && posX > 0) { // West neighbor
    uint8_t d = distanceGrid[posY][posX-1];
    switch(currentDirection) {
      case 0: distLeft    = d; break;
      case 1: distBack    = d; break;
      case 2: distRight   = d; break;
      case 3: distForward = d; break;
    }
  }
  
  // Determine minimum distance among available moves
  uint8_t minDist = currDist;
  if (distForward < minDist) minDist = distForward;
  if (distLeft < minDist)    minDist = distLeft;
  if (distRight < minDist)   minDist = distRight;
  if (distBack < minDist)    minDist = distBack;
  
  // Prefer forward > left > right > back
  if (distForward == minDist) {
    moveForwardOneCell(fastRun ? fastSpeed : baseSpeed);
  } else if (distLeft == minDist) {
    turnByDegrees(-90);
    moveForwardOneCell(fastRun ? fastSpeed : baseSpeed);
  } else if (distRight == minDist) {
    turnByDegrees(90);
    moveForwardOneCell(fastRun ? fastSpeed : baseSpeed);
  } else if (distBack == minDist) {
    turnByDegrees(90);
    turnByDegrees(90);
    moveForwardOneCell(fastRun ? fastSpeed : baseSpeed);
  }
}

// ======================
// Setup and Main Loop
// ======================
void setup() {
  Serial.begin(115200);
  
  // Initialize motor control pins
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  driveStop();
  
  // Setup ultrasonic sensor pins
  pinMode(TRIG_FRONT, OUTPUT);
  pinMode(ECHO_FRONT, INPUT);
  pinMode(TRIG_LEFT, OUTPUT);
  pinMode(ECHO_LEFT, INPUT);
  pinMode(TRIG_RIGHT, OUTPUT);
  pinMode(ECHO_RIGHT, INPUT);
  
  // Initialize MPU6050
  Wire.begin();
  mpu.initialize();
  if(mpu.testConnection()){
    Serial.println("MPU6050 connected");
  } else {
    Serial.println("MPU6050 connection failed");
  }
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
  
  // Gyro calibration
  long sum = 0;
  for (int i = 0; i < 200; i++) {
    int16_t gz;
    mpu.getRotation(NULL, NULL, &gz);
    sum += gz;
    delay(5);
  }
  gyroZoffset = sum / 200.0;
  Serial.print("Gyro Z offset: "); Serial.println(gyroZoffset);
  
  // Initialize maze mapping arrays
  for (int y = 0; y < MAZE_SIZE; y++) {
    for (int x = 0; x < MAZE_SIZE; x++) {
      wallsGrid[y][x] = 0;
      distanceGrid[y][x] = MAX_DISTANCE;
    }
  }
  
  // Set maze outer boundaries:
  for (int i = 0; i < MAZE_SIZE; i++) {
    wallsGrid[0][i]       |= 0x04; // South wall for bottom row
    wallsGrid[MAZE_SIZE-1][i] |= 0x01; // North wall for top row
    wallsGrid[i][0]       |= 0x08; // West wall for left column
    wallsGrid[i][MAZE_SIZE-1] |= 0x02; // East wall for right column
  }
  // Mark starting cell boundaries if needed (e.g. start at (0,0))
  wallsGrid[START_Y][START_X] |= 0x04; // Example: south wall at start
  wallsGrid[START_Y][START_X] |= 0x08; // Example: west wall at start
  
  // Set initial position and orientation
  posX = START_X;
  posY = START_Y;
  currentDirection = 0;  // Assume starting facing North
  
  // Load maze data from EEPROM if available
  if(EEPROM.read(EEPROM_FLAG_ADDR) == 0xA5) {
    for (int idx = 0; idx < MAZE_SIZE * MAZE_SIZE; idx++) {
      int cx = idx % MAZE_SIZE;
      int cy = idx / MAZE_SIZE;
      wallsGrid[cy][cx] = EEPROM.read(EEPROM_MAZE_START_ADDR + idx);
    }
    Serial.println("Loaded maze from EEPROM.");
    computeDistances(GOAL_X, GOAL_Y);
  } else {
    // Initialize distance grid with a simple Manhattan distance heuristic
    for (int y = 0; y < MAZE_SIZE; y++) {
      for (int x = 0; x < MAZE_SIZE; x++) {
        distanceGrid[y][x] = abs(GOAL_X - x) + abs(GOAL_Y - y);
      }
    }
  }
}

void loop() {
  static bool mazeSolved = false;
  static bool fastRun = false;
  
  if(!mazeSolved) {
    // Exploration phase: scan walls, update distances, and decide next move.
    scanWalls();
    computeDistances(GOAL_X, GOAL_Y);
    decideAndMove(false);
    if(posX == GOAL_X && posY == GOAL_Y) {
      mazeSolved = true;
      Serial.println("Goal reached! Exploration complete.");
      EEPROM.update(EEPROM_FLAG_ADDR, 0xA5);
      int idx = 0;
      for (int y = 0; y < MAZE_SIZE; y++) {
        for (int x = 0; x < MAZE_SIZE; x++) {
          EEPROM.update(EEPROM_MAZE_START_ADDR + idx, wallsGrid[y][x]);
          idx++;
        }
      }
      Serial.println("Maze data saved to EEPROM.");
      // Prepare for a fast run using the known maze.
      computeDistances(GOAL_X, GOAL_Y);
      posX = START_X; posY = START_Y;
      currentDirection = 0;
      fastRun = true;
    }
  } 
  else if(fastRun) {
    if(posX == GOAL_X && posY == GOAL_Y) {
      driveStop();
      Serial.println("Fast run complete!");
      fastRun = false;
      while(true) { delay(1000); }
    } else {
      decideAndMove(true);
    }
  }
  delay(5);
}