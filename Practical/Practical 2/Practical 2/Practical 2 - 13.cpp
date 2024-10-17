#define _CRT_SECURE_NO_WARNINGS
#include "3DGraphics.h"

#define WIDTH 1200
#define HEIGHT 900

GLuint VAO, VBO[2], EBO;
GLchar* vertexSource, * fragmentSource; // �ҽ��ڵ� ���� ����
GLuint vertexShader, fragmentShader; //���̴� ��ü
GLuint shaderProgramID; // ���̴� ���α׷�

Object cube{};
Object tetra{};
Coordinate Coordinate_system{};

bool which_figure{};
bool mode[2]{};
unsigned int which_face{};
unsigned int which_face2{};
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
	//glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);

	InitBuffer();
	CreateShaderProgram();
	//���� �Է��Լ�




	//���� �Լ�
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	glutDisplayFunc(Render);
	
	glutMainLoop();
}
//�׸��� �Լ�
GLvoid Render()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	//��ȯ��� ����
	Make_Matrix();
	//��ǥ�� �׸���
	Draw_Coordinate(Coordinate_system);
	if (which_figure)
	{
		Draw_Tetra(0.0f, 0.0f, 0.0f, 0.5f, tetra);
		
	}
	else
	{
		Draw_Cube(0.0f, 0.0f, 0.0f, 0.5f, cube);
	}
	

	glutSwapBuffers();
}

GLvoid InitBuffer()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &cube.vao);
	glGenBuffers(2, cube.vbo);
	glGenBuffers(1, &cube.EBO);
	glGenVertexArrays(1, &tetra.vao);
	glGenBuffers(2, tetra.vbo);
	glGenBuffers(1, &tetra.EBO);
	glGenVertexArrays(1, &Coordinate_system.vao);
	glGenBuffers(2, Coordinate_system.vbo);
	glGenBuffers(1, &Coordinate_system.EBO);
}

GLvoid Make_Matrix() {
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
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 0.0f });  // Y��: �ʷ�
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 0.0f });  // Y��: �ʷ�
	obj.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 1.0f });  // Z��: �Ķ�
	obj.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 1.0f });  // Z��: �Ķ�

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

void Draw_Cube(float x, float y, float z, float size, Object obj) {
	obj.vertex.clear();
	obj.color.clear();
	obj.indexlist.clear();

	// ���� �߰�
	obj.vertex.emplace_back(glm::vec3{ x - size, y + size, z - size });
	obj.vertex.emplace_back(glm::vec3{ x - size, y + size, z + size });
	obj.vertex.emplace_back(glm::vec3{ x + size, y + size, z + size });
	obj.vertex.emplace_back(glm::vec3{ x + size, y + size, z - size });

	obj.vertex.emplace_back(glm::vec3{ x - size, y - size, z - size });
	obj.vertex.emplace_back(glm::vec3{ x - size, y - size, z + size });
	obj.vertex.emplace_back(glm::vec3{ x + size, y - size, z + size });
	obj.vertex.emplace_back(glm::vec3{ x + size, y - size, z - size });

	// ���� �߰�
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 1.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 1.0f });
	obj.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 1.0f });

	obj.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 0.0f });
	obj.color.emplace_back(glm::vec3{ 1.0f, 1.0f, 0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 1.0f });
	if (which_face == 1 || which_face2 == 1)
	{
		obj.indexlist.emplace_back(Index{ 0, 1, 2 });
		obj.indexlist.emplace_back(Index{ 0, 2, 3 });
	}
	if (which_face == 2 || which_face2 == 2)
	{
		obj.indexlist.emplace_back(Index{ 1, 5, 6 });
		obj.indexlist.emplace_back(Index{ 1, 6, 2 });
	}
	if (which_face == 3 || which_face2 == 3)
	{
		obj.indexlist.emplace_back(Index{ 2, 6, 7 });
		obj.indexlist.emplace_back(Index{ 2, 7, 3 });
	}
	if (which_face == 4 || which_face2 == 4)
	{
		obj.indexlist.emplace_back(Index{ 0, 4, 5 });
		obj.indexlist.emplace_back(Index{ 0, 5, 1 });
	}
	if (which_face == 5 || which_face2 == 5)
	{
		obj.indexlist.emplace_back(Index{ 5, 4, 6 });
		obj.indexlist.emplace_back(Index{ 4, 7, 6 });
	}
	if (which_face == 6 || which_face2 == 6)
	{
		obj.indexlist.emplace_back(Index{ 0, 7, 4 });
		obj.indexlist.emplace_back(Index{ 0, 3, 7 });
	}
	// VBO ������Ʈ
	UpdateVBO(obj);
	glDrawElements(GL_TRIANGLES, 3 * obj.indexlist.size(), GL_UNSIGNED_INT, 0);
	glutPostRedisplay();
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void Draw_Tetra(float x, float y, float z, float size, Object obj) {
	obj.vertex.clear();
	obj.color.clear();
	obj.indexlist.clear();
	float a = 0.85 * size;
	// ���� �߰�
	obj.vertex.emplace_back(glm::vec3{ x, y + size, z });
	obj.vertex.emplace_back(glm::vec3{ x, y - size / 2, z + a});
	obj.vertex.emplace_back(glm::vec3{ x + 0.85 * a, y - size / 2, z - a / 2 });
	obj.vertex.emplace_back(glm::vec3{ x - 0.85 * a, y - size / 2, z -a /2});

	// ���� �߰�
	obj.color.emplace_back(glm::vec3{ 1.0f, 1.0f, 0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 1.0f });
	obj.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 1.0f });
	obj.color.emplace_back(glm::vec3{ 0.5f, 0.5f, 0.5f });

	if (which_face == 7 || which_face2 == 7)
	{
		obj.indexlist.emplace_back(Index{ 0, 1, 2 });
	}
	if (which_face == 8 || which_face2 == 2)
	{
		obj.indexlist.emplace_back(Index{ 0, 2, 3 });
	}
	if (which_face == 9 || which_face2 == 9)
	{
		obj.indexlist.emplace_back(Index{ 0, 3, 1 });
	}
	if (which_face == 10 || which_face2 == 10)
	{
		obj.indexlist.emplace_back(Index{ 1, 2, 3 });
	}
	// VBO ������Ʈ
	UpdateVBO(obj);
	glDrawElements(GL_TRIANGLES, 3 * obj.indexlist.size(), GL_UNSIGNED_INT, 0);
	glutPostRedisplay();
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
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

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case '1':
		which_face = 1;
		which_figure = false;
		which_face2 = 0;
		break;
	case '2':
		which_face = 2;
		which_figure = false;
		which_face2 = 0;
		break;
	case '3':
		which_face = 3;
		which_figure = false;
		which_face2 = 0;
		break;
	case '4':
		which_face = 4;
		which_figure = false;
		which_face2 = 0;
		break;
	case '5':
		which_face = 5;
		which_figure = false;
		which_face2 = 0;
		break;
	case '6':
		which_face = 6;
		which_figure = false;
		which_face2 = 0;
		break;
	case '7':
		which_face = 7;
		which_figure = true;
		which_face2 = 1;
		break;
	case '8':
		which_face = 8;
		which_figure = true;
		which_face2 = 1;
		break;
	case '9':
		which_face = 9;
		which_figure = true;
		which_face2 = 1;
		break;
	case '0':
		which_face = 10;
		which_figure = true;
		which_face2 = 1;
		break;
	case 'c':
		which_figure = false;
		which_face = rand() % 6 + 1;
		do
		{
			which_face2 = rand() % 6 + 1;
		} while (which_face == which_face2);
		break;
	case 't':
		which_figure = true;
		which_face = rand() % 4+7;
		do
		{
			which_face2 = rand() % 4 + 7;
		} while (which_face == which_face2);
		break;
	};
	glutPostRedisplay(); // ���� �ð����� ȭ���� �ٽ� �׸���
}

void Timer(int value) {

}

void Mouse(int button, int state, int x, int y) {

}

void Motion(int x, int y) {

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
	// glsl ���� �б�
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
