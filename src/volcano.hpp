#pragma once

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "comp308.hpp"


struct vertex {
	int p = 0; // index for point in m_points
	int t = 0; // index for uv in m_uvs
	int n = 0; // index for normal in m_normals
};

struct triangle {
	vertex v[3]; //requires 3 verticies
};

class Geometry {
private:

	// Feilds for storing raw obj information
	std::string m_filename;
	std::vector<comp308::vec3> m_points;	// Point list
	std::vector<comp308::vec2> m_uvs;		// Texture Coordinate list
	std::vector<comp308::vec3> m_normals;	// Normal list
	std::vector<triangle> m_triangles;		// Triangle/Face list

	// IDs for the display list to render
	GLuint m_displayListPoly = 0; // DisplayList for Polygon
	GLuint m_displayListWire = 0; // DisplayList for Wireframe

	void readOBJ(std::string);
	void createNormals();
	void createDisplayListPoly();

public:
	Geometry(std::string);
	// ~GeometryManager();

	void renderGeometry();
	float angle;
	float getHeight(int x,int z);
};