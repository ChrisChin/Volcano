#include <math.h>
#include <string>
#include <vector>

#include "comp308.hpp"
#include "lava.hpp"
#include "volcano.hpp"

using namespace std;
using namespace comp308;

vector<particle> particles;
Geometry* geometry;

Lava::Lava(Geometry* geo){
geometry = geo;
particles.clear();
initParticles();
}

void Lava::initParticles()										// All Setup For OpenGL Goes Here
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);					// Type Of Blending To Perform
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);	// Really Nice Perspective Calculations
	glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);				// Really Nice Point Smoothing

}


void Lava::renderLava(){
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Enable flags for normal rendering
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glDisable(GL_LIGHTING);						// Disable Depth Testing

	for (int i =0;i <particles.size();i++)					// Loop Through All The particless
	{
		glPushMatrix();
		if (particles[i].alive)							// If The particles Is alive
		{
			float x=particles[i].x;						// Grab Our particles X Position
			float y=particles[i].y;						// Grab Our particles Y Position
			float z=particles[i].z;					// particles Z Pos + Zoom

			//calculateNextMove(particles[i]);		//for city
			particles[i].x+=particles[i].xi/(slowdown);// Move On The X Axis By X Speed
			particles[i].z+=particles[i].zi/(slowdown);// Move On The Z Axis By Z Speed
			particles[i].y=getHeight(particles[i].x,particles[i].z)+1;// Move On The Y finding height on volcano

			// Draw The particles Using Our RGB Values, Fade The particles Based On It's Life
			glColor4f(particles[i].r,particles[i].g,particles[i].b,particles[i].life);
			glTranslatef(x,y,z);
			glutSolidSphere(0.5,10,10);

			particles[i].xi+=particles[i].xg;			// Take Pull On X Axis Into Account
			//particles[loop].yi+=particles[loop].yg;			// Take Pull On Y Axis Into Account
			particles[i].zi+=particles[i].zg;			// Take Pull On Z Axis Into Account
			particles[i].life-=particles[i].fade;		// Reduce particless Life By 'Fade'
			
			if (particles[i].life<0.0f)					// If particles Is Burned Out
			{
				particles.erase(particles.begin()+i); //delete from vector 
			}

		}
		glPopMatrix();
    }
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

float Lava::getHeight(float x, float z){
	int intX = round(x);
	int intZ = round(z);
	if(intX > 0 && intX<60 && intZ>0 && intZ< 60 )
		return geometry->getHeight(intX,intZ);
	else
		return 0;
}
