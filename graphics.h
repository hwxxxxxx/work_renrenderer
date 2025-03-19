#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "api.h"
#include "renderbuffer.h"
#include "model.h"
#include "IShader.h"

struct DrawData {
	Model* model;
	IShader* shader;
	RenderBuffer* render_buffer;
};

int graphics_draw_triangle(DrawData* app_data);
#endif
