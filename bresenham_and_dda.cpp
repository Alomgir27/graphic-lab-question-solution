#include <GL/glut.h>
#include <iostream>
#include <cmath>
using namespace std;

// Fixed variable names
int x_start = -6, y_start = 13;
int x_end = 8, y_end = 2;

// Function to draw a line using DDA algorithm
void drawLineDDA(int x0, int y0, int x1, int y1) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    float x_inc = dx / (float)steps;
    float y_inc = dy / (float)steps;
    float x = x0;
    float y = y0;
    glBegin(GL_POINTS);
    for (int i = 0; i <= steps; i++) {
        glVertex2i(round(x), round(y));
        x += x_inc;
        y += y_inc;
    }
    glEnd();
}

// Function to draw a line using Bresenham's algorithm
void drawLineBresenham(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int x = x0;
    int y = y0;
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    glBegin(GL_POINTS);
    while (true) {
        glVertex2i(x, y);
        if (x == x1 && y == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x += sx; }
        if (e2 < dx) { err += dx; y += sy; }
    }
    glEnd();
}

// Display function for DDA window
void displayDDA() {
    glClear(GL_COLOR_BUFFER_BIT);
    // Draw axes
    glColor3f(0.0, 0.0, 0.0); // Black axes
    glLineWidth(1.0);
    glBegin(GL_LINES);
        glVertex2f(-10.0f, 0.0f); // X axis
        glVertex2f(10.0f, 0.0f);
        glVertex2f(0.0f, -5.0f); // Y axis
        glVertex2f(0.0f, 15.0f);
    glEnd();
    // Draw DDA line
    glColor3f(1.0, 0.0, 0.0); // Red for DDA
    glPointSize(5.0);
    drawLineDDA(x_start, y_start, x_end, y_end);
    glFlush();
}

// Display function for Bresenham window
void displayBresenham() {
    glClear(GL_COLOR_BUFFER_BIT);
    // Draw axes
    glColor3f(0.0, 0.0, 0.0); // Black axes
    glLineWidth(1.0);
    glBegin(GL_LINES);
        glVertex2f(-10.0f, 0.0f); // X axis
        glVertex2f(10.0f, 0.0f);
        glVertex2f(0.0f, -5.0f); // Y axis
        glVertex2f(0.0f, 15.0f);
    glEnd();
    // Draw Bresenham line
    glColor3f(0.0, 0.0, 1.0); // Blue for Bresenham
    glPointSize(5.0);
    drawLineBresenham(x_start, y_start, x_end, y_end);
    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    int win1 = glutCreateWindow("DDA Line Algorithm");
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-10.0, 10.0, -5.0, 15.0);
    glutDisplayFunc(displayDDA);

    glutInitWindowPosition(700, 100);
    int win2 = glutCreateWindow("Bresenham's Line Algorithm");
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-10.0, 10.0, -5.0, 15.0);
    glutDisplayFunc(displayBresenham);

    glutMainLoop();
    return 0;
}
