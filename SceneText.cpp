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
#define LIGHT_NUMBER 5

int chosenCar;
float SceneText::LightChange = 0.5f;

SceneText::SceneText()
{
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = NULL;
	}
	srand(time(NULL));
}

SceneText::~SceneText()
{
}

void SceneText::Init()
{
	cameraMode = 1;
	gamer.setRadius(1);
	walkingX = 5;
	walkingZ = 5;
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	//sphere = new CCollision;
	//cube = new CRectangle;

	for (int i = 0; i < 5; ++i) {
		carDisplay[i] = new CCollision;
	}
	for (int i = 0; i < 3; ++i)
	{
		arcadeMachine[i] = new CRectangle;
	}
	for (int i = 0; i < 4; ++i)
	{
		skybox[i] = new CRectangle;
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

	// init objects
	InitObjs();

	//INIT CLOCK
	firstBestTime = true;

	meshList[GEO_CHAR] = MeshBuilder::GenerateQuad("char", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_CHAR]->textureID = LoadTGA("Image//char.tga");

	meshList[GEO_LIGHTSPHERE] = MeshBuilder::GenerateSphere("lightBall", Color(1.f, 1.f, 1.f), 9, 36, 1.f);

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//calibri.tga");

	meshList[GEO_TETIME] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TETIME]->textureID = LoadTGA("Image//calibri.tga");

	meshList[GEO_DICE] = MeshBuilder::GenerateSphere("sphere", Color(1.f, 0, 0), 9, 36, 1.f);

	meshList[GEO_CUBE] = MeshBuilder::GenerateCuboid("cuboid", Color(1.f, 0, 0), 1, 1, 1);

	MainMenu = true;
	PauseMenu = false;

	coords.X = 0;
	coords.Y = 18;

	ElapsedTime2 = 0.0f; 
	BounceTime = 0.0f; 
}

void SceneText::Update(double dt)
{
	//Pause Menu Key
	if (Application::IsKeyPressed('L'/*'P'*/)) //Change to P when you remove the debug, thanks!
		PauseMenu = true;

	ElapsedTime2 += dt; 
	if (MainMenu == true || PauseMenu == true) // when either Menu's bool is true
	{
		if (BounceTime > ElapsedTime2) // prevents Update() from running every frame if a key has been pressed
			return;

		if (Application::IsKeyPressed('W')) // to move cursor upwards
		{
			coords.Y++;
			if (coords.Y > 18)
				coords.Y = 18;
			BounceTime = ElapsedTime2 + 0.125;
		}

		if (Application::IsKeyPressed('S')) // to move cursor downwards
		{
			coords.Y--;
			if (coords.Y < 17)
				coords.Y = 17;
			BounceTime = ElapsedTime2 + 0.125;
		}

		SceneText::M_Menu();
		SceneText::P_Menu();
		SceneText::ExitGame();
	}

	else
	{
	/*sphere->set_transformation('t', Vector3(spheree.translation.x, spheree.translation.y, spheree.translation.z));
	sphere->roundCollision(gamer, playerPos, 1);
	cube->set_transformation('t', Vector3(8, 0, 5));
	static_cast<CRectangle*>(cube)->RectCollision(gamer, playerPos, 6.8f, 4.f, 2.3f, 2.f);*/

	//collisions
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

	static_cast<CRectangle*>(arcadeMachine[0])->RectCollision(gamer, playerPos, -21.8f, -24.f, 4.f, 3.f);
	static_cast<CRectangle*>(arcadeMachine[1])->RectCollision(gamer, playerPos, -17.f, -24.f, 4.f, 3.f);
	static_cast<CRectangle*>(arcadeMachine[2])->RectCollision(gamer, playerPos, -12.2f, -24.f, 4.f, 3.f);
	static_cast<CRectangle*>(skybox[0])->RectCollision(gamer, playerPos, -38.f, -25.f, 1.f, 50.f); //left
	static_cast<CRectangle*>(skybox[1])->RectCollision(gamer, playerPos, 37.5f, -25.f, 1.f, 50.f);//right
	static_cast<CRectangle*>(skybox[2])->RectCollision(gamer, playerPos, -37.5f, -25, 75.f, 1);//front
	static_cast<CRectangle*>(skybox[3])->RectCollision(gamer, playerPos, -37.5f, 25, 75.f, 1);//back
	//

	CCollision* objects[12] = { carDisplay[0], carDisplay[1], carDisplay[2], carDisplay[3], carDisplay[4],
		arcadeMachine[0], arcadeMachine[1], arcadeMachine[2], skybox[0], skybox[1], skybox[2], skybox[3] };

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

	if (Application::IsKeyPressed(VK_SPACE))
	{
		if (cameraMode != 1)
			cameraMode = 1;
	}
	if (Application::IsKeyPressed(VK_F3))
	{
		if (cameraMode != 2)
		{
			cameraMode = 2;
		}
	}

	if (Application::IsKeyPressed('V'))
		walkingX -= (float)(20 * dt);
	if (Application::IsKeyPressed('N'))
		walkingX += (float)(20 * dt);
	if (Application::IsKeyPressed('G'))
		walkingZ -= (float)(20 * dt);
	if (Application::IsKeyPressed('B'))
		walkingZ += (float)(20 * dt);

	for (int i = 0; i < 12; i++)
	{
		if (objects[i]->getCollide() == true) {
			if (Application::IsKeyPressed('V'))
			{
				walkingX += objects[i]->getOverlap() * (playerPos.translation.x - objects[i]->get_transformation().translation.x) / objects[i]->getDistance();
				walkingZ += objects[i]->getOverlap() * (playerPos.translation.z - objects[i]->get_transformation().translation.z) / objects[i]->getDistance();
			}
			if (Application::IsKeyPressed('N'))
			{
				walkingX += objects[i]->getOverlap() * (playerPos.translation.x - objects[i]->get_transformation().translation.x) / objects[i]->getDistance();
				walkingZ += objects[i]->getOverlap() * (playerPos.translation.z - objects[i]->get_transformation().translation.z) / objects[i]->getDistance();

			}
			if (Application::IsKeyPressed('G'))
			{
				walkingX += objects[i]->getOverlap() * (playerPos.translation.x - objects[i]->get_transformation().translation.x) / objects[i]->getDistance();
				walkingZ += objects[i]->getOverlap() * (playerPos.translation.z - objects[i]->get_transformation().translation.z) / objects[i]->getDistance();

			}
			if (Application::IsKeyPressed('B'))
			{
				walkingX += objects[i]->getOverlap() * (playerPos.translation.x - objects[i]->get_transformation().translation.x) / objects[i]->getDistance();
				walkingZ += objects[i]->getOverlap() * (playerPos.translation.z - objects[i]->get_transformation().translation.z) / objects[i]->getDistance();
			}
		}
	}
	if (cameraMode == 1) {
		camera.Update(dt);
	}
	else if (cameraMode == 2) {
		camera.Update2(playerPos, dt);
	}

	ClosestNPC = -1;
	for (int i = 0; i < NUM_NPC; i++) {
		if ((NPCs_transform[i].translation - playerPos.translation).Length() < 5.f) {
			if ((NPCs_transform[i].translation - playerPos.translation).Length() < (NPCs_transform[ClosestNPC].translation - playerPos.translation).Length()) {
				ClosestNPC = i;
			}
			//NPCinRange = true;
		}
		else {
			//NPCinRange = false;
		}
	}

	if (Application::IsKeyPressed('M')) { // changed to interaction key
		M_pressed = true;
	}
	else {
		M_pressed = false;
	}

	for (int i = 0; i < 5; ++i)
	{
		if ((obj_transform[ENV_CAR_DISPLAY_PLATFORM_1 + i].translation - playerPos.translation).Length() < 7.5f) {

			if (obj_transform[ENV_CAR_KEN + i].scaling.x < .27f)
				obj_transform[ENV_CAR_KEN + i].scaling += Vector3(.5f * dt, .5f * dt, .5f * dt);

			obj_transform[ENV_CAR_KEN + i].rotationY.angle += (float)(10 * dt);

			if (obj_transform[ENV_CAR_KEN + i].translation.y < 2)
				obj_transform[ENV_CAR_KEN + i].translation.y += (float)(5 * dt);

			if (M_pressed) // not so useless code
				chosenCar = ENV_CAR_KEN + i;
		}
		else {
			if (obj_transform[ENV_CAR_KEN + i].scaling.x > .1f)
				obj_transform[ENV_CAR_KEN + i].scaling -= Vector3(dt, dt, dt);

			if (obj_transform[ENV_CAR_KEN + i].translation.y > 1)
				obj_transform[ENV_CAR_KEN + i].translation.y -= (float)(5 * dt);

		}
	}

	// animation:
	// ANIMATION 1: WALK======================
	if (Application::IsKeyPressed(VK_NUMPAD2))
	{
		AnimationReset();
		retractArmA1 = false;
		retractArmA1 = false;
		stopA1 = false;
		A1Lcount = 0;
		A1Acount = 0;

		animateTalking = false;
		animateIdle = false;
		animateArcade = false;
		animateCoffee = false;
		animateWalk = true;
	}
	if (animateWalk == true && animateTalking == false
		&& animateCoffee == false && animateIdle == false
		&& animateArcade == false)
	{
		playAnimationWalk(dt);
	}
	//========================================
	// ANIMATION 2: COFFEE====================
	if (Application::IsKeyPressed(VK_NUMPAD3))
	{
		AnimationReset();
		renderCup = false;
		retractArmA2 = false;
		stopA2 = false;

		animateWalk = false;
		animateTalking = false;
		animateIdle = false;
		animateArcade = false;
		animateCoffee = true;
	}
	if (animateCoffee == true && animateTalking == false
		&& animateIdle == false && animateArcade == false
		&& animateWalk == false)
	{
		playAnimationCoffee(dt);
	}
	//=========================================

	// ANIMATION 3: TALK=======================
	if (Application::IsKeyPressed(VK_NUMPAD4))
	{
		AnimationReset();
		A3count = 0;
		armUpA3 = true;
		stopA3 = false;

		animateWalk = false;
		animateCoffee = false;
		animateIdle = false;
		animateArcade = false;
		animateTalking = true;
	}
	if (animateTalking == true && animateCoffee == false
		&& animateIdle == false && animateArcade == false
		&& animateWalk == false)
	{
		playAnimationTalking(dt);
	}
	//=========================================

	// ANIMATION 4: IDLE=======================
	if (Application::IsKeyPressed(VK_NUMPAD5))
	{
		AnimationReset();
		animateIdle = false;
		armUpA4 = true;
		stopA4 = false;

		animateWalk = false;
		animateCoffee = false;
		animateTalking = false;
		animateArcade = false;
		animateIdle = true;
	}
	if (animateIdle == true && animateTalking == false
		&& animateCoffee == false && animateArcade == false
		&& animateWalk == false)
	{
		playAnimationIdle(dt);
	}
	//==========================================

	// ANIMATION 5: PLAY GAME===================
	if (Application::IsKeyPressed(VK_NUMPAD6))
	{
		AnimationReset();
		stopA5 = false;

		animateWalk = false;
		animateCoffee = false;
		animateTalking = false;
		animateIdle = false;
		animateArcade = true;
	}
	if (animateArcade == true && animateIdle == false
		&& animateTalking == false && animateCoffee == false
		&& animateWalk == false)
	{
		playAnimationArcade(dt);
	}
	//==========================================

	//TIMER=====================================
	if (Application::IsKeyPressed(VK_NUMPAD8))
	{
		eTimeStart = true;
	}

	if (Application::IsKeyPressed(VK_NUMPAD9))
	{
		if (eTimeStart == true)
		{
			if (lapcount < 2)
			{
				LapTimes[lapcount] = ElapsedTime;
				++lapcount;
			}
			else if (lapcount == 2)
			{
				LapTimes[lapcount] = ElapsedTime;

				// LOGIC FOR BEST TIME
				if (firstBestTime == true)
				{
					BestTime = LapTimes[lapcount];
					firstBestTime = false;
				}
				else
				{
					if (LapTimes[lapcount] <= BestTime)
					{
						BestTime = LapTimes[lapcount];
					}
					else
					{
						BestTime = BestTime;
					}
				}

				displayBestTime = true;

				//RESET LOGIC
				eTimeStart = false;

				lapcount = 0;
				ElapsedTime = 0;

				for (int i = 0; i < 3; ++i)
				{
					LapTimes[i] = NULL;
				}
			}
		}
	}


	// TIMER START AND STOP
	if (eTimeStart == true)
	{
		ElapsedTime = ElapsedTime += dt;
	}
	else
	{
		ElapsedTime = ElapsedTime;
	}
	//============================================
	// NPC behaviour:
	for (int i = 0; i < NUM_NPC; ++i) {
		std::cout << NPCs[i].get_activity() << std::endl;
		if (NPCs[i].get_activity() == "chilling") {
			static float NPC_time_wait = 0;
			NPC_time_wait += (float)(dt * 10);
			if (NPC_time_wait > 10) {
				NPC_time_wait = 0;
				NPCs[i].set_idle();
			}
		}
		else if (NPCs[i].get_activity() == "walking") {
			Vector3 distance = NPCs[i].get_walk() - NPCs_transform[i].translation;
			if (distance.Length() > 0) {
				float angle = Math::RadianToDegree(acos(distance.Normalized().Dot(Vector3(1, 0, 0))));
				if (distance.z > 0) {
					angle = -angle;
				}
				if ((NPCs_transform[i].rotationY.angle < angle + 1) && (NPCs_transform[i].rotationY.angle > angle - 1)) {
					correct_angle = true;
				}
				else if (NPCs_transform[i].rotationY.angle < angle + 1) {
					NPCs_transform[i].rotationY.angle += (float)(dt * 50);
				}
				else if (NPCs_transform[i].rotationY.angle > angle - 1) {
					NPCs_transform[i].rotationY.angle -= (float)(dt * 50);
				}
				if (NPCs_transform[i].translation != NPCs[i].get_walk() && correct_angle == true) {
					playAnimationWalk(dt);
					NPCs_transform[i].translation += distance.Normalized() * (float)(10 * dt);
					if (distance.Length() < 1) {
						NPCs_transform[i].translation = NPCs[i].get_walk();
						correct_angle = false;
					}
				}
			}
			else {
				static float NPC_time_wait = 0;
				NPC_time_wait += (float)(dt * 10);
				if (NPC_time_wait > 10) {
					NPC_time_wait = 0;
					NPCs[i].set_idle();
				}
			}
		}
		else if (NPCs[i].get_activity() == "getting coffee") {
			// go to coffee machine
			Vector3 distance = NPCs[i].get_walk() - NPCs_transform[i].translation;
			if (distance.Length() > 0) {
				float angle = Math::RadianToDegree(acos(distance.Normalized().Dot(Vector3(1, 0, 0))));
				if (distance.z > 0) {
					angle = -angle;
				}
				if ((NPCs_transform[i].rotationY.angle < angle + 1) && (NPCs_transform[i].rotationY.angle > angle - 1)) {
					correct_angle = true;
				}
				else if (NPCs_transform[i].rotationY.angle < angle + 1) {
					NPCs_transform[i].rotationY.angle += (float)(dt * 50);
				}
				else if (NPCs_transform[i].rotationY.angle > angle - 1) {
					NPCs_transform[i].rotationY.angle -= (float)(dt * 50);
				}
				if (NPCs_transform[i].translation != NPCs[i].get_walk() && correct_angle == true) {
					NPCs_transform[i].translation += distance.Normalized() * (float)(10 * dt);
					if (distance.Length() < 1) {
						NPCs_transform[i].translation = NPCs[i].get_walk();
						correct_angle = false;
					}
				}
			}
			else {
				static float NPC_time_wait = 0;
				NPC_time_wait += (float)(dt * 10);
				if (NPC_time_wait > 10) {
					NPC_time_wait = 0;
					NPCs[i].set_idle();
				}
			}
		}
		else if (NPCs[i].get_activity() == "gaming") {
			// to go arcade cabinet
			bool close_to_machine = false;
			Vector3 distance = NPCs[i].get_walk() - NPCs_transform[i].translation;
			if (distance.Length() > 0) {
				float angle = Math::RadianToDegree(acos(distance.Normalized().Dot(Vector3(1, 0, 0))));
				if (distance.z > 0) {
					angle = -angle;
				}
				if ((NPCs_transform[i].rotationY.angle < angle + 1) && (NPCs_transform[i].rotationY.angle > angle - 1)) {
					correct_angle = true;
				}
				else if (NPCs_transform[i].rotationY.angle < angle + 1) {
					NPCs_transform[i].rotationY.angle += (float)(dt * 50);
				}
				else if (NPCs_transform[i].rotationY.angle > angle - 1) {
					NPCs_transform[i].rotationY.angle -= (float)(dt * 50);
				}
				if (NPCs_transform[i].translation != NPCs[i].get_walk() && correct_angle == true) {
					NPCs_transform[i].translation += distance.Normalized() * (float)(10 * dt);
					if (distance.Length() < 1) {
						NPCs_transform[i].translation = NPCs[i].get_walk();
						correct_angle = false;
					}
				}
			}
			else {
				static float NPC_time_wait = 0;
				NPC_time_wait += (float)(dt * 10);
				if (NPC_time_wait > 10) {
					NPC_time_wait = 0;
					NPCs[i].set_idle();
				}
			}
		}
	}

	if (LightChange == 0.5 && LightChange <= 1)
	{
		LightChange += 0.5 * dt;
	}
	else {
		LightChange = 0.5f;
	}
}
}

void SceneText::Render()
{
	//Clear color & depth buffer every frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viewStack.LoadIdentity();
	viewStack.LookAt(camera.position.x, camera.position.y, camera.position.z, camera.target.x, camera.target.y, camera.target.z, camera.up.x, camera.up.y, camera.up.z);
	modelStack.LoadIdentity();
	RenderLights();

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

	// blue machine
	RenderObject(meshList[GEO_ENV_ARCADE_MACHINE_B], obj_transform[ENV_ARCADE_MACHINE_B], true, false);
	RenderObject(meshList[GEO_ENV_JOYSTICK_BASE1], obj_transform[ENV_JOYSTICK_BASE1], true, false);
	RenderObject(meshList[GEO_ENV_JOYSTICK_CONTROLLER1], obj_transform[ENV_JOYSTICK_CONTROLLER1], false, false);
	modelStack.PopMatrix();
	RenderObject(meshList[GEO_ENV_ARCADE_BUTTON_EXT_B], obj_transform[ENV_ARCADE_BUTTON_EXT_B], true, false);
	RenderObject(meshList[GEO_ENV_ARCADE_BUTTON_INT_B], obj_transform[ENV_ARCADE_BUTTON_INT_B], false, false);
	modelStack.PopMatrix();
	modelStack.PopMatrix();

	// green machine
	RenderObject(meshList[GEO_ENV_ARCADE_MACHINE_G], obj_transform[ENV_ARCADE_MACHINE_G], true, false);
	int j = ENV_ARCADE_BUTTON_EXT1;
	for (int i = GEO_ENV_ARCADE_BUTTON_EXT1; i <= GEO_ENV_ARCADE_BUTTON_EXT9; i += 2) {
		RenderObject(meshList[i], obj_transform[j], true, false);
		RenderObject(meshList[i + 1], obj_transform[j + 1], false, false);
		modelStack.PopMatrix();
		j += 2;
	}
	modelStack.PopMatrix();

	RenderObject(meshList[GEO_ENV_ARCADE_MACHINE_P], obj_transform[ENV_ARCADE_MACHINE_P], false, true);
	RenderObject(meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_1], obj_transform[ENV_CAR_DISPLAY_PLATFORM_1], true, true);
	RenderObject(meshList[GEO_CAR_KEN], obj_transform[ENV_CAR_KEN], false, true);
	modelStack.PopMatrix();
	RenderObject(meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_2], obj_transform[ENV_CAR_DISPLAY_PLATFORM_2], true, true);
	RenderObject(meshList[GEO_CAR_JOSQUIN], obj_transform[ENV_CAR_JOSQUIN], false, true);
	modelStack.PopMatrix();
	RenderObject(meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_3], obj_transform[ENV_CAR_DISPLAY_PLATFORM_3], true, true);
	RenderObject(meshList[GEO_CAR_KLYDE], obj_transform[ENV_CAR_KLYDE], false, true);
	modelStack.PopMatrix();
	RenderObject(meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_4], obj_transform[ENV_CAR_DISPLAY_PLATFORM_4], true, true);
	RenderObject(meshList[GEO_CAR_SHAHIR], obj_transform[ENV_CAR_SHAHIR], false, true);
	modelStack.PopMatrix();
	RenderObject(meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_5], obj_transform[ENV_CAR_DISPLAY_PLATFORM_5], true, true);
	RenderObject(meshList[GEO_CAR_EMILY], obj_transform[ENV_CAR_EMILY], false, true);
	modelStack.PopMatrix();
	RenderObject(meshList[GEO_ENV_SIGNPOST_1], obj_transform[ENV_SIGNPOST_1], false, true);
	RenderObject(meshList[GEO_ENV_SIGNPOST_2], obj_transform[ENV_SIGNPOST_2], false, true);
	RenderObject(meshList[GEO_ENV_SIGNPOST_3], obj_transform[ENV_SIGNPOST_3], false, true);
	RenderObject(meshList[GEO_ENV_SIGNPOST_4], obj_transform[ENV_SIGNPOST_4], false, true);
	RenderObject(meshList[GEO_ENV_SIGNPOST_5], obj_transform[ENV_SIGNPOST_5], false, true);
	RenderObject(meshList[GEO_ENV_COFFEE_MACHINE], obj_transform[ENV_COFFEE_MACHINE], false, true);
	RenderObject(meshList[GEO_ENV_COFFEE_CUP], obj_transform[ENV_COFFEE_CUP], false, true);
	RenderObject(meshList[GEO_TELEPORTER], obj_transform[ENV_TELEPORTER], false, true);
	RenderObject(meshList[GEO_TELEPORTER2], obj_transform[ENV_TELEPORTER2], false, true);

	//NPC RENDER==========================================================================
	RenderObject(meshList[GEO_NPCBODY], NPCs_transform[T_NPCBODY], true, false);
	RenderObject(meshList[GEO_NPCHEAD], NPCs_transform[T_NPCHEAD], false, true);

	RenderObject(meshList[GEO_NPCUPPERARM], NPCs_transform[T_NPCRUPPERARM], true, true);
	RenderObject(meshList[GEO_NPCLOWERARM], NPCs_transform[T_NPCRLOWERARM], true, true);
	if (renderCup == false)
	{
		RenderObject(meshList[GEO_NPCCLAW], NPCs_transform[T_NPCRFCLAW], false, true);
	}
	else
	{
		RenderObject(meshList[GEO_NPCCLAW], NPCs_transform[T_NPCRFCLAW], true, true);
		RenderObject(meshList[GEO_ENV_COFFEE_CUP], obj_transform[ENV_COFFEE_CUP], false, true);
		modelStack.PopMatrix();
	}
	RenderObject(meshList[GEO_NPCCLAW], NPCs_transform[T_NPCRBCLAW], false, true);
	modelStack.PopMatrix();
	modelStack.PopMatrix();

	RenderObject(meshList[GEO_NPCUPPERARM], NPCs_transform[T_NPCLUPPERARM], true, true);
	RenderObject(meshList[GEO_NPCLOWERARM], NPCs_transform[T_NPCLLOWERARM], true, true);
	RenderObject(meshList[GEO_NPCCLAW], NPCs_transform[T_NPCLFCLAW], false, true);
	RenderObject(meshList[GEO_NPCCLAW], NPCs_transform[T_NPCLBCLAW], false, true);
	modelStack.PopMatrix();
	modelStack.PopMatrix();

	RenderObject(meshList[GEO_NPCUPPERLEG], NPCs_transform[T_NPCRUPPERLEG], true, true);
	RenderObject(meshList[GEO_NPCLOWERLEG], NPCs_transform[T_NPCRLOWERLEG], false, true);
	modelStack.PopMatrix();

	RenderObject(meshList[GEO_NPCUPPERLEG], NPCs_transform[T_NPCLUPPERLEG], true, true);
	RenderObject(meshList[GEO_NPCLOWERLEG], NPCs_transform[T_NPCLLOWERLEG], false, true);
	modelStack.PopMatrix();

	modelStack.PopMatrix();
	//====================================================================================

	//TIMER TEXT RENDER===================================================================
	RenderTextOnScreen(meshList[GEO_TETIME], "Elapsed Time: " + std::to_string(ElapsedTime), Color(1, 0, 1), 2.5, 0, 23);
	if (displayBestTime == true)
	{
		RenderTextOnScreen(meshList[GEO_TETIME], "Best Time: " + std::to_string(BestTime), Color(1, 0, 1), 2.5, 0, 21);
	}
	if (eTimeStart == true)
	{
		RenderTextOnScreen(meshList[GEO_TETIME], "Lap 1 Time: " + std::to_string(LapTimes[0]), Color(1, 0, 1), 2.5, 0, 20);
		RenderTextOnScreen(meshList[GEO_TETIME], "Lap 2 Time: " + std::to_string(LapTimes[1]), Color(1, 0, 1), 2.5, 0, 19);
	}
	//======================================================================================

	//No transform needed
	RenderTextOnScreen(meshList[GEO_TEXT], "Current FPS: " + print_fps(), Color(0, 1, 0), 2, 0, 18);
	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(chosenCar), Color(0, 1, 0), 2, 0, 2);

	modelStack.PushMatrix();
	if (M_pressed && ClosestNPC != -1) {

		NPCs[ClosestNPC].activity(true);

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

	if (MainMenu == true)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], ">", Color(0, 1, 0), 3, coords.X, coords.Y);
		RenderTextOnScreen(meshList[GEO_TEXT], "Start Game", Color(0, 1, 0), 3, 1, 18);
		RenderTextOnScreen(meshList[GEO_TEXT], "Exit", Color(0, 1, 0), 3, 1, 17);
	}

	if (PauseMenu == true)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], ">", Color(0, 1, 0), 3, coords.X, coords.Y);
		RenderTextOnScreen(meshList[GEO_TEXT], "Resume", Color(0, 1, 0), 3, 1, 18);
		RenderTextOnScreen(meshList[GEO_TEXT], "Exit", Color(0, 1, 0), 3, 1, 17);
	}
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
	// 0
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
	// 1
	m_parameters[U_LIGHT_CEILING1_POSITION] = glGetUniformLocation(m_programID, "lights[1].position_cameraspace");
	m_parameters[U_LIGHT_CEILING1_COLOR] = glGetUniformLocation(m_programID, "lights[1].color");
	m_parameters[U_LIGHT_CEILING1_POWER] = glGetUniformLocation(m_programID, "lights[1].power");
	m_parameters[U_LIGHT_CEILING1_KC] = glGetUniformLocation(m_programID, "lights[1].kC");
	m_parameters[U_LIGHT_CEILING1_KL] = glGetUniformLocation(m_programID, "lights[1].kL");
	m_parameters[U_LIGHT_CEILING1_KQ] = glGetUniformLocation(m_programID, "lights[1].kQ");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
	m_parameters[U_LIGHT_CEILING1_TYPE] = glGetUniformLocation(m_programID, "lights[1].type");
	m_parameters[U_LIGHT_CEILING1_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[1].spotDirection");
	m_parameters[U_LIGHT_CEILING1_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[1].cosCutoff");
	m_parameters[U_LIGHT_CEILING1_COSINNER] = glGetUniformLocation(m_programID, "lights[1].cosInner");
	m_parameters[U_LIGHT_CEILING1_EXPONENT] = glGetUniformLocation(m_programID, "lights[1].exponent");

	//2
	m_parameters[U_LIGHT_CEILING2_POSITION] = glGetUniformLocation(m_programID, "lights[2].position_cameraspace");
	m_parameters[U_LIGHT_CEILING2_COLOR] = glGetUniformLocation(m_programID, "lights[2].color");
	m_parameters[U_LIGHT_CEILING2_POWER] = glGetUniformLocation(m_programID, "lights[2].power");
	m_parameters[U_LIGHT_CEILING2_KC] = glGetUniformLocation(m_programID, "lights[2].kC");
	m_parameters[U_LIGHT_CEILING2_KL] = glGetUniformLocation(m_programID, "lights[2].kL");
	m_parameters[U_LIGHT_CEILING2_KQ] = glGetUniformLocation(m_programID, "lights[2].kQ");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
	m_parameters[U_LIGHT_CEILING2_TYPE] = glGetUniformLocation(m_programID, "lights[2].type");
	m_parameters[U_LIGHT_CEILING2_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[2].spotDirection");
	m_parameters[U_LIGHT_CEILING2_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[2].cosCutoff");
	m_parameters[U_LIGHT_CEILING2_COSINNER] = glGetUniformLocation(m_programID, "lights[2].cosInner");
	m_parameters[U_LIGHT_CEILING2_EXPONENT] = glGetUniformLocation(m_programID, "lights[2].exponent");

	//3
	m_parameters[U_LIGHT_CEILING3_POSITION] = glGetUniformLocation(m_programID, "lights[3].position_cameraspace");
	m_parameters[U_LIGHT_CEILING3_COLOR] = glGetUniformLocation(m_programID, "lights[3].color");
	m_parameters[U_LIGHT_CEILING3_POWER] = glGetUniformLocation(m_programID, "lights[3].power");
	m_parameters[U_LIGHT_CEILING3_KC] = glGetUniformLocation(m_programID, "lights[3].kC");
	m_parameters[U_LIGHT_CEILING3_KL] = glGetUniformLocation(m_programID, "lights[3].kL");
	m_parameters[U_LIGHT_CEILING3_KQ] = glGetUniformLocation(m_programID, "lights[3].kQ");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
	m_parameters[U_LIGHT_CEILING3_TYPE] = glGetUniformLocation(m_programID, "lights[3].type");
	m_parameters[U_LIGHT_CEILING3_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[3].spotDirection");
	m_parameters[U_LIGHT_CEILING3_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[3].cosCutoff");
	m_parameters[U_LIGHT_CEILING3_COSINNER] = glGetUniformLocation(m_programID, "lights[3].cosInner");
	m_parameters[U_LIGHT_CEILING3_EXPONENT] = glGetUniformLocation(m_programID, "lights[3].exponent");

	//4
	m_parameters[U_LIGHT_CEILING4_POSITION] = glGetUniformLocation(m_programID, "lights[4].position_cameraspace");
	m_parameters[U_LIGHT_CEILING4_COLOR] = glGetUniformLocation(m_programID, "lights[4].color");
	m_parameters[U_LIGHT_CEILING4_POWER] = glGetUniformLocation(m_programID, "lights[4].power");
	m_parameters[U_LIGHT_CEILING4_KC] = glGetUniformLocation(m_programID, "lights[4].kC");
	m_parameters[U_LIGHT_CEILING4_KL] = glGetUniformLocation(m_programID, "lights[4].kL");
	m_parameters[U_LIGHT_CEILING4_KQ] = glGetUniformLocation(m_programID, "lights[4].kQ");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
	m_parameters[U_LIGHT_CEILING4_TYPE] = glGetUniformLocation(m_programID, "lights[4].type");
	m_parameters[U_LIGHT_CEILING4_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[4].spotDirection");
	m_parameters[U_LIGHT_CEILING4_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[4].cosCutoff");
	m_parameters[U_LIGHT_CEILING4_COSINNER] = glGetUniformLocation(m_programID, "lights[4].cosInner");
	m_parameters[U_LIGHT_CEILING4_EXPONENT] = glGetUniformLocation(m_programID, "lights[4].exponent");

	// 0
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
	// 0
	light[1].type = Light::LIGHT_POINT;
	light[1].position.Set(20, 5, 0);
	light[1].color.Set(LightChange, 0.f, 0.f);
	light[1].power = 1;
	light[1].kC = 1.f;
	light[1].kL = 0.01f;
	light[1].kQ = 0.001f;
	light[1].cosCutoff = cos(Math::DegreeToRadian(45));
	light[1].cosInner = cos(Math::DegreeToRadian(30));
	light[1].exponent = 3.f;
	light[1].spotDirection.Set(0.f, 1.f, 0.f);
	// 0
	light[2].type = Light::LIGHT_POINT;
	light[2].position.Set(-20, 5, 0);
	light[2].color.Set(LightChange, 0.f, 0.f);
	light[2].power = 1;
	light[2].kC = 1.f;
	light[2].kL = 0.01f;
	light[2].kQ = 0.001f;
	light[2].cosCutoff = cos(Math::DegreeToRadian(45));
	light[2].cosInner = cos(Math::DegreeToRadian(30));
	light[2].exponent = 3.f;
	light[2].spotDirection.Set(0.f, 1.f, 0.f);
	// 0
	light[3].type = Light::LIGHT_POINT;
	light[3].position.Set(20, 5, 20);
	light[3].color.Set(0.f, 0.f, LightChange);
	light[3].power = 1;
	light[3].kC = 1.f;
	light[3].kL = 0.01f;
	light[3].kQ = 0.001f;
	light[3].cosCutoff = cos(Math::DegreeToRadian(45));
	light[3].cosInner = cos(Math::DegreeToRadian(30));
	light[3].exponent = 3.f;
	light[3].spotDirection.Set(0.f, 1.f, 0.f);
	// 0
	light[4].type = Light::LIGHT_POINT;
	light[4].position.Set(20, 5, -20);
	light[4].color.Set(0.f, 0.f, LightChange);
	light[4].power = 1;
	light[4].kC = 1.f;
	light[4].kL = 0.01f;
	light[4].kQ = 0.001f;
	light[4].cosCutoff = cos(Math::DegreeToRadian(45));
	light[4].cosInner = cos(Math::DegreeToRadian(30));
	light[4].exponent = 3.f;
	light[4].spotDirection.Set(0.f, 1.f, 0.f);

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

	glUniform1i(m_parameters[U_LIGHT_CEILING1_TYPE], light[1].type);
	glUniform3fv(m_parameters[U_LIGHT_CEILING1_COLOR], 1, &light[1].color.r);
	glUniform1f(m_parameters[U_LIGHT_CEILING1_POWER], light[1].power);
	glUniform1f(m_parameters[U_LIGHT_CEILING1_KC], light[1].kC);
	glUniform1f(m_parameters[U_LIGHT_CEILING1_KL], light[1].kL);
	glUniform1f(m_parameters[U_LIGHT_CEILING1_KQ], light[1].kQ);
	glUniform3fv(m_parameters[U_LIGHT_CEILING1_SPOTDIRECTION], 1, &light[1].spotDirection.x);
	glUniform1f(m_parameters[U_LIGHT_CEILING1_COSCUTOFF], light[1].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT_CEILING1_COSINNER], light[1].cosInner);
	glUniform1f(m_parameters[U_LIGHT_CEILING1_EXPONENT], light[1].exponent);

	glUniform1i(m_parameters[U_LIGHT_CEILING2_TYPE], light[2].type);
	glUniform3fv(m_parameters[U_LIGHT_CEILING2_COLOR], 1, &light[2].color.r);
	glUniform1f(m_parameters[U_LIGHT_CEILING2_POWER], light[2].power);
	glUniform1f(m_parameters[U_LIGHT_CEILING2_KC], light[2].kC);
	glUniform1f(m_parameters[U_LIGHT_CEILING2_KL], light[2].kL);
	glUniform1f(m_parameters[U_LIGHT_CEILING2_KQ], light[2].kQ);
	glUniform3fv(m_parameters[U_LIGHT_CEILING2_SPOTDIRECTION], 1, &light[2].spotDirection.x);
	glUniform1f(m_parameters[U_LIGHT_CEILING2_COSCUTOFF], light[2].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT_CEILING2_COSINNER], light[2].cosInner);
	glUniform1f(m_parameters[U_LIGHT_CEILING2_EXPONENT], light[2].exponent);

	glUniform1i(m_parameters[U_LIGHT_CEILING3_TYPE], light[3].type);
	glUniform3fv(m_parameters[U_LIGHT_CEILING3_COLOR], 1, &light[3].color.r);
	glUniform1f(m_parameters[U_LIGHT_CEILING3_POWER], light[3].power);
	glUniform1f(m_parameters[U_LIGHT_CEILING3_KC], light[3].kC);
	glUniform1f(m_parameters[U_LIGHT_CEILING3_KL], light[3].kL);
	glUniform1f(m_parameters[U_LIGHT_CEILING3_KQ], light[3].kQ);
	glUniform3fv(m_parameters[U_LIGHT_CEILING3_SPOTDIRECTION], 1, &light[3].spotDirection.x);
	glUniform1f(m_parameters[U_LIGHT_CEILING3_COSCUTOFF], light[3].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT_CEILING3_COSINNER], light[3].cosInner);
	glUniform1f(m_parameters[U_LIGHT_CEILING3_EXPONENT], light[3].exponent);

	glUniform1i(m_parameters[U_LIGHT_CEILING4_TYPE], light[4].type);
	glUniform3fv(m_parameters[U_LIGHT_CEILING4_COLOR], 1, &light[4].color.r);
	glUniform1f(m_parameters[U_LIGHT_CEILING4_POWER], light[4].power);
	glUniform1f(m_parameters[U_LIGHT_CEILING4_KC], light[4].kC);
	glUniform1f(m_parameters[U_LIGHT_CEILING4_KL], light[4].kL);
	glUniform1f(m_parameters[U_LIGHT_CEILING4_KQ], light[4].kQ);
	glUniform3fv(m_parameters[U_LIGHT_CEILING4_SPOTDIRECTION], 1, &light[4].spotDirection.x);
	glUniform1f(m_parameters[U_LIGHT_CEILING4_COSCUTOFF], light[4].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT_CEILING4_COSINNER], light[4].cosInner);
	glUniform1f(m_parameters[U_LIGHT_CEILING4_EXPONENT], light[4].exponent);

	glUniform1i(m_parameters[U_NUMLIGHTS], 5);
}

// ANIMATION FUNCTIONS=========================================
void SceneText::AnimationReset()
{
	NPCs_transform[T_NPCRUPPERARM].rotationX.angle = 0;
	NPCs_transform[T_NPCRLOWERARM].rotationX.angle = 0;
	NPCs_transform[T_NPCRLOWERARM].rotationZ.angle = 0;
	NPCs_transform[T_NPCRFCLAW].rotationY.angle = 0;
	NPCs_transform[T_NPCRBCLAW].rotationY.angle = 180;

	NPCs_transform[T_NPCLUPPERARM].rotationX.angle = 0;
	NPCs_transform[T_NPCLLOWERARM].rotationX.angle = 0;
	NPCs_transform[T_NPCLLOWERARM].rotationZ.angle = 0;
	NPCs_transform[T_NPCLFCLAW].rotationY.angle = 0;
	NPCs_transform[T_NPCLBCLAW].rotationY.angle = 180;

	NPCs_transform[T_NPCRUPPERLEG].rotationX.angle = 0;
	NPCs_transform[T_NPCLUPPERLEG].rotationX.angle = 0;
}

void SceneText::playAnimationWalk(double time)
{
	if (stopA1 == false)
	{
		//ARMS
		if (retractArmA1 == false)
		{
			if (NPCs_transform[T_NPCRUPPERARM].rotationX.angle >= -30
				&& NPCs_transform[T_NPCLUPPERARM].rotationX.angle <= 30)
			{
				NPCs_transform[T_NPCRUPPERARM].rotationX.angle -= (float)(100 * time);
				NPCs_transform[T_NPCLUPPERARM].rotationX.angle += (float)(100 * time);
			}
			if (NPCs_transform[T_NPCRUPPERARM].rotationX.angle <= -30
				&& NPCs_transform[T_NPCLUPPERARM].rotationX.angle >= 30)
			{
				++A1Acount;
				retractArmA1 = true;
			}
		}
		else
		{
			if (NPCs_transform[T_NPCRUPPERARM].rotationX.angle <= 30
				&& NPCs_transform[T_NPCLUPPERARM].rotationX.angle >= -30)
			{
				NPCs_transform[T_NPCRUPPERARM].rotationX.angle += (float)(100 * time);
				NPCs_transform[T_NPCLUPPERARM].rotationX.angle -= (float)(100 * time);
			}
			if (NPCs_transform[T_NPCRUPPERARM].rotationX.angle >= 30
				&& NPCs_transform[T_NPCLUPPERARM].rotationX.angle <= -30)
			{
				++A1Acount;
				retractArmA1 = false;
			}
		}

		//LEGS
		if (retractLegA1 == false)
		{
			if (NPCs_transform[T_NPCRUPPERLEG].rotationX.angle >= -30
				&& NPCs_transform[T_NPCLUPPERLEG].rotationX.angle <= 30)
			{
				NPCs_transform[T_NPCRUPPERLEG].rotationX.angle -= (float)(100 * time);
				NPCs_transform[T_NPCLUPPERLEG].rotationX.angle += (float)(100 * time);
			}
			if (NPCs_transform[T_NPCRUPPERLEG].rotationX.angle <= -30
				&& NPCs_transform[T_NPCLUPPERLEG].rotationX.angle >= 30)
			{
				++A1Lcount;
				retractLegA1 = true;
			}
		}
		else
		{
			if (NPCs_transform[T_NPCRUPPERLEG].rotationX.angle <= 30
				&& NPCs_transform[T_NPCLUPPERLEG].rotationX.angle >= -30)
			{
				NPCs_transform[T_NPCRUPPERLEG].rotationX.angle += (float)(100 * time);
				NPCs_transform[T_NPCLUPPERLEG].rotationX.angle -= (float)(100 * time);
			}
			if (NPCs_transform[T_NPCRUPPERLEG].rotationX.angle >= 30
				&& NPCs_transform[T_NPCLUPPERLEG].rotationX.angle <= -30)
			{
				++A1Lcount;
				retractLegA1 = false;
			}
		}

		if (A1Acount == 2 && A1Lcount == 2
			&& NPCs_transform[T_NPCRUPPERARM].rotationX.angle <= 0
			&& NPCs_transform[T_NPCLUPPERARM].rotationX.angle >= 0
			&& NPCs_transform[T_NPCRUPPERLEG].rotationX.angle <= 0
			&& NPCs_transform[T_NPCLUPPERLEG].rotationX.angle >= 0)
		{
			stopA1 = true;
		}
	}
}

void SceneText::playAnimationCoffee(double time)
{
	if (stopA2 == false)
	{
		// COFFEE ANIMATION PART 1
		if (extendArmA2 == false && renderCup == false)
		{
			if (NPCs_transform[T_NPCRLOWERARM].rotationX.angle >= -90)
			{
				NPCs_transform[T_NPCRLOWERARM].rotationX.angle -= (float)(90 * time);
			}
			if (NPCs_transform[T_NPCRFCLAW].rotationY.angle >= -90
				&& NPCs_transform[T_NPCRBCLAW].rotationY.angle >= 90)
			{
				NPCs_transform[T_NPCRFCLAW].rotationY.angle -= (float)(90 * time);
				NPCs_transform[T_NPCRBCLAW].rotationY.angle -= (float)(90 * time);
			}

			// CHECK TO GO TO NEXT PART
			if (NPCs_transform[T_NPCRLOWERARM].rotationX.angle <= -90
				&& NPCs_transform[T_NPCRFCLAW].rotationY.angle <= -90
				&& NPCs_transform[T_NPCRBCLAW].rotationY.angle <= 90)
			{
				extendArmA2 = true;
			}
		}

		//COFFEE ANIMATION PART 2
		if (extendArmA2 == true && stopA2 == false && renderCup == false)
		{
			if (NPCs_transform[T_NPCRLOWERARM].rotationX.angle <= 0)
			{
				NPCs_transform[T_NPCRLOWERARM].rotationX.angle += (float)(90 * time);
			}
			if (NPCs_transform[T_NPCRUPPERARM].rotationX.angle >= -90)
			{
				NPCs_transform[T_NPCRUPPERARM].rotationX.angle -= (float)(90 * time);
			}

			// CHECK TO GO TO NEXT PART
			if (NPCs_transform[T_NPCRLOWERARM].rotationX.angle >= 0
				&& NPCs_transform[T_NPCRUPPERARM].rotationX.angle <= -90)
			{
				renderCup = true;
				extendArmA2 = false;
				retractArmA2 = true;
			}
		}

		//COFFEE ANIMATION PART 3
		if (extendArmA2 == false && retractArmA2 == true && renderCup == true)
		{
			if (NPCs_transform[T_NPCRUPPERARM].rotationX.angle <= 0)
			{
				NPCs_transform[T_NPCRUPPERARM].rotationX.angle += (float)(90 * time);
			}
			if (NPCs_transform[T_NPCRLOWERARM].rotationX.angle >= -90)
			{
				NPCs_transform[T_NPCRLOWERARM].rotationX.angle -= (float)(90 * time);
			}

			// CHECK TO END
			if (NPCs_transform[T_NPCRUPPERARM].rotationX.angle >= 0
				&& NPCs_transform[T_NPCRLOWERARM].rotationX.angle <= -90)
			{
				stopA2 = true;
			}
		}
	}
}

void SceneText::playAnimationTalking(double time)
{
	renderCup = false;

	if (stopA3 == false)
	{
		if (armUpA3 == true)
		{
			if (NPCs_transform[T_NPCRUPPERARM].rotationX.angle >= -45
				&& NPCs_transform[T_NPCLUPPERARM].rotationX.angle >= -45)
			{
				NPCs_transform[T_NPCRUPPERARM].rotationX.angle -= (float)(30 * time);
				NPCs_transform[T_NPCLUPPERARM].rotationX.angle -= (float)(30 * time);
			}
			if (NPCs_transform[T_NPCRLOWERARM].rotationX.angle >= -70
				&& NPCs_transform[T_NPCLLOWERARM].rotationX.angle >= -70)
			{
				NPCs_transform[T_NPCRLOWERARM].rotationX.angle -= (float)(60 * time);
				NPCs_transform[T_NPCLLOWERARM].rotationX.angle -= (float)(60 * time);
			}

			if (NPCs_transform[T_NPCRUPPERARM].rotationX.angle <= -45
				&& NPCs_transform[T_NPCLUPPERARM].rotationX.angle <= -45
				&& NPCs_transform[T_NPCRLOWERARM].rotationX.angle <= -70
				&& NPCs_transform[T_NPCLLOWERARM].rotationX.angle <= -70)
			{
				armUpA3 = false;
			}
		}

		else
		{
			if (NPCs_transform[T_NPCRUPPERARM].rotationX.angle <= 0
				&& NPCs_transform[T_NPCLUPPERARM].rotationX.angle <= 0)
			{
				NPCs_transform[T_NPCRUPPERARM].rotationX.angle += (float)(50 * time);
				NPCs_transform[T_NPCLUPPERARM].rotationX.angle += (float)(50 * time);
			}
			if (NPCs_transform[T_NPCRLOWERARM].rotationX.angle <= 0
				&& NPCs_transform[T_NPCLLOWERARM].rotationX.angle <= 0)
			{
				NPCs_transform[T_NPCRLOWERARM].rotationX.angle += (float)(80 * time);
				NPCs_transform[T_NPCLLOWERARM].rotationX.angle += (float)(80 * time);
			}

			if (NPCs_transform[T_NPCRUPPERARM].rotationX.angle >= 0
				&& NPCs_transform[T_NPCLUPPERARM].rotationX.angle >= 0
				&& NPCs_transform[T_NPCRLOWERARM].rotationX.angle >= 0
				&& NPCs_transform[T_NPCLLOWERARM].rotationX.angle >= 0)
			{
				armUpA3 = true;
				++A3count;
			}
		}

		if (A3count == 2)
		{
			stopA3 = true;
			A3count = 0;
		}
	}
}

void SceneText::playAnimationIdle(double time)
{
	renderCup = false;

	if (stopA4 == false)
	{
		if (armUpA4 == true)
		{
			if (NPCs_transform[T_NPCRUPPERARM].rotationX.angle >= -20
				&& NPCs_transform[T_NPCLUPPERARM].rotationX.angle >= -20)
			{
				NPCs_transform[T_NPCRUPPERARM].rotationX.angle -= (float)(40 * time);
				NPCs_transform[T_NPCLUPPERARM].rotationX.angle -= (float)(40 * time);
			}
			if (NPCs_transform[T_NPCRLOWERARM].rotationZ.angle <= 100
				&& NPCs_transform[T_NPCLLOWERARM].rotationZ.angle >= -100)
			{
				NPCs_transform[T_NPCRLOWERARM].rotationZ.angle += (float)(110 * time);
				NPCs_transform[T_NPCLLOWERARM].rotationZ.angle -= (float)(110 * time);
			}

			if (NPCs_transform[T_NPCRUPPERARM].rotationX.angle <= -20
				&& NPCs_transform[T_NPCLUPPERARM].rotationX.angle <= -20
				&& NPCs_transform[T_NPCRLOWERARM].rotationZ.angle >= 100
				&& NPCs_transform[T_NPCLLOWERARM].rotationZ.angle <= -100)
			{
				armUpA4 = false;
				stopA4 = true;
			}
		}
	}
}

void SceneText::playAnimationArcade(double time)
{
	renderCup = false;

	if (stopA5 == false)
	{
		if (NPCs_transform[T_NPCRUPPERARM].rotationX.angle >= -45
			&& NPCs_transform[T_NPCLUPPERARM].rotationX.angle >= -45)
		{
			NPCs_transform[T_NPCRUPPERARM].rotationX.angle -= (float)(50 * time);
			NPCs_transform[T_NPCLUPPERARM].rotationX.angle -= (float)(50 * time);
		}
		if (NPCs_transform[T_NPCRLOWERARM].rotationX.angle >= -40
			&& NPCs_transform[T_NPCLLOWERARM].rotationX.angle >= -40)
		{
			NPCs_transform[T_NPCRLOWERARM].rotationX.angle -= (float)(50 * time);
			NPCs_transform[T_NPCLLOWERARM].rotationX.angle -= (float)(50 * time);
		}
		if (NPCs_transform[T_NPCRFCLAW].rotationY.angle >= -90
			&& NPCs_transform[T_NPCRBCLAW].rotationY.angle >= 90)
		{
			NPCs_transform[T_NPCRFCLAW].rotationY.angle -= (float)(120 * time);
			NPCs_transform[T_NPCRBCLAW].rotationY.angle -= (float)(120 * time);
		}

		if (NPCs_transform[T_NPCLFCLAW].rotationY.angle <= 90
			&& NPCs_transform[T_NPCLBCLAW].rotationY.angle <= 270)
		{
			NPCs_transform[T_NPCLFCLAW].rotationY.angle += (float)(120 * time);
			NPCs_transform[T_NPCLBCLAW].rotationY.angle += (float)(120 * time);
		}
	}
}
//==============================================================

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
	// NPC INIT============================================================================
	meshList[GEO_NPCBODY] = MeshBuilder::GenerateOBJ("NPC", "obj//NPC_Body.obj");
	//meshList[GEO_NPCBODY]->material.kAmbient.Set(1, 1, 1);
	//meshList[GEO_NPCBODY]->material.kDiffuse.Set(1, 1, 1);
	//meshList[GEO_NPCBODY]->material.kSpecular.Set(1, 1, 1);
	//meshList[GEO_NPCBODY]->material.kShininess = 0.6f;

	NPCs_transform[T_NPCBODY].translation = Vector3(0, 2, 0);
	NPCs_transform[T_NPCBODY].rotationX.angle = 0;
	NPCs_transform[T_NPCBODY].rotationY.angle = 0;
	NPCs_transform[T_NPCBODY].rotationZ.angle = 0;
	NPCs_transform[T_NPCBODY].scaling = Vector3(1, 1, 1);

	meshList[GEO_NPCHEAD] = MeshBuilder::GenerateOBJ("NPC", "obj//NPC_Head.obj");
	NPCs_transform[T_NPCHEAD].translation = Vector3(0, 3.35, 0);
	NPCs_transform[T_NPCHEAD].rotationX.angle = 0;
	NPCs_transform[T_NPCHEAD].rotationY.angle = 0;
	NPCs_transform[T_NPCHEAD].rotationZ.angle = 0;
	NPCs_transform[T_NPCHEAD].scaling = Vector3(1, 1, 1);

	// RIGHT ARM
	meshList[GEO_NPCUPPERARM] = MeshBuilder::GenerateOBJ("NPC", "obj//NPC_UpperArm2.obj");
	NPCs_transform[T_NPCRUPPERARM].translation = Vector3(-1.3, 3, 0);
	NPCs_transform[T_NPCRUPPERARM].rotationX.angle = 0;
	NPCs_transform[T_NPCRUPPERARM].rotationY.angle = 0;
	NPCs_transform[T_NPCRUPPERARM].rotationZ.angle = 0;
	NPCs_transform[T_NPCRUPPERARM].scaling = Vector3(1, 1, 1);

	meshList[GEO_NPCLOWERARM] = MeshBuilder::GenerateOBJ("NPC", "obj//NPC_LowerArm2.obj");
	NPCs_transform[T_NPCRLOWERARM].translation = Vector3(0, -2.15, 0);
	NPCs_transform[T_NPCRLOWERARM].rotationX.angle = 0;
	NPCs_transform[T_NPCRLOWERARM].rotationY.angle = 0;
	NPCs_transform[T_NPCRLOWERARM].rotationZ.angle = 0;
	NPCs_transform[T_NPCRLOWERARM].scaling = Vector3(1, 1, 1);

	meshList[GEO_NPCCLAW] = MeshBuilder::GenerateOBJ("NPC", "obj//NPC_Claw.obj");
	NPCs_transform[T_NPCRFCLAW].translation = Vector3(0, -2.45, 0);
	NPCs_transform[T_NPCRFCLAW].rotationX.angle = 0;
	NPCs_transform[T_NPCRFCLAW].rotationY.angle = 0;
	NPCs_transform[T_NPCRFCLAW].rotationZ.angle = 0;
	NPCs_transform[T_NPCRFCLAW].scaling = Vector3(1, 1, 1);

	meshList[GEO_NPCCLAW] = MeshBuilder::GenerateOBJ("NPC", "obj//NPC_Claw.obj");
	NPCs_transform[T_NPCRBCLAW].translation = Vector3(0, -2.45, 0);
	NPCs_transform[T_NPCRBCLAW].rotationX.angle = 0;
	NPCs_transform[T_NPCRBCLAW].rotationY.angle = 180;
	NPCs_transform[T_NPCRBCLAW].rotationZ.angle = 0;
	NPCs_transform[T_NPCRBCLAW].scaling = Vector3(1, 1, 1);

	// LEFT ARM
	meshList[GEO_NPCUPPERARM] = MeshBuilder::GenerateOBJ("NPC", "obj//NPC_UpperArm2.obj");
	NPCs_transform[T_NPCLUPPERARM].translation = Vector3(1.3, 3, 0);
	NPCs_transform[T_NPCLUPPERARM].rotationX.angle = 0;
	NPCs_transform[T_NPCLUPPERARM].rotationY.angle = 0;
	NPCs_transform[T_NPCLUPPERARM].rotationZ.angle = 0;
	NPCs_transform[T_NPCLUPPERARM].scaling = Vector3(1, 1, 1);

	meshList[GEO_NPCLOWERARM] = MeshBuilder::GenerateOBJ("NPC", "obj//NPC_LowerArm2.obj");
	NPCs_transform[T_NPCLLOWERARM].translation = Vector3(0, -2.15, 0);
	NPCs_transform[T_NPCLLOWERARM].rotationX.angle = 0;
	NPCs_transform[T_NPCLLOWERARM].rotationY.angle = 0;
	NPCs_transform[T_NPCLLOWERARM].rotationZ.angle = 0;
	NPCs_transform[T_NPCLLOWERARM].scaling = Vector3(1, 1, 1);

	meshList[GEO_NPCCLAW] = MeshBuilder::GenerateOBJ("NPC", "obj//NPC_Claw.obj");
	NPCs_transform[T_NPCLFCLAW].translation = Vector3(0, -2.45, 0);
	NPCs_transform[T_NPCLFCLAW].rotationX.angle = 0;
	NPCs_transform[T_NPCLFCLAW].rotationY.angle = 0;
	NPCs_transform[T_NPCLFCLAW].rotationZ.angle = 0;
	NPCs_transform[T_NPCLFCLAW].scaling = Vector3(1, 1, 1);

	meshList[GEO_NPCCLAW] = MeshBuilder::GenerateOBJ("NPC", "obj//NPC_Claw.obj");
	NPCs_transform[T_NPCLBCLAW].translation = Vector3(0, -2.45, 0);
	NPCs_transform[T_NPCLBCLAW].rotationX.angle = 0;
	NPCs_transform[T_NPCLBCLAW].rotationY.angle = 180;
	NPCs_transform[T_NPCLBCLAW].rotationZ.angle = 0;
	NPCs_transform[T_NPCLBCLAW].scaling = Vector3(1, 1, 1);

	// LEFT LEG
	meshList[GEO_NPCUPPERLEG] = MeshBuilder::GenerateOBJ("NPC", "obj//NPC_UpperLeg2.obj");
	NPCs_transform[T_NPCLUPPERLEG].translation = Vector3(-0.7, 0.5, 0);
	NPCs_transform[T_NPCLUPPERLEG].rotationX.angle = 0;
	NPCs_transform[T_NPCLUPPERLEG].rotationY.angle = 0;
	NPCs_transform[T_NPCLUPPERLEG].rotationZ.angle = 0;
	NPCs_transform[T_NPCLUPPERLEG].scaling = Vector3(1, 1, 1);

	meshList[GEO_NPCLOWERLEG] = MeshBuilder::GenerateOBJ("NPC", "obj//NPC_LowerLeg.obj");
	NPCs_transform[T_NPCLLOWERLEG].translation = Vector3(0, -5.1, 0.5);
	NPCs_transform[T_NPCLLOWERLEG].rotationX.angle = 0;
	NPCs_transform[T_NPCLLOWERLEG].rotationY.angle = 0;
	NPCs_transform[T_NPCLLOWERLEG].rotationZ.angle = 0;
	NPCs_transform[T_NPCLLOWERLEG].scaling = Vector3(1, 1, 1);

	// RIGHT LEG
	meshList[GEO_NPCUPPERLEG] = MeshBuilder::GenerateOBJ("NPC", "obj//NPC_UpperLeg2.obj");
	NPCs_transform[T_NPCRUPPERLEG].translation = Vector3(0.7, 0.5, 0);
	NPCs_transform[T_NPCRUPPERLEG].rotationX.angle = 0;
	NPCs_transform[T_NPCRUPPERLEG].rotationY.angle = 0;
	NPCs_transform[T_NPCRUPPERLEG].rotationZ.angle = 0;
	NPCs_transform[T_NPCRUPPERLEG].scaling = Vector3(1, 1, 1);

	meshList[GEO_NPCLOWERLEG] = MeshBuilder::GenerateOBJ("NPC", "obj//NPC_LowerLeg.obj");
	NPCs_transform[T_NPCRLOWERLEG].translation = Vector3(0, -5.1, 0.5);
	NPCs_transform[T_NPCRLOWERLEG].rotationX.angle = 0;
	NPCs_transform[T_NPCRLOWERLEG].rotationY.angle = 0;
	NPCs_transform[T_NPCRLOWERLEG].rotationZ.angle = 0;
	NPCs_transform[T_NPCRLOWERLEG].scaling = Vector3(1, 1, 1);
	//=====================================================================================
}

void SceneText::InitObjs()
{
	meshList[GEO_ENV_ARCADE_BUTTON_EXT_B] = MeshBuilder::GenerateOBJ("arcade button exterior", "obj//Arcade Button_Exterior.obj");
	meshList[GEO_ENV_ARCADE_BUTTON_EXT_B]->textureID = LoadTGA("Image//arcade_button.tga");
	meshList[GEO_ENV_ARCADE_BUTTON_EXT_B]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_ARCADE_BUTTON_EXT_B]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_ARCADE_BUTTON_EXT_B]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_ARCADE_BUTTON_EXT_B]->material.kShininess = 0.6f;

	obj_transform[ENV_ARCADE_BUTTON_EXT_B].translation = Vector3(0.2f, 1.43f, 0.4f);
	obj_transform[ENV_ARCADE_BUTTON_EXT_B].rotationX.angle = 18;
	obj_transform[ENV_ARCADE_BUTTON_EXT_B].rotationY.angle = 0;
	obj_transform[ENV_ARCADE_BUTTON_EXT_B].rotationZ.angle = 0;
	obj_transform[ENV_ARCADE_BUTTON_EXT_B].scaling = Vector3(1.f, 1.f, 1.f);

	meshList[GEO_ENV_ARCADE_BUTTON_INT_B] = MeshBuilder::GenerateOBJ("arcade button interior", "obj//Arcade Button_Interior.obj");
	meshList[GEO_ENV_ARCADE_BUTTON_INT_B]->textureID = LoadTGA("Image//arcade_button.tga");
	meshList[GEO_ENV_ARCADE_BUTTON_INT_B]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_ARCADE_BUTTON_INT_B]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_ARCADE_BUTTON_INT_B]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_ARCADE_BUTTON_INT_B]->material.kShininess = 0.6f;

	obj_transform[ENV_ARCADE_BUTTON_INT_B].translation = Vector3(0, 0, 0);
	obj_transform[ENV_ARCADE_BUTTON_INT_B].rotationX.angle = 0;
	obj_transform[ENV_ARCADE_BUTTON_INT_B].rotationY.angle = 0;
	obj_transform[ENV_ARCADE_BUTTON_INT_B].rotationZ.angle = 0;
	obj_transform[ENV_ARCADE_BUTTON_INT_B].scaling = Vector3(1.f, 1.f, 1.f);

	meshList[GEO_ENV_JOYSTICK_BASE1] = MeshBuilder::GenerateOBJ("joystick base", "obj//Joystick_Base.obj");
	meshList[GEO_ENV_JOYSTICK_BASE1]->textureID = LoadTGA("Image//Joystick_Base.tga");
	meshList[GEO_ENV_JOYSTICK_BASE1]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_JOYSTICK_BASE1]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_JOYSTICK_BASE1]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_JOYSTICK_BASE1]->material.kShininess = 0.6f;

	obj_transform[ENV_JOYSTICK_BASE1].translation = Vector3(-0.4f, 1.43f, 0.4f);
	obj_transform[ENV_JOYSTICK_BASE1].rotationX.angle = 18;
	obj_transform[ENV_JOYSTICK_BASE1].rotationY.angle = 0;
	obj_transform[ENV_JOYSTICK_BASE1].rotationZ.angle = 0;
	obj_transform[ENV_JOYSTICK_BASE1].scaling = Vector3(0.25f, 0.25f, 0.25f);

	meshList[GEO_ENV_JOYSTICK_CONTROLLER1] = MeshBuilder::GenerateOBJ("joystick control", "obj//Joystick_Control.obj");
	meshList[GEO_ENV_JOYSTICK_CONTROLLER1]->textureID = LoadTGA("Image//Joystick_Control.tga");
	meshList[GEO_ENV_JOYSTICK_CONTROLLER1]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_JOYSTICK_CONTROLLER1]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_JOYSTICK_CONTROLLER1]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_JOYSTICK_CONTROLLER1]->material.kShininess = 0.6f;

	obj_transform[ENV_JOYSTICK_CONTROLLER1].translation = Vector3(0, 0, 0);       
	obj_transform[ENV_JOYSTICK_CONTROLLER1].rotationX.angle = 0;
	obj_transform[ENV_JOYSTICK_CONTROLLER1].rotationY.angle = 0;
	obj_transform[ENV_JOYSTICK_CONTROLLER1].rotationZ.angle = 0;
	obj_transform[ENV_JOYSTICK_CONTROLLER1].scaling = Vector3(1.f, 1.f, 1.f);

	// render texture for button green machine
	for (int i = GEO_ENV_ARCADE_BUTTON_EXT1; i <= GEO_ENV_ARCADE_BUTTON_EXT9; i += 2) {

		meshList[i] = MeshBuilder::GenerateOBJ("arcade button exterior", "obj//Arcade Button_Exterior.obj");
		meshList[i]->textureID = LoadTGA("Image//arcade_button.tga");
		meshList[i]->material.kAmbient.Set(1, 1, 1);
		meshList[i]->material.kDiffuse.Set(1, 1, 1);
		meshList[i]->material.kSpecular.Set(1, 1, 1);
		meshList[i]->material.kShininess = 0.6f;

		meshList[i + 1] = MeshBuilder::GenerateOBJ("arcade button interior", "obj//Arcade Button_Interior.obj");
		meshList[i + 1]->textureID = LoadTGA("Image//arcade_button.tga");
		meshList[i + 1]->material.kAmbient.Set(1, 1, 1);
		meshList[i + 1]->material.kDiffuse.Set(1, 1, 1);
		meshList[i + 1]->material.kSpecular.Set(1, 1, 1);
		meshList[i + 1]->material.kShininess = 0.6f;

	}

	// init transform arcade button for green machine
	for (int i = ENV_ARCADE_BUTTON_EXT1; i <= ENV_ARCADE_BUTTON_EXT9; i += 2) {
		static float j = 0.f;
		static float k = 0.f;
		static float l = 0.f;
		static int check = 1;

		if (check <= 3) { //row 1

			obj_transform[i].translation = Vector3(-0.2f + j, 1.49f, 0.23f);
			obj_transform[i].rotationX.angle = 18;
			obj_transform[i].rotationY.angle = 0;
			obj_transform[i].rotationZ.angle = 0;
			obj_transform[i].scaling = Vector3(1.f, 1.f, 1.f);

			obj_transform[i + 1].translation = Vector3(0, 0, 0);
			obj_transform[i + 1].rotationX.angle = 0;
			obj_transform[i + 1].rotationY.angle = 0;
			obj_transform[i + 1].rotationZ.angle = 0;
			obj_transform[i + 1].scaling = Vector3(1.f, 1.f, 1.f);

			j += 0.2f;

		}
		else if (check <= 6) {  // row 2


			obj_transform[i].translation = Vector3(-0.2f + k, 1.43f, 0.4f);
			obj_transform[i].rotationX.angle = 18;
			obj_transform[i].rotationY.angle = 0;
			obj_transform[i].rotationZ.angle = 0;
			obj_transform[i].scaling = Vector3(1.f, 1.f, 1.f);

			obj_transform[i + 1].translation = Vector3(0, 0, 0);
			obj_transform[i + 1].rotationX.angle = 0;
			obj_transform[i + 1].rotationY.angle = 0;
			obj_transform[i + 1].rotationZ.angle = 0;
			obj_transform[i + 1].scaling = Vector3(1.f, 1.f, 1.f);

			k += 0.2f;
		}
		else if (check > 6) {   // row 3

			obj_transform[i].translation = Vector3(-0.2f + l, 1.37f, 0.6f);
			obj_transform[i].rotationX.angle = 18;
			obj_transform[i].rotationY.angle = 0;
			obj_transform[i].rotationZ.angle = 0;
			obj_transform[i].scaling = Vector3(1.f, 1.f, 1.f);

			obj_transform[i + 1].translation = Vector3(0, 0, 0);
			obj_transform[i + 1].rotationX.angle = 0;
			obj_transform[i + 1].rotationY.angle = 0;
			obj_transform[i + 1].rotationZ.angle = 0;
			obj_transform[i + 1].scaling = Vector3(1.f, 1.f, 1.f);

			l += 0.2f;
		}

		check += 1;
	}

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
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_1]->textureID = LoadTGA("Image//Car_Stand_Display.tga");
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_1]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_1]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_1]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_1]->material.kShininess = 0.6f;

	obj_transform[ENV_CAR_DISPLAY_PLATFORM_1].translation = Vector3(-28, -1, 1.f);
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_1].rotationX.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_1].rotationY.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_1].rotationZ.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_1].scaling = Vector3(3.f, 3.f, 3.f);

	meshList[GEO_CAR_KEN] = MeshBuilder::GenerateOBJ("carKen", "obj//Ken_Car_Display.obj");
	meshList[GEO_CAR_KEN]->textureID = LoadTGA("Image//car1.tga");
	meshList[GEO_CAR_KEN]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_CAR_KEN]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_CAR_KEN]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_CAR_KEN]->material.kShininess = 0.6f;

	obj_transform[ENV_CAR_KEN].translation = Vector3(0, 1, 0.f);
	obj_transform[ENV_CAR_KEN].rotationX.angle = 0;
	obj_transform[ENV_CAR_KEN].rotationY.angle = 90;
	obj_transform[ENV_CAR_KEN].rotationZ.angle = 0;
	obj_transform[ENV_CAR_KEN].scaling = Vector3(0.1f, 0.1f, 0.1f);

	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_2] = MeshBuilder::GenerateOBJ("car platform 2", "obj//Car Stand Display.obj");
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_2]->textureID = LoadTGA("Image//Car_Stand_Display.tga");
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_2]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_2]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_2]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_2]->material.kShininess = 0.6f;

	obj_transform[ENV_CAR_DISPLAY_PLATFORM_2].translation = Vector3(-28, -1, 18);
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_2].rotationX.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_2].rotationY.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_2].rotationZ.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_2].scaling = Vector3(3.f, 3.f, 3.f);

	meshList[GEO_CAR_JOSQUIN] = MeshBuilder::GenerateOBJ("carJosquin", "obj//Josquin_car_display.obj");
	meshList[GEO_CAR_JOSQUIN]->textureID = LoadTGA("Image//Josquin_car.tga");
	meshList[GEO_CAR_JOSQUIN]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_CAR_JOSQUIN]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_CAR_JOSQUIN]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_CAR_JOSQUIN]->material.kShininess = 0.6f;

	obj_transform[ENV_CAR_JOSQUIN].translation = Vector3(0, 1, 0.f);
	obj_transform[ENV_CAR_JOSQUIN].rotationX.angle = 0;
	obj_transform[ENV_CAR_JOSQUIN].rotationY.angle = 0;
	obj_transform[ENV_CAR_JOSQUIN].rotationZ.angle = 0;
	obj_transform[ENV_CAR_JOSQUIN].scaling = Vector3(.1f, .1f, .1f);

	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_3] = MeshBuilder::GenerateOBJ("car platform 3", "obj//Car Stand Display.obj");
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_3]->textureID = LoadTGA("Image//Car_Stand_Display.tga");
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_3]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_3]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_3]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_3]->material.kShininess = 0.6f;

	obj_transform[ENV_CAR_DISPLAY_PLATFORM_3].translation = Vector3(28, -1, 17);
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_3].rotationX.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_3].rotationY.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_3].rotationZ.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_3].scaling = Vector3(3.f, 3.f, 3.f);

	meshList[GEO_CAR_KLYDE] = MeshBuilder::GenerateOBJ("carKlyde", "obj//Klyde_car.obj");
	meshList[GEO_CAR_KLYDE]->textureID = LoadTGA("Image//Klyde_Car.tga");
	meshList[GEO_CAR_KLYDE]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_CAR_KLYDE]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_CAR_KLYDE]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_CAR_KLYDE]->material.kShininess = 0.6f;

	obj_transform[ENV_CAR_KLYDE].translation = Vector3(0, 1, 0.f);
	obj_transform[ENV_CAR_KLYDE].rotationX.angle = 0;
	obj_transform[ENV_CAR_KLYDE].rotationY.angle = 180;
	obj_transform[ENV_CAR_KLYDE].rotationZ.angle = 0;
	obj_transform[ENV_CAR_KLYDE].scaling = Vector3(0.1f, 0.1f, 0.1f);

	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_4] = MeshBuilder::GenerateOBJ("car platform 4", "obj//Car Stand Display.obj");
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_4]->textureID = LoadTGA("Image//Car_Stand_Display.tga");
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_4]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_4]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_4]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_4]->material.kShininess = 0.6f;

	obj_transform[ENV_CAR_DISPLAY_PLATFORM_4].translation = Vector3(28, -1, 1);
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_4].rotationX.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_4].rotationY.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_4].rotationZ.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_4].scaling = Vector3(3.f, 3.f, 3.f);

	meshList[GEO_CAR_SHAHIR] = MeshBuilder::GenerateOBJ("carSHahir", "obj//shahir_car_display.obj");
	meshList[GEO_CAR_SHAHIR]->textureID = LoadTGA("Image//shahir_car.tga");
	meshList[GEO_CAR_SHAHIR]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_CAR_SHAHIR]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_CAR_SHAHIR]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_CAR_SHAHIR]->material.kShininess = 0.6f;

	obj_transform[ENV_CAR_SHAHIR].translation = Vector3(0, 1, 0.f);
	obj_transform[ENV_CAR_SHAHIR].rotationX.angle = 0;
	obj_transform[ENV_CAR_SHAHIR].rotationY.angle = 0;
	obj_transform[ENV_CAR_SHAHIR].rotationZ.angle = 0;
	obj_transform[ENV_CAR_SHAHIR].scaling = Vector3(0.1f, 0.1f, 0.1f);

	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_5] = MeshBuilder::GenerateOBJ("car platform 5", "obj//Car Stand Display.obj");
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_5]->textureID = LoadTGA("Image//Car_Stand_Display.tga");
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_5]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_5]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_5]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_CAR_DISPLAY_PLATFORM_5]->material.kShininess = 0.6f;

	obj_transform[ENV_CAR_DISPLAY_PLATFORM_5].translation = Vector3(28, -1, -16);
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_5].rotationX.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_5].rotationY.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_5].rotationZ.angle = 0;
	obj_transform[ENV_CAR_DISPLAY_PLATFORM_5].scaling = Vector3(3.f, 3.f, 3.f);

	meshList[GEO_CAR_EMILY] = MeshBuilder::GenerateOBJ("carSHahir", "obj//Emily_Car_Display.obj");
	meshList[GEO_CAR_EMILY]->textureID = LoadTGA("Image//Emily_car.tga");
	meshList[GEO_CAR_EMILY]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_CAR_EMILY]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_CAR_EMILY]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_CAR_EMILY]->material.kShininess = 0.6f;

	obj_transform[ENV_CAR_EMILY].translation = Vector3(0, 1, 0.f);
	obj_transform[ENV_CAR_EMILY].rotationX.angle = 0;
	obj_transform[ENV_CAR_EMILY].rotationY.angle = 0;
	obj_transform[ENV_CAR_EMILY].rotationZ.angle = 0;
	obj_transform[ENV_CAR_EMILY].scaling = Vector3(0.1f, 0.1f, 0.1f);

	meshList[GEO_ENV_COFFEE_MACHINE] = MeshBuilder::GenerateOBJ("coffee machine", "obj//Coffee Machine.obj");
	meshList[GEO_ENV_COFFEE_MACHINE]->textureID = LoadTGA("Image//Coffee_Machine.tga");
	meshList[GEO_ENV_COFFEE_MACHINE]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_COFFEE_MACHINE]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_COFFEE_MACHINE]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_COFFEE_MACHINE]->material.kShininess = 0.6f;

	obj_transform[ENV_COFFEE_MACHINE].translation = Vector3(10, 0, -23);
	obj_transform[ENV_COFFEE_MACHINE].rotationX.angle = 0;
	obj_transform[ENV_COFFEE_MACHINE].rotationY.angle = 0;
	obj_transform[ENV_COFFEE_MACHINE].rotationZ.angle = 0;
	obj_transform[ENV_COFFEE_MACHINE].scaling = Vector3(3.f, 3.f, 3.f);

	meshList[GEO_ENV_COFFEE_CUP] = MeshBuilder::GenerateOBJ("coffee cup", "obj//Coffee Cup.obj");
	meshList[GEO_ENV_COFFEE_CUP]->textureID = LoadTGA("Image//Coffee Cup_Silver.tga");
	meshList[GEO_ENV_COFFEE_CUP]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_COFFEE_CUP]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_COFFEE_CUP]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_COFFEE_CUP]->material.kShininess = 0.6f;

	obj_transform[ENV_COFFEE_CUP].translation = Vector3(-0.35, -0.25, 0);
	obj_transform[ENV_COFFEE_CUP].rotationX.angle = 0;
	obj_transform[ENV_COFFEE_CUP].rotationY.angle = 0;
	obj_transform[ENV_COFFEE_CUP].rotationZ.angle = -90;
	obj_transform[ENV_COFFEE_CUP].scaling = Vector3(4.f, 4.f, 4.f);

	meshList[GEO_ENV_SIGNPOST_1] = MeshBuilder::GenerateOBJ("signpost1", "obj//Signpost.obj");
	meshList[GEO_ENV_SIGNPOST_1]->textureID = LoadTGA("Image//Signpost.tga");
	meshList[GEO_ENV_SIGNPOST_1]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_SIGNPOST_1]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_SIGNPOST_1]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_SIGNPOST_1]->material.kShininess = 0.6f;

	obj_transform[ENV_SIGNPOST_1].translation = Vector3(-20, 0, -2);
	obj_transform[ENV_SIGNPOST_1].rotationX.angle = 0;
	obj_transform[ENV_SIGNPOST_1].rotationY.angle = 0;
	obj_transform[ENV_SIGNPOST_1].rotationZ.angle = 0;
	obj_transform[ENV_SIGNPOST_1].scaling = Vector3(2.f, 2.f, 2.f);

	meshList[GEO_ENV_SIGNPOST_2] = MeshBuilder::GenerateOBJ("signpost2", "obj//Signpost.obj");
	meshList[GEO_ENV_SIGNPOST_2]->textureID = LoadTGA("Image//Signpost.tga");
	meshList[GEO_ENV_SIGNPOST_2]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_SIGNPOST_2]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_SIGNPOST_2]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_SIGNPOST_2]->material.kShininess = 0.6f;

	obj_transform[ENV_SIGNPOST_2].translation = Vector3(-20, 0, 13.f);
	obj_transform[ENV_SIGNPOST_2].rotationX.angle = 0;
	obj_transform[ENV_SIGNPOST_2].rotationY.angle = 0;
	obj_transform[ENV_SIGNPOST_2].rotationZ.angle = 0;
	obj_transform[ENV_SIGNPOST_2].scaling = Vector3(2.f, 2.f, 2.f);

	meshList[GEO_ENV_SIGNPOST_3] = MeshBuilder::GenerateOBJ("signpost3", "obj//Signpost.obj");
	meshList[GEO_ENV_SIGNPOST_3]->textureID = LoadTGA("Image//Signpost.tga");
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
	meshList[GEO_ENV_SIGNPOST_4]->textureID = LoadTGA("Image//Signpost.tga");
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
	meshList[GEO_ENV_SIGNPOST_5]->textureID = LoadTGA("Image//Signpost.tga");
	meshList[GEO_ENV_SIGNPOST_5]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_ENV_SIGNPOST_5]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_ENV_SIGNPOST_5]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_ENV_SIGNPOST_5]->material.kShininess = 0.6f;

	obj_transform[ENV_SIGNPOST_5].translation = Vector3(20, 0, -10.f);
	obj_transform[ENV_SIGNPOST_5].rotationX.angle = 0;
	obj_transform[ENV_SIGNPOST_5].rotationY.angle = 180;
	obj_transform[ENV_SIGNPOST_5].rotationZ.angle = 0;
	obj_transform[ENV_SIGNPOST_5].scaling = Vector3(2.f, 2.f, 2.f);

	//spawn point
	meshList[GEO_TELEPORTER] = MeshBuilder::GenerateOBJ("signpost5", "obj//teleporter.obj");
	meshList[GEO_TELEPORTER]->textureID = LoadTGA("Image//teleporter.tga");
	meshList[GEO_TELEPORTER]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_TELEPORTER]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_TELEPORTER]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_TELEPORTER]->material.kShininess = 0.6f;

	obj_transform[ENV_TELEPORTER].translation = Vector3(0, 0.2f, 5.f);
	obj_transform[ENV_TELEPORTER].rotationX.angle = 0;
	obj_transform[ENV_TELEPORTER].rotationY.angle = 180;
	obj_transform[ENV_TELEPORTER].rotationZ.angle = 0;
	obj_transform[ENV_TELEPORTER].scaling = Vector3(2.f, 2.f, 2.f);

	// to gacha scene
	meshList[GEO_TELEPORTER2] = MeshBuilder::GenerateOBJ("signpost5", "obj//teleporter.obj");
	meshList[GEO_TELEPORTER2]->textureID = LoadTGA("Image//teleporter.tga");
	meshList[GEO_TELEPORTER2]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_TELEPORTER2]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_TELEPORTER2]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_TELEPORTER2]->material.kShininess = 0.6f;

	obj_transform[ENV_TELEPORTER2].translation = Vector3(-24, 0.f, -23.f);
	obj_transform[ENV_TELEPORTER2].rotationX.angle = 0;
	obj_transform[ENV_TELEPORTER2].rotationY.angle = 180;
	obj_transform[ENV_TELEPORTER2].rotationZ.angle = 0;
	obj_transform[ENV_TELEPORTER2].scaling = Vector3(2.f, 2.f, 2.f);

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

void SceneText::RenderLights()
{
	// default ================================================================================================================================================================================================
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
	// default end ============================================================================================================================================================================================
	// ceiling light 1 ========================================================================================================================================================================================
	if (light[1].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[1].position.x, light[1].position.y, light[1].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT_CEILING1_POSITION], 1, &lightDirection_cameraspace.x);
	}
	// if it is spot light, pass in position and direction 
	else if (light[1].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[1].position;
		glUniform3fv(m_parameters[U_LIGHT_CEILING1_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[1].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT_CEILING1_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		// default is point light (only position since point light is 360 degrees)
		Position lightPosition_cameraspace = viewStack.Top() * light[1].position;
		glUniform3fv(m_parameters[U_LIGHT_CEILING1_POSITION], 1, &lightPosition_cameraspace.x);
	}
	// ceiling light 1 end ======================================================================================================================================================================================
	// ceiling light 2 ==========================================================================================================================================================================================
	if (light[2].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[2].position.x, light[2].position.y, light[2].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT_CEILING2_POSITION], 1, &lightDirection_cameraspace.x);
	}
	// if it is spot light, pass in position and direction 
	else if (light[2].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[2].position;
		glUniform3fv(m_parameters[U_LIGHT_CEILING2_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[2].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT_CEILING2_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		// default is point light (only position since point light is 360 degrees)
		Position lightPosition_cameraspace = viewStack.Top() * light[2].position;
		glUniform3fv(m_parameters[U_LIGHT_CEILING2_POSITION], 1, &lightPosition_cameraspace.x);
	}
	// ceiling light 2 end ======================================================================================================================================================================================
	// ceiling light 3 ==========================================================================================================================================================================================
	if (light[3].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[3].position.x, light[3].position.y, light[3].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT_CEILING3_POSITION], 1, &lightDirection_cameraspace.x);
	}
	// if it is spot light, pass in position and direction 
	else if (light[3].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[3].position;
		glUniform3fv(m_parameters[U_LIGHT_CEILING3_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[3].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT_CEILING3_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		// default is point light (only position since point light is 360 degrees)
		Position lightPosition_cameraspace = viewStack.Top() * light[3].position;
		glUniform3fv(m_parameters[U_LIGHT_CEILING3_POSITION], 1, &lightPosition_cameraspace.x);
	}
	// ceiling light 3 end ======================================================================================================================================================================================
	// ceiling light 4 ==========================================================================================================================================================================================
	if (light[4].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[4].position.x, light[4].position.y, light[4].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT_CEILING4_POSITION], 1, &lightDirection_cameraspace.x);
	}
	// if it is spot light, pass in position and direction 
	else if (light[4].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[4].position;
		glUniform3fv(m_parameters[U_LIGHT_CEILING4_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[4].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT_CEILING4_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		// default is point light (only position since point light is 360 degrees)
		Position lightPosition_cameraspace = viewStack.Top() * light[4].position;
		glUniform3fv(m_parameters[U_LIGHT_CEILING4_POSITION], 1, &lightPosition_cameraspace.x);
	}
	// ceiling light 4 end ======================================================================================================================================================================================
	
	
}

void SceneText::RenderSkybox()
{
	modelStack.PushMatrix();
		skybox_transform[0].translation = Vector3(-37.5f, 12.5f, 0);
		skybox_transform[0].scaling = Vector3(50.f, 25.f, 50.f);
		skybox_transform[0].rotationY.angle = 90;
		RenderObject(meshList[GEO_LEFT], skybox_transform[0], false, true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
		skybox_transform[1].translation = Vector3(37.5f, 12.5f, 0);
		skybox_transform[1].scaling = Vector3(50.f, 25.f, 50.f);
		skybox_transform[1].rotationY.angle = -90;
		RenderObject(meshList[GEO_RIGHT], skybox_transform[1], false, true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
		skybox_transform[2].translation = Vector3(0, 25.f, 0);
		skybox_transform[2].scaling = Vector3(50.f, 75.f, 50.f);
		skybox_transform[2].rotationX.angle = 90;
		skybox_transform[2].rotationZ.angle = 90;
		RenderObject(meshList[GEO_TOP], skybox_transform[2], false, true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
		//skybox_transform[3].translation = Vector3(0, 0, 0);
		skybox_transform[3].rotationX.angle = -90;
		skybox_transform[3].rotationZ.angle = 90;
		skybox_transform[3].scaling = Vector3(50.f, 75.f, 50.f);
		RenderObject(meshList[GEO_BOTTOM], skybox_transform[3], false, true);
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
		skybox_transform[4].translation = Vector3(0, 12.5f, -25.f);
		skybox_transform[4].scaling = Vector3(75.f, 25.f, 50.f);
		RenderObject(meshList[GEO_FRONT], skybox_transform[4], false, true);
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
		skybox_transform[5].translation = Vector3(0, 12.5f, 25.f);
		skybox_transform[5].scaling = Vector3(75.f, 25.f, 50.f);
		skybox_transform[5].rotationY.angle = 180;
		RenderObject(meshList[GEO_BACK], skybox_transform[5], false, true);
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

void SceneText::M_Menu()
{
	if (Application::IsKeyPressed(VK_SPACE) && coords.X == 0 && coords.Y == 18) // checks for coords of cursor
		MainMenu = false;
}

void SceneText::P_Menu()
{
	if (Application::IsKeyPressed(VK_SPACE) && coords.X == 0 && coords.Y == 18) // checks for coords of cursor
		PauseMenu = false;
}

void SceneText::ExitGame()
{
	if (Application::IsKeyPressed(VK_SPACE) && coords.X == 0 && coords.Y == 17) // checks for coords of cursor
		Exit();
}