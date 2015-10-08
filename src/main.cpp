#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "comp308.hpp"
#include "imageLoader.hpp"
#include "shaderLoader.hpp"

using namespace std;
using namespace comp308;


// Global variables
// 
GLuint g_winWidth  = 640;
GLuint g_winHeight = 480;
GLuint g_mainWindow = 0;


// Projection values
// 
float g_fovy = 20.0;
float g_znear = 0.1;
float g_zfar = 1000.0;


// Mouse controlled Camera values
//
bool g_mouseDown = false;
vec2 g_mousePos;
float g_yRotation = 0;
float g_xRotation = 0;
float g_zoomFactor = 1.0;


// Scene information
//
GLuint g_texture = 0;
GLuint g_shader = 0;
bool g_useShader = false;

//particles INFORMATION
int	MAX_particles =	1000;

float	slowdown=2.0f;				

GLuint	loop;						// Misc Loop Variable
GLuint	texture[1];					// Storage For Our particles Texture

typedef struct						// particles
{
	bool	alive;					// alive
	float	life;					// particles Life time 
	float	fade;					// Fade Speed
	float	r;						// Red Value
	float	g;						// Green Value
	float	b;						// Blue Value
	float	x;						// X Position
	float	y;						// Y Position
	float	z;						// Z Position
	float	xi;						// X Direction
	float	yi;						// Y Direction
	float	zi;						// Z Direction
	float	xg;						// X Gravity
	float	yg;						// Y Gravity
	float	zg;						// Z Gravity
}
particle;							// particless Structure
vector<particle> particles;	// particles Array (Room For particles Info)
vec3  lavaColor = vec3(1,0.4f,0);



// Sets up where and what the light is
// Called once on start up
// 
void initLight() {
	float direction[]	  = {0.0f, 0.0f, 1.0f, 0.0f};
	float diffintensity[] = {0.7f, 0.7f, 0.7f, 1.0f};
	float ambient[]       = {1,1,1, 1.0f};

	glLightfv(GL_LIGHT0, GL_POSITION, direction);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffintensity);
	glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);	
	
	
	glEnable(GL_LIGHT0);
}

void Initparticless(GLvoid)										// All Setup For OpenGL Goes Here
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f,0.0f,0.0f,0.0f);					// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glDisable(GL_DEPTH_TEST);							// Disable Depth Testing
	glEnable(GL_BLEND);									// Enable Blending
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);					// Type Of Blending To Perform
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);	// Really Nice Perspective Calculations
	glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);				// Really Nice Point Smoothing
	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping
	glBindTexture(GL_TEXTURE_2D,texture[0]);			// Select Our Texture

	for (int i =0;i < 1000 ;i++)				// Initials All The Textures
	{
		particle p;
		p.alive=true;								// Make All The particless alive
		p.life=5.0f;								// Give All The particless Full Life
		p.fade=0.05f;	// Random Fade Speed
		p.r=lavaColor[0];// Select Red Rainbow Color
		p.g=lavaColor[1];	// Select Red Rainbow Color
		p.b=lavaColor[2];	// Select Red Rainbow Color
		p.xi=float((rand()%50)-26.0f)*10.0f;		// Random Speed On X Axis
		p.yi=float((rand()%50)-25.0f)*10.0f;		// Random Speed On Y Axis
		p.zi=float((rand()%50)-25.0f)*10.0f;		// Random Speed On Z Axis
		p.xg=0.0f;									// Set Horizontal Pull To Zero
		p.yg=-0.8f;								// Set Vertical Pull Downward
		p.zg=0.0f;									// Set Pull On Z Axis To Zero

		particles.push_back(p);
	}

}


void initTexture() {
	image tex("work/res/textures/Particle.bmp");

	glActiveTexture(GL_TEXTURE0); // Use slot 0, need to use GL_TEXTURE1 ... etc if using more than one texture PER OBJECT
	glGenTextures(1, &g_texture); // Generate texture ID
	glBindTexture(GL_TEXTURE_2D, g_texture); // Bind it as a 2D texture
	
	// Setup sampling strategies
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Finnaly, actually fill the data into our texture
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tex.w, tex.h, GL_RGBA, GL_UNSIGNED_BYTE, tex.dataPointer());
}




void initShader() {
	g_shader = makeShaderProgram("work/res/shaders/shaderDemo.vert", "work/res/shaders/shaderDemo.frag");
}


// Sets up where the camera is in the scene
// Called every frame
// 
void setUpCamera() {
	// Set up the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(g_fovy, float(g_winWidth) / float(g_winHeight), g_znear, g_zfar);

	// Set up the view part of the model view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0, 0, -50 * g_zoomFactor);
	glRotatef(g_xRotation, 1, 0, 0);
	glRotatef(g_yRotation, 0, 1, 0);
}

float getHeight(float x, float z){
	float dist=( sqrt( (pow(x,2) + pow(z,2)) ) );
	
	return 5.0f - dist;
}

// Draw function
//
void draw() {

	// Black background
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Enable flags for normal rendering
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glDisable(GL_LIGHTING);
	setUpCamera();

	// Texture setup
		//

		// Enable Drawing texures
		//glEnable(GL_TEXTURE_2D);
		// Set the location for binding the texture
		glActiveTexture(GL_TEXTURE0);
		// Bind the texture
		glBindTexture(GL_TEXTURE_2D, g_texture);
				

		// Render a single square as our geometry
		// You would normally render your geometry here
	for (int i =0;i <particles.size();i++)					// Loop Through All The particless
	{
		glPushMatrix();
		if (particles[i].alive)							// If The particles Is alive
		{
			float x=particles[i].x;						// Grab Our particles X Position
			float y=particles[i].y;						// Grab Our particles Y Position
			float z=particles[i].z;					// particles Z Pos + Zoom

			// Draw The particles Using Our RGB Values, Fade The particles Based On It's Life
			glColor4f(particles[i].r,particles[i].g,particles[i].b,particles[i].life);
			glScaled(0.4,0.4,0.4);
			glBegin(GL_QUADS);
 
				glVertex3f(x+-0.5f,y+ -0.5f,z+ -0.5f); 
				glVertex3f(x+-0.5f,y+  1.0f-0.5f,z+  -0.5f); 
				glVertex3f(x+1.0f-0.5f,y+  1.0f-0.5f,z+  -0.5f); 
				glVertex3f(x+1.0f-0.5f,y+  -0.5f,z+  -0.5f);  
		 
				glVertex3f(x+-0.5f,y+  -0.5f, z+ 1.0f-0.5f);  
				glVertex3f(x+-0.5f,y+  1.0f-0.5f,z+  1.0f-0.5f);  
				glVertex3f(x+1.0f-0.5f,y+  1.0f-0.5f,z+  1.0f-0.5f); 
				glVertex3f(x+1.0f-0.5f,y+  -0.5f, z+ 1.0f-0.5f);  
		 
				glVertex3f(x+-0.5f,y+  1.0f-0.5f,z+  -0.5f);  
				glVertex3f(x+1.0f-0.5f,y+  1.0f-0.5f,z+  -0.5f); 
				glVertex3f(x+1.0f-0.5f,y+  1.0f-0.5f,z+  1.0f-0.5f); 
				glVertex3f(x+-0.5f,y+  1.0f-0.5f,z+  1.0f-0.5f); 
		 
				glVertex3f(x+-0.5f,y+  -0.5f,z+  -0.5f);  
				glVertex3f(x+1.0f-0.5f,y+  -0.5f,z+  -0.5f); 
				glVertex3f(x+1.0f-0.5f,y+  -0.5f,z+  1.0f-0.5f); 
				glVertex3f(x+-0.5f,y+  -0.5f,z+  1.0f-0.5f); 
		 
				glVertex3f(x+-0.5f, y+ -0.5f,z+  -0.5f);  
				glVertex3f(x+-0.5f,y+  1.0f-0.5f,z+  -0.5f); 
				glVertex3f(x+-0.5f,y+  1.0f-0.5f,z+  1.0f-0.5f); 
				glVertex3f(x+-0.5f,y+  -0.5f,z+  1.0f-0.5f);  
		 
				glVertex3f(x+1.0f-0.5f,y+  -0.5f,z+  -0.5f);  
				glVertex3f(x+1.0f-0.5f,y+  1.0f-0.5f,z+  -0.5f); 
				glVertex3f(x+1.0f-0.5f,y+  1.0f-0.5f,z+  1.0f-0.5f); 
				glVertex3f(x+1.0f-0.5f,y+  -0.5f,z+  1.0f-0.5f); 
 			glEnd();									// Done Building Triangle Strip

			particles[i].x+=particles[i].xi/(slowdown*1000);// Move On The X Axis By X Speed
			particles[i].z+=particles[i].zi/(slowdown*1000);// Move On The Z Axis By Z Speed
			particles[i].y=getHeight(particles[i].x,particles[i].z);// Move On The Y Axis By Y Speed   //particles[loop].y+=particles[loop].yi/(slowdown*1000);// Move On The Y Axis By Y Speed

			particles[i].xi+=particles[i].xg;			// Take Pull On X Axis Into Account
			//particles[loop].yi+=particles[loop].yg;			// Take Pull On Y Axis Into Account
			particles[i].zi+=particles[i].zg;			// Take Pull On Z Axis Into Account
			particles[i].life-=particles[i].fade;		// Reduce particless Life By 'Fade'
			//cout<<"particles "<<i<<" x:"<<particles[i].x<<" y:"<<particles[i].y<<" z:"<<particles[i].z<<"Fade: "<<particles[i].fade<<endl;
			if (particles[i].life<0.0f)					// If particles Is Burned Out
			{
				particles[i].life=5.0f;					// Give It New Life
				particles[i].fade=0.05;	// Random Fade Value
				particles[i].x=0.0f;						// Center On X Axis
				particles[i].y=5.0f;						// Center On Y Axis
				particles[i].z=0.0f;						// Center On Z Axis
				particles[i].xi=float((rand()%50)-26.0f)*10.0f;		// Random Speed On X Axis
				particles[i].yi=float((rand()%50)-25.0f)*10.0f;		// Random Speed On Y Axis
				particles[i].zi=float((rand()%50)-25.0f)*10.0f;		// Random Speed On Z Axis
				particles[i].r=lavaColor[0];			// Select Red From Color Table
				particles[i].g=lavaColor[1];			// Select Green From Color Table
				particles[i].b=lavaColor[2];			// Select Blue From Color Table
			}

		}
		glPopMatrix();
    }
		glFlush();



	// Disable flags for cleanup (optional)
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_NORMALIZE);

	// Move the buffer we just drew to the front
	glutSwapBuffers();

	// Queue the next frame to be drawn straight away
	glutPostRedisplay();
}


// Reshape function
// 
GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(90.0f,(GLfloat)width/(GLfloat)height,0.1f,5.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}


// Keyboard callback
// Called once per button state change
//
void keyboardCallback(unsigned char key, int x, int y) {
	cout << "Keyboard Callback :: key=" << key << ", x,y=(" << x << "," << y << ")" << endl;
	// YOUR CODE GOES HERE
	// ...
}


// Special Keyboard callback
// Called once per button state change
//
void specialCallback(int key, int x, int y) {
	cout << "Special Callback :: key=" << key << ", x,y=(" << x << "," << y << ")" << endl;
	// YOUR CODE GOES HERE
	// ...
}


// Mouse Button Callback function
// Called once per button state change
// 
void mouseCallback(int button, int state, int x, int y) {
	cout << "Mouse Callback :: button=" << button << ", state=" << state << ", x,y=(" << x << "," << y << ")" << endl;
	// YOUR CODE GOES HERE
	// ...
	switch(button){

		case 0: // left mouse button
			g_mouseDown = (state==0);
			g_mousePos = vec2(x, y);
			break;

		case 2: // right mouse button
			if (state==0)
				g_useShader = !g_useShader;
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
// at least one mouse button has an alive state
// 
void mouseMotionCallback(int x, int y) {
	cout << "Mouse Motion Callback :: x,y=(" << x << "," << y << ")" << endl;
	// YOUR CODE GOES HERE
	// ...
	if (g_mouseDown) {
		vec2 dif = vec2(x,y) - g_mousePos;
		g_mousePos = vec2(x,y);
		g_yRotation += 0.3 * dif.x;
		g_xRotation += 0.3 * dif.y;
	}
}

void createMoreLava(int j){
	cout<<"SADFDFSDFSDKFJHBSDJFHSDJFHSDJFBNJSDFBJSDBFJHDSBFJSDBFJSDHBFJSDBFJSDBFJHSDBFJSDBFJSDBFJHBSDF"<<endl;
for (int i =0;i < 200 ;i++)				// Initials All The Textures
	{
		particle p;
		p.alive=true;								// Make All The particless alive
		p.life=5.0f;								// Give All The particless Full Life
		p.fade=0.05f;	// Random Fade Speed
		p.r=lavaColor[0];// Select Red Rainbow Color
		p.g=lavaColor[1];	// Select Red Rainbow Color
		p.b=lavaColor[2];	// Select Red Rainbow Color
		p.xi=float((rand()%50)-26.0f)*10.0f;		// Random Speed On X Axis
		p.yi=float((rand()%50)-25.0f)*10.0f;		// Random Speed On Y Axis
		p.zi=float((rand()%50)-25.0f)*10.0f;		// Random Speed On Z Axis
		p.xg=0.0f;									// Set Horizontal Pull To Zero
		p.yg=-0.8f;								// Set Vertical Pull Downward
		p.zg=0.0f;									// Set Pull On Z Axis To Zero

		particles.push_back(p);
	}

	glutTimerFunc(100,createMoreLava,1);
}

//Main program
// 
int main(int argc, char **argv) {

	if(argc != 1){
		cout << "No arguments expected" << endl;
		exit(EXIT_FAILURE);
	}

	// Initialise GL, GLU and GLUT
	glutInit(&argc, argv);

	// Setting up the display
	// - RGB color model + Alpha Channel = GLUT_RGBA
	// - Double buffered = GLUT_DOUBLE
	// - Depth buffer = GLUT_DEPTH
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);

	// Initialise window size and create window
	glutInitWindowSize(g_winWidth, g_winHeight);
	g_mainWindow = glutCreateWindow("COMP308 Assignment 3");


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
	glutReshapeFunc(ReSizeGLScene);
	
	glutKeyboardFunc(keyboardCallback);
	glutSpecialFunc(specialCallback);

	glutMouseFunc(mouseCallback);
	glutMotionFunc(mouseMotionCallback);

	initLight();
	initShader();
	initTexture();
	Initparticless();
	glutTimerFunc(100,createMoreLava,1);

	// Loop required by GLUT
	// This will not return until we tell GLUT to finish
	glutMainLoop();


	// Don't forget to delete all pointers that we made
	return 0;
}