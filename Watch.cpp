#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cmath>

using namespace std;

void Draw();
void Bezel();
void BezelMarkers();
void LineLoopCircle(float centerX, float centerY, float radius, int width, int numSegments);
void TriangleStripRing(float centerX, float centerY, float innerRadius, float outerRadius, int numSegments);
void MouseClick(int button, int state, int x, int y);

float bezelRotation = 0.0f;

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Watch");

    glClearColor(0.55f, 0.55f, 0.55f, 1.0f);

    glutDisplayFunc(Draw);
    glutMouseFunc(MouseClick);
    glutMainLoop();
    
    return 0;
};

void Draw()
{   
    glClear(GL_COLOR_BUFFER_BIT);

    
    Bezel();

    glutSwapBuffers();
}

void Bezel()
{   
    glPushMatrix();

    glRotatef(bezelRotation, 0.0f, 0.0f, -1.0f);
    // Outer 
    glColor3f(216.0f / 255.0f, 216.0f / 255.0f, 216.0f / 255.0f);
    LineLoopCircle(0, 0, 0.75, 5, 50);

    // Ring
    glColor3f(40.0f / 255.0f, 40.0f / 255.0f, 43.0f / 255.0f);
    TriangleStripRing(0.0f, 0.0f, 0.655f, 0.745f, 50);

    glColor3f(216.0f / 255.0f, 216.0f / 255.0f, 216.0f / 255.0f);
    BezelMarkers();

    // Inner 
    glColor3f(216.0f / 255.0f, 216.0f / 255.0f, 216.0f / 255.0f);
    LineLoopCircle(0, 0, 0.65, 5, 50);

    glPopMatrix();
}

void BezelMarkers()
{   
    glBegin(GL_POLYGON);
        glVertex2f(0.0f, 0.67f);
        glVertex2f(0.05f, 0.73f);
        glVertex2f(-0.05f, 0.73f);
    glEnd();
}

void TriangleStripRing(float centerX, float centerY, float innerRadius, float outerRadius, int numSegments)
{

    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= numSegments; ++i) {
        float angle = 2.0f * M_PI * i / numSegments;

        float xOuter = centerX + outerRadius * cos(angle);
        float yOuter = centerY + outerRadius * sin(angle);

        float xInner = centerX + innerRadius * cos(angle);
        float yInner = centerY + innerRadius * sin(angle);

        glVertex2f(xOuter, yOuter);
        glVertex2f(xInner, yInner);
    }
    glEnd();
}

void LineLoopCircle(float centerX, float centerY, float radius, int width, int numSegments)
{   
    glLineWidth(width);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < numSegments; ++i) {
        float angle = 2.0f * M_PI * i / numSegments; 
        float x = centerX + radius * cos(angle);
        float y = centerY + radius * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

void MouseClick(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        // Rotate bezel by 5 degrees clockwise each click
        bezelRotation += 5.0f;
        if (bezelRotation >= 360.0f)
            bezelRotation -= 360.0f;

        glutPostRedisplay(); // Redraw scene
    }
}


