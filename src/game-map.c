#include "game-map.h"
#include <stdlib.h>

void map_create(GameMap *map, unsigned int nChunks) {
  glGenBuffers(1, &map->ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, map->ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER, SIZEOF_CHUNK * nChunks, NULL,
               GL_STATIC_DRAW);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

  map->chunks = malloc(SIZEOF_CHUNK *
                       nChunks); // TODO: is it better to just map the memory?

  map->size = nChunks;
}

void map_release(GameMap *map) { glDeleteBuffers(1, &map->ssbo); }

Chunk *map_get_chunk(GameMap *map, uint32_t x, uint32_t y) {
  for (Chunk *chunk = map->chunks; chunk < map->chunks + map->size; ++chunk) {
    if (chunk->x == x && chunk->y == y)
      return chunk;
  }
  return NULL;
}