#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "comp308.hpp"
#include "volcano.hpp"


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
particle;	

class Lava{
private:

	comp308::vec3  lavaColor = comp308::vec3(1,0.4f,0);
	float slowdown = 3000.0f;



	float getHeight(float,float);

public:
	std::vector<particle> particles;

	Lava(Geometry*);
	void initLava();
	void renderLava();
	void initParticles();
};
