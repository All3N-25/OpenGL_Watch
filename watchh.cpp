#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cmath>
#include <vector>
#include <ctime>
#include <stdio.h>

using namespace std;

// --- Draw Functions ---
void Draw();
void DrawBackground();
void Crown();
void Bezel();
void BezelMarkers();
void DrawText();	//For full screen
void DrawText2();
void Body();
void BodyMarkers();
void ClockHands();
void Lugs();
void Strap();
void Glass();
void UpdateClock(int); 
void initLighting();


// --- VBO Initializers ---
void initCrownVBO();
void initBezelVBO();
void initBodyVBO();
void initHandsVBO();
void initLugsVBO();
void initStrapVBO();
void initGlassVBO();
void initBackgroundVBO();
void DeleteVBO();

// --- Geometry Generators ---
void CircleGen(float centerX, float centerY, float radius, int width, int numSegments, std::vector<GLfloat>& vertices);
void GenerateCirlceVertices(float centerX, float centerY, float radius, int numSegments, std::vector<GLfloat>& vertices);
void GenerateRingVertices(float centerX, float centerY, float innerRadius, float outerRadius, int numSegments, std::vector<GLfloat>& vertices);

// --- Controls ---
void MouseClick(int button, int state, int x, int y);
void mouseWheel(int wheel, int direction, int x, int y);
void keyboardMonitor(unsigned char key, int x, int y);

// --- Global Variables ---
float bezelRotation = 0.0f;
float scale = 1.0f;
bool rotating = false;
int numSegments = 100;
const int ROTATE_DEGREES = 0;

// VBOs
GLuint crownVBO;
size_t crownSize;

GLuint innerBezelVBO, outerBezelVBO, ringBezelVBO, triangleBezelVBO, lineBezelVBO, dotBezelVBO;
size_t ringBezelSize, innerBezelSize, outerBezelSize, triangleBezelSize, lineBezelSize, dotBezelSize;

GLuint backgroundBodyVBO, logoBodyVBO, lineBodyVBO, circleBodyVBO, RectangleBodyVBO, triangleBodyVBO;
size_t backgroundBodySize, circleBodySize;

GLuint hourHandVBO, minuteHandVBO, secondHandVBO, secondHandLollipopVBO, centerPinVBO;
size_t hourHandSize, minuteHandSize, secondHandSize, secondHandLollipopSize, centerPinSize;

// VBOs for bg 
GLuint backgroundVBO;
size_t backgroundSize;

float bgTranslateY = 0.0f;
const float LINE_SPACING = 0.04f;

// Updated VBOs to include Color Buffers for metallic gradient effect
GLuint lugsVBO, lugsColorVBO;
size_t lugsSize;
GLuint strapVBO, strapColorVBO;
size_t strapSize;

GLuint glassVBO;
size_t glassSize;

// Colors
GLfloat silverColor[] = { 216.0f / 255.0f, 216.0f / 255.0f, 216.0f / 255.0f };
GLfloat silverMetalColor[] = { 158.0f / 255.0f, 168.0f / 255.0f, 174.0f / 255.0f };
GLfloat brushedSteelColor[] = { 120.0f / 255.0f, 125.0f / 255.0f, 130.0f / 255.0f };
GLfloat luminousColor[] = { 250.0f / 255.0f, 245.0f / 255.0f, 239.0f / 255.0f };
GLfloat blackColor[] = { 30.0f / 255.0f, 30.0f / 255.0f, 33.0f / 255.0f };
GLfloat darkGrayColor[] = { 40.0f / 255.0f, 40.0f / 255.0f, 43.0f / 255.0f };
GLfloat bgDarkMetal[] = { 0.10f, 0.10f, 0.12f };
GLfloat bgMidMetal[] = { 0.18f, 0.18f, 0.20f };
GLfloat bgLightMetal[] = { 0.28f, 0.28f, 0.30f };

// ============ MAIN FUNCTION ============
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Watch");

	glutFullScreen();					 // Iuncomment ung nasa body para mas malinaw text
	glClearColor(0.15f, 0.15f, 0.15f, 1.0f); // Dark grey background

	glutDisplayFunc(Draw);
	glutMouseFunc(MouseClick);
	glutMouseWheelFunc(mouseWheel); // scaling control registered here
	glutKeyboardFunc(keyboardMonitor);

	GLenum err = glewInit();
	if (err == GLEW_OK)
	{
		initLighting();

		initCrownVBO();
		initBezelVBO();
		initBodyVBO();
		initHandsVBO();
		initLugsVBO();
		initStrapVBO();
		initGlassVBO();
		initBackgroundVBO();

		// Change: Call UpdateClock with a shorter interval (50ms) for smoother animation
		glutTimerFunc(50, UpdateClock, 0);
		glutMainLoop();
	}

	DeleteVBO();
	return 0;
}

// ============ Lighting Setup ============
void initLighting() {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDisable(GL_DEPTH_TEST);

	// Adjusted light position for better metallic gradients
	GLfloat lightPos[] = { -1.5f, 2.0f, 2.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	GLfloat ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat diffuseLight[] = { 0.9f, 0.9f, 0.95f, 1.0f }; // Slightly cool light
	GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	GLfloat matSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat matShininess[] = { 100.0f };

	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

	glEnable(GL_NORMALIZE);
}

// ============ Primary Draw Function ============
void Draw()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glNormal3f(0.0f, 0.0f, 1.0f);

	DrawBackground();

	glEnable(GL_LIGHTING);
	Strap();
	Lugs();
	Crown();

	// Draw Body (Includes Scalable Logo)
	Body();

	// Draw Bezel
	Bezel();

	glEnable(GL_LIGHTING);
	ClockHands();

	// Draw Glass Top Layer
	Glass();

	glutSwapBuffers();
}

// ============ Glass with Pizza Reflection ============
void initGlassVBO() {
	vector<GLfloat> glassVertices;
	GenerateCirlceVertices(0, 0, 0.65f, numSegments, glassVertices);

	glGenBuffers(1, &glassVBO);
	glBindBuffer(GL_ARRAY_BUFFER, glassVBO);
	glBufferData(GL_ARRAY_BUFFER, glassVertices.size() * sizeof(GLfloat), glassVertices.data(), GL_STATIC_DRAW);
	glassSize = glassVertices.size() / 2;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Glass() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// 1. Base Glass Layer (Specular Highlight)
	glEnable(GL_LIGHTING);
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, glassVBO);
	glVertexPointer(2, GL_FLOAT, 0, 0);

	GLfloat glassSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat glassShininess[] = { 128.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, glassSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, glassShininess);

	glColor4f(0.0f, 0.02f, 0.05f, 0.05f); // Almost invisible base
	glDrawArrays(GL_POLYGON, 0, (GLsizei)glassSize);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// 2. Pizza Shape Reflection (Unlit)
	glDisable(GL_LIGHTING);

	glPushMatrix();
	// Rotate to place the slice in the top-left quadrant
	glRotatef(120.0f, 0.0f, 0.0f, 1.0f);

	// Use Triangle Fan to create a wedge/pizza slice shape
	glBegin(GL_TRIANGLE_FAN);

	// Center/Tip of the pizza slice
	glColor4f(1.0f, 1.0f, 1.0f, 0.05f);
	glVertex2f(0.0f, 0.0f);

	// Crust/Arc Edge (Brighter)
	glColor4f(1.0f, 1.0f, 1.0f, 0.35f);

	float radius = 0.64f;
	int sliceSegments = 40;
	float angleWidth = 80.0f * (M_PI / 180.0f); // 80 degree slice
	float startAngle = -angleWidth / 2.0f;

	for (int i = 0; i <= sliceSegments; i++) {
		float theta = startAngle + angleWidth * (float)i / (float)sliceSegments;
		float x = radius * cos(theta);
		float y = radius * sin(theta);
		glVertex2f(x, y);
	}
	glEnd();

	// Inner highlight
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
	glVertex2f(0.0f, 0.0f);

	glColor4f(1.0f, 1.0f, 1.0f, 0.15f);
	float innerRadius = 0.45f;
	for (int i = 0; i <= sliceSegments; i++) {
		float theta = startAngle + angleWidth * (float)i / (float)sliceSegments;
		float x = innerRadius * cos(theta);
		float y = innerRadius * sin(theta);
		glVertex2f(x, y);
	}
	glEnd();

	glPopMatrix();

	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
}

// ============ Lugs (Metallic Gradient) ============
void initLugsVBO() {
	vector<GLfloat> lugsVertices;
	vector<GLfloat> lugsColors;

	auto pushColor = [&](float r, float g, float b) {
		for (int i = 0; i < 4; i++) { lugsColors.push_back(r); lugsColors.push_back(g); lugsColors.push_back(b); }
		};

	// Top Left Lug
	GLfloat tl[] = { -0.55f, 0.45f, -0.35f, 0.95f, -0.22f, 0.95f, -0.28f, 0.60f };
	lugsVertices.insert(lugsVertices.end(), tl, tl + 8);
	lugsColors.push_back(0.4f); lugsColors.push_back(0.4f); lugsColors.push_back(0.4f);
	lugsColors.push_back(0.8f); lugsColors.push_back(0.8f); lugsColors.push_back(0.8f);
	lugsColors.push_back(0.7f); lugsColors.push_back(0.7f); lugsColors.push_back(0.7f);
	lugsColors.push_back(0.3f); lugsColors.push_back(0.3f); lugsColors.push_back(0.3f);

	// Top Right Lug
	GLfloat tr[] = { 0.28f, 0.60f, 0.22f, 0.95f, 0.35f, 0.95f, 0.55f, 0.45f };
	lugsVertices.insert(lugsVertices.end(), tr, tr + 8);
	lugsColors.push_back(0.7f); lugsColors.push_back(0.7f); lugsColors.push_back(0.7f);
	lugsColors.push_back(0.8f); lugsColors.push_back(0.8f); lugsColors.push_back(0.8f);
	lugsColors.push_back(0.4f); lugsColors.push_back(0.4f); lugsColors.push_back(0.4f);
	lugsColors.push_back(0.3f); lugsColors.push_back(0.3f); lugsColors.push_back(0.3f);

	// Bottom Left Lug
	GLfloat bl[] = { -0.55f, -0.45f, -0.35f, -0.95f, -0.22f, -0.95f, -0.28f, -0.60f };
	lugsVertices.insert(lugsVertices.end(), bl, bl + 8);
	lugsColors.push_back(0.3f); lugsColors.push_back(0.3f); lugsColors.push_back(0.3f);
	lugsColors.push_back(0.4f); lugsColors.push_back(0.4f); lugsColors.push_back(0.4f);
	lugsColors.push_back(0.8f); lugsColors.push_back(0.8f); lugsColors.push_back(0.8f);
	lugsColors.push_back(0.7f); lugsColors.push_back(0.7f); lugsColors.push_back(0.7f);

	// Bottom Right Lug
	GLfloat br[] = { 0.28f, -0.60f, 0.22f, -0.95f, 0.35f, -0.95f, 0.55f, -0.45f };
	lugsVertices.insert(lugsVertices.end(), br, br + 8);
	lugsColors.push_back(0.7f); lugsColors.push_back(0.7f); lugsColors.push_back(0.7f);
	lugsColors.push_back(0.8f); lugsColors.push_back(0.8f); lugsColors.push_back(0.8f);
	lugsColors.push_back(0.4f); lugsColors.push_back(0.4f); lugsColors.push_back(0.4f);
	lugsColors.push_back(0.3f); lugsColors.push_back(0.3f); lugsColors.push_back(0.3f);


	glGenBuffers(1, &lugsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, lugsVBO);
	glBufferData(GL_ARRAY_BUFFER, lugsVertices.size() * sizeof(GLfloat), lugsVertices.data(), GL_STATIC_DRAW);
	lugsSize = lugsVertices.size() / 2;

	glGenBuffers(1, &lugsColorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, lugsColorVBO);
	glBufferData(GL_ARRAY_BUFFER, lugsColors.size() * sizeof(GLfloat), lugsColors.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Lugs() {
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, lugsVBO);
	glVertexPointer(2, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, lugsColorVBO);
	glColorPointer(3, GL_FLOAT, 0, 0);

	glDrawArrays(GL_QUADS, 0, (GLsizei)lugsSize);

	glDisableClientState(GL_COLOR_ARRAY);

	// Outlines
	glBindBuffer(GL_ARRAY_BUFFER, lugsVBO);
	glVertexPointer(2, GL_FLOAT, 0, 0);
	glColor3fv(darkGrayColor);
	glLineWidth(1.0f);
	for (int i = 0; i < 4; i++) glDrawArrays(GL_LINE_LOOP, i * 4, 4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// ============ Strap (Metallic Gradient) ============
void initStrapVBO() {
	vector<GLfloat> strapVertices;
	vector<GLfloat> strapColors;

	float darkSteel[] = { 0.35f, 0.36f, 0.38f };
	float midSteel[] = { 0.6f, 0.61f, 0.63f };
	float brightSteel[] = { 0.85f, 0.86f, 0.88f };

	auto pushCol = [&](float* c) { for (int k = 0; k < 4; k++) { strapColors.push_back(c[0]); strapColors.push_back(c[1]); strapColors.push_back(c[2]); } };

	GLfloat topCenter[] = { -0.18f, 0.92f, 0.18f, 0.92f, 0.18f, 1.2f, -0.18f, 1.2f };
	strapVertices.insert(strapVertices.end(), topCenter, topCenter + 8);
	pushCol(brightSteel);

	GLfloat topLeft[] = { -0.35f, 0.95f, -0.18f, 0.92f, -0.18f, 1.2f, -0.35f, 1.2f };
	strapVertices.insert(strapVertices.end(), topLeft, topLeft + 8);
	pushCol(darkSteel);

	GLfloat topRight[] = { 0.18f, 0.92f, 0.35f, 0.95f, 0.35f, 1.2f, 0.18f, 1.2f };
	strapVertices.insert(strapVertices.end(), topRight, topRight + 8);
	pushCol(darkSteel);

	GLfloat botCenter[] = { -0.18f, -0.92f, 0.18f, -0.92f, 0.18f, -1.2f, -0.18f, -1.2f };
	strapVertices.insert(strapVertices.end(), botCenter, botCenter + 8);
	pushCol(brightSteel);

	GLfloat botLeft[] = { -0.35f, -0.95f, -0.18f, -0.92f, -0.18f, -1.2f, -0.35f, -1.2f };
	strapVertices.insert(strapVertices.end(), botLeft, botLeft + 8);
	pushCol(darkSteel);

	GLfloat botRight[] = { 0.18f, -0.92f, 0.35f, -0.95f, 0.35f, -1.2f, 0.18f, -1.2f };
	strapVertices.insert(strapVertices.end(), botRight, botRight + 8);
	pushCol(darkSteel);

	glGenBuffers(1, &strapVBO);
	glBindBuffer(GL_ARRAY_BUFFER, strapVBO);
	glBufferData(GL_ARRAY_BUFFER, strapVertices.size() * sizeof(GLfloat), strapVertices.data(), GL_STATIC_DRAW);
	strapSize = strapVertices.size() / 2;

	glGenBuffers(1, &strapColorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, strapColorVBO);
	glBufferData(GL_ARRAY_BUFFER, strapColors.size() * sizeof(GLfloat), strapColors.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Strap() {
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, strapVBO);
	glVertexPointer(2, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, strapColorVBO);
	glColorPointer(3, GL_FLOAT, 0, 0);

	glDrawArrays(GL_QUADS, 0, (GLsizei)strapSize);

	glDisableClientState(GL_COLOR_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, strapVBO);
	glVertexPointer(2, GL_FLOAT, 0, 0);
	glColor3fv(darkGrayColor);
	glLineWidth(1.0f);
	for (int i = 0; i < 6; i++) glDrawArrays(GL_LINE_LOOP, i * 4, 4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// ============ Existing Functions (Crown, Bezel, etc.) ============
void initCrownVBO()
{
	vector<GLfloat> crownVertices;
	float crownInnerX = 0.755f;
	float crownOuterX = 0.815f;
	float crownHalfY = 0.05f;
	float ridgeStep = 0.02f;

	GLfloat mainQuad[] = {
		crownInnerX, crownHalfY, crownInnerX, -crownHalfY,
		crownOuterX, -crownHalfY, crownOuterX, crownHalfY
	};
	crownVertices.insert(crownVertices.end(), mainQuad, mainQuad + 8);

	for (float y = -crownHalfY; y <= crownHalfY; y += ridgeStep)
	{
		crownVertices.push_back(crownInnerX);
		crownVertices.push_back(y);
		crownVertices.push_back(crownOuterX);
		crownVertices.push_back(y);
	}
	crownVertices.push_back(crownOuterX);
	crownVertices.push_back(crownHalfY);
	crownVertices.push_back(crownOuterX);
	crownVertices.push_back(-crownHalfY);

	glGenBuffers(1, &crownVBO);
	glBindBuffer(GL_ARRAY_BUFFER, crownVBO);
	glBufferData(GL_ARRAY_BUFFER, crownVertices.size() * sizeof(GLfloat), crownVertices.data(), GL_STATIC_DRAW);
	crownSize = crownVertices.size() / 2;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Crown()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, crownVBO);
	glVertexPointer(2, GL_FLOAT, 0, 0);
	glColor3fv(silverColor);
	glDrawArrays(GL_QUADS, 0, 4);
	glColor3fv(silverMetalColor);
	glLineWidth(1.5f);
	glDrawArrays(GL_LINES, 4, (GLsizei)(crownSize - 4));
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void initBezelVBO()
{
	vector<GLfloat> innerBezelVertices;
	vector<GLfloat> outerBezelVertices;
	vector<GLfloat> ringBezelVertices;

	GenerateRingVertices(0, 0, 0.655f, 0.745f, numSegments, ringBezelVertices);
	glGenBuffers(1, &ringBezelVBO);
	glBindBuffer(GL_ARRAY_BUFFER, ringBezelVBO);
	glBufferData(GL_ARRAY_BUFFER, ringBezelVertices.size() * sizeof(GLfloat), ringBezelVertices.data(), GL_STATIC_DRAW);
	ringBezelSize = ringBezelVertices.size() / 2;

	GenerateCirlceVertices(0, 0, 0.65f, numSegments, innerBezelVertices);
	glGenBuffers(1, &innerBezelVBO);
	glBindBuffer(GL_ARRAY_BUFFER, innerBezelVBO);
	glBufferData(GL_ARRAY_BUFFER, innerBezelVertices.size() * sizeof(GLfloat), innerBezelVertices.data(), GL_STATIC_DRAW);
	innerBezelSize = innerBezelVertices.size() / 2;

	GenerateCirlceVertices(0, 0, 0.745f, numSegments, outerBezelVertices);
	glGenBuffers(1, &outerBezelVBO);
	glBindBuffer(GL_ARRAY_BUFFER, outerBezelVBO);
	glBufferData(GL_ARRAY_BUFFER, outerBezelVertices.size() * sizeof(GLfloat), outerBezelVertices.data(), GL_STATIC_DRAW);
	outerBezelSize = outerBezelVertices.size() / 2;

	GLfloat triangleBezelArray[] = { 0.0f, 0.67f, 0.05f, 0.73f, -0.05f, 0.73f };
	glGenBuffers(1, &triangleBezelVBO);
	glBindBuffer(GL_ARRAY_BUFFER, triangleBezelVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleBezelArray), triangleBezelArray, GL_STATIC_DRAW);
	triangleBezelSize = (sizeof(triangleBezelArray) / sizeof(GLfloat)) / 2;

	GLfloat lineBezelArray[] = { -0.005f, 0.665f, 0.005f, 0.665f, 0.005f, 0.735f, -0.005f, 0.735f };
	glGenBuffers(1, &lineBezelVBO);
	glBindBuffer(GL_ARRAY_BUFFER, lineBezelVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineBezelArray), lineBezelArray, GL_STATIC_DRAW);

	lineBezelSize = sizeof(lineBezelArray) / (2 * sizeof(GLfloat));

	GLfloat dotBezelArray[] = { 0.0f, 0.68f };
	glGenBuffers(1, &dotBezelVBO);
	glBindBuffer(GL_ARRAY_BUFFER, dotBezelVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(dotBezelArray), dotBezelArray, GL_STATIC_DRAW);
	dotBezelSize = sizeof(dotBezelArray) / (2 * sizeof(GLfloat));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Bezel()
{
	glPushMatrix();
	glRotatef(bezelRotation, 0.0f, 0.0f, -1.0f);
	glEnableClientState(GL_VERTEX_ARRAY);

	glEnable(GL_LIGHTING);
	glBindBuffer(GL_ARRAY_BUFFER, ringBezelVBO);
	glVertexPointer(2, GL_FLOAT, 0, 0);
	glColor3fv(blackColor);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)ringBezelSize);

	glDisable(GL_LIGHTING);
	glColor3fv(silverColor);
	BezelMarkers();

	glBindBuffer(GL_ARRAY_BUFFER, innerBezelVBO);
	glVertexPointer(2, GL_FLOAT, 0, 0);
	glColor4f(silverColor[0], silverColor[1], silverColor[2], 0.5f);
	glLineWidth(5.0f);
	glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)innerBezelSize);

	glBindBuffer(GL_ARRAY_BUFFER, outerBezelVBO);
	glVertexPointer(2, GL_FLOAT, 0, 0);
	glColor3fv(silverColor);
	glLineWidth(3.0f);
	glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)outerBezelSize);

	glEnable(GL_LIGHTING);

	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glPopMatrix();
}

void initHandsVBO()
{
	vector<GLfloat> hourVerts, minVerts, secLollipopVerts, pinVerts;

	GLfloat hourHandArr[] = { 0.0f, 0.32f, -0.02f, 0.3f, -0.035f, 0.0f, 0.035f, 0.0f, 0.02f, 0.3f };
	hourVerts.insert(hourVerts.end(), hourHandArr, hourHandArr + 10);
	glGenBuffers(1, &hourHandVBO);
	glBindBuffer(GL_ARRAY_BUFFER, hourHandVBO);
	glBufferData(GL_ARRAY_BUFFER, hourVerts.size() * sizeof(GLfloat), hourVerts.data(), GL_STATIC_DRAW);
	hourHandSize = hourVerts.size() / 2;

	GLfloat minHandArr[] = { 0.0f, 0.48f, -0.015f, 0.46f, -0.03f, 0.0f, 0.03f, 0.0f, 0.015f, 0.46f };
	minVerts.insert(minVerts.end(), minHandArr, minHandArr + 10);
	glGenBuffers(1, &minuteHandVBO);
	glBindBuffer(GL_ARRAY_BUFFER, minuteHandVBO);
	glBufferData(GL_ARRAY_BUFFER, minVerts.size() * sizeof(GLfloat), minVerts.data(), GL_STATIC_DRAW);
	minuteHandSize = minVerts.size() / 2;

	GLfloat secondHandVerts[] = { 0.0f, 0.55f, 0.0f, -0.1f };
	glGenBuffers(1, &secondHandVBO);
	glBindBuffer(GL_ARRAY_BUFFER, secondHandVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(secondHandVerts), secondHandVerts, GL_STATIC_DRAW);
	secondHandSize = 2;

	CircleGen(0.0f, -0.08f, 0.03f, 10, numSegments / 4, secLollipopVerts);
	glGenBuffers(1, &secondHandLollipopVBO);
	glBindBuffer(GL_ARRAY_BUFFER, secondHandLollipopVBO);
	glBufferData(GL_ARRAY_BUFFER, secLollipopVerts.size() * sizeof(GLfloat), secLollipopVerts.data(), GL_STATIC_DRAW);
	secondHandLollipopSize = secLollipopVerts.size() / 2;

	CircleGen(0.0f, 0.0f, 0.02f, 10, numSegments / 5, pinVerts);
	glGenBuffers(1, &centerPinVBO);
	glBindBuffer(GL_ARRAY_BUFFER, centerPinVBO);
	glBufferData(GL_ARRAY_BUFFER, pinVerts.size() * sizeof(GLfloat), pinVerts.data(), GL_STATIC_DRAW);
	centerPinSize = pinVerts.size() / 2;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ClockHands() {
	glPushMatrix();
	if (ROTATE_DEGREES != 0) glRotatef(ROTATE_DEGREES, 0, 0, 1);

	time_t rawtime;
	struct tm timeinfo;
	time(&rawtime);
	struct tm* timeinfo_ptr = localtime(&rawtime);
	if (timeinfo_ptr) timeinfo = *timeinfo_ptr;
	else {
		timeinfo.tm_hour = 0;
		timeinfo.tm_min = 0;
		timeinfo.tm_sec = 0;
	}

	double sec_frac = timeinfo.tm_sec / 60.0;
	double min_frac = (timeinfo.tm_min + sec_frac) / 60.0;
	double hour_frac = ((timeinfo.tm_hour % 12) + min_frac) / 12.0;

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Hour
	glPushMatrix();
	glRotatef((float)(-hour_frac * 360.0), 0.0f, 0.0f, 1.0f);
	glBindBuffer(GL_ARRAY_BUFFER, hourHandVBO);
	glVertexPointer(2, GL_FLOAT, 0, (void*)0);
	glColor3fv(luminousColor);
	glDrawArrays(GL_POLYGON, 0, (GLsizei)hourHandSize);
	glLineWidth(2.0f);
	glColor3fv(silverMetalColor);
	glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)hourHandSize);
	glPopMatrix();

	// Minute
	glPushMatrix();
	glRotatef((float)(-min_frac * 360.0), 0.0f, 0.0f, 1.0f);
	glBindBuffer(GL_ARRAY_BUFFER, minuteHandVBO);
	glVertexPointer(2, GL_FLOAT, 0, (void*)0);
	glColor3fv(luminousColor);
	glDrawArrays(GL_POLYGON, 0, (GLsizei)minuteHandSize);
	glLineWidth(2.0f);
	glColor3fv(silverMetalColor);
	glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)minuteHandSize);
	glPopMatrix();

	// Second
	glPushMatrix();
	glRotatef((float)(-sec_frac * 360.0), 0.0f, 0.0f, 1.0f);
	glLineWidth(2.0f);
	glColor3fv(silverColor);
	glBindBuffer(GL_ARRAY_BUFFER, secondHandVBO);
	glVertexPointer(2, GL_FLOAT, 0, (void*)0);
	glDrawArrays(GL_LINES, 0, (GLsizei)secondHandSize);
	glColor3fv(blackColor);
	glBindBuffer(GL_ARRAY_BUFFER, secondHandLollipopVBO);
	glVertexPointer(2, GL_FLOAT, 0, (void*)0);
	glDrawArrays(GL_POLYGON, 0, (GLsizei)secondHandLollipopSize);
	glPopMatrix();

	glBindBuffer(GL_ARRAY_BUFFER, centerPinVBO);
	glVertexPointer(2, GL_FLOAT, 0, (void*)0);
	glColor3fv(silverMetalColor);
	glDrawArrays(GL_POLYGON, 0, (GLsizei)centerPinSize);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();
}

void DeleteVBO()
{
	glDeleteBuffers(1, &crownVBO);
	glDeleteBuffers(1, &lugsVBO);
	glDeleteBuffers(1, &lugsColorVBO);
	glDeleteBuffers(1, &strapVBO);
	glDeleteBuffers(1, &strapColorVBO);
	glDeleteBuffers(1, &glassVBO);
	glDeleteBuffers(1, &innerBezelVBO);
	glDeleteBuffers(1, &outerBezelVBO);
	glDeleteBuffers(1, &ringBezelVBO);
	glDeleteBuffers(1, &triangleBezelVBO);
	glDeleteBuffers(1, &lineBezelVBO);
	glDeleteBuffers(1, &dotBezelVBO);
	glDeleteBuffers(1, &backgroundBodyVBO);
	glDeleteBuffers(1, &logoBodyVBO);
	glDeleteBuffers(1, &lineBodyVBO);
	glDeleteBuffers(1, &circleBodyVBO);
	glDeleteBuffers(1, &RectangleBodyVBO);
	glDeleteBuffers(1, &triangleBodyVBO);
	glDeleteBuffers(1, &hourHandVBO);
	glDeleteBuffers(1, &minuteHandVBO);
	glDeleteBuffers(1, &secondHandVBO);
	glDeleteBuffers(1, &secondHandLollipopVBO);
	glDeleteBuffers(1, &centerPinVBO);
	glDeleteBuffers(1, &backgroundVBO);
}

void BezelMarkers()
{
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

	for (float angle = 0.0f; angle < 360.0f; angle += 30.0f)
	{
		glPushMatrix();
		glRotatef(angle, 0.0f, 0.0f, -1.0f);

		if (angle == 0.0f) {
			glBindBuffer(GL_ARRAY_BUFFER, triangleBezelVBO);
			glVertexPointer(2, GL_FLOAT, 0, 0);
			glDrawArrays(GL_POLYGON, 0, triangleBezelSize);
		}
		else if (angle == 60.0f || angle == 120.0f || angle == 180.0f || angle == 240.0f || angle == 300.0f) {
			glPushMatrix();
			glTranslatef(-0.03f, 0.67f, 0.0f);
			glScalef(0.0005f, 0.0005f, 1.0f);
			glLineWidth(2.0f);
			const char* str = (angle == 60) ? "10" : (angle == 120) ? "20" : (angle == 180) ? "30" : (angle == 240) ? "40" : "50";
			for (const char* p = str; *p; p++) glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
			glPopMatrix();
		}
		else {
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
	CircleGen(0, 0, 0.649, 10, numSegments, backgroundBodyVertices);
	glGenBuffers(1, &backgroundBodyVBO);
	glBindBuffer(GL_ARRAY_BUFFER, backgroundBodyVBO);
	glBufferData(GL_ARRAY_BUFFER, backgroundBodyVertices.size() * sizeof(GLfloat), backgroundBodyVertices.data(), GL_STATIC_DRAW);
	backgroundBodySize = backgroundBodyVertices.size() / 2;

	GLfloat LogoBodyArray[] = {
		0.075f, 0.23f, -0.075f, 0.23f, -0.065f, 0.165f, 0.0f, 0.14f, 0.065f, 0.165f, 0.075f, 0.23f,
		0.037f, 0.215f, -0.033f, 0.215f, -0.038f, 0.195f, 0.033f, 0.195f, 0.033f, 0.168f, -0.038f, 0.168f
	};
	glGenBuffers(1, &logoBodyVBO);
	glBindBuffer(GL_ARRAY_BUFFER, logoBodyVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(LogoBodyArray), LogoBodyArray, GL_STATIC_DRAW);

	GLfloat lineBodyArray[] = { 0.0f, 0.64f, 0.0f, 0.61f };
	glGenBuffers(1, &lineBodyVBO);
	glBindBuffer(GL_ARRAY_BUFFER, lineBodyVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineBodyArray), lineBodyArray, GL_STATIC_DRAW);

	vector<GLfloat> circleBodyVertices;
	CircleGen(0, 0.55, 0.03, 10, numSegments, circleBodyVertices);
	glGenBuffers(1, &circleBodyVBO);
	glBindBuffer(GL_ARRAY_BUFFER, circleBodyVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * circleBodyVertices.size(), circleBodyVertices.data(), GL_STATIC_DRAW);
	circleBodySize = circleBodyVertices.size() / 2;

	GLfloat triangleBodyArray[] = { 0.09f, 0.57f, -0.09f, 0.57f, 0.0f, 0.45f, 0.0f, 0.4f, 0.0f, 0.45f };
	glGenBuffers(1, &triangleBodyVBO);
	glBindBuffer(GL_ARRAY_BUFFER, triangleBodyVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleBodyArray), triangleBodyArray, GL_STATIC_DRAW);

	GLfloat RectangleArray[] = { 0.55f, 0.035f, 0.55f, -0.035f, 0.33f, -0.035f, 0.33f, 0.035f };
	glGenBuffers(1, &RectangleBodyVBO);
	glBindBuffer(GL_ARRAY_BUFFER, RectangleBodyVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(RectangleArray), RectangleArray, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DrawText() {
    glPushMatrix();

    glColor3fv(silverColor);

    glTranslatef(-0.085f, 0.25f, 0.0f);

    glScalef(0.0005f, 0.0005f, 1.0f);

    const unsigned char brand[] = "SEIKO";
    glutStrokeString(GLUT_STROKE_ROMAN, brand);

    glPopMatrix();
	
	//Automatic
	glPushMatrix();

    glColor3fv(silverColor);

    glTranslatef(-0.0765f, -0.25f, 0.0f);

    glScalef(0.0002f, 0.0002f, 1.0f);

    const unsigned char mode[] = "AUTOMATIC";
    glutStrokeString(GLUT_STROKE_ROMAN, mode);

    glPopMatrix();
}

void Body()
{
	glDisable(GL_LIGHTING);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, backgroundBodyVBO);
	glVertexPointer(2, GL_FLOAT, 0, 0);
	glColor3fv(darkGrayColor);
	glDrawArrays(GL_POLYGON, 0, (GLsizei)backgroundBodySize);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	DrawText();

	// D pwede sa fullscreen
	// const unsigned char brand[100] = { "SEIKO" }; 
	// glColor3fv(silverColor); 
	// glRasterPos2f(-0.085f, 0.25f); 
	// glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, brand);

	// const unsigned char mode[100] = { "AUTOMATIC" }; 
	// glColor3fv(silverColor); 
	// glRasterPos2f(-0.095f, -0.25f); 
	// glutBitmapString(GLUT_BITMAP_HELVETICA_12, mode);

	glEnableClientState(GL_VERTEX_ARRAY);
	glPushMatrix();
	glTranslatef(0.0f, 0.18f, 0.0f);
	glScalef(scale, scale, 1.0f);
	glTranslatef(0.0f, -0.18f, 0.0f);
	glLineWidth(2.5);
	glBindBuffer(GL_ARRAY_BUFFER, logoBodyVBO);
	glVertexPointer(2, GL_FLOAT, 0, 0);
	glDrawArrays(GL_LINE_LOOP, 0, 6);
	glDrawArrays(GL_LINE_STRIP, 6, 2);
	glDrawArrays(GL_LINE_STRIP, 7, 3);
	glDrawArrays(GL_LINE_STRIP, 9, 3);
	glPopMatrix();

	BodyMarkers();

	glDisableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_LIGHTING);
}

void BodyMarkers()
{
	glBindBuffer(GL_ARRAY_BUFFER, lineBodyVBO);
	glVertexPointer(2, GL_FLOAT, 0, 0);
	for (float angle = 0.0f; angle < 360.0f; angle += 6.0f)
	{
		if (fmod(angle, 30) == 0) glLineWidth(6);
		else glLineWidth(2);
		glPushMatrix();
		glRotatef(angle, 0.0f, 0.0f, -1.0f);
		glDrawArrays(GL_LINES, 0, 2);
		glPopMatrix();
	}

	glBindBuffer(GL_ARRAY_BUFFER, triangleBodyVBO);
	glVertexPointer(2, GL_FLOAT, 0, 0);
	glColor3fv(luminousColor);
	glDrawArrays(GL_POLYGON, 0, 3);
	glLineWidth(3.0f);
	glColor3fv(silverMetalColor);
	glDrawArrays(GL_LINE_LOOP, 0, 5);

	for (float angle = 30.0f; angle < 360.0f; angle += 30.0f)
	{
		glPushMatrix();
		glRotatef(angle, 0.0f, 0.0f, -1.0f);
		if (angle == 90) {
			glPopMatrix();
			continue;
		}
		else if (angle == 180 || angle == 270) {
			glPushMatrix();
			glTranslatef(0.0f, 0.55f + 0.025f, 0.0f);
			glScalef(1.0f, 1.8f, 1.0f);
			glTranslatef(0.0f, -(0.55f + 0.025f), 0.0f);
			glColor3fv(luminousColor);
			glDrawArrays(GL_POLYGON, 0, (GLsizei)circleBodySize);
			glLineWidth(3.0f);
			glColor3fv(silverMetalColor);
			glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)circleBodySize);
			glBegin(GL_LINES);
			glVertex2f(0.0f, (0.55f + 0.025f) - 0.055f);
			glVertex2f(0.0f, (0.55f + 0.025f) - (0.045f * 2));
			glEnd();
			glPopMatrix();
		}
		else {
			glBindBuffer(GL_ARRAY_BUFFER, circleBodyVBO);
			glVertexPointer(2, GL_FLOAT, 0, 0);
			glColor3fv(luminousColor);
			glDrawArrays(GL_POLYGON, 0, (GLsizei)circleBodySize);
			glLineWidth(3.0f);
			glColor3fv(silverMetalColor);
			glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)circleBodySize);
		}
		glPopMatrix();
	}

	glBindBuffer(GL_ARRAY_BUFFER, RectangleBodyVBO);
	glVertexPointer(2, GL_FLOAT, 0, 0);
	glColor3fv(luminousColor);
	glDrawArrays(GL_POLYGON, 0, 4);
	glLineWidth(2.0f);
	glColor3fv(silverMetalColor);
	glDrawArrays(GL_LINE_LOOP, 0, 4);

	DrawText2();

	// Hindi pwede sa fullscreen
	// const unsigned char g_mode[100] = { "GROUP 2" };
	// glColor3fv(darkGrayColor);
	// glRasterPos2f(0.335f, -0.020f);
	// glutBitmapString(GLUT_BITMAP_HELVETICA_18, g_mode);
}

void DrawText2() 
{
    glPushMatrix();

    glColor3fv(darkGrayColor);

    // Position in world coordinates (same as your RasterPos)
    glTranslatef(0.335f, -0.020f, 0.0f);

    // Scale for appropriate size (adjust if needed)
    glScalef(0.00032f, 0.00032f, 1.0f);

    const unsigned char g_mode[] = "GROUP 2";
    glutStrokeString(GLUT_STROKE_ROMAN, g_mode);

    glPopMatrix();
}

void GenerateCirlceVertices(float centerX, float centerY, float radius, int numSegments, std::vector<GLfloat>& vertices)
{
	for (int i = 0; i < numSegments; ++i) {
		float angle = 2.0f * M_PI * i / numSegments;
		vertices.push_back(centerX + radius * cos(angle));
		vertices.push_back(centerY + radius * sin(angle));
	}
}
void GenerateRingVertices(float centerX, float centerY, float innerRadius, float outerRadius, int numSegments, std::vector<GLfloat>& vertices)
{
	for (int i = 0; i <= numSegments; ++i) {
		float angle = 2.0f * M_PI * i / numSegments;
		vertices.push_back(centerX + outerRadius * cos(angle));
		vertices.push_back(centerY + outerRadius * sin(angle));
		vertices.push_back(centerX + innerRadius * cos(angle));
		vertices.push_back(centerY + innerRadius * sin(angle));
	}
}
void CircleGen(float centerX, float centerY, float radius, int width, int numSegments, std::vector<GLfloat>& vertices)
{
	for (int i = 0; i < numSegments; ++i) {
		float angle = 2.0f * M_PI * i / numSegments;
		vertices.push_back(centerX + radius * cos(angle));
		vertices.push_back(centerY + radius * sin(angle));
	}
}
void RotateBezel()
{
	if (rotating) {
		bezelRotation += 1.0f;
		if (bezelRotation >= 360.0f) bezelRotation -= 360.0f;
		glutPostRedisplay();
	}
}
void MouseClick(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			rotating = true;
			glutIdleFunc(RotateBezel);
		}
		else if (state == GLUT_UP) {
			rotating = false;
			glutIdleFunc(nullptr);
		}
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		bezelRotation += 5.0f;
		if (bezelRotation >= 360.0f) bezelRotation -= 360.0f;
		glutPostRedisplay();
	}
	else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
		bezelRotation = 0.0f;
		scale = 1.0f;
		glutPostRedisplay();
	}
}
void mouseWheel(int wheel, int direction, int x, int y)
{
	if (direction > 0) scale += 0.05f;
	else scale -= 0.05f;
	if (scale < 0.1f) scale = 0.1f;
	glutPostRedisplay();
}
void keyboardMonitor(unsigned char key, int x, int y)
{
	if (key == 27) {
		DeleteVBO();
		exit(0);
	}
}

void UpdateClock(int) {
	const float SCROLL_STEP = 0.005f;

	bgTranslateY += SCROLL_STEP;

	if (bgTranslateY > LINE_SPACING) {
		bgTranslateY -= LINE_SPACING;
	}

	glutPostRedisplay();
	glutTimerFunc(50, UpdateClock, 0);
}

void initBackgroundVBO() {
	GLfloat bgVertices[] = {
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f,
		-1.0f,  1.0f
	};

	glGenBuffers(1, &backgroundVBO);
	glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bgVertices), bgVertices, GL_STATIC_DRAW);
	backgroundSize = 4;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DrawBackground() {
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glPushMatrix();
	glTranslatef(0.0f, bgTranslateY, 0.0f);


	glColor3fv(bgDarkMetal);
	glBegin(GL_QUADS);
	glVertex2f(-1.0f, -1.0f);
	glVertex2f(1.0f, -1.0f);
	glVertex2f(1.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glEnd();

	glLineWidth(2.0f);

	glBegin(GL_LINES);

	for (float y = -1.1f; y <= 1.1f + LINE_SPACING; y += LINE_SPACING) {

		glColor3fv(bgMidMetal);
		float lineY = y + bgTranslateY;

		
		glVertex2f(-1.0f, lineY);
		glVertex2f(1.0f, lineY);
	}
	glEnd();

	glPopMatrix();
}
