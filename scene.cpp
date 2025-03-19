#include "scene.h"

Scene::Scene(RenderBuffer* render_Buffer) {
	this->frameBuffer = render_Buffer;
	float aspect = (float)this->frameBuffer->width / (float)this->frameBuffer->height;
	camera = new Camera(CAMERA_POSITION, CAMERA_TARGET, aspect);
	light = new GameObject(Vector3f(2, 2, 2));
	enable_shadow = true;
}

int Scene::tick(float delta_time) {
	// 获取相机位置和方向
	Vector3f camera_pos = camera->get_position();
	Vector3f camera_dir = camera->get_forward();

	// 计算光源方向
	Vector3f light_pos = light->transform.position;
	Vector3f target_pos = camera->get_target_position();
	Vector3f light_dir = (light_pos - target_pos).normalize();

	// 清空文本缓冲区
	text[0] = '\0';

	// 添加相机位置信息
	snprintf(text, 500, "camera pos: (%.1f, %.1f, %.1f)\n", camera_pos.x, camera_pos.y, camera_pos.z);

	// 添加相机方向信息（转换为角度）
	strcat(text, "camera dir: (");
	strcat(text, format_degrees(camera_dir.x));
	strcat(text, ", ");
	strcat(text, format_degrees(camera_dir.y));
	strcat(text, ", ");
	strcat(text, format_degrees(camera_dir.z));
	strcat(text, ")\n");

	// 添加相机控制提示
	strcat(text, "press mouse [Left] to rotate camera\n");
	strcat(text, "press mouse [Right] to move camera\n");
	strcat(text, "press key [Space] to reset camera\n\n");

	// 添加光源方向信息（转换为角度）
	strcat(text, "light dir: (");
	strcat(text, format_degrees(light_dir.x));
	strcat(text, ", ");
	strcat(text, format_degrees(light_dir.y));
	strcat(text, ", ");
	strcat(text, format_degrees(light_dir.z));
	strcat(text, ")\n");

	// 添加光源控制提示
	strcat(text, "press key [A] or [D] to rotate light dir\n\n");

	// 添加阴影状态信息
	strcat(text, "shadow: ");
	strcat(text, enable_shadow ? "On\n" : "Off\n");

	// 添加阴影控制提示
	strcat(text, "press key [E] to switch shadow\n\n");
	return 0;
}

void Scene::on_key_input(keycode_t key, int pressed) {
	if (pressed)
	{
		switch (key)
		{
		case KEY_A:
			light->transform.position = proj<3>(rotate(TO_RADIANS(-5), 0, 1, 1) * embed<4>(light->transform.position, 1.f));
			break;
		case KEY_D:
			light->transform.position = proj<3>(rotate(TO_RADIANS(5), 0, 1, 1) * embed<4>(light->transform.position, 1.f));
			break;
		case KEY_E:
			enable_shadow = !enable_shadow;
			break;
		default:
			break;
		}
	}
}

// 辅助函数：将弧度转换为角度并格式化为字符串
const char* Scene::format_degrees(float radians) {
	static char buffer[20];
	snprintf(buffer, 20, "%.1f", TO_DEGREES(radians));
	return buffer;
}

char* Scene::get_text() {
	return text;
}

Scene::~Scene() {
	delete camera;
	delete light;
}

SingleModelScene::SingleModelScene(const char* file_name, RenderBuffer* render_Buffer) :Scene(render_Buffer) {
	gameobject = new GameObject_StaticModel(file_name);

	material = new Material();
	material->diffuse_map = gameobject->model->get_diffuse_map();
	material->normal_map = gameobject->model->get_normal_map();
	material->specular_map = gameobject->model->get_specular_map();
	material->color = Color::White;
	material->specular = Color::White;
	material->gloss = 50;
	material->bump_scale = 1;

	shaderInfos[0].des = "6.NormalMap";
	shaderInfos[0].shader = new NormalMapShader();
	shaderInfos[1].des = "5.Blinn";
	shaderInfos[1].shader = new BlinnShader();
	shaderInfos[2].des = "4.Texture_Light(no shadow)";
	shaderInfos[2].shader = new TextureWithLightShader();
	shaderInfos[3].des = "3.Texture(no shadow)";
	shaderInfos[3].shader = new TextureShader();
	shaderInfos[4].des = "2.Toon(no shadow)";
	shaderInfos[4].shader = new ToonShader();
	shaderInfos[5].des = "1.Ground(no shadow)";
	shaderInfos[5].shader = new GroundShader();


	cur_shader_index = 0;

	draw_model = new DrawModel(light, gameobject, material, shaderInfos[cur_shader_index].shader);
}

SingleModelScene::~SingleModelScene() {
	delete draw_model;
	delete material;
	delete gameobject;
	for (int i = 0; i < 6; i++)
	{
		delete shaderInfos[i].shader;
	}
}

int SingleModelScene::tick(float delta_time) {
	Scene::tick(delta_time);

	char line[50] = "";
	snprintf(line, 50, "shader: %s\n", shaderInfos[cur_shader_index].des);
	strcat(text, line);
	snprintf(line, 50, "press key [Q] to switch shader\n\n");
	strcat(text, line);

	int num = draw_model->draw(camera, frameBuffer, enable_shadow);
	return num;
}

void SingleModelScene::on_key_input(keycode_t key, int pressed) {
	Scene::on_key_input(key, pressed);

	if (pressed)
	{
		switch (key)
		{
		case KEY_Q:
			delete draw_model;
			cur_shader_index = (cur_shader_index + 1 + 6) % 6;
			if (cur_shader_index >= 3) enable_shadow = false;
			else enable_shadow = true;
			draw_model = new DrawModel(light, gameobject, material, shaderInfos[cur_shader_index].shader);


			break;
		default:
			break;
		}
	}
}