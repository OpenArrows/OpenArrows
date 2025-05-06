#include "arrows.h"
#include <glad/gl.h>

struct GameMap {
  // Instead of continuously copying data across GPU and CPU, we store the map
  // on the CPU and copy the updates to the GPU. This works because we only
  // modify map (place and remove arrows and chunks) on the host
  GLuint ssbo;   // GPU
  Chunk *chunks; // CPU
  unsigned int size;
};
typedef struct GameMap GameMap;

void map_create(GameMap *map, unsigned int nChunks);

void map_release(GameMap *map);

Chunk *map_get_chunk(GameMap *map, uint32_t x, uint32_t y);