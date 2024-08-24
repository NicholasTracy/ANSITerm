/*
 * ANSITerm Library - Arduino ANSI Terminal Control Library
 * https://github.com/NicholasTracy/ANSITerm
 * 
 * Author: Nicholas Tracy (2024)
 * https://github.com/NicholasTracy
 *
 * File: GraphicsExample.ino
 * 
 * Description:
 * This example simulates a simple top-down view game similar to classic Zelda or Pokemon games.
 * The player can move in all directions and shoot projectiles. Enemies also move in all directions
 * and attempt to attack the player. The game features rooms with walls, enemies, and basic collision detection.
 * 
 * License:
 * This library is licensed under the GNU General Public License v3 (GPLv3).
 * You are free to use, modify, and distribute this library, provided that you comply with 
 * the terms of the GPLv3. There are no warranties, expressed or implied, provided with 
 * this software.
 * 
 * For more details, see the full license at: https://www.gnu.org/licenses/gpl-3.0.en.html
 * 
 */

#include <ANSITerm.h>

// Initialize ANSITerm with the Serial stream
ANSITerm terminal(Serial);

// Game constants
const char PLAYER_CHAR = ANSI_SMILEY;
const char WALL_CHAR = ANSI_BLOCK_HALF;
const char ENEMY_CHAR = ANSI_SPADE;
const char PROJECTILE_CHAR = '*';

const int SCREEN_WIDTH = 40;
const int SCREEN_HEIGHT = 20;

enum Direction { NONE, UP, DOWN, LEFT, RIGHT };

// Player state
int playerX = 10;
int playerY = 10;
Direction playerDirection = NONE;
bool isAlive = true;

// Enemy state
int enemyX = 30;
int enemyY = 5;
Direction enemyDirection = NONE;

// Projectiles
int playerProjectileX = -1;
int playerProjectileY = -1;
Direction playerProjectileDirection = NONE;

int enemyProjectileX = -1;
int enemyProjectileY = -1;
Direction enemyProjectileDirection = NONE;

// Function declarations
void drawScreen();
void movePlayer(char direction);
void moveEnemy();
void shootPlayerProjectile();
void shootEnemyProjectile();
void checkCollisions();
void updateProjectiles();
bool isCollision(int x, int y);

void setup() {
    // Begin serial communication at 9600 baud
    Serial.begin(9600);

    // Wait for a successful serial connection
    while (!Serial) {
        ; // Wait for serial port to connect. Needed for native USB port only boards
    }

    // Initialize the terminal with color names
    terminal.begin(true, true, true, true, "white", "black"); // white text on black background

    // Draw initial screen
    drawScreen();
}

void loop() {
    // Main game loop
    if (isAlive && Serial.available()) {
        char input = Serial.read();
        movePlayer(input);
        moveEnemy();
        updateProjectiles();
        shootPlayerProjectile();
        shootEnemyProjectile();
        checkCollisions();
        drawScreen();
    }
}

// Function to draw the screen
void drawScreen() {
    terminal.clearScreen();

    // Draw walls (create rooms)
    terminal.setTextColor("blue");
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        terminal.writeTextAt(0, i, String(WALL_CHAR).c_str());
        terminal.writeTextAt(SCREEN_HEIGHT - 1, i, String(WALL_CHAR).c_str());
    }
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        terminal.writeTextAt(i, 0, String(WALL_CHAR).c_str());
        terminal.writeTextAt(i, SCREEN_WIDTH - 1, String(WALL_CHAR).c_str());
    }
    for (int i = 5; i < SCREEN_HEIGHT - 5; i++) {
        terminal.writeTextAt(i, 20, String(WALL_CHAR).c_str());
    }

    // Draw player
    if (isAlive) {
        terminal.setTextColor("green");
        terminal.writeTextAt(playerY, playerX, String(PLAYER_CHAR).c_str());
    }

    // Draw enemy
    terminal.setTextColor("red");
    terminal.writeTextAt(enemyY, enemyX, String(ENEMY_CHAR).c_str());

    // Draw player projectile if it exists
    if (playerProjectileX >= 0 && playerProjectileY >= 0) {
        terminal.setTextColor("yellow");
        terminal.writeTextAt(playerProjectileY, playerProjectileX, String(PROJECTILE_CHAR).c_str());
    }

    // Draw enemy projectile if it exists
    if (enemyProjectileX >= 0 && enemyProjectileY >= 0) {
        terminal.setTextColor("magenta");
        terminal.writeTextAt(enemyProjectileY, enemyProjectileX, String(PROJECTILE_CHAR).c_str());
    }
}

// Function to move the player
void movePlayer(char direction) {
    int newX = playerX;
    int newY = playerY;

    switch (direction) {
        case 'w':
            newY = playerY - 1;
            playerDirection = UP;
            break;
        case 'a':
            newX = playerX - 1;
            playerDirection = LEFT;
            break;
        case 's':
            newY = playerY + 1;
            playerDirection = DOWN;
            break;
        case 'd':
            newX = playerX + 1;
            playerDirection = RIGHT;
            break;
        case ' ':
            // Fire projectile
            shootPlayerProjectile();
            return;
    }

    if (!isCollision(newX, newY)) {
        playerX = newX;
        playerY = newY;
    }
}

// Function to move the enemy (simple AI that moves towards the player)
void moveEnemy() {
    int deltaX = playerX - enemyX;
    int deltaY = playerY - enemyY;

    if (abs(deltaX) > abs(deltaY)) {
        if (deltaX > 0) {
            if (!isCollision(enemyX + 1, enemyY)) {
                enemyX++;
                enemyDirection = RIGHT;
            }
        } else {
            if (!isCollision(enemyX - 1, enemyY)) {
                enemyX--;
                enemyDirection = LEFT;
            }
        }
    } else {
        if (deltaY > 0) {
            if (!isCollision(enemyX, enemyY + 1)) {
                enemyY++;
                enemyDirection = DOWN;
            }
        } else {
            if (!isCollision(enemyX, enemyY - 1)) {
                enemyY--;
                enemyDirection = UP;
            }
        }
    }
}

// Function to shoot a projectile from the player
void shootPlayerProjectile() {
    if (playerProjectileX < 0 && playerProjectileY < 0) { // Only shoot if there's no existing projectile
        playerProjectileX = playerX;
        playerProjectileY = playerY;
        playerProjectileDirection = playerDirection;
    }
}

// Function to shoot a projectile from the enemy
void shootEnemyProjectile() {
    if (enemyProjectileX < 0 && enemyProjectileY < 0) { // Only shoot if there's no existing projectile
        enemyProjectileX = enemyX;
        enemyProjectileY = enemyY;
        enemyProjectileDirection = enemyDirection;
    }
}

// Function to update the position of projectiles
void updateProjectiles() {
    // Update player projectile position
    if (playerProjectileX >= 0 && playerProjectileY >= 0) {
        switch (playerProjectileDirection) {
            case UP:
                playerProjectileY--;
                break;
            case DOWN:
                playerProjectileY++;
                break;
            case LEFT:
                playerProjectileX--;
                break;
            case RIGHT:
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

    // Update enemy projectile position
    if (enemyProjectileX >= 0 && enemyProjectileY >= 0) {
        switch (enemyProjectileDirection) {
            case UP:
                enemyProjectileY--;
                break;
            case DOWN:
                enemyProjectileY++;
                break;
            case LEFT:
                enemyProjectileX--;
                break;
            case RIGHT:
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

// Function to check for collisions
void checkCollisions() {
    // Check if the player's projectile hits the enemy
    if (playerProjectileX == enemyX && playerProjectileY == enemyY) {
        terminal.setTextColor("red");
        terminal.writeTextAt(enemyY, enemyX, "X"); // Mark enemy's death
        terminal.writeTextAt(SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2 - 5, "YOU WIN!");
        isAlive = false;
        return;
    }

    // Check if the enemy's projectile hits the player
    if (enemyProjectileX == playerX && enemyProjectileY == playerY) {
        terminal.setTextColor("red");
        terminal.writeTextAt(playerY, playerX, "X"); // Mark player's death
        terminal.writeTextAt(SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2 - 5, "GAME OVER");
        isAlive = false;
        return;
    }
}

// Function to check for wall collisions
bool isCollision(int x, int y) {
    if (x <= 0 || x >= SCREEN_WIDTH - 1 || y <= 0 || y >= SCREEN_HEIGHT - 1) {
        return true;
    }
    if (x == 20 && y >= 5 && y <= SCREEN_HEIGHT - 5) {
        return true;
    }
    return false;
}