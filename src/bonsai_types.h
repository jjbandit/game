#ifndef BONSAI_TYPES_H
#define BONSAI_TYPES_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <inttypes.h>
#include <climits>

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062
#define TWOPI (2.0*PI)
#define PIf (float(PI))

struct world;
struct canonical_position;

// 8 Bit types
typedef uint8_t  u8;

// 16 Bit types
typedef uint16_t u16;
typedef int16_t  s16;

// 32 Bit types
typedef int32_t  s32;
typedef uint32_t u32;
typedef u32      b32;
typedef float    real32;
typedef real32   r32;
typedef r32      radians;
typedef r32      degrees;

// 64 Bit types
typedef int64_t  s64;
typedef uint64_t u64;
typedef u64      umm;
typedef double   r64;

struct v2i
{
  s32 x, y;
};

struct v2
{
  r32 x, y;
};


union v3
{
  struct { float x, y, z; };
  struct { float r, g, b; };

  struct {
    v2 xy;
    float Ignored0_;
  };

  struct {
    float Ignored1_;
    v2 yz;
  };

  float E[3];
};


// Note: OpenGL matrices have x first
union v4
{
  struct { float x, y, z, w; };
  struct { float r, g, b, a; };

  struct {
    v2 xy;
    float Ignored0_;
    float Ignored01_;
  };

  struct {
    v3 xyz;
    float Ignored02_;
  };

  struct {
    v3 rgb;
    float Ignored03_;
  };

  float E[4];


  v4()
  {
    *this = v4(0,0,0,1);
  }

  v4(v3 v, float w)
  {
    this->x = v.x;
    this->y = v.y;
    this->z = v.z;
    this->w = w;
  }

  v4(float w, v3 v)
  {
    *this = v4(v, w);
  }

  v4( float x, float y, float z, float w)
  {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
  }

  r32&
  operator[](int index)
  {
    r32& Result = this->E[index];
    return Result;
  }

};

v4
V4(v3 V, r32 w)
{
  v4 Result(V, w);
  return Result;
}

v4 V4(float x, float y, float z, float w)
{
  v4 Result;

  Result.x = x;
  Result.y = y;
  Result.z = z;
  Result.w = w;

  return Result;
}

typedef v4 Quaternion;


struct m4
{
  v4 E[4];

  v4&
  operator[](int index)
  {
    v4& Result = this->E[index];
    return Result;
  }
};

inline glm::mat4
GLM4(m4 M)
{
  glm::mat4 Result(
    glm::vec4(M[0][0], M[0][1], M[0][2], M[0][3]),
    glm::vec4(M[1][0], M[1][1], M[1][2], M[1][3]),
    glm::vec4(M[2][0], M[2][1], M[2][2], M[2][3]),
    glm::vec4(M[3][0], M[3][1], M[3][2], M[3][3])
  );

  return Result;
}

inline m4
GLM4(glm::mat4 M)
{
  m4 Result = {
    V4(M[0][0], M[0][1], M[0][2], M[0][3]),
    V4(M[1][0], M[1][1], M[1][2], M[1][3]),
    V4(M[2][0], M[2][1], M[2][2], M[2][3]),
    V4(M[3][0], M[3][1], M[3][2], M[3][3]),
  };

  return Result;
}

#define Col0(M, N) M[N][0]
#define Col1(M, N) M[N][1]
#define Col2(M, N) M[N][2]
#define Col3(M, N) M[N][3]

#define Mul( RowN, B ) V4( \
  RowN[0]*Col0(B,0) + RowN[1]*Col0(B,1) + RowN[2]*Col0(B,2) + RowN[3]*Col0(B,3), \
  RowN[0]*Col1(B,0) + RowN[1]*Col1(B,1) + RowN[2]*Col1(B,2) + RowN[3]*Col1(B,3), \
  RowN[0]*Col2(B,0) + RowN[1]*Col2(B,1) + RowN[2]*Col2(B,2) + RowN[3]*Col2(B,3), \
  RowN[0]*Col3(B,0) + RowN[1]*Col3(B,1) + RowN[2]*Col3(B,2) + RowN[3]*Col3(B,3) )

m4
operator*(m4 A, m4 B)
{

  glm::mat4 MA = GLM4(A);
  glm::mat4 MB = GLM4(B);
  m4 GlmResult = GLM4(MA * MB);


  /* v4 A0 = A[0]; */
  /* v4 A1 = A[1]; */
  /* v4 A2 = A[2]; */
  /* v4 A3 = A[3]; */

  /* v4 B0 = B[0]; */
  /* v4 B1 = B[1]; */
  /* v4 B2 = B[2]; */
  /* v4 B3 = B[3]; */


  m4 Result = {};

  // OpenGL matrices 
  Result[0].w = A[0][0]*B[0][0] + A[0][1]*B[1][0] + A[0][2]*B[2][0] + A[0][3]*B[3][0];
  Result[1].w = A[0][0]*B[0][1] + A[0][1]*B[1][1] + A[0][2]*B[2][1] + A[0][3]*B[3][1];
  Result[2].w = A[0][0]*B[0][2] + A[0][1]*B[1][2] + A[0][2]*B[2][2] + A[0][3]*B[3][2];
  Result[3].w = A[0][0]*B[0][3] + A[0][1]*B[1][3] + A[0][2]*B[2][3] + A[0][3]*B[3][3];


  GlmResult = GLM4(MA * MB);
  return GlmResult;
}

Quaternion
operator*(Quaternion A, Quaternion B)
{
  Quaternion Result(0,0,0,0);

  Result.w = A.w*B.w - A.x*B.x - A.y*B.y - A.z*B.z;
  Result.x = A.w*B.x + A.x*B.w + A.y*B.z - A.z*B.y;
  Result.y = A.w*B.y + A.y*B.w + A.z*B.x - A.x*B.z;
  Result.z = A.w*B.z + A.z*B.w + A.x*B.y - A.y*B.x;

  return Result;
}

struct voxel_position
{
  s32 x;
  s32 y;
  s32 z;
};

inline voxel_position
Voxel_Position(v3 Offset)
{
  voxel_position Result;

  Result.x = (int)Offset.x;
  Result.y = (int)Offset.y;
  Result.z = (int)Offset.z;

  return Result;
}


inline bool
operator==(v2 P1, v2 P2)
{
  bool Result = ( P1.x == P2.x && P1.y == P2.y);
  return Result;
}

inline bool
operator!=(v2 P1, v2 P2)
{
  bool Result = !(P1 == P2);
  return Result;
}
inline bool
operator==(v3 P1, v3 P2)
{
  bool Result;

  Result = (
    P1.x == P2.x &&
    P1.y == P2.y &&
    P1.z == P2.z );

  return Result;
}

inline bool
operator!=(v3 P1, v3 P2)
{
  bool Result = !(P1 == P2);
  return Result;
}

inline bool
operator==(voxel_position P1, voxel_position P2)
{
  bool Result;

  Result = (
    P1.x == P2.x &&
    P1.y == P2.y &&
    P1.z == P2.z );

  return Result;
}

inline bool
operator!=(voxel_position P1, voxel_position P2)
{
  bool Result = !(P1 == P2);
  return Result;
}

inline voxel_position
operator-(voxel_position P1, int i)
{
  voxel_position Result;

  Result.x = P1.x - i;
  Result.y = P1.y - i;
  Result.z = P1.z - i;

  return Result;
}

inline voxel_position
operator*(voxel_position P1, int i)
{
  voxel_position Result;

  Result.x = P1.x * i;
  Result.y = P1.y * i;
  Result.z = P1.z * i;

  return Result;
}

inline voxel_position
operator*(int i, voxel_position P1)
{
  voxel_position Result = P1*i;
  return Result;
}

inline voxel_position
operator*(voxel_position P1, voxel_position P2)
{
  voxel_position Result;

  Result.x = P2.x * P1.x;
  Result.y = P2.y * P1.y;
  Result.z = P2.z * P1.z;

  return Result;
}

inline v3
operator*(v3 P1, voxel_position P2)
{
  v3 Result;

  Result.x = (float)P2.x * P1.x;
  Result.y = (float)P2.y * P1.y;
  Result.z = (float)P2.z * P1.z;

  return Result;
}

inline v3
operator+(v3 Vec, voxel_position Pos)
{
  v3 Result;

  Result.x = Vec.x + Pos.x;
  Result.y = Vec.y + Pos.y;
  Result.z = Vec.z + Pos.z;

  return Result;
}

inline voxel_position
operator+(voxel_position Pos, v3 Vec)
{
  voxel_position Result = Voxel_Position(Vec + Pos);
  return Result;
}

inline voxel_position
operator-(voxel_position Pos, v3 Vec)
{
  voxel_position Result;

  Result.x = Pos.x- (int)Vec.x;
  Result.y = Pos.y- (int)Vec.y;
  Result.z = Pos.z- (int)Vec.z;

  return Result;
}

inline v3
operator-(v3 Vec, voxel_position Pos)
{
  v3 Result;

  Result.x = Vec.x - Pos.x;
  Result.y = Vec.y - Pos.y;
  Result.z = Vec.z - Pos.z;

  return Result;
}

inline voxel_position
operator+(voxel_position P1, int i)
{
  voxel_position Result;

  Result.x = P1.x + i;
  Result.y = P1.y + i;
  Result.z = P1.z + i;

  return Result;
}

inline voxel_position
operator+(voxel_position P1, voxel_position P2)
{
  voxel_position Result;

  Result.x = P2.x + P1.x;
  Result.y = P2.y + P1.y;
  Result.z = P2.z + P1.z;

  return Result;
}

inline voxel_position
operator-(voxel_position P1, voxel_position P2)
{
  voxel_position Result;

  Result.x = P1.x - P2.x;
  Result.y = P1.y - P2.y;
  Result.z = P1.z - P2.z;

  return Result;
}

inline bool
operator<(v3 P1, v3 P2)
{
  bool Result = true;

  Result &= P1.x < P2.x;
  Result &= P1.y < P2.y;
  Result &= P1.z < P2.z;

  return Result;
}

inline bool
operator>(v3 P1, v3 P2)
{
  bool Result = true;

  Result &= P1.x > P2.x;
  Result &= P1.y > P2.y;
  Result &= P1.z > P2.z;

  return Result;
}

inline bool
operator<(voxel_position P1, voxel_position P2)
{
  bool Result = true;

  Result &= P1.x < P2.x;
  Result &= P1.y < P2.y;
  Result &= P1.z < P2.z;

  return Result;
}

inline bool
operator>(voxel_position P1, voxel_position P2)
{
  bool Result = true;

  Result &= P1.x > P2.x;
  Result &= P1.y > P2.y;
  Result &= P1.z > P2.z;

  return Result;
}

inline bool
operator<=(voxel_position P1, voxel_position P2)
{
  bool Result = true;

  Result &= P1.x <= P2.x;
  Result &= P1.y <= P2.y;
  Result &= P1.z <= P2.z;

  return Result;
}

inline bool
operator>=(voxel_position P1, voxel_position P2)
{
  bool Result = true;

  Result &= P1.x >= P2.x;
  Result &= P1.y >= P2.y;
  Result &= P1.z >= P2.z;

  return Result;
}

inline v3
operator*(voxel_position P1, float f)
{
  v3 Result;

  Result.x = P1.x * f;
  Result.y = P1.y * f;
  Result.z = P1.z * f;

  return Result;
}

typedef voxel_position chunk_dimension;
typedef voxel_position world_position;

struct canonical_position
{
  v3 Offset;
  world_position WorldP;
};

inline v3
V3(r32 I)
{
  v3 Result;

  Result.x = I;
  Result.y = I;
  Result.z = I;

  return Result;
}

inline v3
V3(s32 I)
{
  v3 Result;

  Result.x = (r32)I;
  Result.y = (r32)I;
  Result.z = (r32)I;

  return Result;
}

inline v3
V3(voxel_position wp)
{
  v3 Result;

  Result.x = (float)wp.x;
  Result.y = (float)wp.y;
  Result.z = (float)wp.z;

  return Result;
}

inline v3
V3(int x, int y, int z)
{
  v3 Result = {};

  Result.x = (float)x;
  Result.y = (float)y;
  Result.z = (float)z;

  return Result;
}

inline v3
V3(float x, float y, float z)
{
  v3 Result = {};

  Result.x = x;
  Result.y = y;
  Result.z = z;

  return Result;
}

inline v3
operator+(v3 A, v3 B)
{
  v3 Result;

  Result.x = A.x + B.x;
  Result.y = A.y + B.y;
  Result.z = A.z + B.z;

  return Result;
}

struct linei
{
  voxel_position MinP;
  voxel_position MaxP;

  linei() {}

  linei(voxel_position MinP, voxel_position MaxP)
  {
    this->MinP = MinP;
    this->MaxP = MaxP;
  }

  linei(v3 MinP, v3 MaxP)
  {
    this->MinP = Voxel_Position(MinP);
    this->MaxP = Voxel_Position(MaxP);
  }

};

struct line
{
  v3 MinP;
  v3 MaxP;

  line() {}

  line(v3 MinP, v3 MaxP)
  {
    this->MinP = MinP;
    this->MaxP = MaxP;
  }

  line(voxel_position MinP, voxel_position MaxP)
  {
    this->MinP = V3(MinP);
    this->MaxP = V3(MaxP);
  }
};

struct rect2
{
  v2 Min;
  v2 Max;
};

struct aabb
{
  v3 Center;
  v3 Radius;

  aabb(v3 Center, v3 Radius)
  {
    this->Center = Center;
    this->Radius = Radius;
  }

  aabb(world_position Center, world_position Radius)
  {
    this->Center = V3(Center);
    this->Radius = V3(Radius);
  }

  aabb()
  {
    this->Center = V3(0,0,0);
    this->Radius = V3(0,0,0);
  }

};

#define POINT_BUFFER_SIZE 8
struct point_buffer
{
  s32 Count;
  voxel_position Points[POINT_BUFFER_SIZE];
};

inline aabb
operator+(aabb AABB, v3 V)
{
  aabb Result = AABB;
  Result.Radius = AABB.Radius + V;
  return Result;
}

inline m4
Translate( v3 v )
{
#if 1
  m4 Result;

  Result.E[0] = v4(1,0,0,v.x);
  Result.E[1] = v4(0,1,0,v.y);
  Result.E[2] = v4(0,0,1,v.z);
  Result.E[3] = v4(0,0,0,1);
#else
  glm::mat4 TransMatrix = glm::translate(glm::mat4(), glm::vec3(v.x, v.y, v.z));
  m4 Result = GLM4(TransMatrix);


#endif

  return Result;
}

inline world_position
World_Position(v3 V)
{
  voxel_position Result;

  Result.x = (int)V.x;
  Result.y = (int)V.y;
  Result.z = (int)V.z;

  return Result;
};

inline voxel_position
Voxel_Position(int P)
{
  voxel_position Result;

  Result.x = P;
  Result.y = P;
  Result.z = P;

  return Result;
};

inline voxel_position
Voxel_Position(int x, int y, int z)
{
  voxel_position Result;

  Result.x = x;
  Result.y = y;
  Result.z = z;

  return Result;
};

inline world_position
World_Position(s32 P)
{
  chunk_dimension Result = Voxel_Position(P);
  return Result;
}

inline world_position
World_Position(int x, int y, int z)
{
  chunk_dimension Result = Voxel_Position(x,y,z);
  return Result;
}


inline canonical_position
Canonical_Position(s32 I)
{
  canonical_position Result;
  Result.Offset = V3(I);
  Result.WorldP = World_Position(I);
  return Result;
}

inline canonical_position
Canonical_Position()
{
  canonical_position Result;
  Result.Offset = V3(0,0,0);
  Result.WorldP = World_Position(0,0,0);
  return Result;
}

inline canonical_position
Canonical_Position(v3 Offset, world_position WorldP )
{
  canonical_position Result;
  Result.Offset = Offset;
  Result.WorldP = WorldP;
  return Result;
}

inline canonical_position
Canonical_Position(voxel_position Offset, world_position WorldP )
{
  canonical_position Result;
  Result.Offset = V3(Offset);
  Result.WorldP = WorldP;
  return Result;
}

// NOTE : The maximum bound is non-inclusive; 0 is part of the chunk
// while the furthest point in x,y or z is the next chunk
inline canonical_position
Canonicalize( chunk_dimension WorldChunkDim, v3 Offset, world_position WorldP )
{
  canonical_position Result;

  Result.Offset = Offset;
  Result.WorldP = WorldP;

  if ( Result.Offset.x >= WorldChunkDim.x )
  {
    int ChunkWidths = (int)(Result.Offset.x / WorldChunkDim.x);
    Result.Offset.x -= WorldChunkDim.x*ChunkWidths;
    Result.WorldP.x += ChunkWidths;
  }
  if ( Result.Offset.y >= WorldChunkDim.y )
  {
    int ChunkWidths = (int)(Result.Offset.y / WorldChunkDim.y);
    Result.Offset.y -= WorldChunkDim.y*ChunkWidths;
    Result.WorldP.y += ChunkWidths;
  }
  if ( Result.Offset.z >= WorldChunkDim.z )
  {
    int ChunkWidths = (int)(Result.Offset.z / WorldChunkDim.z);
    Result.Offset.z -= WorldChunkDim.z*ChunkWidths;
    Result.WorldP.z += ChunkWidths;
  }

  if ( Result.Offset.x < 0 )
  {
    int ChunkWidths = (int)((Result.Offset.x-WorldChunkDim.x) / -WorldChunkDim.x);
    Result.Offset.x += WorldChunkDim.x*ChunkWidths;
    Result.WorldP.x -= ChunkWidths;
  }
  if ( Result.Offset.y < 0 )
  {
    int ChunkWidths = (int)((Result.Offset.y-WorldChunkDim.y) / -WorldChunkDim.y);
    Result.Offset.y += WorldChunkDim.y*ChunkWidths;
    Result.WorldP.y -= ChunkWidths;
  }
  if ( Result.Offset.z < 0 )
  {
    int ChunkWidths = (int)((Result.Offset.z-WorldChunkDim.z) / -WorldChunkDim.z);
    Result.Offset.z += WorldChunkDim.z*ChunkWidths;
    Result.WorldP.z -= ChunkWidths;
  }

  return Result;
}

inline canonical_position
Canonicalize( chunk_dimension WorldChunkDim, canonical_position CP )
{
  canonical_position Result = Canonicalize( WorldChunkDim, CP.Offset, CP.WorldP );
  return Result;
}

inline canonical_position
Canonical_Position( chunk_dimension WorldChunkDim, v3 Offset, world_position WorldP )
{
  canonical_position Result = Canonical_Position(Offset, WorldP);
  Result = Canonicalize(WorldChunkDim, Result);
  return Result;
}

inline canonical_position
Canonical_Position(chunk_dimension WorldChunkDim, voxel_position Offset, world_position WorldP )
{
  canonical_position Result = Canonical_Position(Offset, WorldP);
  Result = Canonicalize(WorldChunkDim, Result);
  return Result;
}

inline chunk_dimension
Chunk_Dimension(int x, int y, int z)
{
  chunk_dimension Result = Voxel_Position(x,y,z);
  return Result;
}

inline int
Volume(v3 Dim)
{
  return (Dim.x*Dim.y*Dim.z);
}

inline int
Volume(chunk_dimension Dim)
{
  return (Dim.x*Dim.y*Dim.z);
}

v2 V2(float x,float y)
{
  v2 Result = {x,y};
  return Result;
}

v2i
V2i(s32 x, s32 y)
{
  v2i Result = {x,y};
  return Result;
}

v2 operator+=(v2 P1, v2 P2)
{
  v2 Result;

  Result.x = P1.x + P2.x;
  Result.y = P1.y + P2.y;

  return Result;
}

v2
operator*(v2 P1, r32 F)
{
  v2 Result;

  Result.x = P1.x * F;
  Result.y = P1.y * F;

  return Result;
}

v2
operator-(v2 P1, v2 P2)
{
  v2 Result;

  Result.x = P1.x - P2.x;
  Result.y = P1.y - P2.y;

  return Result;
}

v2 operator+(v2 P1, v2 P2)
{
  v2 Result;

  Result.x = P1.x + P2.x;
  Result.y = P1.y + P2.y;

  return Result;
}

v2
operator*(float f, v2 P)
{
  v2 Result;

  Result.x = P.x *= f;
  Result.y = P.y *= f;

  return Result;
}

inline canonical_position
operator-(canonical_position A, v3 B)
{
  canonical_position Result = A;

  Result.Offset.x = A.Offset.x - B.x;
  Result.Offset.y = A.Offset.y - B.y;
  Result.Offset.z = A.Offset.z - B.z;

  return Result;
}

inline canonical_position
operator+(canonical_position A, v3 B)
{
  canonical_position Result = A;

  Result.Offset.x = A.Offset.x + B.x;
  Result.Offset.y = A.Offset.y + B.y;
  Result.Offset.z = A.Offset.z + B.z;

  return Result;
}

inline v3
operator%(v3 A, int i)
{
  v3 Result;

  Result.x = (float)((int)A.x % i);
  Result.y = (float)((int)A.y % i);
  Result.z = (float)((int)A.z % i);

  return Result;
}

inline v3
operator/(v3 A, r32 B)
{
  v3 Result;

  Result.x = A.x / B;
  Result.y = A.y / B;
  Result.z = A.z / B;

  return Result;
}

inline v3
operator/(v3 A, v3 B)
{
  v3 Result;

  Result.x = A.x / B.x;
  Result.y = A.y / B.y;
  Result.z = A.z / B.z;

  return Result;
}

inline v3
operator/(v3 A, int B)
{
  v3 Result;

  Result.x = A.x / (float)B;
  Result.y = A.y / (float)B;
  Result.z = A.z / (float)B;

  return Result;
}

inline v3
operator/(voxel_position A, r32 f)
{
  v3 Result;

  Result.x = A.x / f;
  Result.y = A.y / f;
  Result.z = A.z / f;

  return Result;
}


inline voxel_position
operator/(voxel_position A, s32 i)
{
  voxel_position Result;

  Result.x = A.x / i;
  Result.y = A.y / i;
  Result.z = A.z / i;

  return Result;
}

inline v3
operator/(v3 A, voxel_position B)
{
  v3 Result;

  Result.x = A.x / B.x;
  Result.y = A.y / B.y;
  Result.z = A.z / B.z;

  return Result;
}

inline voxel_position&
operator-=(voxel_position& A, voxel_position B)
{
  A.x -= B.x;
  A.y -= B.y;
  A.z -= B.z;

  return(A);
}

inline voxel_position&
operator+=(voxel_position& A, voxel_position B)
{
  A.x += B.x;
  A.y += B.y;
  A.z += B.z;

  return(A);
}

inline v3&
operator+=(v3& A, float f)
{
  A.x += f;
  A.y += f;
  A.z += f;

  return(A);
}

inline v3&
operator+=(v3& A, v3 B)
{
  A = A + B;
  return(A);
}

inline canonical_position&
operator+=(canonical_position& A, float B)
{
	A.Offset += B;
  return(A);
}

inline canonical_position&
operator+=(canonical_position& A, v3 B)
{
	A.Offset += B;
  return(A);
}

inline canonical_position&
operator+=(canonical_position& A, canonical_position B)
{
  A.Offset += B.Offset;
  A.WorldP += B.WorldP;
  return(A);
}

inline v3
operator-(v3 A, float f)
{
  v3 Result;

  Result.x = A.x - f;
  Result.y = A.y - f;
  Result.z = A.z - f;

  return Result;
}

inline v3
operator+(v3 A, float f)
{
  v3 Result;

  Result.x = A.x + f;
  Result.y = A.y + f;
  Result.z = A.z + f;

  return Result;
}

inline v3
operator+(float f, v3 A)
{
  v3 Result = A+f;
  return Result;
}

inline v3
operator-(v3 A, v3 B)
{
  v3 Result;

  Result.x = A.x - B.x;
  Result.y = A.y - B.y;
  Result.z = A.z - B.z;

  return Result;
}

inline line
operator+(line A, v3 B)
{
  line Result;

  Result.MinP.x = A.MinP.x + B.x;
  Result.MinP.y = A.MinP.y + B.y;
  Result.MinP.z = A.MinP.z + B.z;

  Result.MaxP.x = A.MaxP.x + B.x;
  Result.MaxP.y = A.MaxP.y + B.y;
  Result.MaxP.z = A.MaxP.z + B.z;

  return Result;
}

inline line
operator-(line A, v3 B)
{
  line Result;

  Result.MinP.x = A.MinP.x - B.x;
  Result.MinP.y = A.MinP.y - B.y;
  Result.MinP.z = A.MinP.z - B.z;

  Result.MaxP.x = A.MaxP.x - B.x;
  Result.MaxP.y = A.MaxP.y - B.y;
  Result.MaxP.z = A.MaxP.z - B.z;

  return Result;
}

inline v3&
operator-=(v3& A, v3 B)
{
  A = A - B;
  return(A);
}

inline v3
operator*(v3 A, v3 B)
{
  v3 Result;

  Result.x = A.x * B.x;
  Result.y = A.y * B.y;
  Result.z = A.z * B.z;

  return Result;
}

inline v3
operator*(v3 A, float f)
{
  v3 Result;

  Result.x = A.x * f;
  Result.y = A.y * f;
  Result.z = A.z * f;

  return Result;
}

inline v3
operator*(float f, v3 A)
{
  v3 Result = A * f;
  return Result;
}

inline v3&
operator*=(v3& A, float f)
{
  A.x = A.x * f;
  A.y = A.y * f;
  A.z = A.z * f;

  return A;
}

inline v3&
operator/=(v3& A, float f)
{
  A.x = A.x / f;
  A.y = A.y / f;
  A.z = A.z / f;

  return A;
}

v4 operator*(v4 A, int B)
{
  v4 Result(0,0,0,0);

  Result.x = A.x * (float)B;
  Result.y = A.y * (float)B;
  Result.z = A.z * (float)B;
  Result.w = A.w * (float)B;

  return Result;
}

v4 operator*=(v4 A, int B)
{
  A = A * B;
  return A;
}

v4 operator+(v4 A, v4 B)
{
  v4 Result(0,0,0,0);

  Result.x = A.x + B.x;
  Result.y = A.y + B.y;
  Result.z = A.z + B.z;
  Result.w = A.w + B.w;

  return Result;
}

inline canonical_position
operator-(canonical_position P1, canonical_position P2)
{
  canonical_position Result;

  Result.Offset = P1.Offset - P2.Offset;
  Result.WorldP = P1.WorldP - P2.WorldP;

  return Result;
}

inline voxel_position
operator&(voxel_position P1, voxel_position P2)
{
  voxel_position Result;

  Result.x = P1.x & (s32)P2.x;
  Result.y = P1.y & (s32)P2.y;
  Result.z = P1.z & (s32)P2.z;

  return Result;
}

inline v3
operator^(v3 P1, v3 P2)
{
  v3 Result;

  Result.x = (r32)((s32)P1.x ^ (s32)P2.x);
  Result.y = (r32)((s32)P1.y ^ (s32)P2.y);
  Result.z = (r32)((s32)P1.z ^ (s32)P2.z);

  return Result;
}

inline voxel_position
operator^(voxel_position P1, voxel_position P2)
{
  voxel_position Result;

  Result.x = P1.x ^ P2.x;
  Result.y = P1.y ^ P2.y;
  Result.z = P1.z ^ P2.z;

  return Result;
}

inline Quaternion
Conjugate( Quaternion q )
{
  Quaternion Result(-q.x, -q.y, -q.z, q.w);
  return Result;
}

v3
HalfDim( v3 P1 )
{
  v3 Result = P1 / 2;
  return Result;
}

inline u64
Kilobytes(u32 Bytes)
{
  u64 Result = Bytes * 1024;
  return Result;
}

inline u64
Megabytes(u32 Number)
{
  u64 Result = Number*Kilobytes(1024);
  return Result;
}

inline u64
Gigabytes(u32 Number)
{
  u64 Result = Number*Megabytes(1024);
  return Result;
}

inline u64
Terabytes(u32 Number)
{
  u64 Result = Number*Gigabytes(1024);
  return Result;
}

#endif
