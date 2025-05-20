#include <GL/glut.h>
#include <iostream>
#include <cmath>
using namespace std;

// Window dimensions
const int WINDOW_SIZE = 500;

// Original shape coordinates (a simple square)
float original[4][2] = {
    {-30, -30}, // Bottom left
    {30, -30},  // Bottom right
    {30, 30},   // Top right
    {-30, -30}  // Back to bottom left to complete the shape
};

// Draw coordinate axes
void drawAxes() {
    glColor3f(0.5, 0.5, 0.5); // Gray
    glLineWidth(1.0);
    glBegin(GL_LINES);
        // X axis
        glVertex2f(-100.0f, 0.0f);
        glVertex2f(100.0f, 0.0f);
        // Y axis
        glVertex2f(0.0f, -100.0f);
        glVertex2f(0.0f, 100.0f);
    glEnd();
}

// Draw the original shape
void drawOriginalShape() {
    glColor3f(0.0, 0.0, 0.0); // Black
    glLineWidth(2.0);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 4; i++) {
        glVertex2f(original[i][0], original[i][1]);
    }
    glEnd();
}

// Translation transformation
void drawTranslatedShape(float tx, float ty) {
    glColor3f(1.0, 0.0, 0.0); // Red
    glLineWidth(2.0);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 4; i++) {
        // Simple translation: add translation values to coordinates
        float x = original[i][0] + tx;
        float y = original[i][1] + ty;
        glVertex2f(x, y);
    }
    glEnd();
}

// Scaling transformation
void drawScaledShape(float sx, float sy) {
    glColor3f(0.0, 1.0, 0.0); // Green
    glLineWidth(2.0);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 4; i++) {
        // Simple scaling: multiply coordinates by scale factors
        float x = original[i][0] * sx;
        float y = original[i][1] * sy;
        glVertex2f(x, y);
    }
    glEnd();
}

// Rotation transformation
void drawRotatedShape(float angle) {
    glColor3f(0.0, 0.0, 1.0); // Blue
    glLineWidth(2.0);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 4; i++) {
        // Convert angle to radians
        float rad = angle * M_PI / 180.0;
        // Simple rotation formulas
        float x = original[i][0] * cos(rad) - original[i][1] * sin(rad);
        float y = original[i][0] * sin(rad) + original[i][1] * cos(rad);
        glVertex2f(x, y);
    }
    glEnd();
}

// Reflection transformation (around X-axis)
void drawReflectedShapeX() {
    glColor3f(1.0, 0.0, 1.0); // Magenta
    glLineWidth(2.0);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 4; i++) {
        // Reflection around X-axis: flip the y-coordinate
        float x = original[i][0];
        float y = -original[i][1];
        glVertex2f(x, y);
    }
    glEnd();
}

// Reflection transformation (around Y-axis)
void drawReflectedShapeY() {
    glColor3f(0.0, 1.0, 1.0); // Cyan
    glLineWidth(2.0);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 4; i++) {
        // Reflection around Y-axis: flip the x-coordinate
        float x = -original[i][0];
        float y = original[i][1];
        glVertex2f(x, y);
    }
    glEnd();
}

// Shearing transformation (X-direction)
void drawShearedShapeX(float shx) {
    glColor3f(1.0, 0.5, 0.0); // Orange
    glLineWidth(2.0);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 4; i++) {
        // Shearing in X-direction
        float x = original[i][0] + shx * original[i][1];
        float y = original[i][1];
        glVertex2f(x, y);
    }
    glEnd();
}

// Shearing transformation (Y-direction)
void drawShearedShapeY(float shy) {
    glColor3f(0.5, 0.0, 0.5); // Purple
    glLineWidth(2.0);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 4; i++) {
        // Shearing in Y-direction
        float x = original[i][0];
        float y = original[i][1] + shy * original[i][0];
        glVertex2f(x, y);
    }
    glEnd();
}

// Display function for translation window
void displayTranslation() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawAxes();
    drawOriginalShape();
    drawTranslatedShape(50, 20); // Translate by (50, 20)
    glFlush();
}

// Display function for scaling window
void displayScaling() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawAxes();
    drawOriginalShape();
    drawScaledShape(1.5, 0.5); // Scale by (1.5, 0.5)
    glFlush();
}

// Display function for rotation window
void displayRotation() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawAxes();
    drawOriginalShape();
    drawRotatedShape(45); // Rotate by 45 degrees
    glFlush();
}

// Display function for reflection around X-axis window
void displayReflectionX() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawAxes();
    drawOriginalShape();
    drawReflectedShapeX();
    glFlush();
}

// Display function for reflection around Y-axis window
void displayReflectionY() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawAxes();
    drawOriginalShape();
    drawReflectedShapeY();
    glFlush();
}

// Display function for X-shearing window
void displayShearingX() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawAxes();
    drawOriginalShape();
    drawShearedShapeX(0.5); // Shear in X-direction with factor 0.5
    glFlush();
}

// Display function for Y-shearing window
void displayShearingY() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawAxes();
    drawOriginalShape();
    drawShearedShapeY(0.5); // Shear in Y-direction with factor 0.5
    glFlush();
}

// Setup viewing
void setupGL() {
    glClearColor(1.0, 1.0, 1.0, 1.0); // White background
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-100.0, 100.0, -100.0, 100.0); 
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
    
    // Create Translation window
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Translation Transformation");
    setupGL();
    glutDisplayFunc(displayTranslation);
    
    // Create Scaling window
    glutInitWindowPosition(WINDOW_SIZE + 70, 50);
    glutCreateWindow("Scaling Transformation");
    setupGL();
    glutDisplayFunc(displayScaling);
    
    // Create Rotation window
    glutInitWindowPosition(50, WINDOW_SIZE + 70);
    glutCreateWindow("Rotation Transformation");
    setupGL();
    glutDisplayFunc(displayRotation);
    
    // Create Reflection X window
    glutInitWindowPosition(WINDOW_SIZE + 70, WINDOW_SIZE + 70);
    glutCreateWindow("Reflection around X-axis");
    setupGL();
    glutDisplayFunc(displayReflectionX);
    
    // Create Reflection Y window
    glutInitWindowPosition(50, 2 * WINDOW_SIZE + 90);
    glutCreateWindow("Reflection around Y-axis");
    setupGL();
    glutDisplayFunc(displayReflectionY);
    
    // Create Shearing X window
    glutInitWindowPosition(WINDOW_SIZE + 70, 2 * WINDOW_SIZE + 90);
    glutCreateWindow("X-Shearing Transformation");
    setupGL();
    glutDisplayFunc(displayShearingX);
    
    // Create Shearing Y window
    glutInitWindowPosition(50, 3 * WINDOW_SIZE + 110);
    glutCreateWindow("Y-Shearing Transformation");
    setupGL();
    glutDisplayFunc(displayShearingY);
    
    glutMainLoop();
    return 0;
} 