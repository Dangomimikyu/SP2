#ifndef SCENEMG1_H
#define SCENEMG1_H

#include "Scene.h"
#include <MatrixStack.h>
#include <transformation.h>
#include "Camera2.h"
#include "Mesh.h"
#include "Light.h"
#include "CCollision.h"
#include "CCar.h"
#include "CRectangle.h"
#include "SceneText.h"


class SceneMG1 : public Scene
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
		GEO_RACETRACK, // new
		GEO_FENCE_IL, //I = inner, O = outer
		GEO_FENCE_IT, // L = left, R = right, T = top , B = bottom
		GEO_FENCE_IR, 
		GEO_FENCE_IB,
		GEO_FENCE_OL,
		GEO_FENCE_OT,
		GEO_FENCE_OR,
		GEO_FENCE_OB,
		GEO_FENCE_TLD,  // D = diagonal
		GEO_FENCE_TRD,
		GEO_FENCE_BLD,
		GEO_FENCE_BRD,// end of new
		GEO_LIGHTSPHERE,
		GEO_TEXT,
		GEO_CAR1,
		GEO_WHEELS1,
		GEO_SPHERE,
		GEO_CAR_KEN,
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

	transform obj_transform[NUM_GEOMETRY]; //new

	Camera2 camera;
	CCar car1;
	transform car1Transform;
	CCollision* cube;
	CCollision* sphere;
	transform cubeTransform;
	transform sphereTransform;
	
	transform skybox_transform[6]; // left = 0, right = 1, up = 2, down = 3, front = 4, back = 5

	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderObject(Mesh* mesh, transform object,bool hierarchical, bool enableLight);
	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderMesh(Mesh* mesh, transform object, bool enableLight);
	void RenderObject(Mesh* mesh, transform object, bool enableLight);
	void RenderSkybox();
	std::string print_fps();
	float distanceX;
	float distanceZ;
	float rotateY;
	bool forward;

	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);

public:
	SceneMG1();
	~SceneMG1();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
};

#endif