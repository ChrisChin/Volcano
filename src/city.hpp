#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "comp308.hpp"



using namespace std;
using namespace comp308;

class City {

public:
	City();
	void render();
	void createNoiseMap();
	int getCityHeight(int x, int y);
	void initTexture();
	void loadTexture(GLuint num, std::string name);
};