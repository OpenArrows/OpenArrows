// Definitions of game map elements' in-memory representation (arrow states,
// chunks, etc.)

// This header is intended to be used both from C and GLSL, so we need to
// provide some layer of compatiblity
#ifdef GLSL
// Enums are not a thing in GLSL. Instead, use uint constants
#define ENUM(NAME) const NAME
#define END_ENUM(NAME)
#else
#include <stdint.h>

#define ENUM(NAME) typedef enum NAME : unsigned int {
#define END_ENUM(NAME)                                                         \
  }                                                                            \
  NAME

// GLSL uint is always a 32-bit unsigned integer
typedef uint32_t uint;
#endif

#define CHUNK_SIZE 16

// GLSL doesn't support sizeof operator, so we need to define the size of each
// type by hand
#define SIZEOF_INT 4
#define SIZEOF_UINT 4

#ifdef GLSL
#define ArrowType uint
#endif
ENUM(ArrowType) Void = 0, SourceBlock = 1, WireArrow = 2 END_ENUM(ArrowType);

#ifdef GLSL
#define ArrowRotation uint // TODO: does rotation have to be 4 bytes?
#endif
ENUM(ArrowRotation)
Up = 0, Right = 1, Down = 2, Left = 3 END_ENUM(ArrowRotation);

#ifdef GLSL
#define ArrowSignal uint
#endif
ENUM(ArrowSignal) None = 0, Red = 1 END_ENUM(ArrowSignal);

struct Arrow {
  // TODO: can we store the arrow state in a more compact way?
  ArrowType type;
  ArrowRotation rotation;
  uint flipped;
};
#ifndef GLSL
typedef struct Arrow Arrow;
#endif

#define SIZEOF_ARROW                                                           \
  (/* type */ SIZEOF_UINT + /* rotation */ SIZEOF_UINT +                       \
   /* flipped */ SIZEOF_UINT)

struct ArrowState {
  ArrowSignal signal;
  uint received;
};
#ifndef GLSL
typedef struct ArrowState ArrowState;
#endif

#define SIZEOF_ARROW_STATE                                                     \
  (/* signal */ SIZEOF_UINT + /* received */ SIZEOF_UINT)

struct Chunk {
  // x and y are both 16-bit signed integers
#ifdef GLSL
  uint xy; // but such an integer type does not
           // exist in GLSL
#else
  // FIXME: is this device-consistent?
  int16_t x;
  int16_t y;
#endif
  Arrow arrows[CHUNK_SIZE * CHUNK_SIZE];
  uint adjacentChunks[8]; // It's better to cache adjacent chunks than make a
                          // look-up each time. Adjacent chunks are stored
                          // clockwise, starting from the top:
                          //    701
                          //    6#2
                          //    543
};
#ifndef GLSL
typedef struct Chunk Chunk;
#endif

#define SIZEOF_CHUNK                                                           \
  (/* xy */ SIZEOF_UINT +                                                      \
   /* arrows */ SIZEOF_ARROW * CHUNK_SIZE * CHUNK_SIZE +                       \
   /* adjacentChunks */ SIZEOF_UINT * 8)

#ifdef GLSL
const float PI = 3.1415926535897932384626433832795;

// Creates a GL matrix from arrow info
mat2 rot(in Arrow arrow) {
  // TODO: flip
  float a = float(arrow.rotation) * PI / 2.0;
  return mat2(cos(a), -sin(a), sin(a), cos(a));
}
#endif