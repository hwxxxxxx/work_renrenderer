#include "graphics.h"
#include "iostream"
#include "platform.h"
#include "scene.h"

static const char* const WINDOW_TITLE = "RenRenderer";
static const int WINDOW_WIDTH = 1000;
static const int WINDOW_HEIGHT = 600;
static const int WINDOW_TEXT_WIDTH = 270;
static const int WINDOW_TEXT_HEIGHT = 310;

struct SceneInfo {
	const char* name;
	Scene* scene;
};

int scene_count = 3;
SceneInfo scene_info;
RenderBuffer* frame_buffer = nullptr;
const float REFRESH_SCREEN_TEXT_TIME = 0.1;

SceneInfo load_scene(int scene_index) {
	if (scene_info.scene)
	{
		delete scene_info.scene;
	}

	SceneInfo ret;

	switch (scene_index)
	{
	case 0:
		ret.name = "1.african_head";
		ret.scene = new SingleModelScene("../../assets/african_head/african_head.obj", frame_buffer);
		break;
	case 1:
		ret.name = "2.diablo3_pose";
		ret.scene = new SingleModelScene("../../assets/diablo3_pose/diablo3_pose.obj", frame_buffer);
		break;
	case 2:
		ret.name = "3.floor";
		ret.scene = new SingleModelScene("../../assets/floor/floor.obj", frame_buffer);
		break;
	default:
		ret.name = "1.african_head";
		ret.scene = new SingleModelScene("../../assets/african_head/african_head.obj", frame_buffer);
		break;
	}

	return ret;
}


void key_callback(window_t* window, keycode_t key, int pressed) {
	if (scene_info.scene)
	{
		scene_info.scene->on_key_input(key, pressed);
	}
}

int main()
{
	platform_initialize();

	window_t* window;
	window = window_create(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TEXT_WIDTH, WINDOW_TEXT_HEIGHT);
	frame_buffer = new RenderBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);

	int scene_index = 0;
	scene_info = load_scene(scene_index);

	callbacks_t callbacks = callbacks_t();
	callbacks.button_callback = button_callback;
	callbacks.scroll_callback = scroll_callback;
	callbacks.key_callback = key_callback;
	input_set_callbacks(window, callbacks);

	Record record = Record();
	record.window_width = WINDOW_WIDTH;
	record.window_height = WINDOW_HEIGHT;
	window_set_userdata(window, &record);

	int num_frames = 0;
	float prev_time = platform_get_time();
	float print_time = prev_time;

	const int text_size = 500;
	char screen_text[text_size];
	int show_num_frames = 0;
	int show_avg_millis = 0;
	float refresh_screen_text_timer = 0;
	
	snprintf(screen_text, text_size, "fps: - -, avg: - -ms\n");

	while (!window_should_close(window)) {
		float curr_time = platform_get_time();
		float delta_time = curr_time - prev_time;

		// 更新摄像机控制
		update_camera(window, scene_info.scene->camera, &record);

		if (input_key_pressed(window, KEY_W)) {
			scene_index = (scene_index - 1 + scene_count) % scene_count;
			scene_info = load_scene(scene_index);
		}
		else if (input_key_pressed(window, KEY_S)) {
			scene_index = (scene_index + 1 + scene_count) % scene_count;
			scene_info = load_scene(scene_index);
		}

		// 更新场景
		int total_triangles = scene_info.scene->tick(delta_time);

		// 计算帧率和耗时
		num_frames += 1;
		if (curr_time - print_time >= 1) {
			int sum_millis = (int)((curr_time - print_time) * 1000);
			int avg_millis = sum_millis / num_frames;

			show_num_frames = num_frames;
			show_avg_millis = avg_millis;
			num_frames = 0;
			print_time = curr_time;
		}
		prev_time = curr_time;

		// 把帧缓存绘制到UI窗口
		window_draw_buffer(window, frame_buffer);

		//更新显示文本信息
		refresh_screen_text_timer += delta_time;
		if (refresh_screen_text_timer > REFRESH_SCREEN_TEXT_TIME)
		{
			// 清空屏幕文本缓冲区
			screen_text[0] = '\0';

			// 添加帧率和平均帧时间信息
			snprintf(screen_text, text_size, "fps: %3d, avg: %3d ms,triangle nums: %3d\n\n", show_num_frames, show_avg_millis, total_triangles);

			// 添加场景名称和切换场景提示
			strcat(screen_text, "scene: ");
			strcat(screen_text, scene_info.name);
			strcat(screen_text, "\npress key [W] or [S] to switch scene\n\n");
			// 添加场景自定义文本信息
			strcat(screen_text, scene_info.scene->get_text());

			// 在窗口中绘制文本
			window_draw_text(window, screen_text);

			// 重置计时器
			refresh_screen_text_timer -= REFRESH_SCREEN_TEXT_TIME;
		}

		// 重置摄像机输入参数
		record.orbit_delta = Vector2f(0, 0);
		record.pan_delta = Vector2f(0, 0);
		record.dolly_delta = 0;
		record.single_click = 0;
		record.double_click = 0;

		// 清除颜色缓存和深度缓存
		frame_buffer->renderbuffer_clear_color(Color::Black);
		frame_buffer->renderbuffer_clear_depth(std::numeric_limits<float>::max());

		input_poll_events();
	}

	delete scene_info.scene;
	delete frame_buffer;
	window_destroy(window);
}
