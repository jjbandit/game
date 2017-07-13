#ifndef GAME_H
#define GAME_H

enum model_index
{
  ModelIndex_Enemy,
  ModelIndex_Player,
  ModelIndex_Loot,
  ModelIndex_Projectile,
  ModelIndex_Proton,

  ModelIndex_Count,
};

struct game_state
{
  World                   *world;
  platform                *Plat;
  entity                  *Player;
  Camera_Object           *Camera;

  RenderGroup             *RG;
  ShadowRenderGroup       *SG;
  debug_text_render_group *DebugRG;

  memory_arena *Memory;

  random_series Entropy;

  entity *Enemies[TOTAL_ENTITY_COUNT];
  projectile *Projectiles[TOTAL_PROJECTILE_COUNT];

  model *Models;
};


#endif
