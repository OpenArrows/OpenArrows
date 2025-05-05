// Game map in-memory representation definitions (arrow types, signals, chunks,
// etc.)

// This header is intended to be used both from C and GLSL, so we need to
// provide some layer of compatiblity
#ifdef GLSL
// Enums are not a thing in GLSL. Instead, use uint constants
#define ENUM(NAME) const NAME
#define END_ENUM

// GLSL struct members can't have initializers, so it's a no-op
#define DEFVAL(VALUE)
#else
#define ENUM(NAME) enum NAME : unsigned int {
#define END_ENUM }

#define DEFVAL(VALUE) = VALUE

// `unsigned int` is `uint` in GLSL, but it's simpler to typedef on the C side
typedef unsigned int uint;
#endif

#define CHUNK_SIZE 16

#ifdef GLSL
#define ArrowType uint
#endif
ENUM(ArrowType) Void = 0 END_ENUM;

#ifdef GLSL
#define ArrowRotation uint
#endif
ENUM(ArrowRotation) Up = 0, Right = 1, Down = 2, Left = 3 END_ENUM;

#ifdef GLSL
#define ArrowSignal uint
#endif
ENUM(ArrowSignal) None = 0 END_ENUM;

struct Arrow {
  ArrowType type DEFVAL(Void);
  ArrowRotation rotation DEFVAL(Up); // TODO: does rotation have to be 4 bytes?
  ArrowSignal signal DEFVAL(None);
};
#ifndef GLSL
typedef struct Arrow Arrow;
#endif

// GLSL doesn't support sizeof operator, so we need to define the size of each
// type by hand
#define SIZEOF_ARROW (/* type */ 4 + /* rotation */ 4 + /* signal */ 4)

struct Chunk {
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
  (/* arrows */ SIZEOF_ARROW * CHUNK_SIZE * CHUNK_SIZE +                       \
   /* adjacentChunks */ 4 * 8)