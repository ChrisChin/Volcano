#include <vector>
#include "comp308.hpp"

using namespace comp308;

struct CloudParticle{
	vec3 particleCentre = vec3(0,0,0);
	double distToCloud = 0;
	bool operator== (CloudParticle &other){
	if(particleCentre.x == other.particleCentre.x
		&& particleCentre.y == other.particleCentre.y
		&& particleCentre.z == other.particleCentre.z){
		return true;
	}
	return false;
	}
};

struct Cloud{
	int index = 0;
	vec3 cloudCentre = vec3(0,0,0);
	CloudParticle parts[500];
};

class Weather{
private:
	void initialiseCloud();
	void createCloud(int index);
	void transitionCloud();
	void growClouds();
	void cloudExtinction();
	void renderParticles(CloudParticle p);
public:
	int windDir = 3;
	double windVelocity = 0.2;
	Weather();
	void renderClouds();
};