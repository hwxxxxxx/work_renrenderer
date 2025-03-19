#include "IShader.h"

//������������������������������������������������ GroundShader ������������������������������������������������

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

//������������������������������������������������ GroundShader ������������������������������������������������



//������������������������������������������������ ToonShader ������������������������������������������������

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

//������������������������������������������������ ToonShader ������������������������������������������������



//������������������������������������������������ TextureShader ������������������������������������������������

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

//������������������������������������������������ TextureShader ������������������������������������������������



//������������������������������������������������ TextureWithLightShader ������������������������������������������������

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

//������������������������������������������������ TextureWithLightShader ������������������������������������������������



//������������������������������������������������ BlinnShader ������������������������������������������������

shader_struct_v2f BlinnShader::vertex(shader_struct_a2v* a2v) {
	// ��������ṹ�� v2f
	shader_struct_v2f v2f;
	// ������ռ�����ת��Ϊ�ü��ռ�����
	v2f.clip_pos = ObjectToClipPos(a2v->obj_pos);
	// ������ռ�����ת��Ϊ����ռ�����
	v2f.world_pos = ObjectToWorldPos(a2v->obj_pos);
	// ������ռ䷨��ת��Ϊ����ռ䷨��
	v2f.world_normal = ObjectToWorldNormal(a2v->obj_normal);
	// ֱ�Ӵ�����������
	v2f.uv = a2v->uv;
	return v2f;
}

bool BlinnShader::fragment(shader_struct_v2f* v2f, Color& color) {
	// ��һ������ռ䷨��
	Vector3f worldNormalDir = (v2f->world_normal).normalize();
	// �������������в�����ɫ�����������ɫ��ˣ��õ�������������ɫ
	Color albedo = tex_diffuse(v2f->uv) * shader_data->matrial->color;
	// ���㻷���⣺������ǿ�ȳ�����������ɫ
	Color ambient = shader_data->ambient * albedo;
	// ���㷨�����Դ����ĵ������������ [0, 1] ��Χ��
	float n_dot_l = saturate(worldNormalDir * WorldLightDir());
	// ����������⣺��Դ��ɫ������������ɫ���Ե��
	Color diffuse = shader_data->light_color * albedo * n_dot_l;
	// ����۲췽�򣬲���һ��
	Vector3f viewDir = WorldSpaceViewDir(v2f->world_pos).normalize();
	// �������������۲췽�����Դ�����ƽ��ֵ��������һ��
	Vector3f halfDir = (viewDir + WorldLightDir()).normalize();
	// ���㾵�淴��⣺��Դ��ɫ���Բ��ʾ��淴��ǿ�ȳ��Է������������ĵ�����ݴη�
	Color spcular = shader_data->light_color * shader_data->matrial->specular * std::pow(saturate(worldNormalDir * halfDir), shader_data->matrial->gloss);
	// ������ռ�����ת��Ϊ��Դ�ü��ռ�����
	Vector4f depth_pos = shader_data->light_vp_matrix * embed<4>(v2f->world_pos);
	// �жϵ�ǰƬ���Ƿ�����Ӱ��
	int shadow = is_in_shadow(depth_pos, n_dot_l);
	// ����������ɫ�������� + (������� + ���淴���) * ��Ӱ����
	color = ambient + (diffuse + spcular) * shadow;
	return false;
}

//������������������������������������������������ BlinnShader ������������������������������������������������



//������������������������������������������������ NormalMapShader ������������������������������������������������

shader_struct_v2f NormalMapShader::vertex(shader_struct_a2v* a2v) {
	shader_struct_v2f v2f;
	v2f.clip_pos = ObjectToClipPos(a2v->obj_pos);
	v2f.world_pos = ObjectToWorldPos(a2v->obj_pos);
	v2f.world_normal = ObjectToWorldNormal(a2v->obj_normal);
	v2f.uv = a2v->uv;

	return v2f;
}

bool NormalMapShader::fragment(shader_struct_v2f* v2f, Color& color) {
	// ��ȡ��ֵ�������ռ䷨��
	Vector3f normal = v2f->world_normal;

	// �ֽⷨ�ߵķ���
	float x = normal.x;
	float y = normal.y;
	float z = normal.z;

	// ������������ t
	Vector3f t = Vector3f(x * y / std::sqrt(x * x + z * z), std::sqrt(x * x + z * z), z * y / std::sqrt(x * x + z * z));

	// ���㸱�������� b��ͨ�����ߺ����ߵĲ����
	Vector3f b = cross(normal, t);

	// ���� TBN ����Tangent-Bitangent-Normal ����
	Matrix3x3 TBN;
	TBN[0] = Vector3f(t.x, b.x, normal.x); // ��һ�У���������
	TBN[1] = Vector3f(t.y, b.y, normal.y); // �ڶ��У�����������
	TBN[2] = Vector3f(t.z, b.z, normal.z); // �����У���������

	// �ӷ��������в�����������
	Vector3f bump = tex_normal(v2f->uv);

	// ���ݲ��ʵİ�͹�������ӵ������������� x �� y ����
	bump.x = bump.x * shader_data->matrial->bump_scale;
	bump.y = bump.y * shader_data->matrial->bump_scale;

	// ���㷨�������� z ������ȷ�����������ǵ�λ������
	bump.z = sqrt(1.0 - saturate(Vector2f(bump.x, bump.y) * Vector2f(bump.x, bump.y)));

	// ���������������߿ռ�ת��������ռ䣬����һ��
	normal = (TBN * bump).normalize();

	// ʹ��ת����ķ����������й��ռ���
	Vector3f worldNormalDir = normal;

	// �������������в�����ɫ�����������ɫ��ˣ��õ�������������ɫ
	Color albedo = tex_diffuse(v2f->uv) * shader_data->matrial->color;

	// ���㻷���⣺������ǿ�ȳ�����������ɫ
	Color ambient = shader_data->ambient * albedo;

	// ���㷨�����Դ����ĵ������������ [0, 1] ��Χ��
	float n_dot_l = saturate(worldNormalDir * WorldLightDir());

	// ����������⣺��Դ��ɫ������������ɫ���Ե��
	Color diffuse = shader_data->light_color * albedo * n_dot_l;

	// ����۲췽�򣬲���һ��
	Vector3f viewDir = WorldSpaceViewDir(v2f->world_pos).normalize();

	// �������������۲췽�����Դ�����ƽ��ֵ��������һ��
	Vector3f halfDir = (viewDir + WorldLightDir()).normalize();

	// ���㾵�淴��⣺��Դ��ɫ���Բ��ʾ��淴��ǿ�ȳ��Է������������ĵ�����ݴη�
	Color spcular = shader_data->light_color * shader_data->matrial->specular * std::pow(saturate(worldNormalDir * halfDir), shader_data->matrial->gloss);

	// ������ռ�����ת��Ϊ��Դ�ü��ռ�����
	Vector4f depth_pos = shader_data->light_vp_matrix * embed<4>(v2f->world_pos);

	// �жϵ�ǰƬ���Ƿ�����Ӱ��
	int shadow = is_in_shadow(depth_pos, n_dot_l);

	// ����������ɫ�������� + (������� + ���淴���) * ��Ӱ����
	color = ambient + (diffuse + spcular) * shadow;
	return false;
}

//������������������������������������������������ NormalMapShader ������������������������������������������������



//������������������������������������������������ ShadowShader ������������������������������������������������

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

//������������������������������������������������ ShadowShader ������������������������������������������������