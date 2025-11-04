#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cmath>

using namespace std;

void Draw();
void Bezel();
void BezelMarkers();
void Body();

void Logo();
void CircleGen(float centerX, float centerY, float radius, int width, int numSegments);
void LineLoopCircle(float centerX, float centerY, float radius, int width, int numSegments);
void TriangleStripRing(float centerX, float centerY, float innerRadius, float outerRadius, int numSegments);

void MouseClick(int button, int state, int x, int y);
void mouseWheel(int wheel, int direction, int x, int y);

float bezelRotation = 0.0f;
float scale = 1.0f;
bool rotating = false; 

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Watch");

    glClearColor(0.55f, 0.55f, 0.55f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glutDisplayFunc(Draw);
    glutMouseFunc(MouseClick);
    glutMouseWheelFunc(mouseWheel);

    GLenum err = glewInit();
    if (err == GLEW_OK)
        glutMainLoop();
    
    return 0;
};

// Drawers

void Draw()
{   
    glClear(GL_COLOR_BUFFER_BIT);

    Bezel();
    Body();

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
    glColor4f(216.0f / 255.0f, 216.0f / 255.0f, 216.0f / 255.0f, 0.5f); 
    LineLoopCircle(0, 0, 0.65, 5, 50);

    glPopMatrix();
}

void BezelMarkers()
{       
    // Add line bars every 25 degrees
     for (float angle = 0.0f; angle < 360.0f; angle += 30.0f)
    {   
        // Rotate to the specific angle around the Z-axis
        glPushMatrix();

        // Draw a small rectangle at the top position (aligned with the bezel ring)
        glRotatef(angle, 0.0f, 0.0f, -1.0f);

        if (angle == 0.0f)
        {
            glBegin(GL_POLYGON);
            glVertex2f(0.0f, 0.67f);
            glVertex2f(0.05f, 0.73f);
            glVertex2f(-0.05f, 0.73f);
            glEnd();
        }
        else if (angle == 60.0f)
        {   
            glPushMatrix();

            glTranslatef(-0.03f, 0.67f, 0.0f); // Adjust position (x, y)
            glScalef(0.0005f, 0.0005f, 1.0f);  // Adjust scale
            glLineWidth(2.0f);

            const unsigned char str10[] = "10";
            for (const char* p = (const char*)str10; *p; p++)
                glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);

            glPopMatrix();
        }
        else if (angle == 120.0f)
        {   
            glPushMatrix();


            glTranslatef(-0.03f, 0.67f, 0.0f); // Adjust position (x, y)
            glScalef(0.0005f, 0.0005f, 1.0f);  // Adjust scale
            glLineWidth(2.0f);

            const unsigned char str10[] = "20";
            for (const char* p = (const char*)str10; *p; p++)
                glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);

            glPopMatrix(); 
        }
        else if (angle == 180.0f)
        {   
            glPushMatrix();

            glTranslatef(-0.03f, 0.67f, 0.0f); // Adjust position (x, y)
            glScalef(0.0005f, 0.0005f, 1.0f);  // Adjust scale
            glLineWidth(2.0f);

            const unsigned char str10[] = "30";
            for (const char* p = (const char*)str10; *p; p++)
                glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);

            glPopMatrix();
        }
        else if (angle == 240.0f)
        {   
            glPushMatrix();

            glTranslatef(-0.03f, 0.67f, 0.0f); // Adjust position (x, y)
            glScalef(0.0005f, 0.0005f, 1.0f);  // Adjust scale
            glLineWidth(2.0f);

            const unsigned char str10[] = "40";
            for (const char* p = (const char*)str10; *p; p++)
                glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);

            glPopMatrix();
        }
        else if (angle == 300.0f)
        {   
            glPushMatrix();

            glTranslatef(-0.03f, 0.67f, 0.0f); // Adjust position (x, y)
            glScalef(0.0005f, 0.0005f, 1.0f);  // Adjust scale
            glLineWidth(2.0f);

            const unsigned char str10[] = "50";
            for (const char* p = (const char*)str10; *p; p++)
                glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);

            glPopMatrix();
        }
        else
        {
            glBegin(GL_POLYGON);
                glVertex2f(-0.005f, 0.665f); // Left-bottom
                glVertex2f(0.005f, 0.665f);  // Right-bottom
                glVertex2f(0.005f, 0.735f);  // Right-top
                glVertex2f(-0.005f, 0.735f); // Left-top
            glEnd();
        }
        glPopMatrix();
    }
}

void Body()
{   
    //Background
    glColor3f(40.0f / 255.0f, 40.0f / 255.0f, 43.0f / 255.0f); //Matte
    CircleGen(0, 0, 0.649, 10, 50);

    //Brand Name
    const unsigned char brand[100] = {"SEIKO"};
    glColor3f(216.0f / 255.0f, 216.0f / 255.0f, 216.0f / 255.0f);
    glRasterPos2f(-0.085f, 0.25f);
    glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, brand);

    const unsigned char mode[100] = {"AUTOMATIC"};
    glColor3f(216.0f / 255.0f, 216.0f / 255.0f, 216.0f / 255.0f);
    glRasterPos2f(-0.095f, -0.25f);
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, mode);

    //Logo
    glPushMatrix();
        glScalef(scale, scale, 1.0f);
        Logo();
    glPopMatrix();
}

// Shape Generators

void Logo()
{   
    // Container (raised by +0.07 on Y-axis)
    glLineWidth(2.5);
    glBegin(GL_LINE_LOOP);
        glVertex2f(0.075f, 0.23f);
        glVertex2f(-0.075f, 0.23f); 
        glVertex2f(-0.065f, 0.165f);
        glVertex2f(0.0f, 0.14f);
        glVertex2f(0.065f, 0.165f); 
        glVertex2f(0.075f, 0.23f); 
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINE_STRIP);
        glVertex2f(0.037f, 0.215f);  
        glVertex2f(-0.033f, 0.215f);  
        glVertex2f(-0.038f, 0.195f); 
        glVertex2f(0.033f, 0.195f);   
        glVertex2f(0.033f, 0.168f); 
        glVertex2f(-0.038f, 0.168f); 
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
    for (int i = 0; i < numSegments; ++i) 
    {
        float angle = 2.0f * M_PI * i / numSegments; 
        float x = centerX + radius * cos(angle);
        float y = centerY + radius * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

void CircleGen(float centerX, float centerY, float radius, int width, int numSegments)
{
    glBegin(GL_POLYGON);
    for (int i = 0; i < numSegments; ++i)
    {
        float angle = 2.0f * M_PI * i / numSegments; 
        float x = centerX + radius * cos(angle);
        float y = centerY + radius * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

// Transformations

void RotateBezel()
{
    if (rotating)
    {
        bezelRotation += 1.0f;
        if (bezelRotation >= 360.0f)
            bezelRotation -= 360.0f;
        glutPostRedisplay();
    }
}

// Controls

void MouseClick(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            rotating = true;
            glutIdleFunc(RotateBezel); 
        }
        else if (state == GLUT_UP)
        {
            rotating = false;
            glutIdleFunc(nullptr);
        }
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        bezelRotation += 5.0f;
        if (bezelRotation >= 360.0f)
            bezelRotation -= 360.0f;
        glutPostRedisplay();
        }
    else if (button = GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
    {
        bezelRotation = 0.0f;
        scale = 1.0f;
        glutPostRedisplay();
    }
}

void mouseWheel(int wheel, int direction, int x, int y)
{
    if(direction > 0) 
        scale += 0.05f; 
    else 
        scale -= 0.05f;

    if(scale < 0.1f) 
        scale = 0.1f;
    glutPostRedisplay();
}








