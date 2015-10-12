
#include <cmath>
#include <string>
#include <cstdlib>
#include <vector>
#include "weather.hpp"
#include "comp308.hpp"

using namespace std;
using namespace comp308;

double windVelocity = 0.02;
int windDir = -1;

GLdouble particleRadius = 5;
int cloudIndex = 0;
Cloud clouds[10];

Weather::Weather(){
	initialiseWind();
	initialiseCloud();
}

void Weather::initialiseCloud(){
	Cloud c;

	CloudParticle cp;
	cp.particleCentre = vec3(0,50,0);
	CloudParticle cp2;
	cp2.particleCentre = vec3(5,50,0);
	CloudParticle cp3;
	cp3.particleCentre = vec3(-5,50,0);
	CloudParticle cp4;
	cp4.particleCentre = vec3(0,55,0);
	CloudParticle cp5;
	cp5.particleCentre = vec3(5,55,0);
	CloudParticle cp6;
	cp6.particleCentre = vec3(10,50,0);

	c.parts[c.index] = cp;
	c.parts[c.index+1] = cp2;
	c.parts[c.index+2] = cp3;
	c.parts[c.index+3] = cp4;
	c.parts[c.index+4] = cp5;
	c.parts[c.index+5] = cp6;
	c.index = 6;
	clouds[cloudIndex] = c;
	cloudIndex++;
}

void Weather::transitionCloud(){
	for(int i=0; i < cloudIndex; i++){
		Cloud* c = &clouds[i];
		for(int j=0; j < c->index; j++){
			CloudParticle* cp = &c->parts[j];
			switch(windDir){
				case 0: //North
					cp->particleCentre.z -= windVelocity;
					break;
				case 1: //North East
					cp->particleCentre.z -= (windVelocity/2);
					cp->particleCentre.x += (windVelocity/2);
					break;
				case 2: //East
					cp->particleCentre.x += windVelocity;
					break;
				case 3: //South East
					cp->particleCentre.z += (windVelocity/2);
					cp->particleCentre.x += (windVelocity/2);
					break;
				case 4: //South
					cp->particleCentre.z += windVelocity;
					break;
				case 5: //South West
					cp->particleCentre.z += (windVelocity/2);
					cp->particleCentre.x -= (windVelocity/2);
					break;
				case 6: //West
					cp->particleCentre.x -= windVelocity;
					break;
				case 7: //North West
					cp->particleCentre.z -= (windVelocity/2);
					cp->particleCentre.x -= (windVelocity/2);
					break;
			}
		}	
	}
}

void Weather::initialiseWind(){
	windDir = rand() % 8;
}

void Weather::growClouds(){
//Grow from the edges of the cloud
	for(int i=0; i < cloudIndex; i++){
		Cloud* c = &clouds[i];
		for(int j=0; j < c->index; j++){

		}
	}
}

//Need to find distance from centre of cloud
//Higher probability of extinction further away from centre
void Weather::cloudExtinction(Cloud c){
	cout << "swag" << c.index << endl;
}

void Weather::renderClouds(){
	transitionCloud();
	for(int i=0; i < cloudIndex; i++){
		Cloud c = clouds[i];
		for(int j=0; j < c.index; j++){
			renderParticles(c.parts[j]);
		}
	}
}

void Weather::renderParticles(CloudParticle p){
	//cout << p.centreX << " " << p.centreY << " " << p.centreZ << endl;
	glColor3f(1.0f,1.0f,1.0f);
	glPushMatrix();
	glTranslatef(p.particleCentre.x,p.particleCentre.y,p.particleCentre.z);
	glutSolidSphere(particleRadius,20,20);
	glPopMatrix();
}