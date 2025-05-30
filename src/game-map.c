#include "game-map.h"
#include <stdlib.h>

void map_init(GameMap *map) {
  map->map.chunks = calloc(map->size, SIZEOF_CHUNK);
  // TODO: handle malloc failure

  glGenBuffers(1, &map->map.ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, map->map.ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER, SIZEOF_CHUNK * map->size,
               map->map.chunks, GL_STATIC_DRAW);

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, map->map.bufferIndex,
                   map->map.ssbo);

  glGenBuffers(1, &map->state.ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, map->state.ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               SIZEOF_ARROW_STATE * 2 * CHUNK_SIZE * CHUNK_SIZE * map->size,
               NULL, GL_STATIC_READ);
  glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R8UI, GL_RED_INTEGER,
                    GL_UNSIGNED_BYTE, NULL);

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, map->state.bufferIndex,
                   map->state.ssbo);
}

void map_deinit(GameMap *map) {
  glDeleteBuffers(1, &map->map.ssbo);
  glDeleteBuffers(1, &map->state.ssbo);
  free(map->map.chunks);
}

void map_sync(GameMap *map) {
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, map->map.ssbo);
  glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, SIZEOF_CHUNK * map->size,
                  map->map.chunks);
}

void map_sync_chunk(GameMap *map, Chunk *chunk) {
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, map->map.ssbo);
  glBufferSubData(GL_SHADER_STORAGE_BUFFER, chunk - map->map.chunks,
                  SIZEOF_CHUNK, chunk);
}

Chunk *map_get_chunk(GameMap *map, uint16_t x, uint16_t y) {
  for (Chunk *chunk = map->map.chunks; chunk < map->map.chunks + map->size;
       ++chunk) {
    if (chunk->x == x && chunk->y == y)
      return chunk;
  }
  return NULL;
}