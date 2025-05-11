#include "game-map.h"
#include <stdlib.h>

void map_init(GameMap *map, unsigned int nChunks) {
  glGenBuffers(1, &map->ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, map->ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER, SIZEOF_CHUNK * nChunks, NULL,
               GL_STATIC_DRAW);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

  map->chunks = malloc(SIZEOF_CHUNK * nChunks);

  map->size = nChunks;
}

void map_deinit(GameMap *map) { glDeleteBuffers(1, &map->ssbo); }

void map_sync(GameMap *map) {
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, map->ssbo);
  glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, SIZEOF_CHUNK * map->size,
                  map->chunks);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void map_sync_chunk(GameMap *map, Chunk *chunk) {
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, map->ssbo);
  glBufferSubData(GL_SHADER_STORAGE_BUFFER, chunk - map->chunks, SIZEOF_CHUNK,
                  chunk);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

Chunk *map_get_chunk(GameMap *map, uint16_t x, uint16_t y) {
  for (Chunk *chunk = map->chunks; chunk < map->chunks + map->size; ++chunk) {
    if (chunk->x == x && chunk->y == y)
      return chunk;
  }
  return NULL;
}