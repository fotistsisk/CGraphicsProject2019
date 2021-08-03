#ifndef BIM_ENGINE_RENDERER_H
#define BIM_ENGINE_RENDERER_H

#include "GLEW\glew.h"
#include "glm\glm.hpp"
#include <vector>
#include "ShaderProgram.h"
#include "SpotlightNode.h"
#include "Level.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

#define NUMBER_OF_ROAD_TILES 29

class Renderer
{
public:
	enum RENDERING_MODE
	{
		TRIANGLES,
		LINES,
		POINTS
	};

protected:
	int												m_screen_width, m_screen_height;
	glm::mat4										m_view_matrix;
	glm::mat4										m_projection_matrix;
	glm::vec3										m_camera_position;
	glm::vec3										m_camera_target_position;
	glm::vec3										m_camera_up_vector;
	glm::vec2										m_camera_movement;
	glm::vec2										m_camera_look_angle_destination;

	//for the post shader.
	int												loseCoins;
	int												loseCoinsDuration;
	int												loseCoinsCurrentFrame;

	glm::vec3 explosionPos; // explosion of ball!
	
	// Geometry Rendering Intermediate Buffer
	GLuint m_fbo;
	GLuint m_fbo_depth_texture;
	GLuint m_fbo_texture;
	
	GLuint m_vao_fbo, m_vbo_fbo_vertices;


	float m_continous_time;
	unsigned int waveTimer; // for movement: spawn pirates in update function
	unsigned int piratesInWaveTimer; // 2 secs.
	unsigned int currentTime;

	typedef struct
	{
		int numOfPirates;
		std::vector<int> types;

	} Wave;

	Wave wave; //represents a wave.


	// Rendering Mode
	RENDERING_MODE m_rendering_mode;

	// Lights
	SpotLightNode m_spotlight_node;

	Level* level;

	TTF_Font* font;
	unsigned int fontSize;
	
	// Meshes
	// pirate, terrain, tower, treasure , cannonball, plane_green , plane_red

	//terrain
	class GeometryNode* terrain;
	glm::mat4 terrain_transformation_matrix;
	glm::mat4 terrain_transformation_normal_matrix;

	//roadTiles
	class GeometryNode* roadTiles[NUMBER_OF_ROAD_TILES]; // a static array of GeometryNodes with 29 positions , 1 for every Geometry Node.
	glm::mat4 roadTiles_transformation_matrix[NUMBER_OF_ROAD_TILES]; // every tile has its own matrix .
	glm::mat4 roadTiles_transformation_normal_matrix[NUMBER_OF_ROAD_TILES];

	//chest --> maybe in chest class, see it later
	class GeometryNode* renderChest;
	glm::mat4 chest_transformation_matrix;
	glm::mat4 chest_transformation_normal_matrix;


	// green tile
	class GeometryNode* greenTile;
	glm::mat4 greenTile_transformation_matrix;
	glm::mat4 greenTile_transformation_normal_matrix;

	
	//red tile
	class GeometryNode* redTile;
	glm::mat4 redTile_transformation_matrix;
	glm::mat4 redTile_transformation_normal_matrix;

	//cube
	class GeometryNode* cube[NUMBER_OF_ROAD_TILES]; // static array of geometry node objects that represent the cubes.
	glm::mat4 cube_transformation_matrix[NUMBER_OF_ROAD_TILES];
	glm::mat4 cube_transformation_normal_matrix[NUMBER_OF_ROAD_TILES];


	

	
	// Protected Functions
	bool InitRenderingTechniques();
	bool InitIntermediateShaderBuffers();
	bool InitCommonItems();
	bool InitLightSources();
	bool InitGeometricMeshes();

	void DrawGeometryNode(class GeometryNode* node, glm::mat4 model_matrix, glm::mat4 normal_matrix);
	void DrawGeometryNodeToShadowMap(class GeometryNode* node, glm::mat4 model_matrix, glm::mat4 normal_matrix);

	ShaderProgram								m_geometry_rendering_program;
	ShaderProgram								m_postprocess_program;
	ShaderProgram								m_spot_light_shadow_map_program;
	ShaderProgram								m_text_program;

	//maybe image and text(?)

	//particles!!
	ParticleEmitter								particleEmitter; //chest...
	//ParticleSwirl								particleSwirler;
	ShaderProgram								m_particles_program;

	std::vector<ParticleEmitter>				explosionsParticles; // pirate hit.
	bool										isEnergy;


public:
	Renderer();
	~Renderer();
	bool										Init(int SCREEN_WIDTH, int SCREEN_HEIGHT);
	void										Update(float dt);
	bool										ResizeBuffers(int SCREEN_WIDTH, int SCREEN_HEIGHT);
	bool										ReloadShaders();
	void										Render();

	// Passes
	void										RenderShadowMaps();
	void										RenderGeometry();
	void										RenderToOutFB();

	void										RenderText(std::string, SDL_Color, int,int,int ); //for text 

	void										setFontSize(unsigned int);

	
	
	// Set functions
	void										SetRenderingMode(RENDERING_MODE mode);
	void										SetLevel(Level* lvl);

	// Camera Function
	void										CameraMoveForward(bool enable);
	void										CameraMoveBackWard(bool enable);
	void										CameraMoveLeft(bool enable);
	void										CameraMoveRight(bool enable);
	void										CameraLook(glm::vec2 lookDir);	

	// Get functions
	Level *										GetLevel();


	
	
};

#endif
