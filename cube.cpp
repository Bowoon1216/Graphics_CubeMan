//
// Display a color cube
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.  We us an orthographic projection
//   as the default projetion.

#include "cube.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

glm::mat4 projectMat;
glm::mat4 viewMat;

GLuint pvmMatrixID;


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

// Vertices of a unit cube centered at origin, sides aligned with axes
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

// RGBA colors
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

//----------------------------------------------------------------------------

// vertex(점) 4개 받아서 삼각형 2개 제작 (v0,v1,v2/v0,v2,v3)
int Index = 0;
void
quad(int a, int b, int c, int d) //각각이 v0,v1,v2,v3라고 생각해라
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a];  Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b];  Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c];  Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a];  Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c];  Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d];  Index++;
}

//----------------------------------------------------------------------------

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

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

	// Load shaders and use the resulting shader program
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points)));

	pvmMatrixID = glGetUniformLocation(program, "mPVM");

	projectMat = glm::perspective(glm::radians(65.0f), 1.0f, 0.1f, 100.0f);
	viewMat = glm::lookAt(glm::vec3(4.0f, 0.0f, 0.0f), glm::vec3(0, 1.0f, 0), glm::vec3(0, 1, 0));


	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

//----------------------------------------------------------------------------

void drawCube(glm::mat4 modelMat)
{ 	
	//몸의 한 덩어리 그리기
	glm::mat4 pvmMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
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
    drawCube(headMat);

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

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}
