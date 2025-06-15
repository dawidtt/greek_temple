/*
Niniejszy program jest wolnym oprogramowaniem; moĹĽesz go
rozprowadzaÄ‡ dalej i / lub modyfikowaÄ‡ na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez FundacjÄ™ Wolnego
Oprogramowania - wedĹ‚ug wersji 2 tej Licencji lub(wedĹ‚ug twojego
wyboru) ktĂłrejĹ› z pĂłĹşniejszych wersji.

Niniejszy program rozpowszechniany jest z nadziejÄ…, iĹĽ bÄ™dzie on
uĹĽyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyĹ›lnej
gwarancji PRZYDATNOĹšCI HANDLOWEJ albo PRZYDATNOĹšCI DO OKREĹšLONYCH
ZASTOSOWAĹ�.W celu uzyskania bliĹĽszych informacji siÄ™gnij do
Powszechnej Licencji Publicznej GNU.

Z pewnoĹ›ciÄ… wraz z niniejszym programem otrzymaĹ‚eĹ› teĹĽ egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeĹ›li nie - napisz do Free Software Foundation, Inc., 59 Temple
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


#include "load.h"
#include <vector>
#include <iostream>
#include "Object3D.h"

Object3D olimpia_floor, olimpia_roof_base, olimpia_roof_decorations, olimpia_roof_roof, 
olimpia_roof_others, olimpia_wall_front, olimpia_wall_right, olimpia_wall_left, olimpia_wall_back,
ceramic_vase, human, statue1, statue_gate_right, statue_gate_left, statue3,
horse_head, stand, lightbulb, lightbulb1, wind_eagle,
column1, column2, column3, column4, column5, column6, column7, column8, column9, column10,
column11, column12, column13, column14, column15, column16, column17, column18, column19, column20,
column21, column22, column23, column24, column25, column26, column27, column28, column29, column30,
column31, column32, column33, column34, column35,
grass;

enum HumanState { WALKING_A, TURNING_TO_B, WALKING_B, TURNING_TO_A };
HumanState humanState = WALKING_B;
glm::vec3 humanStartPos = glm::vec3(0.5f, 0.05f, 3.5f);
glm::vec3 humanEndPos = glm::vec3(-1.5f, 0.05f, 0.0f);

float humanRotationA = 90.0f; 
float humanRotationB = 270.0f;  
float humanWalkSpeed = 0.5f;   
float humanTurnSpeed = 180.0f; 

glm::vec3 eagle_center = glm::vec3(-0.5f, 3.0f, 1.0f); 
float eagle_radiusX = 4.0f;   
float eagle_radiusZ = 4.5f;   
float eagle_flight_speed = 0.4f; 
float eagle_current_angle = 0.0f; 

float speed_x = 0;
float speed_y = 0;
float aspectRatio = 1;


float vaseStartY = 0.18f;
float vaseAmplitude = 0.02f; 
float vaseFreq = 2.0f;

ShaderProgram* sp;
ShaderProgram* sp_shadow;






bool moveForward = false;
bool moveBackward = false;
bool rotateLeft = false;
bool rotateRight = false;
bool rotateUp = false;   // Nowa flaga dla obracania kamery w górę
bool rotateDown = false; // Nowa flaga dla obracania kamery w dół
float moveSpeed = 0.8f;
float turnSpeed = 1.4f; // stopnie

glm::vec3 cameraPos = glm::vec3(-0.5f, 0.2f, 2.8f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


float yaw = -90.0f; // 0° to w kierunku x, -90° to w kierunku z (do przodu)
float pitch = 0.0f; // Dodaj zmienną pitch, początkowo 0 stopni (patrzymy prosto)
const float MAX_PITCH = 20.0f; // Maksymalny kąt pochylenia w górę
const float MIN_PITCH = -20.0f; // Minimalny kąt pochylenia w dół (np. -30 stopni)

GLuint tex0;
GLuint tex1;

GLuint depthMapFBO;
GLuint depthMap;
const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;

void animateEagle(float);

struct border {
	glm::vec3 center;
	float radius; // PromieĹ„ zakazanego obszaru
};


std::vector<border> borders;


bool isInBorder(const glm::vec3& position) {
	for (const auto& area : borders) {
		
		float dx = position.x - area.center.x;
		float dy = position.y - area.center.y;
		float dz = position.z - area.center.z;

		
		float distanceSq = (dx * dx) + (dy * dy) + (dz * dz);

		
		float radiusSq = area.radius * area.radius;

		
		if (distanceSq < radiusSq) {
			return true; 
		}
	}
	return false; 
}
void addBorder(const glm::vec3& position, float radius) {
	borders.push_back({ position, radius });
}

struct borderAABB {
	glm::vec3 min; 
	glm::vec3 max; 
};

std::vector<borderAABB> bordersAABB;

void addBorderAABB(const glm::vec3& minPoint, const glm::vec3& maxPoint) {
	bordersAABB.push_back({ minPoint, maxPoint });
}

bool isInBorderAABB(const glm::vec3& position) {
	for (const auto& area : bordersAABB) {
		if (position.x >= area.min.x && position.x <= area.max.x &&
			position.y >= area.min.y && position.y <= area.max.y &&
			position.z >= area.min.z && position.z <= area.max.z) {
			return true;
		}
	}
	return false; 
}


void updateCameraFront(float yaw, float pitch) {
	
	float yawRad = glm::radians(yaw);
	float pitchRad = glm::radians(pitch);

	
	cameraFront.x = cos(yawRad) * cos(pitchRad);
	cameraFront.y = sin(pitchRad); 
	cameraFront.z = sin(yawRad) * cos(pitchRad);

	cameraFront = glm::normalize(cameraFront);
}


//Procedura obsĹ‚ugi bĹ‚Ä™dĂłw
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_W) moveForward = true;
		if (key == GLFW_KEY_S) moveBackward = true;
		if (key == GLFW_KEY_A) rotateLeft = true;
		if (key == GLFW_KEY_D) rotateRight = true;
		if (key == GLFW_KEY_UP) rotateUp = true;   
		if (key == GLFW_KEY_DOWN) rotateDown = true; 
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_W) moveForward = false;
		if (key == GLFW_KEY_S) moveBackward = false;
		if (key == GLFW_KEY_A) rotateLeft = false;
		if (key == GLFW_KEY_D) rotateRight = false;
		if (key == GLFW_KEY_UP) rotateUp = false;   
		if (key == GLFW_KEY_DOWN) rotateDown = false; 
	}
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	if (height == 0) return;
	aspectRatio = (float)width / (float)height;
	glViewport(0, 0, width, height);
}


GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	//Wczytanie do pamiÄ™ci komputera
	std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
	unsigned width, height;   //Zmienne do ktĂłrych wczytamy wymiary obrazka
	//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);

	//Import do pamiÄ™ci karty graficznej
	glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
	//Wczytaj obrazek do pamiÄ™ci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}

void animateHuman(float);

//Procedura inicjujÄ…ca
void initOpenGLProgram(GLFWwindow* window) {
	
	glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE); // Optymalizacja, nie rysuje tylnych Ĺ›cian trĂłjkÄ…tĂłw
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);

	sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
	sp_shadow = new ShaderProgram("shadow_shader.vert", NULL, "shadow_shader.frag");

	// Początek Framebuffera do cieni 
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



	olimpia_floor.loadModel("./obj/olimpia_floor.obj");
	olimpia_floor.position = glm::vec3(0.0f, -0.09f, 1.0f);
	olimpia_floor.scale = glm::vec3(0.125f);
	olimpia_floor.rotation = glm::vec3(0.0f, -90.0f, 0.0f);
	olimpia_floor.tex0 = readTexture("./textures/marble_column.png");
	olimpia_floor.tex1 = readTexture("./textures/marble_column_normal.png");


	olimpia_roof_base.loadModel("./obj/olimpia_roof_base.obj");
	olimpia_roof_base.position = glm::vec3(0.04f, -0.65f, -1.2f);
	olimpia_roof_base.scale = glm::vec3(0.132f);
	olimpia_roof_base.rotation = glm::vec3(0.0f, -90.0f, 0.0f);
	olimpia_roof_base.tex0 = readTexture("./textures/marble_column.png");
	olimpia_roof_base.tex1 = readTexture("./textures/marble_column_normal.png");

	olimpia_roof_decorations.loadModel("./obj/olimpia_roof_decorations.obj");
	olimpia_roof_decorations.position = glm::vec3(0.04f, -0.65f, -1.2f);
	olimpia_roof_decorations.scale = glm::vec3(0.132f);
	olimpia_roof_decorations.rotation = glm::vec3(0.0f, -90.0f, 0.0f);
	olimpia_roof_decorations.tex0 = readTexture("./textures/roof_decorations.png");
	olimpia_roof_decorations.tex1 = readTexture("./textures/roof_decorations_normal.png");


	olimpia_roof_roof.loadModel("./obj/olimpia_roof_roof.obj");
	olimpia_roof_roof.position = glm::vec3(0.04f, -0.65f, -1.2f);
	olimpia_roof_roof.scale = glm::vec3(0.132f);
	olimpia_roof_roof.rotation = glm::vec3(0.0f, -90.0f, 0.0f);
	olimpia_roof_roof.tex0 = readTexture("./textures/marble_column.png");
	olimpia_roof_roof.tex1 = readTexture("./textures/marble_column_normal.png");

	olimpia_roof_others.loadModel("./obj/olimpia_roof_others.obj");
	olimpia_roof_others.position = glm::vec3(0.04f, -0.65f, -1.2f);
	olimpia_roof_others.scale = glm::vec3(0.132f);
	olimpia_roof_others.rotation = glm::vec3(0.0f, -90.0f, 0.0f);
	olimpia_roof_others.tex0 = readTexture("./textures/roof_decorations.png");
	olimpia_roof_others.tex1 = readTexture("./textures/roof_decorations_normal.png");

	olimpia_wall_front.loadModel("./obj/olimpia_wall_front.obj");
	olimpia_wall_front.position = glm::vec3(0.04f, -0.22f, -1.0f);
	olimpia_wall_front.scale = glm::vec3(0.132f);
	olimpia_wall_front.rotation = glm::vec3(0.0f, -90.0f, 0.0f);
	olimpia_wall_front.tex0 = readTexture("./textures/marble_column.png");
	olimpia_wall_front.tex1 = readTexture("./textures/marble_column_normal.png");

	olimpia_wall_right.loadModel("./obj/olimpia_wall_right.obj");
	olimpia_wall_right.position = glm::vec3(0.02f, -0.8f, -1.52f);
	olimpia_wall_right.scale = glm::vec3(0.138f);
	olimpia_wall_right.rotation = glm::vec3(0.0f, -90.0f, 0.0f);
	olimpia_wall_right.tex0 = readTexture("./textures/marble_column.png");
	olimpia_wall_right.tex1 = readTexture("./textures/marble_column_normal.png");

	olimpia_wall_left.loadModel("./obj/olimpia_wall_left.obj");
	olimpia_wall_left.position = glm::vec3(0.09f, -0.8f, -1.575f);
	olimpia_wall_left.scale = glm::vec3(0.138f);
	olimpia_wall_left.rotation = glm::vec3(0.0f, -90.0f, 0.0f);
	olimpia_wall_left.tex0 = readTexture("./textures/marble_column.png");
	olimpia_wall_left.tex1 = readTexture("./textures/marble_column_normal.png");


	

	// floor border boki
	addBorderAABB(glm::vec3(0.94f, 0.0f, -3.4f), glm::vec3(1.2f, 0.6f, 4.32f));
	addBorderAABB(glm::vec3(-2.5f, 0.0f, -3.4f), glm::vec3(-2.17f, 0.6f, 4.32f));

	// floor border boki
	addBorderAABB(glm::vec3(-2.17f, 0.0f, -3.8f), glm::vec3(0.94f, 0.6f, -3.4f));
	addBorderAABB(glm::vec3(-2.17f, 0.0f, 4.32f), glm::vec3(0.94f, 0.6f, 4.72f));



	//front sciana lewo
	addBorderAABB(glm::vec3(-1.15f, 0.0f, 1.38f), glm::vec3(-0.78f, 0.3f, 1.75f));


	//front sciana prawo
	addBorderAABB(glm::vec3(-0.36f, 0.0f, 1.42f), glm::vec3(0.05f, 0.3f, 1.75f));

	//front kolumny 
	addBorder(glm::vec3(-0.92f, 0.0f, 1.9f), 0.32f);
	addBorder(glm::vec3(-0.22f, 0.0f, 1.9f), 0.32f);


	//lewa sciana border
	addBorderAABB(glm::vec3(-1.5f, 0.0f, -2.0f), glm::vec3(-1.15f, 0.3f, 1.75f));


	//lewa sciana wnetrze border
	addBorderAABB(glm::vec3(-1.15f, 0.0f, -2.0f), glm::vec3(-0.85f, 0.3f, 1.1f));



	//prawa sciana border
	addBorderAABB(glm::vec3(-0.05f, 0.0f, -2.0f), glm::vec3(0.3f, 0.3f, 1.75f));

	//prawa sciana wnetrze border
	addBorderAABB(glm::vec3(-0.35f, 0.0f, -2.0f), glm::vec3(0.05f, 0.3f, 1.1f));

	//tylna sciana border
	addBorderAABB(glm::vec3(-1.5f, 0.0f, -1.9f), glm::vec3(0.3f, 0.3f, -1.45f));










	olimpia_wall_back.loadModel("./obj/olimpia_wall_back.obj");
	olimpia_wall_back.position = glm::vec3(0.04f, -0.24f, -1.4f);
	olimpia_wall_back.scale = glm::vec3(0.132f);
	olimpia_wall_back.rotation = glm::vec3(0.0f, -90.0f, 0.0f);
	olimpia_wall_back.tex0 = readTexture("./textures/marble_column.png");
	olimpia_wall_back.tex1 = readTexture("./textures/marble_column_normal.png");




	statue1.loadModel("./obj/statue1.obj");
	statue1.position = glm::vec3(-0.2f, 0.0f, 3.0f);
	statue1.scale = glm::vec3(0.6f);
	statue1.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	statue1.tex0 = readTexture("./textures/marble_column.png");
	statue1.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(statue1.position, 0.3f);


	statue_gate_right.loadModel("./obj/statue2.obj");
	statue_gate_right.position = glm::vec3(-0.05f, 0.0f, 1.25f);
	statue_gate_right.scale = glm::vec3(0.003f);
	statue_gate_right.rotation = glm::vec3(-90.0f, 0.0f, -90.0f);
	statue_gate_right.tex0 = readTexture("./textures/statues.png");
	statue_gate_right.tex1 = readTexture("./textures/statues_normal.png");

	addBorder(statue_gate_right.position, 0.3f);

	statue_gate_left.loadModel("./obj/statue2.obj");
	statue_gate_left.position = glm::vec3(-1.05f, 0.0f, 1.25f);
	statue_gate_left.scale = glm::vec3(0.003f);
	statue_gate_left.rotation = glm::vec3(-90.0f, 0.0f, 90.0f);
	statue_gate_left.tex0 = readTexture("./textures/statues.png");
	statue_gate_left.tex1 = readTexture("./textures/statues_normal.png");

	addBorder(statue_gate_left.position, 0.3f);



	statue3.loadModel("./obj/statue3.obj");
	statue3.position = glm::vec3(-0.6f, 0.0f, -1.4f);
	statue3.scale = glm::vec3(0.008f);
	statue3.rotation = glm::vec3(-90.0f, 0.0f, 0.0f);
	statue3.tex0 = readTexture("./textures/statues.png");
	statue3.tex1 = readTexture("./textures/statues_normal.png");

	addBorder(statue3.position, 0.3f);

	wind_eagle.loadModel("./obj/wind_eagle.obj"); // UWAGA: Upewnij siÄ™, ĹĽe nazwa pliku jest poprawna
	wind_eagle.scale = glm::vec3(0.005f);     // Skala moĹĽe wymagaÄ‡ dostosowania
	// UWAGA: UĹĽywamy istniejÄ…cej tekstury. ZmieĹ„ na wĹ‚asnÄ…, jeĹ›li masz.
	wind_eagle.tex0 = readTexture("./textures/marble_column.png");
	wind_eagle.tex1 = readTexture("./textures/marble_column_normal.png");

	horse_head.loadModel("./obj/horse_head.obj");
	horse_head.position = glm::vec3(-1.0f, 0.0f, 3.0f);
	horse_head.scale = glm::vec3(0.6f);
	horse_head.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	horse_head.tex0 = readTexture("./textures/marble_column.png");
	horse_head.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(horse_head.position, 0.3f);



	ceramic_vase.loadModel("./obj/ceramic_vase.obj");
	ceramic_vase.position = glm::vec3(-0.6f, vaseStartY, 0.0f);
	ceramic_vase.scale = glm::vec3(0.4f);
	ceramic_vase.rotation = glm::vec3(0.0f, -90.0f, 0.0f);
	ceramic_vase.tex0 = readTexture("./textures/gold.png");
	ceramic_vase.tex1 = readTexture("./textures/gold_normal.png");

	stand.loadModel("./obj/stand.obj");
	stand.position = glm::vec3(-0.6f, 0.0f, 0.0f);
	stand.scale = glm::vec3(0.2f);
	stand.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	stand.tex0 = readTexture("./textures/wood.png");
	stand.tex1 = readTexture("./textures/wood_normal.png");

	addBorder(stand.position, 0.26f);

	lightbulb.loadModel("./obj/lightbulb.obj");
	lightbulb.position = glm::vec3(-0.6f, 0.8f, 0.0f);
	lightbulb.scale = glm::vec3(1.2f);
	lightbulb.rotation = glm::vec3(0.0f, 0.0f, 180.0f);
	lightbulb.tex0 = readTexture("./textures/gold.png");
	lightbulb.tex1 = readTexture("./textures/gold_normal.png");

	lightbulb1.loadModel("./obj/lightbulb.obj");
	lightbulb1.position = glm::vec3(-0.5f, 0.5f, -2.0f); // Pozycja drugiego Ĺ›wiatĹ‚a
	lightbulb1.scale = glm::vec3(1.2f);
	lightbulb1.rotation = glm::vec3(0.0f, 0.0f, 180.0f);
	lightbulb1.tex0 = readTexture("./textures/gold.png");


	//front kolumny

	column1.loadModel("./obj/doric_column.obj");
	column1.position = glm::vec3(-1.8f, 0.0f, 2.25f);
	column1.scale = glm::vec3(0.085f);
	column1.tex0 = readTexture("./textures/marble_column.png");
	column1.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column1.position, 0.26f);

	column2.loadModel("./obj/doric_column.obj");
	column2.position = glm::vec3(-1.4f, 0.0f, 2.25f);
	column2.scale = glm::vec3(0.085f);
	column2.tex0 = readTexture("./textures/marble_column.png");
	column2.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column2.position, 0.26f);


	column3.loadModel("./obj/doric_column.obj");
	column3.position = glm::vec3(-1.0f, 0.0f, 2.25f);
	column3.scale = glm::vec3(0.085f);
	column3.tex0 = readTexture("./textures/marble_column.png");
	column3.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column3.position, 0.26f);


	column4.loadModel("./obj/doric_column.obj");
	column4.position = glm::vec3(-0.2f, 0.0f, 2.25f);
	column4.scale = glm::vec3(0.085f);
	column4.tex0 = readTexture("./textures/marble_column.png");
	column4.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column4.position, 0.26f);


	column5.loadModel("./obj/doric_column.obj");
	column5.position = glm::vec3(0.2f, 0.0f, 2.25f);
	column5.scale = glm::vec3(0.085f);
	column5.tex0 = readTexture("./textures/marble_column.png");
	column5.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column5.position, 0.26f);


	column6.loadModel("./obj/doric_column.obj");
	column6.position = glm::vec3(0.6f, 0.0f, 2.25f);
	column6.scale = glm::vec3(0.085f);
	column6.tex0 = readTexture("./textures/marble_column.png");
	column6.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column6.position, 0.26f);


	//tyl kolumny

	column7.loadModel("./obj/doric_column.obj");
	column7.position = glm::vec3(0.6f, 0.0f, -2.35f);
	column7.scale = glm::vec3(0.085f);
	column7.tex0 = readTexture("./textures/marble_column.png");
	column7.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column7.position, 0.26f);


	column8.loadModel("./obj/doric_column.obj");
	column8.position = glm::vec3(-1.8f, 0.0f, -2.35f);
	column8.scale = glm::vec3(0.085f);
	column8.tex0 = readTexture("./textures/marble_column.png");
	column8.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column8.position, 0.26f);


	column9.loadModel("./obj/doric_column.obj");
	column9.position = glm::vec3(-1.4f, 0.0f, -2.35f);
	column9.scale = glm::vec3(0.085f);
	column9.tex0 = readTexture("./textures/marble_column.png");
	column9.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column9.position, 0.26f);


	column10.loadModel("./obj/doric_column.obj");
	column10.position = glm::vec3(-1.0f, 0.0f, -2.35f);
	column10.scale = glm::vec3(0.085f);
	column10.tex0 = readTexture("./textures/marble_column.png");
	column10.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column10.position, 0.26f);


	column11.loadModel("./obj/doric_column.obj");
	column11.position = glm::vec3(-0.6f, 0.0f, -2.35f);
	column11.scale = glm::vec3(0.085f);
	column11.tex0 = readTexture("./textures/marble_column.png");
	column11.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column11.position, 0.26f);


	column12.loadModel("./obj/doric_column.obj");
	column12.position = glm::vec3(-0.2f, 0.0f, -2.35f);
	column12.scale = glm::vec3(0.085f);
	column12.tex0 = readTexture("./textures/marble_column.png");
	column12.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column12.position, 0.26f);


	column13.loadModel("./obj/doric_column.obj");
	column13.position = glm::vec3(-0.2f, 0.0f, -2.35f);
	column13.scale = glm::vec3(0.085f);
	column13.tex0 = readTexture("./textures/marble_column.png");
	column13.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column13.position, 0.26f);


	column14.loadModel("./obj/doric_column.obj");
	column14.position = glm::vec3(0.2f, 0.0f, -2.35f);
	column14.scale = glm::vec3(0.085f);
	column14.tex0 = readTexture("./textures/marble_column.png");
	column14.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column14.position, 0.26f);


	column15.loadModel("./obj/doric_column.obj");
	column15.position = glm::vec3(0.6f, 0.0f, -2.35f);
	column15.scale = glm::vec3(0.085f);
	column15.tex0 = readTexture("./textures/marble_column.png");
	column15.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column15.position, 0.26f);


	// sciany prawo


	column16.loadModel("./obj/doric_column.obj");
	column16.position = glm::vec3(0.6f, 0.0f, 1.79f);
	column16.scale = glm::vec3(0.085f);
	column16.tex0 = readTexture("./textures/marble_column.png");
	column16.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column16.position, 0.26f);



	column17.loadModel("./obj/doric_column.obj");
	column17.position = glm::vec3(0.6f, 0.0f, 1.33f);
	column17.scale = glm::vec3(0.085f);
	column17.tex0 = readTexture("./textures/marble_column.png");
	column17.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column17.position, 0.26f);



	column18.loadModel("./obj/doric_column.obj");
	column18.position = glm::vec3(0.6f, 0.0f, 0.87f);
	column18.scale = glm::vec3(0.085f);
	column18.tex0 = readTexture("./textures/marble_column.png");
	column18.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column18.position, 0.26f);


	column19.loadModel("./obj/doric_column.obj");
	column19.position = glm::vec3(0.6f, 0.0f, 0.41f);
	column19.scale = glm::vec3(0.085f);
	column19.tex0 = readTexture("./textures/marble_column.png");
	column19.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column19.position, 0.26f);


	column20.loadModel("./obj/doric_column.obj");
	column20.position = glm::vec3(0.6f, 0.0f, -0.05f);
	column20.scale = glm::vec3(0.085f);
	column20.tex0 = readTexture("./textures/marble_column.png");
	column20.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column20.position, 0.26f);



	column21.loadModel("./obj/doric_column.obj");
	column21.position = glm::vec3(0.6f, 0.0f, -0.51f);
	column21.scale = glm::vec3(0.085f);
	column21.tex0 = readTexture("./textures/marble_column.png");
	column21.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column21.position, 0.26f);



	column22.loadModel("./obj/doric_column.obj");
	column22.position = glm::vec3(0.6f, 0.0f, -0.97f);
	column22.scale = glm::vec3(0.085f);
	column22.tex0 = readTexture("./textures/marble_column.png");
	column22.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column22.position, 0.26f);



	column23.loadModel("./obj/doric_column.obj");
	column23.position = glm::vec3(0.6f, 0.0f, -1.43f);
	column23.scale = glm::vec3(0.085f);
	column23.tex0 = readTexture("./textures/marble_column.png");
	column23.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column23.position, 0.26f);



	column24.loadModel("./obj/doric_column.obj");
	column24.position = glm::vec3(0.6f, 0.0f, -1.89f);
	column24.scale = glm::vec3(0.085f);
	column24.tex0 = readTexture("./textures/marble_column.png");
	column24.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column24.position, 0.26f);



	column25.loadModel("./obj/doric_column.obj");
	column25.position = glm::vec3(0.6f, 0.0f, -2.35f);
	column25.scale = glm::vec3(0.085f);
	column25.tex0 = readTexture("./textures/marble_column.png");
	column25.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column25.position, 0.26f);


	// sciana lewo


	column26.loadModel("./obj/doric_column.obj");
	column26.position = glm::vec3(-1.8f, 0.0f, 1.79f);
	column26.scale = glm::vec3(0.085f);
	column26.tex0 = readTexture("./textures/marble_column.png");
	column26.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column26.position, 0.26f);



	column27.loadModel("./obj/doric_column.obj");
	column27.position = glm::vec3(-1.8f, 0.0f, 1.33f);
	column27.scale = glm::vec3(0.085f);
	column27.tex0 = readTexture("./textures/marble_column.png");
	column27.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column27.position, 0.26f);



	column28.loadModel("./obj/doric_column.obj");
	column28.position = glm::vec3(-1.8f, 0.0f, 0.87f);
	column28.scale = glm::vec3(0.085f);
	column28.tex0 = readTexture("./textures/marble_column.png");
	column28.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column28.position, 0.26f);


	column29.loadModel("./obj/doric_column.obj");
	column29.position = glm::vec3(-1.8f, 0.0f, 0.41f);
	column29.scale = glm::vec3(0.085f);
	column29.tex0 = readTexture("./textures/marble_column.png");
	column29.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column29.position, 0.26f);


	column30.loadModel("./obj/doric_column.obj");
	column30.position = glm::vec3(-1.8f, 0.0f, -0.05f);
	column30.scale = glm::vec3(0.085f);
	column30.tex0 = readTexture("./textures/marble_column.png");
	column30.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column30.position, 0.26f);



	column31.loadModel("./obj/doric_column.obj");
	column31.position = glm::vec3(-1.8f, 0.0f, -0.51f);
	column31.scale = glm::vec3(0.085f);
	column31.tex0 = readTexture("./textures/marble_column.png");
	column31.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column31.position, 0.26f);



	column32.loadModel("./obj/doric_column.obj");
	column32.position = glm::vec3(-1.8f, 0.0f, -0.97f);
	column32.scale = glm::vec3(0.085f);
	column32.tex0 = readTexture("./textures/marble_column.png");
	column32.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column32.position, 0.26f);



	column33.loadModel("./obj/doric_column.obj");
	column33.position = glm::vec3(-1.8f, 0.0f, -1.43f);
	column33.scale = glm::vec3(0.085f);
	column33.tex0 = readTexture("./textures/marble_column.png");
	column33.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column33.position, 0.26f);



	column34.loadModel("./obj/doric_column.obj");
	column34.position = glm::vec3(-1.8f, 0.0f, -1.89f);
	column34.scale = glm::vec3(0.085f);
	column34.tex0 = readTexture("./textures/marble_column.png");
	column34.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column34.position, 0.26f);



	column35.loadModel("./obj/doric_column.obj");
	column35.position = glm::vec3(-1.8f, 0.0f, -2.35f);
	column35.scale = glm::vec3(0.085f);
	column35.tex0 = readTexture("./textures/marble_column.png");
	column35.tex1 = readTexture("./textures/marble_column_normal.png");

	addBorder(column35.position, 0.26f);





	grass.loadModel("./obj/grass.obj");
	grass.position = glm::vec3(0.0f, -0.1f, 1.0f);
	grass.scale = glm::vec3(0.045f);
	grass.rotation = glm::vec3(90.0f, 0.0f, 0.0f);
	grass.tex0 = readTexture("./textures/grass.png");
	grass.tex1 = readTexture("./textures/grass_normal.png");

	human.loadModel("./obj/human.obj");
	human.position = humanStartPos;
	human.scale = glm::vec3(0.02f);
	human.rotation = glm::vec3(0.0f, humanRotationA, 0.0f);
	human.tex0 = readTexture("./textures/marble_column.png");
	human.tex1 = readTexture("./textures/marble_column_normal.png");


}


//Zwolnienie zasobĂłw zajÄ™tych przez program
void freeOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, ktĂłry naleĹĽy wykonaÄ‡ po zakoĹ„czeniu pÄ™tli gĹ‚Ăłwnej************

	delete sp;
	delete sp_shadow;
	glDeleteFramebuffers(1, &depthMapFBO);
	glDeleteTextures(1, &depthMap);
}

void drawObjects(ShaderProgram* current_sp, const glm::mat4& P, const glm::mat4& V) {
	// Funkcja pomocnicza do rysowania wszystkich obiektĂłw
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
	statue_gate_right.draw(current_sp, P, V);
	statue_gate_left.draw(current_sp, P, V);
	statue3.draw(current_sp, P, V);
	horse_head.draw(current_sp, P, V);
	stand.draw(current_sp, P, V);
	lightbulb.draw(current_sp, P, V);
	lightbulb1.draw(current_sp, P, V);
	wind_eagle.draw(current_sp, P, V);




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
	human.draw(current_sp, P, V);
	grass.draw(current_sp, P, V);
}


//Procedura rysujÄ…ca zawartoĹ›Ä‡ sceny
void drawScene(GLFWwindow* window, float angle_x, float angle_y) {
	// --- KROK 1: Renderowanie mapy gĹ‚Ä™bi (z perspektywy Ĺ›wiatĹ‚a) ---
	// (Ta czÄ™Ĺ›Ä‡ pozostaje bez zmian, poniewaĹĽ cieĹ„ rzuca tylko pierwsze, kierunkowe Ĺ›wiatĹ‚o)
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	glm::vec3 lightPos(8.0f, 5.0f, 10.0f); // GĹ‚Ăłwne Ĺ›wiatĹ‚o (sĹ‚oĹ„ce)

	float near_plane = 1.0f, far_plane = 20.5f;
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;

	sp_shadow->use();
	glUniformMatrix4fv(sp_shadow->u("lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	drawObjects(sp_shadow, lightProjection, lightView);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Normalne renderowanie sceny (z widoku kamery)
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	sp->use();

	glm::mat4 V = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glm::mat4 P = glm::perspective(50.0f * PI / 180.0f, aspectRatio, 0.01f, 50.0f);

	// PrzekaĹĽ uniformy dla słońca
	glUniformMatrix4fv(sp->u("lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	glUniform1f(sp->u("shadowMapSize"), (float)SHADOW_WIDTH);
	glUniform3fv(sp->u("directionalLightPos"), 1, glm::value_ptr(lightPos));
	glUniform3fv(sp->u("viewPos"), 1, glm::value_ptr(cameraPos)); // Pozycja kamery dla odbiÄ‡ lustrzanych

	// PrzekaĹĽ uniformy dla żarówki
	glm::vec3 pointLightPosition = glm::vec3(-0.6f, 0.5f, 0.0f); // Pozycja w Ĺ›rodku budynku
	glm::vec3 pointLightColor = glm::vec3(1.0f, 0.6f, 0.2f);     // CiepĹ‚y, pomaraĹ„czowy kolor
	glUniform3fv(sp->u("pointLightPos"), 1, glm::value_ptr(pointLightPosition));
	glUniform3fv(sp->u("pointLightColor"), 1, glm::value_ptr(pointLightColor));
	// Parametry tłumienia.
	glUniform1f(sp->u("linear"), 0.22f);
	glUniform1f(sp->u("quadratic"), 0.20f);
	// -----------------------------------------------------------

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glUniform1i(sp->u("shadowMap"), 2);

	// Rysuj obiekty uĹĽywajÄ…c gĹ‚Ăłwnego shadera
	drawObjects(sp, P, V);

	glfwSwapBuffers(window);
}


int main(void)
{
	GLFWwindow* window; //WskaĹşnik na obiekt reprezentujÄ…cy okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurÄ™ obsĹ‚ugi bĹ‚Ä™dĂłw

	if (!glfwInit()) { //Zainicjuj bibliotekÄ™ GLFW
		fprintf(stderr, "Nie moĹĽna zainicjowaÄ‡ GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //UtwĂłrz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //JeĹĽeli okna nie udaĹ‚o siÄ™ utworzyÄ‡, to zamknij program
	{
		fprintf(stderr, "Nie moĹĽna utworzyÄ‡ okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje siÄ™ aktywny i polecenia OpenGL bÄ™dÄ… dotyczyÄ‡ wĹ‚aĹ›nie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrĂłt plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekÄ™ GLEW
		fprintf(stderr, "Nie moĹĽna zainicjowaÄ‡ GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujÄ…ce

	//GĹ‚Ăłwna pÄ™tla
	float angle_x = 0; //Aktualny kÄ…t obrotu obiektu
	float angle_y = 0; //Aktualny kÄ…t obrotu obiektu

	float lastFrameTime = 0.0f;
	glfwSetTime(0); //Zeruj timer
	while (!glfwWindowShouldClose(window)) //Tak dĹ‚ugo jak okno nie powinno zostaÄ‡ zamkniÄ™te
	{
		float currentFrameTime = glfwGetTime();
		float deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;


		ceramic_vase.position.y = vaseStartY + vaseAmplitude * sin(currentFrameTime * vaseFreq);

		float standRotationSpeed = 30.0f; 


		stand.rotation.y += standRotationSpeed * deltaTime;

		if (stand.rotation.y > 360.0f) stand.rotation.y -= 360.0f;


		glm::vec3 flatCameraFront = cameraFront;
		flatCameraFront.y = 0.0f;
		flatCameraFront = glm::normalize(flatCameraFront);


		if (moveForward)
			if (isInBorder(cameraPos + moveSpeed * flatCameraFront * deltaTime) || isInBorderAABB(cameraPos + moveSpeed * flatCameraFront * deltaTime)) {
				drawScene(window, angle_x, angle_y); //Wykonaj procedurę rysującą
				glfwPollEvents();
				continue;
			}
			else cameraPos += moveSpeed * flatCameraFront * deltaTime;
		if (moveBackward)
			if (isInBorder(cameraPos - moveSpeed * flatCameraFront * deltaTime) || isInBorderAABB(cameraPos - moveSpeed * flatCameraFront * deltaTime)) {
				drawScene(window, angle_x, angle_y); //Wykonaj procedurę rysującą
				glfwPollEvents();
				continue;
			}
			else cameraPos -= moveSpeed * flatCameraFront * deltaTime;
		if (rotateLeft) {
			yaw -= turnSpeed * deltaTime * 60.0f; // Prędkość w stopniach/sekundę
			updateCameraFront(yaw, pitch);
		}
		if (rotateRight) {
			yaw += turnSpeed * deltaTime * 60.0f;
			updateCameraFront(yaw, pitch);
		}
		if (rotateUp) {
			pitch += turnSpeed * deltaTime * 50.0f;
			if (pitch > MAX_PITCH) {
				pitch = MAX_PITCH; // Ogranicz pitch, aby nie przekroczyć maksymalnego kąta w górę
			}
			updateCameraFront(yaw, pitch); // WAŻNE: Zaktualizuj cameraFront
		}
		if (rotateDown) {
			pitch -= turnSpeed * deltaTime * 50.0f;
			if (pitch < MIN_PITCH) {
				pitch = MIN_PITCH; // Ogranicz pitch, aby nie przekroczyć minimalnego kąta w dół
			}
			updateCameraFront(yaw, pitch); // WAŻNE: Zaktualizuj cameraFront
		}

		animateHuman(deltaTime);
		animateEagle(deltaTime);


		drawScene(window, angle_x, angle_y); //Wykonaj procedurÄ™ rysujÄ…cÄ…
		glfwPollEvents(); //Wykonaj procedury callback w zaleznoĹ›ci od zdarzeĹ„ jakie zaszĹ‚y.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //UsuĹ„ kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajÄ™te przez GLFW
	exit(EXIT_SUCCESS);
}

void animateHuman(float deltaTime) {
	switch (humanState) {
	case WALKING_A: {
		human.position.x += humanWalkSpeed * deltaTime;
		if (human.position.x >= humanStartPos.x) {
			human.position.x = humanStartPos.x; 
			humanState = TURNING_TO_A;
		}
		break;
	}
	case TURNING_TO_B: {
		human.rotation.y += humanTurnSpeed * deltaTime;
		if (human.rotation.y >= humanRotationB) {
			human.rotation.y = humanRotationB; 
			humanState = WALKING_A; 
		}
		break;
	}
	case WALKING_B: {
		human.position.x -= humanWalkSpeed * deltaTime;
		if (human.position.x <= humanEndPos.x) {
			human.position.x = humanEndPos.x; 
			humanState = TURNING_TO_B;
		}
		break;
	}
	case TURNING_TO_A: {
		human.rotation.y -= humanTurnSpeed * deltaTime;
		if (human.rotation.y <= humanRotationA) {
			human.rotation.y = humanRotationA; 
			humanState = WALKING_B; // pętla
		}
		break;
	}
	}
}

void animateEagle(float deltaTime) {

	eagle_current_angle += eagle_flight_speed * deltaTime;

	if (eagle_current_angle > 2 * PI) {
		eagle_current_angle -= 2 * PI;
	}

	wind_eagle.position.x = eagle_center.x + eagle_radiusX * cos(eagle_current_angle);
	wind_eagle.position.z = eagle_center.z + eagle_radiusZ * sin(eagle_current_angle);
	wind_eagle.position.y = eagle_center.y + 0.2f * sin(eagle_current_angle * 2.0f);

	float tangent_x = -eagle_radiusX * sin(eagle_current_angle);
	float tangent_z = eagle_radiusZ * cos(eagle_current_angle);

	float angle_rad = atan2(tangent_z, tangent_x);

	wind_eagle.rotation.y = -glm::degrees(angle_rad) + 90.0f;
}