


#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "comp308.hpp"
#include "city.hpp"
#include "imageLoader.hpp"



using namespace std;
using namespace comp308;


//NoiseMap
const int mapX = 100;
const int mapY = 100;
int noiseMap[mapX][mapY];
bool noiseMapCreated = false;

//Colour map
comp308::vec3* colourMap[mapX][mapY];
int textureMap[mapX][mapY];


//building information
int Hlength = 3;
int Hheight = 2;
int Hrange = 3;

//Road info
int Roadgap = mapX/5;


bool firsttime = true;


//Texture info
GLuint g_texture = 0;
GLuint g_textures[8];

City::City(){
	createNoiseMap();
	initTexture();
}

void City::render(){
	
	glClear( GL_COLOR_BUFFER_BIT); 
	
	glPushMatrix();
	glTranslatef(-50.0, 0.0, 30.0);


	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 
	
	glBindTexture(GL_TEXTURE_2D, g_textures[0]);

	
	glMatrixMode(GL_TEXTURE);
	glScalef(40.0f,40.0f,40.0f);
	glMatrixMode(GL_MODELVIEW);
	
	//City Base
	//glColor3f(1.0, 0.894, 0.769);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.0, -1.0, 0.0);
		glTexCoord2f(0.0f, 1.0f);
  		glVertex3f(mapX*Hlength, -1.0, 0.0);
  		glTexCoord2f(1.0f, 1.0f);
  		glVertex3f(mapX*Hlength, -1.0, mapY*Hlength);
  		glTexCoord2f(1.0f, 0.0f);
  		glVertex3f(0.0, -1.0, mapY*Hlength);
	glEnd();
  	glFlush();
	
	

	
	glMatrixMode(GL_TEXTURE);
	glScalef(0.025f,0.025f,0.025f);
	glMatrixMode(GL_MODELVIEW);
  	firsttime = false;
  	
 //  	int randTexture = rand() % 3 + 1;
	// glBindTexture(GL_TEXTURE_2D, g_textures[randTexture]);
	
	if(noiseMapCreated){
		for(int a=0; a<mapY; a++){
			for(int b=0; b<mapX; b++){
				if(noiseMap[a][b]!=(-1)){

					int randTexture = textureMap[a][b];
					glBindTexture(GL_TEXTURE_2D, g_textures[randTexture]);
					
					//glColor3f(colourMap[a][b]->x, colourMap[a][b]->y, colourMap[a][b]->z);
					//floor
					glBegin(GL_QUADS);
					glTexCoord2f(0.0f, 0.0f);
					glVertex3f(b*Hlength, 0.0, a*Hlength);
					glTexCoord2f(0.0f, 1.0f);
  					glVertex3f((b*Hlength)+Hlength, 0.0,  a*Hlength);
  					glTexCoord2f(1.0f, 1.0f);
  					glVertex3f((b*Hlength)+Hlength, 0.0, (a*Hlength)+Hlength);
  					glTexCoord2f(1.0f, 0.0f);
  					glVertex3f(b*Hlength, 0.0, (a*Hlength)+Hlength);
  				
  					//left
  					glTexCoord2f(0.0f, 0.0f);
  					glVertex3f(b*Hlength, 0.0, a*Hlength);
  					glTexCoord2f(0.0f, 1.0f);
  					glVertex3f(b*Hlength, noiseMap[a][b], a*Hlength);
  					glTexCoord2f(1.0f, 1.0f);
  					glVertex3f(b*Hlength, noiseMap[a][b], (a*Hlength)+Hlength);
  					glTexCoord2f(1.0f, 0.0f);
  					glVertex3f(b*Hlength, 0.0, (a*Hlength)+Hlength);

  					//back
  					glTexCoord2f(0.0f, 0.0f);
  					glVertex3f(b*Hlength, 0.0, a*Hlength);
  					glTexCoord2f(0.0f, 1.0f);
  					glVertex3f(b*Hlength, noiseMap[a][b], a*Hlength);
  					glTexCoord2f(1.0f, 1.0f);
  					glVertex3f((b*Hlength)+Hlength, noiseMap[a][b], a*Hlength);
  					glTexCoord2f(1.0f, 0.0f);
  					glVertex3f((b*Hlength)+Hlength, 0.0, a*Hlength);

  					//right
  					glTexCoord2f(0.0f, 0.0f);
  					glVertex3f((b*Hlength)+Hlength, 0.0, a*Hlength);
  					glTexCoord2f(0.0f, 1.0f);
  					glVertex3f((b*Hlength)+Hlength, noiseMap[a][b], a*Hlength);
  					glTexCoord2f(1.0f, 1.0f);
  					glVertex3f((b*Hlength)+Hlength, noiseMap[a][b], (a*Hlength)+Hlength);
  					glTexCoord2f(1.0f, 0.0f);
  					glVertex3f((b*Hlength)+Hlength, 0.0, (a*Hlength)+Hlength);

  					//front
  					glTexCoord2f(0.0f, 0.0f);
  					glVertex3f(b*Hlength, 0.0, (a*Hlength)+Hlength);
  					glTexCoord2f(0.0f, 1.0f);
  					glVertex3f(b*Hlength, noiseMap[a][b], (a*Hlength)+Hlength);
  					glTexCoord2f(1.0f, 1.0f);
  					glVertex3f((b*Hlength)+Hlength, noiseMap[a][b], (a*Hlength)+Hlength);
  					glTexCoord2f(1.0f, 0.0f);
  					glVertex3f((b*Hlength)+Hlength, 0.0, (a*Hlength)+Hlength);

  					//top
  					glTexCoord2f(0.0f, 0.0f);
  					glVertex3f(b*Hlength,noiseMap[a][b] , a*Hlength);
  					glTexCoord2f(0.0f, 1.0f);
  					glVertex3f((b*Hlength)+Hlength, noiseMap[a][b],  a*Hlength);
  					glTexCoord2f(1.0f, 1.0f);
  					glVertex3f((b*Hlength)+Hlength, noiseMap[a][b], (a*Hlength)+Hlength);
  					glTexCoord2f(1.0f, 0.0f);
  					glVertex3f(b*Hlength, noiseMap[a][b], (a*Hlength)+Hlength);
  					glEnd();
  					glFlush(); 

  				}
  			}
  		}
  	}
 	glPopMatrix();
 	//glFlush(); 

	
	glDisable(GL_TEXTURE_2D);
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

			//random textures
			int randTexture = (rand() % 7) + 1;
			textureMap[a][b] = randTexture;




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

int City::getCityHeight(int x, int y){
	return noiseMap[y][x];
}

void City::loadTexture(GLuint num, std::string name){
	image tex("Volcano/res/textures/" + name + ".jpg");
	glActiveTexture(GL_TEXTURE0); // Use slot 0, need to use GL_TEXTURE1 ... etc if using more than one texture PER OBJECT
	glGenTextures(1, &g_texture); // Generate texture ID
	//glBindTexture(GL_TEXTURE_2D, g_texture); // Bind it as a 2D texture

	glBindTexture(GL_TEXTURE_2D, num); // Bind it as a 2D texture

	
	// Setup sampling strategies
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Finnaly, actually fill the data into our texture
	
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tex.w, tex.h, tex.glFormat(), GL_UNSIGNED_BYTE, tex.dataPointer());




}

void City::initTexture() {
	
	glGenTextures(8, g_textures); // Generate texture ID
	loadTexture(g_textures[0], "stoneroad");
	loadTexture(g_textures[1], "stonewall");
	loadTexture(g_textures[2], "stonewall2");
	loadTexture(g_textures[3], "stonewall3");
	loadTexture(g_textures[4], "stonewall4");
	loadTexture(g_textures[5], "stonewall5");
	loadTexture(g_textures[6], "stonewall6");
	loadTexture(g_textures[7], "stonewall7");
	
	

	
}
