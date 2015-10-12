#include <vector>
#include "comp308.hpp"

using namespace comp308;

struct CloudParticle{
	vec3 particleCentre = vec3(0,0,0);
};

struct Cloud{
	int index = 0;
	vec3 cloudCentre = vec3(0,0,0);
	CloudParticle parts[50];
};

class Weather{
private:
	void initialiseCloud();
	void transitionCloud();
	void initialiseWind();
	void growClouds();
	void cloudExtinction(Cloud c);
	void renderParticles(CloudParticle p);
public:
	Weather();
	void renderClouds();
};