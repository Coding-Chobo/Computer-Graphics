#define _CRT_SECURE_NO_WARNINGS
#include "3DGraphics.h"

#define WIDTH 1200
#define HEIGHT 900
#define PI 3.1415926535897932384620588419716939
#define MAX_LINE_LENGTH 1024
GLchar* vertexSource, * fragmentSource; // �ҽ��ڵ� ���� ����
GLuint vertexShader, fragmentShader; //���̴� ��ü
GLuint shaderProgramID; // ���̴� ���α׷�

Object cube{}, tetra{};
Object objfile{};
Coordinate Coordinate_system{};
unsigned int shape{ 0 };
GLfloat rotate_angle_x;
GLfloat rotate_angle_y;
bool mode_w{ false };
unsigned int mode_animate_x{ 0 };
unsigned int mode_animate_y{ 0 };
void mainLoop() {
	glutPostRedisplay(); // �ٽ� �׸��� ��û
}
void main(int argc, char** argv)
{
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
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	
	read_obj_file("Daven.obj", objfile);

	InitBuffer();
	CreateShaderProgram();
	//���� ���� �ʱ�ȭ
	init_figure();
	//���� �Լ�
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);
	glutDisplayFunc(Render);
	glutIdleFunc(mainLoop); // ���� ����
	glutMainLoop(); //�̺�Ʈ ���� ����
}
//�׸��� �Լ�
GLvoid Render()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	init_Matrix();
	// ��ǥ�� �׸��� (ȸ���̳� ��ȯ ������� ����)
	Draw_Coordinate(Coordinate_system);

	// ��ȯ��� ���� �� ����
	Make_Matrix();
	std::cout << objfile.indexlist.size() << std::endl;
	UpdateVBO(objfile);
	glDrawElements(GL_TRIANGLES, 3 * objfile.indexlist.size(), GL_UNSIGNED_INT, 0);
	// ���� �׸���
	if (shape == 1)
	{
		UpdateVBO(cube);
		if (!mode_w)
		{
			glDrawElements(GL_TRIANGLES, 3 * cube.indexlist.size(), GL_UNSIGNED_INT, 0);
		}
		else
		{
			glDrawElements(GL_LINE_LOOP, 3 * cube.indexlist.size(), GL_UNSIGNED_INT, 0);
		}
	}
	else if (shape == 2)
	{
		UpdateVBO(tetra);
		if (mode_w)
		{
			glDrawElements(GL_LINE_LOOP, 3 * tetra.indexlist.size(), GL_UNSIGNED_INT, 0);
		}
		else
		{
			glDrawElements(GL_TRIANGLES, 3 * tetra.indexlist.size(), GL_UNSIGNED_INT, 0);
		}
	}

	glutSwapBuffers();
}


GLvoid InitBuffer()
{
	glGenVertexArrays(1, &cube.vao);
	glGenBuffers(2, cube.vbo);
	glGenBuffers(1, &cube.EBO);

	glGenVertexArrays(1, &objfile.vao);
	glGenBuffers(2, objfile.vbo);
	glGenBuffers(1, &objfile.EBO);

	glGenVertexArrays(1, &tetra.vao);
	glGenBuffers(2, tetra.vbo);
	glGenBuffers(1, &tetra.EBO);
	glGenVertexArrays(1, &Coordinate_system.vao);
	glGenBuffers(2, Coordinate_system.vbo);
	glGenBuffers(1, &Coordinate_system.EBO);
}
GLvoid init_Matrix() {
	// ��ǥ�� �׸��� ���� ��ȯ �ʱ�ȭ
	glm::mat4 identityMatrix = glm::mat4(1.0f);
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "trans");
	identityMatrix = glm::rotate(identityMatrix, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	identityMatrix = glm::rotate(identityMatrix, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(identityMatrix));
}
GLvoid Make_Matrix() {
	float rotate_gap = PI / 90;
	switch (mode_animate_x)
	{
	case 1://x�� ���� ����
		rotate_angle_x += rotate_gap;
		break;
	case 2://x�� ���� ����
		rotate_angle_x -= rotate_gap;
		break;
	default:
		break;
	}
	switch (mode_animate_y)
	{
	case 3://y�� ���� ����
		rotate_angle_y += rotate_gap;
		break;
	case 4://y�� ���� ����
		rotate_angle_y -= rotate_gap;
		break;
	default:
		break;
	}
	// �� location �ε��� ����
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "trans");

	// �� �� ��ȯ ���� �ʱ�ȭ
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 scale = glm::mat4(1.0f);
	glm::mat4 rot = glm::mat4(1.0f);
	glm::mat4 move = glm::mat4(1.0f);

	// ��ȯ
	scale = glm::scale(scale, glm::vec3(0.5f, 0.6f, 0.5f));
	move = glm::translate(move, glm::vec3(0.0f, 0.0f, 0.0f));
	rot = glm::rotate(rot, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	rot = glm::rotate(rot, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	rot = glm::rotate(rot, glm::radians(rotate_angle_x), glm::vec3(1.0f, 0.0f, 0.0f));
	rot = glm::rotate(rot, glm::radians(rotate_angle_y), glm::vec3(0.0f, 1.0f, 0.0f));


	// ��ȯ ���� ó��
	model = model * scale * move * rot;

	// �� ��ȯ ����
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
}

void Draw_Coordinate(Coordinate obj) {
	obj.vertex.clear();
	obj.color.clear();
	obj.indexlist.clear();

	// ���� �߰�
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });  // ����
	obj.vertex.emplace_back(glm::vec3{ 5.0f, 0.0f, 0.0f });  // X��
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });  // ����
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 5.0f, 0.0f });  // Y��
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });  // ����
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 0.0f, 5.0f });  // Z��

	// ���� �߰� (�� ���� ���� ����)
	obj.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 0.0f });  // X��: ����
	obj.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 0.0f });  // X��: ����
	obj.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 1.0f });  // Y��: �Ķ�
	obj.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 1.0f });  // Y��: �Ķ�
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 0.0f });  // Z��: �ʷ�
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 0.0f });  // Z��: �ʷ�


	// �ε��� �߰�
	obj.indexlist.emplace_back(0);  // X��
	obj.indexlist.emplace_back(1);

	obj.indexlist.emplace_back(2);  // Y��
	obj.indexlist.emplace_back(3);

	obj.indexlist.emplace_back(4);  // Z��
	obj.indexlist.emplace_back(5);

	// VBO ������Ʈ
	UpdateVBO(obj);

	// �ε����� ������ ��ǥ���� ������ 3���̹Ƿ� �� 6��
	glDrawElements(GL_LINES, obj.indexlist.size(), GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}
void Timer(int value) {
	
}

void Mouse(int button, int state, int x, int y) {

}
void Motion(int x, int y) {

}
void SpecialKeyboard(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		if (shape == 1)
		{
			for (int i = 0; i < cube.vertex.size(); i++)
			{
				cube.vertex.at(i).x -= 0.05f;
			}
		}
		else
		{
			for (int i = 0; i < tetra.vertex.size(); i++)
			{
				tetra.vertex.at(i).x -= 0.05f;
			}
		}
		break;
	case GLUT_KEY_RIGHT:
		if (shape == 1)
		{
			for (int i = 0; i < cube.vertex.size(); i++)
			{
				cube.vertex.at(i).x += 0.05f;
			}
		}
		else
		{
			for (int i = 0; i < tetra.vertex.size(); i++)
			{
				tetra.vertex.at(i).x += 0.05f;
			}
		}
		break;
	case GLUT_KEY_UP:
		if (shape == 1)
		{
			for (int i = 0; i < cube.vertex.size(); i++)
			{
				cube.vertex.at(i).y += 0.05f;
			}
		}
		else
		{
			for (int i = 0; i < tetra.vertex.size(); i++)
			{
				tetra.vertex.at(i).y += 0.05f;
			}
		}
		break;
	case GLUT_KEY_DOWN:
		if (shape == 1)
		{
			for (int i = 0; i < cube.vertex.size(); i++)
			{
				cube.vertex.at(i).y -= 0.05f;
			}
		}
		else
		{
			for (int i = 0; i < tetra.vertex.size(); i++)
			{
				tetra.vertex.at(i).y -= 0.05f;
			}
		}
		break;
	default:
		break;
	}
}
GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'c':
		shape = 1;
		break;
	case 't':
		shape = 2;
		break;
	case 'h':
		glEnable(GL_CULL_FACE);
		break;
	case 'H':
		glDisable(GL_CULL_FACE);
		break;
	case 'w':
		mode_w = true;
		break;
	case 'W':
		mode_w = false;
	case 'x':
		mode_animate_x = 1;
		break;
	case 'X':
		mode_animate_x = 2;
		break;
	case 'y':
		mode_animate_y = 3;
		break;
	case 'Y':
		mode_animate_y = 4;
		break;
	case 's':
		init_figure();
		mode_w = { false };
		rotate_angle_x = 0.0f;
		rotate_angle_y = 0.0f;
		mode_animate_x = { 0 };
		mode_animate_y = { 0 };
		break;
	default:
		break;
	}
}

GLvoid UpdateVBO(Object object) {
	// VAO ���ε�
	glBindVertexArray(object.vao);

	// ť�� ��ġ ���� ���ε�
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, object.vertex.size() * sizeof(glm::vec3), object.vertex.data(), GL_DYNAMIC_DRAW);

	// ť�� �ε��� ���� ���ε�
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.EBO);
	// ��ü �ε��� �迭�� ũ�� ����
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, object.indexlist.size() * sizeof(Index), object.indexlist.data(), GL_DYNAMIC_DRAW);

	// ��ġ �Ӽ� ����
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(0);

	// ť�� ���� ���� ���ε�
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, object.color.size() * sizeof(glm::vec3), object.color.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(1);
}

GLvoid UpdateVBO(Coordinate object) {
	// VAO ���ε�
	glBindVertexArray(object.vao);

	// ��ǥ�� ���� ���� ���ε�
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, object.vertex.size() * sizeof(glm::vec3), object.vertex.data(), GL_DYNAMIC_DRAW);

	// ��ǥ�� �ε��� ���� ���ε�
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, object.indexlist.size() * sizeof(unsigned int), object.indexlist.data(), GL_DYNAMIC_DRAW);

	// ��ġ �Ӽ� ����
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(0);

	// ��ǥ�� ���� ���� ���ε�
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, object.color.size() * sizeof(glm::vec3), object.color.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(1);
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
void read_obj_file(const char* filename, Object& model) {
	FILE* file;
	fopen_s(&file, filename, "r");
	if (!file) {
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}

	char line[MAX_LINE_LENGTH];
	std::vector<glm::vec3> temp_vertices;
	std::vector<Index> temp_indices;

	while (fgets(line, sizeof(line), file)) {
		read_newline(line); // ���๮�� ����

		// ����(Vertex) ������ ó��
		if (line[0] == 'v' && line[1] == ' ') {
			glm::vec3 vertex;
			sscanf_s(line + 2, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}

		// ��(Face) ������ ó��
		else if (line[0] == 'f' && line[1] == ' ') {
			unsigned int v1, v2, v3;
			sscanf_s(line + 2, "%u %u %u", &v1, &v2, &v3);

			// OBJ �ε����� 1���� �����ϱ� ������ 0���� �����ϵ��� ����
			Index face = { v1 - 1, v2 - 1, v3 - 1 };
			temp_indices.push_back(face);
		}
	}

	fclose(file);

	// Object ����ü�� ���� �� �ε��� �����͸� ����
	model.vertex = temp_vertices; // ���� �����͸� �״�� ����
	model.indexlist = temp_indices; // �ε��� �����͸� �״�� ����

	// ������ ���Ƿ� ���� (���� ���� ����)
	for (size_t i = 0; i < model.vertex.size(); i++) {
		model.color.push_back(glm::vec3(1.0f, 0.0f, 0.0f)); // ���������� ����
	}
}


void init_figure() {

	//---------------------------�����ü-----------------------------
	tetra.vertex.clear();
	tetra.color.clear();
	tetra.indexlist.clear();
	float size = 0.5f, x = 0.0f, y = 0.0f, z = 0.0f;
	float a = 0.85 * size;
	// ���� �߰�
	tetra.vertex.emplace_back(glm::vec3{ x, y + size, z });
	tetra.vertex.emplace_back(glm::vec3{ x, y - size / 2, z + a });
	tetra.vertex.emplace_back(glm::vec3{ x + 0.85 * a, y - size / 2, z - a / 2 });
	tetra.vertex.emplace_back(glm::vec3{ x - 0.85 * a, y - size / 2, z - a / 2 });

	// ���� �߰�
	tetra.color.emplace_back(glm::vec3{ 1.0f, 1.0f, 0.0f });
	tetra.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 1.0f });
	tetra.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 1.0f });
	tetra.color.emplace_back(glm::vec3{ 0.5f, 0.5f, 0.5f });

	//�ε��� ����Ʈ �߰�
	tetra.indexlist.emplace_back(Index{ 0, 1, 2 });
	tetra.indexlist.emplace_back(Index{ 0, 2, 3 });
	tetra.indexlist.emplace_back(Index{ 0, 3, 1 });
	tetra.indexlist.emplace_back(Index{ 1, 2, 3 });

	//---------------------------������ü-----------------------------
	cube.vertex.clear();
	cube.color.clear();
	cube.indexlist.clear();

	// ���� �߰�
	cube.vertex.emplace_back(glm::vec3{ x - size, y + size, z - size });
	cube.vertex.emplace_back(glm::vec3{ x - size, y + size, z + size });
	cube.vertex.emplace_back(glm::vec3{ x + size, y + size, z + size });
	cube.vertex.emplace_back(glm::vec3{ x + size, y + size, z - size });

	cube.vertex.emplace_back(glm::vec3{ x - size, y - size, z - size });
	cube.vertex.emplace_back(glm::vec3{ x - size, y - size, z + size });
	cube.vertex.emplace_back(glm::vec3{ x + size, y - size, z + size });
	cube.vertex.emplace_back(glm::vec3{ x + size, y - size, z - size });

	// ���� �߰�
	cube.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 0.0f });
	cube.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 1.0f });
	cube.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 1.0f });
	cube.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 1.0f });

	cube.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 0.0f });
	cube.color.emplace_back(glm::vec3{ 1.0f, 1.0f, 0.0f });
	cube.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 0.0f });
	cube.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 1.0f });

	// �ε��� �߰�
	cube.indexlist.emplace_back(Index{ 0, 1, 2 });
	cube.indexlist.emplace_back(Index{ 0, 2, 3 });
	cube.indexlist.emplace_back(Index{ 1, 5, 6 });
	cube.indexlist.emplace_back(Index{ 1, 6, 2 });
	cube.indexlist.emplace_back(Index{ 2, 6, 7 });
	cube.indexlist.emplace_back(Index{ 2, 7, 3 });

	cube.indexlist.emplace_back(Index{ 0, 4, 5 });
	cube.indexlist.emplace_back(Index{ 0, 5, 1 });
	cube.indexlist.emplace_back(Index{ 5, 4, 6 });
	cube.indexlist.emplace_back(Index{ 4, 7, 6 });
	cube.indexlist.emplace_back(Index{ 0, 7, 4 });
	cube.indexlist.emplace_back(Index{ 0, 3, 7 });

	// VBO ������Ʈ
	UpdateVBO(tetra);
	UpdateVBO(cube);
}


