





#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "comp308.hpp"
#include "city.hpp"



using namespace std;
using namespace comp308;


//NoiseMap
const int mapX = 100;
const int mapY = 100;
int noiseMap[mapX][mapY];
bool noiseMapCreated = false;

//Colour map
comp308::vec3* colourMap[mapX][mapY];


//building information
int Hlength = 3;
int Hheight = 2;
int Hrange = 3;

//Road info
int Roadgap = mapX/5;


//Texture info
GLuint g_textures[1];

City::City(){
	createNoiseMap();
}

void City::render(){
	
	glClear( GL_COLOR_BUFFER_BIT); 
	
	glPushMatrix();
	glTranslatef(-5.0, 0.0, 40.0);

	//City Base
	glColor3f(1.0, 0.894, 0.769);
	glBegin(GL_QUADS);
		glVertex3f(0.0, -1.0, 0.0);
  		glVertex3f(mapX*Hlength, -1.0, 0.0);
  		glVertex3f(mapX*Hlength, -1.0, mapY*Hlength);
  		glVertex3f(0.0, -1.0, mapY*Hlength);
	glEnd();
  	glFlush();
	
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 
	
	glBindTexture(GL_TEXTURE_2D, g_textures[0]);
	if(noiseMapCreated){
		for(int a=0; a<mapY; a++){
			for(int b=0; b<mapX; b++){
				if(noiseMap[a][b]!=(-1)){

					glColor3f(colourMap[a][b]->x, colourMap[a][b]->y, colourMap[a][b]->z);
					//floor
					glBegin(GL_QUADS);
					glVertex3f(b*Hlength, 0.0, a*Hlength);
  					glVertex3f((b*Hlength)+Hlength, 0.0,  a*Hlength);
  					glVertex3f((b*Hlength)+Hlength, 0.0, (a*Hlength)+Hlength);
  					glVertex3f(b*Hlength, 0.0, (a*Hlength)+Hlength);
  				
  					//left
  					glVertex3f(b*Hlength, 0.0, a*Hlength);
  					glVertex3f(b*Hlength, noiseMap[a][b], a*Hlength);
  					glVertex3f(b*Hlength, noiseMap[a][b], (a*Hlength)+Hlength);
  					glVertex3f(b*Hlength, 0.0, (a*Hlength)+Hlength);

  					//back
  					glVertex3f(b*Hlength, 0.0, a*Hlength);
  					glVertex3f(b*Hlength, noiseMap[a][b], a*Hlength);
  					glVertex3f((b*Hlength)+Hlength, noiseMap[a][b], a*Hlength);
  					glVertex3f((b*Hlength)+Hlength, 0.0, a*Hlength);

  					//right
  					glVertex3f((b*Hlength)+Hlength, 0.0, a*Hlength);
  					glVertex3f((b*Hlength)+Hlength, noiseMap[a][b], a*Hlength);
  					glVertex3f((b*Hlength)+Hlength, noiseMap[a][b], (a*Hlength)+Hlength);
  					glVertex3f((b*Hlength)+Hlength, 0.0, (a*Hlength)+Hlength);

  					//front
  					glVertex3f(b*Hlength, 0.0, (a*Hlength)+Hlength);
  					glVertex3f(b*Hlength, noiseMap[a][b], (a*Hlength)+Hlength);
  					glVertex3f((b*Hlength)+Hlength, noiseMap[a][b], (a*Hlength)+Hlength);
  					glVertex3f((b*Hlength)+Hlength, 0.0, (a*Hlength)+Hlength);

  					//top
  					glVertex3f(b*Hlength,noiseMap[a][b] , a*Hlength);
  					glVertex3f((b*Hlength)+Hlength, noiseMap[a][b],  a*Hlength);
  					glVertex3f((b*Hlength)+Hlength, noiseMap[a][b], (a*Hlength)+Hlength);
  					glVertex3f(b*Hlength, noiseMap[a][b], (a*Hlength)+Hlength);
  					glEnd();
  					glFlush(); 

  				}
  			}
  		}
  	}
 	glPopMatrix();
 	//glFlush(); 

	
	
}

void City::createNoiseMap(){
	for(int a=0; a<mapY; a++){
		for(int b=0; b<mapX; b++){

			//setting colours
			float randR = rand() % 100;
			float randG = rand() % 100;
			float randB = rand() % 100;
			randR = randR/100;
			randG = randG/100;
			randB = randB/100;

			comp308::vec3* colour = new vec3(randR, randG, randB); 
			colourMap[a][b] = colour;


			//House colours
			// comp308::vec3* sampleMap[2];
			// comp308::vec3* sienna = new vec3(0.627, 0.322, 0.176); 
			// comp308::vec3* grey = new vec3(0.663, 0.663, 0.663); 
			// sampleMap[0] = sienna;
			// sampleMap[1] = grey;
			// int random = rand() % 2;
			

			// colourMap[a][b] = sampleMap[random];
			

			//setting heights
			int number = rand() % 3 ;
			if(number==1){
				float randheight = rand() % Hrange + Hheight ;
				noiseMap[a][b]= randheight;	
			}else {
				noiseMap[a][b]= -1;
			}
			
			//setting roads
			if(b%Roadgap==0){
				noiseMap[a][b]= -1;
				noiseMap[a][b+1]= -1;
				noiseMap[a][b+2]= -1;
				b=b+2;	
			}

			if(a%Roadgap==0){
				noiseMap[a][b]= -1;

			}




		}
	}

	noiseMapCreated = true;



}

