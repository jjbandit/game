#include <cstring>
#include <csignal>

#include <sstream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <bonsai.h>
#include <render.h>

#include <stdio.h>

#include <colors.h>

#define BufferLocalFace \
  BufferFace( \
      world, \
      localVertexData, \
      sizeof(localVertexData), \
      localNormalData, \
      sizeof(localNormalData), \
      FaceColor)

bool
InitializeRenderGroup( RenderGroup *RG )
{
  GL_Global->glGenFramebuffers(1, &RG->FBO);
  GL_Global->glBindFramebuffer(GL_FRAMEBUFFER, RG->FBO);

  glGenTextures(1, &RG->ColorTexture);
  glBindTexture(GL_TEXTURE_2D, RG->ColorTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenTextures(1, &RG->NormalTexture);
  glBindTexture(GL_TEXTURE_2D, RG->NormalTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenTextures(1, &RG->PositionTexture);
  glBindTexture(GL_TEXTURE_2D, RG->PositionTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Depth texture
  glGenTextures(1, &RG->DepthTexture);
  glBindTexture(GL_TEXTURE_2D, RG->DepthTexture);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32,
      SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
  //

  GL_Global->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, RG->DepthTexture, 0);
  GL_Global->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RG->ColorTexture,    0);
  GL_Global->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, RG->NormalTexture,   0);
  GL_Global->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, RG->PositionTexture, 0);

  u32 attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
  GL_Global->glDrawBuffers(3, attachments);


  if (GL_Global->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    return false;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  GL_Global->glBindFramebuffer(GL_FRAMEBUFFER, 0);

  RG->ShaderID = LoadShaders( "SimpleVertexShader.vertexshader",
                              "SimpleFragmentShader.fragmentshader" );

  RG->MVPID                = GL_Global->glGetUniformLocation(RG->ShaderID, "MVP");
  RG->ModelMatrixID        = GL_Global->glGetUniformLocation(RG->ShaderID, "M");
  /* RG->LightPID             = glGetUniformLocation(RG->ShaderID, "LightP_worldspace"); */


  RG->LightingShader = LoadShaders( "Lighting.vertexshader",
                                    "Lighting.fragmentshader" );


  RG->DepthBiasMVPID          = GL_Global->glGetUniformLocation(RG->LightingShader, "DepthBiasMVP");
  RG->ShadowMapTextureUniform = GL_Global->glGetUniformLocation(RG->LightingShader, "shadowMap");
  RG->ColorTextureUniform     = GL_Global->glGetUniformLocation(RG->LightingShader, "gColor");
  RG->NormalTextureUniform    = GL_Global->glGetUniformLocation(RG->LightingShader, "gNormal");
  RG->PositionTextureUniform  = GL_Global->glGetUniformLocation(RG->LightingShader, "gPosition");
  RG->DepthTextureUniform     = GL_Global->glGetUniformLocation(RG->LightingShader, "gDepth");
  RG->GlobalLightDirectionID  = GL_Global->glGetUniformLocation(RG->LightingShader, "GlobalLightDirection");
  RG->ViewMatrixUniform       = GL_Global->glGetUniformLocation(RG->LightingShader, "ViewMatrix");
  RG->CameraPosUniform        = GL_Global->glGetUniformLocation(RG->LightingShader, "CameraPosUniform");


  RG->SimpleTextureShaderID = LoadShaders( "Passthrough.vertexshader",
                                           "SimpleTexture.fragmentshader" );

  RG->SimpleTextureUniform = GL_Global->glGetUniformLocation(RG->SimpleTextureShaderID, "Texture");
  //
  // Quad vertex buffer
  GL_Global->glGenBuffers(1, &RG->quad_vertexbuffer);
  GL_Global->glBindBuffer(GL_ARRAY_BUFFER, RG->quad_vertexbuffer);
  GL_Global->glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);



  //
  // World Data buffers
  u32 vertexbuffer;
  u32 colorbuffer;
  u32 normalbuffer;

  GL_Global->glGenBuffers(1, &vertexbuffer);
  GL_Global->glGenBuffers(1, &colorbuffer);
  GL_Global->glGenBuffers(1, &normalbuffer);

  RG->vertexbuffer = vertexbuffer;
  RG->colorbuffer  = colorbuffer;
  RG->normalbuffer = normalbuffer;

  return true;
}

bool
InitializeShadowBuffer(ShadowRenderGroup *ShadowGroup)
{
  // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
  GL_Global->glGenFramebuffers(1, &ShadowGroup->FramebufferName);
  GL_Global->glBindFramebuffer(GL_FRAMEBUFFER, ShadowGroup->FramebufferName);

  AssertNoGlErrors;

  // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
  glGenTextures(1, &ShadowGroup->DepthTexture);
  glBindTexture(GL_TEXTURE_2D, ShadowGroup->DepthTexture);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16,
      SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

  AssertNoGlErrors;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

  AssertNoGlErrors;

  // No color output in the bound framebuffer, only depth.
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  AssertNoGlErrors;

  GL_Global->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ShadowGroup->DepthTexture, 0);
  AssertNoGlErrors;

  ShadowGroup->ShaderID        = LoadShaders( "DepthRTT.vertexshader", "DepthRTT.fragmentshader");
  ShadowGroup->MVP_ID          = GL_Global->glGetUniformLocation(ShadowGroup->ShaderID, "depthMVP");

  AssertNoGlErrors;

  if(GL_Global->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    return false;

  AssertNoGlErrors;


  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  GL_Global->glBindFramebuffer(GL_FRAMEBUFFER, 0);

 return true;
}

void
RenderQuad(RenderGroup *RG)
{
  // TODO(Jesse): Please explain to me why I cannot draw two of these to the screen between clears
  /* glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); */

  GL_Global->glEnableVertexAttribArray(0);
  GL_Global->glBindBuffer(GL_ARRAY_BUFFER, RG->quad_vertexbuffer);
  GL_Global->glVertexAttribPointer(
    0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );

  glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles

  GL_Global->glDisableVertexAttribArray(0);
}

glm::mat4
GetDepthMVP(World *world, Camera_Object *Camera)
{
  glm::vec3 GlobalLightDirection =  glm::vec3( sin(GlobalLightTheta), 1.0, -2.0);
  GlobalLightDirection = glm::normalize( GlobalLightDirection );

  // Compute the MVP matrix from the light's point of view
  glm::mat4 depthProjectionMatrix = glm::ortho<float>(-Proj_XY,Proj_XY, -Proj_XY,Proj_XY, -Proj_Z,Proj_Z);

  glm::vec3 P = GLV3(GetRenderP(world, Camera->Target+GLV3(GlobalLightDirection), Camera));
  glm::vec3 Target = GLV3(GetRenderP(world, Camera->Target, Camera));

  glm::vec3 Front = glm::normalize(Target-P);
  glm::vec3 Right = glm::cross( Front, glm::vec3(0,1,0) );
  glm::vec3 Up = glm::cross(Right, Front);

  glm::mat4 depthViewMatrix = glm::lookAt(P, Target, Up);

  return depthProjectionMatrix * depthViewMatrix;
}

void
DrawWorldToFullscreenQuad(World *world, RenderGroup *RG, ShadowRenderGroup *SG, Camera_Object *Camera)
{
  GL_Global->glBindFramebuffer(GL_FRAMEBUFFER, 0);
  GL_Global->glUseProgram(RG->LightingShader);
  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

  glm::vec3 GlobalLightDirection =  glm::vec3( sin(GlobalLightTheta), 1.0, -2.0);
  GlobalLightDirection = glm::normalize( GlobalLightDirection );

  GL_Global->glUniform3fv(RG->GlobalLightDirectionID, 1, &GlobalLightDirection[0]);

  glm::mat4 biasMatrix(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0
  );

  glm::mat4 depthBiasMVP = biasMatrix * GetDepthMVP(world, Camera);
  GL_Global->glUniformMatrix4fv(RG->DepthBiasMVPID, 1, GL_FALSE, &depthBiasMVP[0][0]);

  glm::mat4 VP = RG->Basis.ViewMatrix;

  GL_Global->glUniformMatrix4fv(RG->ViewMatrixUniform, 1, GL_FALSE, &VP[0][0]);

  v3 CameraRenderP = GetRenderP(world, Camera->P, Camera);
  GL_Global->glUniform3fv(RG->CameraPosUniform, 1, &CameraRenderP.E[0]);

  GL_Global->glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, RG->ColorTexture);
  GL_Global->glUniform1i(RG->ColorTextureUniform, 0);

  GL_Global->glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, RG->NormalTexture);
  GL_Global->glUniform1i(RG->NormalTextureUniform, 1);

  GL_Global->glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, RG->PositionTexture);
  GL_Global->glUniform1i(RG->PositionTextureUniform, 2);

#if DEBUG_DRAW_SHADOW_MAP_TEXTURE
  glUseProgram(RG->SimpleTextureShaderID);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, RG->DepthTexture);
  glUniform1i(RG->SimpleTextureUniform, 0);
#endif

  RenderQuad(RG);

  return;
}

void
RenderShadowMap(World *world, ShadowRenderGroup *SG, RenderGroup *RG, Camera_Object *Camera)
{
  glViewport(0, 0, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);

  glm::mat4 depthMVP = GetDepthMVP(world, Camera);

  GL_Global->glBindFramebuffer(GL_FRAMEBUFFER, SG->FramebufferName);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  GL_Global->glUseProgram(SG->ShaderID);
  GL_Global->glUniformMatrix4fv(SG->MVP_ID, 1, GL_FALSE, &depthMVP[0][0]);

  /* glBindTexture(GL_TEXTURE_2D, SG->Texture); */

  // 1rst attribute buffer : vertices
  GL_Global->glEnableVertexAttribArray(0);
  GL_Global->glBindBuffer(GL_ARRAY_BUFFER, RG->vertexbuffer);
  GL_Global->glBufferData(GL_ARRAY_BUFFER, world->VertexData.filled, world->VertexData.Data, GL_STATIC_DRAW);
  GL_Global->glVertexAttribPointer(
    0,                  // The attribute we want to configure
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );

  glDrawArrays(GL_TRIANGLES, 0, world->VertexCount);

  GL_Global->glDisableVertexAttribArray(0);

  GL_Global->glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return;
}

void
RenderWorld(World *world, RenderGroup *RG)
{
  GL_Global->glBindFramebuffer(GL_FRAMEBUFFER, RG->FBO);

  GL_Global->glUseProgram(RG->ShaderID);
  glViewport(0,0,SCR_WIDTH,SCR_HEIGHT);

  glm::mat4 mvp = RG->Basis.ProjectionMatrix * RG->Basis.ViewMatrix * glm::mat4(1);

  GL_Global->glUniformMatrix4fv(RG->MVPID,         1, GL_FALSE, &mvp[0][0]);
  GL_Global->glUniformMatrix4fv(RG->ModelMatrixID, 1, GL_FALSE, &RG->Basis.ModelMatrix[0][0]);

  // Vertices
  GL_Global->glEnableVertexAttribArray(0);
  GL_Global->glBindBuffer(GL_ARRAY_BUFFER, RG->vertexbuffer);
  GL_Global->glBufferData(GL_ARRAY_BUFFER, world->VertexData.filled, world->VertexData.Data, GL_STATIC_DRAW);
  GL_Global->glVertexAttribPointer(
    0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );

  // Colors
  GL_Global->glEnableVertexAttribArray(1);
  GL_Global->glBindBuffer(GL_ARRAY_BUFFER, RG->colorbuffer);
  GL_Global->glBufferData(GL_ARRAY_BUFFER, world->ColorData.filled, world->ColorData.Data, GL_STATIC_DRAW);
  GL_Global->glVertexAttribPointer(
    1,                  // attribute 1. No particular reason for 1, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );

  // Normals
  GL_Global->glEnableVertexAttribArray(2);
  GL_Global->glBindBuffer(GL_ARRAY_BUFFER, RG->normalbuffer);
  GL_Global->glBufferData(GL_ARRAY_BUFFER, world->NormalData.filled, world->NormalData.Data, GL_STATIC_DRAW);
  GL_Global->glVertexAttribPointer(
    2,
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );

  glDrawArrays(GL_TRIANGLES, 0, world->VertexCount);

  GL_Global->glDisableVertexAttribArray(0);
  GL_Global->glDisableVertexAttribArray(1);
  GL_Global->glDisableVertexAttribArray(2);

}


void
FlushRenderBuffers(
    World *world,
    RenderGroup *RG,
    ShadowRenderGroup *SG,
    Camera_Object *Camera
  )
{

  RenderShadowMap(world, SG, RG, Camera);

  RenderWorld(world, RG);

  AssertNoGlErrors;

  world->VertexCount = 0;

  world->VertexData.filled = 0;
  world->NormalData.filled = 0;
  world->ColorData.filled = 0;

  return;
}
inline void
BufferFace (
    World *world,

    float* VertsPositions,
    int sizeofVertPositions,

    float* Normals,
    int sizeofNormals,

    const float* VertColors
  )
{
  world->VertexData.filled += sizeofVertPositions;
  world->NormalData.filled += sizeofNormals;
  world->ColorData.filled += sizeofNormals;

  if ( world->VertexData.filled > world->VertexData.bytesAllocd ||
       world->ColorData.filled > world->ColorData.bytesAllocd ||
       world->NormalData.filled > world->NormalData.bytesAllocd )
  {
    world->VertexData.filled -= sizeofVertPositions;
    world->NormalData.filled -= sizeofNormals;
    world->ColorData.filled  -= sizeofNormals;

    // TODO(Jesse): Instead of Asserting, do this
    /* FlushRenderBuffers( world, RG, SG, Camera); */
    // Out of memory, panic!
    /* Assert(!"Out of memory"); */
    return;
  }

  memcpy( &world->VertexData.Data[world->VertexCount*3], VertsPositions, sizeofVertPositions );
  memcpy( &world->NormalData.Data[world->VertexCount*3], Normals, sizeofNormals );
  memcpy( &world->ColorData.Data[world->VertexCount*3],  VertColors, sizeofNormals );

  tris += 2;
  world->VertexCount += 6;

  return;
}

inline void
BufferRightFace(
    World *world,
    glm::vec3 worldP,
    const float* FaceColor,
    float Diameter = VOXEL_DIAMETER
    )
{
  float localVertexData[] =
  {
    worldP.x + Diameter, worldP.y +  Diameter, worldP.z +  Diameter,
    worldP.x + Diameter, worldP.y                  , worldP.z                  ,
    worldP.x + Diameter, worldP.y +  Diameter, worldP.z                  ,

    worldP.x + Diameter, worldP.y                  , worldP.z                  ,
    worldP.x + Diameter, worldP.y +  Diameter, worldP.z +  Diameter,
    worldP.x + Diameter, worldP.y                  , worldP.z +  Diameter
  };

  float localNormalData[] =
  {
     1, 0, 0,
     1, 0, 0,
     1, 0, 0,

     1, 0, 0,
     1, 0, 0,
     1, 0, 0
  };

  BufferLocalFace;
  return;
}

inline void
BufferLeftFace(
    World *world,
    glm::vec3 worldP,
    const float* FaceColor,
    float Diameter = VOXEL_DIAMETER
    )
{
  float localVertexData[] =
  {
    worldP.x                  , worldP.y                  , worldP.z                  ,
    worldP.x                  , worldP.y +  Diameter, worldP.z +  Diameter,
    worldP.x                  , worldP.y +  Diameter, worldP.z                  ,

    worldP.x                  , worldP.y                  , worldP.z                  ,
    worldP.x                  , worldP.y                  , worldP.z +  Diameter,
    worldP.x                  , worldP.y +  Diameter, worldP.z +  Diameter
  };

  float localNormalData[] =
  {
     -1, 0, 0,
     -1, 0, 0,
     -1, 0, 0,

     -1, 0, 0,
     -1, 0, 0,
     -1, 0, 0
  };

  BufferLocalFace;
  return;
}

inline void
BufferBottomFace(
    World *world,
    glm::vec3 worldP,
    const float* FaceColor,
    float Diameter = VOXEL_DIAMETER
    )
{
  float localVertexData[] =
  {
    worldP.x +  Diameter, worldP.y                  , worldP.z +  Diameter,
    worldP.x                  , worldP.y                  , worldP.z                  ,
    worldP.x +  Diameter, worldP.y                  , worldP.z                  ,

    worldP.x +  Diameter, worldP.y                  , worldP.z +  Diameter,
    worldP.x                  , worldP.y                  , worldP.z +  Diameter,
    worldP.x                  , worldP.y                  , worldP.z
  };

  float localNormalData[] =
  {
     0,-1, 0,
     0,-1, 0,
     0,-1, 0,

     0,-1, 0,
     0,-1, 0,
     0,-1, 0
  };

  BufferLocalFace;
  return;
}

inline void
BufferTopFace(
    World *world,
    glm::vec3 worldP,
    const float* FaceColor,
    float Diameter = VOXEL_DIAMETER
    )
{
  float localVertexData[] =
  {
    worldP.x +  Diameter, worldP.y +  Diameter, worldP.z +  Diameter,
    worldP.x +  Diameter, worldP.y +  Diameter, worldP.z                  ,
    worldP.x                  , worldP.y +  Diameter, worldP.z                  ,

    worldP.x +  Diameter, worldP.y +  Diameter, worldP.z +  Diameter,
    worldP.x                  , worldP.y +  Diameter, worldP.z                  ,
    worldP.x                  , worldP.y +  Diameter, worldP.z +  Diameter
  };

  float localNormalData[] =
  {
    0, 1, 0,
    0, 1, 0,
    0, 1, 0,

    0, 1, 0,
    0, 1, 0,
    0, 1, 0
  };

  BufferLocalFace;
  return;
}

inline void
BufferFrontFace(
    World *world,
    glm::vec3 worldP,
    const float* FaceColor,
    float Diameter = VOXEL_DIAMETER
    )
{
  float localVertexData[] =
  {
    worldP.x                  , worldP.y +  Diameter, worldP.z +  Diameter,
    worldP.x                  , worldP.y                  , worldP.z +  Diameter,
    worldP.x +  Diameter, worldP.y                  , worldP.z +  Diameter,

    worldP.x +  Diameter, worldP.y +  Diameter, worldP.z +  Diameter,
    worldP.x                  , worldP.y +  Diameter, worldP.z +  Diameter,
    worldP.x +  Diameter, worldP.y                  , worldP.z +  Diameter
  };

  float localNormalData[] =
  {
    0, 0, 1,
    0, 0, 1,
    0, 0, 1,

    0, 0, 1,
    0, 0, 1,
    0, 0, 1
  };

  BufferLocalFace;
  return;
}

inline void
BufferBackFace(
    World *world,
    glm::vec3 worldP,
    const float* FaceColor,
    float Diameter = VOXEL_DIAMETER
    )
{
  float localVertexData[] =
  {
    worldP.x +  Diameter, worldP.y +  Diameter, worldP.z                  ,
    worldP.x                  , worldP.y                  , worldP.z                  ,
    worldP.x                  , worldP.y +  Diameter, worldP.z                  ,

    worldP.x +  Diameter, worldP.y +  Diameter, worldP.z                  ,
    worldP.x +  Diameter, worldP.y                  , worldP.z                  ,
    worldP.x                  , worldP.y                  , worldP.z                  ,
  };

  float localNormalData[] =
  {
    0, 0, -1,
    0, 0, -1,
    0, 0, -1,

    0, 0, -1,
    0, 0, -1,
    0, 0, -1,
  };

  BufferLocalFace;
  return;
}

bool IsRightChunkBoundary( chunk_dimension ChunkDim, int idx )
{
  return (idx+1) % (int)ChunkDim.x == 0;
}

bool IsLeftChunkBoundary( chunk_dimension ChunkDim, int idx )
{
  return (idx) % (int)ChunkDim.x == 0;
}

bool IsTopChunkBoundary( chunk_dimension ChunkDim, int idx )
{
  return ((idx/(int)ChunkDim.x)+1) % (int)ChunkDim.y == 0;
}

bool IsBottomChunkBoundary( chunk_dimension ChunkDim, int idx )
{
  return (idx/(int)ChunkDim.x) % (int)ChunkDim.y == 0;
}

float
GetTheta(v3 P1, v3 P2, v3 Axis)
{
  float DotP1P2 = Dot(P1,P2);

  float LP1, LP2;

  LP1 = Length(P1);
  LP2 = Length(P2);

  Assert(LP1 != 0);
  Assert(LP2 != 0);

  float cosTheta = DotP1P2 / (LP1*LP2);
  cosTheta = ClampMinus1To1(cosTheta);
  float theta = acos( cosTheta );

  Assert(theta >= -1 || theta <= 1);
  return theta;
}

Quaternion
RotatePoint(v3 P1, v3 P2)
{
  P1 = Normalize(P1);
  P2 = Normalize(P2);
  v3 Axis = Normalize(Cross(P1, P2));

  float theta = GetTheta(P1, P2, Axis);

  Quaternion Result( cos(theta/2), (Axis*sin(theta/2)) );

  Assert(Length(Result.xyz) > 0);

  return Result;
}

void
DEBUG_DrawLine(World *world, v3 P1, v3 P2, int ColorIndex, float Thickness )
{
  // 2 verts per line, 3 floats per vert

  float localNormalData[] =
  {
     0, 0, 0,
     0, 0, 0,
     0, 0, 0,

     0, 0, 0,
     0, 0, 0,
     0, 0, 0
  };

  float FaceColors[32];
  GetColorData( ColorIndex, FaceColors);

  {
    float localVertexData[] =
    {
      P1.x, P1.y, P1.z,
      P2.x, P2.y, P2.z,
      P1.x + Thickness, P1.y,  P1.z + Thickness,

      P2.x, P2.y, P2.z,
      P1.x, P1.y, P1.z,
      P2.x + Thickness, P2.y, P2.z + Thickness
    };


    BufferFace(world,
        localVertexData,
        sizeof(localVertexData),
        localNormalData,
        sizeof(localNormalData),
        FaceColors);
  }

  {
    float localVertexData[] =
    {
      P1.x, P1.y, P1.z,
      P2.x, P2.y, P2.z,
      P1.x, P1.y + Thickness,  P1.z,

      P2.x, P2.y, P2.z,
      P1.x, P1.y, P1.z,
      P2.x, P2.y + Thickness, P2.z
    };


    BufferFace(world,
        localVertexData,
        sizeof(localVertexData),
        localNormalData,
        sizeof(localNormalData),
        FaceColors);
  }

#if 0
  // This is for anti-aliasing the lines; it draws extra triangles along the edges which can be set to alpha 0
  {
    float localVertexData[] =
    {
      P1.x + Thickness, P1.y + Thickness, P1.z + Thickness,
      P2.x, P2.y, P2.z,
      P2.x + Thickness, P2.y + Thickness, P2.z + Thickness,

      P2.x + Thickness, P2.y + Thickness, P2.z + Thickness,
      P1.x + Thickness, P1.y + Thickness, P1.z + Thickness,
      P1.x, P1.y, P1.z,
    };

    BufferFace(world,
        localVertexData,
        sizeof(localVertexData),
        localNormalData,
        sizeof(localNormalData),
        FaceColors);
  }
#endif

  return;
}

void
DEBUG_DrawLine(World *world, line Line, int ColorIndex, float Thickness )
{
  DEBUG_DrawLine(world, Line.MinP, Line.MaxP, ColorIndex, Thickness);
  return;
}

void
DEBUG_DrawAABB( World *world, v3 MinP, v3 MaxP, Quaternion Rotation, int ColorIndex, float Thickness = 0.05f )
{
  /* v3 HalfDim = (GetRenderP(world, MaxCP) - GetRenderP(world, MinCP)) / 2; */

  // Start in model space
  /* v3 MinP = HalfDim * -1; */
  /* v3 MaxP = HalfDim; */

  // Compute verticies
  v3 TopRL = V3(MinP.x, MaxP.y, MinP.z);
  v3 TopRR = V3(MaxP.x, MaxP.y, MinP.z);
  v3 TopFL = V3(MinP.x, MaxP.y, MaxP.z);
  v3 TopFR = V3(MaxP.x, MaxP.y, MaxP.z);
  v3 BotRL = V3(MinP.x, MinP.y, MinP.z);
  v3 BotRR = V3(MaxP.x, MinP.y, MinP.z);
  v3 BotFL = V3(MinP.x, MinP.y, MaxP.z);
  v3 BotFR = V3(MaxP.x, MinP.y, MaxP.z);

  /* // Apply rotation to verts */
  /* TopRL = ((Rotation * Quaternion(1, TopRL)) * Conjugate(Rotation)).xyz; */
  /* TopRR = ((Rotation * Quaternion(1, TopRR)) * Conjugate(Rotation)).xyz; */
  /* TopFL = ((Rotation * Quaternion(1, TopFL)) * Conjugate(Rotation)).xyz; */
  /* TopFR = ((Rotation * Quaternion(1, TopFR)) * Conjugate(Rotation)).xyz; */
  /* BotRL = ((Rotation * Quaternion(1, BotRL)) * Conjugate(Rotation)).xyz; */
  /* BotRR = ((Rotation * Quaternion(1, BotRR)) * Conjugate(Rotation)).xyz; */
  /* BotFL = ((Rotation * Quaternion(1, BotFL)) * Conjugate(Rotation)).xyz; */
  /* BotFR = ((Rotation * Quaternion(1, BotFR)) * Conjugate(Rotation)).xyz; */

  /* // Translate into world space */
  /* TopRL += HalfDim + MinCP.Offset; */
  /* TopRR += HalfDim + MinCP.Offset; */
  /* TopFL += HalfDim + MinCP.Offset; */
  /* TopFR += HalfDim + MinCP.Offset; */
  /* BotRL += HalfDim + MinCP.Offset; */
  /* BotRR += HalfDim + MinCP.Offset; */
  /* BotFL += HalfDim + MinCP.Offset; */
  /* BotFR += HalfDim + MinCP.Offset; */

  /* // Translate into render space */
  /* TopRL = GetRenderP(world, Canonical_Position(TopRL, MinCP.WorldP)); */
  /* TopRR = GetRenderP(world, Canonical_Position(TopRR, MinCP.WorldP)); */
  /* TopFL = GetRenderP(world, Canonical_Position(TopFL, MinCP.WorldP)); */
  /* TopFR = GetRenderP(world, Canonical_Position(TopFR, MinCP.WorldP)); */
  /* BotRL = GetRenderP(world, Canonical_Position(BotRL, MinCP.WorldP)); */
  /* BotRR = GetRenderP(world, Canonical_Position(BotRR, MinCP.WorldP)); */
  /* BotFL = GetRenderP(world, Canonical_Position(BotFL, MinCP.WorldP)); */
  /* BotFR = GetRenderP(world, Canonical_Position(BotFR, MinCP.WorldP)); */

  // Render
  //
  // Top
  DEBUG_DrawLine(world, TopRL, TopRR, ColorIndex, Thickness);
  DEBUG_DrawLine(world, TopFL, TopFR, ColorIndex, Thickness);
  DEBUG_DrawLine(world, TopFL, TopRL, ColorIndex, Thickness);
  DEBUG_DrawLine(world, TopFR, TopRR, ColorIndex, Thickness);

  // Right
  DEBUG_DrawLine(world, TopFR, BotFR, ColorIndex, Thickness);
  DEBUG_DrawLine(world, TopRR, BotRR, ColorIndex, Thickness);

  // Left
  DEBUG_DrawLine(world, TopFL, BotFL, ColorIndex, Thickness);
  DEBUG_DrawLine(world, TopRL, BotRL, ColorIndex, Thickness);

  // Bottom
  DEBUG_DrawLine(world, BotRL, BotRR, ColorIndex, Thickness);
  DEBUG_DrawLine(world, BotFL, BotFR, ColorIndex, Thickness);
  DEBUG_DrawLine(world, BotFL, BotRL, ColorIndex, Thickness);
  DEBUG_DrawLine(world, BotFR, BotRR, ColorIndex, Thickness);

  return;
}

void
DEBUG_DrawAABB( World *world, AABB Rect, Quaternion Rotation, int ColorIndex, float Thickness = 0.05f )
{
  DEBUG_DrawAABB( world, Rect.MinCorner, Rect.MaxCorner, Rotation, ColorIndex, Thickness );
  return;
}

v3
GetModelSpaceP(chunk_data *chunk, v3 P)
{
  /* v3 HalfDim = chunk->Dim/2; */
  /* v3 Result = P - HalfDim; */

  v3 Result = P;

  return Result;
}

void
DEBUG_DrawChunkAABB( World *world, world_position WorldP, Camera_Object *Camera, Quaternion Rotation, int ColorIndex )
{
  v3 MinP = GetRenderP(world, Canonical_Position(world, V3(0,0,0), WorldP), Camera);
  v3 MaxP = GetRenderP(world, Canonical_Position(world, CHUNK_DIMENSION, WorldP), Camera);

  DEBUG_DrawAABB(world, MinP, MaxP , Rotation, ColorIndex );

  return;
}

void
DEBUG_DrawChunkAABB( World *world, world_chunk *chunk, Camera_Object *Camera, Quaternion Rotation, int ColorIndex )
{
  v3 MinP = GetRenderP(world, Canonical_Position(world, V3(0,0,0), chunk->WorldP), Camera);
  v3 MaxP = GetRenderP(world, Canonical_Position(world, V3(chunk->Data->Dim), chunk->WorldP), Camera);

  DEBUG_DrawAABB(world, MinP, MaxP , Rotation, ColorIndex );

  return;
}

v3
Rotate(v3 P, Quaternion Rotation)
{
  v3 Result = ((Rotation * Quaternion(0, P)) * Conjugate(Rotation)).xyz;
  return Result;
}

line
Rotate(line Line, Quaternion Rotation)
{
  line Result;

  Result.MinP = Rotate(Line.MinP, Rotation);
  Result.MaxP = Rotate(Line.MaxP, Rotation);

  return Result;
}

void
DEBUG_DrawPointMarker( World *world, v3 RenderP, int ColorIndex, float Diameter)
{
  float FaceColors[FACE_COLOR_SIZE];
  GetColorData(ColorIndex, &FaceColors[0]);;

  RenderP = RenderP - (Diameter/2);

  BufferRightFace(  world, GLV3(RenderP), FaceColors, Diameter);
  BufferLeftFace(   world, GLV3(RenderP), FaceColors, Diameter);
  BufferBottomFace( world, GLV3(RenderP), FaceColors, Diameter);
  BufferTopFace(    world, GLV3(RenderP), FaceColors, Diameter);
  BufferFrontFace(  world, GLV3(RenderP), FaceColors, Diameter);
  BufferBackFace(   world, GLV3(RenderP), FaceColors, Diameter);

  return;
}

inline float
DistanceToPlane(plane *Plane, v3 P)
{
  float x = Plane->P.x;
  float y = Plane->P.y;
  float z = Plane->P.z;

  float a = Plane->Normal.x;
  float b = Plane->Normal.y;
  float c = Plane->Normal.z;

  float d = Plane->d;
  Assert(a*x + b*y + c*z + d == 0);

  float Distance = a*P.x + b*P.y + c*P.z + d;
  return Distance;
}

inline bool
IsInFrustum(World *world, Camera_Object *Camera, canonical_position P)
{
  bool Result = true;

  v3 TestP = GetRenderP(world, P, Camera);

  Result &= (DistanceToPlane(&Camera->Frust.Top, TestP) > 0);
  Result &= (DistanceToPlane(&Camera->Frust.Bot, TestP) > 0);
  Result &= (DistanceToPlane(&Camera->Frust.Left, TestP) > 0);
  Result &= (DistanceToPlane(&Camera->Frust.Right, TestP) > 0);

  return Result;
}

voxel_position
Clamp01( voxel_position V )
{
  voxel_position Result = V;
  if (Result.x < 0) Result.x = 0;
  if (Result.y < 0) Result.y = 0;
  if (Result.z < 0) Result.z = 0;

  if (Result.x > 1) Result.x = 1;
  if (Result.y > 1) Result.y = 1;
  if (Result.z > 1) Result.z = 1;

  return Result;
}

void
PushBoundaryVoxel( chunk_data *chunk, Voxel voxel )
{
  Assert( chunk->BoundaryVoxelCount < Volume(chunk->Dim) );

  chunk->BoundaryVoxels[chunk->BoundaryVoxelCount] = voxel;
  chunk->BoundaryVoxelCount++;
}

bool
BuildExteriorBoundaryVoxels( World *world, world_chunk *chunk, world_chunk *Neighbor, voxel_position NeighborVector )
{
  voxel_position nvSq = (NeighborVector*NeighborVector);

  voxel_position AbsInvNeighborVector = ((nvSq-1)*(nvSq-1));

  voxel_position LocalPlane = ClampPositive(chunk->Data->Dim-1) * AbsInvNeighborVector + 1;

  voxel_position LocalOffset = ClampPositive(chunk->Data->Dim*NeighborVector - nvSq);

  voxel_position Start = Voxel_Position(0,0,0);

  for ( int z = Start.z; z < LocalPlane.z; ++z )
  {
    for ( int y = Start.y; y < LocalPlane.y; ++y )
    {
      for ( int x = Start.x; x < LocalPlane.x; ++x )
      {
        voxel_position LocalVoxelP = Voxel_Position(x+LocalOffset.x, y+LocalOffset.y, z+LocalOffset.z);

        if ( !IsFilledInChunk(world, chunk, LocalVoxelP ) )
          continue;

        voxel_position NeighborP = ClampPositive(
            (LocalVoxelP -
            (chunk->Data->Dim * NeighborVector) ) -
            (NeighborVector*NeighborVector));

        if ( ! IsFilledInChunk( world, Neighbor, NeighborP) )
        {
          Voxel voxel = chunk->Data->Voxels[GetIndex(LocalVoxelP, chunk->Data)];

          if (NeighborVector.x > 0)
            voxel.flags = SetFlag(voxel.flags, Voxel_RightFace);

          if (NeighborVector.x < 0)
            voxel.flags = SetFlag(voxel.flags, Voxel_LeftFace);

          if (NeighborVector.y > 0)
            voxel.flags = SetFlag(voxel.flags, Voxel_TopFace);

          if (NeighborVector.y < 0)
            voxel.flags = SetFlag(voxel.flags, Voxel_BottomFace);

          if (NeighborVector.z > 0)
            voxel.flags = SetFlag(voxel.flags, Voxel_FrontFace);

          if (NeighborVector.z < 0)
            voxel.flags = SetFlag(voxel.flags, Voxel_BackFace);

      voxel_position P = GetVoxelP(voxel);
          Assert( P == LocalVoxelP);
          PushBoundaryVoxel( chunk->Data, voxel );
        }
      }
    }
  }


  return true;
}

bool
IsInsideChunk( voxel_position Dim, voxel_position P )
{
  bool Result = false;

  Result = (
              P.x >= 0 &&
              P.y >= 0 &&
              P.z >= 0 &&

              P.x < Dim.x &&
              P.y < Dim.y &&
              P.z < Dim.z
           );

  return Result;
}

void
BuildInteriorBoundaryVoxels(World *world, chunk_data *chunk, world_position WorldP)
{
  chunk->flags = UnSetFlag( chunk->flags, Chunk_RebuildInteriorBoundary );

  for ( int z = 0; z < chunk->Dim.z ; ++z )
  {
    for ( int y = 0; y < chunk->Dim.y ; ++y )
    {
      for ( int x = 0; x < chunk->Dim.x ; ++x )
      {
        voxel_position VoxelP = Voxel_Position(x,y,z);

        if ( NotFilled( chunk, VoxelP ) )
          continue;

    voxel_position rightVoxel = VoxelP + Voxel_Position(1, 0, 0);
    voxel_position leftVoxel = VoxelP - Voxel_Position(1, 0, 0);

    voxel_position topVoxel = VoxelP + Voxel_Position(0, 1, 0);
    voxel_position botVoxel = VoxelP - Voxel_Position(0, 1, 0);

    voxel_position frontVoxel = VoxelP + Voxel_Position(0, 0, 1);
    voxel_position backVoxel = VoxelP - Voxel_Position(0, 0, 1);

        Voxel voxel = chunk->Voxels[GetIndex(Voxel_Position(x,y,z), chunk)];

        bool DidPushVoxel = false;

        if ( IsInsideChunk( chunk->Dim, rightVoxel  ) && NotFilled( chunk, rightVoxel  ))
        {
          voxel.flags = SetFlag(voxel.flags, Voxel_RightFace);
          DidPushVoxel = true;
        }
        if ( IsInsideChunk( chunk->Dim, leftVoxel  ) && NotFilled( chunk, leftVoxel  ))
        {
          voxel.flags = SetFlag(voxel.flags, Voxel_LeftFace);
          DidPushVoxel = true;
        }
        if ( IsInsideChunk( chunk->Dim, botVoxel   ) && NotFilled( chunk, botVoxel   ))
        {
          voxel.flags = SetFlag(voxel.flags, Voxel_BottomFace);
          DidPushVoxel = true;
        }
        if ( IsInsideChunk( chunk->Dim, topVoxel   ) && NotFilled( chunk, topVoxel   ))
        {
          voxel.flags = SetFlag(voxel.flags, Voxel_TopFace);
          DidPushVoxel = true;
        }
        if ( IsInsideChunk( chunk->Dim, frontVoxel ) && NotFilled( chunk, frontVoxel ))
        {
          voxel.flags = SetFlag(voxel.flags, Voxel_FrontFace);
          DidPushVoxel = true;
        }
        if ( IsInsideChunk( chunk->Dim, backVoxel  ) && NotFilled( chunk, backVoxel  ))
        {
          voxel.flags = SetFlag(voxel.flags, Voxel_BackFace);
          DidPushVoxel = true;
        }

        if (DidPushVoxel)
        {
          voxel_position P = GetVoxelP(voxel);
          Assert( P == Voxel_Position(x,y,z));
          PushBoundaryVoxel(chunk, voxel);
        }

      }
    }
  }
}

bool
IsInFrustum( World *world, Camera_Object *Camera, world_chunk *Chunk )
{
  v3 ChunkMid = V3(CD_X, CD_Y, CD_Z)/2;
  canonical_position P1 = Canonical_Position( world, ChunkMid, Chunk->WorldP );
  bool Result = IsInFrustum(world, Camera, P1 );
  return Result;
}

inline bool
IsFacingPoint( glm::vec3 FaceToPoint, v3 FaceNormal )
{
  bool Result = IsFacingPoint(GLV3(FaceToPoint), FaceNormal);
  return Result;
}

void
ClearFramebuffers(RenderGroup *RG, ShadowRenderGroup *SG)
{
  glClearColor(0.35f, 0.35f, 0.35f, 0.0f);

  GL_Global->glBindFramebuffer(GL_FRAMEBUFFER, RG->FBO);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  GL_Global->glBindFramebuffer(GL_FRAMEBUFFER, SG->FramebufferName);
  glClear(GL_DEPTH_BUFFER_BIT);

  GL_Global->glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  return;
}

void
BufferChunkMesh(
    World *world,
    chunk_data *chunk,
    world_position WorldP,
    RenderGroup *RG,
    ShadowRenderGroup *SG,
    Camera_Object *Camera,
    v3 Offset = V3(0,0,0)
  )
{
  float FaceColors[FACE_COLOR_SIZE];

  int MaxChunkMeshBytes = chunk->BoundaryVoxelCount * VERT_PER_VOXEL * BYTES_PER_VERT;
  if (world->VertexData.filled + MaxChunkMeshBytes > world->VertexData.bytesAllocd )
  {
    FlushRenderBuffers( world, RG, SG, Camera);
  }

  for ( int i = 0; i < chunk->BoundaryVoxelCount; ++i )
  {
    VoxelsIndexed ++;

    Voxel V = chunk->BoundaryVoxels[i];

    GetColorData(GetVoxelColor(V), &FaceColors[0]);;

    glm::vec3 RenderP =
      GLV3(GetRenderP(world, Canonical_Position(world, Offset+GetVoxelP(V), WorldP), Camera));

    if ( IsSet( V.flags, Voxel_RightFace ) )
    {
      BufferRightFace(  world, RenderP, FaceColors);
    }
    if ( IsSet( V.flags, Voxel_LeftFace ) )
    {
      BufferLeftFace(   world, RenderP, FaceColors);
    }
    if ( IsSet( V.flags, Voxel_BottomFace ) )
    {
      BufferBottomFace( world, RenderP, FaceColors);
    }
    if ( IsSet( V.flags, Voxel_TopFace ) )
    {
      BufferTopFace(    world, RenderP, FaceColors);
    }
    if ( IsSet( V.flags, Voxel_FrontFace ) )
    {
      BufferFrontFace(  world, RenderP, FaceColors);
    }
    if ( IsSet( V.flags, Voxel_BackFace ) )
    {
      BufferBackFace(   world, RenderP, FaceColors);
    }
  }


  return;
}

void
BuildBoundaryVoxels( World *world, world_chunk *WorldChunk)
{
  chunk_data* chunk = WorldChunk->Data;
  if ( IsSet(chunk->flags, Chunk_RebuildInteriorBoundary) )
  {
    chunk->BoundaryVoxelCount = 0;
    BuildInteriorBoundaryVoxels( world, chunk, WorldChunk->WorldP );
  }

#if 0
  if ( IsSet(chunk->flags, Chunk_RebuildExteriorTop   ) )
  {
    voxel_position  TopVector    = Voxel_Position(0,1,0);
    world_chunk *Top = GetWorldChunk( world, WorldChunk->WorldP + TopVector   );
    if ( Top && IsSet( Top->Data->flags, Chunk_Initialized) )
    {
      BuildExteriorBoundaryVoxels( world,  WorldChunk,  Top,    TopVector   );
      chunk->flags = UnSetFlag( chunk->flags, Chunk_RebuildExteriorTop );
    }
  }

  if ( IsSet(chunk->flags, Chunk_RebuildExteriorBot   ) )
  {
    voxel_position  BotVector    = Voxel_Position(0,-1,0);
    world_chunk *Bot = GetWorldChunk( world, WorldChunk->WorldP + BotVector   );
    if ( Bot && IsSet( Bot->Data->flags, Chunk_Initialized) )
    {
      BuildExteriorBoundaryVoxels( world,  WorldChunk,  Bot,    BotVector   );
      chunk->flags = UnSetFlag( chunk->flags, Chunk_RebuildExteriorBot );
    }
  }

  if ( IsSet(chunk->flags, Chunk_RebuildExteriorLeft  ) )
  {
    voxel_position  LeftVector   = Voxel_Position(-1,0,0);
    world_chunk *Left = GetWorldChunk( world, WorldChunk->WorldP + LeftVector  );
    if ( Left && IsSet( Left->Data->flags, Chunk_Initialized) )
    {
      BuildExteriorBoundaryVoxels( world,  WorldChunk,  Left,   LeftVector  );
      chunk->flags = UnSetFlag( chunk->flags, Chunk_RebuildExteriorLeft );
    }
  }

  if ( IsSet(chunk->flags, Chunk_RebuildExteriorRight ) )
  {
    voxel_position  RightVector  = Voxel_Position(1,0,0);
    world_chunk *Right = GetWorldChunk( world, WorldChunk->WorldP + RightVector );
    if ( Right && IsSet( Right->Data->flags, Chunk_Initialized) )
    {
      BuildExteriorBoundaryVoxels( world,  WorldChunk,  Right,  RightVector );
      chunk->flags = UnSetFlag( chunk->flags, Chunk_RebuildExteriorRight );
    }
  }

  if ( IsSet(chunk->flags, Chunk_RebuildExteriorFront ) )
  {
    voxel_position  FrontVector  = Voxel_Position(0,0,1);
    world_chunk *Front = GetWorldChunk( world, WorldChunk->WorldP + FrontVector );
    if ( Front && IsSet( Front->Data->flags, Chunk_Initialized) )
    {
      BuildExteriorBoundaryVoxels( world,  WorldChunk,  Front,  FrontVector );
      chunk->flags = UnSetFlag( chunk->flags, Chunk_RebuildExteriorFront );
    }
  }

  if ( IsSet(chunk->flags, Chunk_RebuildExteriorBack  ) )
  {
    voxel_position  BackVector   = Voxel_Position(0,0,-1);
    world_chunk *Back = GetWorldChunk( world, WorldChunk->WorldP + BackVector  );
    if ( Back && IsSet( Back->Data->flags, Chunk_Initialized) )
    {
      BuildExteriorBoundaryVoxels( world,  WorldChunk,  Back,   BackVector  );
      chunk->flags = UnSetFlag( chunk->flags, Chunk_RebuildExteriorBack );
    }
  }
#endif

  return;
}

void
DrawWorldChunk(
    World *world,
    world_chunk *WorldChunk,
    Camera_Object *Camera,
    RenderGroup *RG,
    ShadowRenderGroup *SG
  )
{
  if (IsInFrustum(world, Camera, WorldChunk) )
  {
    if (IsSet(WorldChunk->Data->flags, Chunk_Initialized) )
    {
      BufferChunkMesh(world, WorldChunk->Data, WorldChunk->WorldP, RG, SG, Camera);
    }
  }

  return;
}

void
DrawEntity(
    World *world,
    Entity *entity,
    Camera_Object *Camera,
    RenderGroup *RG,
    ShadowRenderGroup *SG
  )
{
  // Debug light code
  /* glm::vec3 LightP = GLV3(GetRenderP(world, entity->P + entity->Model.Dim/2)); */
  /* glUniform3fv(RG->LightPID, 1, &LightP[0]); */
  //

  if (!entity->Spawned)
    return;

  if ( IsSet(entity->Model->flags, Chunk_Initialized) )
  {

    if ( IsSet(entity->Model->flags, Chunk_RebuildInteriorBoundary) )
    {
      BuildInteriorBoundaryVoxels(world, entity->Model, entity->P.WorldP);
    }

    BufferChunkMesh(world, entity->Model, entity->P.WorldP, RG, SG, Camera, entity->P.Offset);
  }

  return;
}