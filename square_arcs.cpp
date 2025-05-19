#include <GL/freeglut.h>
#include <iostream>
#include <cmath>
#include <vector>

// Window dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Struct to store square parameters
struct SquareArc {
    int centerX, centerY;
    int size;
    
    SquareArc(int cx, int cy, int s)
        : centerX(cx), centerY(cy), size(s) {}
};

// Square storage
std::vector<SquareArc> squares;
bool definingSquare = false;
int squareStage = 0;
int centerX, centerY, cornerX, cornerY;

// Function to plot a pixel
void drawPixel(int x, int y) {
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}

// Midpoint circle algorithm to draw an arc
void drawCircleArc(int centerX, int centerY, int radius, double startAngle, double endAngle) {
    // Convert angles to range [0, 2π]
    while (startAngle < 0) startAngle += 2 * M_PI;
    while (endAngle < 0) endAngle += 2 * M_PI;
    
    startAngle = fmod(startAngle, 2 * M_PI);
    endAngle = fmod(endAngle, 2 * M_PI);
    
    // If end angle is less than start angle, add 2π to end angle
    if (endAngle < startAngle) {
        endAngle += 2 * M_PI;
    }
    
    int x = 0;
    int y = radius;
    int p = 1 - radius;
    
    // Plot a point if it's within the arc range
    auto plotPoint = [&](int x, int y) {
        // Calculate angle of point
        double angle = atan2(y, x);
        if (angle < 0) angle += 2 * M_PI;
        
        // Check if angle is within range
        double angleCopy = angle;
        while (angleCopy < startAngle) angleCopy += 2 * M_PI;
        while (angleCopy > endAngle) angleCopy -= 2 * M_PI;
        
        bool inRange = (angleCopy >= startAngle && angleCopy <= endAngle);
        
        if (inRange) {
            drawPixel(centerX + x, centerY + y);
        }
    };
    
    // Draw the circle arc using 8-way symmetry
    while (x <= y) {
        plotPoint(x, y);
        plotPoint(y, x);
        plotPoint(-x, y);
        plotPoint(-y, x);
        plotPoint(-x, -y);
        plotPoint(-y, -x);
        plotPoint(x, -y);
        plotPoint(y, -x);
        
        x++;
        if (p < 0) {
            p += 2 * x + 1;
        } else {
            y--;
            p += 2 * (x - y) + 1;
        }
    }
}

// Draw a square with four arcs at the corners
void drawSquareWithArcs(int centerX, int centerY, int size) {
    int halfSize = size / 2;
    int cornerRadius = size / 4; // Radius of the arcs
    
    // Calculate corner positions
    int topLeftX = centerX - halfSize;
    int topLeftY = centerY + halfSize;
    
    int topRightX = centerX + halfSize;
    int topRightY = centerY + halfSize;
    
    int bottomLeftX = centerX - halfSize;
    int bottomLeftY = centerY - halfSize;
    
    int bottomRightX = centerX + halfSize;
    int bottomRightY = centerY - halfSize;
    
    // Draw the square outline (without corners)
    glBegin(GL_LINES);
    
    // Top line (excluding corners)
    glVertex2i(topLeftX + cornerRadius, topLeftY);
    glVertex2i(topRightX - cornerRadius, topRightY);
    
    // Right line (excluding corners)
    glVertex2i(topRightX, topRightY - cornerRadius);
    glVertex2i(bottomRightX, bottomRightY + cornerRadius);
    
    // Bottom line (excluding corners)
    glVertex2i(bottomLeftX + cornerRadius, bottomLeftY);
    glVertex2i(bottomRightX - cornerRadius, bottomRightY);
    
    // Left line (excluding corners)
    glVertex2i(topLeftX, topLeftY - cornerRadius);
    glVertex2i(bottomLeftX, bottomLeftY + cornerRadius);
    
    glEnd();
    
    // Draw the four arcs at the corners
    
    // Top-left corner arc
    drawCircleArc(
        topLeftX + cornerRadius, topLeftY - cornerRadius, // Arc center
        cornerRadius,
        M_PI, 3 * M_PI / 2 // Start at left (π), end at top (3π/2)
    );
    
    // Top-right corner arc
    drawCircleArc(
        topRightX - cornerRadius, topRightY - cornerRadius, // Arc center
        cornerRadius,
        3 * M_PI / 2, 2 * M_PI // Start at top (3π/2), end at right (2π)
    );
    
    // Bottom-right corner arc
    drawCircleArc(
        bottomRightX - cornerRadius, bottomRightY + cornerRadius, // Arc center
        cornerRadius,
        0, M_PI / 2 // Start at right (0), end at bottom (π/2)
    );
    
    // Bottom-left corner arc
    drawCircleArc(
        bottomLeftX + cornerRadius, bottomLeftY + cornerRadius, // Arc center
        cornerRadius,
        M_PI / 2, M_PI // Start at bottom (π/2), end at left (π)
    );
}

// Display callback function
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 1.0f, 1.0f);
    
    // Draw all saved squares with arcs
    for (const auto& square : squares) {
        drawSquareWithArcs(square.centerX, square.centerY, square.size);
    }
    
    // Draw the square being defined
    if (definingSquare && squareStage >= 2) {
        int size = sqrt(pow(centerX - cornerX, 2) + pow(centerY - cornerY, 2)) * 2;
        drawSquareWithArcs(centerX, centerY, size);
    }
    
    glutSwapBuffers();
}

// Calculate distance between two points
double distance(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

// Mouse callback function
void mouse(int button, int state, int x, int y) {
    // Adjust y coordinate (OpenGL origin is at bottom-left)
    y = WINDOW_HEIGHT - y;
    
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (!definingSquare) {
            // Start defining a new square
            definingSquare = true;
            squareStage = 1;
            centerX = x;
            centerY = y;
        } else {
            squareStage++;
            
            if (squareStage == 2) {
                // Define square size based on distance from center to corner
                cornerX = x;
                cornerY = y;
                int size = sqrt(pow(centerX - cornerX, 2) + pow(centerY - cornerY, 2)) * 2;
                
                // Complete the square definition
                squares.push_back(SquareArc(centerX, centerY, size));
                definingSquare = false;
                squareStage = 0;
            }
        }
        
        glutPostRedisplay();
    }
}

// Motion callback function
void motion(int x, int y) {
    // Adjust y coordinate (OpenGL origin is at bottom-left)
    y = WINDOW_HEIGHT - y;
    
    if (definingSquare && squareStage == 1) {
        cornerX = x;
        cornerY = y;
        glutPostRedisplay();
    }
}

// Keyboard callback function
void keyboard(unsigned char key, int x, int y) {
    if (key == 27) { // ESC key
        exit(0);
    } else if (key == 'c' || key == 'C') {
        // Clear all squares
        squares.clear();
        definingSquare = false;
        squareStage = 0;
        glutPostRedisplay();
    }
}

// Initialize OpenGL settings
void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glPointSize(2.0f);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Square with Four Arcs");
    
    // Register callbacks
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutPassiveMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    
    init();
    
    std::cout << "Square with Four Arcs Demo" << std::endl;
    std::cout << "1. Click to set the center of the square" << std::endl;
    std::cout << "2. Click to set the size of the square" << std::endl;
    std::cout << "Press 'C' to clear all squares" << std::endl;
    std::cout << "Press 'ESC' to exit" << std::endl;
    
    glutMainLoop();
    return 0;
} 