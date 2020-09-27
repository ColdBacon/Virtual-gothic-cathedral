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

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "lodepng.h"
#include "shaderprogram.h"
#include <iostream>
#include <fstream>

using namespace std;

#include <fstream>

GLuint textures[37];

void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

int main(void);

// ustawienia wymiarów okna
const unsigned int height = 800;
const unsigned int width = 1000;

const unsigned int collision_detector_x = 412; //od -54 do 28
const unsigned int collision_detector_y = 232; //od -23 do 23
bool collision_detector[collision_detector_x][collision_detector_y];//tablica do wykrywania zderzeń

std::vector <std::vector<glm::vec4>> whole_vertices;
std::vector <std::vector<glm::vec4>> whole_normals;
std::vector <std::vector<glm::vec2>> whole_uvs;

std::string Map_Kd[37];//zmienne do przechowywania informacji o materiałach
glm::vec3 Kd[37];
glm::vec3 Ka[37];
glm::vec3 Ks[37];

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, -32.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = 0.0f; //prawo lewo
float pitch = 0.0f; // góra dół
float lastX = 400;
float lastY = 300;

#pragma pack(push, 1)
typedef struct Mesh {
	int mesh_number;
	vector< glm::vec4> vertices_vector;
	vector< glm::vec4 > normals_vector;
	vector< glm::vec2 > uvs_vector;
};
#pragma pack(pop)

struct Vertex {
	float X;
	float Y;
	float Z;
	float W;
};

struct TexCoord {
	float X;
	float Y;
};

void writeFile(
	std::vector < glm::vec4 > vertices_out,
	std::vector < glm::vec4 > normals_out,
	std::vector < glm::vec2 > uvs_out,
	int i)//numer meshu
{
	std::ofstream plik_vertex("binary/" + std::to_string(i) + "vertex.bin", std::ios::binary);
	for (int i = 0; i < vertices_out.size(); i++) {
		Vertex v;
		v.X = vertices_out[i].x;
		v.Y = vertices_out[i].y;
		v.Z = vertices_out[i].z;
		v.W = vertices_out[i].w;
		plik_vertex.write((const char*)&v, sizeof v);
	}
	plik_vertex.close();
	std::ofstream plik_normal("binary/" + std::to_string(i) + "normal.bin", std::ios::binary);
	for (int i = 0; i < normals_out.size(); i++) {
		Vertex v;
		v.X = normals_out[i].x;
		v.Y = normals_out[i].y;
		v.Z = normals_out[i].z;
		v.W = normals_out[i].w;
		plik_normal.write((const char*)&v, sizeof v);
	}
	plik_normal.close();
	std::ofstream plik_TexCoord("binary/" + std::to_string(i) + "TexCoord.bin", std::ios::binary);
	for (int i = 0; i < uvs_out.size(); i++) {
		TexCoord v;
		v.X = uvs_out[i].x;
		v.Y = uvs_out[i].y;
		plik_TexCoord.write((const char*)&v, sizeof v);
	}
	plik_TexCoord.close();
}

void readFile(int mes_number) {
	std::vector < glm::vec4 > vertices_out;
	std::vector < glm::vec2 > uvs_out;
	std::vector < glm::vec4 > normals_out;

	std::ifstream plik_vertex("binary/" + std::to_string(mes_number) + "vertex.bin", std::ios::binary);
	if (plik_vertex){
		Vertex v;
		while (plik_vertex.read((char*)&v, sizeof v)) {
			//printf("vertices_out: %f, %f, %f, %f\n", v.X, v.Y, v.Z, v.W);
			vertices_out.push_back(glm::vec4(v.X, v.Y, v.Z, v.W));
		}
	}
	plik_vertex.close();
	std::ifstream plik_normal("binary/" + std::to_string(mes_number) + "normal.bin", std::ios::binary);
	if (plik_normal) {
		Vertex v;
		while (plik_normal.read((char*)&v, sizeof v)) {
			//printf("normals_out: %f, %f, %f, %f\n", v.X, v.Y, v.Z, v.W);
			normals_out.push_back(glm::vec4(v.X, v.Y, v.Z, v.W));
		}
	}
	plik_normal.close();
	std::ifstream plik_TexCoord("binary/" + std::to_string(mes_number) + "TexCoord.bin", std::ios::binary);
	if(plik_TexCoord) {
		TexCoord v;
		while (plik_TexCoord.read((char*)&v, sizeof v)) {
			//printf("uvs_out: %f, %f\n", v.X, v.Y);
			uvs_out.push_back(glm::vec2(v.X, v.Y));
		}
	}
	plik_TexCoord.close();

	whole_vertices.push_back(vertices_out);
	whole_normals.push_back(normals_out);
	whole_uvs.push_back(uvs_out);
}


void processInput(GLFWwindow* window)
{
	int future_x, future_z, actual_x, actual_z;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	const float cameraSpeed = 0.05f; // adjust accordingly
	future_x = int(5 * (cameraPos.x + cameraSpeed * cameraFront.x)) + (collision_detector_x / 2 + 65);
	future_z = int(5 * (cameraPos.z + cameraSpeed * cameraFront.z)) + (collision_detector_y / 2);
	actual_x = int(5 * cameraPos.x) + (collision_detector_x / 2 + 65);
	actual_z = int(5 * cameraPos.z) + (collision_detector_y / 2);
	if (cameraPos.x < 0) actual_x--;
	if (cameraPos.z < 0) actual_z--;
	//printf("przyszle wspolrzedne przed w x: %f, z: %f\n", cameraPos.x + cameraSpeed * cameraFront.x, cameraPos.z + cameraSpeed * cameraFront.z);//przed if

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		future_x = int(5 * (cameraPos.x + cameraSpeed * cameraFront.x)) + (collision_detector_x / 2 + 65);
		future_z = int(5 * (cameraPos.z + cameraSpeed * cameraFront.z)) + (collision_detector_y / 2);
		if (cameraPos.x + cameraSpeed * cameraFront.x < 0) future_x--;//odejmuję 1 ponieważ przy zmianie znaku współrzędnej, nie zmieniało się zaokrąglenie
		if (cameraPos.z + cameraSpeed * cameraFront.z < 0) future_z--;
		if (collision_detector[future_x][future_z] == 1) cameraPos += glm::vec3(cameraSpeed * cameraFront.x, 0, cameraSpeed * cameraFront.z);
		//if (collision_detector[actual_x][actual_z] == 0) cameraPos -= glm::vec3(cameraSpeed * cameraFront.x, 0, cameraSpeed * cameraFront.z);
		//printf("przyszle wspolrzedne po w x: %f, z: %f\n", cameraPos.x + cameraSpeed * cameraFront.x, cameraPos.z + cameraSpeed * cameraFront.z);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		future_x = int(5 * (cameraPos.x - cameraSpeed * cameraFront.x)) + (collision_detector_x / 2 + 65);
		future_z = int(5 * (cameraPos.z - cameraSpeed * cameraFront.z)) + (collision_detector_y / 2);
		if (cameraPos.x - cameraSpeed * cameraFront.x < 0) future_x--;
		if (cameraPos.z - cameraSpeed * cameraFront.z < 0) future_z--;
		if (collision_detector[future_x][future_z] == 1) cameraPos -= glm::vec3(cameraSpeed * cameraFront.x, 0, cameraSpeed * cameraFront.z);
		//if (collision_detector[actual_x][actual_z] == 0) cameraPos += glm::vec3(cameraSpeed * cameraFront.x, 0, cameraSpeed * cameraFront.z);
		//printf("przyszle wspolrzedne s x: %f, z: %f\n", cameraPos.x - cameraSpeed * cameraFront.x, cameraPos.z - cameraSpeed * cameraFront.z);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		glm::vec3 to_add = glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		future_x = int(5 * (cameraPos.x - to_add.x)) + (collision_detector_x / 2 + 65);
		future_z = int(5 * (cameraPos.z - to_add.z)) + (collision_detector_y / 2);
		if (cameraPos.x - to_add.x < 0) future_x--;
		if (cameraPos.z - to_add.z < 0) future_z--;
		if (collision_detector[future_x][future_z] == 1) cameraPos -= to_add;
		//if (collision_detector[actual_x][actual_z] == 0) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		//printf("przyszle wspolrzedne a x: %f, z: %f\n", cameraPos.x - to_add.x, cameraPos.z - to_add.z);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		glm::vec3 to_add = glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		future_x = int(5 * (cameraPos.x + to_add.x)) + (collision_detector_x / 2 + 65);
		future_z = int(5 * (cameraPos.z + to_add.z)) + (collision_detector_y / 2);
		if (cameraPos.x + to_add.x < 0) future_x--;
		if (cameraPos.z + to_add.z < 0) future_z--;
		if (collision_detector[future_x][future_z] == 1) cameraPos += to_add;
		//if (collision_detector[actual_x][actual_z] == 0) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		//printf("przyszle wspolrzedne d x: %f, z: %f\n", cameraPos.x + to_add.x, cameraPos.z + to_add.z);
	}
	if (actual_x > 290 && actual_x < 374 && actual_z > 75 && actual_z < 158) cameraPos.y = -30.5;
	else if (actual_x == 290 && actual_z < 135 && actual_z > 96) cameraPos.y = -30.5;
	else if ((actual_x == 289 || actual_x == 288) && actual_z < 134 && actual_z > 97) cameraPos.y = -30.5;
	else if ((actual_x == 287 || actual_x == 286) && actual_z < 133 && actual_z > 98) cameraPos.y = -30.5;
	else if (actual_x == 285 && actual_z < 131 && actual_z > 99) cameraPos.y = -30.5;
	else if (actual_x == 284 && actual_z < 130 && actual_z > 100) cameraPos.y = -30.5;
	else if (actual_x == 283 && actual_z < 128 && actual_z > 102) cameraPos.y = -30.5;
	else cameraPos.y = -32;
	//printf("wspolrzedne x: %f, z: %f\n", cameraPos.x, cameraPos.z);
	//printf("wymiar bloku przyszlego x: %d, z: %d, wartosc tablicy: %d\n", future_x, future_z, collision_detector[future_x][future_z]);
	//printf("wymiar bloku terazniejszego x: %d, z: %d, wartosc tablicy: %d\n", actual_x, actual_z, collision_detector[actual_x][actual_z]);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; 
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0 );

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

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	initShaders();
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0, 0, 0, 1); //Ustaw kolor czyszczenia bufora kolorów
	glEnable(GL_DEPTH_TEST); //Włącz test głębokości na pikselach
	for (int i = 0; i < 37; i++) {
		textures[i] = readTexture(("models/" + Map_Kd[i]).c_str());
		printf("%d: %s\n", textures[i], ("models/" + Map_Kd[i]).c_str());
	}
}

//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
	freeShaders();
	//************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************
	glDeleteTextures(37, textures);
}

//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window,
	std::vector <std::vector<glm::vec4>> whole_vertices,
	std::vector <std::vector<glm::vec4>> whole_normals,
	std::vector <std::vector<glm::vec2>> whole_uvs
) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wyczyść bufor koloru i bufor głębokości
	for (int i = 0; i < 37; i++) {
		glm::mat4 M = glm::mat4(1.0f); //Zainicjuj macierz modelu macierzą jednostkową
		glm::mat4 V = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); //Wylicz macierz widoku
		glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 100.0f); //Wylicz macierz rzutowania

		spLambert->use(); //Aktywuj program cieniujący
		glUniformMatrix4fv(spLambert->u("P"), 1, false, glm::value_ptr(P)); //Załaduj do programu cieniującego macierz rzutowania
		glUniformMatrix4fv(spLambert->u("V"), 1, false, glm::value_ptr(V)); //Załaduj do programu cieniującego macierz widoku
		glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(M)); //Załaduj do programu cieniującego macierz modelu

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 4, GL_FLOAT, false, 0, whole_vertices[i].data());
		glVertexAttribPointer(1, 4, GL_FLOAT, false, 0, whole_normals[i].data());
		glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, whole_uvs[i].data());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glUniform1i(spLambert->u("tex"), 0);

		glUniform4f(spLambert->u("Ka"), Ka[i].x, Ka[i].y, Ka[i].z, 1);
		glUniform4f(spLambert->u("Kd"), Kd[i].x, Kd[i].y, Kd[i].z, 1);
		glUniform4f(spLambert->u("Ks"), Ks[i].x, Ks[i].y, Ks[i].z, 1);

		glDrawArrays(GL_TRIANGLES, 0, whole_vertices[i].size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}

	glfwSwapBuffers(window); //Skopiuj bufor tylny do bufora przedniego
}


int main(void)
{
	/*
	bool loadout = Loader.LoadFile("models/katedra.obj");
	//ofstream file1("whole_data.txt", ios::binary);

	const int ilosc_obiektów = Loader.LoadedMeshes.size();
	
	Loader.LoadedVertices;
	//Mesh meshes_struct[2];

	for (int i = 0; i < Loader.LoadedMeshes.size(); i++) {
		objl::Mesh curMesh = Loader.LoadedMeshes[i];
		std::vector < glm::vec4 > vertices_out;//według trójkątów
		std::vector < glm::vec2 > uvs_out;
		std::vector < glm::vec4 > normals_out;

		//meshes_struct[i].mesh_number = i;

		for (int j = 0; j < curMesh.Indices.size(); j++) {
			
			//meshes_struct[i].vertices_vector.push_back(glm::vec4(curMesh.Vertices[curMesh.Indices[j]].Position.X, curMesh.Vertices[curMesh.Indices[j]].Position.Y, curMesh.Vertices[curMesh.Indices[j]].Position.Z, 1));
			//meshes_struct[i].normals_vector.push_back(glm::vec4(curMesh.Vertices[curMesh.Indices[j]].Normal.X, curMesh.Vertices[curMesh.Indices[j]].Normal.Y, curMesh.Vertices[curMesh.Indices[j]].Normal.Z, 0));
			//meshes_struct[i].uvs_vector.push_back(glm::vec2(curMesh.Vertices[curMesh.Indices[j]].TextureCoordinate.X, curMesh.Vertices[curMesh.Indices[j]].TextureCoordinate.Y));

			vertices_out.push_back(glm::vec4(curMesh.Vertices[curMesh.Indices[j]].Position.X, curMesh.Vertices[curMesh.Indices[j]].Position.Y, curMesh.Vertices[curMesh.Indices[j]].Position.Z, 1));
			//printf("vertices_out %d: %f, %f, %f\n", curMesh.Indices[j], curMesh.Vertices[curMesh.Indices[j]].Position.X, curMesh.Vertices[curMesh.Indices[j]].Position.Y, curMesh.Vertices[curMesh.Indices[j]].Position.Z);
			normals_out.push_back(glm::vec4(curMesh.Vertices[curMesh.Indices[j]].Normal.X, curMesh.Vertices[curMesh.Indices[j]].Normal.Y, curMesh.Vertices[curMesh.Indices[j]].Normal.Z, 0));
			//printf("normals_out %d: %f, %f, %f\n", curMesh.Indices[j], curMesh.Vertices[curMesh.Indices[j]].Normal.X, curMesh.Vertices[curMesh.Indices[j]].Normal.Y, curMesh.Vertices[curMesh.Indices[j]].Normal.Z);
			uvs_out.push_back(glm::vec2(curMesh.Vertices[curMesh.Indices[j]].TextureCoordinate.X, curMesh.Vertices[curMesh.Indices[j]].TextureCoordinate.Y));
			//printf("uvs_out %d: %f, %f, %f\n", curMesh.Indices[j], curMesh.Vertices[curMesh.Indices[j]].TextureCoordinate.X, curMesh.Vertices[curMesh.Indices[j]].TextureCoordinate.Y);
		}

		writeFile(vertices_out, normals_out, uvs_out, i);

		whole_vertices.push_back(vertices_out);
		whole_normals.push_back(normals_out);
		whole_uvs.push_back(uvs_out);
	}
	*/

	/*
	file1.write((char*)&meshes_struct, sizeof(meshes_struct)); // zapisujemy dane do pliku
	file1.close();

	//Mesh all_meshes[2];
	ifstream file2("whole_data.txt", ios::binary);
	
	//file2.read((char*)&all_meshes, sizeof(all_meshes));

	char* temp = new char[sizeof(Mesh)]; // tymczasowy bufor na dane
	file2.read(temp, sizeof(Mesh)); // wczytujemy dane do bufora
	Mesh* m = (Mesh*)(temp); // rzutujemy zawartość bufora na typ File
	
	for (size_t i = 0; i < 2; i++) {
		printf("Mesh: %d\n", all_meshes[i].mesh_number);
		printf("%d\n",all_meshes[i].vertices_vector.size());
		for (size_t j = 0; j < all_meshes[i].vertices_vector.size(); j++) {
			printf("vertices_out %d: %f, %f, %f, %f\n", j, all_meshes[i].vertices_vector[j].x, all_meshes[i].vertices_vector[j].y, all_meshes[i].vertices_vector[j].z, all_meshes[i].vertices_vector[j].w);
			printf("normals_out %d: %f, %f, %f, %f\n", j, all_meshes[i].normals_vector[j].x, all_meshes[i].normals_vector[j].y, all_meshes[i].normals_vector[j].z, all_meshes[i].normals_vector[j].w);
			printf("uvs_out %d: %f, %f\n", j, all_meshes[i].uvs_vector[j].x, all_meshes[i].uvs_vector[j].y);
		}
	}
	*/
	/*
	for (size_t j = 0; j < one_mesh.vertices_vector.size(); j ++) {
		printf("vertices_out %d: %f, %f, %f\n", j,one_mesh.vertices_vector[j].x, one_mesh.vertices_vector[j].y, one_mesh.vertices_vector[j].z,one_mesh.vertices_vector[j].w);
		printf("normals_out %d: %f, %f, %f\n", j,one_mesh.normals_vector[j].x, one_mesh.normals_vector[j].y, one_mesh.normals_vector[j].z,one_mesh.normals_vector[j].w);
		printf("uvs_out %d: %f, %f\n", j,one_mesh.uvs_vector[j].x, one_mesh.uvs_vector[j].y);
	}
	*/
	
	std::fstream wczytaj_Map_Kd, wczytaj_Kd, wczytaj_Ka, wczytaj_Ks;
	wczytaj_Map_Kd.open("txt/Map_Kd.txt", std::ios::in);
	wczytaj_Kd.open("txt/Kd.txt", std::ios::in);
	wczytaj_Ks.open("txt/Ks.txt", std::ios::in);
	wczytaj_Ka.open("txt/Ka.txt", std::ios::in);
	if (wczytaj_Map_Kd.is_open())
	{
		int i = 0;
		std::string sciezka;
		while (wczytaj_Map_Kd >> sciezka) //dopóki jest co czytać
		{
			Map_Kd[i++] = sciezka;
		}
	}

	if (wczytaj_Kd.is_open())
	{
		int i = 0;
		float X, Y, Z;
		while (wczytaj_Kd >> X >> Y >> Z) //dopóki jest co czytać
		{
			Kd[i++] = glm::vec3(X, Y, Z);
		}
	}

	if (wczytaj_Ka.is_open())
	{
		int i = 0;
		float X, Y, Z;
		while (wczytaj_Ka >> X >> Y >> Z) //dopóki jest co czytać
		{
			Ka[i++] = glm::vec3(X, Y, Z);
		}
	}

	if (wczytaj_Ks.is_open())
	{
		int i = 0;
		float X, Y, Z;
		while (wczytaj_Ks >> X >> Y >> Z) //dopóki jest co czytać
		{
			Ks[i++] = glm::vec3(X, Y, Z);
		}
	}

	for (int i = 0; i < 37; i++) {
		readFile(i);
	}

	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	for (int i = 0; i < collision_detector_x; i++) {
		for (int j = 0; j < collision_detector_y; j++) {
			collision_detector[i][j] = 1;
		}
	}

	for (int i = 0; i < collision_detector_x; i++) {
		for (int j = 0; j < collision_detector_y; j++) {
			for (int k = 0; k < 4; k++) {
				if (i > 60 + k * 78 && i < 91 + k * 78 && j>38 && j < 66) collision_detector[i][j] = 0;//kolumny po lewej
				if (i > 60 + k * 78 && i < 91 + k * 78 && j>166 && j < 194) collision_detector[i][j] = 0;//kolumny po prawej
			}
			for (int m = 0; m < 3; m++) {
				//okrągłe kolumny w ściane po lewej i prawej
				if (i > 64 + m * 78 && i < 87 + m * 78 && (j < 2 || j > 230)) collision_detector[i][j] = 0;
				else if (i > 65 + m * 78 && i < 86 + m * 78 && (j < 3 || j > 229)) collision_detector[i][j] = 0;
				else if (i > 66 + m * 78 && i < 85 + m * 78 && (j < 4 || j > 228)) collision_detector[i][j] = 0;
				else if (i > 68 + m * 78 && i < 83 + m * 78 && (j < 5 || j > 227)) collision_detector[i][j] = 0;
				else if (i > 70 + m * 78 && i < 81 + m * 78 && (j < 6 || j > 225)) collision_detector[i][j] = 0;
				else if (i > 74 + m * 78 && i < 77 + m * 78 && (j < 7 || j > 224)) collision_detector[i][j] = 0;
			}
			if (i == 0 || i == collision_detector_x - 1 || j == 0 || j == collision_detector_y - 1) collision_detector[i][j] = 0;//mury katedry
			else if (i > 347 && i < 379 && ((j > 69 && j < 101) || (j > 131 && j < 162))) collision_detector[i][j] = 0; //kolumny wbudowane w podest
			else if (i < 13 && ((j > 38 && j < 66) || (j > 166 && j < 194))) collision_detector[i][j] = 0; //kwadratowe kolumny wbudowane w ściane obok wejścia
			else if (i > 317 && i < 346 && j > 94 && j < 136) collision_detector[i][j] = 0; //ołtarzu
			else if (i > 316 && i < 404 && (j < 0.5698 * i - 174.61 || j > -0.5698 * i + 406.6294)) collision_detector[i][j] = 0;//lewy i prawy skos ściany
			else if ((i < 3 && (j < 6 || j > 226)) || (i < 5 && (j < 5 || j > 227)) || (i < 7 && (j < 4 || j > 228)) || (i < 8 && (j < 3 || j > 229)) || (i < 9 && (j < 2 || j > 230))) collision_detector[i][j] = 0; //okrągłe kolumny w rogu
			else if (i > 246 && i < 257 && ((j > 60 && j < 110) || (j > 120 && j < 172))) collision_detector[i][j] = 0; // ławka 1 od ołtarza
			else if (i > 234 && i < 245 && ((j > 60 && j < 111) || (j > 121 && j < 172))) collision_detector[i][j] = 0; // ławka 2
			else if (i > 222 && i < 233 && ((j > 60 && j < 110) || (j > 121 && j < 172))) collision_detector[i][j] = 0; // ławka 3
			else if (i > 211 && i < 221 && ((j > 60 && j < 111) || (j > 120 && j < 172))) collision_detector[i][j] = 0; // ławka 4
			else if (i > 200 && i < 210 && ((j > 60 && j < 109) || (j > 121 && j < 172))) collision_detector[i][j] = 0; // ławka 5
			else if (i > 188 && i < 199 && ((j > 60 && j < 110) || (j > 121 && j < 172))) collision_detector[i][j] = 0; // ławka 6
			else if (i > 177 && i < 187 && ((j > 60 && j < 110) || (j > 120 && j < 172))) collision_detector[i][j] = 0; // ławka 7
			else if (i > 165 && i < 176 && ((j > 60 && j < 111) || (j > 121 && j < 172))) collision_detector[i][j] = 0; // ławka 8
			else if (i > 154 && i < 164 && ((j > 60 && j < 111) || (j > 120 && j < 172))) collision_detector[i][j] = 0; // ławka 9
			else if (i > 142 && i < 153 && ((j > 60 && j < 111) || (j > 120 && j < 172))) collision_detector[i][j] = 0; // ławka 10
			else if ((i > 299 && (j < 2 || j > 228)) || (i > 301 && (j < 4 || j > 226)) || (i > 304 && (j < 5 || j > 225)) || (i > 306 && (j < 6 || j > 225))) collision_detector[i][j] = 0; // kolumny 2 od konca na rogu
			else if ((i > 403 && (j < 61 || j > 171)) || (i > 404 && (j < 62 || j > 170)) || (i > 405 && (j < 63 || j > 168)) || (i > 406 && (j < 64 || j > 167)) || (i > 408 && (j < 65 || j > 166)) || (i > 409 && (j < 66 || j > 165))) collision_detector[i][j] = 0; //kolumny w rogach na końcu katedry
		}
	}

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(width, height, "Virtual gothic cathedral", NULL, NULL); //utwórz okno o rozmiarze i kolorze

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	// tell GLFW to capture our mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę pre
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	//Główna pętla
	glfwSetTime(0); //Wyzeruj licznik czasu
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		//angle += speed * glfwGetTime(); //Oblicz kąt o jaki obiekt obrócił się podczas poprzedniej klatki
		processInput(window);
		glfwSetTime(0); //Wyzeruj licznik czasu
		drawScene(window, whole_vertices, whole_normals, whole_uvs); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}


