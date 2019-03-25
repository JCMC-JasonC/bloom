// Core Libraries
#include <iostream>
#include <string>
#include <math.h>
#include <map> // for std::map
#include <memory> // for std::shared_ptr
#include <fstream>

// 3rd Party Libraries
#define GLEW_STATIC
#include <GLEW\glew.h>
#include <GLUT/freeglut.h>
#include <TTK\OBJMesh.h>
#include <TTK\Camera.h>
#include <TTK\Texture2D.h>
#include <imgui\imgui_impl.h>
#include <glm\vec3.hpp>
#include <glm\gtx\color_space.hpp>
#include "FrameBufferObject.h"

// User Libraries
#include "Shader.h"
#include "ShaderProgram.h"
#include "GameObject.h"
#include "TTK\Utilities.h"

// Defines and Core variables
#define FRAMES_PER_SECOND 60
const int FRAME_DELAY = 1000 / FRAMES_PER_SECOND; // Milliseconds per frame

int windowWidth = 1920;
int windowHeight = 1080;

glm::vec3 mousePosition; // x,y,0
glm::vec3 mousePositionFlipped; // x, height - y, 0

// A few conversions to know
const float degToRad = 3.14159f / 180.0f;
const float radToDeg = 180.0f / 3.14159f;

float deltaTime = 0.0f; // amount of time since last update (set every frame in timer callback)

glm::vec3 position;
float movementSpeed = 5.0f;
glm::vec4 lightPos;

bool paused = false;

static int mode = 1;

// Cameras
TTK::Camera playerCamera; // the camera you move around with wasd + mouse
TTK::Camera renderCamera; // the camera we render the scene with when generating the fbo texture

// Asset databases
// A std::map is just like a std::vector, but instead of using an integer to index into the array, you can use a templated type
// In the following maps we use strings as the indices into the arrays
std::map<std::string, std::shared_ptr<TTK::MeshBase>> meshes;
std::map<std::string, std::shared_ptr<GameObject>> gameobjects;
std::map<std::string, std::shared_ptr<TTK::Texture2D>> textures;

// Materials
std::map<std::string, std::shared_ptr<Material>> materials;

FrameBufferObject fbo;

void initializeShaders()
{
	std::string shaderPath = "../../Assets/Shaders/";

	// Load shaders

	// Load Vertex Shaders
	Shader v_default;
	v_default.loadShaderFromFile(shaderPath + "default_v.glsl", GL_VERTEX_SHADER);

	// Load Fragment Shaders
	Shader f_default, f_unlitTex, f_distort;
	f_default.loadShaderFromFile(shaderPath + "default_f.glsl", GL_FRAGMENT_SHADER);
	f_unlitTex.loadShaderFromFile(shaderPath + "unlitTexture_f.glsl", GL_FRAGMENT_SHADER);
	f_distort.loadShaderFromFile(shaderPath + "distort_f.glsl", GL_FRAGMENT_SHADER);

	// Default lambert material
	// This is how we access elements in the materials map
	materials["default"] = std::make_shared<Material>();
	materials["default"]->shader->attachShader(v_default); // Vertex Shader
	materials["default"]->shader->attachShader(f_default); // Fragment Shader
	materials["default"]->shader->linkProgram();

	materials["unlitTexture"] = std::make_shared<Material>();
	materials["unlitTexture"]->shader->attachShader(v_default); // Vertex Shader
	materials["unlitTexture"]->shader->attachShader(f_unlitTex); // Fragment Shader
	materials["unlitTexture"]->shader->linkProgram();

	materials["distort"] = std::make_shared<Material>();
	materials["distort"]->shader->attachShader(v_default); // Vertex Shader
	materials["distort"]->shader->attachShader(f_distort); // Fragment Shader
	materials["distort"]->shader->linkProgram();

	fbo.createFrameBuffer(windowWidth, windowHeight, true);
}

void loadMeshes()
{
	// Load meshes
	std::string meshPath = "../../Assets/Models/";

	std::shared_ptr<TTK::OBJMesh> floorMesh = std::make_shared<TTK::OBJMesh>();
	std::shared_ptr<TTK::OBJMesh> sphereMesh = std::make_shared<TTK::OBJMesh>();
	std::shared_ptr<TTK::OBJMesh> torusMesh = std::make_shared<TTK::OBJMesh>();
	std::shared_ptr<TTK::OBJMesh> cubeMesh = std::make_shared<TTK::OBJMesh>();

	floorMesh->loadMesh(meshPath + "floor.obj");
	sphereMesh->loadMesh(meshPath + "sphere.obj");
	torusMesh->loadMesh(meshPath + "torus.obj");
	cubeMesh->loadMesh(meshPath + "cube.obj");

	// Note: looking up a mesh by it's string name is not the fastest thing,
	// you don't want to do this every frame, once in a while (like now) is fine.
	// If you need you need constant access to a mesh (i.e. you need it every frame),
	// store a reference to it so you don't need to look it up every time.
	meshes["floor"] = floorMesh;
	meshes["sphere"] = sphereMesh;
	meshes["torus"] = torusMesh;
	meshes["cube"] = cubeMesh;
	meshes["quad"] = createQuadMesh();
}

void loadTextures()
{
	// Load textures
	std::string texturesPath = "../../Assets/Textures/";

	// ... try to put a texture on an object
}

void initializeScene()
{
	loadMeshes();
	loadTextures();

	// Create objects
	// Get reference to default material (created in initializeShaders())
	auto defaultMaterial = materials["default"];

	gameobjects["floor"] = std::make_shared<GameObject>(glm::vec3(0.0f, 0.0f, 0.0f), meshes["floor"], defaultMaterial);
	gameobjects["sphere"] = std::make_shared<GameObject>(glm::vec3(0.0f, 5.0f, 0.0f), meshes["sphere"], defaultMaterial);
	
	// Set object properties
	gameobjects["sphere"]->colour = glm::vec4(1.0f);

	// Generate a bunch of objects in a circle
	int numObjects = 12;
	float circleStep = 360.f / numObjects;
	float colorStep = 1.0f / numObjects;
	for (int i = 0; i < numObjects; i++)
	{
		glm::vec3 pos, colour = getColorFromHue(colorStep * i);
		std::string name = ("torus" + std::to_string(i));
		pos.x = cos(circleStep*(float)i*degToRad) * 10.0f;
		pos.y = 2.0f;
		pos.z = sin(circleStep*(float)i*degToRad) * 10.0f;
		gameobjects[name] = std::make_shared<GameObject>(pos, meshes["torus"], defaultMaterial);
		gameobjects[name]->colour = glm::vec4(colour, 1.0f);
	}
}

void updateScene()
{
	// Move light in simple circular path
	static float ang = 0.0f;

	if (!paused)
		ang += deltaTime; // comment out to pause light
	const float radius = 15.0f;
	lightPos.x = cos(ang) * radius;
	lightPos.y = cos(ang*4.0f) * 2.0f + 15.0f;
	lightPos.z = sin(ang) * radius;
	lightPos.w = 1.0f;

	gameobjects["sphere"]->setPosition(lightPos);

	// Update all game objects
	for (auto itr = gameobjects.begin(); itr != gameobjects.end(); ++itr)
	{
		auto gameobject = itr->second;

		// Remember: root nodes are responsible for updating all of its children
		// So we need to make sure to only invoke update() for the root nodes.
		// Otherwise some objects would get updated twice in a frame!
		if (gameobject->isRoot())
			gameobject->update(deltaTime);
	}
}

void drawScene(TTK::Camera& cam)
{
	for (auto itr = gameobjects.begin(); itr != gameobjects.end(); ++itr)
	{
		auto gameobject = itr->second;

		if (gameobject->isRoot())
			gameobject->draw(cam);
	}
}

// Helpful function to apply a shader program on all objects
void setMaterialForAllGameObjects(std::string materialName)
{
	auto mat = materials[materialName];
	for (auto itr = gameobjects.begin(); itr != gameobjects.end(); ++itr)
	{
		itr->second->material = mat;
	}
}

// This is where we draw stuff
void DisplayCallbackFunction(void)
{
	// Update cameras
	renderCamera.update();
	playerCamera.update();

	// Update all gameobjects
	updateScene();

	// Set material properties
	auto defaultMaterial = materials["default"];
	defaultMaterial->vec4Uniforms["u_lightPos"] = playerCamera.viewMatrix * lightPos; // EYE SPACE	

	// draw the scene
	TTK::StartUI(windowWidth, windowHeight);

	if (mode == 1) // render straight to back buffer
	{
		// Clear back buffer
		glClearColor(0.5f, 0.5f, 0.6f, 0.0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		drawScene(renderCamera);
	}
	else if (mode == 2)// fbo demo
	{
		// clear fbo
		fbo.bindFrameBufferForDrawing();
		fbo.clearFrameBuffer(glm::vec4(0.5f, 0.5f, 0.6f, 0.0));
		drawScene(renderCamera);
		fbo.unbindFrameBuffer(windowWidth, windowHeight);

		// clear back buffer
		glClearColor(0.0f, 0.0f, 0.0f, 0.0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		static bool drawWireframe = false;
		ImGui::Checkbox("Draw Wire", &drawWireframe);

		if (drawWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		materials["unlitTexture"]->bind();
		materials["unlitTexture"]->mat4Uniforms["u_mvp"] = playerCamera.viewProjMatrix * glm::translate(glm::vec3(0.0f, 2.0f, 0.0f));
		materials["unlitTexture"]->sendUniforms();
		fbo.bindTextureForSampling(GL_TEXTURE0);
		meshes["quad"]->draw();

		static float offset = 0.0f;
		static bool applyOffset = true;

		static float waveLength = 0.35;
		static float period = 0.167;
		static float amplitude = 0.03;

		ImGui::DragFloat("Wave Length", &waveLength, 0.001);
		ImGui::DragFloat("Period", &period, 0.001);
		ImGui::DragFloat("Amplitude", &amplitude, 0.001);
		ImGui::Checkbox("Apply Offset", &applyOffset);

		static bool drawDistort = false;
		ImGui::Checkbox("Draw Distort", &drawDistort);

		if (drawDistort)
		{
			static auto distortMat = materials["distort"];
			distortMat->bind();
			distortMat->vec4Uniforms["u_texelSize"] = glm::vec4((1.0f / (float)windowWidth, 1.0f / (float)windowHeight, 0.0f, 0.0f));
			distortMat->mat4Uniforms["u_mvp"] = playerCamera.viewProjMatrix * glm::translate(glm::vec3(2.0f, 4.0f, 0.0f));

			if (applyOffset)
			{
				static float modifier = 1.0f;
				offset += deltaTime * modifier;
				if (offset >= 1.0f || offset <= 0.0f)
					modifier *= -1.0f;
				distortMat->floatUniforms["waveLength"] = waveLength + offset;
				distortMat->floatUniforms["period"] = period + offset;
				distortMat->floatUniforms["amplitude"] = amplitude;
			}
			else
			{
				distortMat->floatUniforms["waveLength"] = waveLength;
				distortMat->floatUniforms["period"] = period;
				distortMat->floatUniforms["amplitude"] = amplitude;
			}
			distortMat->sendUniforms();
			fbo.bindTextureForSampling(GL_TEXTURE0);
			meshes["quad"]->draw();
		}

		fbo.unbindTexture(GL_TEXTURE0);
		gameobjects["floor"]->draw(playerCamera);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	ImGui::Checkbox("Animate Light", &paused);
	TTK::EndUI();

	/* Swap Buffers to Make it show up on screen */
	glutSwapBuffers();
}

/* function void KeyboardCallbackFunction(unsigned char, int,int)
* Description:
*   - this handles keyboard input when a button is pressed
*/
void KeyboardCallbackFunction(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: // the escape key
		glutExit();
	break;
	case 'q': // the 'q' key
	case 'Q':
		playerCamera.moveUp();
		break;
	case 'e':
	case 'E':
		playerCamera.moveDown();
	case 'W':
	case 'w':
		playerCamera.moveForward();
		break;
	case 'S':
	case 's':
		playerCamera.moveBackward();
		break;
	case 'A':
	case 'a':
		playerCamera.moveRight();
		break;
	case 'D':
	case 'd':
		playerCamera.moveLeft();
		break;
	}

	// Imgui

	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[(int)key] = true;
	io.AddInputCharacter((int)key); // this is what makes keyboard input work in imgui

	// This is what makes the backspace button work
	int keyModifier = glutGetModifiers();
	switch (keyModifier)
	{
	case GLUT_ACTIVE_SHIFT:
		io.KeyShift = true;
		break;

	case GLUT_ACTIVE_CTRL:
		io.KeyCtrl = true;
		break;

	case GLUT_ACTIVE_ALT:
		io.KeyAlt = true;
		break;
	}
}

/* function void KeyboardUpCallbackFunction(unsigned char, int,int)
* Description:
*   - this handles keyboard input when a button is lifted
*/
void KeyboardUpCallbackFunction(unsigned char key, int x, int y)
{
	// Imgui

	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[key] = false;

	int keyModifier = glutGetModifiers();
	io.KeyShift = false;
	io.KeyCtrl = false;
	io.KeyAlt = false;
	switch (keyModifier)
	{
	case GLUT_ACTIVE_SHIFT:
		io.KeyShift = true;
		break;

	case GLUT_ACTIVE_CTRL:
		io.KeyCtrl = true;
		break;

	case GLUT_ACTIVE_ALT:
		io.KeyAlt = true;
		break;
	}

	switch (key)
	{
	case '1':
			mode = 1;
			break;
	case '2':
			mode = 2;
			break;
	}
}

/* function TimerCallbackFunction(int value)
* Description:
*  - this is called many times per second
*  - this enables you to animate things
*  - no drawing, just changing the state
*  - changes the frame number and calls for a redisplay
*  - FRAME_DELAY is the number of milliseconds to wait before calling the timer again
*/
void TimerCallbackFunction(int value)
{
	// Calculate new deltaT for potential updates and physics calculations
	static int elapsedTimeAtLastTick = 0;
	int totalElapsedTime = glutGet(GLUT_ELAPSED_TIME);

	deltaTime = float(totalElapsedTime - elapsedTimeAtLastTick);
	deltaTime /= 1000.0f;
	elapsedTimeAtLastTick = totalElapsedTime;

	/* this call makes it actually show up on screen */
	glutPostRedisplay();
	/* this call gives it a proper frame delay to hit our target FPS */
	glutTimerFunc(FRAME_DELAY, TimerCallbackFunction, 0);
}

/* function WindowReshapeCallbackFunction()
* Description:
*  - this is called whenever the window is resized
*  - and sets up the projection matrix properly
*/
void WindowReshapeCallbackFunction(int w, int h)
{
	/* Update our Window Properties */
	windowWidth = w;
	windowHeight = h;

	playerCamera.winHeight = (float)h;
	playerCamera.winWidth = (float)w;

	glViewport(0, 0, w, h);
}


void MouseClickCallbackFunction(int button, int state, int x, int y)
{
	mousePosition.x = (float)x;
	mousePosition.y = (float)y;

	mousePositionFlipped = mousePosition;
	mousePositionFlipped.y = windowHeight - mousePosition.y;

	ImGui::GetIO().MouseDown[0] = !state;
}

void SpecialInputCallbackFunction(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		position.z += movementSpeed * deltaTime;
		break;
	case GLUT_KEY_DOWN:
		position.z -= movementSpeed * deltaTime;
		break;
	case GLUT_KEY_LEFT:
		position.x += movementSpeed * deltaTime;
		break;
	case GLUT_KEY_RIGHT:
		position.x -= movementSpeed * deltaTime;
		break;
	}
}

// Called when the mouse is clicked and moves
void MouseMotionCallbackFunction(int x, int y)
{
	ImGui::GetIO().MousePos = ImVec2((float)x, (float)y);

	if (!ImGui::GetIO().WantCaptureMouse)
	{
		if (mousePosition.length() > 0)
			playerCamera.processMouseMotion(x, y, (int)mousePosition.x, (int)mousePosition.y, deltaTime);
	}

	mousePosition.x = (float)x;
	mousePosition.y = (float)y;

	mousePositionFlipped = mousePosition;
	mousePositionFlipped.y = windowHeight - mousePosition.y;

}

// Called when the mouse is moved inside the window
void MousePassiveMotionCallbackFunction(int x, int y)
{
	ImGui::GetIO().MousePos = ImVec2((float)x, (float)y);

	mousePositionFlipped.x = (float)x;
	mousePositionFlipped.y = (float)(windowHeight - y);
}

/* function main()
* Description:
*  - this is the main function
*  - does initialization and then calls glutMainLoop() to start the event handler
*/
int main(int argc, char **argv)
{
	// Memory Leak Detection
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	/* initialize the window and OpenGL properly */

	//////////////////////////////////////////////////////////////////////////
	// NOTE:
	// Must set a CORE_PROFILE for render doc to work
	// Must use FREEGLUT instead of GLUT
	//////////////////////////////////////////////////////////////////////////
	glutInitContextVersion(4, 4);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInit(&argc, argv);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("Tutorial");

	auto s = glGetString(GL_VERSION);
	std::cout << s << std::endl;

	/* set up our function callbacks */
	glutDisplayFunc(DisplayCallbackFunction);
	glutKeyboardFunc(KeyboardCallbackFunction);
	glutKeyboardUpFunc(KeyboardUpCallbackFunction);
	glutReshapeFunc(WindowReshapeCallbackFunction);
	glutMouseFunc(MouseClickCallbackFunction);
	glutMotionFunc(MouseMotionCallbackFunction);
	glutTimerFunc(1, TimerCallbackFunction, 0);
	glutSpecialFunc(SpecialInputCallbackFunction);
	glutPassiveMotionFunc(MousePassiveMotionCallbackFunction);


	// Init GLEW
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cout << "TTK::InitializeTTK Error: GLEW failed to init" << std::endl;
	}
	printf("OpenGL version: %s, GLSL version: %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

	// Init ImGUI
	TTK::InitImGUI();

	int num_ext = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &num_ext);
	for (int i = 0; i < num_ext; i++)
	{
		const char* str = (const char*)glGetStringi(GL_EXTENSIONS, i);
		if (!strcmp(str, "GL_ARB_compatibility"))
			printf("Compatiblity Profile! RENDER DOC WILL NOT WORK!!\n");
	}

	// Init GL
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Initialize scene
	initializeShaders();
	initializeScene();

	/* Start Game Loop */
	deltaTime = (float)glutGet(GLUT_ELAPSED_TIME);
	deltaTime /= 1000.0f;

	glutMainLoop();

	return 0;
}