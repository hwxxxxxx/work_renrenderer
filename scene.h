#pragma once

#include "api.h"
#include "gameobject.h"
#include "camera.h"
#include "platform.h"
#include "IShader.h"

static Color AMBIENT = Color(54.f / 255, 58.f / 255, 66.f / 255);
static Color LightColor = Color(255.f / 255, 244.f / 255, 214.f / 255);

class DrawModel {
public:
	GameObject* light;
	GameObject_StaticModel* gameobject;
	Material* material;
	ShaderData* shader_data;
	DrawData* shadow_draw_data;
	DrawData* draw_data;
	RenderBuffer* shdaow_map;
	IShader* shader;
	ShadowShader* shadow_shader;


	DrawModel(GameObject* light, GameObject_StaticModel* go, Material* material, IShader* shader)
	{
		this->light = light;
		this->gameobject = go;
		this->material = material;
		this->shader = shader;

		shader_data = new ShaderData();
		shader_data->matrial = material;

		shadow_draw_data = nullptr;
		shadow_shader = nullptr;
		shdaow_map = nullptr;

		draw_data = new DrawData();
		draw_data->model = gameobject->model;
		draw_data->shader = shader;
		draw_data->shader->shader_data = shader_data;
	}

	~DrawModel()
	{
		delete shader_data;
		delete draw_data;
		if (shadow_shader) delete shadow_shader;
		if (shdaow_map)  delete shdaow_map;
		if (shadow_draw_data)  delete shadow_draw_data;
	}

	int	draw(Camera* camera, RenderBuffer* frameBuffer, bool enable_shadow)
	{
		int total_triangles = 0;
		Matrix4x4 view_matrix = camera->get_view_matrix();
		Matrix4x4 projection_matrix = camera->get_proj_matrix();
		Matrix4x4 model_matrix = gameobject->GetModelMatrix();
		Matrix4x4 model_matrix_I = model_matrix.invert();

		// 设置摄像机的位置
		shader_data->view_Pos = camera->get_position();
		// 计算光源方向：从光源位置指向摄像机目标位置，并进行归一化
		shader_data->light_dir = (light->transform.position - camera->get_target_position()).normalize();
		// 设置光源的颜色
		shader_data->light_color = LightColor;
		// 设置环境光强度
		shader_data->ambient = AMBIENT;
		// 设置模型矩阵（将物体从局部坐标系变换到世界坐标系）
		shader_data->model_matrix = model_matrix;
		// 设置模型矩阵的逆矩阵（用于法线变换等操作）
		shader_data->model_matrix_I = model_matrix_I;
		// 设置视图矩阵（将物体从世界坐标系变换到摄像机坐标系）
		shader_data->view_matrix = view_matrix;
		// 设置投影矩阵（将物体从摄像机坐标系变换到裁剪空间）
		shader_data->projection_matrix = projection_matrix;
		// 计算宽高比（用于正交投影矩阵的计算）
		float aspect = (float)frameBuffer->width / (float)frameBuffer->height;
		// 计算光源的视图投影矩阵：
		// 1. 使用正交投影矩阵（orthographic）定义光源的视锥体
		// 2. 使用 lookat 矩阵定义光源的视图矩阵（从光源位置看向摄像机目标位置）
		// 3. 将正交投影矩阵和视图矩阵相乘，得到光源的视图投影矩阵
		shader_data->light_vp_matrix = orthographic(aspect, 1, 0, 5) * lookat(light->transform.position, camera->get_target_position(), Vector3f(0, 1, 0));
		// 计算摄像机的视图投影矩阵：
		// 将投影矩阵和视图矩阵相乘，得到摄像机的视图投影矩阵
		shader_data->camera_vp_matrix = projection_matrix * view_matrix;
		// 设置是否启用阴影
		shader_data->enable_shadow = enable_shadow;

		if (enable_shadow)
		{
			if (!shadow_draw_data)
			{
				shadow_shader = new ShadowShader();
				shdaow_map = new RenderBuffer(frameBuffer->width, frameBuffer->height);
				shadow_draw_data = new DrawData();
				shadow_draw_data->model = gameobject->model;
				shadow_draw_data->shader = shadow_shader;
				shadow_draw_data->shader->shader_data = shader_data;
				shadow_draw_data->render_buffer = shdaow_map;
			}

			total_triangles += graphics_draw_triangle(shadow_draw_data);
			shader_data->shadow_map = shdaow_map;
		}

		draw_data->render_buffer = frameBuffer;
		total_triangles += graphics_draw_triangle(draw_data);


		if (enable_shadow)
		{
			shdaow_map->renderbuffer_clear_color(Color::Black);
			shdaow_map->renderbuffer_clear_depth(std::numeric_limits<float>::max());
		}
		return total_triangles;
	}
};

struct ShaderInfo {
	const char* des;
	IShader* shader;
};

class Scene {
public:
	GameObject* light;
	Camera* camera;
	RenderBuffer* frameBuffer;
	char text[500];
	bool enable_shadow;
	Scene(RenderBuffer* frameBuffer);
	~Scene();

	virtual int tick(float delta_time);
	virtual void on_key_input(keycode_t key, int pressed);
	const char* format_degrees(float radians);
	char* get_text();
};

class SingleModelScene :public Scene {
private:
	GameObject_StaticModel* gameobject;
	Material* material;
	ShaderInfo shaderInfos[6];
	DrawModel* draw_model;
	int cur_shader_index;
public:
	SingleModelScene(const char* file_name, RenderBuffer* render_Buffer);
	~SingleModelScene();

	int tick(float delta_time) override;
	void on_key_input(keycode_t key, int pressed) override;
};