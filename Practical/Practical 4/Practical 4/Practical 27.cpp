#define _CRT_SECURE_NO_WARNINGS
#include "CG_OPGL.h"

#define WIDTH 1200
#define HEIGHT 900
#define PI 3.1415926535897932384620588419716939
#define MAX_LINE_LENGTH 1024
GLchar* vertexSource, * fragmentSource; // �ҽ��ڵ� ���� ����
GLuint vertexShader, fragmentShader; //���̴� ��ü
GLuint shaderProgramID; // ���̴� ���α׷�

//�׷��� ������Ʈ ����
Object board{}, light{};
vector<Object> tetra{}, planet{}, snow{};
FreeCamera camera{};

GLuint vao;
GLuint vbo[3];
GLuint EBO;
XYZ lightColor{ 1.0f,1.0f ,1.0f };
//�ǽ��� �ʿ��� ������
XYZ tps = { 0.0f, 0.15f,-0.1f };
//�ǽ��� �ʿ��� ������
int shape{};
int count{};
int lightdir{};
int light_color{};
bool mode_orbit{};
float temp_power{};
bool mode_snow{};
//-----------------------------------------------------------------------
void main(int argc, char** argv)
{
	srand(time(NULL));
	// ������ ����
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("OpenGL Practical");

	// GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	glewInit();
	// ��������
	//glEnable(GL_CULL_FACE);
	//glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	InitBuffer();
	CreateShaderProgram();
	//���� ���� �ʱ�ȭ
	init_figure();
	init_camera();
	//���� �Լ�
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);
	glutTimerFunc(50, Timer, 0);
	glutDisplayFunc(Render);
	glutIdleFunc(mainLoop); // ���� ����
	glutMainLoop(); //�̺�Ʈ ���� ����
}
//�׸��� �Լ�
GLvoid Render() {
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	tetra[shape].Draw_object();
	light.Draw_object_parent();
	board.Draw_object();

	for (int i = 0; i < planet.size(); i++)
	{
		planet[i].Draw_object_parent();
	}
	if (mode_snow)
	{
		for (int i = 0; i < snow.size(); i++)
		{
			snow[i].Draw_object();
		}
	}
	//checkFrameBuffer();
	glutSwapBuffers();
}

void checkFrameBuffer() {
	int width = WIDTH;
	int height = HEIGHT;
	std::vector<unsigned char> pixels(3 * width * height);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

	// ������ ���� �����͸� Ȯ��
	bool hasData = false;
	for (size_t i = 0; i < pixels.size(); ++i) {
		if (pixels[i] != 0) {
			hasData = true;
			break;
		}
	}

	if (hasData) {
		std::cout << "������ ���ۿ� �����Ͱ� �ֽ��ϴ�." << std::endl;
	}
	else {
		std::cout << "������ ���۰� ��� �ֽ��ϴ�." << std::endl;
	}
}
GLvoid InitBuffer()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, vbo);
	glGenBuffers(1, &EBO);
}

//������Ʈ�� �׸��� �Լ�
void Object::Draw_object() {
	Make_Matrix();
	UpdateVBO();
	for (int i = 0; i < facecnt; i++)
	{
		glDrawArrays(GL_TRIANGLES, i * 3, 3);
	}
}
void Object::Draw_object_parent() {
	Make_Matrix_parent();
	UpdateVBO();
	for (int i = 0; i < facecnt; i++)
	{
		glDrawArrays(GL_TRIANGLES, i * 3, 3);
	}
}
//������Ʈ�� ��ȯ ���
void Object::Make_Matrix() {
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "model");
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "view");
	unsigned int projectLocation = glGetUniformLocation(shaderProgramID, "project");
	// ��ȯ ���� ������ �����մϴ�.
	glm::mat4 modelMatrix = glm::mat4(1.0f);  // �� ��ȯ
	glm::mat4 viewMatrix = glm::mat4(1.0f);   // �� ��ȯ (ī�޶� ��ȯ)
	glm::mat4 projectionMatrix = glm::mat4(1.0f); // ���� ��ȯ (�������� ��ȯ)
	// ��ȯ ���� ������ �����մϴ�.

	modelMatrix = glm::translate(modelMatrix, glm::vec3(this->transform.x, this->transform.y, this->transform.z));

	// ���ڸ� ȸ��: ������ ��ġ���� ��ü�� ���ڸ����� ȸ����ŵ�ϴ�.
	modelMatrix = glm::rotate(modelMatrix, glm::radians(this->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));  // Y�� ȸ��
	modelMatrix = glm::rotate(modelMatrix, glm::radians(this->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));  // X�� ȸ��
	modelMatrix = glm::rotate(modelMatrix, glm::radians(this->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));  // Z�� ȸ��
	// ũ�� ��ȯ
	modelMatrix = glm::scale(modelMatrix, glm::vec3(this->scaling.x, this->scaling.y, this->scaling.z));

	viewMatrix = glm::lookAt(
		glm::vec3(camera.x, camera.y, camera.z), // ī�޶� ��ġ
		glm::vec3(camera.at_x, camera.at_y, camera.at_z), // ī�޶� �ٶ󺸴� ����
		glm::vec3(0.0f, 1.0f, 0.0f)  // ���� �� ����
	);
	// ���� ����
	projectionMatrix = glm::perspective(
		glm::radians(90.0f), // �þ߰�
		(float)WIDTH / (float)HEIGHT, // ��Ⱦ��
		0.1f, 100.0f // Ŭ�� ��� (near, far)
	);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(projectLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
}
//������Ʈ�� ��ȯ ��ģ���ӣ�
void Object::Make_Matrix_parent() {
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "model");
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "view");
	unsigned int projectLocation = glGetUniformLocation(shaderProgramID, "project");
	// ��ȯ ���� ������ �����մϴ�.
	glm::mat4 modelMatrix = glm::mat4(1.0f);  // �� ��ȯ
	glm::mat4 viewMatrix = glm::mat4(1.0f);   // �� ��ȯ (ī�޶� ��ȯ)
	glm::mat4 projectionMatrix = glm::mat4(1.0f); // ���� ��ȯ (�������� ��ȯ)
	// ��ȯ ���� ������ �����մϴ�.

	modelMatrix = glm::translate(modelMatrix, glm::vec3(this->parent_transform->x, this->parent_transform->y, this->parent_transform->z));

	//��Ӱ���
	modelMatrix = glm::rotate(modelMatrix, glm::radians(this->orbit_theta), glm::vec3(0.0f, 1.0f, 0.0f));  // Y�� ȸ��

	modelMatrix = glm::translate(modelMatrix, glm::vec3(this->transform.x, this->transform.y, this->transform.z));

	// ���ڸ� ȸ��: ������ ��ġ���� ��ü�� ���ڸ����� ȸ����ŵ�ϴ�.
	modelMatrix = glm::rotate(modelMatrix, glm::radians(this->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));  // Y�� ȸ��
	modelMatrix = glm::rotate(modelMatrix, glm::radians(this->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));  // X�� ȸ��
	modelMatrix = glm::rotate(modelMatrix, glm::radians(this->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));  // Z�� ȸ��
	// ũ�� ��ȯ
	modelMatrix = glm::scale(modelMatrix, glm::vec3(this->scaling.x, this->scaling.y, this->scaling.z));



	viewMatrix = glm::lookAt(
		glm::vec3(camera.x, camera.y, camera.z), // ī�޶� ��ġ
		glm::vec3(camera.at_x, camera.at_y, camera.at_z), // ī�޶� �ٶ󺸴� ����
		glm::vec3(0.0f, 1.0f, 0.0f)  // ���� �� ����
	);
	// ���� ����
	projectionMatrix = glm::perspective(
		glm::radians(90.0f), // �þ߰�
		(float)WIDTH / (float)HEIGHT, // ��Ⱦ��
		0.1f, 100.0f // Ŭ�� ��� (near, far)
	);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(projectLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
}
// BoundingBox ������Ʈ �Լ�
void Object::UpdateBoundingBox() {
	if (this->vertex.empty()) return;

	// �ʱⰪ ����: ù ��° ������ ��ǥ�� �������� �ʱ�ȭ
	float minX = this->vertex[0].x, minY = this->vertex[0].y, minZ = this->vertex[0].z;
	float maxX = this->vertex[0].x, maxY = this->vertex[0].y, maxZ = this->vertex[0].z;

	// �� ������ ��ȸ�ϸ� �ּ�/�ִ밪�� ã��
	for (const auto& v : this->vertex) {
		minX = std::min(minX, v.x);
		minY = std::min(minY, v.y);
		minZ = std::min(minZ, v.z);
		maxX = std::max(maxX, v.x);
		maxY = std::max(maxY, v.y);
		maxZ = std::max(maxZ, v.z);
	}

	// BoundingBox ����, transform ���� �߰��Ͽ� ���� ��ǥ�� ����
	this->hitbox.left_front_top = { minX + this->transform.x, maxY + this->transform.y, maxZ + this->transform.z };
	this->hitbox.left_front_bottom = { minX + this->transform.x, minY + this->transform.y, maxZ + this->transform.z };
	this->hitbox.right_front_top = { maxX + this->transform.x, maxY + this->transform.y, maxZ + this->transform.z };
	this->hitbox.right_front_bottom = { maxX + this->transform.x, minY + this->transform.y, maxZ + this->transform.z };

	this->hitbox.left_back_top = { minX + this->transform.x, maxY + this->transform.y, minZ + this->transform.z };
	this->hitbox.left_back_bottom = { minX + this->transform.x, minY + this->transform.y, minZ + this->transform.z };
	this->hitbox.right_back_top = { maxX + this->transform.x, maxY + this->transform.y, minZ + this->transform.z };
	this->hitbox.right_back_bottom = { maxX + this->transform.x, minY + this->transform.y, minZ + this->transform.z };
}
//������Ʈ�� ���� ����
void Object::UpdateVBO() {
	// VAO ���ε�
	glBindVertexArray(vao);

	// ���� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, this->vertex.size() * sizeof(glm::vec3), this->vertex.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	// ��� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, this->normal.size() * sizeof(glm::vec3), this->normal.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(1);

	// �ؽ��� ����
	//glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	//glBufferData(GL_ARRAY_BUFFER, this->texture.size() * sizeof(glm::vec2), this->texture.data(), GL_STREAM_DRAW);
	//glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
	//glEnableVertexAttribArray(3);

	//// �ε��� ����
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indexlist.size() * sizeof(Index), this->indexlist.data(), GL_STATIC_DRAW);


	glUseProgram(shaderProgramID);
	unsigned int lightPosLocation = glGetUniformLocation(shaderProgramID, "lightPos");
	glUniform3f(lightPosLocation, light.transform.x, light.transform.y, light.transform.z);

	unsigned int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor");
	glUniform3f(lightColorLocation, light.power * light.color[0].g, light.power * light.color[0].g, light.power * light.color[0].b);

	unsigned int objColorLocation = glGetUniformLocation(shaderProgramID, "objectColor");
	glUniform3f(objColorLocation, this->color[0].r, this->color[0].g, this->color[0].b);

	unsigned int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos");
	glUniform3f(viewPosLocation, camera.x, camera.y, camera.z);
}

void Timer(int value) {
	for (int i = 0; i < planet.size(); i++)
	{
		planet[i].orbit_theta += (3 - i) * 0.7f;
	}
	if (mode_snow)
	{
		for (int i = 0; i < snow.size(); i++)
		{
			if (snow[i].transform.y > 0.0f)
			{
				snow[i].transform.y -= snow[i].power;
			}
			else
			{
				snow[i].transform.y = 4.0f;
			}

		}
	}


	if (mode_orbit)
	{
		float vx = light.transform.x;
		float vz = light.transform.z;
		light.transform.x = vx * cos(glm::radians(-2.0f)) - vz * sin(glm::radians(-2.0f));
		light.transform.z = vx * sin(glm::radians(-2.0f)) + vz * cos(glm::radians(-2.0f));
	}
	glutTimerFunc(50, Timer, 0);
	glutPostRedisplay(); // �ٽ� �׸��� ��û
}
void Mouse(int button, int state, int x, int y) {

}
void Motion(int x, int y) {

}
void SpecialKeyboard(int key, int x, int y)
{
	switch (key)
	{

	default:
		break;
	}
}
GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'y':
		mode_orbit = !mode_orbit;
		break;
	case 'n':
		mode_snow = !mode_snow;
		break;
	case 'm':
		temp_power = light.power;
		light.power = 0.0f;
		break;
	case 'M':
		light.power = temp_power;
		break;
	case '+':
		if (light.power < 1.0f)
		{
			light.power += 0.1f;
		}
		break;
	case '-':
		if (light.power > 0.0f)
		{
			light.power -= 0.1f;
		}
		break;
	case 'c':
		light_color = (light_color + 1) % 3;
		switch (light_color)
		{
		case 0:
			light.color[0] = {1.0f,1.0f ,1.0f};
			break;
		case 1:
			light.color[0] = { 0.5f,1.0f ,0.5f };
			break;
		case 2:
			light.color[0] = { 1.0f,0.5f ,0.5f };
			break;
		default:
			break;
		}
		break;
	case 'z':
		glm::vec3 direction = glm::normalize(glm::vec3{ light.parent_transform->x - light.transform.x,
											0.0f,light.parent_transform->z - light.transform.z });
		light.transform.x += direction.x / 10;
		light.transform.z += direction.z / 10;
		break;
	case 'Z':
		glm::vec3 direction2 = glm::normalize(glm::vec3{ light.parent_transform->x - light.transform.x,
											0.0f,light.parent_transform->z - light.transform.z });
		light.transform.x -= direction2.x / 10;
		light.transform.z -= direction2.z / 10;
		break;
	case 'p':
		lightdir = (lightdir + 1) % 4;
		light.transform.x = glm::cos(glm::radians(lightdir * 90.f));
		light.transform.z = glm::sin(glm::radians(lightdir * 90.f));
 		break;
	case 'w':
		camera.z -= 0.1f;
		break;
	case 's':
		camera.z += 0.1f;
		break;
	case 'a':
		camera.x -= 0.1f;
		break;
	case 'd':
		camera.x += 0.1f;
		break;
	case 'x':
		camera.y -= 0.1f;
		break;
	case 32:
		camera.y += 0.1f;
		break;
	case '1':
		shape = 0;
		break;
	case '2':
		shape = 1;
		break;
	case '3':
		shape = 2;
		break;
	case '4':
		shape = 3;
		break;
	case '5':
		shape = 4;
		break;
	case 'q':
		exit(0);
		break;
	}
}

void mainLoop() {


	glutPostRedisplay(); // �ٽ� �׸��� ��û
}
bool is_crash(Object& objA, Object& objB) {
	// AABB ������Ʈ
	objA.UpdateBoundingBox();
	objB.UpdateBoundingBox();

	// �浹 �˻�
	// X�� ��ħ �˻�
	bool xOverlap = objA.hitbox.right_back_bottom.x >= objB.hitbox.left_back_bottom.x &&
		objA.hitbox.left_back_bottom.x <= objB.hitbox.right_back_bottom.x;

	// Y�� ��ħ �˻�
	bool yOverlap = objA.hitbox.left_front_top.y >= objB.hitbox.left_front_bottom.y &&
		objA.hitbox.left_front_bottom.y <= objB.hitbox.left_front_top.y;

	// Z�� ��ħ �˻�
	bool zOverlap = objA.hitbox.left_front_top.z >= objB.hitbox.left_back_top.z &&
		objA.hitbox.left_back_top.z <= objB.hitbox.left_front_top.z;

	// ��� �࿡�� ��ġ�� �浹 �߻�
	return xOverlap && yOverlap && zOverlap;
}

void init_figure() {
	//---------------------------board, light�����-----------------------------
	tetra.clear();
	read_obj_file("cube.obj", board);
	AddColors(board, 0.5f, 0.5f, 1.0f);
	read_obj_file("cube.obj", light);
	AddColors(light, 1.0f, 1.0f, 1.0f);
	//---------------------------tetra-----------------------------
	Object objfile;
	read_obj_file("tetra.obj", objfile);
	AddColors(objfile, 1.0f, 0.5f, 0.5f);
	tetra.push_back(objfile);
	for (int i = 0; i < 4; i++)
	{
		Object temp{};
		temp.color = tetra[0].color;
		for (int j = 0; j < tetra[i].facecnt; j++)
		{
			//�鸶�� 3���� ������ �����ϴ� ����
			for (int k = 0; k < 3; k++)
			{
				glm::vec3 temp_vertex{};
				temp.vertex.push_back(tetra[i].vertex[(3 * j) + (k)]);
				temp_vertex.x = (tetra[i].vertex[(3 * j) + (k)].x + tetra[i].vertex[(3 * j) + (k + 1) % 3].x) / 2;
				temp_vertex.y = (tetra[i].vertex[(3 * j) + (k)].y + tetra[i].vertex[(3 * j) + (k + 1) % 3].y) / 2;
				temp_vertex.z = (tetra[i].vertex[(3 * j) + (k)].z + tetra[i].vertex[(3 * j) + (k + 1) % 3].z) / 2;
				temp.vertex.push_back(temp_vertex);
				temp_vertex.x = (tetra[i].vertex[(3 * j) + (k)].x + tetra[i].vertex[(3 * j) + (k + 2) % 3].x) / 2;
				temp_vertex.y = (tetra[i].vertex[(3 * j) + (k)].y + tetra[i].vertex[(3 * j) + (k + 2) % 3].y) / 2;
				temp_vertex.z = (tetra[i].vertex[(3 * j) + (k)].z + tetra[i].vertex[(3 * j) + (k + 2) % 3].z) / 2;
				temp.vertex.push_back(temp_vertex);
				temp.normal.push_back(tetra[i].normal[(3 * j) + (k + 0) % 3]);
				temp.normal.push_back(tetra[i].normal[(3 * j) + (k + 1) % 3]);
				temp.normal.push_back(tetra[i].normal[(3 * j) + (k + 2) % 3]);
				temp.facecnt++;
			}
		}
		tetra.push_back(temp);
	}
	for (int i = 0; i < tetra.size(); i++)
	{
		std::cout << i+1 << "��° ���� Vertex ���� : " << tetra[i].vertex.size() << "Normal ���� : " << tetra[i].normal.size() << "face ���� : " << tetra[i].facecnt << std::endl;
	}
	//---------------------------planet-----------------------------
	Object sphere;
	read_obj_file("sphere.obj", sphere);
	sphere.transform = {1.0f,2.0f,0.0f };
	sphere.scaling = { 5.0f,5.0f,5.0f };
	planet.push_back(sphere);
	sphere.transform = { 3.0f,1.0f,0.0f };
	sphere.scaling = { 8.0f,8.0f,8.0f };
	planet.push_back(sphere);
	sphere.transform = { -2.0f,1.5f,0.0f };
	sphere.scaling = { 10.0f,10.0f,10.0f };
	planet.push_back(sphere);
	for (int i = 0; i < planet.size(); i++)
	{
		planet[i].parent_transform = &tetra[0].transform;
	}
	//---------------------------snow-----------------------------
	sphere.scaling = { 5.0f,5.0f ,5.0f };
	for (int i = 0; i < 100; i++)
	{
		sphere.transform.x = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 6.0f - 3.0f;
		sphere.transform.y = 3.0f;
		sphere.transform.z = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 6.0f - 3.0f;
		sphere.power = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) / 2.0f + 0.05;
		snow.push_back(sphere);
	}





	//-----------------------------����������----------------------------
	AddColors(planet[0], 1.0f, 1.0f, 0.5f);
	AddColors(planet[1], 0.5f, 1.0f, 1.0f);
	AddColors(planet[2], 1.0f, 0.5f, 1.0f);
	//-------------------------��ȯ���� �Է�-------------------------
	board.scaling = { 10.0f,0.1f,10.0f };
	board.transform.y = -0.05f;
	light.transform = { 0.0f,1.0f,1.0f };
	light.scaling = { 0.2f,0.2f,0.2f };
	light.power = 1.0f;
	light.parent_transform = &tetra[0].transform;
}
void init_camera() {
	//3��Ī ī�޶�
	camera.x = 0.0f, camera.y = 1.0f, camera.z = 3.0f;
	camera.at_x = 0.0f, camera.at_y = 0.0f, camera.at_z = 0.0f;
	camera.forward = glm::vec3{ 0.0f,0.0f,1.0f };
	camera.up = glm::vec3{ 0.0f,1.0f,0.0f };
	camera.right = glm::vec3{ 1.0f,0.0f,0.0f };
}

void Update_camera(FreeCamera& camera, float angle_xz, float angle_y) {
	float vx = camera.forward.x;
	float vz = camera.forward.z;
	camera.forward.x = vx * cos(glm::radians(angle_xz)) - vz * sin(glm::radians(angle_xz));
	camera.forward.z = vx * sin(glm::radians(angle_xz)) + vz * cos(glm::radians(angle_xz));
	camera.right.x = camera.forward.x * cos(glm::radians(90.0)) - camera.forward.z * sin(glm::radians(90.0));
	camera.right.z = camera.forward.x * sin(glm::radians(90.0)) + camera.forward.z * cos(glm::radians(90.0));
}
void Update_camera(FixedCamera& camera, float angle_xz, float angle_y) {
	float vx = camera.forward.x;
	float vz = camera.forward.z;
	camera.forward.x = vx * cos(glm::radians(angle_xz)) - vz * sin(glm::radians(angle_xz));
	camera.forward.z = vx * sin(glm::radians(angle_xz)) + vz * cos(glm::radians(angle_xz));
	camera.right.x = camera.forward.x * cos(glm::radians(90.0)) - camera.forward.z * sin(glm::radians(90.0));
	camera.right.z = camera.forward.x * sin(glm::radians(90.0)) + camera.forward.z * cos(glm::radians(90.0));
}
void AddColors(Object& fig, float r, float g, float b) {
	fig.color.clear();
	// ���� ������
	for (int i = 0; i < fig.vertex.size(); i++) {
		fig.color.emplace_back(glm::vec3{ r,g,b });
	}
}
GLvoid Reshape(int w, int h)
{
	// ����Ʈ �⺻ WIDTH HEIGHT�� ����
	glViewport(0, 0, w, h);
}
char* GetBuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;

	// ���� �б� �������� ����
	fopen_s(&fptr, file, "rb");

	// ����ó��
	if (!fptr) return NULL;

	// ���� buf�� ��ȯ
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);

	// ���� ����
	fclose(fptr);

	buf[length] = 0;
	return buf;
}
GLint CreateShader(const char* file, int type)
{
	GLchar* source = GetBuf(file);

	// ��ü ����
	GLint shader = glCreateShader(type);
	glShaderSource(shader, 1, (const GLchar**)&source, 0);
	glCompileShader(shader);

	// ������ ���� üũ
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

	if (!result)
	{
		glGetShaderInfoLog(shader, 512, NULL, errorLog);
		std::cerr << "ERROR: ������ ����\n" << errorLog << std::endl;
		return 0;
	}

	return shader;
}
void CreateShaderProgram()
{
	vertexShader = CreateShader("vertex.vs", GL_VERTEX_SHADER);
	fragmentShader = CreateShader("fragment.fs", GL_FRAGMENT_SHADER);

	// ���̴� ���α׷� ����
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);

	// ���̴� ����
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// ���̴� ���α׷� ���
	glUseProgram(shaderProgramID);
}
// OBJ ���� �б� �Լ�
void read_obj_file(const char* filename, Object& model) {
	FILE* file;
	fopen_s(&file, filename, "r");
	if (!file) {
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}

	char line[MAX_LINE_LENGTH];
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec3> temp_vertices2;
	std::vector<glm::vec3> temp_normals;
	std::vector<glm::vec3> temp_normals2;
	std::vector<glm::vec2> temp_textures;
	std::vector<Index> temp_indices;
	model.vertex.clear();
	model.normal.clear();
	model.color.clear();
	model.texture.clear();
	model.indexlist.clear();
	while (fgets(line, sizeof(line), file)) {
		read_newline(line); // ���๮�� ����

		// ����(Vertex) ������ ó��
		if (line[0] == 'v' && line[1] == ' ') {
			glm::vec3 vertex;
			sscanf_s(line + 2, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		// ����(Vertex) ������ ó��
		if (line[0] == 'v' && line[1] == 'n') {
			glm::vec3 normal;
			sscanf_s(line + 2, "%f %f %f", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		// ����(Vertex) ������ ó��
		if (line[0] == 'v' && line[1] == 't') {
			glm::vec2 texture;
			sscanf_s(line + 2, "%f %f", &texture.x, &texture.y);
			temp_textures.push_back(texture);
		}
		// ��(Face) ������ ó��
		else if (line[0] == 'f' && line[1] == ' ') {
			unsigned int v1, v2, v3;
			unsigned int vt1, vt2, vt3; // �ؽ�ó ��ǥ �ε���
			unsigned int vn1, vn2, vn3; // ���� ���� �ε���
			sscanf_s(line + 2, "%u/%u/%u %u/%u/%u %u/%u/%u",
				&v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);

			// OBJ �ε����� 1���� �����ϱ� ������ 0���� �����ϵ��� ����
			//Index face = { v1 - 1, v2 - 1,v3 - 1 };
			//temp_indices.push_back(face);
			temp_vertices2.push_back(glm::vec3{ temp_vertices[v1 - 1] });
			temp_vertices2.push_back(glm::vec3{ temp_vertices[v2 - 1] });
			temp_vertices2.push_back(glm::vec3{ temp_vertices[v3 - 1] });
			temp_normals2.push_back(glm::vec3{ temp_normals[vn1 - 1] });
			temp_normals2.push_back(glm::vec3{ temp_normals[vn2 - 1] });
			temp_normals2.push_back(glm::vec3{ temp_normals[vn3 - 1] });
			model.facecnt++;
		}
	}

	fclose(file);

	// Object ����ü�� ���� �� �ε��� �����͸� ����
	model.vertex = temp_vertices2; // ���� �����͸� �״�� ����
	model.normal = temp_normals2; // �ε��� �����͸� �״�� ����
	model.texture = temp_textures; // �ε��� �����͸� �״�� ����
	model.indexlist = temp_indices; // �ε��� �����͸� �״�� ����

	// ������ ���Ƿ� ���� (���� ���� ����)
	AddColors(model, 0.5f, 0.5f, 0.5f);

	model.transform = { 0.0f,0.0f,0.0f };
	model.scaling = { 1.0f,1.0f,1.0f };
	model.rotation = { 0.0f,0.0f,0.0f };
}