#include "SceneMG1.h"
#include "GL\glew.h"
#include "Application.h"
#include <Mtx44.h>
#include "shader.hpp"
#include "MeshBuilder.h"
#include "Utility.h"
#include "LoadTGA.h"
#include <Transformation.h>
#include <math.h>

#define ROT_LIMIT 45.f;
#define SCALE_LIMIT 5.f;
#define LSPEED 10.f

SceneMG1::SceneMG1()
{
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = NULL;
	}
}

SceneMG1::~SceneMG1()
{
}

void SceneMG1::Init()
{
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	distanceX = -5.f;
	distanceZ = -5.f;
	rotateY = 0;
	cube = new CRectangle;
	sphere = new CCollision;
	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);
	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	camera.Init(Vector3(0, 30, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	car1.setSpeed(0);
	car1.setAcceleration(0);
	car1.setRadius(3);

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

	// init skybox ==============================================================================================================
	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1), 1.01f, 1.01f);
	meshList[GEO_LEFT]->textureID = LoadTGA("Image//carTrackLeft.tga");

	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1), 1.01f, 1.01f);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Image//carTrackRight.tga");

	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1), 1.01f, 1.01f);
	meshList[GEO_TOP]->textureID = LoadTGA("Image//carTrackTop.tga");

	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1), 1.01f, 1.01f);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Image//bottom.tga");

	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1), 1.01f, 1.01f);
	meshList[GEO_FRONT]->textureID = LoadTGA("Image//carTrackFront.tga");

	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1), 1.01f, 1.01f);
	meshList[GEO_BACK]->textureID = LoadTGA("Image//carTrackBack.tga");
	// end init skybox =========================================================================================================

	meshList[GEO_CHAR] = MeshBuilder::GenerateQuad("char", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_CHAR]->textureID = LoadTGA("Image//char.tga");


	meshList[GEO_LIGHTSPHERE] = MeshBuilder::GenerateSphere("lightBall", Color(1.f, 1.f, 1.f), 9, 36, 1.f);

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//calibri.tga");

	meshList[GEO_CAR1] = MeshBuilder::GenerateOBJ("car1", "obj//car1.obj");
	meshList[GEO_CAR1]->textureID = LoadTGA("Image//car1.tga");
	car1Transform.scaling = Vector3(.2f, .2f, .2f);

	meshList[GEO_WHEELS1] = MeshBuilder::GenerateOBJ("wheels1", "obj//wheels.obj");
	meshList[GEO_WHEELS1]->textureID = LoadTGA("Image//car1.tga");


	meshList[GEO_DICE] = MeshBuilder::GenerateCuboid("cube", Color(1, 0, 0), 2, 2, 2);

	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("sphere", Color(0, 1.f, 0), 9, 36, 1.f);


	meshList[GEO_RACETRACK] = MeshBuilder::GenerateOBJ("raceTrack", "obj//raceTrack.obj");
	meshList[GEO_RACETRACK]->textureID = LoadTGA("Image//racetrack.tga");
	meshList[GEO_RACETRACK]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_RACETRACK]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_RACETRACK]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_RACETRACK]->material.kShininess = 0.6f;

	obj_transform[GEO_RACETRACK].translation = Vector3(0, 0, 0);
	obj_transform[GEO_RACETRACK].rotationX.angle = 0;
	obj_transform[GEO_RACETRACK].rotationY.angle = 0;
	obj_transform[GEO_RACETRACK].rotationZ.angle = 0;
	obj_transform[GEO_RACETRACK].scaling = Vector3(1.7f, 1.7f, 1.7f);

	meshList[GEO_FENCE_IL] = MeshBuilder::GenerateOBJ("fence_il", "obj//fence.obj");
	meshList[GEO_FENCE_IL]->textureID = LoadTGA("Image//fence.tga");
	meshList[GEO_FENCE_IL]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_FENCE_IL]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_FENCE_IL]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_FENCE_IL]->material.kShininess = 0.6f;
	
	obj_transform[GEO_FENCE_IL].translation = Vector3(-4.7, 1, 0);
	obj_transform[GEO_FENCE_IL].rotationX.angle = 0;
	obj_transform[GEO_FENCE_IL].rotationY.angle = -90;
	obj_transform[GEO_FENCE_IL].rotationZ.angle = 0;
	obj_transform[GEO_FENCE_IL].scaling = Vector3(4.f, 1.f, 1.f);

	meshList[GEO_FENCE_IR] = MeshBuilder::GenerateOBJ("fence_ir", "obj//fence.obj");
	meshList[GEO_FENCE_IR]->textureID = LoadTGA("Image//fence.tga");
	meshList[GEO_FENCE_IR]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_FENCE_IR]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_FENCE_IR]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_FENCE_IR]->material.kShininess = 0.6f;

	obj_transform[GEO_FENCE_IR].translation = Vector3(4.7f, 1, 0);
	obj_transform[GEO_FENCE_IR].rotationX.angle = 0;
	obj_transform[GEO_FENCE_IR].rotationY.angle = 90;
	obj_transform[GEO_FENCE_IR].rotationZ.angle = 0;
	obj_transform[GEO_FENCE_IR].scaling = Vector3(4.f, 1.f, 1.f);

	meshList[GEO_FENCE_IT] = MeshBuilder::GenerateOBJ("fence_it", "obj//fence.obj");
	meshList[GEO_FENCE_IT]->textureID = LoadTGA("Image//fence.tga");
	meshList[GEO_FENCE_IT]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_FENCE_IT]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_FENCE_IT]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_FENCE_IT]->material.kShininess = 0.6f;

	obj_transform[GEO_FENCE_IT].translation = Vector3(0, 1, -7.8f);
	obj_transform[GEO_FENCE_IT].rotationX.angle = 0;
	obj_transform[GEO_FENCE_IT].rotationY.angle = 180;
	obj_transform[GEO_FENCE_IT].rotationZ.angle = 0;
	obj_transform[GEO_FENCE_IT].scaling = Vector3(2.3f, 1.f, 1.f);

	meshList[GEO_FENCE_IB] = MeshBuilder::GenerateOBJ("fence_ib", "obj//fence.obj");
	meshList[GEO_FENCE_IB]->textureID = LoadTGA("Image//fence.tga");
	meshList[GEO_FENCE_IB]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_FENCE_IB]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_FENCE_IB]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_FENCE_IB]->material.kShininess = 0.6f;

	obj_transform[GEO_FENCE_IB].translation = Vector3(0, 1, 7.8f);
	obj_transform[GEO_FENCE_IB].rotationX.angle = 0;
	obj_transform[GEO_FENCE_IB].rotationY.angle = 0;
	obj_transform[GEO_FENCE_IB].rotationZ.angle = 0;
	obj_transform[GEO_FENCE_IB].scaling = Vector3(2.3f, 1.f, 1.f);

	meshList[GEO_FENCE_OL] = MeshBuilder::GenerateOBJ("fence_ol", "obj//fence.obj");
	meshList[GEO_FENCE_OL]->textureID = LoadTGA("Image//fence.tga");
	meshList[GEO_FENCE_OL]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_FENCE_OL]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_FENCE_OL]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_FENCE_OL]->material.kShininess = 0.6f;

	obj_transform[GEO_FENCE_OL].translation = Vector3(-10.1f, 1, 0);
	obj_transform[GEO_FENCE_OL].rotationX.angle = 0;
	obj_transform[GEO_FENCE_OL].rotationY.angle = 90;
	obj_transform[GEO_FENCE_OL].rotationZ.angle = 0;
	obj_transform[GEO_FENCE_OL].scaling = Vector3(4.7f, 1.f, 1.f);
	
	meshList[GEO_FENCE_OR] = MeshBuilder::GenerateOBJ("fence_or", "obj//fence.obj");
	meshList[GEO_FENCE_OR]->textureID = LoadTGA("Image//fence.tga");
	meshList[GEO_FENCE_OR]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_FENCE_OR]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_FENCE_OR]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_FENCE_OR]->material.kShininess = 0.6f;

	obj_transform[GEO_FENCE_OR].translation = Vector3(10.1f, 1, 0);
	obj_transform[GEO_FENCE_OR].rotationX.angle = 0;
	obj_transform[GEO_FENCE_OR].rotationY.angle = -90;
	obj_transform[GEO_FENCE_OR].rotationZ.angle = 0;
	obj_transform[GEO_FENCE_OR].scaling = Vector3(4.7f, 1.f, 1.f);

	meshList[GEO_FENCE_OT] = MeshBuilder::GenerateOBJ("fence_ot", "obj//fence.obj");
	meshList[GEO_FENCE_OT]->textureID = LoadTGA("Image//fence.tga");
	meshList[GEO_FENCE_OT]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_FENCE_OT]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_FENCE_OT]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_FENCE_OT]->material.kShininess = 0.6f;

	obj_transform[GEO_FENCE_OT].translation = Vector3(0, 1, -12.6f);
	obj_transform[GEO_FENCE_OT].rotationX.angle = 0;
	obj_transform[GEO_FENCE_OT].rotationY.angle = 0;
	obj_transform[GEO_FENCE_OT].rotationZ.angle = 0;
	obj_transform[GEO_FENCE_OT].scaling = Vector3(3.5f, 1.f, 1.f);

	meshList[GEO_FENCE_OB] = MeshBuilder::GenerateOBJ("fence_ob", "obj//fence.obj");
	meshList[GEO_FENCE_OB]->textureID = LoadTGA("Image//fence.tga");
	meshList[GEO_FENCE_OB]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_FENCE_OB]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_FENCE_OB]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_FENCE_OB]->material.kShininess = 0.6f;

	obj_transform[GEO_FENCE_OB].translation = Vector3(0, 1, 12.6f);
	obj_transform[GEO_FENCE_OB].rotationX.angle = 0;
	obj_transform[GEO_FENCE_OB].rotationY.angle = 180;
	obj_transform[GEO_FENCE_OB].rotationZ.angle = 0;
	obj_transform[GEO_FENCE_OB].scaling = Vector3(3.5f, 1.f, 1.f);

	meshList[GEO_FENCE_TLD] = MeshBuilder::GenerateOBJ("fence_tld", "obj//fence.obj");
	meshList[GEO_FENCE_TLD]->textureID = LoadTGA("Image//fence.tga");
	meshList[GEO_FENCE_TLD]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_FENCE_TLD]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_FENCE_TLD]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_FENCE_TLD]->material.kShininess = 0.6f;

	obj_transform[GEO_FENCE_TLD].translation = Vector3(-8.6f, 1, -11.f);
	obj_transform[GEO_FENCE_TLD].rotationX.angle = 0;
	obj_transform[GEO_FENCE_TLD].rotationY.angle = 45;
	obj_transform[GEO_FENCE_TLD].rotationZ.angle = 0;
	obj_transform[GEO_FENCE_TLD].scaling = Vector3(1.2f, 1.f, 1.f);

	meshList[GEO_FENCE_TRD] = MeshBuilder::GenerateOBJ("fence_trd", "obj//fence.obj");
	meshList[GEO_FENCE_TRD]->textureID = LoadTGA("Image//fence.tga");
	meshList[GEO_FENCE_TRD]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_FENCE_TRD]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_FENCE_TRD]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_FENCE_TRD]->material.kShininess = 0.6f;

	obj_transform[GEO_FENCE_TRD].translation = Vector3(8.6f, 1, -11.f);
	obj_transform[GEO_FENCE_TRD].rotationX.angle = 0;
	obj_transform[GEO_FENCE_TRD].rotationY.angle = -45;
	obj_transform[GEO_FENCE_TRD].rotationZ.angle = 0;
	obj_transform[GEO_FENCE_TRD].scaling = Vector3(1.2f, 1.f, 1.f);

	meshList[GEO_FENCE_BLD] = MeshBuilder::GenerateOBJ("fence_bld", "obj//fence.obj");
	meshList[GEO_FENCE_BLD]->textureID = LoadTGA("Image//fence.tga");
	meshList[GEO_FENCE_BLD]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_FENCE_BLD]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_FENCE_BLD]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_FENCE_BLD]->material.kShininess = 0.6f;

	obj_transform[GEO_FENCE_BLD].translation = Vector3(-8.6f, 1, 11.f);
	obj_transform[GEO_FENCE_BLD].rotationX.angle = 0;
	obj_transform[GEO_FENCE_BLD].rotationY.angle = 135;
	obj_transform[GEO_FENCE_BLD].rotationZ.angle = 0;
	obj_transform[GEO_FENCE_BLD].scaling = Vector3(1.2f, 1.f, 1.f);

	meshList[GEO_FENCE_BRD] = MeshBuilder::GenerateOBJ("fence_brd", "obj//fence.obj");
	meshList[GEO_FENCE_BRD]->textureID = LoadTGA("Image//fence.tga");
	meshList[GEO_FENCE_BRD]->material.kAmbient.Set(1, 1, 1);
	meshList[GEO_FENCE_BRD]->material.kDiffuse.Set(1, 1, 1);
	meshList[GEO_FENCE_BRD]->material.kSpecular.Set(1, 1, 1);
	meshList[GEO_FENCE_BRD]->material.kShininess = 0.6f;

	obj_transform[GEO_FENCE_BRD].translation = Vector3(8.6f, 1, 11.f);
	obj_transform[GEO_FENCE_BRD].rotationX.angle = 0;
	obj_transform[GEO_FENCE_BRD].rotationY.angle = -135;
	obj_transform[GEO_FENCE_BRD].rotationZ.angle = 0;
	obj_transform[GEO_FENCE_BRD].scaling = Vector3(1.2f, 1.f, 1.f);
}

void SceneMG1::Update(double dt)
{
	/*cube->set_transformation('t', Vector3(cubeTransform.translation.x, 0, cubeTransform.translation.z));*/
	static_cast<CRectangle*>(cube)->RectCollision(car1, car1Transform, 10, 10, 10, 10);
	sphere->set_transformation('t', Vector3(sphereTransform.translation.x, 0, sphereTransform.translation.z));
	sphere->roundCollision(car1, car1Transform, 2);
	
	car1.setRadius(3);

	CCollision* object[2] = { cube, sphere };
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
	if (Application::IsKeyPressed('V') && car1.getSpeed() != 0)
	{
		rotateY += (float)(2);
	}
	if (Application::IsKeyPressed('N') && car1.getSpeed() != 0)
	{
		rotateY -= (float)(2);
	}
	if (Application::IsKeyPressed('G'))
	{

		if (car1.getSpeed() < 50 && car1.getAcceleration() >= 0) {
			car1.setSpeed(10);
			car1.setAcceleration(car1.getAcceleration() + (0.1f));
			car1.setSpeed(car1.getSpeed());
			distanceZ += (float)((car1.getSpeed() * dt) * sin(Math::DegreeToRadian(-rotateY)));
			distanceX += (float)((car1.getSpeed() * dt) * cos(Math::DegreeToRadian(-rotateY)));
		}
		else {
			distanceZ += (float)((car1.getSpeed() * dt) * sin(Math::DegreeToRadian(-rotateY)));
			distanceX += (float)((car1.getSpeed() * dt) * cos(Math::DegreeToRadian(-rotateY)));
		}
	}
	else {
		if (Application::IsKeyPressed('G') == false && car1.getSpeed() > 0 && Application::IsKeyPressed('B') == false)
		{
			car1.setAcceleration(-0.1);
			car1.setSpeed(car1.getSpeed());
			distanceZ += (float)((car1.getSpeed() * dt) * sin(Math::DegreeToRadian(-rotateY)));
			distanceX += (float)((car1.getSpeed() * dt) * cos(Math::DegreeToRadian(-rotateY)));
		}
		if (car1.getAcceleration() < 0)
		{
			car1.setAcceleration(0);
		}
		if (car1.getSpeed() < 0 && Application::IsKeyPressed('B') == false)
		{
			car1.setSpeed(0);
		}
	}
	if (Application::IsKeyPressed('B'))
	{
		if (car1.getAcceleration() > -5 && car1.getSpeed() > -5)
		{
			car1.setAcceleration(car1.getAcceleration() - 0.5f);
			car1.setSpeed(car1.getSpeed() - 0.05);
		}
		distanceZ += (float)((car1.getSpeed() * dt) * sin(Math::DegreeToRadian(-rotateY)));
		distanceX += (float)((car1.getSpeed() * dt) * cos(Math::DegreeToRadian(-rotateY)));

	}

	for(int i =0 ; i < 2; i ++)
	{ 

		 if (object[i]->getCollide() == true )
		{
			/*if (Application::IsKeyPressed('G'))
			{
				car1.setSpeed(0);
				car1.setAcceleration(0);
				distanceX += object[i]->getOverlap() * (car1Transform.translation.x - object[i]->get_transformation().translation.x) / object[i]->getDistance();
				distanceZ += object[i]->getOverlap() * (car1Transform.translation.z - object[i]->get_transformation().translation.z) / object[i]->getDistance();
			}
			
			if (Application::IsKeyPressed('B'))
			{
				car1.setSpeed(0);
				car1.setAcceleration(0);
				distanceX += object[i]->getOverlap() * (car1Transform.translation.x - object[i]->get_transformation().translation.x) / object[i]->getDistance();
				distanceZ += object[i]->getOverlap() * (car1Transform.translation.z - object[i]->get_transformation().translation.z) / object[i]->getDistance();
			}*/
			 distanceX += object[i]->getOverlap() * (car1Transform.translation.x - object[i]->get_transformation().translation.x)  / object[0]->getDistance() ;
			 distanceZ += object[i]->getOverlap() * (car1Transform.translation.z - object[i]->get_transformation().translation.z)   / object[0]->getDistance() ;
			  car1.setSpeed(0);
			  car1.setAcceleration(0);
			
		}
	}
	/*if (Application::IsKeyPressed('B'))
	{
		if (car1.getSpeed() > 3)
		{
			car1.setAcceleration((car1.getAcceleration() - 0.03));
			car1.setSpeed(car1.getSpeed());
		}
		distanceZ += car1.getSpeed() * dt;
	}*/


	camera.Update(dt);
}

void SceneMG1::Render()
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

	//raceTrack
	RenderObject(meshList[GEO_RACETRACK], obj_transform[GEO_RACETRACK], true, false);
	RenderObject(meshList[GEO_FENCE_IL], obj_transform[GEO_FENCE_IL], false, false);
	RenderObject(meshList[GEO_FENCE_IR], obj_transform[GEO_FENCE_IR], false, false);
	RenderObject(meshList[GEO_FENCE_IT], obj_transform[GEO_FENCE_IT], false, false);
	RenderObject(meshList[GEO_FENCE_IB], obj_transform[GEO_FENCE_IB], false, false);
	RenderObject(meshList[GEO_FENCE_OL], obj_transform[GEO_FENCE_OL], false, false);
	RenderObject(meshList[GEO_FENCE_OR], obj_transform[GEO_FENCE_OR], false, false);
	RenderObject(meshList[GEO_FENCE_OT], obj_transform[GEO_FENCE_OT], false, false);
	RenderObject(meshList[GEO_FENCE_OB], obj_transform[GEO_FENCE_OB], false, false);
	RenderObject(meshList[GEO_FENCE_TLD], obj_transform[GEO_FENCE_TLD], false, false);
	RenderObject(meshList[GEO_FENCE_TRD], obj_transform[GEO_FENCE_TRD], false, false);
	RenderObject(meshList[GEO_FENCE_BLD], obj_transform[GEO_FENCE_BLD], false, false);
	RenderObject(meshList[GEO_FENCE_BRD], obj_transform[GEO_FENCE_BRD], false, false);
	modelStack.PopMatrix();
	// dice
	//transform dice;
	//dice.translation = Vector3(0, -1, 0);
	//dice.rotateAngle = 45;
	//dice.rotation = Vector3(1, 0, 0);
	//dice.scaling = Vector3(5, 5, 5);

	// dice end

	// world text
	modelStack.PushMatrix();
	RenderText(meshList[GEO_TEXT], "THIS IS MG1", Color(0, 1, 0));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	car1Transform.translation = Vector3(distanceX , 0, distanceZ + 2);
	car1Transform.rotationY.angle = rotateY + 90;
	RenderObject(meshList[GEO_CAR1], car1Transform, true, true);
	RenderMesh(meshList[GEO_WHEELS1], true);
	modelStack.PopMatrix();
	//on-screen text
	/*RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(distanceX) + "+" + std::to_string(distanceZ), Color(1, 0, 1), 3, 0, 0);*/
	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(cube->getDistance()), Color(1, 0, 1), 3, 0, 0);
	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(car1.getAcceleration()), Color(1, 0, 1), 3, 0, 2);

	modelStack.PushMatrix();
	cubeTransform.translation = Vector3(cube->get_transformation().translation.x, 0, cube->get_transformation().translation.z);
	RenderObject(meshList[GEO_DICE], cubeTransform, false, true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	sphereTransform.translation = Vector3(0.f, 0, 25.f);
	RenderObject(meshList[GEO_SPHERE], sphereTransform, false, true);

	modelStack.PushMatrix();
	if (cube->getCollide() == false) {
		RenderTextOnScreen(meshList[GEO_TEXT], "Collision false", Color(0, 1, 0), 2, 0, 1);
	}
	else {
		RenderTextOnScreen(meshList[GEO_TEXT], "Collision true", Color(0, 1, 0), 2, 0, 1);
	}
	modelStack.PopMatrix();
}

void SceneMG1::Exit()
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

void SceneMG1::RenderMesh(Mesh* mesh, bool enableLight)
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

void SceneMG1::RenderObject(Mesh* mesh, transform object, bool hierarchical, bool enableLight)
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

void SceneMG1::RenderSkybox()
{
	modelStack.PushMatrix();
	skybox_transform[0].translation = Vector3(-25.f, 25.f, 0);
	skybox_transform[0].scaling = Vector3(50.f, 50.f, 50.f);
	skybox_transform[0].rotationY.angle = 90;
	skybox_transform[0].rotationX.angle = 90;
	RenderObject(meshList[GEO_LEFT], skybox_transform[0], false, false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	skybox_transform[1].translation = Vector3(25.f, 25.f, 0);
	skybox_transform[1].scaling = Vector3(50.f, 50.f, 50.f);
	skybox_transform[1].rotationY.angle = -90;
	skybox_transform[1].rotationX.angle = -90;
	RenderObject(meshList[GEO_RIGHT], skybox_transform[1], false, false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	skybox_transform[2].translation = Vector3(0, 50.f, 0);
	skybox_transform[2].scaling = Vector3(50.f, 50.f, 50.f);
	skybox_transform[2].rotationX.angle = 90;
	skybox_transform[2].rotationZ.angle = 90;
	RenderObject(meshList[GEO_TOP], skybox_transform[2], false, false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	//skybox_transform[3].translation = Vector3(0, 0, 0);
	skybox_transform[3].rotationX.angle = -90;
	skybox_transform[3].rotationZ.angle = 90;
	skybox_transform[3].scaling = Vector3(50.f, 50.f, 50.f);
	RenderObject(meshList[GEO_BOTTOM], skybox_transform[3], false, false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	skybox_transform[4].translation = Vector3(0, 25.f, -25.f);
	skybox_transform[4].scaling = Vector3(50.f, 50.f, 50.f);
	skybox_transform[4].rotationZ.angle = 90;
	RenderObject(meshList[GEO_FRONT], skybox_transform[4], false, false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	skybox_transform[5].translation = Vector3(0, 25.f, 25.f);
	skybox_transform[5].scaling = Vector3(50.f, 50.f, 50.f);
	skybox_transform[5].rotationY.angle = 180;
	skybox_transform[5].rotationZ.angle = -90;
	RenderObject(meshList[GEO_BACK], skybox_transform[5], false, false);
	modelStack.PopMatrix();
}

std::string SceneMG1::print_fps()
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

void SceneMG1::RenderText(Mesh* mesh, std::string text, Color color)
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

void SceneMG1::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
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