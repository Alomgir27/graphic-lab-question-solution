#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <cmath>

#define M_PI 3.14159265358979323846

// Window dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Viewport dimensions
const int VIEWPORT_X = 400;
const int VIEWPORT_Y = 0;
const int VIEWPORT_WIDTH = 400;
const int VIEWPORT_HEIGHT = 600;

// Region codes for Cohen-Sutherland algorithm
const int INSIDE = 0; // 0000
const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000

// Point structure
struct Point {
    int x, y;
    
    Point(int _x, int _y) : x(_x), y(_y) {}
    
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

// Line structure
struct Line {
    Point start, end;
    
    Line(Point _start, Point _end) : start(_start), end(_end) {}
};

// Polygon structure (renamed to MyPolygon to avoid conflict with Windows API)
struct MyPolygon {
    std::vector<Point> vertices;
    
    void addVertex(int x, int y) {
        vertices.push_back(Point(x, y));
    }
    
    void clear() {
        vertices.clear();
    }
};

// Clipping rectangle structure
struct ClippingRect {
    int xmin, ymin, xmax, ymax;
    
    ClippingRect(int _xmin, int _ymin, int _xmax, int _ymax)
        : xmin(_xmin), ymin(_ymin), xmax(_xmax), ymax(_ymax) {}
};

// Application state
MyPolygon polygon;
MyPolygon clippedPolygon;
ClippingRect clipRect(100, 100, 300, 500);
bool drawingPolygon = false;
bool polygonClosed = false;

// Function to compute region code for a point
int computeRegionCode(int x, int y, const ClippingRect& rect) {
    int code = INSIDE;
    
    if (x < rect.xmin)
        code |= LEFT;
    else if (x > rect.xmax)
        code |= RIGHT;
    
    if (y < rect.ymin)
        code |= BOTTOM;
    else if (y > rect.ymax)
        code |= TOP;
    
    return code;
}

// Cohen-Sutherland line clipping algorithm
bool clipLine(Line& line, const ClippingRect& rect, Line& clippedLine) {
    double x1 = line.start.x;
    double y1 = line.start.y;
    double x2 = line.end.x;
    double y2 = line.end.y;
    
    int code1 = computeRegionCode((int)x1, (int)y1, rect);
    int code2 = computeRegionCode((int)x2, (int)y2, rect);
    
    bool accept = false;
    
    while (true) {
        if ((code1 | code2) == 0) {
            accept = true;
            break;
        } else if ((code1 & code2) != 0) {
            break;
        } else {
            int code = code1 ? code1 : code2;
            double x = 0, y = 0;
            
            if (code & TOP) {
                if (y2 != y1)
                    x = x1 + (x2 - x1) * (rect.ymax - y1) / (y2 - y1);
                else
                    x = x1; // horizontal line
                y = rect.ymax;
            } else if (code & BOTTOM) {
                if (y2 != y1)
                    x = x1 + (x2 - x1) * (rect.ymin - y1) / (y2 - y1);
                else
                    x = x1; // horizontal line
                y = rect.ymin;
            } else if (code & RIGHT) {
                if (x2 != x1)
                    y = y1 + (y2 - y1) * (rect.xmax - x1) / (x2 - x1);
                else
                    y = y1; // vertical line
                x = rect.xmax;
            } else if (code & LEFT) {
                if (x2 != x1)
                    y = y1 + (y2 - y1) * (rect.xmin - x1) / (x2 - x1);
                else
                    y = y1; // vertical line
                x = rect.xmin;
            }
            
            if (code == code1) {
                x1 = x;
                y1 = y;
                code1 = computeRegionCode((int)x1, (int)y1, rect);
            } else {
                x2 = x;
                y2 = y;
                code2 = computeRegionCode((int)x2, (int)y2, rect);
            }
        }
    }
    
    if (accept) {
        clippedLine = Line(Point((int)std::round(x1), (int)std::round(y1)), Point((int)std::round(x2), (int)std::round(y2)));
    }
    
    return accept;
}

// Function to clip a polygon using the Sutherland-Hodgman algorithm
void clipPolygon() {
    if (polygon.vertices.size() < 3) return; // Need at least a triangle
    
    clippedPolygon.clear();
    
    // Create a vector of lines from polygon vertices
    std::vector<Line> lines;
    for (size_t i = 0; i < polygon.vertices.size(); i++) {
        size_t next = (i + 1) % polygon.vertices.size();
        lines.push_back(Line(polygon.vertices[i], polygon.vertices[next]));
    }
    
    // Clip each line and collect the resulting points
    std::vector<Point> clipPoints;
    for (auto& line : lines) {
        Line clippedLine(Point(0, 0), Point(0, 0));
        if (clipLine(line, clipRect, clippedLine)) {
            clipPoints.push_back(clippedLine.start);
            
            // Avoid duplicates
            if (clipPoints.empty() || !(clipPoints.back() == clippedLine.end)) {
                clipPoints.push_back(clippedLine.end);
            }
        }
    }
    
    // If we have at least 3 points, we can form a polygon
    if (clipPoints.size() >= 3) {
        for (const auto& p : clipPoints) {
            clippedPolygon.addVertex(p.x, p.y);
        }
    }
}

// Draw a point
void drawPoint(int x, int y, float size = 5.0f) {
    glPointSize(size);
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}

// Draw a line
void drawLine(const Line& line) {
    glBegin(GL_LINES);
    glVertex2i(line.start.x, line.start.y);
    glVertex2i(line.end.x, line.end.y);
    glEnd();
}

// Draw a polygon
void drawPolygon(const MyPolygon& poly, bool filled = false) {
    if (poly.vertices.empty()) return;
    
    if (filled) {
        glBegin(GL_POLYGON);
        for (const auto& vertex : poly.vertices) {
            glVertex2i(vertex.x, vertex.y);
        }
        glEnd();
    } else {
        glBegin(GL_LINE_LOOP);
        for (const auto& vertex : poly.vertices) {
            glVertex2i(vertex.x, vertex.y);
        }
        glEnd();
    }
    
    // Draw vertices
    for (const auto& vertex : poly.vertices) {
        drawPoint(vertex.x, vertex.y);
    }
}

// Draw the clipping rectangle
void drawClippingRect() {
    glBegin(GL_LINE_LOOP);
    glVertex2i(clipRect.xmin, clipRect.ymin);
    glVertex2i(clipRect.xmax, clipRect.ymin);
    glVertex2i(clipRect.xmax, clipRect.ymax);
    glVertex2i(clipRect.xmin, clipRect.ymax);
    glEnd();
}

// Map coordinates from original space to viewport space
Point mapToViewport(const Point& p) {
    float xRatio = (float)(p.x - clipRect.xmin) / (clipRect.xmax - clipRect.xmin);
    float yRatio = (float)(p.y - clipRect.ymin) / (clipRect.ymax - clipRect.ymin);
    
    int viewX = VIEWPORT_X + xRatio * VIEWPORT_WIDTH;
    int viewY = VIEWPORT_Y + yRatio * VIEWPORT_HEIGHT;
    
    return Point(viewX, viewY);
}

// Display callback function
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Draw in the original window (left side)
    glViewport(0, 0, VIEWPORT_X, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, VIEWPORT_X, 0, WINDOW_HEIGHT);
    
    // Draw the clipping rectangle
    glColor3f(0.0f, 1.0f, 1.0f); // Cyan
    drawClippingRect();
    
    // Draw the original polygon
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow
    drawPolygon(polygon, false);
    
    // Draw in the viewport window (right side)
    glViewport(VIEWPORT_X, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(VIEWPORT_X, VIEWPORT_X + VIEWPORT_WIDTH, 0, VIEWPORT_HEIGHT);
    
    // Draw the clipped polygon in the viewport
    glColor3f(0.0f, 1.0f, 0.0f); // Green
    if (!clippedPolygon.vertices.empty()) {
        MyPolygon viewportPolygon;
        for (const auto& vertex : clippedPolygon.vertices) {
            Point viewportPoint = mapToViewport(vertex);
            viewportPolygon.addVertex(viewportPoint.x, viewportPoint.y);
        }
        drawPolygon(viewportPolygon, true);
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

// Keyboard callback function
void keyboard(unsigned char key, int x, int y) {
    if (key == 27) { // ESC key
        exit(0);
    } else if (key == 'c' || key == 'C') {
        // Clear the polygon
        polygon.clear();
        clippedPolygon.clear();
        drawingPolygon = false;
        polygonClosed = false;
        glutPostRedisplay();
    } else if (key == 'r' || key == 'R') {
        // Reset the clipping rectangle
        clipRect = ClippingRect(100, 100, 300, 500);
        if (polygonClosed) {
            clipPolygon();
        }
        glutPostRedisplay();
    }
}

// Initialize OpenGL settings
void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glPointSize(5.0f);
    glLineWidth(2.0f);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Polygon Clipping with Viewport");
    
    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    
    init();
    
    std::cout << "Polygon Clipping with Viewport Demo (Static Polygon)" << std::endl;
    std::cout << "The original and clipped polygons are shown." << std::endl;
    std::cout << "Press 'R' to reset the clipping rectangle" << std::endl;
    std::cout << "Press 'ESC' to exit" << std::endl;
    
    glutMainLoop();
    return 0;
} 