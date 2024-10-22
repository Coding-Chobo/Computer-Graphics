#define _CRT_SECURE_NO_WARNINGS
#include "3DGraphics.h"

#define WIDTH 1200
#define HEIGHT 900

GLuint VAO, VBO[2], EBO;
GLchar* vertexSource, * fragmentSource; // �ҽ��ڵ� ���� ����
GLuint vertexShader, fragmentShader; // ���̴� ��ü
GLuint shaderProgramID; // ���̴� ���α׷�

Object cube{}, tetra{};
Coordinate Coordinate_system{};

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
    glutTimerFunc(100, Timer, 0);

    // ���� ���� Ȱ��ȭ
    glEnable(GL_DEPTH_TEST);

    InitBuffer();
    CreateShaderProgram();
    init_figure();

    // ���� �Լ�
    glutReshapeFunc(Reshape);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutKeyboardFunc(Keyboard);
    glutDisplayFunc(Render);

    glutMainLoop();
}

// �׸��� �Լ�
GLvoid Render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramID);

    // ��ǥ�� �׸���
    Draw_Coordinate(Coordinate_system);

    // ť�� �׸���
    glBindVertexArray(cube.vao);
    glDrawElements(GL_TRIANGLES, cube.indexlist.size() * 3, GL_UNSIGNED_INT, 0);

    // �����ü �׸���
    glBindVertexArray(tetra.vao);
    glDrawElements(GL_TRIANGLES, tetra.indexlist.size() * 3, GL_UNSIGNED_INT, 0);

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

void DrawCube(float x, float y, float z, float size, Object obj) {
    // VBO ������Ʈ
    UpdateVBO(obj);
    std::cout << "Index count: " << obj.indexlist.size() << std::endl;
}

GLvoid UpdateVBO(Object object) {
    // VAO ���ε�
    glBindVertexArray(object.vao);

    // ��ġ ���� ���ε�
    glBindBuffer(GL_ARRAY_BUFFER, object.vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, object.vertex.size() * sizeof(glm::vec3), object.vertex.data(), GL_DYNAMIC_DRAW);

    // �ε��� ���� ���ε�
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, object.indexlist.size() * sizeof(Index), object.indexlist.data(), GL_DYNAMIC_DRAW);

    // ��ġ �Ӽ� ����
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
    glEnableVertexAttribArray(0);

    // ���� ���� ���ε�
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

void Draw_Tetra(float x, float y, float z, float size, Object obj) {
    glutPostRedisplay();
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void init_figure() {

    //---------------------------������ü-----------------------------
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

    // �ε��� ����Ʈ �߰�
    tetra.indexlist.emplace_back(Index{ 0, 1, 2 });
    tetra.indexlist.emplace_back(Index{ 0, 2, 3 });
    tetra.indexlist.emplace_back(Index{ 0, 3, 1 });
    tetra.indexlist.emplace_back(Index{ 1, 2, 3 });

    //---------------------------�����ü-----------------------------
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

void Timer(int value) {
    // ȭ���� �ٽ� �׸����� ��û
    glutPostRedisplay();
    glutTimerFunc(100, Timer, value);
}

void Mouse(int button, int state, int x, int y) {}

void Motion(int x, int y) {}

GLvoid Keyboard(unsigned char key, int x, int y) {}