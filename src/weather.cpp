
#include <cmath>
#include <string>
#include <cstdlib>
#include <vector>
#include "weather.hpp"
#include "comp308.hpp"

using namespace std;
using namespace comp308;

double windVelocity = 0.05;
int windDir = -1;

GLdouble particleRadius = 3;
int cloudIndex = 0;
Cloud clouds[10];

float cloudHeight = 50;

int cloudAmount = 5;

Weather::Weather(){
	initialiseWind();
	initialiseCloud();
}

void Weather::initialiseCloud(){

	for(int i=0; i<cloudAmount; i++){
		createCloud(cloudIndex);
	}
}

void Weather::createCloud(int index){
	int randomX = (rand() % 50) - (rand() % 50);
	int randomZ = (rand() % 50) - (rand() % 50);

	Cloud c;
	c.cloudCentre = vec3(randomX,cloudHeight,randomZ);

	CloudParticle cp;
	cp.particleCentre = vec3(randomX,cloudHeight,randomZ);
	cp.distToCloud = abs(distance(c.cloudCentre,cp.particleCentre));
	CloudParticle cp2;
	cp2.particleCentre = vec3(randomX+particleRadius,cloudHeight,randomZ);
	cp2.distToCloud = abs(distance(c.cloudCentre,cp2.particleCentre));
	CloudParticle cp3;
	cp3.particleCentre = vec3(randomX-particleRadius,cloudHeight,randomZ);
	cp3.distToCloud = abs(distance(c.cloudCentre,cp3.particleCentre));
	CloudParticle cp4;
	cp4.particleCentre = vec3(randomX,cloudHeight,randomZ+particleRadius);
	cp4.distToCloud = abs(distance(c.cloudCentre,cp4.particleCentre));

	c.parts[c.index] = cp;
	c.parts[c.index+1] = cp2;
	c.parts[c.index+2] = cp3;
	c.parts[c.index+3] = cp4;
	c.index = 4;
	clouds[index] = c;
	if(index == cloudIndex){
		cloudIndex += 1;
	}
}

void Weather::transitionCloud(){
	for(int i=0; i < cloudIndex; i++){
		Cloud* c = &clouds[i];
		for(int j=0; j < c->index; j++){
			CloudParticle* cp = &c->parts[j];
			switch(windDir){
				case 0: //North
					cp->particleCentre.z -= windVelocity;
					c->cloudCentre.z -= windVelocity;
					break;
				case 1: //North East
					cp->particleCentre.z -= (windVelocity/2);
					cp->particleCentre.x += (windVelocity/2);
					c->cloudCentre.z -= (windVelocity/2);
					c->cloudCentre.x += (windVelocity/2);
					break;
				case 2: //East
					cp->particleCentre.x += windVelocity;
					c->cloudCentre.x += windVelocity;
					break;
				case 3: //South East
					cp->particleCentre.z += (windVelocity/2);
					cp->particleCentre.x += (windVelocity/2);
					c->cloudCentre.z += (windVelocity/2);
					c->cloudCentre.x += (windVelocity/2);
					break;
				case 4: //South
					cp->particleCentre.z += windVelocity;
					c->cloudCentre.z += windVelocity;
					break;
				case 5: //South West
					cp->particleCentre.z += (windVelocity/2);
					cp->particleCentre.x -= (windVelocity/2);
					c->cloudCentre.z += (windVelocity/2);
					c->cloudCentre.x -= (windVelocity/2);
					break;
				case 6: //West
					cp->particleCentre.x -= windVelocity;
					c->cloudCentre.x -= windVelocity;
					break;
				case 7: //North West
					cp->particleCentre.z -= (windVelocity/2);
					cp->particleCentre.x -= (windVelocity/2);
					c->cloudCentre.z -= (windVelocity/2);
					c->cloudCentre.x -= (windVelocity/2);
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
			int shouldGrow = rand() % 10;
			if((shouldGrow == 2 || shouldGrow == 6) && c->index < 500){
				CloudParticle* current = &c->parts[j];
				CloudParticle cp;
				int growDir = rand()%5;
				float x;
				float y;
				float z;
				switch (growDir){
					//Grow upwards
					case 0:
						x = current->particleCentre.x;
						y = current->particleCentre.y + particleRadius;
						z = current->particleCentre.z;
						break;

					//Grow left
					case 1:
						x = current->particleCentre.x - particleRadius;
						y = current->particleCentre.y;
						z = current->particleCentre.z;
						break;

					//Grow right
					case 2:
						x = current->particleCentre.x + particleRadius;
						y = current->particleCentre.y;
						z = current->particleCentre.z;

					//Grow backwards
					case 3:
						x = current->particleCentre.x;
						y = current->particleCentre.y;
						z = current->particleCentre.z - particleRadius;
						break;

					//Grow forwards
					case 4:
						x = current->particleCentre.x;
						y = current->particleCentre.y;
						z = current->particleCentre.z + particleRadius;
						break;
				}
				cp.particleCentre = vec3(x,y,z);
				cp.distToCloud = abs(distance(c->cloudCentre,cp.particleCentre));
				c->parts[c->index] = cp;
				c->index += 1;
			}
		}
	}
}

//Higher probability of extinction further away from centre
void Weather::cloudExtinction(){
	int extinctIndex = 0;
	CloudParticle extinct[100];
	for(int i=0; i < cloudIndex; i++){
		Cloud* c = &clouds[i];
		for(int j=0; j < c->index; j++){
			CloudParticle current = c->parts[j];
			if(current.distToCloud > 8*particleRadius && extinctIndex < 100){
				extinct[extinctIndex] = current;
				extinctIndex += 1;
			}
		}
	}
	int random = -1;
	if(extinctIndex > 1){
		random = rand() % extinctIndex;
		CloudParticle extinctParticle = extinct[random];
		for(int a=0; a < cloudIndex; a++){
			Cloud* c = &clouds[a];
			for(int b=0; b < c->index; b++){
				CloudParticle current = c->parts[b];
				if(current == extinctParticle){
					CloudParticle temp = c->parts[c->index-1];
					c->parts[b] = temp;
					c->index -= 1;
				}
			}
		}
	}
}

void Weather::renderClouds(){
	int shouldGrow = rand() % 100;
	if(shouldGrow == 2){
		growClouds();
	}
	if(shouldGrow == 28){
		cloudExtinction();
	}
	transitionCloud();
	for(int i=0; i < cloudIndex; i++){
		Cloud c = clouds[i];
		//cout << "Cloud Centre" << c.cloudCentre << endl;
		if(c.cloudCentre.x < -300 || c.cloudCentre.x > 300
			|| c.cloudCentre.z < -300 || c.cloudCentre.z > 300){
			createCloud(i);
			continue;
		}
		for(int j=0; j < c.index; j++){
			renderParticles(c.parts[j]);
		}
	}
}

void Weather::renderParticles(CloudParticle p){
	glColor3f(0.5f,0.5f,0.5f);
	glPushMatrix();
	glTranslatef(p.particleCentre.x,p.particleCentre.y,p.particleCentre.z);
	glutSolidSphere(particleRadius,20,20);
	glPopMatrix();
}