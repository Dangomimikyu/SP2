#ifndef SCENETEXT_H
#define SCENETEXT_H
#define NUM_OBJ 10

#include "Scene.h"
#include <MatrixStack.h>
#include "Camera2.h"
#include "Mesh.h"
#include "Light.h"
#include <transformation.h>
#include <Windows.h>
#include "CMoney.h"
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

		GEO_TELEPORTER,
		GEO_TELEPORTER2,
		GEO_ENV_ARCADE_BUTTON_EXT_B,
		GEO_ENV_ARCADE_BUTTON_INT_B,

		GEO_ENV_ARCADE_BUTTON_EXT1,
		GEO_ENV_ARCADE_BUTTON_INT1,
		GEO_ENV_ARCADE_BUTTON_EXT2,
		GEO_ENV_ARCADE_BUTTON_INT2,
		GEO_ENV_ARCADE_BUTTON_EXT3,
		GEO_ENV_ARCADE_BUTTON_INT3,
		GEO_ENV_ARCADE_BUTTON_EXT4,
		GEO_ENV_ARCADE_BUTTON_INT4,
		GEO_ENV_ARCADE_BUTTON_EXT5,
		GEO_ENV_ARCADE_BUTTON_INT5,
		GEO_ENV_ARCADE_BUTTON_EXT6,
		GEO_ENV_ARCADE_BUTTON_INT6,
		GEO_ENV_ARCADE_BUTTON_EXT7,
		GEO_ENV_ARCADE_BUTTON_INT7,
		GEO_ENV_ARCADE_BUTTON_EXT8,
		GEO_ENV_ARCADE_BUTTON_INT8,
		GEO_ENV_ARCADE_BUTTON_EXT9,
		GEO_ENV_ARCADE_BUTTON_INT9,

		GEO_ENV_ARCADE_BUTTON_EXT_P1,
		GEO_ENV_ARCADE_BUTTON_INT_P1,
		GEO_ENV_ARCADE_BUTTON_EXT_P2,
		GEO_ENV_ARCADE_BUTTON_INT_P2,
		GEO_ENV_ARCADE_BUTTON_EXT_P3,
		GEO_ENV_ARCADE_BUTTON_INT_P3,
		GEO_ENV_ARCADE_BUTTON_EXT_P4,
		GEO_ENV_ARCADE_BUTTON_INT_P4,

		GEO_ENV_ARCADE_HEADSET,
		GEO_ENV_JOYSTICK_BASE1,
		GEO_ENV_JOYSTICK_CONTROLLER1,
		GEO_ENV_JOYSTICK_BASE2,
		GEO_ENV_JOYSTICK_CONTROLLER2,
		GEO_ENV_ARCADE_MACHINE_B,
		GEO_ENV_ARCADE_MACHINE_G,
		GEO_ENV_ARCADE_MACHINE_P,
		GEO_ENV_CAR_DISPLAY_PLATFORM_1,
		GEO_ENV_CAR_DISPLAY_PLATFORM_2,
		GEO_ENV_CAR_DISPLAY_PLATFORM_3,
		GEO_ENV_CAR_DISPLAY_PLATFORM_4,
		GEO_ENV_CAR_DISPLAY_PLATFORM_5,
		GEO_ENV_SIGNPOST_1,
		GEO_ENV_SIGNPOST_2,
		GEO_ENV_SIGNPOST_3,
		GEO_ENV_SIGNPOST_4,
		GEO_ENV_SIGNPOST_5,
		GEO_ENV_COFFEE_MACHINE,
		GEO_ENV_COFFEE_CUP,
		GEO_ENV_GACHAEGG,

		GEO_NPC_BOB_HEAD,
		GEO_NPC_BOB_BODY,
		GEO_NPC_BOB_CLAW,
		GEO_NPC_BOB_LEG_LOWER,
		GEO_NPC_BOB_ARM_LOWER,
		GEO_NPC_BOB_LEG_HIGHER,
		GEO_NPC_BOB_ARM_HIGHER,

		// NPC GEOMETRY============
		GEO_NPCHEAD,
		GEO_NPCBODY,
		GEO_NPCUPPERARM,
		GEO_NPCLOWERARM,
		GEO_NPCCLAW,
		GEO_NPCUPPERLEG,
		GEO_NPCLOWERLEG,
		//==========================
		GEO_TETIME,

		GEO_CAR_KEN,
		GEO_WHEELS_KEN,
		GEO_CAR_JOSQUIN,
		GEO_WHEELS_JOSQUIN,
		GEO_CAR_KLYDE,
		GEO_WHEELS_KLYDE,
		GEO_CAR_SHAHIR,
		GEO_WHEELS_SHAHIR,
		GEO_CAR_EMILY,
		GEO_WHEELS_EMILY,

		NUM_GEOMETRY,
	};
	
	enum NPCs
	{
		NPC_BOB,
		NPC_EINS,
		NUM_NPC
	};

	enum NPC_body
	{
		NPC_BOB_HEAD,
		NPC_BOB_BODY,
		NPC_BOB_CLAW,
		NPC_BOB_LEG_LOWER,
		NPC_BOB_ARM_LOWER,
		NPC_BOB_LEG_HIGHER,
		NPC_BOB_ARM_HIGHER,

		//NPC TRANFORMS===============
		T_NPCHEAD,
		T_NPCBODY,
		T_NPCLUPPERARM,
		T_NPCLLOWERARM,
		T_NPCLFCLAW,
		T_NPCLBCLAW,
		T_NPCRUPPERARM,
		T_NPCRLOWERARM,
		T_NPCRFCLAW,
		T_NPCRBCLAW,
		T_NPCLUPPERLEG,
		T_NPCLLOWERLEG,
		T_NPCRUPPERLEG,
		T_NPCRLOWERLEG,
		//=============================

		NUM_NPC_PARTS
	};

	enum env_obj
	{
		ENV_TELEPORTER,
		ENV_TELEPORTER2,
		ENV_ARCADE_BUTTON_EXT_B,
		ENV_ARCADE_BUTTON_INT_B,

		ENV_ARCADE_BUTTON_EXT1,
		ENV_ARCADE_BUTTON_INT1,
		ENV_ARCADE_BUTTON_EXT2,
		ENV_ARCADE_BUTTON_INT2,
		ENV_ARCADE_BUTTON_EXT3,
		ENV_ARCADE_BUTTON_INT3,
		ENV_ARCADE_BUTTON_EXT4,
		ENV_ARCADE_BUTTON_INT4,
		ENV_ARCADE_BUTTON_EXT5,
		ENV_ARCADE_BUTTON_INT5,
		ENV_ARCADE_BUTTON_EXT6,
		ENV_ARCADE_BUTTON_INT6,
		ENV_ARCADE_BUTTON_EXT7,
		ENV_ARCADE_BUTTON_INT7,
		ENV_ARCADE_BUTTON_EXT8,
		ENV_ARCADE_BUTTON_INT8,
		ENV_ARCADE_BUTTON_EXT9,
		ENV_ARCADE_BUTTON_INT9,

		ENV_CAR_KEN,
		ENV_CAR_JOSQUIN,
		ENV_CAR_KLYDE,
		ENV_CAR_SHAHIR,
		ENV_CAR_EMILY,
		ENV_WHEELS_EMILY,
		ENV_WHEELS_KEN,
		ENV_WHEELS_JOSQUIN,
		ENV_WHEELS_KLYDE,
		ENV_WHEELS_SHAHIR,

		ENV_ARCADE_BUTTON_EXT_P1,
		ENV_ARCADE_BUTTON_INT_P1,
		ENV_ARCADE_BUTTON_EXT_P2,
		ENV_ARCADE_BUTTON_INT_P2,
		ENV_ARCADE_BUTTON_EXT_P3,
		ENV_ARCADE_BUTTON_INT_P3,
		ENV_ARCADE_BUTTON_EXT_P4,
		ENV_ARCADE_BUTTON_INT_P4,

		ENV_ARCADE_HEADSET,
		ENV_JOYSTICK_BASE1,
		ENV_JOYSTICK_CONTROLLER1,
		ENV_JOYSTICK_BASE2,
		ENV_JOYSTICK_CONTROLLER2, 
		ENV_ARCADE_MACHINE_B,
		ENV_ARCADE_MACHINE_G,
		ENV_ARCADE_MACHINE_P,
		ENV_CAR_DISPLAY_PLATFORM_1,
		ENV_CAR_DISPLAY_PLATFORM_2,
		ENV_CAR_DISPLAY_PLATFORM_3,
		ENV_CAR_DISPLAY_PLATFORM_4,
		ENV_CAR_DISPLAY_PLATFORM_5,
		ENV_SIGNPOST_1,
		ENV_SIGNPOST_2,
		ENV_SIGNPOST_3,
		ENV_SIGNPOST_4,
		ENV_SIGNPOST_5,
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

		U_LIGHT_CEILING1_POSITION,
		U_LIGHT_CEILING1_COLOR,
		U_LIGHT_CEILING1_POWER,
		U_LIGHT_CEILING1_KC,
		U_LIGHT_CEILING1_KL,
		U_LIGHT_CEILING1_KQ,

		U_LIGHT_CEILING2_POSITION,
		U_LIGHT_CEILING2_COLOR,
		U_LIGHT_CEILING2_POWER,
		U_LIGHT_CEILING2_KC,
		U_LIGHT_CEILING2_KL,
		U_LIGHT_CEILING2_KQ,

		U_LIGHT_CEILING3_POSITION,
		U_LIGHT_CEILING3_COLOR,
		U_LIGHT_CEILING3_POWER,
		U_LIGHT_CEILING3_KC,
		U_LIGHT_CEILING3_KL,
		U_LIGHT_CEILING3_KQ,

		U_LIGHT_CEILING4_POSITION,
		U_LIGHT_CEILING4_COLOR,
		U_LIGHT_CEILING4_POWER,
		U_LIGHT_CEILING4_KC,
		U_LIGHT_CEILING4_KL,
		U_LIGHT_CEILING4_KQ,


		U_LIGHTENABLED,
		//add these enum in UNIFORM_TYPE before U_TOTAL
		U_LIGHT0_TYPE,
		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,

		U_LIGHT_CEILING1_TYPE,
		U_LIGHT_CEILING1_SPOTDIRECTION,
		U_LIGHT_CEILING1_COSCUTOFF,
		U_LIGHT_CEILING1_COSINNER,
		U_LIGHT_CEILING1_EXPONENT,

		U_LIGHT_CEILING2_TYPE,
		U_LIGHT_CEILING2_SPOTDIRECTION,
		U_LIGHT_CEILING2_COSCUTOFF,
		U_LIGHT_CEILING2_COSINNER,
		U_LIGHT_CEILING2_EXPONENT,

		U_LIGHT_CEILING3_TYPE,
		U_LIGHT_CEILING3_SPOTDIRECTION,
		U_LIGHT_CEILING3_COSCUTOFF,
		U_LIGHT_CEILING3_COSINNER,
		U_LIGHT_CEILING3_EXPONENT,

		U_LIGHT_CEILING4_TYPE,
		U_LIGHT_CEILING4_SPOTDIRECTION,
		U_LIGHT_CEILING4_COSCUTOFF,
		U_LIGHT_CEILING4_COSINNER,
		U_LIGHT_CEILING4_EXPONENT,


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
	Light light[5];

	transform gamer_transform[1];

	NPC NPCs[NUM_NPC];
	transform NPCs_transform[NUM_NPC_PARTS];
	bool correct_angle;

	// objects
	CCollision* objects[10];
	transform obj_transform[NUM_OBJECTS];

	transform skybox_transform[6]; // left = 0, right = 1, up = 2, down = 3, front = 4, back = 5
  
	CPlayer gamer;
	CCollision* cube;
	CCollision* sphere;

	CCollision* carDisplay[5];
	CCollision* arcadeMachine[3];
	CCollision* skybox[4];

	//bezier curve points
	transform playerPos;
	transform spheree;
	transform cubee;

	//light color change
	static	float LightChange;
  
	Camera2 camera;
	int cameraMode;

	float walkingX;
	float walkingZ;
	bool M_pressed;
	int ClosestNPC;

	//CLOCK===========================
	bool eTimeStart;
	double ElapsedTime;
	bool displayBestTime;
	bool firstBestTime;
	/*static*/double BestTime;

	int lapcount;
	double LapTimes[3];
	//================================
	void AnimationReset();

	// ANIMATION 1: WALK==============
	bool stopA1;
	bool animateWalk;
	bool retractArmA1;
	bool retractLegA1;
	int A1Lcount;
	int A1Acount;

	void playAnimationWalk(double time);
	//================================

	// ANIMATION 2: COFFEE============
	bool stopA2;
	bool animateCoffee;
	bool extendArmA2;
	bool retractArmA2;
	bool renderCup;

	void playAnimationCoffee(double time);
	//================================

	// ANIMATION 3: TALKING===========
	int A3count;
	bool stopA3;
	bool animateTalking;
	bool armUpA3;

	void playAnimationTalking(double time);
	//================================

	// ANIMATION 4: IDLE==============
	bool animateIdle;
	bool armUpA4;
	bool stopA4;

	void playAnimationIdle(double time);
	//================================

	// ANIMATION 5: PLAY GAME=========
	bool animateArcade;
	bool stopA5;

	void playAnimationArcade(double time);
	//================================

	void InitSkybox();
	void InitLights();
	void InitNPCs();
	void InitObjs();

	//================================

	//Menus
	typedef struct _COORD {
		SHORT X;
		SHORT Y;
	} COORD, * PCOORD;

	COORD coords; // sets the coords of the cursor
	CMoney money;

	bool MainMenu; // to setup for Main Menu / M_Menu()
	bool PauseMenu; // to setup for Pause Menu / P_Menu()
	float BounceTime, ElapsedTime2; // BounceTime is to prevent Update() function being called too fast, ElapsedTime2 is only for PauseMenu

	void M_Menu();
	void P_Menu();
	void ExitGame(); // Exit the Scene (to be updated)

	void RenderMesh(Mesh* mesh, bool enableLight);

	void RenderObject(Mesh* mesh, transform object, bool hierarchical, bool enableLight);
	void RenderLights();
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