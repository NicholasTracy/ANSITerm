/*
 * ANSITerm — GraphicExample
 * https://github.com/NicholasTracy/ANSITerm
 *
 * What you learn: Timed game ticks independent of input, incremental terminal updates
 * (erase old cell / restore walls / draw sprites only where needed), and non-blocking Serial keys.
 *
 * Hardware: USB serial at 9600 baud; UTF-8 terminal. Mouse reporting stays off (see ButtonsExample).
 *
 * License: LGPL-3.0 — see LICENSE.txt in the library root.
 */

#include <ANSITerm.h>

ANSITerm terminal(Serial);

const char* PLAYER_CHAR = ANSI_SMILEY;
const char* WALL_CHAR = ANSI_BLOCK_MEDIUM;
// ASCII so terminals like PuTTY (without Unicode skull glyphs) still show an opponent.
const char* ENEMY_CHAR = "&";
const char* PROJECTILE_CHAR = "*";

const int SCREEN_WIDTH = 40;
const int SCREEN_HEIGHT = 20;
const unsigned long TICK_MS = 65;

// Opponent moves once per N simulation ticks (projectiles still every tick).
const uint8_t ENEMY_MOVE_EVERY_N_TICKS = 3;
// How often the enemy tries the "other" axis first (weaving / not bee-lining).
const uint8_t ENEMY_INDIRECT_MOVE_PERCENT = 42;
const int8_t ENEMY_WANDER_CLAMP = 10;

enum Direction : uint8_t { DIR_NONE, DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT };

enum class EndState : uint8_t { Playing, Won, Lost };

EndState endState = EndState::Playing;
bool endBannerDrawn = false;

int playerX = 10;
int playerY = 10;
Direction playerDirection = DIR_NONE;
Direction playerLastTravelDir = DIR_UP;
bool isAlive = true;

int enemyX = 30;
int enemyY = 5;
Direction enemyDirection = DIR_NONE;
Direction enemyLastTravelDir = DIR_RIGHT;

int playerProjectileX = -1;
int playerProjectileY = -1;
Direction playerProjectileDirection = DIR_NONE;

int enemyProjectileX = -1;
int enemyProjectileY = -1;
Direction enemyProjectileDirection = DIR_NONE;

uint8_t enemyFireCooldown = 25;

uint8_t enemyMoveTickCounter = 0;
int8_t enemyWanderX = 0;
int8_t enemyWanderY = 0;

// Last rendered snapshot (for dirty updates)
int prevPX = -1;
int prevPY = -1;
int prevEX = -1;
int prevEY = -1;
int prevPPx = -1;
int prevPPy = -1;
int prevEPx = -1;
int prevEPy = -1;

unsigned long lastSimMs = 0;
bool keyHandledThisLoop = false;

void flushSerialInput();
void resetGameState();
void fullRedraw();
void restoreCell(int x, int y);
bool isWallCell(int x, int y);
void syncSnapshot();
void renderIncremental();
void processKeys();
bool movePlayer(char direction);
bool spawnPlayerProjectile();
bool spawnEnemyProjectile();
void simulationTick();
void updateProjectiles();
void checkCollisions();
bool isCollision(int x, int y);
bool cellBlocksPlayer(int x, int y);
bool cellBlocksEnemy(int x, int y);

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ;
    }

    terminal.begin(true, true, false, true, "white", "black");
    flushSerialInput();

    resetGameState();
    fullRedraw();
    lastSimMs = millis();
}

void loop() {
    if (terminal.reconnected()) {
        terminal.begin(true, true, false, true, "white", "black");
        flushSerialInput();
        resetGameState();
        fullRedraw();
        lastSimMs = millis();
    }

    keyHandledThisLoop = false;
    processKeys();

    const bool playing = (endState == EndState::Playing);
    const unsigned long now = millis();
    const bool tickDue = playing && (now - lastSimMs >= TICK_MS);

    if (tickDue) {
        lastSimMs = now;
        simulationTick();
    }

    if (tickDue || keyHandledThisLoop || (endState != EndState::Playing && !endBannerDrawn)) {
        renderIncremental();
    }
}

void flushSerialInput() {
    while (Serial.available() > 0) {
        Serial.read();
    }
}

void resetGameState() {
    playerX = 10;
    playerY = 10;
    enemyX = 30;
    enemyY = 5;
    playerDirection = DIR_NONE;
    playerLastTravelDir = DIR_UP;
    enemyDirection = DIR_NONE;
    enemyLastTravelDir = DIR_RIGHT;
    playerProjectileX = playerProjectileY = -1;
    enemyProjectileX = enemyProjectileY = -1;
    playerProjectileDirection = DIR_NONE;
    enemyProjectileDirection = DIR_NONE;
    isAlive = true;
    endState = EndState::Playing;
    endBannerDrawn = false;
    enemyFireCooldown = 25;
    enemyMoveTickCounter = 0;
    enemyWanderX = 0;
    enemyWanderY = 0;
    prevPX = prevPY = prevEX = prevEY = -1;
    prevPPx = prevPPy = prevEPx = prevEPy = -1;
}

bool isWallCell(int x, int y) {
    if (x <= 0 || x >= SCREEN_WIDTH - 1 || y <= 0 || y >= SCREEN_HEIGHT - 1) {
        return true;
    }
    if (x == 20 && y >= 5 && y <= SCREEN_HEIGHT - 5) {
        return true;
    }
    return false;
}

void restoreCell(int x, int y) {
    const uint8_t row = static_cast<uint8_t>(y);
    const uint8_t col = static_cast<uint8_t>(x);
    if (isWallCell(x, y)) {
        terminal.setTextColor("blue");
        terminal.writeTextAt(row, col, WALL_CHAR);
    } else {
        terminal.deleteAtPosition(row, col);
    }
}

void fullRedraw() {
    terminal.clearScreen();

    terminal.setTextColor("blue");
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        terminal.writeTextAt(0, i, WALL_CHAR);
        terminal.writeTextAt(SCREEN_HEIGHT - 1, i, WALL_CHAR);
    }
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        terminal.writeTextAt(i, 0, WALL_CHAR);
        terminal.writeTextAt(i, SCREEN_WIDTH - 1, WALL_CHAR);
    }
    for (int i = 5; i < SCREEN_HEIGHT - 5; i++) {
        terminal.writeTextAt(i, 20, WALL_CHAR);
    }

    if (isAlive) {
        terminal.setTextColor("green");
        terminal.writeTextAt(playerY, playerX, PLAYER_CHAR);
    }

    terminal.setTextColor("red");
    terminal.writeTextAt(enemyY, enemyX, ENEMY_CHAR);

    if (playerProjectileX >= 0) {
        terminal.setTextColor("yellow");
        terminal.writeTextAt(playerProjectileY, playerProjectileX, PROJECTILE_CHAR);
    }
    if (enemyProjectileX >= 0) {
        terminal.setTextColor("magenta");
        terminal.writeTextAt(enemyProjectileY, enemyProjectileX, PROJECTILE_CHAR);
    }

    syncSnapshot();
}

void syncSnapshot() {
    prevPX = playerX;
    prevPY = playerY;
    prevEX = enemyX;
    prevEY = enemyY;
    prevPPx = playerProjectileX;
    prevPPy = playerProjectileY;
    prevEPx = enemyProjectileX;
    prevEPy = enemyProjectileY;
}

void renderIncremental() {
    if (endState != EndState::Playing && !endBannerDrawn) {
        if (prevPPx >= 0) {
            restoreCell(prevPPx, prevPPy);
        }
        if (prevEPx >= 0) {
            restoreCell(prevEPx, prevEPy);
        }

        terminal.setTextColor("red");
        if (endState == EndState::Won) {
            terminal.writeTextAt(enemyY, enemyX, "X");
            terminal.writeTextAt(SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2 - 5, "YOU WIN!");
        } else {
            terminal.writeTextAt(playerY, playerX, "X");
            terminal.writeTextAt(SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2 - 5, "GAME OVER");
        }
        terminal.setTextColor("white");
        terminal.writeTextAt(SCREEN_HEIGHT / 2 + 1, SCREEN_WIDTH / 2 - 10, "Press R to restart");

        endBannerDrawn = true;
        syncSnapshot();
        return;
    }

    if (endState != EndState::Playing) {
        return;
    }

    if (isAlive && (prevPX != playerX || prevPY != playerY)) {
        restoreCell(prevPX, prevPY);
        terminal.setTextColor("green");
        terminal.writeTextAt(playerY, playerX, PLAYER_CHAR);
    }

    if (prevEX != enemyX || prevEY != enemyY) {
        restoreCell(prevEX, prevEY);
        terminal.setTextColor("red");
        terminal.writeTextAt(enemyY, enemyX, ENEMY_CHAR);
    }

    if (prevPPx >= 0
        && (playerProjectileX < 0 || prevPPx != playerProjectileX || prevPPy != playerProjectileY)) {
        restoreCell(prevPPx, prevPPy);
    }
    if (playerProjectileX >= 0) {
        terminal.setTextColor("yellow");
        terminal.writeTextAt(playerProjectileY, playerProjectileX, PROJECTILE_CHAR);
    }

    if (prevEPx >= 0
        && (enemyProjectileX < 0 || prevEPx != enemyProjectileX || prevEPy != enemyProjectileY)) {
        restoreCell(prevEPx, prevEPy);
    }
    if (enemyProjectileX >= 0) {
        terminal.setTextColor("magenta");
        terminal.writeTextAt(enemyProjectileY, enemyProjectileX, PROJECTILE_CHAR);
    }

    syncSnapshot();
}

void processKeys() {
    while (Serial.available()) {
        const char input = static_cast<char>(Serial.read());
        if (input == '\r' || input == '\n') {
            continue;
        }

        if (endState != EndState::Playing) {
            if (input == 'r' || input == 'R') {
                resetGameState();
                fullRedraw();
                lastSimMs = millis();
                keyHandledThisLoop = true;
            }
            continue;
        }

        if (movePlayer(input)) {
            keyHandledThisLoop = true;
        }
    }
}

bool movePlayer(char direction) {
    int newX = playerX;
    int newY = playerY;

    switch (direction) {
        case 'w':
        case 'W':
            newY = playerY - 1;
            playerDirection = DIR_UP;
            break;
        case 'a':
        case 'A':
            newX = playerX - 1;
            playerDirection = DIR_LEFT;
            break;
        case 's':
        case 'S':
            newY = playerY + 1;
            playerDirection = DIR_DOWN;
            break;
        case 'd':
        case 'D':
            newX = playerX + 1;
            playerDirection = DIR_RIGHT;
            break;
        case ' ':
            return spawnPlayerProjectile();
        default:
            return false;
    }

    if (cellBlocksPlayer(newX, newY)) {
        return false;
    }
    playerX = newX;
    playerY = newY;
    playerLastTravelDir = playerDirection;
    return true;
}

void simulationTick() {
    enemyMoveTickCounter++;
    if (enemyMoveTickCounter >= ENEMY_MOVE_EVERY_N_TICKS) {
        enemyMoveTickCounter = 0;
        moveEnemy();
    }

    updateProjectiles();

    if (enemyProjectileX < 0 && enemyFireCooldown > 0) {
        enemyFireCooldown--;
    }
    if (enemyProjectileX < 0 && enemyFireCooldown == 0) {
        if (spawnEnemyProjectile()) {
            enemyFireCooldown = 22;
        } else {
            enemyFireCooldown = 4;
        }
    }

    checkCollisions();
}

static bool enemyTryMove(int nx, int ny, Direction d) {
    if (cellBlocksEnemy(nx, ny)) {
        return false;
    }
    enemyX = nx;
    enemyY = ny;
    enemyDirection = d;
    enemyLastTravelDir = d;
    return true;
}

static void enemyGreedyStep(int dx, int dy, bool horizontalFirst) {
    if (horizontalFirst) {
        if (dx > 0 && enemyTryMove(enemyX + 1, enemyY, DIR_RIGHT)) {
            return;
        }
        if (dx < 0 && enemyTryMove(enemyX - 1, enemyY, DIR_LEFT)) {
            return;
        }
        if (dy > 0 && enemyTryMove(enemyX, enemyY + 1, DIR_DOWN)) {
            return;
        }
        if (dy < 0 && enemyTryMove(enemyX, enemyY - 1, DIR_UP)) {
            return;
        }
    } else {
        if (dy > 0 && enemyTryMove(enemyX, enemyY + 1, DIR_DOWN)) {
            return;
        }
        if (dy < 0 && enemyTryMove(enemyX, enemyY - 1, DIR_UP)) {
            return;
        }
        if (dx > 0 && enemyTryMove(enemyX + 1, enemyY, DIR_RIGHT)) {
            return;
        }
        if (dx < 0 && enemyTryMove(enemyX - 1, enemyY, DIR_LEFT)) {
            return;
        }
    }
}

static void clampEnemyWander() {
    if (enemyWanderX > ENEMY_WANDER_CLAMP) {
        enemyWanderX = ENEMY_WANDER_CLAMP;
    } else if (enemyWanderX < -ENEMY_WANDER_CLAMP) {
        enemyWanderX = -ENEMY_WANDER_CLAMP;
    }
    if (enemyWanderY > ENEMY_WANDER_CLAMP) {
        enemyWanderY = ENEMY_WANDER_CLAMP;
    } else if (enemyWanderY < -ENEMY_WANDER_CLAMP) {
        enemyWanderY = -ENEMY_WANDER_CLAMP;
    }
}

void moveEnemy() {
    enemyWanderX += static_cast<int8_t>(random(3) - 1);
    enemyWanderY += static_cast<int8_t>(random(3) - 1);
    clampEnemyWander();

    const int tx = playerX + enemyWanderX;
    const int ty = playerY + enemyWanderY;
    const int dx = tx - enemyX;
    const int dy = ty - enemyY;

    if (dx == 0 && dy == 0) {
        enemyWanderX += static_cast<int8_t>(random(5) - 2);
        enemyWanderY += static_cast<int8_t>(random(5) - 2);
        clampEnemyWander();
        return;
    }

    bool horizontalFirst = abs(dx) >= abs(dy);
    if (random(100) < ENEMY_INDIRECT_MOVE_PERCENT) {
        horizontalFirst = !horizontalFirst;
    }
    enemyGreedyStep(dx, dy, horizontalFirst);
}

bool spawnPlayerProjectile() {
    if (playerProjectileX >= 0 || playerProjectileY >= 0) {
        return false;
    }
    Direction dir = playerLastTravelDir;
    if (dir == DIR_NONE) {
        dir = DIR_UP;
    }
    int px = playerX;
    int py = playerY;
    switch (dir) {
        case DIR_UP:
            py--;
            break;
        case DIR_DOWN:
            py++;
            break;
        case DIR_LEFT:
            px--;
            break;
        case DIR_RIGHT:
            px++;
            break;
        default:
            return false;
    }
    if (cellBlocksPlayer(px, py)) {
        return false;
    }
    playerProjectileX = px;
    playerProjectileY = py;
    playerProjectileDirection = dir;
    return true;
}

bool spawnEnemyProjectile() {
    if (enemyProjectileX >= 0 || enemyProjectileY >= 0) {
        return false;
    }
    Direction dir = enemyLastTravelDir;
    if (dir == DIR_NONE) {
        dir = DIR_RIGHT;
    }
    int px = enemyX;
    int py = enemyY;
    switch (dir) {
        case DIR_UP:
            py--;
            break;
        case DIR_DOWN:
            py++;
            break;
        case DIR_LEFT:
            px--;
            break;
        case DIR_RIGHT:
            px++;
            break;
        default:
            return false;
    }
    if (cellBlocksEnemy(px, py)) {
        return false;
    }
    enemyProjectileX = px;
    enemyProjectileY = py;
    enemyProjectileDirection = dir;
    return true;
}

void updateProjectiles() {
    if (playerProjectileX >= 0) {
        switch (playerProjectileDirection) {
            case DIR_UP:
                playerProjectileY--;
                break;
            case DIR_DOWN:
                playerProjectileY++;
                break;
            case DIR_LEFT:
                playerProjectileX--;
                break;
            case DIR_RIGHT:
                playerProjectileX++;
                break;
            default:
                break;
        }

        if (isCollision(playerProjectileX, playerProjectileY)) {
            playerProjectileX = -1;
            playerProjectileY = -1;
        }
    }

    if (enemyProjectileX >= 0) {
        switch (enemyProjectileDirection) {
            case DIR_UP:
                enemyProjectileY--;
                break;
            case DIR_DOWN:
                enemyProjectileY++;
                break;
            case DIR_LEFT:
                enemyProjectileX--;
                break;
            case DIR_RIGHT:
                enemyProjectileX++;
                break;
            default:
                break;
        }

        if (isCollision(enemyProjectileX, enemyProjectileY)) {
            enemyProjectileX = -1;
            enemyProjectileY = -1;
        }
    }
}

void checkCollisions() {
    if (playerProjectileX >= 0 && playerProjectileX == enemyX && playerProjectileY == enemyY) {
        playerProjectileX = playerProjectileY = -1;
        endState = EndState::Won;
        return;
    }

    if (enemyProjectileX >= 0 && enemyProjectileX == playerX && enemyProjectileY == playerY && isAlive) {
        enemyProjectileX = enemyProjectileY = -1;
        isAlive = false;
        endState = EndState::Lost;
        return;
    }

    if (endState == EndState::Playing && isAlive && playerX == enemyX && playerY == enemyY) {
        isAlive = false;
        endState = EndState::Lost;
    }
}

bool isCollision(int x, int y) {
    return isWallCell(x, y);
}

bool cellBlocksPlayer(int x, int y) {
    if (isWallCell(x, y)) {
        return true;
    }
    return x == enemyX && y == enemyY;
}

bool cellBlocksEnemy(int x, int y) {
    if (isWallCell(x, y)) {
        return true;
    }
    return x == playerX && y == playerY;
}
