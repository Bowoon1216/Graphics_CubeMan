#include "sphere.h"

void Sphere::makeUV(int nLongi, int nLati)
{
	int nverts = nLongi*nLati;
	float radius = 1;
	vector<glm::vec4> vertList;

	nLongitude = nLongi;
	nLatitude = nLati;
	// make vertex list
	for (int v = 0; v < nLati + 1; v++)
	{
		for (int u = 0; u < nLongi; u++)
		{
			float theta = 2 * PI * u / nLongi; // longitude
			float phi = PI * v / nLati;  // latitude
			float x = glm::sin(phi) * glm::cos(theta) * radius;
			float y = glm::sin(phi) * glm::sin(theta) * radius;
			float z = glm::cos(phi) * radius;
			vertList.push_back(glm::vec4(x, y, z, 1));
		}
	}

	// make triangles
	for (int v = 0; v < nLati; v++)
	{
		for (int u = 0; u < nLongi; u++)
		{
			int u2 = (u + 1) % nLongi, v2 = v + 1;
			// v0=(u, v)    v1=(u2, v)   <= quadangle
			// v2=(u, v2)   v3=(u2, v2)

			// triangle (u, v), (u, v2), (u2, v2)
			verts.push_back(vertList[u + v * nLongi]);  // v0
			verts.push_back(vertList[u + v2 * nLongi]);  // v2
			verts.push_back(vertList[u2 + v2 * nLongi]);  // v3

			// triangle (u, v), (u2, v2), (u2, v)
			verts.push_back(vertList[u + v * nLongi]);  // v0
			verts.push_back(vertList[u2 + v2 * nLongi]);  // v3
			verts.push_back(vertList[u2 + v * nLongi]);  // v1

			glm::vec2 texcoord[4];
			computeTexCoordQuad(texcoord, u, v, u2, v2);
			texCoords.push_back(texcoord[0]);
			texCoords.push_back(texcoord[2]);
			texCoords.push_back(texcoord[3]);

			texCoords.push_back(texcoord[0]);
			texCoords.push_back(texcoord[3]);
			texCoords.push_back(texcoord[1]);
		}
	}
}


void Sphere::computeNormals()
{
	for (int i = 0; i < verts.size(); i++)
	{
		glm::vec4 n;
		for (int k = 0; k < 3; k++)
		{
			n[k] = verts[i][k];
		}
		n[3] = 0.0;
		glm::normalize(n);
		normals.push_back(n);
	}
}

void Sphere::computeTexCoordQuad(glm::vec2 texcoord[4], int u, int v, int u2, int v2)
{
	const int U = 0, V = 1;
	// v0=(u, v)    v1=(u2, v)   <= quadangle
	// v2=(u, v2)   v3=(u2, v2)

	texcoord[0][U] = texcoord[2][U] = (float)u / nLongitude;
	texcoord[1][U] = texcoord[3][U] = (float)u2 / nLongitude;

	texcoord[0][V] = texcoord[1][V] = (float)v / nLatitude;
	texcoord[2][V] = texcoord[3][V] = (float)v2 / nLatitude;

	if (u2 == 0) // last column
	{
		texcoord[1][U] = texcoord[3][U] = 1.0;
	}
	//texcoord[u] = atan2(normals[y], normals[x]) / (2 * PI) + 0.5;
	//texcoord[v] = acos(normals[z]/sqrt(length(normal))) / PI;
}
