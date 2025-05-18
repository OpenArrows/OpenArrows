#include "arrows.h"
#include <glad/gl.h>

struct GameMap {
  unsigned int size;
  struct {
    // Instead of continuously copying data across GPU and CPU, we store the map
    // on the CPU and copy the updates to the GPU. This works because we only
    // modify map (place and remove arrows and chunks) on the host
    GLuint ssbo;   // GPU
    Chunk *chunks; // CPU
    GLuint bufferIndex;
  } map;
  struct {
    // But dynamic map data, such as signal states, is stored in another buffer
    // to allow for better optimization
    GLuint ssbo;
    GLuint bufferIndex;
  } state;
};
typedef struct GameMap GameMap;

void map_init(GameMap *map);

void map_deinit(GameMap *map);

void map_sync(GameMap *map);

void map_sync_chunk(GameMap *map, Chunk *chunk);

Chunk *map_get_chunk(GameMap *map, uint16_t x, uint16_t y);