#include "cube.h"
#include "sphere.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "texture.hpp"

enum eShadeMode { NO_LIGHT, GOURAUD, PHONG, NUM_LIGHT_MODE };

glm::mat4 projectMat = glm::perspective(glm::radians(65.0f), 1.0f, 0.1f, 100.0f);
glm::mat4 viewMat = glm::lookAt(glm::vec3(4.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
glm::mat4 modelMat = glm::mat4(1.0f);

//GLuint pvmMatrixID;

int shadeMode = NO_LIGHT;
int isTexture = false;
int isRotate = false;

// 큐브와 구를 위한 VAO 및 셰이더 프로그램 ID
GLuint cubeVAO;
GLuint cubeProgram;
GLuint sphereVAO;
GLuint sphereProgram;

//lighting & texture
GLuint projectMatrixID;
GLuint viewMatrixID;
GLuint modelMatrixID;
GLuint shadeModeID_cube;
GLuint textureModeID_cube;

GLuint shadeModeID;
GLuint textureModeID;

Sphere sphere(50, 50);

GLuint sphereModelID;
GLuint sphereViewID;
GLuint sphereProjID;


//초기 각도 선언
float leftArmAngle = 0.0;
float leftForeArmAngle = 0.0;
float rightArmAngle = 0.0;
float rightForeArmAngle = 0.0;
float lUpperLegAngle = 0.0;
float lLowerLegAngle = 0.0;
float rUpperLegAngle = 0.0;
float rLowerLegAngle = 0.0;

// keyframe
//왼윗팔, 왼아랫팔, 오른윗팔, 오른아랫팔, 왼허벅지, 왼종아리, 오른허벅지, 오른종아리
float keyFrame[8][6] = {
    {-90.0f, 0.0f, 90.0f, 120.0f, 180.0f, -135.0f},	//왼윗팔
    {0.0f, -90.0f, -90.0f, -90.0f, 0.0f, 0.0f},	//왼아랫팔
    {120.0f, 180.0f, -135.0f,-90.0f, 0.0f, 90.0f},	//오른윗팔
    {-90.0f, 0.0f, 0.0f, 0.0f, -90.0f, -90.0f},	//오른아랫팔
    {30.0f, 30.0f, 0.0f, 0.0f, -30.0f, 0.0f},	//왼허벅지
    {0.0f, 30.0f, 30.0f, 0.0f, 0.0f, 0.0f},	//왼종아리
    {0.0f, -30.0f, 0.0f, 30.0f, 30.0f, 0.0f},	//오른허벅지
    {0.0f, 0.0f, 0.0f, 0.0f, 30.0f, 30.0f}	//오른종아리
};

typedef glm::vec4  color4;
typedef glm::vec4  point4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices]; //정육면체를 만드는데 필요한 모든 vertex 정보
color4 colors[NumVertices];
point4 normals[NumVertices];

//각 vertex에 대한 position
point4 vertices[8] = {
	point4(-0.5, -0.5, 0.5, 1.0),
	point4(-0.5, 0.5, 0.5, 1.0),
	point4(0.5, 0.5, 0.5, 1.0),
	point4(0.5, -0.5, 0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5, 0.5, -0.5, 1.0),
	point4(0.5, 0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0)
};

//각 vertex에 대한 colors
color4 vertex_colors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),  // black
	color4(0.0, 1.0, 1.0, 1.0),   // cyan
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 1.0, 1.0, 1.0)  // white
};


// vertex(점) 4개 받아서 삼각형 2개 제작 (v0,v1,v2/v0,v2,v3)
int Index = 0;
void
quad(int a, int b, int c, int d) //각각이 v0,v1,v2,v3라고 생각해라
{
	// 한 면(두 삼각형)의 노말 벡터 계산
    point4 u = vertices[b] - vertices[a];
    point4 v = vertices[c] - vertices[b];
    
    // 외적(cross product)을 사용하여 노말 계산
    // vec3로 변환 후 계산하고 다시 vec4로
    glm::vec3 normal = glm::normalize(glm::cross(glm::vec3(u), glm::vec3(v)));
    point4 normal_vec4 = point4(normal.x, normal.y, normal.z, 0.0); // w=0 (방향)

    // 6개의 정점 모두에 같은 노말 벡터, 색상, 위치를 할당
    normals[Index] = normal_vec4; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    normals[Index] = normal_vec4; colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
    normals[Index] = normal_vec4; colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
    normals[Index] = normal_vec4; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    normals[Index] = normal_vec4; colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
    normals[Index] = normal_vec4; colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;}

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube()
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

//----------------------------------------------------------------------------
// OpenGL initialization
void
init()
{
	colorcube();

	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);

	// Load shaders and use the resulting shader program
	cubeProgram = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(cubeProgram);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(cubeProgram, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(cubeProgram, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points)));
	
	GLuint vNormal = glGetAttribLocation(cubeProgram, "vNormal");
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0,
        BUFFER_OFFSET(sizeof(points) + sizeof(colors)));

	//pvmMatrixID = glGetUniformLocation(program, "mPVM");
	projectMatrixID = glGetUniformLocation(cubeProgram, "mProject");
	glUniformMatrix4fv(projectMatrixID, 1, GL_FALSE, &projectMat[0][0]);

	viewMatrixID = glGetUniformLocation(cubeProgram, "mView");
	glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &viewMat[0][0]);

	modelMatrixID = glGetUniformLocation(cubeProgram, "mModel");
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);

	
	shadeModeID_cube = glGetUniformLocation(cubeProgram, "shadeMode");
	glUniform1i(shadeModeID_cube, shadeMode);

	textureModeID_cube = glGetUniformLocation(cubeProgram, "isTexture");
	glUniform1i(textureModeID_cube, isTexture);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void
initSphere(){
	// Create a vertex array object
	//GLuint sphereVAO[1];
	glGenVertexArrays(1, &sphereVAO);
	glBindVertexArray(sphereVAO);

	// Create and initialize a buffer object
	GLuint buffer[1];
	glGenBuffers(1, buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);

	int vertSize = sizeof(sphere.verts[0])*sphere.verts.size();
	int normalSize = sizeof(sphere.normals[0])*sphere.normals.size();
	int texSize = sizeof(sphere.texCoords[0])*sphere.texCoords.size();
	glBufferData(GL_ARRAY_BUFFER, vertSize + normalSize + texSize,
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertSize, sphere.verts.data());
	glBufferSubData(GL_ARRAY_BUFFER, vertSize, normalSize, sphere.normals.data());
	glBufferSubData(GL_ARRAY_BUFFER, vertSize+normalSize, texSize, sphere.texCoords.data());

	// Load shaders and use the resulting shader program
	sphereProgram = InitShader("sphere_vshader.glsl", "sphere_fshader.glsl");
	glUseProgram(sphereProgram);

	// set up vertex arrays vshader에 입력으로 들어감
	GLuint vPosition = glGetAttribLocation(sphereProgram, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(sphereProgram, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(vertSize));

	GLuint vTexCoord = glGetAttribLocation(sphereProgram, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(vertSize+normalSize));

	sphereProjID = glGetUniformLocation(sphereProgram, "mProject");
	glUniformMatrix4fv(sphereProjID, 1, GL_FALSE, &projectMat[0][0]);

	sphereViewID = glGetUniformLocation(sphereProgram, "mView");
	glUniformMatrix4fv(sphereViewID, 1, GL_FALSE, &viewMat[0][0]);

	sphereModelID = glGetUniformLocation(sphereProgram, "mModel");
	glUniformMatrix4fv(sphereModelID, 1, GL_FALSE, &modelMat[0][0]);

	shadeModeID = glGetUniformLocation(sphereProgram, "shadeMode");
	glUniform1i(shadeModeID, shadeMode);

	textureModeID = glGetUniformLocation(sphereProgram, "isTexture");
	glUniform1i(textureModeID, isTexture);

	// Load the texture using any two methods
	GLuint Texture = loadBMP_custom("earth.bmp");
	//GLuint Texture = loadDDS("uvtemplate.DDS");

	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID = glGetUniformLocation(sphereProgram, "sphereTexture");

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);

	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(TextureID, 0);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
}
//----------------------------------------------------------------------------

void drawSphere(glm::mat4 modelMat)
{
    glUseProgram(sphereProgram);
    glBindVertexArray(sphereVAO);

    glUniformMatrix4fv(sphereModelID, 1, GL_FALSE, &modelMat[0][0]);
	glUniformMatrix4fv(sphereViewID, 1, GL_FALSE, &viewMat[0][0]); 
    glUniformMatrix4fv(sphereProjID, 1, GL_FALSE, &projectMat[0][0]);
	glUniform1i(shadeModeID, shadeMode);
    glUniform1i(textureModeID, isTexture);
    glDrawArrays(GL_TRIANGLES, 0, sphere.verts.size());
}

void drawCube(glm::mat4 modelMat)
{ 	
    glUseProgram(cubeProgram);
    glBindVertexArray(cubeVAO);

	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);
    glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &viewMat[0][0]);
    glUniformMatrix4fv(projectMatrixID, 1, GL_FALSE, &projectMat[0][0]);

	glUniform1i(shadeModeID_cube, shadeMode);
    glUniform1i(textureModeID_cube, isTexture);

	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}

void drawHuman(glm::mat4 modelMat)
{
	//몸통
	glm::mat4 bodyMat = glm::scale(modelMat, glm::vec3(0.5,0.8, 0.2));
	drawCube(bodyMat);

	//머리
	glm::mat4 headMat = glm::translate(modelMat, glm::vec3(0.0, 0.6, 0.0));
    headMat = glm::scale(headMat, glm::vec3(0.4, 0.3, 0.2));
	drawSphere(headMat);

	{
		// 왼팔
		glm::mat4 lArm = glm::translate(modelMat, glm::vec3(0.375, 0.4, 0.0));
		lArm = glm::rotate(lArm, glm::radians(leftArmAngle), glm::vec3(1, 0, 0));
		lArm = glm::translate(lArm, glm::vec3(0.0, -0.2, 0.0)); 
		//그리는 맷을 따로 두고 스케일 안된거를 자식에게 물려주기
		glm::mat4 lArmDrawMat = glm::scale(lArm, glm::vec3(0.15, 0.4, 0.15));
		drawCube(lArmDrawMat);

			// 왼아래팔
			glm::mat4 lForeArm = glm::translate(lArm, glm::vec3(0.0, -0.2, 0)); 
			lForeArm = glm::rotate(lForeArm, glm::radians(leftForeArmAngle), glm::vec3(1, 0, 0));
			lForeArm = glm::translate(lForeArm, glm::vec3(0.0, -0.25, 0.0)); 
			
			glm::mat4 lForeArmDrawMat = glm::scale(lForeArm, glm::vec3(0.15, 0.4, 0.15));
			drawCube(lForeArmDrawMat);
	}

	{
		// 오른팔
		glm::mat4 rArm = glm::translate(modelMat, glm::vec3(-0.375, 0.4, 0.0));
		rArm = glm::rotate(rArm, glm::radians(rightArmAngle), glm::vec3(1, 0, 0));
		rArm = glm::translate(rArm, glm::vec3(0.0, -0.2, 0.0)); 
		glm::mat4 rArmDrawMat = glm::scale(rArm, glm::vec3(0.15, 0.4, 0.15));
		drawCube(rArmDrawMat);

			// 오른아래팔 
			glm::mat4 rForeArm = glm::translate(rArm, glm::vec3(0.0, -0.2, 0)); 
			rForeArm = glm::rotate(rForeArm, glm::radians(rightForeArmAngle), glm::vec3(1, 0, 0));
			rForeArm = glm::translate(rForeArm, glm::vec3(0.0, -0.25, 0.0)); 
			
			glm::mat4 rForeArmDrawMat = glm::scale(rForeArm, glm::vec3(0.15, 0.4, 0.15));
			drawCube(rForeArmDrawMat);
	}

	{
		//왼 허벅지
		glm::mat4 lUpperLeg = glm::translate(modelMat, glm::vec3(0.13, -0.4, 0.0));
		lUpperLeg = glm::rotate(lUpperLeg, glm::radians(lUpperLegAngle), glm::vec3(1, 0, 0));
		lUpperLeg = glm::translate(lUpperLeg, glm::vec3(0.0, -0.3, 0.0)); 
		glm::mat4 lUpperLegDrawMat = glm::scale(lUpperLeg, glm::vec3(0.2, 0.5, 0.2));
		drawCube(lUpperLegDrawMat);

			// 왼 종아리
			glm::mat4 lLowerLeg = glm::translate(lUpperLeg, glm::vec3(0.0, -0.25, 0)); 
			lLowerLeg = glm::rotate(lLowerLeg, glm::radians(lLowerLegAngle), glm::vec3(1, 0, 0));
			lLowerLeg = glm::translate(lLowerLeg, glm::vec3(0.0, -0.3, 0.0)); 
			
			glm::mat4 lLowerLegDrawMat = glm::scale(lLowerLeg, glm::vec3(0.2, 0.5, 0.2));
			drawCube(lLowerLegDrawMat);
	}
{
		//오른 허벅지
		glm::mat4 rUpperLeg = glm::translate(modelMat, glm::vec3(-0.13, -0.4, 0.0));
		rUpperLeg = glm::rotate(rUpperLeg, glm::radians(rUpperLegAngle), glm::vec3(1, 0, 0));
		rUpperLeg = glm::translate(rUpperLeg, glm::vec3(0.0, -0.3, 0.0)); 
		glm::mat4 rUpperLegDrawMat = glm::scale(rUpperLeg, glm::vec3(0.2, 0.5, 0.2));
		drawCube(rUpperLegDrawMat);

			// 오른 종아리
			glm::mat4 rLowerLeg = glm::translate(rUpperLeg, glm::vec3(0.0, -0.25, 0)); 
			rLowerLeg = glm::rotate(rLowerLeg, glm::radians(rLowerLegAngle), glm::vec3(1, 0, 0));
			rLowerLeg = glm::translate(rLowerLeg, glm::vec3(0.0, -0.3, 0.0)); 
			
			glm::mat4 rLowerLegDrawMat = glm::scale(rLowerLeg, glm::vec3(0.2, 0.5, 0.2));
			drawCube(rLowerLegDrawMat);
	}
}


void display(void)
{
	glm::mat4 worldMat, pvmMat;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	worldMat = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	
	drawHuman(worldMat);

	glutSwapBuffers();
}

//----------------------------------------------------------------------------
// 정규화: 각도를 (-180, 180] (또는 [-180,180]) 범위로 맞춤
static float normalizeAngle(float a) {
    while (a <= -180.0f) a += 360.0f;
    while (a > 180.0f)  a -= 360.0f;
    return a;
}

// 보간 함수 (각도 래핑 처리 포함)
float getInterpolatedAngle(float* keyframes, int numKeys, float time, float cycleDuration) {
    if (numKeys <= 0) return 0.0f;

    float t = fmod(time, cycleDuration) / cycleDuration; // 0..1
    float seg = t * numKeys;
    int curr = int(floor(seg)) % numKeys;
    if (curr < 0) curr += numKeys;
    int next = (curr + 1) % numKeys;
    float localT = seg - floor(seg); // 구간 내 비율 0..1

    float a0 = normalizeAngle(keyframes[curr]);
    float a1 = normalizeAngle(keyframes[next]);

	float diff = normalizeAngle(a1-a0);

    float angle = a0 + diff * localT;

    return normalizeAngle(angle);
}

void idle()
{
	static int prevTime = glutGet(GLUT_ELAPSED_TIME);
	int currTime = glutGet(GLUT_ELAPSED_TIME);
	prevTime = currTime;

	float cycleDuration = 3.0f;

	float elapsedTime = currTime / 1000.0f; // 경과 시간

	//팔
	leftArmAngle = getInterpolatedAngle(keyFrame[0], 6, elapsedTime, cycleDuration);
	leftForeArmAngle = getInterpolatedAngle(keyFrame[1], 6, elapsedTime, cycleDuration);
	rightArmAngle = getInterpolatedAngle(keyFrame[2], 6, elapsedTime, cycleDuration);
	rightForeArmAngle = getInterpolatedAngle(keyFrame[3], 6, elapsedTime, cycleDuration);
	//다리
	lUpperLegAngle = getInterpolatedAngle(keyFrame[4], 6, elapsedTime, cycleDuration);
    lLowerLegAngle = getInterpolatedAngle(keyFrame[5], 6, elapsedTime, cycleDuration);
    rUpperLegAngle = getInterpolatedAngle(keyFrame[6], 6, elapsedTime, cycleDuration);
    rLowerLegAngle = getInterpolatedAngle(keyFrame[7], 6, elapsedTime, cycleDuration);

    glutPostRedisplay();
}

//----------------------------------------------------------------------------

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '1': // side view
		viewMat = glm::lookAt(
			glm::vec3(4.0f, 0.0f, 0.0f),  
			glm::vec3(0.0f, 1.0f, 0.0f),  
			glm::vec3(0.0f, 1.0f, 0.0f)   
		);
		break;

	case '2': // over-the-shoulder
		viewMat = glm::lookAt(
			glm::vec3(0.0f, 1.5f, -4.0f), 
			glm::vec3(0.0f, 1.0f, 0.0f),   
			glm::vec3(0.0f, 1.0f, 0.0f)
		);
		break;

	case '3': // front view
		viewMat = glm::lookAt(
			glm::vec3(0.0f, 1.5f, 4.0f),   
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);
		break;
	case 'l': case 'L':
		shadeMode = (++shadeMode % NUM_LIGHT_MODE);
		glUniform1i(shadeModeID, shadeMode);
		glutPostRedisplay();
		break;
	case 'r': case 'R':
		isRotate = !isRotate;
		glutPostRedisplay();
		break;
	case 't': case 'T':
		isTexture = !isTexture;
		glUniform1i(textureModeID, isTexture);
		glutPostRedisplay();
		break;
	case 033:  // Escape key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;

	}

	glutPostRedisplay(); // 카메라 즉시 반영
}


//----------------------------------------------------------------------------

void resize(int w, int h)
{
	float ratio = (float)w / (float)h;
	glViewport(0, 0, w, h);

	projectMat = glm::perspective(glm::radians(65.0f), ratio, 0.1f, 100.0f);

	glutPostRedisplay();
}

//----------------------------------------------------------------------------

int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Swimming CubeMan");

	glewInit();

	init();
	initSphere();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}
