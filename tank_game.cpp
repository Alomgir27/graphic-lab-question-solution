#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <chrono>
#include <algorithm>
#include <cstdio>
#include <ctime>

// Define M_PI if not defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Window dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Game constants
const int TANK_SIZE = 30;
const int TANK_SPEED = 3;
const int BULLET_SPEED = 5;
const int BULLET_SIZE = 5;
const int ENEMY_SIZE = 30;
const int ENEMY_SPEED = 1;
const int MAX_ENEMIES = 10;
const int SPAWN_DELAY = 3000; // ms
const int FIRE_DELAY = 500;   // ms



// Game objects
struct Tank {
    float x, y;
    float angle;
    bool moveForward, moveBackward;
    bool rotateLeft, rotateRight;
    int lastFireTime;
    bool autoFire;  // New flag for automatic firing
    
    Tank(float _x, float _y)
        : x(_x), y(_y), angle(0),
          moveForward(false), moveBackward(false),
          rotateLeft(false), rotateRight(false),
          lastFireTime(-FIRE_DELAY), autoFire(false) {}
};

struct Bullet {
    float x, y;
    float dx, dy;
    bool active;
    
    Bullet(float _x, float _y, float _dx, float _dy)
        : x(_x), y(_y), dx(_dx), dy(_dy), active(true) {}
};

struct Enemy {
    float x, y;
    bool active;
    
    Enemy(float _x, float _y)
        : x(_x), y(_y), active(true) {}
};

// Game state
Tank player(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
std::vector<Bullet> bullets;
std::vector<Enemy> enemies;
int lastSpawnTime = 0;
int score = 0;
bool gameOver = false;
std::random_device rd;
std::mt19937 gen(rd());

// --- Add muzzle flash state ---
bool showMuzzleFlash = false;
int muzzleFlashTime = 0;
const int MUZZLE_FLASH_DURATION = 100; // ms

// Helper function to get current time in milliseconds
int getCurrentTime() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

// Helper function to generate random float in range [min, max]
float randomFloat(float min, float max) {
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

// Function to check collision between two circles
bool checkCollision(float x1, float y1, float r1, float x2, float y2, float r2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float distance = sqrt(dx * dx + dy * dy);
    return distance < (r1 + r2);
}

// Draw a tank
void drawTank(float x, float y, float angle, bool isPlayer) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glRotatef(angle * 180.0f / M_PI, 0.0f, 0.0f, 1.0f);
    
    // Draw tank body
    if (isPlayer) {
        glColor3f(0.0f, 0.7f, 0.0f); // Green for player
    } else {
        glColor3f(0.8f, 0.0f, 0.0f); // Red for enemy
    }
    
    glBegin(GL_QUADS);
    glVertex2f(-TANK_SIZE / 2, -TANK_SIZE / 2);
    glVertex2f(TANK_SIZE / 2, -TANK_SIZE / 2);
    glVertex2f(TANK_SIZE / 2, TANK_SIZE / 2);
    glVertex2f(-TANK_SIZE / 2, TANK_SIZE / 2);
    glEnd();
    
    // Draw tank cannon
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(-2.0f, 0.0f);
    glVertex2f(2.0f, 0.0f);
    glVertex2f(2.0f, TANK_SIZE);
    glVertex2f(-2.0f, TANK_SIZE);
    glEnd();
    
    // --- Draw muzzle flash for player ---
    if (isPlayer && showMuzzleFlash) {
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow flash
        glBegin(GL_TRIANGLES);
        glVertex2f(-6.0f, TANK_SIZE);
        glVertex2f(6.0f, TANK_SIZE);
        glVertex2f(0.0f, TANK_SIZE + 18.0f);
        glEnd();
    }
    
    glPopMatrix();
}

// Draw a bullet
void drawBullet(float x, float y) {
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow
    
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    
    glBegin(GL_QUADS);
    glVertex2f(-BULLET_SIZE / 2, -BULLET_SIZE / 2);
    glVertex2f(BULLET_SIZE / 2, -BULLET_SIZE / 2);
    glVertex2f(BULLET_SIZE / 2, BULLET_SIZE / 2);
    glVertex2f(-BULLET_SIZE / 2, BULLET_SIZE / 2);
    glEnd();
    
    glPopMatrix();
}

// Render text using GLUT bitmap fonts
void renderText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

// Spawn a new enemy at a random position
void spawnEnemy() {
    if (enemies.size() >= MAX_ENEMIES) return;
    
    // Choose a random edge of the screen
    int edge = rand() % 4;
    float x, y;
    
    switch (edge) {
        case 0: // Top
            x = randomFloat(0, WINDOW_WIDTH);
            y = 0;
            break;
        case 1: // Right
            x = WINDOW_WIDTH;
            y = randomFloat(0, WINDOW_HEIGHT);
            break;
        case 2: // Bottom
            x = randomFloat(0, WINDOW_WIDTH);
            y = WINDOW_HEIGHT;
            break;
        case 3: // Left
            x = 0;
            y = randomFloat(0, WINDOW_HEIGHT);
            break;
    }
    
    enemies.push_back(Enemy(x, y));
}

// Fire a bullet from the player tank
void fireBullet() {
    int currentTime = getCurrentTime();
    if (currentTime - player.lastFireTime < FIRE_DELAY) return;
    player.lastFireTime = currentTime;
    
    // Calculate bullet spawn position at the end of the tank's cannon
    float bulletX = player.x + cos(player.angle) * (TANK_SIZE + 5);
    float bulletY = player.y + sin(player.angle) * (TANK_SIZE + 5);
    
    bullets.push_back(Bullet(
        bulletX, bulletY,
        cos(player.angle) * BULLET_SPEED,
        sin(player.angle) * BULLET_SPEED
    ));
    // Show muzzle flash
    showMuzzleFlash = true;
    muzzleFlashTime = currentTime;
    std::cout << "fireBullet() called: bullet at (" << bulletX << ", " << bulletY << ")\n";
}

// Update game state
void update() {
    if (gameOver) return;
    
    // Move player tank
    if (player.moveForward) {
        player.x += cos(player.angle) * TANK_SPEED;
        player.y += sin(player.angle) * TANK_SPEED;
    }
    
    if (player.moveBackward) {
        player.x -= cos(player.angle) * TANK_SPEED;
        player.y -= sin(player.angle) * TANK_SPEED;
    }
    
    // Rotate player tank
    if (player.rotateLeft) {
        player.angle -= 0.05f;
    }
    
    if (player.rotateRight) {
        player.angle += 0.05f;
    }
    
    // Auto-fire if enabled
    if (player.autoFire) {
        int currentTime = getCurrentTime();
        if (currentTime - player.lastFireTime >= FIRE_DELAY) {
            fireBullet();
        }
    }
    
    // Keep player within bounds
    if (player.x < 0) player.x = 0;
    if (player.x > WINDOW_WIDTH) player.x = WINDOW_WIDTH;
    if (player.y < 0) player.y = 0;
    if (player.y > WINDOW_HEIGHT) player.y = WINDOW_HEIGHT;
    
    // Update bullets
    for (auto& bullet : bullets) {
        if (!bullet.active) continue;
        
        bullet.x += bullet.dx;
        bullet.y += bullet.dy;
        
        // Check if bullet is out of bounds
        if (bullet.x < 0 || bullet.x > WINDOW_WIDTH ||
            bullet.y < 0 || bullet.y > WINDOW_HEIGHT) {
            bullet.active = false;
            continue;
        }
        
        // Check for collision with enemies
        for (auto& enemy : enemies) {
            if (!enemy.active) continue;
            
            if (checkCollision(
                    bullet.x, bullet.y, BULLET_SIZE / 2,
                    enemy.x, enemy.y, ENEMY_SIZE / 2)) {
                bullet.active = false;
                enemy.active = false;
                score++;
                break;
            }
        }
    }
    
    // Remove inactive bullets
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return !b.active; }),
        bullets.end()
    );
    
    // Remove inactive enemies
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](const Enemy& e) { return !e.active; }),
        enemies.end()
    );
    
    // Update enemies - move towards player
    for (auto& enemy : enemies) {
        float dx = player.x - enemy.x;
        float dy = player.y - enemy.y;
        float distance = sqrt(dx * dx + dy * dy);
        
        if (distance > 0) {
            enemy.x += (dx / distance) * ENEMY_SPEED;
            enemy.y += (dy / distance) * ENEMY_SPEED;
        }
        
        // Check for collision with player
        if (checkCollision(
                enemy.x, enemy.y, ENEMY_SIZE / 2,
                player.x, player.y, TANK_SIZE / 2)) {
            gameOver = true;
            break;
        }
    }
    
    // Spawn new enemies
    if (getCurrentTime() - lastSpawnTime > SPAWN_DELAY) {
        lastSpawnTime = getCurrentTime();
        spawnEnemy();
    }
    
    // --- Hide muzzle flash after duration ---
    int currentTime = getCurrentTime();
    if (showMuzzleFlash && currentTime - muzzleFlashTime > MUZZLE_FLASH_DURATION) {
        showMuzzleFlash = false;
    }
}

// Display callback function
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Draw player tank
    drawTank(player.x, player.y, player.angle, true);
    
    // Draw bullets
    for (const auto& bullet : bullets) {
        if (bullet.active) {
            drawBullet(bullet.x, bullet.y);
        }
    }
    
    // Draw enemies
    for (const auto& enemy : enemies) {
        if (enemy.active) {
            drawTank(enemy.x, enemy.y, 0.0f, false);
        }
    }
    
    // --- Enhanced score display ---
    glColor3f(1.0f, 1.0f, 0.0f);
    char scoreText[50];
    sprintf(scoreText, "Score: %d", score);
    glPushMatrix();
    glScalef(2.0f, 2.0f, 1.0f); // Make text larger
    renderText(5, 15, scoreText);
    glPopMatrix();
    
    // Draw game over text if game is over
    if (gameOver) {
        glColor3f(1.0f, 0.0f, 0.0f);
        renderText(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2, "GAME OVER");
        
        glColor3f(1.0f, 1.0f, 1.0f);
        renderText(WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 + 30, "Press R to restart");
    }
    
    glutSwapBuffers();
}

// Reshape callback function
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
}

// Timer callback function
void timer(int value) {
    update();
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // ~60 FPS
}

// Reset game state
void resetGame() {
    player = Tank(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    bullets.clear();
    enemies.clear();
    score = 0;
    gameOver = false;
    lastSpawnTime = getCurrentTime();
}

// Special key callback function (for arrow keys)
void specialKeyDown(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            player.moveForward = true;
            break;
        case GLUT_KEY_DOWN:
            player.moveBackward = true;
            break;
        case GLUT_KEY_LEFT:
            player.rotateLeft = true;
            break;
        case GLUT_KEY_RIGHT:
            player.rotateRight = true;
            break;
    }
}

// Special key up callback function
void specialKeyUp(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            player.moveForward = false;
            break;
        case GLUT_KEY_DOWN:
            player.moveBackward = false;
            break;
        case GLUT_KEY_LEFT:
            player.rotateLeft = false;
            break;
        case GLUT_KEY_RIGHT:
            player.rotateRight = false;
            break;
    }
}

// Key down callback function
void keyDown(unsigned char key, int x, int y) {
    if (key == 'f' || key == 'F' || key == ' ') { // F key or Space
        fireBullet(); // Fire immediately on key press
    } else if (key == 'a' || key == 'A') { // A key for toggling autofire
        player.autoFire = !player.autoFire;
        std::cout << "Auto-fire " << (player.autoFire ? "enabled" : "disabled") << std::endl;
    } else if (key == 'r' || key == 'R') {
        resetGame();
    } else if (key == 27) { // ESC key
        exit(0);
    }
}

// Key up callback function
void keyUp(unsigned char key, int x, int y) {
    // No action needed for space bar
}

// Initialize OpenGL settings
void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    lastSpawnTime = getCurrentTime();
    
    // Seed random number generator
    srand(time(nullptr));
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Tank Game");
    
    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);
    glutSpecialFunc(specialKeyDown);
    glutSpecialUpFunc(specialKeyUp);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    
    init();
    
    std::cout << "Tank Game Controls:" << std::endl;
    std::cout << "Arrow Keys: Move tank" << std::endl;
    std::cout << "F: Fire" << std::endl;
    std::cout << "A: Toggle auto-fire" << std::endl;
    std::cout << "R: Restart game" << std::endl;
    std::cout << "ESC: Exit" << std::endl;
    
    glutMainLoop();
    return 0;
} 