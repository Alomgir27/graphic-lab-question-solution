#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <deque>
#include <random>
#include <ctime>

// Window dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Game constants
const int CELL_SIZE = 20;
const int GRID_WIDTH = WINDOW_WIDTH / CELL_SIZE;
const int GRID_HEIGHT = WINDOW_HEIGHT / CELL_SIZE;
const int INITIAL_SNAKE_LENGTH = 3;
const int GAME_SPEED = 150; // milliseconds between moves

// Direction enumeration
enum Direction {
    UP, RIGHT, DOWN, LEFT
};

// Snake segment structure
struct Segment {
    int x, y;
    
    // Default constructor
    Segment() : x(0), y(0) {}
    
    Segment(int _x, int _y) : x(_x), y(_y) {}
    
    bool operator==(const Segment& other) const {
        return x == other.x && y == other.y;
    }
};

// Game state
std::deque<Segment> snake;
Segment food(0, 0);
Direction direction = RIGHT;
Direction nextDirection = RIGHT;
bool gameOver = false;
int score = 0;
std::mt19937 rng(std::time(nullptr));

// Generate random position within grid
Segment generateRandomPosition() {
    std::uniform_int_distribution<int> distX(0, GRID_WIDTH - 1);
    std::uniform_int_distribution<int> distY(0, GRID_HEIGHT - 1);
    return Segment(distX(rng), distY(rng));
}

// Place food at a random position that's not on the snake
void placeFood() {
    Segment newFood;
    bool validPosition;
    
    do {
        validPosition = true;
        newFood = generateRandomPosition();
        
        // Check if food is on snake
        for (const auto& segment : snake) {
            if (segment == newFood) {
                validPosition = false;
                break;
            }
        }
    } while (!validPosition);
    
    food = newFood;
}

// Initialize game
void initGame() {
    // Clear snake
    snake.clear();
    
    // Create initial snake
    int startX = GRID_WIDTH / 2;
    int startY = GRID_HEIGHT / 2;
    
    for (int i = 0; i < INITIAL_SNAKE_LENGTH; i++) {
        snake.push_front(Segment(startX - i, startY));
    }
    
    // Set initial direction
    direction = RIGHT;
    nextDirection = RIGHT;
    
    // Place food
    placeFood();
    
    // Reset score
    score = 0;
    
    // Game is now running
    gameOver = false;
}

// Move snake
void moveSnake() {
    if (gameOver) return;
    
    // Update direction
    direction = nextDirection;
    
    // Get current head position
    Segment head = snake.front();
    
    // Calculate new head position
    Segment newHead = head;
    switch (direction) {
        case UP:
            newHead.y--;
            break;
        case RIGHT:
            newHead.x++;
            break;
        case DOWN:
            newHead.y++;
            break;
        case LEFT:
            newHead.x--;
            break;
    }
    
    // Check if snake hit wall
    if (newHead.x < 0 || newHead.x >= GRID_WIDTH ||
        newHead.y < 0 || newHead.y >= GRID_HEIGHT) {
        gameOver = true;
        return;
    }
    
    // Check if snake hit itself
    for (size_t i = 1; i < snake.size(); i++) {
        if (snake[i] == newHead) {
            gameOver = true;
            return;
        }
    }
    
    // Add new head
    snake.push_front(newHead);
    
    // Check if snake ate food
    if (newHead == food) {
        // Increase score
        score++;
        
        // Place new food
        placeFood();
    } else {
        // Remove tail if food was not eaten
        snake.pop_back();
    }
}

// Draw a cell on the grid
void drawCell(int x, int y, float r, float g, float b) {
    glColor3f(r, g, b);
    
    float pixelX = x * CELL_SIZE;
    float pixelY = y * CELL_SIZE;
    
    glBegin(GL_QUADS);
    glVertex2f(pixelX, pixelY);
    glVertex2f(pixelX + CELL_SIZE, pixelY);
    glVertex2f(pixelX + CELL_SIZE, pixelY + CELL_SIZE);
    glVertex2f(pixelX, pixelY + CELL_SIZE);
    glEnd();
    
    // Draw cell border
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(pixelX, pixelY);
    glVertex2f(pixelX + CELL_SIZE, pixelY);
    glVertex2f(pixelX + CELL_SIZE, pixelY + CELL_SIZE);
    glVertex2f(pixelX, pixelY + CELL_SIZE);
    glEnd();
}

// Render text using GLUT bitmap fonts
void renderText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

// Display callback function
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Draw snake
    for (size_t i = 0; i < snake.size(); i++) {
        const auto& segment = snake[i];
        
        if (i == 0) {
            // Draw head with different color
            drawCell(segment.x, segment.y, 0.0f, 0.8f, 0.0f);
        } else {
            // Draw body
            drawCell(segment.x, segment.y, 0.0f, 0.6f, 0.0f);
        }
    }
    
    // Draw food
    drawCell(food.x, food.y, 1.0f, 0.0f, 0.0f);
    
    // Draw score
    glColor3f(1.0f, 1.0f, 1.0f);
    char scoreText[50];
    sprintf(scoreText, "Score: %d", score);
    renderText(10, WINDOW_HEIGHT - 30, scoreText);
    
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
    moveSnake();
    glutPostRedisplay();
    
    // Call timer function again after delay
    glutTimerFunc(GAME_SPEED, timer, 0);
}

// Special key callback function (for arrow keys)
void specialKey(int key, int x, int y) {
    // Prevent 180-degree turns
    switch (key) {
        case GLUT_KEY_UP:
            if (direction != DOWN) {
                nextDirection = UP;
            }
            break;
        case GLUT_KEY_RIGHT:
            if (direction != LEFT) {
                nextDirection = RIGHT;
            }
            break;
        case GLUT_KEY_DOWN:
            if (direction != UP) {
                nextDirection = DOWN;
            }
            break;
        case GLUT_KEY_LEFT:
            if (direction != RIGHT) {
                nextDirection = LEFT;
            }
            break;
    }
}

// Keyboard callback function
void keyboard(unsigned char key, int x, int y) {
    if (key == 'r' || key == 'R') {
        // Restart game
        initGame();
    } else if (key == 27) { // ESC key
        exit(0);
    }
}

// Initialize OpenGL settings
void init() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    
    // Initialize game
    initGame();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Snake Game");
    
    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(GAME_SPEED, timer, 0);
    glutSpecialFunc(specialKey);
    glutKeyboardFunc(keyboard);
    
    init();
    
    std::cout << "Snake Game Controls:" << std::endl;
    std::cout << "Arrow Keys: Change direction" << std::endl;
    std::cout << "R: Restart game" << std::endl;
    std::cout << "ESC: Exit" << std::endl;
    
    glutMainLoop();
    return 0;
} 