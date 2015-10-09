
#include <string>
#include <cstdlib>
#include <vector>
#include "weather.hpp"
#include "comp308.hpp"

using namespace std;

int windVelocity = 0;
GLdouble particleRadius = 5;
int cloudIndex = 0;
Cloud clouds[10];

Weather::Weather(){
	initialiseCloud();
}

void Weather::initialiseCloud(){
	Cloud c;

	CloudParticle cp;
	cp.centreX = 0;
	cp.centreY = 50;
	cp.centreZ = 0;
	CloudParticle cp2;
	cp2.centreX = 5;
	cp2.centreY = 50;
	cp2.centreZ = 0;
	CloudParticle cp3;
	cp3.centreX = -5;
	cp3.centreY = 50;
	cp3.centreZ = 0;
	CloudParticle cp4;
	cp4.centreX = 0;
	cp4.centreY = 55;
	cp4.centreZ = 0;
	CloudParticle cp5;
	cp5.centreX = 5;
	cp5.centreY = 55;
	cp5.centreZ = 0;
	CloudParticle cp6;
	cp6.centreX = 10;
	cp6.centreY = 50;
	cp6.centreZ = 0;

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

void Weather::transitionCloud(Cloud c){
	cout << "swag" << c.index << endl;
}

void Weather::growCloud(Cloud c){
	cout << "swag" << c.index << endl;
}

void Weather::cloudExtinction(Cloud c){
	cout << "swag" << c.index << endl;
}

void Weather::renderClouds(){
	for(int i=0; i < cloudIndex; i++){
		Cloud c = clouds[i];
		for(int j=0; j < c.index; j++){
			renderParticles(clouds[i].parts[j]);
		}
	}
}
void Weather::renderParticles(CloudParticle p){
	cout << p.centreX << " " << p.centreY << " " << p.centreZ << endl;
	glColor3f(1.0f,1.0f,1.0f);
	glPushMatrix();
	glTranslatef(p.centreX,p.centreY,p.centreZ);
	glutSolidSphere(particleRadius,20,20);
	glPopMatrix();
}