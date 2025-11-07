#include <GL/glew.h>
#include <GL/freeglut.h>
#define _USE_MATH_DEFINES 
#include <cmath>
#include <vector>
#include <time.h>
#include <stdio.h>


using namespace std;

void Draw();
void Bezel();
void BezelMarkers();
void Body();
void BodyMarkers();
void ClockHands();
void UpdateClock(int);

//For VBO
void initBezelVBO();
void initBodyVBO();
void initHandsVBO();
void DeleteVBO();

//Generators
void CircleGen(float centerX, float centerY, float radius, int width, int numSegments, vector<GLfloat>& vertices);
void GenerateCirlceVertices(float centerX, float centerY, float radius, int numSegments, vector<GLfloat>& vertices);
void GenerateRingVertices(float centerX, float centerY, float innerRadius, float outerRadius, int numSegments, vector<GLfloat>& vertices);

//Controls
void MouseClick(int button, int state, int x, int y);
void mouseWheel(int wheel, int direction, int x, int y);
void keyboardMonitor(unsigned char key, int x, int y);

//Global Variables
float bezelRotation = 0.0f;
float scale = 1.0f;
bool rotating = false;
int numSegments = 100;
const int ROTATE_DEGREES = 0;

GLuint innerBezelVBO, outerBezelVBO, ringBezelVBO, triangleBezelVBO, lineBezelVBO, dotBezelVBO;
size_t outerBezelSize, ringBezelSize, innerBezelSize, triangleBezelSize, lineBezelSize, dotBezelSize;

GLuint backgroundBodyVBO, logoBodyVBO, lineBodyVBO, circleBodyVBO, RectangleBodyVBO, triangleBodyVBO;
size_t backgroundBodySize, circleBodySize;

GLuint hourHandVBO, minuteHandVBO, secondHandVBO, hubVBO, redCircleVBO;
size_t hourHandSize, minuteHandSize, secondHandSize, hubVertexCount, redCircleVertexCount;


// Circle vertex counts
int hourHandCircleSize = 0;
int minuteHandCircleSize = 0;
int secondHandCircleSize = 0;

// ============ MAIN FUNCTION ============
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
    glutKeyboardFunc(keyboardMonitor);

    GLenum err = glewInit();
    if (err == GLEW_OK)
    {
        initBezelVBO();
        initBodyVBO();
        initHandsVBO();
        glutTimerFunc(1000, UpdateClock, 0);
        glutMainLoop();
    }

    DeleteVBO();
    return 0;
};


// Drawers

void Draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    Bezel();
    Body();
    ClockHands();

    glutSwapBuffers();
}

void initBezelVBO()
{
    vector<GLfloat> innerBezelVertices;
    vector<GLfloat> outerBezelVertices;
    vector<GLfloat> ringBezelVertices;

    //Init Outer
    GenerateCirlceVertices(0, 0, 0.75f, numSegments, outerBezelVertices);
    glGenBuffers(1, &outerBezelVBO);
    glBindBuffer(GL_ARRAY_BUFFER, outerBezelVBO);
    glBufferData(GL_ARRAY_BUFFER, outerBezelVertices.size() * sizeof(GLfloat), outerBezelVertices.data(), GL_STATIC_DRAW);
    outerBezelSize = outerBezelVertices.size() / 2;

    //Init Ring
    GenerateRingVertices(0, 0, 0.655f, 0.745, numSegments, ringBezelVertices);
    glGenBuffers(1, &ringBezelVBO);
    glBindBuffer(GL_ARRAY_BUFFER, ringBezelVBO);
    glBufferData(GL_ARRAY_BUFFER, ringBezelVertices.size() * sizeof(GLfloat), ringBezelVertices.data(), GL_STATIC_DRAW);
    ringBezelSize = ringBezelVertices.size() / 2;

    //Init Inner
    GenerateCirlceVertices(0, 0, 0.65f, numSegments, innerBezelVertices);
    glGenBuffers(1, &innerBezelVBO);
    glBindBuffer(GL_ARRAY_BUFFER, innerBezelVBO);
    glBufferData(GL_ARRAY_BUFFER, innerBezelVertices.size() * sizeof(GLfloat), innerBezelVertices.data(), GL_STATIC_DRAW);
    innerBezelSize = innerBezelVertices.size() / 2;

    //Trianlge
    GLfloat triangleBezelArray[] =
    {
        0.0f, 0.67f,
        0.05f, 0.73f,
        -0.05f, 0.73f
    };

    glGenBuffers(1, &triangleBezelVBO);
    glBindBuffer(GL_ARRAY_BUFFER, triangleBezelVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleBezelArray), triangleBezelArray, GL_STATIC_DRAW);
    triangleBezelSize = (sizeof(triangleBezelArray) / sizeof(GLfloat)) / 2;

    //Bar
    GLfloat lineBezelArray[] =
    {
        -0.005f, 0.665f, // Left-bottom
        0.005f, 0.665f,  // Right-bottom
        0.005f, 0.735f,  // Right-top
        -0.005f, 0.735f // Left-top
    };

    glGenBuffers(1, &lineBezelVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lineBezelVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineBezelArray), lineBezelArray, GL_STATIC_DRAW);
    lineBezelSize = sizeof(lineBezelArray) / sizeof(GLfloat);

    //Dotdots
    GLfloat dotBezelArray[] =
    {
        0.0f, 0.68f
    };

    glGenBuffers(1, &dotBezelVBO);
    glBindBuffer(GL_ARRAY_BUFFER, dotBezelVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(dotBezelArray), dotBezelArray, GL_STATIC_DRAW);
    dotBezelSize = sizeof(dotBezelArray) / (2 * sizeof(GLfloat));

    glBindBuffer(GL_ARRAY_BUFFER, 0);// Unbind
}

void Bezel()
{
    glPushMatrix();
    glRotatef(bezelRotation, 0.0f, 0.0f, -1.0f);

    glEnableClientState(GL_VERTEX_ARRAY);

    // Outer 
    glBindBuffer(GL_ARRAY_BUFFER, outerBezelVBO);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glColor3f(216.0f / 255.0f, 216.0f / 255.0f, 216.0f / 255.0f);
    glLineWidth(5.0f);
    glDrawArrays(GL_LINE_LOOP, 0, outerBezelSize);

    // Ring
    glBindBuffer(GL_ARRAY_BUFFER, ringBezelVBO);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glColor3f(30.0f / 255.0f, 30.0f / 255.0f, 33.0f / 255.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, ringBezelSize);

    glColor3f(216.0f / 255.0f, 216.0f / 255.0f, 216.0f / 255.0f);
    BezelMarkers();

    // Inner 
    glBindBuffer(GL_ARRAY_BUFFER, innerBezelVBO);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glColor4f(216.0f / 255.0f, 216.0f / 255.0f, 216.0f / 255.0f, 0.5f);
    glLineWidth(5.0f);
    glDrawArrays(GL_LINE_LOOP, 0, innerBezelSize);

    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glPopMatrix();
}

void BezelMarkers()
{
    // Add dot every 10 deg
    glPointSize(3.0f);
    glBindBuffer(GL_ARRAY_BUFFER, dotBezelVBO);
    glVertexPointer(2, GL_FLOAT, 0, 0);

    for (float angle = 5.0f; angle < 120.0f; angle += 10.0f)
    {
        glPushMatrix();
        glRotatef(angle, 0.0f, 0.0f, -1.0f);
        glDrawArrays(GL_POINTS, 0, dotBezelSize);
        glPopMatrix();
    }

    // Add line bars every 30 degrees
    for (float angle = 0.0f; angle < 360.0f; angle += 30.0f)
    {
        glPushMatrix();

        glRotatef(angle, 0.0f, 0.0f, -1.0f);

        if (angle == 0.0f)
        {
            //Upside Down Triangle
            glBindBuffer(GL_ARRAY_BUFFER, triangleBezelVBO);
            glVertexPointer(2, GL_FLOAT, 0, 0);
            glDrawArrays(GL_POLYGON, 0, triangleBezelSize);
        }
        else if (angle == 60.0f)
        {
            glPushMatrix();

            glTranslatef(-0.03f, 0.67f, 0.0f);
            glScalef(0.0005f, 0.0005f, 1.0f);
            glLineWidth(2.0f);

            const unsigned char str10[] = "10";
            for (const char* p = (const char*)str10; *p; p++)
                glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);

            glPopMatrix();
        }
        else if (angle == 120.0f)
        {
            glPushMatrix();

            glTranslatef(-0.03f, 0.67f, 0.0f);
            glScalef(0.0005f, 0.0005f, 1.0f);
            glLineWidth(2.0f);

            const unsigned char str10[] = "20";
            for (const char* p = (const char*)str10; *p; p++)
                glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);

            glPopMatrix();
        }
        else if (angle == 180.0f)
        {
            glPushMatrix();

            glTranslatef(-0.03f, 0.67f, 0.0f);
            glScalef(0.0005f, 0.0005f, 1.0f);
            glLineWidth(2.0f);

            const unsigned char str10[] = "30";
            for (const char* p = (const char*)str10; *p; p++)
                glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);

            glPopMatrix();
        }
        else if (angle == 240.0f)
        {
            glPushMatrix();

            glTranslatef(-0.03f, 0.67f, 0.0f);
            glScalef(0.0005f, 0.0005f, 1.0f);
            glLineWidth(2.0f);

            const unsigned char str10[] = "40";
            for (const char* p = (const char*)str10; *p; p++)
                glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);

            glPopMatrix();
        }
        else if (angle == 300.0f)
        {
            glPushMatrix();

            glTranslatef(-0.03f, 0.67f, 0.0f);
            glScalef(0.0005f, 0.0005f, 1.0f);
            glLineWidth(2.0f);

            const unsigned char str10[] = "50";
            for (const char* p = (const char*)str10; *p; p++)
                glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);

            glPopMatrix();
        }
        else
        {
            glBindBuffer(GL_ARRAY_BUFFER, lineBezelVBO);
            glVertexPointer(2, GL_FLOAT, 0, 0);
            glDrawArrays(GL_POLYGON, 0, lineBezelSize);
        }

        glPopMatrix();
    }
}

void initBodyVBO()
{
    vector<GLfloat> backgroundBodyVertices;

    //Background
    CircleGen(0, 0, 0.649, 10, numSegments, backgroundBodyVertices);
    glGenBuffers(1, &backgroundBodyVBO);
    glBindBuffer(GL_ARRAY_BUFFER, backgroundBodyVBO);
    glBufferData(GL_ARRAY_BUFFER, backgroundBodyVertices.size() * sizeof(GLfloat), backgroundBodyVertices.data(), GL_STATIC_DRAW);
    backgroundBodySize = backgroundBodyVertices.size() / 2;

    //LOGO
    GLfloat LogoBodyArray[] =
    {
        //Container
        0.075f, 0.23f,
        -0.075f, 0.23f,
        -0.065f, 0.165f,
        0.0f, 0.14f,
        0.065f, 0.165f,
        0.075f, 0.23f,

        //5
        0.037f, 0.215f,
        -0.033f, 0.215f,
        -0.038f, 0.195f,
        0.033f, 0.195f,
        0.033f, 0.168f,
        -0.038f, 0.168f
    };
    glGenBuffers(1, &logoBodyVBO);
    glBindBuffer(GL_ARRAY_BUFFER, logoBodyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(LogoBodyArray), LogoBodyArray, GL_STATIC_DRAW);

    // Line Bars
    GLfloat lineBodyArray[] =
    {
        0.0f, 0.64f, // Top
        0.0f, 0.61f,  // Bot
    };
    glGenBuffers(1, &lineBodyVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lineBodyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineBodyArray), lineBodyArray, GL_STATIC_DRAW);

    // Circles
    vector<GLfloat> circleBodyVertices;

    CircleGen(0, 0.55, 0.03, 10, numSegments, circleBodyVertices);
    glGenBuffers(1, &circleBodyVBO);
    glBindBuffer(GL_ARRAY_BUFFER, circleBodyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * circleBodyVertices.size(), circleBodyVertices.data(), GL_STATIC_DRAW);
    circleBodySize = circleBodyVertices.size() / 2;

    // Triangle
    GLfloat triangleBodyArray[] =
    {
        0.09f, 0.57f,
        -0.09f, 0.57f,
        0.0f, 0.45f,
        0.0f, 0.4f,
        0.0f, 0.45f
    };

    glGenBuffers(1, &triangleBodyVBO);
    glBindBuffer(GL_ARRAY_BUFFER, triangleBodyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleBodyArray), triangleBodyArray, GL_STATIC_DRAW);

    //Rectangle
    GLfloat RectangleArray[] =
    {
        0.55, 0.035,
        0.55, -0.035,
        0.33, -0.035,
        0.33, 0.035
    };

    glGenBuffers(1, &RectangleBodyVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RectangleBodyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(RectangleArray), RectangleArray, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Body()
{
    glEnableClientState(GL_VERTEX_ARRAY);

    //Background
    glBindBuffer(GL_ARRAY_BUFFER, backgroundBodyVBO);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glColor3f(40.0f / 255.0f, 40.0f / 255.0f, 43.0f / 255.0f); //Matte
    glDrawArrays(GL_POLYGON, 0, backgroundBodySize);

    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Brand Name
    const unsigned char brand[100] = { "SEIKO" };
    glColor3f(216.0f / 255.0f, 216.0f / 255.0f, 216.0f / 255.0f);
    glRasterPos2f(-0.085f, 0.25f);
    glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, brand);

    const unsigned char mode[100] = { "AUTOMATIC" };
    glColor3f(216.0f / 255.0f, 216.0f / 255.0f, 216.0f / 255.0f);
    glRasterPos2f(-0.095f, -0.25f);
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, mode);

    glEnableClientState(GL_VERTEX_ARRAY);

    //Logo
    glPushMatrix();
    // Move center to origin
    glTranslatef(0.0f, 0.18f, 0.0f);
    // Scale
    glScalef(scale, scale, 1.0f);

    glTranslatef(0.0f, -0.18f, 0.0f);

    //Container
    glLineWidth(2.5);
    glBindBuffer(GL_ARRAY_BUFFER, logoBodyVBO);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glDrawArrays(GL_LINE_LOOP, 0, 6);

    //5
    glDrawArrays(GL_LINE_STRIP, 6, 2);
    glDrawArrays(GL_LINE_STRIP, 7, 3);
    glDrawArrays(GL_LINE_STRIP, 9, 3);

    glPopMatrix();

    BodyMarkers();

    glDisableClientState(GL_VERTEX_ARRAY);
}

void BodyMarkers()
{
    //Lines
    glBindBuffer(GL_ARRAY_BUFFER, lineBodyVBO);
    glVertexPointer(2, GL_FLOAT, 0, 0);

    for (float angle = 0.0f; angle < 360.0f; angle += 6.0f)
    {
        if (fmod(angle, 30) == 0)
            glLineWidth(6);
        else
            glLineWidth(2);

        glPushMatrix();
        glRotatef(angle, 0.0f, 0.0f, -1.0f);
        glDrawArrays(GL_LINES, 0, 2);
        glPopMatrix();
    }
    //Triangle
    glBindBuffer(GL_ARRAY_BUFFER, triangleBodyVBO);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glColor3f(250.0f / 255.0f, 245.0f / 255.0f, 239.0f / 255.0f);
    glDrawArrays(GL_POLYGON, 0, 3);

    //Triangle Outline
    glLineWidth(3.0f);
    glColor3f(158.0f / 255.0f, 168.0f / 255.0f, 174.0f / 255.0f);
    glDrawArrays(GL_LINE_LOOP, 0, 5);
    //Circles
    for (float angle = 30.0f; angle < 360.0f; angle += 30.0f)
    {
        glPushMatrix();
        glRotatef(angle, 0.0f, 0.0f, -1.0f);
        if (angle == 90)
        {
            glPopMatrix();
            continue;
        }
        else if (angle == 180 || angle == 270)
        {
            glPushMatrix();

            glTranslatef(0.0f, 0.55f + 0.025f, 0.0f);
            glScalef(1.0f, 1.8f, 1.0f);
            glTranslatef(0.0f, -(0.55f + 0.025f), 0.0f);

            glColor3f(250.0f / 255.0f, 245.0f / 255.0f, 239.0f / 255.0f);
            glDrawArrays(GL_POLYGON, 0, circleBodySize);

            glLineWidth(3.0f);
            glColor3f(158.0f / 255.0f, 168.0f / 255.0f, 174.0f / 255.0f);
            glDrawArrays(GL_LINE_LOOP, 0, circleBodySize);

            glLineWidth(3.0f);
            glColor3f(158.0f / 255.0f, 168.0f / 255.0f, 174.0f / 255.0f);
            glBegin(GL_LINES);
            glVertex2f(0.0f, (0.55f + 0.025f) - 0.055);
            glVertex2f(0.0f, (0.55f + 0.025f) - (0.045f * 2));
            glEnd();

            glPopMatrix();
        }
        else
        {
            glBindBuffer(GL_ARRAY_BUFFER, circleBodyVBO);
            glVertexPointer(2, GL_FLOAT, 0, 0);
            glColor3f(250.0f / 255.0f, 245.0f / 255.0f, 239.0f / 255.0f);
            glDrawArrays(GL_POLYGON, 0, circleBodySize);

            //OUTLINE
            glLineWidth(3.0f);
            glColor3f(158.0f / 255.0f, 168.0f / 255.0f, 174.0f / 255.0f);
            glDrawArrays(GL_LINE_LOOP, 0, circleBodySize);
        }
        glPopMatrix();
    }

    // DATE / GROUP NUMBER
    glBindBuffer(GL_ARRAY_BUFFER, RectangleBodyVBO);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glColor3f(250.0f / 255.0f, 245.0f / 255.0f, 239.0f / 255.0f);
    glDrawArrays(GL_POLYGON, 0, 4);

    glLineWidth(2.0f);
    glColor3f(158.0f / 255.0f, 168.0f / 255.0f, 174.0f / 255.0f);
    glDrawArrays(GL_LINE_LOOP, 0, 4);

    const unsigned char mode[100] = { "GROUP 2" };
    glColor3f(40.0f / 255.0f, 40.0f / 255.0f, 43.0f / 255.0f);
    glRasterPos2f(0.335, -0.020f);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, mode);
}

// Shape Generators

void GenerateCirlceVertices(float centerX, float centerY, float radius, int numSegments, vector<GLfloat>& vertices)
{
    for (int i = 0; i < numSegments; ++i)
    {
        float angle = 2.0f * M_PI * i / numSegments;
        vertices.push_back(centerX + radius * cos(angle)); // X coordinate
        vertices.push_back(centerY + radius * sin(angle)); // Y coordinate
    }
}
void GenerateRingVertices(float centerX, float centerY, float innerRadius, float outerRadius, int numSegments, vector<GLfloat>& vertices)
{
    for (int i = 0; i <= numSegments; ++i) {
        float angle = 2.0f * M_PI * i / numSegments;

        float xOuter = centerX + outerRadius * cos(angle);
        float yOuter = centerY + outerRadius * sin(angle);

        float xInner = centerX + innerRadius * cos(angle);
        float yInner = centerY + innerRadius * sin(angle);

        // Store outer vertex first
        vertices.push_back(xOuter);
        vertices.push_back(yOuter);
        // Store inner vertex second
        vertices.push_back(xInner);
        vertices.push_back(yInner);
    }
}

void CircleGen(float centerX, float centerY, float radius, int width, int numSegments, vector<GLfloat>& vertices)
{
    for (int i = 0; i < numSegments; ++i)
    {
        float angle = 2.0f * M_PI * i / numSegments;
        float x = centerX + radius * cos(angle);
        float y = centerY + radius * sin(angle);

        vertices.push_back(x);
        vertices.push_back(y);
    }
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
    else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
    {
        bezelRotation = 0.0f;
        scale = 1.0f;
        glutPostRedisplay();
    }
}

void mouseWheel(int wheel, int direction, int x, int y)
{
    if (direction > 0)
        scale += 0.05f;
    else
        scale -= 0.05f;

    if (scale < 0.1f)
        scale = 0.1f;
    glutPostRedisplay();
}

void keyboardMonitor(unsigned char key, int x, int y)
{
    if (key == 27)
    {
        DeleteVBO();
        exit(0);
    }
}

void DeleteVBO()
{
    //BEZEL
    glDeleteBuffers(1, &innerBezelVBO);
    glDeleteBuffers(1, &outerBezelVBO);
    glDeleteBuffers(1, &ringBezelVBO);
    glDeleteBuffers(1, &triangleBezelVBO);
    glDeleteBuffers(1, &lineBezelVBO);
    glDeleteBuffers(1, &dotBezelVBO);
    //BODY
    glDeleteBuffers(1, &backgroundBodyVBO);
    glDeleteBuffers(1, &logoBodyVBO);
    glDeleteBuffers(1, &lineBodyVBO);
    glDeleteBuffers(1, &circleBodyVBO);
    glDeleteBuffers(1, &RectangleBodyVBO);
    glDeleteBuffers(1, &triangleBodyVBO);
    //HANDS
    glDeleteBuffers(1, &hourHandVBO);
    glDeleteBuffers(1, &minuteHandVBO);
    glDeleteBuffers(1, &secondHandVBO);
}

void initHandsVBO()
{
    // ----- HOUR HAND (Seiko-style broad with oval lume) -----
    vector<GLfloat> hourHandVerts = {
        -0.015f, -0.07f,
         0.015f,  -0.07f,
         0.020f, 0.05f,
         0.040f, 0.30f,
         0.0f,   0.34f,
        -0.040f, 0.30f,
        -0.020f, 0.05f
    };

    glGenBuffers(1, &hourHandVBO);
    glBindBuffer(GL_ARRAY_BUFFER, hourHandVBO);
    glBufferData(GL_ARRAY_BUFFER, hourHandVerts.size() * sizeof(GLfloat), hourHandVerts.data(), GL_STATIC_DRAW);
    hourHandSize = 7;


    // ----- MINUTE HAND (Seiko-style arrow tip with lume) -----
    vector<GLfloat> minuteHandVerts = {
        // Base section (slim rectangular shaft)
        -0.015f, -0.07f,
         0.015f,  -0.07f,
         0.025f,  0.35f,
		 0.025f,  0.45f, // left side of arrow tip
         0.0f,   0.50f, // tip
		-0.025f,  0.45f, // right side of arrow tip
        -0.025f,  0.35f
    };

    glGenBuffers(1, &minuteHandVBO);
    glBindBuffer(GL_ARRAY_BUFFER, minuteHandVBO);
    glBufferData(GL_ARRAY_BUFFER, minuteHandVerts.size() * sizeof(GLfloat), minuteHandVerts.data(), GL_STATIC_DRAW);
    minuteHandSize = 7;

    // ----- SECOND HAND -----
    vector<GLfloat> secondHandVerts = {
        0.0f, -0.05f,
        0.0f, 0.58f,
        -0.007f, -0.05f,
         0.007f, -0.05f,
         0.007f, -0.20f,
        -0.007f, -0.20f
    };

    vector<GLfloat> secondCircle;
    GenerateCirlceVertices(0.0f, -0.10f, 0.015f, 30, secondCircle);
    secondHandVerts.insert(secondHandVerts.end(), secondCircle.begin(), secondCircle.end());

    glGenBuffers(1, &secondHandVBO);
    glBindBuffer(GL_ARRAY_BUFFER, secondHandVBO);
    glBufferData(GL_ARRAY_BUFFER, secondHandVerts.size() * sizeof(GLfloat), secondHandVerts.data(), GL_STATIC_DRAW);
    secondHandSize = 6;
    secondHandCircleSize = secondCircle.size() / 2;

    // ----- RED CIRCLE (base of second hand) -----
    vector<GLfloat> redCircleVerts;
    const int redCircleSegments = 50;
    const float redCircleRadius = 0.020f; 

    redCircleVerts.push_back(0.0f);
    redCircleVerts.push_back(0.0f);

    for (int i = 0; i <= redCircleSegments; ++i) {
        float angle = 2.0f * M_PI * i / redCircleSegments;
        redCircleVerts.push_back(redCircleRadius * cos(angle));
        redCircleVerts.push_back(redCircleRadius * sin(angle));
    }

    glGenBuffers(1, &redCircleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, redCircleVBO);
    glBufferData(GL_ARRAY_BUFFER,
        redCircleVerts.size() * sizeof(GLfloat),
        redCircleVerts.data(), GL_STATIC_DRAW);
    redCircleVertexCount = static_cast<GLsizei>(redCircleVerts.size() / 2);

    // ----- CENTER HUB (smaller metallic pin) -----
    vector<GLfloat> hubVerts;
    const int hubSegments = 50;
    const float hubRadius = 0.012f;

    hubVerts.push_back(0.0f);
    hubVerts.push_back(0.0f);

    for (int i = 0; i <= hubSegments; ++i) {
        float angle = 2.0f * M_PI * i / hubSegments;
        hubVerts.push_back(hubRadius * cos(angle));
        hubVerts.push_back(hubRadius * sin(angle));
    }

    glGenBuffers(1, &hubVBO);
    glBindBuffer(GL_ARRAY_BUFFER, hubVBO);
    glBufferData(GL_ARRAY_BUFFER,
        hubVerts.size() * sizeof(GLfloat),
        hubVerts.data(), GL_STATIC_DRAW);
    hubVertexCount = static_cast<GLsizei>(hubVerts.size() / 2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}



void ClockHands()
{
    glEnableClientState(GL_VERTEX_ARRAY);

    time_t now = time(0);
    struct tm localTime;
    localtime_s(&localTime, &now);

    int hours = localTime.tm_hour % 12;
    int minutes = localTime.tm_min;
    int seconds = localTime.tm_sec;

    float hourAngle = (hours + minutes / 60.0f) * 30.0f;
    float minuteAngle = (minutes + seconds / 60.0f) * 6.0f;
    float secondAngle = seconds * 6.0f;

    GLfloat lumeColor[] = { 227.0f / 255.0f, 250.0f / 255.0f, 211.0f / 255.0f };
    GLfloat metallicColor[] = { 158.0f / 255.0f, 168.0f / 255.0f, 174.0f / 255.0f };
    GLfloat secondHandColor[] = { 200.0f / 255.0f, 20.0f / 255.0f, 20.0f / 255.0f };
    GLfloat centerPinColor[] = { 70.0f / 255.0f, 70.0f / 255.0f, 70.0f / 255.0f };

    // ----- HOUR HAND -----
    glPushMatrix();
    glRotatef(-hourAngle, 0.0f, 0.0f, 1.0f);
    glBindBuffer(GL_ARRAY_BUFFER, hourHandVBO);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glColor3fv(lumeColor);
    glDrawArrays(GL_POLYGON, 0, hourHandSize);
    glColor3fv(metallicColor);
    glDrawArrays(GL_LINE_LOOP, 0, hourHandSize);
    glColor3fv(lumeColor);
    glDrawArrays(GL_POLYGON, hourHandSize, hourHandCircleSize);
    glPopMatrix();

    // ----- MINUTE HAND -----
    glPushMatrix();
    glRotatef(-minuteAngle, 0.0f, 0.0f, 1.0f);
    glBindBuffer(GL_ARRAY_BUFFER, minuteHandVBO);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glColor3fv(lumeColor);
    glDrawArrays(GL_POLYGON, 0, minuteHandSize);
    glColor3fv(metallicColor);
    glDrawArrays(GL_LINE_LOOP, 0, minuteHandSize);
    glColor3fv(lumeColor);
    glDrawArrays(GL_POLYGON, minuteHandSize, minuteHandCircleSize);
    glPopMatrix();

    // ----- SECOND HAND -----
    glPushMatrix();
    glRotatef(-secondAngle, 0.0f, 0.0f, 1.0f);
    glBindBuffer(GL_ARRAY_BUFFER, secondHandVBO);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glColor3fv(secondHandColor);
    glDrawArrays(GL_LINES, 0, 2);
    glDrawArrays(GL_POLYGON, 2, 4);
    glColor3fv(lumeColor);
    glDrawArrays(GL_POLYGON, secondHandSize, secondHandCircleSize);
    glPopMatrix();

    // ----- RED BASE CIRCLE (under second hand) -----
    glColor3f(0.85f, 0.0f, 0.0f);
    glBindBuffer(GL_ARRAY_BUFFER, redCircleVBO);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, redCircleVertexCount);

    // ----- CENTER HUB (on top) -----
    glColor3fv(centerPinColor);
    glBindBuffer(GL_ARRAY_BUFFER, hubVBO);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, hubVertexCount);

    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void UpdateClock(int value)
{
    glutPostRedisplay();
    glutTimerFunc(1000, UpdateClock, 0);
}