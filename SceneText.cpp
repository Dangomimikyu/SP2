#include "SceneText.h"
#include "GL\glew.h"
#include "Application.h"
#include <Mtx44.h>
#include "shader.hpp"
#include "MeshBuilder.h"
#include "Utility.h"
#include "LoadTGA.h"

#define ROT_LIMIT 45.f;
#define SCALE_LIMIT 5.f;
#define LSPEED 10.f

SceneText::SceneText()
{
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = NULL;
	}
}

SceneText::~SceneText()
{
}

void SceneText::Init()
{
	gamer.setRadius(1);
	walkingX = 5;
	walkingZ = 5;
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	sphere = new CCollision;
	cube = new CRectangle;

	for (int i = 0; i < 5; ++i) {
		carDisplay[i] = new CCollision;
	}

	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	camera.Init(Vector3(0, 0.2f, 4), Vector3(0, 0.2f, 0), Vector3(0, 1, 0));
	//camera.Init(Vector3(0, 0.2f, 4), gamer_transform[0].translation, Vector3(0, 1, 0)); // for when we have the actual player and can keep track of their location to point the camera at them

	Mtx44 projection;
	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);
	
	m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Text.fragmentshader");
	
	//m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Texture.fragmentshader"); 

	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");

	//Get a handle for our "colorTexture" uniform
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");
	// Get a handle for our "textColor" uniform
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");


	glUseProgram(m_programID);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// init lights
	InitLights();
	
	// init skybox
	InitSkybox();

	// init NPCs
	InitNPCs();
	ClosestNPC = -1;

	// init player

	// init objects
	InitObjs();

	meshList[GEO_CHAR] = MeshBuilder::GenerateQuad("char", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_CHAR]->textureID = LoadTGA("Image//char.tga");

	meshList[GEO_LIGHTSPHERE] = MeshBuilder::GenerateSphere("lightBall", Color(1.f, 1.f, 1.f), 9, 36, 1.f);

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//calibri.tga");

	meshList[GEO_DICE] = MeshBuilder::GenerateSphere("sphere", Color(1.f, 0, 0), 9, 36, 1.f);

	meshList[GEO_CUBE] = MeshBuilder::GenerateCuboid("cuboid", Color(1.f, 0, 0), 1, 1, 1);
}

void SceneText::Update(double dt)
{
	sphere->set_transformation('t', Vector3(spheree.translation.x, spheree.translation.y, spheree.translation.z));
	sphere->roundCollision(gamer, playerPos, 1);
	cube->set_transformation('t', Vector3(8, 0, 5));
	static_cast<CRectangle*>(cube)->RectCollision(gamer, playerPos, 6.8f, 4.f, 2.3f, 2.f);
	/*cube.set_transformation('t', Vector3(cubee.translation.x, cubee.translation.y, cubee.translation.z));
	sphere.roundCollision(gamer, player, 1);
	cube.roundCollision(gamer, player, 1);*/

	carDisplay[0]->set_transformation('t', obj_transform[ENV_CAR_DISPLAY_PLATFORM_1].translation);
	carDisplay[0]->roundCollision(gamer, playerPos, 6);

	carDisplay[1]->set_transformation('t', obj_transform[ENV_CAR_DISPLAY_PLATFORM_2].translation);
	carDisplay[1]->roundCollision(gamer, playerPos, 6);

	carDisplay[2]->set_transformation('t', obj_transform[ENV_CAR_DISPLAY_PLATFORM_3].translation);
	carDisplay[2]->roundCollision(gamer, playerPos, 6);

	carDisplay[3]->set_transformation('t', obj_transform[ENV_CAR_DISPLAY_PLATFORM_4].translation);
	carDisplay[3]->roundCollision(gamer, playerPos, 6);

	carDisplay[4]->set_transformation('t', obj_transform[ENV_CAR_DISPLAY_PLATFORM_5].translation);
	carDisplay[4]->roundCollision(gamer, playerPos, 6);


	CCollision* objects[7] = { sphere, cube, carDisplay[0], carDisplay[1], carDisplay[2], carDisplay[3], carDisplay[4] };


	if (Application::IsKeyPressed(0x31))
	{
		glDisable(GL_CULL_FACE);
	}
	else if (Application::IsKeyPressed(0x32))
	{
		glEnable(GL_CULL_FACE);
	}
	else if (Application::IsKeyPressed(0x0))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if (Application::IsKeyPressed(0x34))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (Application::IsKeyPressed('I'))
		light[0].position.z -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('K'))
		light[0].position.z += (float)(LSPEED * dt);
	if (Application::IsKeyPressed('J'))
		light[0].position.x -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('L'))
		light[0].position.x += (float)(LSPEED * dt);
	if (Application::IsKeyPressed('O'))
		light[0].position.y -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('P'))
		light[0].position.y += (float)(LSPEED * dt);

	if (Application::IsKeyPressed('Q'))
	{
		//to do: switch light type to POINT and pass the information to
		light[0].type = Light::LIGHT_POINT;
	}
	else if (Application::IsKeyPressed('W'))
	{
		//to do: switch light type to DIRECTIONAL and pass the
		light[0].type = Light::LIGHT_DIRECTIONAL;
	}
	else if (Application::IsKeyPressed('E'))
	{
		//to do: switch light type to SPOT and pass the information to
		light[0].type = Light::LIGHT_SPOT;
	}

	if (Application::IsKeyPressed('V'))
	{
		walkingX -= (float)(20 * dt);
	}
	if (Application::IsKeyPressed('N'))
		walkingX += (float)(20 * dt);
	if (Application::IsKeyPressed('G'))
		walkingZ -= (float)(20 * dt);
	if (Application::IsKeyPressed('B'))
		walkingZ += (float)(20 * dt);
	for (int i = 0; i < 7; i++)
	{
		
		if (objects[i]->getCollide() == true /*&& timelapse >= 3*/) {
			if (Application::IsKeyPressed('V'))
			{
				/*player1.position.x += data[i].getoverlap() * (player1.position.x - data[i].getPositionX()) / data[i].getDistance();
				player1.position.y += data[i].getoverlap() * (player1.position.y - data[i].getPositionZ()) / data[i].getDistance();*/
				walkingX += objects[i]->getOverlap() * (playerPos.translation.x - objects[i]->get_transformation().translation.x) / objects[i]->getDistance();
				walkingZ += objects[i]->getOverlap() * (playerPos.translation.z - objects[i]->get_transformation().translation.z) / objects[i]->getDistance();
				/*timelapse = 0;*/
			}
			if (Application::IsKeyPressed('N'))
			{
				/*player1.position.x -= 1;*/
				/*timelapse = 0;*/
				walkingX += objects[i]->getOverlap() * (playerPos.translation.x - objects[i]->get_transformation().translation.x) / objects[i]->getDistance();
				walkingZ += objects[i]->getOverlap() * (playerPos.translation.z - objects[i]->get_transformation().translation.z) / objects[i]->getDistance();

			}
			if (Application::IsKeyPressed('G'))
			{
				/*player1.position.z += 1;*/
				/*timelapse = 0;*/
				walkingX += objects[i]->getOverlap() * (playerPos.translation.x - objects[i]->get_transformation().translation.x) / objects[i]->getDistance();
				walkingZ += objects[i]->getOverlap() * (playerPos.translation.z - objects[i]->get_transformation().translation.z) / objects[i]->getDistance();

			}
			if (Application::IsKeyPressed('B'))
			{
				/*player1.position.z -= 1;*/
				/*timelapse = 0;*/
				walkingX += objects[i]->getOverlap() * (playerPos.translation.x - objects[i]->get_transformation().translation.x) / objects[i]->getDistance();
				walkingZ += objects[i]->getOverlap() * (playerPos.translation.z - objects[i]->get_transformation().translation.z) / objects[i]->getDistance();
			}
		}
	}

	// range check for player - NPCs ========================================
	ClosestNPC = -1;
	for (int i = 0; i < NUM_NPC; i++) {
		if ((NPCs_transform[i].translation - playerPos.translation).Length() < 5.f) {
			if ((NPCs_transform[i].translation - playerPos.translation).Length() < (NPCs_transform[ClosestNPC].translation - playerPos.translation).Length()) {
				ClosestNPC = i;
			}	
			NPCinRange = true;
		}
		else {
			NPCinRange = false;
		}
	}
	if (Application::IsKeyPressed('M') && ClosestNPC != -1) {
		NPCs[ClosestNPC].activity(true);
		M_pressed = true;
	}
	else {
		M_pressed = false;
	}

	// range checks for DISPLAY STANDs ===========================
	if ((obj_transform[ENV_CAR_DISPLAY_PLATFORM_1].translation - playerPos.translation).Length() < 15.f) {
		nearCar = true;
	}
	else {
		nearCar = false;
	}

	camera.Update(dt);
}

void SceneText::Render()
{
	//Clear color & depth buffer every frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viewStack.LoadIdentity();
	viewStack.LookAt(camera.position.x, camera.position.y, camera.position.z, camera.target.x, camera.target.y, camera.target.z, camera.up.x, camera.up.y, camera.up.z);
	modelStack.LoadIdentity();

	// passing the light direction if it is a direction light	
	if (light[0].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[0].position.x, light[0].position.y, light[0].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightDirection_cameraspace.x);
	}
	// if it is spot light, pass in position and direction 
	else if (light[0].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[0].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		// default is point light (only position since point light is 360 degrees)
		Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
	}

	RenderSkybox();

	modelStack.PushMatrix();
	modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	RenderMesh(meshList[GEO_LIGHTSPHERE], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	playerPos.translation = Vector3(walkingX, 0, walkingZ);
	RenderObject(meshList[GEO_DICE], playerPos, false, true);
	modelStack.PopMatrix();

	RenderObject(meshList[GEO_NPC_BOB_HEAD], NPCs_transform[NPC_BOB_HEAD], false, true);

	RenderObject(meshList[GEO_ENV_ARCADE_MACHINE_B], obj_transform[ENV_ARCADE_MACHINE_B], false, false);
	RenderObject(meshList[GEO_ENV_ARCADE_MACHINE_G], obj_transform[ENV_ARCADE_MACHINE_G], false, false);
	RenderObject(meshList[GEO_ENV_ARCADE_MACHINE_P], obj_transform[ENV_ARCADE_MACHINE_P], false, false);
	RenderObject(meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_1], obj_transform[ENV_CAR_DISPLAY_PLATFORM_1], true, false);
	RenderObject(meshList[GEO_ENV_CAR], obj_transform[ENV_CAR], false, true);
	modelStack.PopMatrix(); // car hiearchieal to platform 1
	RenderObject(meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_2], obj_transform[ENV_CAR_DISPLAY_PLATFORM_2], false, false);
	RenderObject(meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_3], obj_transform[ENV_CAR_DISPLAY_PLATFORM_3], false, false);
	RenderObject(meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_4], obj_transform[ENV_CAR_DISPLAY_PLATFORM_4], false, false);
	RenderObject(meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_5], obj_transform[ENV_CAR_DISPLAY_PLATFORM_5], false, false);

	RenderObject(meshList[GEO_ENV_SIGNPOST_1], obj_transform[ENV_SIGNPOST_1], false, false);
	RenderObject(meshList[GEO_ENV_SIGNPOST_2], obj_transform[ENV_SIGNPOST_2], false, false);
	RenderObject(meshList[GEO_ENV_SIGNPOST_3], obj_transform[ENV_SIGNPOST_3], false, false);
	RenderObject(meshList[GEO_ENV_SIGNPOST_4], obj_transform[ENV_SIGNPOST_4], false, false);
	RenderObject(meshList[GEO_ENV_SIGNPOST_5], obj_transform[ENV_SIGNPOST_5], false, false);

	spheree.translation = Vector3(0, 0, 0);
	RenderObject(meshList[GEO_DICE], spheree, false, true);

	cubee.translation = Vector3(8, 0, 5);
	cubee.scaling = Vector3(2, 2, 2);
	RenderObject(meshList[GEO_CUBE], cubee, false, true);

	RenderObject(meshList[GEO_ENV_CAR], obj_transform[ENV_CAR], false, true);

	//modelStack.PushMatrix();
	////scale, translate, rotate
	//modelStack.Translate(0, 0, 0);
	//RenderText(meshList[GEO_TEXT], "HELLO WORLD", Color(0, 1, 0));
	//modelStack.PopMatrix();

	//No transform needed
	RenderTextOnScreen(meshList[GEO_TEXT], "Current FPS: " + print_fps(), Color(0, 1, 0), 2, 0, 18);
	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(sphere->getDistance()), Color(0, 1, 0), 2, 0, 0);
	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(NPCinRange), Color(0, 1, 0), 2, 0, 2); // new prints if npc in range

	modelStack.PushMatrix();
	if (sphere->getCollide() == false) {
		RenderTextOnScreen(meshList[GEO_TEXT], "Collision false", Color(0, 1, 0), 2, 0, 1);
	}
	else {
		RenderTextOnScreen(meshList[GEO_TEXT], "Collision true", Color(0, 1, 0), 2, 0, 1);
	}
	modelStack.PopMatrix();

	// NPC - player interaction =======================================
	modelStack.PushMatrix();
	if (M_pressed) {
		if (NPCs[ClosestNPC].get_goodReply()) {
			modelStack.Translate(NPCs_transform[ClosestNPC].translation);
			RenderText(meshList[GEO_TEXT], "how can i help you?", Color(0, 1, 0));
		}
		else if (ClosestNPC != -1) {
			modelStack.Translate(NPCs_transform[ClosestNPC].translation);
			RenderText(meshList[GEO_TEXT], "go away.", Color(0, 1, 0));
		}
	}
	modelStack.PopMatrix();


}

void SceneText::Exit()
{
	// Cleanup here
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if (meshList[i] != NULL)
			delete meshList[i];
	}
	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);

}

void SceneText::InitSkybox()
{
	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1), 1.01f, 1.01f);
	meshList[GEO_LEFT]->textureID = LoadTGA("Image//Building Interior.tga");

	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1), 1.01f, 1.01f);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Image//Building Interior.tga");

	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1), 1.01f, 1.01f);
	meshList[GEO_TOP]->textureID = LoadTGA("Image//Building Interior.tga");

	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1), 1.01f, 1.01f);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Image//Building Floor.tga");

	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1), 1.01f, 1.01f);
	meshList[GEO_FRONT]->textureID = LoadTGA("Image//Building Interior.tga");

	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1), 1.01f, 1.01f);
	meshList[GEO_BACK]->textureID = LoadTGA("Image//Building Interior.tga");
}

void SceneText::InitLights()
{
	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
	m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
	m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");

	light[0].type = Light::LIGHT_POINT;
	light[0].position.Set(0, 5, 0);
	light[0].color.Set(0.5f, 0.5f, 0.5f);
	light[0].power = 1;
	light[0].kC = 1.f;
	light[0].kL = 0.01f;
	light[0].kQ = 0.001f;
	light[0].cosCutoff = cos(Math::DegreeToRadian(45));
	light[0].cosInner = cos(Math::DegreeToRadian(30));
	light[0].exponent = 3.f;
	light[0].spotDirection.Set(0.f, 1.f, 0.f);

	glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);
	glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION], 1, &light[0].spotDirection.x);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], light[0].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], light[0].cosInner);
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], light[0].exponent);
	glUniform1i(m_parameters[U_NUMLIGHTS], 1);
}

void SceneText::InitNPCs()
{
	meshList[GEO_NPC_BOB_HEAD] = MeshBuilder::GenerateOBJ("NPC", "obj//gary.obj");
	meshList[GEO_NPC_BOB_HEAD]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_NPC_BOB_HEAD]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_NPC_BOB_HEAD]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_NPC_BOB_HEAD]->material.kShininess = 0.6f;

	NPCs_transform[NPC_BOB_HEAD].translation = Vector3(5, 0, 0);
	NPCs_transform[NPC_BOB_HEAD].rotationX.angle = 0;
	NPCs_transform[NPC_BOB_HEAD].rotationY.angle = 0;
	NPCs_transform[NPC_BOB_HEAD].rotationZ.angle = 0;
	NPCs_transform[NPC_BOB_HEAD].scaling = Vector3(0.8f, 0.8f, 0.8f);
}

void SceneText::InitObjs()
{
	meshList[GEO_ENV_ARCADE_BUTTON_EXT] = MeshBuilder::GenerateOBJ("arcade button exterior", "obj//Arcade Button_Exterior.obj");
	/*meshList[GEO_ENV_ARCADE_BUTTON_EXT]->textureID = LoadTGA("Image//");*/
	meshList[GEO_ENV_ARCADE_BUTTON_EXT]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_ARCADE_BUTTON_EXT]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_ARCADE_BUTTON_EXT]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_ARCADE_BUTTON_EXT]->material.kShininess = 0.6f;

	obj_transform[ENV_ARCADE_BUTTON_EXT].translation = Vector3(-2.5, 0, -2.5);
	obj_transform[ENV_ARCADE_BUTTON_EXT].rotationX.angle = 0;
	obj_transform[ENV_ARCADE_BUTTON_EXT].rotationY.angle = 0;
	obj_transform[ENV_ARCADE_BUTTON_EXT].rotationZ.angle = 0;
	obj_transform[ENV_ARCADE_BUTTON_EXT].scaling = Vector3(0.8f, 0.8f, 0.8f);
	
	meshList[GEO_ENV_ARCADE_BUTTON_INT] = MeshBuilder::GenerateOBJ("arcade button exterior", "obj//Arcade Button_Interior.obj");
	//meshList[GEO_ENV_ARCADE_BUTTON_INT]->textureID = LoadTGA("Image//");
	meshList[GEO_ENV_ARCADE_BUTTON_INT]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_ARCADE_BUTTON_INT]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_ARCADE_BUTTON_INT]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_ARCADE_BUTTON_INT]->material.kShininess = 0.6f;

	obj_transform[ENV_ARCADE_BUTTON_INT].translation = Vector3(2, -14, 0);
	obj_transform[ENV_ARCADE_BUTTON_INT].rotationX.angle = 0;
	obj_transform[ENV_ARCADE_BUTTON_INT].rotationY.angle = 0;
	obj_transform[ENV_ARCADE_BUTTON_INT].rotationZ.angle = 0;
	obj_transform[ENV_ARCADE_BUTTON_INT].scaling = Vector3(0.8f, 0.8f, 0.8f);
	
	meshList[GEO_ENV_ARCADE_HEADSET] = MeshBuilder::GenerateOBJ("headset", "obj//Arcade VR Headset.obj");
	//meshList[GEO_ENV_ARCADE_HEADSET]->textureID = LoadTGA("Image//");
	meshList[GEO_ENV_ARCADE_HEADSET]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_ARCADE_HEADSET]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_ARCADE_HEADSET]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_ARCADE_HEADSET]->material.kShininess = 0.6f;

	obj_transform[ENV_ARCADE_HEADSET].translation = Vector3(2, -14, 0);
	obj_transform[ENV_ARCADE_HEADSET].rotationX.angle = 0;
	obj_transform[ENV_ARCADE_HEADSET].rotationY.angle = 0;
	obj_transform[ENV_ARCADE_HEADSET].rotationZ.angle = 0;
	obj_transform[ENV_ARCADE_HEADSET].scaling = Vector3(0.8f, 0.8f, 0.8f);
	
	meshList[GEO_ENV_JOYSTICK_BASE] = MeshBuilder::GenerateOBJ("joystick base", "obj//Joystick_Base.obj");
	meshList[GEO_ENV_JOYSTICK_BASE]->textureID = LoadTGA("Image//Joystick_Base.tga");
	meshList[GEO_ENV_JOYSTICK_BASE]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_JOYSTICK_BASE]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_JOYSTICK_BASE]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_JOYSTICK_BASE]->material.kShininess = 0.6f;

	obj_transform[ENV_JOYSTICK_BASE].translation = Vector3(2, -14, 0);
	obj_transform[ENV_JOYSTICK_BASE].rotationX.angle = 0;
	obj_transform[ENV_JOYSTICK_BASE].rotationY.angle = 0;
	obj_transform[ENV_JOYSTICK_BASE].rotationZ.angle = 0;
	obj_transform[ENV_JOYSTICK_BASE].scaling = Vector3(0.8f, 0.8f, 0.8f);
	
	meshList[GEO_ENV_JOYSTICK_CONTROLLER] = MeshBuilder::GenerateOBJ("joystick control", "obj//Joystick_Control.obj");
	meshList[GEO_ENV_JOYSTICK_CONTROLLER]->textureID = LoadTGA("Image//Joystick_Control.tga");
	meshList[GEO_ENV_JOYSTICK_CONTROLLER]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_JOYSTICK_CONTROLLER]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_JOYSTICK_CONTROLLER]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_JOYSTICK_CONTROLLER]->material.kShininess = 0.6f;

	obj_transform[ENV_JOYSTICK_CONTROLLER].translation = Vector3(2, -14, 0);
	obj_transform[ENV_JOYSTICK_CONTROLLER].rotationX.angle = 0;
	obj_transform[ENV_JOYSTICK_CONTROLLER].rotationY.angle = 0;
	obj_transform[ENV_JOYSTICK_CONTROLLER].rotationZ.angle = 0;
	obj_transform[ENV_JOYSTICK_CONTROLLER].scaling = Vector3(0.8f, 0.8f, 0.8f);
	
	meshList[GEO_ENV_ARCADE_MACHINE_B] = MeshBuilder::GenerateOBJ("arcade machine blue", "obj//Arcade Machine.obj");
	meshList[GEO_ENV_ARCADE_MACHINE_B]->textureID = LoadTGA("Image//Arcade Machine Blue.tga");
	//objects[ENV_ARCADE_MACHINE_B] = new CRectangle();
	meshList[GEO_ENV_ARCADE_MACHINE_B]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_ARCADE_MACHINE_B]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_ARCADE_MACHINE_B]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_ARCADE_MACHINE_B]->material.kShininess = 0.6f;

	obj_transform[ENV_ARCADE_MACHINE_B].translation = Vector3(-20.f, 0.f, -23);
	obj_transform[ENV_ARCADE_MACHINE_B].rotationX.angle = 0;
	obj_transform[ENV_ARCADE_MACHINE_B].rotationY.angle = 0;
	obj_transform[ENV_ARCADE_MACHINE_B].rotationZ.angle = 0;
	obj_transform[ENV_ARCADE_MACHINE_B].scaling = Vector3(2.f, 2.f, 2.f);
	
	meshList[GEO_ENV_ARCADE_MACHINE_G] = MeshBuilder::GenerateOBJ("arcade machine green", "obj//Arcade Machine.obj");
	meshList[GEO_ENV_ARCADE_MACHINE_G]->textureID = LoadTGA("Image//Arcade Machine Green.tga");
	//objects[ENV_ARCADE_MACHINE_G] = new CRectangle();
	meshList[GEO_ENV_ARCADE_MACHINE_G]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_ARCADE_MACHINE_G]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_ARCADE_MACHINE_G]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_ARCADE_MACHINE_G]->material.kShininess = 0.6f;

	obj_transform[ENV_ARCADE_MACHINE_G].translation = Vector3(-15.f, 0.f, -23.f);
	obj_transform[ENV_ARCADE_MACHINE_G].rotationX.angle = 0;
	obj_transform[ENV_ARCADE_MACHINE_G].rotationY.angle = 0;
	obj_transform[ENV_ARCADE_MACHINE_G].rotationZ.angle = 0;
	obj_transform[ENV_ARCADE_MACHINE_G].scaling = Vector3(2.f, 2.f, 2.f);
	
	meshList[GEO_ENV_ARCADE_MACHINE_P] = MeshBuilder::GenerateOBJ("arcade machine purple", "obj//Arcade Machine.obj");
	meshList[GEO_ENV_ARCADE_MACHINE_P]->textureID = LoadTGA("Image//Arcade Machine Purple.tga");
	//objects[ENV_ARCADE_MACHINE_P] = new CRectangle();
	meshList[GEO_ENV_ARCADE_MACHINE_P]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_ARCADE_MACHINE_P]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_ARCADE_MACHINE_P]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_ARCADE_MACHINE_P]->material.kShininess = 0.6f;

	obj_transform[ENV_ARCADE_MACHINE_P].translation = Vector3(-10.f, 0.f, -23.f);
	obj_transform[ENV_ARCADE_MACHINE_P].rotationX.angle = 0;
	obj_transform[ENV_ARCADE_MACHINE_P].rotationY.angle = 0;
	obj_transform[ENV_ARCADE_MACHINE_P].rotationZ.angle = 0;
	obj_transform[ENV_ARCADE_MACHINE_P].scaling = Vector3(2.f, 2.f, 2.f);

	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_1] = MeshBuilder::GenerateOBJ("car platform 1", "obj//Car Stand Display.obj");
	//meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_1]->textureID = LoadTGA("Image//Car Stand Platform.tga");
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_1]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_1]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_1]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_1]->material.kShininess = 0.6f;

	obj_transform[ENV_CAR_DISPLAY_PLATFORM_1].translation = Vector3(-28, 0, -6);
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_1].rotationX.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_1].rotationY.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_1].rotationZ.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_1].scaling = Vector3(3.f, 3.f, 3.f);

	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_2] = MeshBuilder::GenerateOBJ("car platform 2", "obj//Car Stand Display.obj");
	//meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_2]->textureID = LoadTGA("Image//Car Stand Platform.tga");
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_2]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_2]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_2]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_2]->material.kShininess = 0.6f;

	obj_transform[ENV_CAR_DISPLAY_PLATFORM_2].translation = Vector3(-28, 0, 12);
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_2].rotationX.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_2].rotationY.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_2].rotationZ.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_2].scaling = Vector3(3.f, 3.f, 3.f);

	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_3] = MeshBuilder::GenerateOBJ("car platform 3", "obj//Car Stand Display.obj");
	//meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_3]->textureID = LoadTGA("Image//Car Stand Platform.tga");
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_3]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_3]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_3]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_3]->material.kShininess = 0.6f;

	obj_transform[ENV_CAR_DISPLAY_PLATFORM_3].translation = Vector3(28, 0, 17);
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_3].rotationX.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_3].rotationY.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_3].rotationZ.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_3].scaling = Vector3(3.f, 3.f, 3.f);

	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_4] = MeshBuilder::GenerateOBJ("car platform 4", "obj//Car Stand Display.obj");
	//meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_4]->textureID = LoadTGA("Image//Car Stand Platform.tga");
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_4]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_4]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_4]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_4]->material.kShininess = 0.6f;

	obj_transform[ENV_CAR_DISPLAY_PLATFORM_4].translation = Vector3(28, 0, 1);
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_4].rotationX.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_4].rotationY.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_4].rotationZ.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_4].scaling = Vector3(3.f, 3.f, 3.f);

	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_5] = MeshBuilder::GenerateOBJ("car platform 5", "obj//Car Stand Display.obj");
	//meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_5]->textureID = LoadTGA("Image//Car Stand Platform.tga");
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_5]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_5]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_5]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_5]->material.kShininess = 0.6f;

	obj_transform[ENV_CAR_DISPLAY_PLATFORM_5].translation = Vector3(28, 0, -16);
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_5].rotationX.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_5].rotationY.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_5].rotationZ.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_5].scaling = Vector3(3.f, 3.f, 3.f);

	meshList[GEO_ENV_COFFEE_MACHINE] = MeshBuilder::GenerateOBJ("coffee machine", "obj//Coffee Machine.obj");
	//meshList[GEO_ENV_COFFEE_MACHINE]->textureID = LoadTGA("Image//Coffee Machine.tga");
	meshList[GEO_ENV_COFFEE_MACHINE]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_COFFEE_MACHINE]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_COFFEE_MACHINE]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_COFFEE_MACHINE]->material.kShininess = 0.6f;

	obj_transform[ENV_COFFEE_MACHINE].translation = Vector3(2, -14, 0);
	obj_transform[ENV_COFFEE_MACHINE].rotationX.angle = 0;
	obj_transform[ENV_COFFEE_MACHINE].rotationY.angle = 0;
	obj_transform[ENV_COFFEE_MACHINE].rotationZ.angle = 0;
	obj_transform[ENV_COFFEE_MACHINE].scaling = Vector3(0.8f, 0.8f, 0.8f);

	meshList[GEO_ENV_COFFEE_CUP] = MeshBuilder::GenerateOBJ("coffee cup", "obj//Coffee Cup.obj");
	//meshList[GEO_ENV_COFFEE_CUP]->textureID = LoadTGA("Image//Coffee Cup_Silver.tga");
	meshList[GEO_ENV_COFFEE_CUP]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_COFFEE_CUP]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_COFFEE_CUP]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_COFFEE_CUP]->material.kShininess = 0.6f;

	obj_transform[ENV_COFFEE_CUP].translation = Vector3(2, -14, 0);
	obj_transform[ENV_COFFEE_CUP].rotationX.angle = 0;
	obj_transform[ENV_COFFEE_CUP].rotationY.angle = 0;
	obj_transform[ENV_COFFEE_CUP].rotationZ.angle = 0;
	obj_transform[ENV_COFFEE_CUP].scaling = Vector3(0.8f, 0.8f, 0.8f);

	meshList[GEO_ENV_SIGNPOST_1] = MeshBuilder::GenerateOBJ("signpost1", "obj//Signpost.obj");
	meshList[GEO_ENV_SIGNPOST_1]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_SIGNPOST_1]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_SIGNPOST_1]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_SIGNPOST_1]->material.kShininess = 0.6f;

	obj_transform[ENV_SIGNPOST_1].translation = Vector3(-20, 0, -10);
	obj_transform[ENV_SIGNPOST_1].rotationX.angle = 0;
	obj_transform[ENV_SIGNPOST_1].rotationY.angle = 0;
	obj_transform[ENV_SIGNPOST_1].rotationZ.angle = 0;
	obj_transform[ENV_SIGNPOST_1].scaling = Vector3(2.f, 2.f, 2.f);

	meshList[GEO_ENV_SIGNPOST_2] = MeshBuilder::GenerateOBJ("signpost2", "obj//Signpost.obj");
	meshList[GEO_ENV_SIGNPOST_2]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_SIGNPOST_2]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_SIGNPOST_2]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_SIGNPOST_2]->material.kShininess = 0.6f;

	obj_transform[ENV_SIGNPOST_2].translation = Vector3(-20, 0, 8.f);
	obj_transform[ENV_SIGNPOST_2].rotationX.angle = 0;
	obj_transform[ENV_SIGNPOST_2].rotationY.angle = 0;
	obj_transform[ENV_SIGNPOST_2].rotationZ.angle = 0;
	obj_transform[ENV_SIGNPOST_2].scaling = Vector3(2.f, 2.f, 2.f);

	meshList[GEO_ENV_SIGNPOST_3] = MeshBuilder::GenerateOBJ("signpost3", "obj//Signpost.obj");
	meshList[GEO_ENV_SIGNPOST_3]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_SIGNPOST_3]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_SIGNPOST_3]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_SIGNPOST_3]->material.kShininess = 0.6f;

	obj_transform[ENV_SIGNPOST_3].translation = Vector3(20, 0, 20.f);
	obj_transform[ENV_SIGNPOST_3].rotationX.angle = 0;
	obj_transform[ENV_SIGNPOST_3].rotationY.angle = 180;
	obj_transform[ENV_SIGNPOST_3].rotationZ.angle = 0;
	obj_transform[ENV_SIGNPOST_3].scaling = Vector3(2.f, 2.f, 2.f);

	meshList[GEO_ENV_SIGNPOST_4] = MeshBuilder::GenerateOBJ("signpost4", "obj//Signpost.obj");
	meshList[GEO_ENV_SIGNPOST_4]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_SIGNPOST_4]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_SIGNPOST_4]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_SIGNPOST_4]->material.kShininess = 0.6f;

	obj_transform[ENV_SIGNPOST_4].translation = Vector3(20, 0, 5.f);
	obj_transform[ENV_SIGNPOST_4].rotationX.angle = 0;
	obj_transform[ENV_SIGNPOST_4].rotationY.angle = 180;
	obj_transform[ENV_SIGNPOST_4].rotationZ.angle = 0;
	obj_transform[ENV_SIGNPOST_4].scaling = Vector3(2.f, 2.f, 2.f);

	meshList[GEO_ENV_SIGNPOST_5] = MeshBuilder::GenerateOBJ("signpost5", "obj//Signpost.obj");
	meshList[GEO_ENV_SIGNPOST_5]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_SIGNPOST_5]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_SIGNPOST_5]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_SIGNPOST_5]->material.kShininess = 0.6f;

	obj_transform[ENV_SIGNPOST_5].translation = Vector3(20, 0, -10.f);
	obj_transform[ENV_SIGNPOST_5].rotationX.angle = 0;
	obj_transform[ENV_SIGNPOST_5].rotationY.angle = 180;
	obj_transform[ENV_SIGNPOST_5].rotationZ.angle = 0;
	obj_transform[ENV_SIGNPOST_5].scaling = Vector3(2.f, 2.f, 2.f);


	// init car for  display (first)
	meshList[GEO_ENV_CAR] = MeshBuilder::GenerateOBJ("car1", "obj//car1.obj");
	meshList[GEO_ENV_CAR]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_CAR]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_CAR]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_CAR]->material.kShininess = 0.6f;

	obj_transform[ENV_CAR].translation = Vector3(0, 1, 0);
	obj_transform[ENV_CAR].rotationX.angle = 0;
	obj_transform[ENV_CAR].rotationY.angle = 0;
	obj_transform[ENV_CAR].rotationZ.angle = 0;

	// doesnt work yet ==========
	if (nearCar) {
		obj_transform[ENV_CAR].scaling = Vector3(.3f, .3f, .3f);
	}
	else {
		obj_transform[ENV_CAR].scaling = Vector3(.1f, .1f, .1f);
	}
	// end ============ 
}

void SceneText::RenderMesh(Mesh* mesh, bool enableLight)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);


	if (enableLight)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose.a[0]);

		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}

	if(mesh->textureID > 0){ 
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);} 
	else { 
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	} 
	mesh->Render(); //this line should only be called once in the whole function

	if(mesh->textureID > 0) glBindTexture(GL_TEXTURE_2D, 0);
}

void SceneText::RenderObject(Mesh* mesh, transform object, bool hierarchical, bool enableLight)
{
	modelStack.PushMatrix();
	modelStack.Translate(object.translation);
	modelStack.Rotate(object.rotationX);
	modelStack.Rotate(object.rotationY);
	modelStack.Rotate(object.rotationZ);
	modelStack.Scale(object.scaling);

	Mtx44 MVP, modelView, modelView_inverse_transpose;

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);


	if (enableLight)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose.a[0]);

		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}

	if (mesh->textureID > 0) {
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	}
	else {
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}
	mesh->Render(); //this line should only be called once in the whole function

	if (mesh->textureID > 0) glBindTexture(GL_TEXTURE_2D, 0);

	if (!hierarchical) {
		modelStack.PopMatrix();
	}
}

//void SceneText::RenderObjectHierarchial(Mesh* mesh, transform object, bool enableLight)
//{
//	modelStack.PushMatrix();
//	modelStack.Translate(object.translation);
//	modelStack.Rotate(object.rotationX);
//	modelStack.Rotate(object.rotationY);
//	modelStack.Rotate(object.rotationZ);
//	modelStack.Scale(object.scaling);
//
//	Mtx44 MVP, modelView, modelView_inverse_transpose;
//
//	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
//	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
//
//	modelView = viewStack.Top() * modelStack.Top();
//	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
//
//
//	if (enableLight)
//	{
//		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
//		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
//		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose.a[0]);
//
//		//load material
//		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
//		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
//		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
//		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
//	}
//	else
//	{
//		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
//	}
//
//	if (mesh->textureID > 0) {
//		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
//		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
//	}
//	else {
//		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
//	}
//	mesh->Render(); //this line should only be called once in the whole function
//
//	if (mesh->textureID > 0) glBindTexture(GL_TEXTURE_2D, 0);
//
//}

void SceneText::RenderSkybox()
{
	modelStack.PushMatrix();
		skybox_transform[0].translation = Vector3(-37.5f, 12.5f, 0);
		skybox_transform[0].scaling = Vector3(50.f, 25.f, 50.f);
		skybox_transform[0].rotationY.angle = 90;
		RenderObject(meshList[GEO_LEFT], skybox_transform[0], false, false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
		skybox_transform[1].translation = Vector3(37.5f, 12.5f, 0);
		skybox_transform[1].scaling = Vector3(50.f, 25.f, 50.f);
		skybox_transform[1].rotationY.angle = -90;
		RenderObject(meshList[GEO_RIGHT], skybox_transform[1], false, false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
		skybox_transform[2].translation = Vector3(0, 25.f, 0);
		skybox_transform[2].scaling = Vector3(50.f, 75.f, 50.f);
		skybox_transform[2].rotationX.angle = 90;
		skybox_transform[2].rotationZ.angle = 90;
		RenderObject(meshList[GEO_TOP], skybox_transform[2], false, false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
		//skybox_transform[3].translation = Vector3(0, 0, 0);
		skybox_transform[3].rotationX.angle = -90;
		skybox_transform[3].rotationZ.angle = 90;
		skybox_transform[3].scaling = Vector3(50.f, 75.f, 50.f);
		RenderObject(meshList[GEO_BOTTOM], skybox_transform[3], false, false);
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
		skybox_transform[4].translation = Vector3(0, 12.5f, -25.f);
		skybox_transform[4].scaling = Vector3(75.f, 25.f, 50.f);
		RenderObject(meshList[GEO_FRONT], skybox_transform[4], false, false);
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
		skybox_transform[5].translation = Vector3(0, 12.5f, 25.f);
		skybox_transform[5].scaling = Vector3(75.f, 25.f, 50.f);
		skybox_transform[5].rotationY.angle = 180;
		RenderObject(meshList[GEO_BACK], skybox_transform[5], false, false);
	modelStack.PopMatrix();
}

void SceneText::RenderText(Mesh* mesh, std::string text, Color color)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;

	glDisable(GL_DEPTH_TEST);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glEnable(GL_DEPTH_TEST);

}

void SceneText::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;

	glDisable(GL_DEPTH_TEST);

	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Scale(size, size, size);
	modelStack.Translate(x, y, 0);

	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();

	glEnable(GL_DEPTH_TEST);
}

std::string SceneText::print_fps()
{
	static float framesPerSecond = 0.0f;
	static int fps;
	static float lastTime = 0.0f;
	float currentTime = GetTickCount() * 0.001f;
	++framesPerSecond;
	std::string output = "";
	if (currentTime - lastTime > 1.f) {
		lastTime = currentTime;
		fps = (int)framesPerSecond;
		framesPerSecond = 0;
	}
	return std::to_string(fps);
}