#include <vector>

struct CloudParticle{
	int centreX = 0;
	int centreY = 0;
	int centreZ = 0;
};

struct Cloud{
	int index = 0;
	CloudParticle parts[50];
};

class Weather{
private:
	void initialiseCloud();
	void transitionCloud(Cloud c);
	void growCloud(Cloud c);
	void cloudExtinction(Cloud c);
	void renderParticles(CloudParticle p);
public:
	Weather();
	void renderClouds();
};