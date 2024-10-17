#define _CRT_SECURE_NO_WARNINGS
#include "3DGraphics.h"

//--------------------------- ���� ���� ---------------------------------
float xpos, ypos{}; // ���콺 Ŭ�� ��ġ ��ǥ
bool canmake{}; // ������ ������ �� �ִ��� ����
float radius{}; // ������ ������
int currentvertexcnt{};
int direction1[4]{};
int direction2[4]{};
int theta{};
bool mode[4]{};
int selectedBlock{-1};
vector<Vertex>vertex{};
vector<Color>colors{};
vector<Figure> info{};
float previousMouseX{}, previousMouseY{};
GLchar* vertexSource{}, * fragmentSource{}; // �ҽ��ڵ� ���� ����
GLuint vertexShader{}, fragmentShader{}; // ���̴� ��ü
GLuint shaderProgramID{}; // ���̴� ���α׷�
GLuint vao{}, vbo[2]{}; // VAO�� VBO



// ���α׷��� ���� �Լ�
void main(int argc, char** argv) {
    srand(time(NULL));
    // ������ ����
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
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
    glutTimerFunc(100, Timer, 0);
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
    
    // VBO �����͸� ������Ʈ�Ͽ� GPU�� �ٽ� ����
    UpdateVBO();

    // ȭ���� �ٽ� �׸����� ��û
    glutPostRedisplay();
}

// ȭ�� �׸��� �ݹ� �Լ�
GLvoid drawScene() {
    // ���� ����
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramID); // ���̴� ���α׷� ���

    glBindVertexArray(vao); // VAO ���ε�
    std::cout << "������ ���� : "<< info.size() << std::endl;
    // ���� �׸���: �� ������ ���� ������ ���� �ٸ��� �׷��� ��
    int offset = 0;
    for (int i = 0; i < info.size(); i++)
    {
        switch (info.at(i).shape)
        {
        case 1://�� �׸���(���� �ﰢ��)
            if (info.at(i).is_active) {
                glDrawArrays(GL_TRIANGLES, offset, 3); // �ﰢ��: 3���� ����
            }
            offset += 3;
            break;
        case 2: //�� �׸���
            if (info.at(i).is_active) {
                glDrawArrays(GL_LINES, offset, 2); // �ﰢ��: 3���� ����
            }
            offset += 2;
            break;
        case 3:// �ﰢ�� �׸���
            if (info.at(i).is_active) {
                glDrawArrays(GL_TRIANGLES, offset, 3); // �ﰢ��: 3���� ����
            }
            offset += 3;
            break;
        case 4: // �簢�� �׸���
            if (info.at(i).is_active) {
                for (int j = 0; j < 2; j++)
                {
                    glDrawArrays(GL_TRIANGLES, offset + j, 3); // �ﰢ��: 3���� ���� 
                }
            }
            offset += 4;
            break;
        case 5: // ������ �׸���
            if (info.at(i).is_active) {
                for (int j = 0; j < 3; j++)
                {
                    glDrawArrays(GL_TRIANGLES, offset + j, 3); // �ﰢ��: 3���� ����
                }
            }
            offset += 5;
            break;
        default:
            break;
        }
         
    }

    glutSwapBuffers(); // ȭ�鿡 ���
}

// ������ �������� �ݹ� �Լ�
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}
// ���� �ʱ�ȭ �Լ�
void init_color(int shape) {
    float r = static_cast<float>(rand()) / RAND_MAX;
    float g = static_cast<float>(rand()) / RAND_MAX;
    float b = static_cast<float>(rand()) / RAND_MAX;
    for (int i = 0; i < shape; i++)
    {
        colors.emplace_back(Color{ r + static_cast<float>(rand()) / RAND_MAX / 5,g + static_cast<float>(rand()) / RAND_MAX / 5,b + static_cast<float>(rand()) / RAND_MAX / 5 });
    }
}

//Ÿ�̸� �Լ�
void Timer(int value) {
    float moving_gap = 0.05f;
    if (info.size() > 15)
    {
        int start_index = 51;
        for (int i = 15; i < info.size(); i++)
        {
            if (info.at(i).shape != 1)
            {
                bool change_dir[2] = {};
                if (selectedBlock != i && info.at(i).is_active)
                {
                    for (int j = 0; j < info.at(i).shape; j++)
                    {
                        switch (info.at(i).dir1)
                        {
                        case 0: // ��
                            vertex.at(j + start_index).x -= moving_gap;
                            if (vertex.at(j + start_index).x < -1.0f)
                            {
                                change_dir[0] = true;
                            }
                            break;
                        case 1: // ��
                            vertex.at(j + start_index).x += moving_gap;
                            if (vertex.at(j + start_index).x > 1.0f)
                            {
                                change_dir[0] = true;
                            }
                            break;
                        default:
                            break;
                        }
                        switch (info.at(i).dir2)
                        {
                        case 0: // ��
                            vertex.at(j + start_index).y -= moving_gap;
                            if (vertex.at(j + start_index).y < -1.0f)
                            {
                                change_dir[1] = true;
                            }
                            break;
                        case 1: // ��
                            vertex.at(j + start_index).y += moving_gap;
                            if (vertex.at(j + start_index).y > 1.0f)
                            {
                                change_dir[1] = true;
                            }
                            break;
                        default:
                            break;
                        }
                    }
                    if (change_dir[0])
                    {
                        info.at(i).dir1 = (info.at(i).dir1 + 1) % 2;
                    }
                    if (change_dir[1])
                    {
                        info.at(i).dir2 = (info.at(i).dir2 + 1) % 2;
                    }
                }
                start_index += info.at(i).shape;
            }
            else
            {    
                bool change_dir[2] = {};
                if (selectedBlock != i && info.at(i).is_active)
                {
                    for (int j = 0; j < 3; j++)
                    {
                        switch (info.at(i).dir1)
                        {
                        case 0: // ��
                            vertex.at(j + start_index).x -= moving_gap;
                            if (vertex.at(j + start_index).x < -1.0f)
                            {
                                change_dir[0] = true;
                            }
                            break;
                        case 1: // ��
                            vertex.at(j + start_index).x += moving_gap;
                            if (vertex.at(j + start_index).x > 1.0f)
                            {
                                change_dir[0] = true;
                            }
                            break;
                        default:
                            break;
                        }
                        switch (info.at(i).dir2)
                        {
                        case 0: // ��
                            vertex.at(j + start_index).y -= moving_gap;
                            if (vertex.at(j + start_index).y < -1.0f)
                            {
                                change_dir[1] = true;
                            }
                            break;
                        case 1: // ��
                            vertex.at(j + start_index).y += moving_gap;
                            if (vertex.at(j + start_index).y > 1.0f)
                            {
                                change_dir[1] = true;
                            }
                            break;
                        default:
                            break;
                        }
                    }
                    if (change_dir[0])
                    {
                        info.at(i).dir1 = (info.at(i).dir1 + 1) % 2;
                    }
                    if (change_dir[1])
                    {
                        info.at(i).dir2 = (info.at(i).dir2 + 1) % 2;
                    }
                }
                start_index += 3;
            }
            switch (info.at(i).dir1)
            {
            case 0: // ��
                info.at(i).checkbox.left -= moving_gap;
                info.at(i).checkbox.right -= moving_gap;
                break;
            case 1: // ��
                info.at(i).checkbox.left += moving_gap;
                info.at(i).checkbox.right += moving_gap;
                break;
            }
            switch (info.at(i).dir2)
            {
            case 0: // ��
                info.at(i).checkbox.top -= moving_gap;
                info.at(i).checkbox.bottom -= moving_gap;
                break;
            case 1: // ��
                info.at(i).checkbox.top += moving_gap;
                info.at(i).checkbox.bottom += moving_gap;
                break;
            }
            
        }
    }


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
        std::cout << "Info�� ����" << info.size() << std::endl;
        for (int i = info.size() - 1; i >= 0; i--) {
            if (info.at(i).is_active)
            {
                BoundingBox& rect = info.at(i).checkbox;
                if ((xpos > rect.left) && (xpos < rect.right) && (ypos > rect.bottom) && (ypos < rect.top)) {
                    selectedBlock = i;
                    previousMouseX = xpos;
                    previousMouseY = ypos;
                    break;
                }
            }
        }
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && selectedBlock != -1)
    {
        for (int i = info.size() - 1; i >= 0; i--)
        {
            if (info.at(i).is_active&&IsRectOverlap(info.at(selectedBlock).checkbox, info.at(i).checkbox) && i!= selectedBlock)
            {
                int newshape = (info.at(i).shape + info.at(selectedBlock).shape);
                if (newshape % 5 != 0)
                {
                    newshape %= 5;
                }
                else
                {
                    newshape = 5;
                }
                switch (newshape)
                {
                case 1:
                    Make_triangle(1);
                    break;
                case 2:
                    Make_Line(0);
                    break;
                case 3:
                    Make_triangle(0);
                    break;
                case 4:
                    Make_Rect(0);
                    break;
                case 5:
                    Make_Penta(0);
                    break;
                default:
                    break;
                }
                info.at(selectedBlock).is_active = false;
                info.at(i).is_active = false;
                break;
            }
        }
        selectedBlock = -1;
    }
    // VBO �����͸� ������Ʈ�Ͽ� GPU�� �ٽ� ����
    UpdateVBO();

    // ȭ���� �ٽ� �׸����� ��û
    glutPostRedisplay();
}

//���콺 ��ǰ˻�(Ŭ����Active)
void Motion(int x, int y) {
    if (selectedBlock != -1) {  // ���õ� ����� ���� ���� ����
        // OpenGL ��ǥ�� ��ȯ
        float xpos = static_cast<float>(x) / (ROW / 2.0f) - 1.0f;
        float ypos = 1.0f - static_cast<float>(y) / (COL / 2.0f);

        // ���콺 �̵��� ���
        float deltaX = xpos - previousMouseX;
        float deltaY = ypos - previousMouseY;

        // ���õ� ������ checkbox ��ǥ�� ������Ʈ
        info.at(selectedBlock).checkbox.left += deltaX;
        info.at(selectedBlock).checkbox.right += deltaX;
        info.at(selectedBlock).checkbox.top += deltaY;
        info.at(selectedBlock).checkbox.bottom += deltaY;

        // ���õ� ������ vertex ��ǥ�� ������Ʈ
        int offset = 0;
        for (int i = 0; i < selectedBlock; i++) {
            
                if (info.at(i).shape == 1)
                {
                    offset += info.at(i).shape * 3;
                }
                else
                {
                    offset += info.at(i).shape;
                }
            
        }

        if (info.at(selectedBlock).shape == 1)
        {
            for (int j = 0; j < 3; j++) {
                vertex.at(offset + j).x += deltaX;
                vertex.at(offset + j).y += deltaY;
            }
        }
        else
        {
            for (int j = 0; j < info.at(selectedBlock).shape; j++) {
                vertex.at(offset + j).x += deltaX;
                vertex.at(offset + j).y += deltaY;
            }
        }
        previousMouseX = xpos;
        previousMouseY = ypos;

        // VBO �����͸� ������Ʈ�Ͽ� GPU�� �ٽ� ����
        UpdateVBO();

        // ȭ���� �ٽ� �׸����� ��û
        glutPostRedisplay();
    }
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

//�簢�� �浹�˻�
bool IsRectOverlap(BoundingBox rect1, BoundingBox rect2) {
    if (rect1.right <= rect2.left || rect1.left >= rect2.right) {
        return false;
    }
    if (rect1.top <= rect2.bottom || rect1.bottom >= rect2.top) {
        return false;
    }
    // �� �� ��ο��� ��ģ�ٸ� �浹�� �߻��� ��
    return true;
}

//���� ����
void Make_Line(int mode) {
    float rnd_x = static_cast<float>(rand()) / RAND_MAX;
    if (rand() % 2)
    {
        rnd_x = -rnd_x;
    }
    float rnd_y = static_cast<float>(rand()) / RAND_MAX;
    if (rand() % 2)
    {
        rnd_y = -rnd_y;
    }
    radius = static_cast<float>(rand()) / RAND_MAX / 5;
    BoundingBox bbox = {
    rnd_x - radius / 3.0f,
    rnd_y + radius / 3.0f,
    rnd_x + radius / 3.0f,
    rnd_y - radius / 3.0f
    };
    info.emplace_back(Figure{ 2, true, bbox,rand()%2,rand() % 2 });
    vertex.emplace_back(Vertex{ rnd_x - radius,rnd_y + radius,0.0f });
    vertex.emplace_back(Vertex{ rnd_x + radius,rnd_y - radius,0.0f });
    init_color(2);
}

//�ﰢ�� ����
void Make_triangle(int size) {
    float rnd_x = static_cast<float>(rand()) / RAND_MAX;
    if (rand() % 2)
    {
        rnd_x = -rnd_x;
    }
    float rnd_y = static_cast<float>(rand()) / RAND_MAX;
    if (rand() % 2)
    {
        rnd_y = -rnd_y;
    }
    if (size == 0) // random size
    {
        radius = static_cast<float>(rand()) / RAND_MAX / 4 + 0.05f;        
    }
    else if (size == 1) // point size
    {
        radius = 0.03f;
    }
    else if (size == 2) // Middle size
    {
        radius = 0.15f;
    }
    BoundingBox bbox = {
        rnd_x - radius / 2.0f,
        rnd_y + radius / 2.0f,
        rnd_x + radius / 2.0f,
        rnd_y - radius / 2.0f
    };

    if (size == 1) { // point size
        info.emplace_back(Figure{ 1, true, bbox ,rand() % 2 ,rand() % 2 });
    }
    else
    {
        info.emplace_back(Figure{ 3, true, bbox ,rand() % 2 ,rand() % 2 });
    }
    
    
    vertex.emplace_back(Vertex{ rnd_x, rnd_y + radius, 0.0f });
    vertex.emplace_back(Vertex{ rnd_x - radius * 0.85f, rnd_y - radius / 2.0f, 0.0f });
    vertex.emplace_back(Vertex{ rnd_x + radius * 0.85f, rnd_y - radius / 2.0f, 0.0f });
    init_color(3);
}

//�簢�� �����
void Make_Rect(int mode) {
    float rnd_x = static_cast<float>(rand()) / RAND_MAX;
    if (rand() % 2)
    {
        rnd_x = -rnd_x;
    }
    float rnd_y = static_cast<float>(rand()) / RAND_MAX;
    if (rand() % 2)
    {
        rnd_y = -rnd_y;
    }
    if (mode == 0) // random size
    {
        radius = static_cast<float>(rand()) / RAND_MAX / 4 + 0.05f;
    }
    else if (mode == 1) // point size
    {
        radius = 0.01f;
    }
    else if (mode == 2) // Middle size
    {
        radius = 0.15f;
    }
    BoundingBox bbox = {
        rnd_x - radius,
        rnd_y + radius,
        rnd_x + radius,
        rnd_y - radius
    };
    info.emplace_back(Figure{ 4, true, bbox,rand() % 2 ,rand() % 2 });
    vertex.emplace_back(Vertex{ rnd_x + radius, rnd_y - radius, 0.0f });
    vertex.emplace_back(Vertex{ rnd_x - radius, rnd_y - radius, 0.0f });
    vertex.emplace_back(Vertex{ rnd_x + radius, rnd_y + radius, 0.0f });
    vertex.emplace_back(Vertex{ rnd_x - radius, rnd_y + radius, 0.0f });
    init_color(4);
}

//������ �����
void Make_Penta(int mode) {
    float rnd_x = static_cast<float>(rand()) / RAND_MAX;
    if (rand() % 2)
    {
        rnd_x = -rnd_x;
    }
    float rnd_y = static_cast<float>(rand()) / RAND_MAX;
    if (rand() % 2)
    {
        rnd_y = -rnd_y;
    }
    float theta = 18.0f;
    
    if (mode == 0) // random size
    {
        radius = static_cast<float>(rand()) / RAND_MAX / 4 + 0.05f;
    }
    else if (mode == 1) // point size
    {
        radius = 0.01f;
    }
    else if (mode == 2) // Middle size
    {
        radius = 0.15f;
    }
    BoundingBox bbox = {
    rnd_x - radius * 0.61f,
    rnd_y + radius * 0.61f,
    rnd_x + radius * 0.61f,
    rnd_y - radius * 0.61f
    };
    info.emplace_back(Figure{ 5, true, bbox,rand() % 2 ,rand() % 2 });

    vertex.emplace_back(Vertex{ rnd_x + radius * cos(glm::radians(theta)), rnd_y + radius * sin(glm::radians(theta)), 0.0f });
    theta += 72.0f;
    vertex.emplace_back(Vertex{ rnd_x + radius * cos(glm::radians(theta)), rnd_y + radius * sin(glm::radians(theta)), 0.0f });
    theta -= 144.0f;
    vertex.emplace_back(Vertex{ rnd_x + radius * cos(glm::radians(theta)), rnd_y + radius * sin(glm::radians(theta)), 0.0f });
    theta += 216.0f;
    vertex.emplace_back(Vertex{ rnd_x + radius * cos(glm::radians(theta)), rnd_y + radius * sin(glm::radians(theta)), 0.0f });
    theta += 72.0f;
    vertex.emplace_back(Vertex{ rnd_x + radius * cos(glm::radians(theta)), rnd_y + radius * sin(glm::radians(theta)), 0.0f });
    init_color(5);
}

// VAO �� VBO �ʱ�ȭ
void InitBuffer() {
    glGenVertexArrays(1, &vao); // VAO ����
    glBindVertexArray(vao); // VAO ���ε�

    glGenBuffers(2, vbo); // 2���� VBO ����

    // ���� ��ǥ �����Ͱ� �����ϴ��� Ȯ��
    if (!vertex.empty()) {
        // ���� ��ǥ �����͸� VBO�� ����
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(Vertex), &vertex[0], GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x)); // x, y, z�� ����
        glEnableVertexAttribArray(0);
    }
    // ���� �����͸� VBO�� ���� (colors �迭�� �ִٰ� ����)
    if (!colors.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(Color), &colors[0], GL_DYNAMIC_DRAW); // colors �����ʹ� ������ ������ �����ȴٰ� ����
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);
    }
}

// VAO �� VBO �ʱ�ȭ
void UpdateVBO() {
    glBindVertexArray(vao);  // VAO ���ε� �߰�

    // ���� �����͸� ������Ʈ
    if (!vertex.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(Vertex), &vertex[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
        glEnableVertexAttribArray(0);
    }

    // ���� �����͸� ������Ʈ
    if (!colors.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(Color), &colors[0], GL_DYNAMIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Color), (void*)offsetof(Color, r));
        glEnableVertexAttribArray(1);
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