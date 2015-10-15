#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "comp308.hpp"

#include "imageLoader.hpp"
#include "shaderLoader.hpp"
#include "shader.h"

#include "volcano.hpp"
#include "city.hpp"
#include "lava.hpp"
#include "weather.hpp"

using namespace std;
using namespace comp308;

// Global variables
// 
GLuint g_winWidth = 1280;
GLuint g_winHeight = 720;
GLuint g_mainWindow = 0;


// Projection values
// 
float g_fovy = 20.0;
float g_znear = 0.1;
float g_zfar = 100000.0;

bool g_mouseDown = false;
vec2 g_mousePos;
float g_yRotation = 0;
float g_xRotation = 0;
float g_zoomFactor = 10;

// Geometry loader and drawer
//
Geometry *g_geometry = nullptr;
Lava *lava = nullptr;

vec3 lightPosition(-1.93849, 11.233, 21.9049);
vec3 lightDirection(-26.4, 355.2,0);

unsigned int FBO;
unsigned int renderTexture, depthTexture, shadowMap;
int shadowMapWidth = 640 * 8;
int shadowMapHeight = 480 * 8;
mat4 shadowMatrix;	//light's modelviewprojectionmatrix
mat4 modelMatrix;
mat4 viewMatrix;
mat4 projectionMatrix;
//shader* mainShader;
//shader* quadRenderShader;
//shader* simpleShader;
//shader* shadowShader;

GLuint g_shader = 0;

#define RENDER_WIDTH 640.0
#define RENDER_HEIGHT 480.0
#define SHADOW_MAP_RATIO 2


//Camera position
float p_camera[3] = { 32, 20, 0 };

//Camera lookAt
float l_camera[3] = { 2, 0, -10 };

//Light position
float p_light[3] = { 3, 5, 0 };

//Light lookAt
float l_light[3] = { 0, 2, -5 };


//Light mouvement circle radius
float light_mvnt = 30.0f;

// Hold id of the framebuffer for light POV rendering
GLuint fboId;

// Z values will be rendered to this texture when using fboId framebuffer
GLuint depthTextureId;

// Use to activate/disable shadowShader
GLhandleARB shadowShaderId;
GLuint shadowMapUniform;

bool useShadowMap = false;

City *city = nullptr;

//camerafields
float xCam,yCam,zCam = 0;


// Sets up where and what the light is
// Called once on start up
// 
void initLight() {
	float direction[] = { 0.0f, 0.0f, -1.0f, 0.0f };
	float diffintensity[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, direction);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffintensity);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	glEnable(GL_LIGHT0);
}

void setUpCamera() {
	// Set up the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(g_fovy, float(g_winWidth) / float(g_winHeight), g_znear, g_zfar);

	// Set up the view part of the model view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(xCam, yCam, -50 * g_zoomFactor + zCam);
	glRotatef(g_xRotation, 1, 0, 0);
	glRotatef(g_yRotation, 0, 1, 0);
}

void generateShadowFBO()
{
	int shadowMapWidth = RENDER_WIDTH * SHADOW_MAP_RATIO;
	int shadowMapHeight = RENDER_HEIGHT * SHADOW_MAP_RATIO;

	//GLfloat borderColor[4] = {0,0,0,0};

	GLenum FBOstatus;

	// Try to use a texture depth component
	glGenTextures(1, &depthTextureId);
	glBindTexture(GL_TEXTURE_2D, depthTextureId);

	// GL_LINEAR does not make sense for depth texture. However, next tutorial shows usage of GL_LINEAR and PCF
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Remove artefact on the edges of the shadowmap
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	//glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor );
	
	// No need to force GL_DEPTH_COMPONENT24, drivers usually give you the max precision if available 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// create a framebuffer object
	glGenFramebuffersEXT(1, &fboId);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);

	// Instruct openGL that we won't bind a color texture with the currently binded FBO
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// attach the texture to FBO depth attachment point
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthTextureId, 0);

	// check FBO status
	FBOstatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT)
		printf("GL_FRAMEBUFFER_COMPLETE_EXT failed, CANNOT use FBO\n");

	// switch back to window-system-provided framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void setupMatrices(float position_x, float position_y, float position_z, float lookAt_x, float lookAt_y, float lookAt_z)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, RENDER_WIDTH / RENDER_HEIGHT, 10, 40000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(position_x, position_y, position_z, lookAt_x, lookAt_y, lookAt_z, 0, 1, 0);
}

// This update only change the position of the light.
//int elapsedTimeCounter = 0;
void update(void)
{

	p_light[0] = light_mvnt * cos(glutGet(GLUT_ELAPSED_TIME) / 1000.0);
	p_light[2] = light_mvnt * sin(glutGet(GLUT_ELAPSED_TIME) / 1000.0);

	//p_light[0] = light_mvnt * cos(3652/1000.0);
	//p_light[2] = light_mvnt * sin(3652/1000.0);
}


void setTextureMatrix(void)
{
	static double modelView[16];
	static double projection[16];

	// This is matrix transform every coordinate x,y,z
	// x = x* 0.5 + 0.5 
	// y = y* 0.5 + 0.5 
	// z = z* 0.5 + 0.5 
	// Moving from unit cube [-1,1] to [0,1]  
	const GLdouble bias[16] = {
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0 };

	// Grab modelview and transformation matrices
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	
	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE7);

	glLoadIdentity();
	glLoadMatrixd(bias);

	// concatating all matrice into one.
	glMultMatrixd(projection);
	glMultMatrixd(modelView);

	// Go back to normal matrix mode
	glMatrixMode(GL_MODELVIEW);
}

// During translation, we also have to maintain the GL_TEXTURE8, used in the shadow shader
// to determine if a vertex is in the shadow.
void startTranslate(float x, float y, float z)
{
	glPushMatrix();
	glTranslatef(x, y, z);

	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE7);
	glPushMatrix();
	glTranslatef(x, y, z);
}

void endTranslate()
{
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void drawObjects(void)
{
	// Ground
	glColor4f(0.3f, 0.3f, 0.3f, 1);
	glBegin(GL_QUADS);
	glVertex3f(-35, 2, -35);
	glVertex3f(-35, 2, 15);
	glVertex3f(15, 2, 15);
	glVertex3f(15, 2, -35);
	glEnd();

	glColor4f(0.9f, 0.9f, 0.9f, 1);

	// Instead of calling glTranslatef, we need a custom function that also maintain the light matrix
	startTranslate(0, 4, -16);
	glutSolidCube(4);
	endTranslate();

	startTranslate(0, 4, -5);
	glutSolidCube(4);
	endTranslate();
}

void initShader() {
	g_shader = makeShaderProgram("Volcano/res/shaders/shaderDemo.vert", "Volcano/res/shaders/shaderDemo.frag");
}

void draw() {

	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Enable flags for normal rendering
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	initLight();
	// Set up camera every frame
	setUpCamera();
	if (useShadowMap){
		//First step: Render from the light POV to a FBO, story depth values only
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);	//Rendering offscreen

		//Using the fixed pipeline to render to the depthbuffer
		glUseProgramObjectARB(0);

		// In the case we render the shadowmap to a higher resolution, the viewport must be modified accordingly.
		glViewport(0, 0, RENDER_WIDTH * SHADOW_MAP_RATIO, RENDER_HEIGHT* SHADOW_MAP_RATIO);

		// Clear previous frame values
		glClear(GL_DEPTH_BUFFER_BIT);

		//Disable color rendering, we only want to write to the Z-Buffer
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		setupMatrices(p_light[0], p_light[1], p_light[2], l_light[0], l_light[1], l_light[2]);

		// Culling switching, rendering only backface, this is done to avoid self-shadowing
		glCullFace(GL_FRONT);
		//drawObjects();
		g_geometry->renderGeometry();

		//Save modelview/projection matrice into texture7, also add a biais
		setTextureMatrix();

		// Now rendering from the camera POV, using the FBO to generate shadows
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);

		//Enabling color write (previously disabled for light POV z-buffer rendering)
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		// Clear previous frame values
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Using the shadow shader
		//glUseProgramObjectARB(shadowShaderId);
		//glUniform1iARB(shadowMapUniform, 7);
		glUseProgram(g_shader);
		glUniform1i(glGetUniformLocation(g_shader, "ShadowMap"), 0);
		glActiveTextureARB(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, depthTextureId);

		//setupMatrices(p_camera[0], p_camera[1], p_camera[2], l_camera[0], l_camera[1], l_camera[2]);
		setUpCamera();
		glCullFace(GL_BACK);
		//drawObjects();
		g_geometry->renderGeometry();
		glFlush();
		glUseProgram(0);
	}
	else{
		glColor3f(0.5f, 0.5f, 0.5f);
		city->render();
		g_geometry->renderGeometry();
		lava->renderLava();

	}

	// Disable flags for cleanup (optional)
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_NORMALIZE);
	glDisable(GL_COLOR_MATERIAL);

	// Move the buffer we just drew to the front
	glutSwapBuffers();

	// Queue the next frame to be drawn straight away
	glutPostRedisplay();
}


// Reshape function
// 
void reshape(int w, int h) {
	if (h == 0) h = 1;

	g_winWidth = w;
	g_winHeight = h;

	// Sets the openGL rendering window to match the window size
	glViewport(0, 0, g_winWidth, g_winHeight);
}

// Keyboard callback
// Called once per button state change
//
void keyboardCallback(unsigned char key, int x, int y) {
	cout << "Keyboard Callback :: key=" << key << ", x,y=(" << x << "," << y << ")" << endl;
	switch (key){

	case 'w': // left mouse button
	zCam += 2;
		break;

	case 'a': // scroll foward/up
	xCam += 2;
		break;

	case 's': // left mouse button
	zCam -= 2;
		break;

	case 'd': // scroll foward/up
	xCam -= 2;
		break;

	}
}


// Special Keyboard callback
// Called once per button state change
//
void specialCallback(int key, int x, int y) {
	cout << "Special Callback :: key=" << key << ", x,y=(" << x << "," << y << ")" << endl;
	switch (key){

	case 101: // left mouse button
	yCam += 2;
		break;

	case 103: // scroll foward/up
	yCam -= 2;
		break;



	}
}


void mouseCallback(int button, int state, int x, int y) {
	cout << "Mouse Callback :: button=" << button << ", state=" << state << ", x,y=(" << x << "," << y << ")" << endl;
	// YOUR CODE GOES HERE
	// ...
	switch (button){

	case 0: // left mouse button
		g_mouseDown = (state == 0);
		g_mousePos = vec2(x, y);
		break;
	case 3: // scroll foward/up
		g_zoomFactor /= 1.1;
		break;

	case 4: // scroll back/down
		g_zoomFactor *= 1.1;
		break;
	}
}

// Mouse Motion Callback function
// Called once per frame if the mouse has moved and
// at least one mouse button has an active state
// 
void mouseMotionCallback(int x, int y) {
	//cout << "Mouse Motion Callback :: x,y=(" << x << "," << y << ")" << endl;
	// YOUR CODE GOES HERE
	// ...
	if (g_mouseDown) {
		vec2 dif = vec2(x, y) - g_mousePos;
		g_mousePos = vec2(x, y);
		g_yRotation += 0.3 * dif.x;
		g_xRotation += 0.3 * dif.y;
	}
}

//Lava flow
void createMoreLava(int j){ //creates lava flow  -nonstop
for (int i =0;i < 10 ;i++)				// Initials All The Textures
	{
		particle p;
		p.x = 0;
		p.y = 10;
		p.z = 0;
		p.alive=true;								// Make All The particless alive
		p.life=50;								// Give All The particless Full Life
		p.fade=0.05f;								// Random Fade Speed
		p.r=1.0f;							// Select Red Rainbow Color
		p.g=0.4f;							// Select Red Rainbow Color
		p.b=0.0f;							// Select Red Rainbow Color
		p.xi=120.0f;								// Random Speed On X Axis
		p.yi=0.0f;								// Random Speed On Y Axis
		p.zi=float(rand()%40)+80.0f;						// Random Speed On Z Axis
		p.xg=0.0f;								// Set Horizontal Pull To Zero
		p.yg=-0.8f;								// Set Vertical Pull Downward
		p.zg=0.0f;								// Set Pull On Z Axis To Zero
		lava->particles.push_back(p);
	}

	glutTimerFunc(100,createMoreLava,1); 					//flow at 100 milliseconds
}

//Main program
// 
int main(int argc, char **argv) {
	// Initialise GL, GLU and GLUT
	glutInit(&argc, argv);

	// Setting up the display
	// - RGB color model + Alpha Channel = GLUT_RGBA
	// - Double buffered = GLUT_DOUBLE
	// - Depth buffer = GLUT_DEPTH
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	// Initialise window size and create window
	glutInitWindowSize(g_winWidth, g_winHeight);
	g_mainWindow = glutCreateWindow("COMP308 Final Project");


	// Initilise GLEW
	// must be done after creating GL context (glutCreateWindow in this case)
	GLenum err = glewInit();
	if (GLEW_OK != err) { // Problem: glewInit failed, something is seriously wrong.
		cerr << "Error: " << glewGetErrorString(err) << endl;
		abort(); // Unrecoverable error
	}

	cout << "Using OpenGL " << glGetString(GL_VERSION) << endl;
	cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;
	
	// Register functions for callback
	glutDisplayFunc(draw);
	glutReshapeFunc(reshape);

	glutKeyboardFunc(keyboardCallback);
	glutSpecialFunc(specialCallback);

	glutMouseFunc(mouseCallback);
	glutMotionFunc(mouseMotionCallback);
	
	// Create a light on the camera
	//initLight();
	generateShadowFBO();
	initShader();

	// Finally create our geometry
	city = new City();
	g_geometry = new Geometry("Volcano/res/assets/volcano.obj");

	//Initate Lava
	lava = new Lava(g_geometry);
	createMoreLava(1);

	// Loop required by GLUT
	// This will not return until we tell GLUT to finish
	glutMainLoop();

	// Don't forget to delete all pointers that we made
	delete city;
	delete g_geometry;
	return 0;
}