#include <cstring>
#include <csignal>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <game.h>

#include <stdio.h>

#include <colors.h>

#define BufferLocalFace \
  BufferFace( \
      worldVertexData, \
      worldColorData, \
      worldNormalData, \
      localVertexData, \
      sizeof(localVertexData), \
      localNormalData, \
      sizeof(localNormalData), \
      FaceColor, \
      ArrayCount(localVertexData), \
      chunkVertCount)

void BufferFace (
    VertexBlock *worldVertexData,
    VertexBlock *worldColorData,
    VertexBlock *worldNormalData,

    float* VertsPositions,
    int sizeofVertPositions,

    float* Normals,
    int sizeofNormals,

    const float* VertColors,

    int numFaceVerts,
    int *chunkVertCount
  )
{
  triCount += 2;

  worldVertexData->filled += sizeofVertPositions;
  worldNormalData->filled += sizeofNormals;
  worldColorData->filled += FACE_COLOR_SIZE;

  if ( worldVertexData->filled > worldVertexData->bytesAllocd ||
       worldColorData->filled > worldColorData->bytesAllocd ||
       worldNormalData->filled > worldNormalData->bytesAllocd
     )
  {
    assert(false); // Out of memory
    worldVertexData->filled -= sizeofVertPositions;
    worldNormalData->filled -= sizeofNormals;
    worldColorData->filled -= FACE_COLOR_SIZE;
    return;
  }

  memcpy( &worldVertexData->Data[*chunkVertCount], VertsPositions, sizeofVertPositions );
  memcpy( &worldNormalData->Data[*chunkVertCount], Normals, sizeofNormals );
  memcpy( &worldColorData->Data[*chunkVertCount], VertColors, FACE_COLOR_SIZE );
  *chunkVertCount += numFaceVerts;
}

void BufferRightFace(
    v3 worldP,
    VertexBlock *worldVertexData,
    VertexBlock *worldColorData,
    VertexBlock *worldNormalData,
    const float* FaceColor,
    int *chunkVertCount )
{
  float localVertexData[] =
  {
    worldP.x + VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS, // ( 1, 1, 1)
    worldP.x + VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS, // ( 1,-1,-1)
    worldP.x + VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS, // ( 1, 1,-1)

    worldP.x + VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS, // ( 1,-1,-1)
    worldP.x + VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS, // ( 1, 1, 1)
    worldP.x + VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS  // ( 1,-1, 1)
  };

  float localNormalData[] =
  {
    worldP.x + VOXEL_RADIUS+1, worldP.y +  VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS, // ( 2, 1, 1)
    worldP.x + VOXEL_RADIUS+1, worldP.y + -VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS, // ( 2,-1,-1)
    worldP.x + VOXEL_RADIUS+1, worldP.y +  VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS, // ( 2, 1,-1)

    worldP.x + VOXEL_RADIUS+1, worldP.y + -VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS, // ( 2,-1,-1)
    worldP.x + VOXEL_RADIUS+1, worldP.y +  VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS, // ( 2, 1, 1)
    worldP.x + VOXEL_RADIUS+1, worldP.y + -VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS  // ( 2,-1, 1)
  };

  BufferLocalFace;
}

void BufferLeftFace(
    v3 worldP,
    VertexBlock *worldVertexData,
    VertexBlock *worldColorData,
    VertexBlock *worldNormalData,
    const float* FaceColor,
    int *chunkVertCount )
{
  float localVertexData[] =
  {
    worldP.x + -VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS, // (-1,-1,-1)
    worldP.x + -VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS, // (-1, 1, 1)
    worldP.x + -VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS, // (-1, 1,-1)

    worldP.x + -VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS, // (-1,-1,-1)
    worldP.x + -VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS, // (-1,-1, 1)
    worldP.x + -VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS  // (-1, 1, 1)
  };

  float localNormalData[] =
  {
    worldP.x + -VOXEL_RADIUS-1, worldP.y + -VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS, // (-2,-1,-1)
    worldP.x + -VOXEL_RADIUS-1, worldP.y +  VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS, // (-2, 1, 1)
    worldP.x + -VOXEL_RADIUS-1, worldP.y +  VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS, // (-2, 1,-1)

    worldP.x + -VOXEL_RADIUS-1, worldP.y + -VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS, // (-2,-1,-1)
    worldP.x + -VOXEL_RADIUS-1, worldP.y + -VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS, // (-2,-1, 1)
    worldP.x + -VOXEL_RADIUS-1, worldP.y +  VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS  // (-2, 1, 1)
  };

  BufferLocalFace;
}

void BufferBottomFace(
    v3 worldP,
    VertexBlock *worldVertexData,
    VertexBlock *worldColorData,
    VertexBlock *worldNormalData,
    const float* FaceColor,
    int *chunkVertCount)
{
  float localVertexData[] =
  {
    worldP.x +  VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS, // ( 1,-1, 1)
    worldP.x + -VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS, // (-1,-1,-1)
    worldP.x +  VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS, // ( 1,-1,-1)

    worldP.x +  VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS, // ( 1,-1, 1)
    worldP.x + -VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS, // (-1,-1, 1)
    worldP.x + -VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS  // (-1,-1,-1)
  };

  float localNormalData[] =
  {
    worldP.x +  VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS-1, worldP.z +  VOXEL_RADIUS, // ( 1,-2, 1)
    worldP.x + -VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS-1, worldP.z + -VOXEL_RADIUS, // (-1,-2,-1)
    worldP.x +  VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS-1, worldP.z + -VOXEL_RADIUS, // ( 1,-2,-1)

    worldP.x +  VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS-1, worldP.z +  VOXEL_RADIUS, // ( 1,-2, 1)
    worldP.x + -VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS-1, worldP.z +  VOXEL_RADIUS, // (-1,-2, 1)
    worldP.x + -VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS-1, worldP.z + -VOXEL_RADIUS  // (-1,-2,-1)
  };

  BufferLocalFace;
}

void BufferTopFace(
    v3 worldP,
    VertexBlock *worldVertexData,
    VertexBlock *worldColorData,
    VertexBlock *worldNormalData,
    const float* FaceColor,
    int *chunkVertCount)
{
  float localVertexData[] =
  {
    worldP.x +  VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS, // ( 1, 1, 1)
    worldP.x +  VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS, // ( 1, 1,-1)
    worldP.x + -VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS, // (-1, 1,-1)

    worldP.x +  VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS, // ( 1, 1, 1)
    worldP.x + -VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS, // (-1, 1,-1)
    worldP.x + -VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS  // (-1, 1, 1)
  };

  float localNormalData[] =
  {
    worldP.x +  VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS+1, worldP.z +  VOXEL_RADIUS, // ( 1, 2, 1)
    worldP.x +  VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS+1, worldP.z + -VOXEL_RADIUS, // ( 1, 2,-1)
    worldP.x + -VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS+1, worldP.z + -VOXEL_RADIUS, // (-1, 2,-1)

    worldP.x +  VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS+1, worldP.z +  VOXEL_RADIUS, // ( 1, 2, 1)
    worldP.x + -VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS+1, worldP.z + -VOXEL_RADIUS, // (-1, 2,-1)
    worldP.x + -VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS+1, worldP.z +  VOXEL_RADIUS  // (-1, 2, 1)
  };

  BufferLocalFace;
}

void BufferFrontFace(
    v3 worldP,
    VertexBlock *worldVertexData,
    VertexBlock *worldColorData,
    VertexBlock *worldNormalData,
    const float* FaceColor,
    int *chunkVertCount)
{
  float localVertexData[] =
  {
    worldP.x + -VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS, // (-1, 1, 1)
    worldP.x + -VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS, // (-1,-1, 1)
    worldP.x +  VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS, // ( 1,-1, 1)

    worldP.x +  VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS, // ( 1, 1, 1)
    worldP.x + -VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS, // (-1, 1, 1)
    worldP.x +  VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS  // ( 1,-1, 1)
  };

  float localNormalData[] =
  {
    worldP.x + -VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS+1, // (-1, 1, 2)
    worldP.x + -VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS+1, // (-1,-1, 2)
    worldP.x +  VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS+1, // ( 1,-1, 2)

    worldP.x +  VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS+1, // ( 1, 1, 2)
    worldP.x + -VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS+1, // (-1, 1, 2)
    worldP.x +  VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS, worldP.z +  VOXEL_RADIUS+1  // ( 1,-1, 2)
  };

  BufferLocalFace;
}

void BufferBackFace(
    v3 worldP,
    VertexBlock *worldVertexData,
    VertexBlock *worldColorData,
    VertexBlock *worldNormalData,
    const float* FaceColor,
    int *chunkVertCount)
{
  float localVertexData[] =
  {
    worldP.x +  VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS, // ( 1, 1,-1)
    worldP.x + -VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS, // (-1,-1,-1)
    worldP.x + -VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS, // (-1, 1,-1)

    worldP.x +  VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS, // ( 1, 1,-1)
    worldP.x +  VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS, // ( 1,-1,-1)
    worldP.x + -VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS, // (-1,-1,-1)
  };

  float localNormalData[] =
  {
    worldP.x +  VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS-1, // ( 1, 1,-2)
    worldP.x + -VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS-1, // (-1,-1,-2)
    worldP.x + -VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS-1, // (-1, 1,-2)

    worldP.x +  VOXEL_RADIUS, worldP.y +  VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS-1, // ( 1, 1,-2)
    worldP.x +  VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS-1, // ( 1,-1,-2)
    worldP.x + -VOXEL_RADIUS, worldP.y + -VOXEL_RADIUS, worldP.z + -VOXEL_RADIUS-1, // (-1,-1,-2)
  };

  BufferLocalFace;
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

void BuildChunkMesh(World *world, Chunk *chunk, v3 WorldCameraOffset )
{
  int numVoxels = chunk->Dim.x * chunk->Dim.y * chunk->Dim.z;

  // Clear out render from last frame
  memset( chunk->VertexData.Data, 0, chunk->VertexData.bytesAllocd );
  memset( chunk->ColorData.Data, 0, chunk->ColorData.bytesAllocd );
  memset( chunk->NormalData.Data, 0, chunk->NormalData.bytesAllocd );

  chunk->VertexData.filled = 0;
  chunk->ColorData.filled = 0;
  chunk->NormalData.filled = 0;

	chunk->Verticies = 0;

  Voxel *VoxelBuffer = chunk->Voxels;

  for ( int x = 0; x < chunk->Dim.x; ++x )
  {
    for ( int y = 0; y < chunk->Dim.y; ++y )
    {
      for ( int z = 0; z < chunk->Dim.z; ++z )
      {
        int i = x + (y*chunk->Dim.x) + (z*chunk->Dim.x*chunk->Dim.y);

        // Loop if this voxel isn't a boundary voxel
        if ( NotSet( VoxelBuffer[i].flags, Volume_Boundary ) )
          continue;

        // Unset the Boundary flag, and re-set it only if we render a face
        // This is only useful for the first pass; all voxels have Volume_Boundary set
        VoxelBuffer[i].flags = UnSetFlag( VoxelBuffer[i].flags, Volume_Boundary );

        canonical_position VoxelP = Canonicalize(
          world,
          V3(x,y,z) + chunk->Offset,
          chunk->WorldP
        );

        canonical_position nextVoxel  = Canonicalize( world, VoxelP + V3(1.0f,0,0) );
        canonical_position prevVoxel  = Canonicalize( world, VoxelP - V3(1.0f,0,0) );

        canonical_position topVoxel   = Canonicalize( world, VoxelP + V3(0,1.0f,0) );
        canonical_position botVoxel   = Canonicalize( world, VoxelP - V3(0,1.0f,0) );

        canonical_position frontVoxel = Canonicalize( world, VoxelP + V3(0,0,1.0f) );
        canonical_position backVoxel  = Canonicalize( world, VoxelP - V3(0,0,1.0f) );

        // This has to be called because on the first pass every voxel has Volume_Boundary set
        if ( IsFilled(world, chunk, VoxelP) ||
            chunk->flags & Chunk_Entity)
        {
          v3 WorldVoxelOffset = VoxelP.Offset + (VoxelP.WorldP * world->ChunkDim);

          v3 VoxelToCamera = Normalize(WorldCameraOffset - WorldVoxelOffset);

          // const float* FaceColors = GetColorData( chunk->Voxels[i].flags );

          if ( NotFilled( world, chunk, nextVoxel  ) || chunk->flags & Chunk_Entity)
          {
            VoxelBuffer[i].flags = SetFlag( VoxelBuffer[i].flags, Volume_Boundary );
            if ( IsFacingCamera(VoxelToCamera, V3(1,0,0)) )
            {
              const float* FaceColors = GetColorData( Voxel_Red );
              BufferRightFace(
                WorldVoxelOffset,
                &chunk->VertexData,
                &chunk->ColorData,
                &chunk->NormalData,
                FaceColors,
                &chunk->Verticies
              );
            }
          }

          if ( NotFilled( world, chunk, prevVoxel ) || chunk->flags & Chunk_Entity)
          {
            VoxelBuffer[i].flags = SetFlag( VoxelBuffer[i].flags, Volume_Boundary );
            if ( IsFacingCamera(VoxelToCamera, V3(-1,0,0)) )
            {
              const float* FaceColors = GetColorData( Voxel_Yellow );
              BufferLeftFace(
                WorldVoxelOffset,
                &chunk->VertexData,
                &chunk->ColorData,
                &chunk->NormalData,
                FaceColors,
                &chunk->Verticies
              );
            }
          }

          if ( NotFilled( world, chunk, botVoxel  ) || chunk->flags & Chunk_Entity)
          {
            VoxelBuffer[i].flags = SetFlag( VoxelBuffer[i].flags, Volume_Boundary );
            if ( IsFacingCamera(VoxelToCamera, V3(0,-1,0)) )
            {
              const float* FaceColors = GetColorData( Voxel_Teal );
              BufferBottomFace(
                WorldVoxelOffset,
                &chunk->VertexData,
                &chunk->ColorData,
                &chunk->NormalData,
                FaceColors,
                &chunk->Verticies
              );
            }
          }

          if ( NotFilled( world, chunk, topVoxel  ) || chunk->flags & Chunk_Entity)
          {
            VoxelBuffer[i].flags = SetFlag( VoxelBuffer[i].flags, Volume_Boundary );
            if ( IsFacingCamera(VoxelToCamera, V3(0,1,0)) )
            {
              const float* FaceColors = GetColorData( Voxel_Green );
              BufferTopFace(
                WorldVoxelOffset,
                &chunk->VertexData,
                &chunk->ColorData,
                &chunk->NormalData,
                FaceColors,
                &chunk->Verticies
              );
            }
          }

          if ( NotFilled( world, chunk, frontVoxel  ) || chunk->flags & Chunk_Entity)
          {
            VoxelBuffer[i].flags = SetFlag( VoxelBuffer[i].flags, Volume_Boundary );
            if ( IsFacingCamera(VoxelToCamera, V3(0,0,1)) )
            {
              const float* FaceColors = GetColorData( Voxel_White );
              BufferFrontFace(
                WorldVoxelOffset,
                &chunk->VertexData,
                &chunk->ColorData,
                &chunk->NormalData,
                FaceColors,
                &chunk->Verticies
              );
            }
          }

          if ( NotFilled( world, chunk, backVoxel  ) || chunk->flags & Chunk_Entity)
          {
            VoxelBuffer[i].flags = SetFlag( VoxelBuffer[i].flags, Volume_Boundary );
            if ( IsFacingCamera(VoxelToCamera, V3(0,0,-1)) )
            {
              const float* FaceColors = GetColorData( Voxel_Purple );
              BufferBackFace(
                WorldVoxelOffset,
                &chunk->VertexData,
                &chunk->ColorData,
                &chunk->NormalData,
                FaceColors,
                &chunk->Verticies
              );
            }
          }
        }
      }
    }
  }

}

void DrawChunk(
    World *world,
    Chunk *chunk,
    v3 WorldCameraOffset,
    GLuint &colorbuffer,
    GLuint &vertexbuffer,
    GLuint &normalbuffer)
{
  BuildChunkMesh( world, chunk, WorldCameraOffset );

  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, chunk->VertexData.bytesAllocd, chunk->VertexData.Data, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glBufferData(GL_ARRAY_BUFFER, chunk->ColorData.bytesAllocd, chunk->ColorData.Data, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
  glBufferData(GL_ARRAY_BUFFER, chunk->NormalData.bytesAllocd, chunk->NormalData.Data, GL_STATIC_DRAW);

  // Vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glVertexAttribPointer(
    0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );

  // Colors
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glVertexAttribPointer(
    1,                  // attribute 1. No particular reason for 1, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );

  // Normals
  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
  glVertexAttribPointer(
    2,
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );

  glDrawArrays(GL_TRIANGLES, 0, chunk->Verticies);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);

}
