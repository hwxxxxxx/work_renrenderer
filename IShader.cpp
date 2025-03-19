#include "IShader.h"

//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ GroundShader ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

shader_struct_v2f GroundShader::vertex(shader_struct_a2v* a2v) {
	shader_struct_v2f v2f;
	v2f.clip_pos = ObjectToClipPos(a2v->obj_pos);
	v2f.intensity = saturate(ObjectToWorldNormal(a2v->obj_normal) * WorldLightDir());
	return v2f;
}

bool GroundShader::fragment(shader_struct_v2f* v2f, Color& color) {
	color = Color::White * v2f->intensity;
	return false;
}

//↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ GroundShader ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑



//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ToonShader ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

shader_struct_v2f ToonShader::vertex(shader_struct_a2v* a2v) {
	shader_struct_v2f v2f;
	v2f.clip_pos = ObjectToClipPos(a2v->obj_pos);
	v2f.intensity = saturate(ObjectToWorldNormal(a2v->obj_normal) * WorldLightDir());
	return v2f;
}

bool ToonShader::fragment(shader_struct_v2f* v2f, Color& color) {
	float intensity = v2f->intensity;
	if (intensity > .85) intensity = 1;
	else if (intensity > .60) intensity = .80;
	else if (intensity > .45) intensity = .60;
	else if (intensity > .30) intensity = .45;
	else if (intensity > .15) intensity = .30;
	color = Color(1, 155 / 255.f, 0) * intensity;
	return false;
}

//↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ToonShader ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑



//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ TextureShader ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

shader_struct_v2f TextureShader::vertex(shader_struct_a2v* a2v) {
	shader_struct_v2f v2f;
	v2f.clip_pos = ObjectToClipPos(a2v->obj_pos);
	v2f.uv = a2v->uv;
	return v2f;
}

bool TextureShader::fragment(shader_struct_v2f* v2f, Color& color) {
	color = tex_diffuse(v2f->uv);
	return false;
}

//↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ TextureShader ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑



//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ TextureWithLightShader ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

shader_struct_v2f TextureWithLightShader::vertex(shader_struct_a2v* a2v) {
	shader_struct_v2f v2f;
	v2f.clip_pos = ObjectToClipPos(a2v->obj_pos);
	v2f.world_normal = ObjectToWorldNormal(a2v->obj_normal);
	v2f.uv = a2v->uv;
	return v2f;
}

bool TextureWithLightShader::fragment(shader_struct_v2f* v2f, Color& color) {
	float intensity = saturate(v2f->world_normal * WorldLightDir());
	color = tex_diffuse(v2f->uv) * intensity;
	return false;
}

//↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ TextureWithLightShader ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑



//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ BlinnShader ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

shader_struct_v2f BlinnShader::vertex(shader_struct_a2v* a2v) {
	// 声明输出结构体 v2f
	shader_struct_v2f v2f;
	// 将对象空间坐标转换为裁剪空间坐标
	v2f.clip_pos = ObjectToClipPos(a2v->obj_pos);
	// 将对象空间坐标转换为世界空间坐标
	v2f.world_pos = ObjectToWorldPos(a2v->obj_pos);
	// 将对象空间法线转换为世界空间法线
	v2f.world_normal = ObjectToWorldNormal(a2v->obj_normal);
	// 直接传递纹理坐标
	v2f.uv = a2v->uv;
	return v2f;
}

bool BlinnShader::fragment(shader_struct_v2f* v2f, Color& color) {
	// 归一化世界空间法线
	Vector3f worldNormalDir = (v2f->world_normal).normalize();
	// 从漫反射纹理中采样颜色，并与材质颜色相乘，得到最终漫反射颜色
	Color albedo = tex_diffuse(v2f->uv) * shader_data->matrial->color;
	// 计算环境光：环境光强度乘以漫反射颜色
	Color ambient = shader_data->ambient * albedo;
	// 计算法线与光源方向的点积，并限制在 [0, 1] 范围内
	float n_dot_l = saturate(worldNormalDir * WorldLightDir());
	// 计算漫反射光：光源颜色乘以漫反射颜色乘以点积
	Color diffuse = shader_data->light_color * albedo * n_dot_l;
	// 计算观察方向，并归一化
	Vector3f viewDir = WorldSpaceViewDir(v2f->world_pos).normalize();
	// 计算半角向量（观察方向与光源方向的平均值），并归一化
	Vector3f halfDir = (viewDir + WorldLightDir()).normalize();
	// 计算镜面反射光：光源颜色乘以材质镜面反射强度乘以法线与半角向量的点积的幂次方
	Color spcular = shader_data->light_color * shader_data->matrial->specular * std::pow(saturate(worldNormalDir * halfDir), shader_data->matrial->gloss);
	// 将世界空间坐标转换为光源裁剪空间坐标
	Vector4f depth_pos = shader_data->light_vp_matrix * embed<4>(v2f->world_pos);
	// 判断当前片段是否在阴影中
	int shadow = is_in_shadow(depth_pos, n_dot_l);
	// 计算最终颜色：环境光 + (漫反射光 + 镜面反射光) * 阴影因子
	color = ambient + (diffuse + spcular) * shadow;
	return false;
}

//↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ BlinnShader ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑



//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ NormalMapShader ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

shader_struct_v2f NormalMapShader::vertex(shader_struct_a2v* a2v) {
	shader_struct_v2f v2f;
	v2f.clip_pos = ObjectToClipPos(a2v->obj_pos);
	v2f.world_pos = ObjectToWorldPos(a2v->obj_pos);
	v2f.world_normal = ObjectToWorldNormal(a2v->obj_normal);
	v2f.uv = a2v->uv;

	return v2f;
}

bool NormalMapShader::fragment(shader_struct_v2f* v2f, Color& color) {
	// 获取插值后的世界空间法线
	Vector3f normal = v2f->world_normal;

	// 分解法线的分量
	float x = normal.x;
	float y = normal.y;
	float z = normal.z;

	// 计算切线向量 t
	Vector3f t = Vector3f(x * y / std::sqrt(x * x + z * z), std::sqrt(x * x + z * z), z * y / std::sqrt(x * x + z * z));

	// 计算副切线向量 b（通过法线和切线的叉积）
	Vector3f b = cross(normal, t);

	// 构建 TBN 矩阵（Tangent-Bitangent-Normal 矩阵）
	Matrix3x3 TBN;
	TBN[0] = Vector3f(t.x, b.x, normal.x); // 第一列：切线向量
	TBN[1] = Vector3f(t.y, b.y, normal.y); // 第二列：副切线向量
	TBN[2] = Vector3f(t.z, b.z, normal.z); // 第三列：法线向量

	// 从法线纹理中采样法线向量
	Vector3f bump = tex_normal(v2f->uv);

	// 根据材质的凹凸缩放因子调整法线向量的 x 和 y 分量
	bump.x = bump.x * shader_data->matrial->bump_scale;
	bump.y = bump.y * shader_data->matrial->bump_scale;

	// 计算法线向量的 z 分量（确保法线向量是单位向量）
	bump.z = sqrt(1.0 - saturate(Vector2f(bump.x, bump.y) * Vector2f(bump.x, bump.y)));

	// 将法线向量从切线空间转换到世界空间，并归一化
	normal = (TBN * bump).normalize();

	// 使用转换后的法线向量进行光照计算
	Vector3f worldNormalDir = normal;

	// 从漫反射纹理中采样颜色，并与材质颜色相乘，得到最终漫反射颜色
	Color albedo = tex_diffuse(v2f->uv) * shader_data->matrial->color;

	// 计算环境光：环境光强度乘以漫反射颜色
	Color ambient = shader_data->ambient * albedo;

	// 计算法线与光源方向的点积，并限制在 [0, 1] 范围内
	float n_dot_l = saturate(worldNormalDir * WorldLightDir());

	// 计算漫反射光：光源颜色乘以漫反射颜色乘以点积
	Color diffuse = shader_data->light_color * albedo * n_dot_l;

	// 计算观察方向，并归一化
	Vector3f viewDir = WorldSpaceViewDir(v2f->world_pos).normalize();

	// 计算半角向量（观察方向与光源方向的平均值），并归一化
	Vector3f halfDir = (viewDir + WorldLightDir()).normalize();

	// 计算镜面反射光：光源颜色乘以材质镜面反射强度乘以法线与半角向量的点积的幂次方
	Color spcular = shader_data->light_color * shader_data->matrial->specular * std::pow(saturate(worldNormalDir * halfDir), shader_data->matrial->gloss);

	// 将世界空间坐标转换为光源裁剪空间坐标
	Vector4f depth_pos = shader_data->light_vp_matrix * embed<4>(v2f->world_pos);

	// 判断当前片段是否在阴影中
	int shadow = is_in_shadow(depth_pos, n_dot_l);

	// 计算最终颜色：环境光 + (漫反射光 + 镜面反射光) * 阴影因子
	color = ambient + (diffuse + spcular) * shadow;
	return false;
}

//↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ NormalMapShader ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑



//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ShadowShader ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

shader_struct_v2f ShadowShader::vertex(shader_struct_a2v* a2v) {
	shader_struct_v2f v2f;
	v2f.clip_pos = ObjectToViewPos(a2v->obj_pos);
	return v2f;
}

bool ShadowShader::fragment(shader_struct_v2f* v2f, Color& color) {
	float factor = v2f->clip_pos[2];
	color = Color::White * factor;
	return false;
}

//↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ShadowShader ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑