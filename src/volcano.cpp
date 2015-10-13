
#include <cmath>
#include <iostream> // input/output streams
#include <fstream>  // file streams
#include <sstream>  // string streams
#include <string>
#include <stdexcept>
#include <vector>
#include <map>
#include <list>

#include "comp308.hpp"
#include "volcano.hpp"

using namespace std;
using namespace comp308;

float minHeight;
float maxHeight;
float minX;
float maxX;
float minZ;
float maxZ;
float heightMap[60][60];

GLfloat ambient[4] = { 0.2, 0.1, 0.0, 1.0 };
GLfloat diffuse[3] = { 1.0, 1.0, 1.0};
GLfloat specular[3] = { 0, 0, 0 };
GLfloat shininess = 0.25;

Geometry::Geometry(string filename) {
	m_filename = filename;
	readOBJ(filename);
	if (m_triangles.size() > 0) {
		createDisplayListPoly();
	}
	float min = 10000;
	float max = 0;
	for (int i = 0; i < m_points.size(); i++){
		if (m_points[i].y < min)
			min = m_points[i].y;
		if (m_points[i].y > max)
			max = m_points[i].y;
	}
	maxHeight = max;
	minHeight = min;
	
	cout << "maxheight " << maxHeight << endl;
	cout << "minHeight " << minHeight << endl;

	float currentminX = 10000;
	float currentmaxX = 0;
	for (int i = 0; i < m_points.size(); i++){
		if (m_points[i].x < currentminX)
			currentminX = m_points[i].x;
		if (m_points[i].x > currentmaxX)
			currentmaxX = m_points[i].x;
	}
	maxX = currentmaxX;
	minX = currentminX;

	cout << "maxX " << maxX << endl;
	cout << "minX " << minX << endl;

	float currentminZ = 10000;
	float currentmaxZ = 0;
	for (int i = 0; i < m_points.size(); i++){
		if (m_points[i].z < currentminZ)
			currentminZ = m_points[i].z;
		if (m_points[i].z > currentmaxZ)
			currentmaxZ = m_points[i].z;
	}
	maxX = currentmaxZ;
	minX = currentminZ;

	cout << "currentmaxZ " << currentmaxZ << endl;
	cout << "currentminZ " << currentminZ << endl;
	for (int i = 0; i < 60; i++){
		for (int j = 0; j < 60; j++){
			heightMap[i][j] = 0;
		}
	}

	for (int i = 0; i < m_points.size(); i++){
		heightMap[((int) m_points[i].x) +30][((int) m_points[i].z)+30] = m_points[i].y;
	}

	//for (int i = 0; i < 60; i++){
	//	for (int j = 0; j < 60; j++){
	//		cout << heightMap[i][j]<< endl;
	//	}
	//}

}

void Geometry::readOBJ(string filename) {

	// Make sure our geometry information is cleared
	m_points.clear();
	m_uvs.clear();
	m_normals.clear();
	m_triangles.clear();

	// Load dummy points because OBJ indexing starts at 1 not 0
	m_points.push_back(vec3(0, 0, 0));
	m_uvs.push_back(vec2(0, 0));
	m_normals.push_back(vec3(0, 0, 1));


	ifstream objFile(filename);

	if (!objFile.is_open()) {
		cerr << "Error reading " << filename << endl;
		throw runtime_error("Error :: could not open file.");
	}

	cout << "Reading file " << filename << endl;

	// good() means that failbit, badbit and eofbit are all not set
	while (objFile.good()) {

		// Pull out line from file
		string line;
		std::getline(objFile, line);
		istringstream objLine(line);
		// Pull out mode from line
		string mode;
		objLine >> mode;

		// Reading like this means whitespace at the start of the line is fine
		// attempting to read from an empty string/line will set the failbit
		if (!objLine.fail()) {

			if (mode == "v") {
				vec3 v;
				objLine >> v.x >> v.y >> v.z;
				m_points.push_back(v);

			}
			else if (mode == "vn") {
				vec3 vn;
				objLine >> vn.x >> vn.y >> vn.z;
				m_normals.push_back(vn);

			}
			else if (mode == "vt") {
				vec2 vt;
				objLine >> vt.x >> vt.y;
				m_uvs.push_back(vt);

			}
			else if (mode == "f") {

				vector<vertex> verts;
				while (objLine.good()){
					vertex v;

					// Assignment code (assumes you have all of v/vt/vn for each vertex)
					objLine >> v.p;		// Scan in position index
					if (m_normals.size()>1)
						objLine.ignore(1);	// Ignore the '/' character
					if (m_uvs.size()>1)
						objLine >> v.t;		// Scan in uv (texture coord) index
					if (m_normals.size()>1)
						objLine.ignore(1);	// Ignore the '/' character
					if (m_normals.size()>1)
						objLine >> v.n;		// Scan in normal index

					verts.push_back(v);
				}

				// IFF we have 3 verticies, construct a triangle
				if (verts.size() == 3){
					triangle tri;
					tri.v[0] = verts[0];
					tri.v[1] = verts[1];
					tri.v[2] = verts[2];
					m_triangles.push_back(tri);
				}
			}
		}
	}

	cout << "Reading OBJ file is DONE." << endl;
	cout << m_points.size() - 1 << " points" << endl;
	cout << m_uvs.size() - 1 << " uv coords" << endl;
	cout << m_normals.size() - 1 << " normals" << endl;
	cout << m_triangles.size() << " faces" << endl;


	// If we didn't have any normals, create them
	if (m_normals.size() <= 1) createNormals();
}

void Geometry::createNormals(){
	//map of point index: normal
	typedef map<int, vec3>pointToNormalMap;
	pointToNormalMap m;
	for (unsigned int i = 0; i < m_triangles.size(); i++){
		vec3 u = m_points[m_triangles[i].v[1].p] - m_points[m_triangles[i].v[0].p];
		vec3 v = m_points[m_triangles[i].v[2].p] - m_points[m_triangles[i].v[1].p];
		vec3 normal = cross(u, v);
		normal.x = normal.x / length(normal);
		normal.y = normal.y / length(normal);
		normal.z = normal.z / length(normal);

		//add the normals to the map
		for (int j = 0; j<3; j++){
			m[m_triangles[i].v[j].p] += normal;
		}
	}
	for (unsigned int i = 0; i < m_triangles.size(); i++){
		for (int j = 0; j<3; j++){
			vec3 normal = m[m_triangles[i].v[j].p];
			normal.x = normal.x / length(normal);
			normal.y = normal.y / length(normal);
			normal.z = normal.z / length(normal);
			m_normals.push_back(normal);
			m_triangles[i].v[j].n = m_normals.size() - 1;
		}
	}
}


void Geometry::createDisplayListPoly() {
	// Delete old list if there is one
	if (m_displayListPoly) glDeleteLists(m_displayListPoly, 1);

	// Create a new list
	cout << "Creating Poly Geometry" << endl;
	m_displayListPoly = glGenLists(1);
	glNewList(m_displayListPoly, GL_COMPILE); 
	//glColor3f(0.2, 0.1, 0);

	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess*128.0);

	glBegin(GL_TRIANGLES);
	for (unsigned int i = 0; i < m_triangles.size(); i++){
		for (int j = 0; j<3; j++){
			//Normals
			float nx = m_normals[m_triangles[i].v[j].n].x;
			float ny = m_normals[m_triangles[i].v[j].n].y;
			float nz = m_normals[m_triangles[i].v[j].n].z;
			glNormal3f(nx, ny, nz);

			//UV
			float tx = m_uvs[m_triangles[i].v[j].t].x;
			float ty = m_uvs[m_triangles[i].v[j].t].y;
			glTexCoord2f(tx, ty);

			//Points
			float x = m_points[m_triangles[i].v[j].p].x;
			float y = m_points[m_triangles[i].v[j].p].y;
			float z = m_points[m_triangles[i].v[j].p].z;
			glVertex3f(x, y, z);
		}
	}
	glEnd();

	glEndList();
	cout << "Finished creating Poly Geometry" << endl;
}

void Geometry::renderGeometry() {		
	glShadeModel(GL_SMOOTH);
	glCallList(m_displayListPoly);
}