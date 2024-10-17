#define _CRT_SECURE_NO_WARNINGS
#include "3DGraphics.h"

//--------------------------- ���� ���� ---------------------------------
float xpos, ypos{}; // ���콺 Ŭ�� ��ġ ��ǥ
vector<Vertex>vertex{};
vector<Index>indexlist{};
Box cube;

float previousMouseX{}, previousMouseY{};
GLchar* vertexSource{}, * fragmentSource{}; // �ҽ��ڵ� ���� ����
GLuint vertexShader{}, fragmentShader{}; // ���̴� ��ü
GLuint shaderProgramID{}; // ���̴� ���α׷�
GLuint vao{}, vbo[2]{}, EBO; // VAO�� VBO



// ���α׷��� ���� �Լ�
void main(int argc, char** argv) {
    srand(time(NULL));
    // ������ ����
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(ROW, COL);
    glutCreateWindow("Practical 1");

    // GLEW �ʱ�ȭ
    glewExperimental = GL_TRUE;
    glewInit();
    InitBuffer(); // ���� �ʱ�ȭ
    make_vertexShaders(); // ���ؽ� ���̴� ����
    make_fragmentShaders(); // �����׸�Ʈ ���̴� ����
    shaderProgramID = make_shaderProgram(); // ���̴� ���α׷� ����
    init_figure();
    // �ݹ� �Լ� ���
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutKeyboardFunc(Keyboard);
    glutMainLoop(); // ���� ���� ����
}



//������ �ʱ�ȭ
void init_figure() {
    Make_cube(0.0f, 0.0f, 0.0f, 0.2f);
    // VBO �����͸� ������Ʈ�Ͽ� GPU�� �ٽ� ����

    // ȭ���� �ٽ� �׸����� ��û
    glutPostRedisplay();
}

// ȭ�� �׸��� �ݹ� �Լ�
GLvoid drawScene() {
    // ���� ����
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramID); // ���̴� ���α׷� ���
    glm::mat4 transformMatrix = glm::mat4(1.0f);
    //--- ������ �𵨸� ��ȯ ��� �����
    transformMatrix = glm::translate(transformMatrix, glm::vec3(0.0f, 0.0f, 0.0f)); //--- model ��Ŀ� �̵� ��ȯ ����
    //--- ���̴� ���α׷����� modelTransform ���� ��ġ ��������
    unsigned int transformLocation = glGetUniformLocation(shaderProgramID, "transform");
    //--- modelTransform ������ ��ȯ �� �����ϱ�
    glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));

    glBindVertexArray(vao); // VAO ���ε�
    glBindVertexArray(cube.VAO); // VAO ���ε�
    // ���� �׸���: �� ������ ���� ������ ���� �ٸ��� �׷��� ��
    int offset = 0;
    std::cout << cube.indexlist.size() << std::endl;
    glDrawElements(GL_TRIANGLES, cube.indexlist.size(), GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glutSwapBuffers(); // ȭ�鿡 ���
}

// ������ �������� �ݹ� �Լ�
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}
// ���� �ʱ�ȭ �Լ�
void random_color(int shape) {
    float r = static_cast<float>(rand()) / RAND_MAX;
    float g = static_cast<float>(rand()) / RAND_MAX;
    float b = static_cast<float>(rand()) / RAND_MAX;

}

//Ÿ�̸� �Լ�
void Timer(int value) {


    // VBO �����͸� ������Ʈ�Ͽ� GPU�� �ٽ� ����
    UpdateVBO();

    // ȭ���� �ٽ� �׸����� ��û
    glutPostRedisplay();
    glutTimerFunc(100, Timer, value);
}

//���콺 Ŭ�� �̺�Ʈ �ݹ� �Լ�
void Mouse(int button, int state, int x, int y) {
    // OpenGL ��ǥ�� ��ȯ (ȭ���� ũ�⸦ ���)
    float xpos = (static_cast<float>(x) / (ROW / 2.0f)) - 1.0f;
    float ypos = 1.0f - (static_cast<float>(y) / (COL / 2.0f));

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

    }
    // VBO �����͸� ������Ʈ�Ͽ� GPU�� �ٽ� ����
    UpdateVBO();

    // ȭ���� �ٽ� �׸����� ��û
    glutPostRedisplay();
}

//���콺 ��ǰ˻�(Ŭ����Active)
void Motion(int x, int y) {

    // VBO �����͸� ������Ʈ�Ͽ� GPU�� �ٽ� ����
    UpdateVBO();

    // ȭ���� �ٽ� �׸����� ��û
    glutPostRedisplay();

}

// Ű���� �Է� �̺�Ʈ �ݹ� �Լ�
GLvoid Keyboard(unsigned char key, int x, int y) {
    switch (key) {

        break;
    }


    // VBO �����͸� ������Ʈ�Ͽ� GPU�� �ٽ� ����
    UpdateVBO();

    // ȭ���� �ٽ� �׸����� ��û
    glutPostRedisplay();
}



//������ü ����
void Make_cube(float x, float y, float z, float size) {

    // ���ؽ� ���� ����
    cube.vertex.emplace_back(Vertex{ x, y, z, 0.7f, 0.0f, 0.7f });
    cube.vertex.emplace_back(Vertex{ x, y + size, z, 0.7f, 0.7f, 0.0f });
    cube.vertex.emplace_back(Vertex{ x + size, y + size, z, 0.0f, 0.0f, 0.7f });
    cube.vertex.emplace_back(Vertex{ x + size, y, z, 0.0f, 0.7f, 0.0f });

    cube.vertex.emplace_back(Vertex{ x, y, z + size, 0.7f, 0.0f, 0.0f });
    cube.vertex.emplace_back(Vertex{ x, y + size, z + size, 0.0f, 0.7f, 0.7f });
    cube.vertex.emplace_back(Vertex{ x + size, y + size, z, 0.7f, 0.0f, 0.7f });
    cube.vertex.emplace_back(Vertex{ x + size, y, z + size, 0.7f, 0.7f, 0.0f });

    // �ε��� ����Ʈ ����
    cube.indexlist.emplace_back(Index{ 0, 1, 2 });
    cube.indexlist.emplace_back(Index{ 0, 2, 3 });
    cube.indexlist.emplace_back(Index{ 0, 4, 5 });
    cube.indexlist.emplace_back(Index{ 0, 5, 1 });
    cube.indexlist.emplace_back(Index{ 1, 5, 6 });
    cube.indexlist.emplace_back(Index{ 1, 6, 2 });
    cube.indexlist.emplace_back(Index{ 2, 6, 7 });
    cube.indexlist.emplace_back(Index{ 2, 7, 3 });
    cube.indexlist.emplace_back(Index{ 5, 4, 7 });
    cube.indexlist.emplace_back(Index{ 5, 7, 6 });
    cube.indexlist.emplace_back(Index{ 4, 0, 3 });
    cube.indexlist.emplace_back(Index{ 4, 3, 7 });

    // �ܼ� �α׷� ���� �� �ε��� ����Ʈ ���
    std::cout << "Vertices:\n";
    for (const auto& v : cube.vertex) {
        std::cout << "Position: (" << v.x << ", " << v.y << ", " << v.z << "), Color: (" << v.r << ", " << v.g << ", " << v.b << ")\n";
    }

    std::cout << "Indices:\n";
    for (const auto& i : cube.indexlist) {
        std::cout << "Index: (" << i.v1 << ", " << i.v2 << ", " << i.v3 << ")\n";
    }

    // VBO �����͸� ������Ʈ�Ͽ� GPU�� �ٽ� ����
    UpdateVBO(cube);
}






// VAO �� VBO �ʱ�ȭ
void InitBuffer() {
    glGenVertexArrays(1, &vao); // VAO ����
    glGenBuffers(2, vbo); // 2���� VBO ����
    glBindVertexArray(vao); // VAO ���ε�

    // ���� ��ǥ �����͸� VBO�� ����
    if (!vertex.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(Vertex), &vertex[0], GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x)); // x, y, z�� ����
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(Vertex), &vertex[0], GL_DYNAMIC_DRAW); // colors �����ʹ� ������ ������ �����ȴٰ� ����
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r)); // r, g, b�� ����
        glEnableVertexAttribArray(1);
    }

    // �ε��� �����͸� EBO�� ����
    if (!indexlist.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexlist.size() * sizeof(Index), &indexlist[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Index), (void*)offsetof(Index, v1)); // ����
        glEnableVertexAttribArray(0); // ����
    }

    glBindVertexArray(0); // VAO ���ε� ����
}

// VBO ������Ʈ
void UpdateVBO() {
    glBindVertexArray(vao);  // VAO ���ε� �߰�

    // ���� �����͸� ������Ʈ
    if (!vertex.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(Vertex), &vertex[0], GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x)); // x, y, z�� ����
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(Vertex), &vertex[0], GL_DYNAMIC_DRAW); // colors �����ʹ� ������ ������ �����ȴٰ� ����
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r)); // r, g, b�� ����
        glEnableVertexAttribArray(1);
    }

    // �ε��� �����͸� EBO�� ����
    if (!indexlist.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexlist.size() * sizeof(Index), &indexlist[0], GL_STATIC_DRAW);
        // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Index), (void*)offsetof(Index, v1)); // ����
        // glEnableVertexAttribArray(0); // ����
    }

    glBindVertexArray(0); // VAO ���ε� ����
}



void UpdateVBO(Box box) {
    glGenVertexArrays(1, &box.VAO); // VAO ����
    glGenBuffers(2, box.VBO); // 2���� VBO ����
    glGenBuffers(1, &box.EBO); //EBO ����
    glBindVertexArray(box.VAO);  // VAO ���ε� �߰�

    // ���� �����͸� ������Ʈ
    if (!box.vertex.empty()) {
        std::cout << "ť�� ���ؽ����� �ʱ�ȭ\n";
        glBindBuffer(GL_ARRAY_BUFFER, box.VBO[0]);
        glBufferData(GL_ARRAY_BUFFER, box.vertex.size() * sizeof(Vertex), &box.vertex[0], GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x)); // x, y, z�� ����
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, box.VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, box.vertex.size() * sizeof(Vertex), &box.vertex[0], GL_DYNAMIC_DRAW); // colors �����ʹ� ������ ������ �����ȴٰ� ����
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r)); // r, g, b�� ����
        glEnableVertexAttribArray(1);
    }
    if (!box.indexlist.empty()) {
        std::cout << "ť�� �ε�������Ʈ �ʱ�ȭ\n";
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, box.EBO);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, box.indexlist.size() * sizeof(Index), &box.indexlist[0], GL_STATIC_DRAW);
        // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Index), (void*)offsetof(Index, v1));
    }

    glBindVertexArray(0);  // VAO ���ε� ����
}

// ���̴� ���α׷� ���� �Լ�
GLuint make_shaderProgram() {
    GLuint shaderID = glCreateProgram();
    glAttachShader(shaderID, vertexShader);
    glAttachShader(shaderID, fragmentShader);
    glLinkProgram(shaderID);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderID;
}

// ���ؽ� ���̴� ���� �Լ�
void make_vertexShaders() {
    vertexSource = filetobuf("vertex.vs");
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
    glCompileShader(vertexShader);
}

// �����׸�Ʈ ���̴� ���� �Լ�
void make_fragmentShaders() {
    fragmentSource = filetobuf("fragment.fs");
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
    glCompileShader(fragmentShader);
}

// ������ ���۷� �о���� �Լ�
char* filetobuf(const char* file) {
    FILE* fptr;
    long length;
    char* buf;

    fptr = fopen(file, "rb");
    if (!fptr) return NULL;

    fseek(fptr, 0, SEEK_END);
    length = ftell(fptr);
    buf = (char*)malloc(length + 1);
    fseek(fptr, 0, SEEK_SET);
    fread(buf, length, 1, fptr);
    fclose(fptr);

    buf[length] = 0;
    return buf;
}