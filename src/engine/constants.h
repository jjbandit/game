#define CHUNK_VOL (CHUNK_HEIGHT*CHUNK_WIDTH*CHUNK_DEPTH)

// 6 verticies per face, 6 faces per voxel
#define VERTS_PER_FACE 6
#define VERTS_PER_VOXEL (VERTS_PER_FACE*6)

#define WORLD_X V3(1,0,0)
#define WORLD_Y V3(0,1,0)
#define WORLD_Z V3(0,0,1)

#define WORLD_HASH_SIZE_MULTIPLIER 8
#define WORLD_HASH_SIZE (VR_X*VR_Y*VR_Z*WORLD_HASH_SIZE_MULTIPLIER)
#define FREELIST_SIZE (65536)

#define NOISE_FREQUENCY 100L

#define WORLD_GRAVITY V3(0.0f, 0.0f, 0.0f)

#define SECONDS_PER_LEVEL 10.0f
#define TITLE_FONT_SIZE  32

#define PLAYER_COUNT             1
#define PLAYER_MAX_HP            1000
#define PLAYER_DRAG              V3(0.9f, 0.9f, 0.0f)

#define MODELS_PATH "models"
#define SHADER_PATH "shaders"
#define SHADER_HEADER "/header.glsl"

#define SCREEN_RATIO 1
#define SCR_WIDTH (3840/SCREEN_RATIO)
#define SCR_HEIGHT (2160/SCREEN_RATIO)

/* #define PLAYER_MODEL MODELS_PATH"/chr_knight.vox" */
/* #define PLAYER_MODEL MODELS_PATH"/ephtracy.vox" */
/* #define PLAYER_MODEL MODELS_PATH"/chr_sword.vox" */
/* #define PLAYER_MODEL MODELS_PATH"/shelf.vox" */
/* #define PLAYER_MODEL MODELS_PATH"/8x8x8.vox" */
/* #define PLAYER_MODEL MODELS_PATH"/alien_bot2.vox" */
/* #define PLAYER_MODEL MODELS_PATH"/chr_rain.vox" */
/* #define PLAYER_MODEL MODELS_PATH"/chr_old.vox" */
/* #define PLAYER_MODEL MODELS_PATH"/3x3x3.vox" */
/* #define PLAYER_MODEL MODELS_PATH"/x-wing.vox" */

#define ENEMY_MODEL MODELS_PATH"/tie-fighter.vox"
#define LOOT_MODEL MODELS_PATH"/3x3x3.vox"
#define PROJECTILE_MODEL MODELS_PATH"/3x3x3.vox"

#define TOTAL_ENTITY_COUNT     (1024)

#define TOTAL_FOLIE_COUNT      64
#define TOTAL_MODEL_COUNT      8
#define TOTAL_PARTICLE_SYSTEMS 32
#define TOTAL_FRAME_EVENT_COUNT 960

#define PARTICLES_PER_SYSTEM   1024

#define ENEMY_SPEED 800
#define PLAYER_SPEED 20000
#define PROJECTILE_SPEED 60

#define ENEMY_DRAG 0

#define LOOT_CHANCE 1

#define PROJECTILE_DRAG 0
#define PROJECTILE_AABB Chunk_Dimension(1,1,1)

#define DEBUG_ENTITY_DIM     Chunk_Dimension(3,3,1)
#define DEBUG_ENTITY_COLLISION_VOL_RADIUS V3(1.0f, 1.0f, 3.0f)

#define DEBUG_CHUNK_AABB              0
#define DEBUG_DRAW_COLLISION_VOLUMES  0

#define DEBUG_WORLD_GENERATION        0
#define DEBUG_DRAW_SHADOW_MAP_TEXTURE 1
#define DEBUG_HIGHLIGHT_VOID_CHUNKS   0
#define DEBUG_DRAW_WORLD_AXIES        1
#define DEBUG_THREAD_COUNT_BIAS       ((u32)(-26))
#define DEBUG_PARTICLE_EFFECTS        1

#define DEFAULT_LINE_THICKNESS       0.15f

#define DEBUG_FRAMES_TO_RUN           -1

#define DEBUG_TEXTURE_SIZE 500

global_variable r32 DEBUG_CAMERA_FOCAL_LENGTH = 50.0f;
#define DEBUG_CAMERA_SCROLL_SPEED  20.0f

// NOTE(Jesse): The position storage needs to be as large as the largest model
// that will ever be loaded. Can we load models in chunks as well?
#define POSITION_BIT_WIDTH 5
#define COLOR_BIT_WIDTH 8

#define FINAL_POSITION_BIT (POSITION_BIT_WIDTH*3)
#define FINAL_COLOR_BIT ((FINAL_POSITION_BIT+COLOR_BIT_WIDTH))

debug_global s32 numFrames = 0;
debug_global r64 accumulatedTime = 0;

debug_global s32 tris = 0;
debug_global s32 VoxelsIndexed = 0;


#define FACE_VERT_COUNT 6

#define MAX_CHUNK_EDGES 8

// THIS MUST MATCH THE DEFINE IN header.glsl
// Also must be a power of two
#define MAX_LIGHTS 1024

#define MIN_LOD_DISTANCE 300

#define VISIBLE_REGION Chunk_Dimension(VR_X,VR_Y,VR_Z)

#define VISIBLE_REGION_RADIUS (VISIBLE_REGION/2)
#define VISIBLE_REGION_VOLUME (VR_X*VR_X*VR_X)