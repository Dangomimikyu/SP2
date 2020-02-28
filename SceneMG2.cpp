#include "SceneMG2.h"
#include "GL\glew.h"
#include "Application.h"
#include <Mtx44.h>
#include "shader.hpp"
#include "MeshBuilder.h"
#include "Utility.h"
#include "LoadTGA.h"
#include <Transformation.h>
#include <ctime>

#define ROT_LIMIT 45.f;
#define SCALE_LIMIT 5.f;
#define LSPEED 10.f

SceneMG2::SceneMG2()
{
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = NULL;
	}
}

SceneMG2::~SceneMG2()
{
}

void SceneMG2::Init()
{
	//srand(time(0));

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);
	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	camera.Init(Vector3(0, 15, 15), Vector3(0, 0, 0), Vector3(0, 1, 0));

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

	//Get a handle for our "colorTexture" uniform
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");
	// Get a handle for our "textColor" uniform
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");


	glUseProgram(m_programID);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	light[0].type = Light::LIGHT_POINT;
	light[0].position.Set(0, 10, 0);
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

	// init skybox ==============================================================================================================
	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_LEFT]->textureID = LoadTGA("Image//left.tga");

	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Image//right.tga");

	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_TOP]->textureID = LoadTGA("Image//top.tga");

	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Image//bottom.tga");

	meshList[ROBOFLOOR] = MeshBuilder::GenerateQuad("Grass", Color(1, 1, 1), 1.f, 1.f);
	meshList[ROBOFLOOR]->textureID = LoadTGA("Image//RoboMole_Floor.tga");

	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_FRONT]->textureID = LoadTGA("Image//front.tga");

	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_BACK]->textureID = LoadTGA("Image//back.tga");
	// end init skybox =========================================================================================================

	meshList[ROBORING] = MeshBuilder::GenerateOBJ("Ring", "OBJ//RoboMole_Ring.obj");
	meshList[ROBORING]->textureID = LoadTGA("Image//RoboMole_Ring.tga");

	meshList[ROBOMOLE] = MeshBuilder::GenerateOBJ("Mole", "OBJ//RoboMole.obj");
	meshList[ROBOMOLE]->textureID = LoadTGA("Image//RoboMole.tga");

	meshList[ROBOHAMMER] = MeshBuilder::GenerateOBJ("Phage", "OBJ//RoboMole_Hammer.obj");
	meshList[ROBOHAMMER]->textureID = LoadTGA("Image//RoboMole_Hammer.tga");

	meshList[GEO_LIGHTSPHERE] = MeshBuilder::GenerateSphere("lightBall", Color(1.f, 1.f, 1.f), 9, 36, 1.f);

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//calibri.tga");

	//init moles' positions
	RoboMole_transform[0].translation = Vector3(-5.f, -2.f, -5.f);
	RoboMole_transform[1].translation = Vector3(0.f, -2.f, -5.f);
	RoboMole_transform[2].translation = Vector3(5.f, -2.f, -5.f);
	RoboMole_transform[3].translation = Vector3(-5.f, -2.f, 0.f);
	RoboMole_transform[4].translation = Vector3(0.f, -2.f, 0.f);
	RoboMole_transform[5].translation = Vector3(5.f, -2.f, 0.f);
	RoboMole_transform[6].translation = Vector3(-5.f, -2.f, 5.f);
	RoboMole_transform[7].translation = Vector3(0.f, -2.f, 5.f);
	RoboMole_transform[8].translation = Vector3(5.f, -2.f, 5.f);

	hammer.translation = Vector3(2.5f, 1.f, 2.5f);
	hammer.rotationY.angle = 45;
	hammer.rotationX.angle = 45;
	HammerAnimation = 7.5;

	ElapsedTime = 0;
	BounceTime = 0;
	GameTime = 30.f;
}

void SceneMG2::Update(double dt)
{
	if (Application::IsKeyPressed(0x31))
	{
		glDisable(GL_CULL_FACE);
	}
	else if (Application::IsKeyPressed(0x32))
	{
		glEnable(GL_CULL_FACE);
	}
	else if (Application::IsKeyPressed(0x33))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if (Application::IsKeyPressed(0x34))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	switch (GameStart)
	{
		case false:
			if (Application::IsKeyPressed('G') && GameEnd == false)
				GameStart = true;
			break;

		case true:
			ElapsedTime += dt;
			GameTime -= dt;

			UpdateGame(dt);

			UpdateHammerLocation(dt);
			UpdateHammerAnimation();
	}

	if (ElapsedTime > 30.f)
	{
		GameEnd = true;
		GameStart = false;
	}

}

void SceneMG2::UpdateGame(double dt)
{
	mRand = robomole.MoleRand();
	std::cout << mRand << std::endl;

	if (RoboMole_transform[mRand].translation.y < 0)
	{
		RoboMole_transform[mRand].translation.y += (float)(10 *dt);
	}

	if (HammerLocation == mRand)
	{
		if (BounceTime > ElapsedTime)
			return;
		else
		{
			robomole.MoleCheck(true);
			BounceTime = ElapsedTime + 0.05f;
		}

		if (RoboMole_transform[mRand].translation.y > -2)
			RoboMole_transform[mRand].translation.y -= (float)(80 * dt);
	}
}

void SceneMG2::UpdateHammerAnimation()
{
	if (Application::IsKeyPressed('R') || Application::IsKeyPressed('T') || Application::IsKeyPressed('Y') ||
		Application::IsKeyPressed('F') || Application::IsKeyPressed('G') || Application::IsKeyPressed('H') ||
		Application::IsKeyPressed('V') || Application::IsKeyPressed('B') || Application::IsKeyPressed('N'))
	{
		hammer.rotationX.angle -= HammerAnimation;
		if (hammer.rotationX.angle == -15 || hammer.rotationX.angle == 45)
			HammerAnimation = -HammerAnimation;
	}
}

void SceneMG2::UpdateHammerLocation(double dt)
{

	if (Application::IsKeyPressed('R'))
	{
		hammer.translation = Vector3(-2.5f, 1.f, -2.5f);
		HammerLocation = 0;
	}

	if (Application::IsKeyPressed('T'))
	{
		hammer.translation = Vector3(2.5f, 1.f, -2.5f);
		HammerLocation = 1;
	}

	if (Application::IsKeyPressed('Y'))
	{
		hammer.translation = Vector3(7.5f, 1.f, -2.5f);
		HammerLocation = 2;
	}

	if (Application::IsKeyPressed('F'))
	{
		hammer.translation = Vector3(-2.5f, 1.f, 2.5f);
		HammerLocation = 3;
	}

	if (Application::IsKeyPressed('G'))
	{
		hammer.translation = Vector3(2.5f, 1.f, 2.5f);
		HammerLocation = 4;
	}

	if (Application::IsKeyPressed('H'))
	{
		hammer.translation = Vector3(7.5f, 1.f, 2.5f);
		HammerLocation = 5;
	}

	if (Application::IsKeyPressed('V'))
	{
		hammer.translation = Vector3(-2.5f, 1.f, 7.5f);
		HammerLocation = 6;
	}

	if (Application::IsKeyPressed('B'))
	{
		hammer.translation = Vector3(2.5f, 1.f, 7.5f);
		HammerLocation = 7;
	}

	if (Application::IsKeyPressed('N'))
	{
		hammer.translation = Vector3(7.5f, 1.f, 7.5f);
		HammerLocation = 8;
	}

}

void SceneMG2::Render()
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

	//on-screen text
	RenderTextOnScreen(meshList[GEO_TEXT], "Current FPS:" + print_fps(), Color(1, 1, 1), 3, 0, 0);

	if (GameStart == false && GameEnd == false)
		RenderTextOnScreen(meshList[GEO_TEXT], "Press G to Start", Color(1, 1, 1), 3, 6, 8);

	if (GameEnd == true)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "You earned $" + std::to_string(robomole.TotalScore() / 5), Color(1, 1, 1), 3, 0, 2);
		RenderTextOnScreen(meshList[GEO_TEXT], "from " + std::to_string(robomole.TotalScore()) + " points.", Color(1, 1, 1), 3, 0, 1);
	}

	if (GameStart == true)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(GameTime), Color(1, 1, 1), 3, 0, 2);
		RenderTextOnScreen(meshList[GEO_TEXT], "Total Score:" + std::to_string(robomole.TotalScore()), Color(1, 1, 1), 3, 0, 1);

	}

	modelStack.PushMatrix();
	RenderMesh(meshList[ROBOHAMMER], hammer, false);
	modelStack.PopMatrix();

	RenderRoboRings();
	RenderRoboMoles();
}

void SceneMG2::Exit()
{
	robomole.MoneyEarned();
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

void SceneMG2::RenderMesh(Mesh* mesh, bool enableLight)
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
}

void SceneMG2::RenderMesh(Mesh* mesh, transform object, bool enableLight)
{
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
}

void SceneMG2::RenderObject(Mesh* mesh, transform object,bool hierarchical, bool enableLight)
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

void SceneMG2::RenderSkybox()
{
	skybox_transform[0].translation = Vector3(-50.f, 25.f, 0);
	skybox_transform[0].scaling = Vector3(100.f, 100.f, 100.f);
	skybox_transform[0].rotationY.angle = 90;
	RenderObject(meshList[GEO_LEFT], skybox_transform[0], false, false);

	skybox_transform[1].translation = Vector3(50.f, 25.f, 0);
	skybox_transform[1].scaling = Vector3(100.f, 100.f, 100.f);
	skybox_transform[1].rotationY.angle = -90;
	RenderObject(meshList[GEO_RIGHT], skybox_transform[1], false, false);

	skybox_transform[2].translation = Vector3(0, 50.f, 0);
	skybox_transform[2].scaling = Vector3(100.f, 100.f, 100.f);
	skybox_transform[2].rotationX.angle = 90;
	skybox_transform[2].rotationZ.angle = 90;
	RenderObject(meshList[GEO_TOP], skybox_transform[2], false, false);

	skybox_transform[3].translation = Vector3(0, 0, 0);
	skybox_transform[3].scaling = Vector3(100.f, 100.f, 100.f);
	skybox_transform[3].rotationX.angle = -90;
	skybox_transform[3].rotationZ.angle = 90;
	RenderObject(meshList[ROBOFLOOR], skybox_transform[3], false, false);

	skybox_transform[4].translation = Vector3(0, 25.f, -50.f);
	skybox_transform[4].scaling = Vector3(100.f, 100.f, 100.f);
	RenderObject(meshList[GEO_FRONT], skybox_transform[4], false, false);

	skybox_transform[5].translation = Vector3(0, 25.f, 50.f);
	skybox_transform[5].scaling = Vector3(100.f, 100.f, 100.f);
	skybox_transform[5].rotationY.angle = 180;
	RenderObject(meshList[GEO_BACK], skybox_transform[5], false, false);
}

std::string SceneMG2::print_fps()
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

void SceneMG2::RenderText(Mesh* mesh, std::string text, Color color)
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

void SceneMG2::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
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
		Mtx44 char_spacing;
		char_spacing.SetToTranslation(i * 1.0f, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * char_spacing;
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

void SceneMG2::RenderRoboRings()
{
	RoboRing_transform[0].translation = Vector3(-5.f, 0.f, -5.f);
	RoboRing_transform[0].rotationY.angle = 90;
	RenderObject(meshList[ROBORING], RoboRing_transform[0], false, false);

	RoboRing_transform[1].translation = Vector3(0.f, 0.f, -5.f);
	RoboRing_transform[1].rotationY.angle = 90;
	RenderObject(meshList[ROBORING], RoboRing_transform[1], false, false);

	RoboRing_transform[2].translation = Vector3(5.f, 0.f, -5.f);
	RoboRing_transform[2].rotationY.angle = 90;
	RenderObject(meshList[ROBORING], RoboRing_transform[2], false, false);

	RoboRing_transform[3].translation = Vector3(-5.f, 0.f, 0.f);
	RoboRing_transform[3].rotationY.angle = 90;
	RenderObject(meshList[ROBORING], RoboRing_transform[3], false, false);

	RoboRing_transform[4].translation = Vector3(0.f, 0.f, 0.f);
	RoboRing_transform[4].rotationY.angle = 90;
	RenderObject(meshList[ROBORING], RoboRing_transform[4], false, false);
	
	RoboRing_transform[5].translation = Vector3(5.f, 0.f, 0.f);
	RoboRing_transform[5].rotationY.angle = 90;
	RenderObject(meshList[ROBORING], RoboRing_transform[5], false, false);
	
	RoboRing_transform[6].translation = Vector3(-5.f, 0.f, 5.f);
	RoboRing_transform[6].rotationY.angle = 90;
	RenderObject(meshList[ROBORING], RoboRing_transform[6], false, false);
	
	RoboRing_transform[7].translation = Vector3(0.f, 0.f, 5.f);
	RoboRing_transform[7].rotationY.angle = 90;
	RenderObject(meshList[ROBORING], RoboRing_transform[7], false, false);
	
	RoboRing_transform[8].translation = Vector3(5.f, 0.f, 5.f);
	RoboRing_transform[8].rotationY.angle = 90;
	RenderObject(meshList[ROBORING], RoboRing_transform[8], false, false);
}

void SceneMG2::RenderRoboMoles()
{
	RenderObject(meshList[ROBOMOLE], RoboMole_transform[0], false, false);
	RenderObject(meshList[ROBOMOLE], RoboMole_transform[1], false, false);
	RenderObject(meshList[ROBOMOLE], RoboMole_transform[2], false, false);
	RenderObject(meshList[ROBOMOLE], RoboMole_transform[3], false, false);
	RenderObject(meshList[ROBOMOLE], RoboMole_transform[4], false, false);
	RenderObject(meshList[ROBOMOLE], RoboMole_transform[5], false, false);
	RenderObject(meshList[ROBOMOLE], RoboMole_transform[6], false, false);
	RenderObject(meshList[ROBOMOLE], RoboMole_transform[7], false, false);
	RenderObject(meshList[ROBOMOLE], RoboMole_transform[8], false, false);
}