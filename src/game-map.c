#include "game-map.h"
#include <stdlib.h>

void map_create(GameMap *map, unsigned int nChunks) {
  glGenBuffers(1, &map->ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, map->ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER, SIZEOF_CHUNK * nChunks, NULL,
               GL_STATIC_DRAW);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void map_release(GameMap *map) { glDeleteBuffers(1, &map->ssbo); }