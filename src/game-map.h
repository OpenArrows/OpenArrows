#include "arrows.h"
#include <glad/gl.h>

struct GameMap {
  GLuint ssbo; // TODO: cache the map on the CPU. We will need to read the map
               // in the future (e.g. when running CPU-bound actions, sending
               // input, etc.), but copying data from GPU is slow; because we
               // should only modify map from the host, we can safely cache the
               // map and send updates to the GPU
};
typedef struct GameMap GameMap;

void map_create(GameMap *map, unsigned int nChunks);

void map_release(GameMap *map);