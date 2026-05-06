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
const char* WALL_CHAR = ANSI_BLOCK_HALF;
const char* ENEMY_CHAR = ANSI_SKULL;
const char PROJECTILE_CHAR = '*';

const int SCREEN_WIDTH = 40;
const int SCREEN_HEIGHT = 20;
const unsigned long TICK_MS = 65;

enum Direction : uint8_t { DIR_NONE, DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT };

enum class EndState : uint8_t { Playing, Won, Lost };

EndState endState = EndState::Playing;
bool endBannerDrawn = false;

int playerX = 10;
int playerY = 10;
Direction playerDirection = DIR_NONE;
bool isAlive = true;

int enemyX = 30;
int enemyY = 5;
Direction enemyDirection = DIR_NONE;

int playerProjectileX = -1;
int playerProjectileY = -1;
Direction playerProjectileDirection = DIR_NONE;

int enemyProjectileX = -1;
int enemyProjectileY = -1;
Direction enemyProjectileDirection = DIR_NONE;

uint8_t enemyFireCooldown = 25;

// Last rendered snapshot (for dirty updates)
int prevPX = -1;
int prevPY = -1;
int prevEX = -1;
int prevEY = -1;
int prevPPx = -1;
int prevPPy = -1;
int prevEPx = -1;
int prevEPy = -1;

unsigned long nextTickMs = 0;
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

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ;
    }

    terminal.begin(true, true, false, true, "white", "black");
    flushSerialInput();

    resetGameState();
    fullRedraw();
    nextTickMs = millis() + TICK_MS;
}

void loop() {
    if (terminal.reconnected()) {
        terminal.begin(true, true, false, true, "white", "black");
        flushSerialInput();
        resetGameState();
        fullRedraw();
        nextTickMs = millis() + TICK_MS;
    }

    keyHandledThisLoop = false;
    processKeys();

    const bool playing = (endState == EndState::Playing);
    const bool tickDue = playing && (static_cast<long>(millis() - nextTickMs) >= 0);

    if (tickDue) {
        nextTickMs = millis() + TICK_MS;
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
    enemyDirection = DIR_NONE;
    playerProjectileX = playerProjectileY = -1;
    enemyProjectileX = enemyProjectileY = -1;
    playerProjectileDirection = DIR_NONE;
    enemyProjectileDirection = DIR_NONE;
    isAlive = true;
    endState = EndState::Playing;
    endBannerDrawn = false;
    enemyFireCooldown = 25;
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
        terminal.writeTextAt(playerProjectileY, playerProjectileX, String(PROJECTILE_CHAR).c_str());
    }
    if (enemyProjectileX >= 0) {
        terminal.setTextColor("magenta");
        terminal.writeTextAt(enemyProjectileY, enemyProjectileX, String(PROJECTILE_CHAR).c_str());
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
        terminal.writeTextAt(playerProjectileY, playerProjectileX, String(PROJECTILE_CHAR).c_str());
    }

    if (prevEPx >= 0
        && (enemyProjectileX < 0 || prevEPx != enemyProjectileX || prevEPy != enemyProjectileY)) {
        restoreCell(prevEPx, prevEPy);
    }
    if (enemyProjectileX >= 0) {
        terminal.setTextColor("magenta");
        terminal.writeTextAt(enemyProjectileY, enemyProjectileX, String(PROJECTILE_CHAR).c_str());
    }

    syncSnapshot();
}

void processKeys() {
    while (Serial.available()) {
        const char input = static_cast<char>(Serial.read());
        if (input == '\r' || input == '\n') {
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

    if (isCollision(newX, newY)) {
        return false;
    }
    playerX = newX;
    playerY = newY;
    return true;
}

void simulationTick() {
    moveEnemy();

    updateProjectiles();

    if (enemyProjectileX < 0 && enemyFireCooldown > 0) {
        enemyFireCooldown--;
    }
    if (enemyProjectileX < 0 && enemyFireCooldown == 0) {
        if (spawnEnemyProjectile()) {
            enemyFireCooldown = 22;
        }
    }

    checkCollisions();
}

void moveEnemy() {
    const int deltaX = playerX - enemyX;
    const int deltaY = playerY - enemyY;

    if (abs(deltaX) > abs(deltaY)) {
        if (deltaX > 0) {
            if (!isCollision(enemyX + 1, enemyY)) {
                enemyX++;
                enemyDirection = DIR_RIGHT;
            }
        } else {
            if (!isCollision(enemyX - 1, enemyY)) {
                enemyX--;
                enemyDirection = DIR_LEFT;
            }
        }
    } else {
        if (deltaY > 0) {
            if (!isCollision(enemyX, enemyY + 1)) {
                enemyY++;
                enemyDirection = DIR_DOWN;
            }
        } else {
            if (!isCollision(enemyX, enemyY - 1)) {
                enemyY--;
                enemyDirection = DIR_UP;
            }
        }
    }
}

bool spawnPlayerProjectile() {
    if (playerProjectileX >= 0 || playerProjectileY >= 0) {
        return false;
    }
    Direction dir = playerDirection;
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
    if (isCollision(px, py)) {
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
    Direction dir = enemyDirection;
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
    if (isCollision(px, py)) {
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
    }
}

bool isCollision(int x, int y) {
    return isWallCell(x, y);
}
