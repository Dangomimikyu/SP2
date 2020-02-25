#ifndef SCENETEXT_H
#define SCENETEXT_H
#define NUM_OBJ 10

#include "Scene.h"
#include <MatrixStack.h>
#include "Camera2.h"
#include "Mesh.h"
#include "Light.h"
#include <transformation.h>
#include "CNPCs.h"
#include "CPlayer.h"
#include "CCollision.h"
#include "CRectangle.h"
#include "CBezier.h"

class SceneText : public Scene
{
	enum GEOMETRY_TYPE
	{
		GEO_AXES = 0,
		GEO_LEFT, 
		GEO_RIGHT, 
		GEO_TOP, 
		GEO_BOTTOM, 
		GEO_FRONT, 
		GEO_BACK,
		GEO_CHAR,
		GEO_DICE,
    GEO_CUBE,
		GEO_LIGHTSPHERE,
		GEO_TEXT,
		GEO_ENV_ARCADE_BUTTON_EXT,
		GEO_ENV_ARCADE_BUTTON_INT,
		GEO_ENV_ARCADE_HEADSET,
		GEO_ENV_JOYSTICK_BASE,
		GEO_ENV_JOYSTICK_CONTROLLER,
		GEO_ENV_ARCADE_MACHINE_B,
		GEO_ENV_ARCADE_MACHINE_G,
		GEO_ENV_ARCADE_MACHINE_P,
		GEO_ENV_CAR_DISPLAY_PLATFORM_1,
		GEO_ENV_CAR_DISPLAY_PLATFORM_2,
		GEO_ENV_CAR_DISPLAY_PLATFORM_3,
		GEO_ENV_CAR_DISPLAY_PLATFORM_4,
		GEO_ENV_CAR_DISPLAY_PLATFORM_5,
		GEO_ENV_COFFEE_MACHINE,
		GEO_ENV_COFFEE_CUP,

		GEO_NPC_BOB_HEAD,
		GEO_NPC_BOB_BODY,
		GEO_NPC_BOB_CLAW,
		GEO_NPC_BOB_LEG_LOWER,
		GEO_NPC_BOB_ARM_LOWER,
		GEO_NPC_BOB_LEG_HIGHER,
		GEO_NPC_BOB_ARM_HIGHER,
		NUM_GEOMETRY,
	};

	enum NPCs
	{
		NPC_BOB_HEAD,
		NPC_BOB_BODY,
		NPC_BOB_CLAW,
		NPC_BOB_LEG_LOWER,
		NPC_BOB_ARM_LOWER,
		NPC_BOB_LEG_HIGHER,
		NPC_BOB_ARM_HIGHER,
		NUM_NPC
	};

	enum env_obj
	{
		ENV_ARCADE_BUTTON_EXT,
		ENV_ARCADE_BUTTON_INT,
		ENV_ARCADE_HEADSET,
		ENV_JOYSTICK_BASE,
		ENV_JOYSTICK_CONTROLLER,
		ENV_ARCADE_MACHINE_B,
		ENV_ARCADE_MACHINE_G,
		ENV_ARCADE_MACHINE_P,
		ENV_CAR_DISPLAY_PLATFORM_1,
		ENV_CAR_DISPLAY_PLATFORM_2,
		ENV_CAR_DISPLAY_PLATFORM_3,
		ENV_CAR_DISPLAY_PLATFORM_4,
		ENV_CAR_DISPLAY_PLATFORM_5,
		ENV_COFFEE_MACHINE,
		ENV_COFFEE_CUP,
		NUM_OBJECTS
	};

	enum Scene5_UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHTENABLED,
		//add these enum in UNIFORM_TYPE before U_TOTAL
		U_LIGHT0_TYPE,
		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,
		U_NUMLIGHTS,
		// add these enum for texture
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		U_TOTAL,
	};

private:
	unsigned m_vertexArrayID;
	unsigned m_programID;
	/*unsigned m_indexBuffer;*/
	Mesh* meshList[NUM_GEOMETRY];

	unsigned m_parameters[U_TOTAL];

	MS modelStack, viewStack, projectionStack;
	Light light[1];

	//CPlayer gamer[1];
	CEntity* gamer[1];
	transform gamer_transform[1];

	//NPC NPCs[NUM_NPC];
	CEntity* NPCs[NUM_NPC];
	transform NPCs_transform[NUM_NPC * 7];

	// objects
	CCollision* objects[10];
	transform obj_transform[NUM_OBJECTS];

	transform skybox_transform[6]; // left = 0, right = 1, up = 2, down = 3, front = 4, back = 5
  
	CPlayer gamer;
	CCollision* cube;
	CCollision* sphere;

	//bezier curve points
	transform playerPos;
	transform spheree;
	transform cubee;
  
	Camera2 camera;
  
	float walkingX; // remove later
	float walkingZ; // remove later
	

	void InitSkybox();
	void InitLights();
	void InitNPCs();
	void InitObjs();

	void RenderMesh(Mesh* mesh, bool enableLight);

	void RenderObject(Mesh* mesh, transform object, bool hierarchical, bool enableLight);
  
	void RenderSkybox();

	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	std::string print_fps();
public:
	SceneText();
	~SceneText();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
};

#endif