#include "sgl.h"
#include <string.h>

void sglClear(uint32_t* buffer, int width, int height) {
	memset(buffer, 0, width * height * sizeof(uint32_t));
}

void sglSetPixel(uint32_t* buffer, int x, int y, size_t bufferWidth, uint32_t color) {
	buffer[x + y * bufferWidth] = color;
}

sglBuffer* sglCreateBuffer(int width, int height) {
	sglBuffer* b = malloc(sizeof(sglBuffer));
	b->width = width;
	b->height = height;
	b->data = malloc(width * height * sgl_member_size(sglBuffer, data));
	return b;
}
void sglDestroyBuffer(sglBuffer* buffer) {
	free(buffer->data);
	free(buffer);
}

