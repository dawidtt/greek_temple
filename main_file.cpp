/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "myCube.h"
#include "myTeapot.h"

#include "load.h"
#include <vector>
#include <iostream>
#include "Object3D.h"

Object3D olimpia_floor;
Object3D olimpia_roof_base;
Object3D olimpia_roof_decorations;
Object3D olimpia_roof_roof;
Object3D olimpia_roof_others;
Object3D olimpia_wall_front;
Object3D olimpia_wall_right;
Object3D olimpia_wall_left;
Object3D olimpia_wall_back;
Object3D ceramic_vase;
//Object3D gold_vase;
Object3D statue1;
//Object3D statue2;
//Object3D statue3;
Object3D horse_head;
Object3D stand;



Object3D column1;
Object3D column2;
Object3D column3;
Object3D column4;
Object3D column5;
Object3D column6;
Object3D column7;
Object3D column8;
Object3D column9;
Object3D column10;
Object3D column11;
Object3D column12;
Object3D column13;
Object3D column14;
Object3D column15;
Object3D column16;
Object3D column17;
Object3D column18;
Object3D column19;
Object3D column20;
Object3D column21;
Object3D column22;
Object3D column23;
Object3D column24;
Object3D column25;
Object3D column26;
Object3D column27;
Object3D column28;
Object3D column29;
Object3D column30;
Object3D column31;
Object3D column32;
Object3D column33;
Object3D column34;
Object3D column35;



Object3D grass;


float speed_x=0;
float speed_y=0;
float aspectRatio=1;


float vaseStartY = 0.18f;
float vaseAmplitude = 0.02f; // Waza będzie unosić się o 0.2 jednostki w górę i o 0.2 w dół
float vaseFreq = 2.0f;

ShaderProgram *sp;
ShaderProgram* sp_shadow;
ShaderProgram* sp_point_shadow;

//Odkomentuj, żeby rysować kostkę
//float* vertices = myCubeVertices;
//float* normals = myCubeNormals;
//float* texCoords = myCubeTexCoords;
//float* colors = myCubeColors;
//int vertexCount = myCubeVertexCount;


//Odkomentuj, żeby rysować czajnik
//float* vertices = myTeapotVertices;
//float* normals = myTeapotVertexNormals;
//float* texCoords = myTeapotTexCoords;
//float* colors = myTeapotColors;
//int vertexCount = myTeapotVertexCount;




bool moveForward = false;
bool moveBackward = false;
bool rotateLeft = false;
bool rotateRight = false;
float moveSpeed = 1.4f;
float turnSpeed = 2.0f; // stopnie

glm::vec3 cameraPos = glm::vec3(-0.5f, 0.2f, 2.8f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


float yaw = -90.0f; // 0° to w kierunku x, -90° to w kierunku z (do przodu)

GLuint tex0;
GLuint tex1;

GLuint depthMapFBO;
GLuint depthMap;
const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;

GLuint pointDepthMapFBO, pointDepthMap;
const unsigned int POINT_SHADOW_WIDTH = 1, POINT_SHADOW_HEIGHT = 1;
float far_plane_point = 15.0f;

struct border {
	glm::vec3 center;
	float radius; // Promień zakazanego obszaru
};

// Globalna tablica zakazanych obszarów
std::vector<border> borders;


bool isInBorder(const glm::vec3& position) {
	for (const auto& area : borders) {
		// Obliczamy różnice współrzędnych
		float dx = position.x - area.center.x;
		float dy = position.y - area.center.y;
		float dz = position.z - area.center.z;

		// Obliczamy kwadrat odległości
		float distanceSq = (dx * dx) + (dy * dy) + (dz * dz);

		// Obliczamy kwadrat promienia
		float radiusSq = area.radius * area.radius;

		// Sprawdzamy, czy pozycja jest wewnątrz zakazanego obszaru (sfery)
		if (distanceSq < radiusSq) {
			return true; // Pozycja jest w tym zakazanym obszarze
		}
	}
	return false; // Pozycja nie znajduje się w żadnym zakazanym obszarze
}
void addBorder(const glm::vec3& position, float radius) {
	borders.push_back({ position, radius });
}


void updateCameraFront(float yaw) {
	float rad = glm::radians(yaw);
	cameraFront.x = cos(rad);
	cameraFront.y = 0.0f; // płaska kamera (tylko obrót w poziomie)
	cameraFront.z = sin(rad);
	cameraFront = glm::normalize(cameraFront);
}


//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}


void keyCallback(GLFWwindow* window,int key,int scancode,int action,int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_W) moveForward = true;
		if (key == GLFW_KEY_S) moveBackward = true;
		if (key == GLFW_KEY_A) rotateLeft = true;
		if (key == GLFW_KEY_D) rotateRight = true;
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_W) moveForward = false;
		if (key == GLFW_KEY_S) moveBackward = false;
		if (key == GLFW_KEY_A) rotateLeft = false;
		if (key == GLFW_KEY_D) rotateRight = false;
	}
}

void windowResizeCallback(GLFWwindow* window,int width,int height) {
    if (height==0) return;
    aspectRatio=(float)width/(float)height;
    glViewport(0,0,width,height);
}


GLuint readTexture(const char* filename) {
    GLuint tex;
    glActiveTexture(GL_TEXTURE0);

    //Wczytanie do pamięci komputera
    std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
    unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
    //Wczytaj obrazek
    unsigned error = lodepng::decode(image, width, height, filename);

    //Import do pamięci karty graficznej
    glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
    glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
    //Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return tex;
}


//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE); // Optymalizacja, nie rysuje tylnych ścian trójkątów
	glfwSetWindowSizeCallback(window,windowResizeCallback);
	glfwSetKeyCallback(window,keyCallback);

	sp=new ShaderProgram("v_simplest.glsl",NULL,"f_simplest.glsl");
	sp_shadow = new ShaderProgram("shadow_shader.vert", NULL, "shadow_shader.frag");
	sp_point_shadow = new ShaderProgram("point_shadow.vert", "point_shadow.geom", "point_shadow.frag");

	// Słońce
	glGenFramebuffers(1, &depthMapFBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//światło w środku
	glGenFramebuffers(1, &pointDepthMapFBO);
	glGenTextures(1, &pointDepthMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, pointDepthMap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			POINT_SHADOW_WIDTH, POINT_SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindFramebuffer(GL_FRAMEBUFFER, pointDepthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, pointDepthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	olimpia_floor.loadModel("olimpia_floor.obj");
	olimpia_floor.position = glm::vec3(0.0f, -0.09f, 1.0f);
	olimpia_floor.scale = glm::vec3(0.125f);
	olimpia_floor.rotation = glm::vec3(0.0f, -90.0f, 0.0f);
	olimpia_floor.tex0 = readTexture("marble_column.png");
	olimpia_floor.tex1 = readTexture("marble_column_normal.png");


	olimpia_roof_base.loadModel("olimpia_roof_base.obj");
	olimpia_roof_base.position = glm::vec3(0.04f, -0.65f, -1.2f);
	olimpia_roof_base.scale = glm::vec3(0.132f);
	olimpia_roof_base.rotation = glm::vec3(0.0f, -90.0f, 0.0f);
	olimpia_roof_base.tex0 = readTexture("marble_column.png");
	olimpia_roof_base.tex1 = readTexture("marble_column_normal.png");

	olimpia_roof_decorations.loadModel("olimpia_roof_decorations.obj");
	olimpia_roof_decorations.position = glm::vec3(0.04f, -0.65f, -1.2f);
	olimpia_roof_decorations.scale = glm::vec3(0.132f);
	olimpia_roof_decorations.rotation = glm::vec3(0.0f, -90.0f, 0.0f);
	olimpia_roof_decorations.tex0 = readTexture("roof_decorations.png");
	olimpia_roof_decorations.tex1 = readTexture("roof_decorations_normal.png");


	olimpia_roof_roof.loadModel("olimpia_roof_roof.obj");
	olimpia_roof_roof.position = glm::vec3(0.04f, -0.65f, -1.2f);
	olimpia_roof_roof.scale = glm::vec3(0.132f);
	olimpia_roof_roof.rotation = glm::vec3(0.0f, -90.0f, 0.0f);
	olimpia_roof_roof.tex0 = readTexture("marble_column.png");
	olimpia_roof_roof.tex1 = readTexture("marble_column_normal.png");

	olimpia_roof_others.loadModel("olimpia_roof_others.obj");
	olimpia_roof_others.position = glm::vec3(0.04f, -0.65f, -1.2f);
	olimpia_roof_others.scale = glm::vec3(0.132f);
	olimpia_roof_others.rotation = glm::vec3(0.0f, -90.0f, 0.0f);
	olimpia_roof_others.tex0 = readTexture("roof_decorations.png");
	olimpia_roof_others.tex1 = readTexture("roof_decorations_normal.png");

	olimpia_wall_front.loadModel("olimpia_wall_front.obj");
	olimpia_wall_front.position = glm::vec3(0.04f, -0.22f, -1.0f);
	olimpia_wall_front.scale = glm::vec3(0.132f);
	olimpia_wall_front.rotation = glm::vec3(0.0f, -90.0f, 0.0f);
	olimpia_wall_front.tex0 = readTexture("marble_column.png");
	olimpia_wall_front.tex1 = readTexture("marble_column_normal.png");

	olimpia_wall_right.loadModel("olimpia_wall_right.obj");
	olimpia_wall_right.position = glm::vec3(0.05f, -0.8f, -1.4f);
	olimpia_wall_right.scale = glm::vec3(0.138f);
	olimpia_wall_right.rotation = glm::vec3(0.0f, -90.0f, 0.0f);
	olimpia_wall_right.tex0 = readTexture("marble_column.png");
	olimpia_wall_right.tex1 = readTexture("marble_column_normal.png");

	olimpia_wall_left.loadModel("olimpia_wall_left.obj");
	olimpia_wall_left.position = glm::vec3(0.05f, -0.8f, -1.4f);
	olimpia_wall_left.scale = glm::vec3(0.138f);
	olimpia_wall_left.rotation = glm::vec3(0.0f, -90.0f, 0.0f);
	olimpia_wall_left.tex0 = readTexture("marble_column.png");
	olimpia_wall_left.tex1 = readTexture("marble_column_normal.png");

	olimpia_wall_back.loadModel("olimpia_wall_back.obj");
	olimpia_wall_back.position = glm::vec3(0.04f, -0.24f, -1.4f);
	olimpia_wall_back.scale = glm::vec3(0.132f);
	olimpia_wall_back.rotation = glm::vec3(0.0f, -90.0f, 0.0f);
	olimpia_wall_back.tex0 = readTexture("marble_column.png");
	olimpia_wall_back.tex1 = readTexture("marble_column_normal.png");


	//gold_vase.loadModel("gold_vase.obj");
	//gold_vase.position = glm::vec3(-0.2f, 0.0f, 3.0f);
	//gold_vase.scale = glm::vec3(0.6f);
	//gold_vase.rotation = glm::vec3(0.0f, -90.0f, 0.0f);
	//gold_vase.tex0 = readTexture("gold2.png");
	//gold_vase.tex1 = readTexture("gold2_normal.png");

	statue1.loadModel("statue1.obj");
	statue1.position = glm::vec3(-0.2f, 0.0f, 3.0f);
	statue1.scale = glm::vec3(0.6f);
	statue1.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	statue1.tex0 = readTexture("marble_column.png");
	statue1.tex1 = readTexture("marble_column_normal.png");

	addBorder(statue1.position, 0.3f);


	//statue2.loadModel("statue2.obj");
	//statue2.position = glm::vec3(-0.2f, 0.0f, 3.0f);
	//statue2.scale = glm::vec3(0.6f);
	//statue2.rotation = glm::vec3(0.0f, -90.0f, 0.0f);
	//statue2.tex0 = readTexture("marble_column.png");
	//statue2.tex1 = readTexture("marble_column_normal.png");


	//statue3.loadModel("statue3.obj");
	//statue3.position = glm::vec3(0.0f, 0.0f, 3.0f);
	//statue3.scale = glm::vec3(0.01f);
	//statue3.rotation = glm::vec3(90.0f, 90.0f, 180.0f);
	//statue3.tex0 = readTexture("marble_column.png");
	//statue3.tex1 = readTexture("marble_column_normal.png");


	horse_head.loadModel("horse_head.obj");
	horse_head.position = glm::vec3(-1.0f, 0.0f, 3.0f);
	horse_head.scale = glm::vec3(0.6f);
	horse_head.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	horse_head.tex0 = readTexture("marble_column.png");
	horse_head.tex1 = readTexture("marble_column_normal.png");

	addBorder(horse_head.position, 0.3f);



	ceramic_vase.loadModel("ceramic_vase.obj");
	ceramic_vase.position = glm::vec3(-0.6f, vaseStartY, 0.0f);
	ceramic_vase.scale = glm::vec3(0.4f);
	ceramic_vase.rotation = glm::vec3(0.0f, -90.0f, 0.0f);
	ceramic_vase.tex0 = readTexture("gold.png");
	ceramic_vase.tex1 = readTexture("gold_normal.png");

	stand.loadModel("stand.obj");
	stand.position = glm::vec3(-0.6f, 0.0f, 0.0f);
	stand.scale = glm::vec3(0.2f);
	stand.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	stand.tex0 = readTexture("wood.png");
	stand.tex1 = readTexture("wood_normal.png");

	addBorder(stand.position, 0.26f);


	//front kolumny

	column1.loadModel("doric_column.obj");
	column1.position = glm::vec3(-1.8f, 0.0f, 2.25f);
	column1.scale = glm::vec3(0.085f);
	column1.tex0 = readTexture("marble_column.png");
	column1.tex1 = readTexture("marble_column_normal.png");

	addBorder(column1.position, 0.26f);

	column2.loadModel("doric_column.obj");
	column2.position = glm::vec3(-1.4f, 0.0f, 2.25f);
	column2.scale = glm::vec3(0.085f);
	column2.tex0 = readTexture("marble_column.png");
	column2.tex1 = readTexture("marble_column_normal.png");

	addBorder(column2.position, 0.26f);


	column3.loadModel("doric_column.obj");
	column3.position = glm::vec3(-1.0f, 0.0f, 2.25f);
	column3.scale = glm::vec3(0.085f);
	column3.tex0 = readTexture("marble_column.png");
	column3.tex1 = readTexture("marble_column_normal.png");

	addBorder(column3.position, 0.26f);


	column4.loadModel("doric_column.obj");
	column4.position = glm::vec3(-0.2f, 0.0f, 2.25f);
	column4.scale = glm::vec3(0.085f);
	column4.tex0 = readTexture("marble_column.png");
	column4.tex1 = readTexture("marble_column_normal.png");

	addBorder(column4.position, 0.26f);


	column5.loadModel("doric_column.obj");
	column5.position = glm::vec3(0.2f, 0.0f, 2.25f);
	column5.scale = glm::vec3(0.085f);
	column5.tex0 = readTexture("marble_column.png");
	column5.tex1 = readTexture("marble_column_normal.png");

	addBorder(column5.position, 0.26f);


	column6.loadModel("doric_column.obj");
	column6.position = glm::vec3(0.6f, 0.0f, 2.25f);
	column6.scale = glm::vec3(0.085f);
	column6.tex0 = readTexture("marble_column.png");
	column6.tex1 = readTexture("marble_column_normal.png");

	addBorder(column6.position, 0.26f);


	//tyl kolumny

	column7.loadModel("doric_column.obj");
	column7.position = glm::vec3(0.6f, 0.0f, -2.35f);
	column7.scale = glm::vec3(0.085f);
	column7.tex0 = readTexture("marble_column.png");
	column7.tex1 = readTexture("marble_column_normal.png");

	addBorder(column7.position, 0.26f);


	column8.loadModel("doric_column.obj");
	column8.position = glm::vec3(-1.8f, 0.0f, -2.35f);
	column8.scale = glm::vec3(0.085f);
	column8.tex0 = readTexture("marble_column.png");
	column8.tex1 = readTexture("marble_column_normal.png");

	addBorder(column8.position, 0.26f);


	column9.loadModel("doric_column.obj");
	column9.position = glm::vec3(-1.4f, 0.0f, -2.35f);
	column9.scale = glm::vec3(0.085f);
	column9.tex0 = readTexture("marble_column.png");
	column9.tex1 = readTexture("marble_column_normal.png");

	addBorder(column9.position, 0.26f);


	column10.loadModel("doric_column.obj");
	column10.position = glm::vec3(-1.0f, 0.0f, -2.35f);
	column10.scale = glm::vec3(0.085f);
	column10.tex0 = readTexture("marble_column.png");
	column10.tex1 = readTexture("marble_column_normal.png");

	addBorder(column10.position, 0.26f);


	column11.loadModel("doric_column.obj");
	column11.position = glm::vec3(-0.6f, 0.0f, -2.35f);
	column11.scale = glm::vec3(0.085f);
	column11.tex0 = readTexture("marble_column.png");
	column11.tex1 = readTexture("marble_column_normal.png");

	addBorder(column11.position, 0.26f);


	column12.loadModel("doric_column.obj");
	column12.position = glm::vec3(-0.2f, 0.0f, -2.35f);
	column12.scale = glm::vec3(0.085f);
	column12.tex0 = readTexture("marble_column.png");
	column12.tex1 = readTexture("marble_column_normal.png");

	addBorder(column12.position, 0.26f);


	column13.loadModel("doric_column.obj");
	column13.position = glm::vec3(-0.2f, 0.0f, -2.35f);
	column13.scale = glm::vec3(0.085f);
	column13.tex0 = readTexture("marble_column.png");
	column13.tex1 = readTexture("marble_column_normal.png");

	addBorder(column13.position, 0.26f);


	column14.loadModel("doric_column.obj");
	column14.position = glm::vec3(0.2f, 0.0f, -2.35f);
	column14.scale = glm::vec3(0.085f);
	column14.tex0 = readTexture("marble_column.png");
	column14.tex1 = readTexture("marble_column_normal.png");

	addBorder(column14.position, 0.26f);


	column15.loadModel("doric_column.obj");
	column15.position = glm::vec3(0.6f, 0.0f, -2.35f);
	column15.scale = glm::vec3(0.085f);
	column15.tex0 = readTexture("marble_column.png");
	column15.tex1 = readTexture("marble_column_normal.png");

	addBorder(column15.position, 0.26f);


	// sciany prawo


	column16.loadModel("doric_column.obj");
	column16.position = glm::vec3(0.6f, 0.0f, 1.79f);
	column16.scale = glm::vec3(0.085f);
	column16.tex0 = readTexture("marble_column.png");
	column16.tex1 = readTexture("marble_column_normal.png");

	addBorder(column16.position, 0.26f);



	column17.loadModel("doric_column.obj");
	column17.position = glm::vec3(0.6f, 0.0f, 1.33f);
	column17.scale = glm::vec3(0.085f);
	column17.tex0 = readTexture("marble_column.png");
	column17.tex1 = readTexture("marble_column_normal.png");

	addBorder(column17.position, 0.26f);



	column18.loadModel("doric_column.obj");
	column18.position = glm::vec3(0.6f, 0.0f, 0.87f);
	column18.scale = glm::vec3(0.085f);
	column18.tex0 = readTexture("marble_column.png");
	column18.tex1 = readTexture("marble_column_normal.png");

	addBorder(column18.position, 0.26f);


	column19.loadModel("doric_column.obj");
	column19.position = glm::vec3(0.6f, 0.0f, 0.41f);
	column19.scale = glm::vec3(0.085f);
	column19.tex0 = readTexture("marble_column.png");
	column19.tex1 = readTexture("marble_column_normal.png");

	addBorder(column19.position, 0.26f);


	column20.loadModel("doric_column.obj");
	column20.position = glm::vec3(0.6f, 0.0f, -0.05f);
	column20.scale = glm::vec3(0.085f);
	column20.tex0 = readTexture("marble_column.png");
	column20.tex1 = readTexture("marble_column_normal.png");

	addBorder(column20.position, 0.26f);



	column21.loadModel("doric_column.obj");
	column21.position = glm::vec3(0.6f, 0.0f, -0.51f);
	column21.scale = glm::vec3(0.085f);
	column21.tex0 = readTexture("marble_column.png");
	column21.tex1 = readTexture("marble_column_normal.png");

	addBorder(column21.position, 0.26f);



	column22.loadModel("doric_column.obj");
	column22.position = glm::vec3(0.6f, 0.0f, -0.97f);
	column22.scale = glm::vec3(0.085f);
	column22.tex0 = readTexture("marble_column.png");
	column22.tex1 = readTexture("marble_column_normal.png");

	addBorder(column22.position, 0.26f);



	column23.loadModel("doric_column.obj");
	column23.position = glm::vec3(0.6f, 0.0f, -1.43f);
	column23.scale = glm::vec3(0.085f);
	column23.tex0 = readTexture("marble_column.png");
	column23.tex1 = readTexture("marble_column_normal.png");

	addBorder(column23.position, 0.26f);



	column24.loadModel("doric_column.obj");
	column24.position = glm::vec3(0.6f, 0.0f, -1.89f);
	column24.scale = glm::vec3(0.085f);
	column24.tex0 = readTexture("marble_column.png");
	column24.tex1 = readTexture("marble_column_normal.png");

	addBorder(column24.position, 0.26f);



	column25.loadModel("doric_column.obj");
	column25.position = glm::vec3(0.6f, 0.0f, -2.35f);
	column25.scale = glm::vec3(0.085f);
	column25.tex0 = readTexture("marble_column.png");
	column25.tex1 = readTexture("marble_column_normal.png");

	addBorder(column25.position, 0.26f);


	// sciana lewo


	column26.loadModel("doric_column.obj");
	column26.position = glm::vec3(-1.8f, 0.0f, 1.79f);
	column26.scale = glm::vec3(0.085f);
	column26.tex0 = readTexture("marble_column.png");
	column26.tex1 = readTexture("marble_column_normal.png");

	addBorder(column26.position, 0.26f);



	column27.loadModel("doric_column.obj");
	column27.position = glm::vec3(-1.8f, 0.0f, 1.33f);
	column27.scale = glm::vec3(0.085f);
	column27.tex0 = readTexture("marble_column.png");
	column27.tex1 = readTexture("marble_column_normal.png");

	addBorder(column27.position, 0.26f);



	column28.loadModel("doric_column.obj");
	column28.position = glm::vec3(-1.8f, 0.0f, 0.87f);
	column28.scale = glm::vec3(0.085f);
	column28.tex0 = readTexture("marble_column.png");
	column28.tex1 = readTexture("marble_column_normal.png");

	addBorder(column28.position, 0.26f);


	column29.loadModel("doric_column.obj");
	column29.position = glm::vec3(-1.8f, 0.0f, 0.41f);
	column29.scale = glm::vec3(0.085f);
	column29.tex0 = readTexture("marble_column.png");
	column29.tex1 = readTexture("marble_column_normal.png");

	addBorder(column29.position, 0.26f);


	column30.loadModel("doric_column.obj");
	column30.position = glm::vec3(-1.8f, 0.0f, -0.05f);
	column30.scale = glm::vec3(0.085f);
	column30.tex0 = readTexture("marble_column.png");
	column30.tex1 = readTexture("marble_column_normal.png");

	addBorder(column30.position, 0.26f);



	column31.loadModel("doric_column.obj");
	column31.position = glm::vec3(-1.8f, 0.0f, -0.51f);
	column31.scale = glm::vec3(0.085f);
	column31.tex0 = readTexture("marble_column.png");
	column31.tex1 = readTexture("marble_column_normal.png");

	addBorder(column31.position, 0.26f);



	column32.loadModel("doric_column.obj");
	column32.position = glm::vec3(-1.8f, 0.0f, -0.97f);
	column32.scale = glm::vec3(0.085f);
	column32.tex0 = readTexture("marble_column.png");
	column32.tex1 = readTexture("marble_column_normal.png");

	addBorder(column32.position, 0.26f);



	column33.loadModel("doric_column.obj");
	column33.position = glm::vec3(-1.8f, 0.0f, -1.43f);
	column33.scale = glm::vec3(0.085f);
	column33.tex0 = readTexture("marble_column.png");
	column33.tex1 = readTexture("marble_column_normal.png");

	addBorder(column33.position, 0.26f);



	column34.loadModel("doric_column.obj");
	column34.position = glm::vec3(-1.8f, 0.0f, -1.89f);
	column34.scale = glm::vec3(0.085f);
	column34.tex0 = readTexture("marble_column.png");
	column34.tex1 = readTexture("marble_column_normal.png");

	addBorder(column34.position, 0.26f);



	column35.loadModel("doric_column.obj");
	column35.position = glm::vec3(-1.8f, 0.0f, -2.35f);
	column35.scale = glm::vec3(0.085f);
	column35.tex0 = readTexture("marble_column.png");
	column35.tex1 = readTexture("marble_column_normal.png");

	addBorder(column35.position, 0.26f);


	


	grass.loadModel("grass.obj");
	grass.position = glm::vec3(0.0f, -0.1f, 1.0f);
	grass.scale = glm::vec3(0.035f);
	grass.rotation = glm::vec3(90.0f, 0.0f, 0.0f);
	grass.tex0 = readTexture("grass.png");
	grass.tex1 = readTexture("grass_normal.png");

}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
    //************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************

    delete sp;
	delete sp_shadow;
	delete sp_point_shadow;
	glDeleteFramebuffers(1, &depthMapFBO);
	glDeleteTextures(1, &depthMap);
	glDeleteFramebuffers(1, &pointDepthMapFBO);
	glDeleteTextures(1, &pointDepthMap);
}

void drawObjects(ShaderProgram* current_sp, const glm::mat4& P, const glm::mat4& V) {
	// Funkcja pomocnicza do rysowania wszystkich obiektów
	olimpia_floor.draw(current_sp, P, V);
	olimpia_roof_base.draw(current_sp, P, V);
	olimpia_roof_decorations.draw(current_sp, P, V);
	olimpia_roof_roof.draw(current_sp, P, V);
	olimpia_roof_others.draw(current_sp, P, V);
	olimpia_wall_front.draw(current_sp, P, V);
	olimpia_wall_right.draw(current_sp, P, V);
	olimpia_wall_left.draw(current_sp, P, V);
	olimpia_wall_back.draw(current_sp, P, V);
	ceramic_vase.draw(current_sp, P, V);
	//gold_vase.draw(current_sp, P, V);
	statue1.draw(current_sp, P, V);
	//statue2.draw(current_sp, P, V);
	//statue3.draw(current_sp, P, V);
	horse_head.draw(current_sp, P, V);
	stand.draw(current_sp, P, V);

	column1.draw(current_sp, P, V);
	column2.draw(current_sp, P, V);
	column3.draw(current_sp, P, V);
	column4.draw(current_sp, P, V);
	column5.draw(current_sp, P, V);
	column6.draw(current_sp, P, V);
	column7.draw(current_sp, P, V);
	column8.draw(current_sp, P, V);
	column9.draw(current_sp, P, V);
	column10.draw(current_sp, P, V);
	column11.draw(current_sp, P, V);
	column12.draw(current_sp, P, V);
	column13.draw(current_sp, P, V);
	column14.draw(current_sp, P, V);
	column15.draw(current_sp, P, V);
	column16.draw(current_sp, P, V);
	column17.draw(current_sp, P, V);
	column18.draw(current_sp, P, V);
	column19.draw(current_sp, P, V);
	column20.draw(current_sp, P, V);
	column21.draw(current_sp, P, V);
	column22.draw(current_sp, P, V);
	column23.draw(current_sp, P, V);
	column24.draw(current_sp, P, V);
	column25.draw(current_sp, P, V);
	column26.draw(current_sp, P, V);
	column27.draw(current_sp, P, V);
	column28.draw(current_sp, P, V);
	column29.draw(current_sp, P, V);
	column30.draw(current_sp, P, V);
	column31.draw(current_sp, P, V);
	column32.draw(current_sp, P, V);
	column33.draw(current_sp, P, V);
	column34.draw(current_sp, P, V);
	column35.draw(current_sp, P, V);
	grass.draw(current_sp, P, V);
}


//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float angle_x, float angle_y) {
	// --- pozycje świateł ---
	glm::vec3 directionalLightPos(8.0f, 7.0f, 10.0f);
	glm::vec3 pointLightPos = glm::vec3(-0.6f, 0.5f, 0.0f);

	// --- PRZEBIEG 1: Cień światła kierunkowego ---
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.5f);
	glm::mat4 lightView = glm::lookAt(directionalLightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;
	sp_shadow->use();
	glUniformMatrix4fv(sp_shadow->u("lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT); // Rozwiązanie na "Peter Panning"
	drawObjects(sp_shadow, lightProjection, lightView);
	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// --- PRZEBIEG 2: Cień światła punktowego ---
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)POINT_SHADOW_WIDTH / (float)POINT_SHADOW_HEIGHT, 0.1f, far_plane_point);
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
	sp_point_shadow->use();
	for (unsigned int i = 0; i < 6; ++i)
		glUniformMatrix4fv(sp_point_shadow->u(("shadowMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));
	glUniform1f(sp_point_shadow->u("far_plane"), far_plane_point);
	glUniform3fv(sp_point_shadow->u("lightPos"), 1, glm::value_ptr(pointLightPos));
	glViewport(0, 0, POINT_SHADOW_WIDTH, POINT_SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, pointDepthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	drawObjects(sp_point_shadow, shadowProj, glm::mat4(1.0f)); // Widok jest już w macierzach
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// --- PRZEBIEG 3: Renderowanie finalnej sceny ---
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	sp->use();
	glm::mat4 V = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glm::mat4 P = glm::perspective(glm::radians(50.0f), aspectRatio, 0.1f, 50.0f);
	// Uniformy dla cienia kierunkowego
	glUniformMatrix4fv(sp->u("lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	glUniform1f(sp->u("shadowMapSize"), (float)SHADOW_WIDTH);
	// Uniformy dla cienia punktowego
	glUniform1f(sp->u("far_plane_point"), far_plane_point);
	glUniform3fv(sp->u("pointLightPos"), 1, glm::value_ptr(pointLightPos));
	glUniform3fv(sp->u("pointLightColor"), 1, glm::value_ptr(glm::vec3(1.0f, 0.6f, 0.2f)));
	glUniform1f(sp->u("linear"), 0.22f);
	glUniform1f(sp->u("quadratic"), 0.20f);
	// Inne uniformy
	glUniform3fv(sp->u("directionalLightPos"), 1, glm::value_ptr(directionalLightPos));
	glUniform3fv(sp->u("viewPos"), 1, glm::value_ptr(cameraPos));
	// Podłączanie tekstur cieni
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glUniform1i(sp->u("shadowMap"), 2);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, pointDepthMap);
	glUniform1i(sp->u("shadowCubeMap"), 3);

	// Rysuj obiekty używając głównego shadera
	drawObjects(sp, P, V);

	glfwSwapBuffers(window);
}


int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	//Główna pętla
	float angle_x=0; //Aktualny kąt obrotu obiektu
	float angle_y=0; //Aktualny kąt obrotu obiektu

	float lastFrameTime = 0.0f;
	glfwSetTime(0); //Zeruj timer
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		float currentFrameTime = glfwGetTime();
		float deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;
		

		ceramic_vase.position.y = vaseStartY + vaseAmplitude * sin(currentFrameTime * vaseFreq);
			

		if (moveForward)
			if (isInBorder(cameraPos + moveSpeed * cameraFront * deltaTime)) {
				drawScene(window, angle_x, angle_y); //Wykonaj procedurę rysującą
				glfwPollEvents();
				continue;
			}
			else cameraPos += moveSpeed * cameraFront * deltaTime;
		if (moveBackward)
			if (isInBorder(cameraPos - moveSpeed * cameraFront * deltaTime)) {
				drawScene(window, angle_x, angle_y); //Wykonaj procedurę rysującą
				glfwPollEvents();
				continue;
			}
			else cameraPos -= moveSpeed * cameraFront * deltaTime;
		if (rotateLeft) {
			yaw -= turnSpeed * deltaTime * 60.0f; // Prędkość w stopniach/sekundę
			updateCameraFront(yaw);
		}
		if (rotateRight) {
			yaw += turnSpeed * deltaTime * 60.0f;
			updateCameraFront(yaw);
		}
		drawScene(window,angle_x,angle_y); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
