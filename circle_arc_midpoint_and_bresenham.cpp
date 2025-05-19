#include <GL/glut.h>
#include <cmath>
#include <iostream>

// Window dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Draw a pixel
void drawPixel(int x, int y) {
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}

// Draw arc using midpoint circle algorithm
void drawCircleArc(int cx, int cy, int r, double sa, double ea) {
    // Normalize angles to [0, 2pi)
    while (sa < 0) sa += 2 * M_PI;
    while (ea < 0) ea += 2 * M_PI;
    sa = fmod(sa, 2 * M_PI);
    ea = fmod(ea, 2 * M_PI);
    if (ea < sa) ea += 2 * M_PI;

    auto plot = [&](int x, int y) {
        double angle = atan2(y, x);
        if (angle < 0) angle += 2 * M_PI;
        double a = angle;
        while (a < sa) a += 2 * M_PI;
        while (a > ea) a -= 2 * M_PI;
        if (a >= sa && a <= ea) drawPixel(cx + x, cy + y);
    };

    int x = 0, y = r, p = 1 - r;
    while (x <= y) {
        plot(x, y); plot(y, x); plot(-x, y); plot(-y, x);
        plot(-x, -y); plot(-y, -x); plot(x, -y); plot(y, -x);
        x++;
        if (p < 0) p += 2 * x + 1;
        else { y--; p += 2 * (x - y) + 1; }
    }
}

// Draw a full circle using Bresenham's algorithm
void drawBresenhamCircle(int cx, int cy, int r) {
    int x = 0, y = r;
    int d = 3 - 2 * r;
    while (x <= y) {
        drawPixel(cx + x, cy + y);
        drawPixel(cx - x, cy + y);
        drawPixel(cx + x, cy - y);
        drawPixel(cx - x, cy - y);
        drawPixel(cx + y, cy + x);
        drawPixel(cx - y, cy + x);
        drawPixel(cx + y, cy - x);
        drawPixel(cx - y, cy - x);
        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    // Static arc parameters
    int centerX = 400;
    int centerY = 300;
    int radius = 200;
    // -30 degrees to 15 degrees in radians
    double startAngle = -M_PI / 6; // -30 degrees
    double endAngle = M_PI / 12;   // 15 degrees

    // Draw axes (red)
    glColor3f(1, 0, 0);
    glBegin(GL_LINES);
    // X axis
    glVertex2i(centerX - radius - 20, centerY);
    glVertex2i(centerX + radius + 20, centerY);
    // Y axis
    glVertex2i(centerX, centerY - radius - 20);
    glVertex2i(centerX, centerY + radius + 20);
    glEnd();

    // Draw center point (green)
    glColor3f(0, 1, 0);
    glPointSize(8.0f);
    drawPixel(centerX, centerY);
    glPointSize(4.0f); // Restore point size

    // Draw arc (white)
    glColor3f(1, 1, 1);
    drawCircleArc(centerX, centerY, radius, startAngle, endAngle);

    // Draw full circle (blue, Bresenham)
    glColor3f(0, 0.5, 1);
    drawBresenhamCircle(centerX, centerY, radius);

    glutSwapBuffers();
    glFlush();
}

void displayBresenhamOnly() {
    glClear(GL_COLOR_BUFFER_BIT);
    int centerX = 400;
    int centerY = 300;
    int radius = 200;
    // Draw full circle (blue, Bresenham)
    glColor3f(0, 0.5, 1);
    drawBresenhamCircle(centerX, centerY, radius);
    glutSwapBuffers();
    glFlush();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void init() {
    glClearColor(0, 0, 0, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glPointSize(4.0f);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    int win1 = glutCreateWindow("Static Circle Arc (WSL Ready)");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    init();
    std::cout << "Static Circle Arc Demo (WSL Ready)\n";
    std::cout << "Arc center: (400, 300), radius: 200, start angle: -30 deg, end angle: 15 deg\n";

    // Create second window for Bresenham circle only
    glutInitWindowPosition(950, 100);
    int win2 = glutCreateWindow("Bresenham Circle Only");
    glutDisplayFunc(displayBresenhamOnly);
    glutReshapeFunc(reshape);
    init();

    glutMainLoop();
    return 0;
} 