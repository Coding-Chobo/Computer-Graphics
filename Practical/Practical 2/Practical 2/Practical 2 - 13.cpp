#define _CRT_SECURE_NO_WARNINGS
#include "3DGraphics.h"

//--------------------------- 전역 변수 ---------------------------------
float xpos, ypos{}; // 마우스 클릭 위치 좌표
bool canmake{}; // 도형을 생성할 수 있는지 여부
float radius{}; // 도형의 반지름
int currentvertexcnt{};
int direction1[4]{};
int direction2[4]{};
int theta{};
bool mode[4]{};
int selectedBlock{ -1 };
vector<Vertex>vertex{};
vector<Index>index{};
Box cube;

float previousMouseX{}, previousMouseY{};
GLchar* vertexSource{}, * fragmentSource{}; // 소스코드 저장 변수
GLuint vertexShader{}, fragmentShader{}; // 세이더 객체
GLuint shaderProgramID{}; // 셰이더 프로그램
GLuint vao{}, vbo[2]{}, EBO; // VAO와 VBO



// 프로그램의 메인 함수
void main(int argc, char** argv) {
    srand(time(NULL));
    // 윈도우 생성
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(ROW, COL);
    glutCreateWindow("Practical 1");

    // GLEW 초기화
    glewExperimental = GL_TRUE;
    glewInit();
    InitBuffer(); // 버퍼 초기화
    make_vertexShaders(); // 버텍스 셰이더 생성
    make_fragmentShaders(); // 프래그먼트 셰이더 생성
    shaderProgramID = make_shaderProgram(); // 셰이더 프로그램 생성
    init_figure();
    glutTimerFunc(100, Timer, 0);
    // 콜백 함수 등록
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutKeyboardFunc(Keyboard);
    glutMainLoop(); // 메인 루프 진입
}



//도형값 초기화
void init_figure() {
    Make_cube(0.0f, 0.0f, 0.0f, 0.1f);
    // VBO 데이터를 업데이트하여 GPU에 다시 전송
    UpdateVBO();

    // 화면을 다시 그리도록 요청
    glutPostRedisplay();
}

// 화면 그리기 콜백 함수
GLvoid drawScene() {
    // 배경색 설정
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramID); // 셰이더 프로그램 사용

    glBindVertexArray(vao); // VAO 바인딩
    // 도형 그리기: 각 도형의 정점 개수에 따라 다르게 그려야 함
    int offset = 0;
    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

    glutSwapBuffers(); // 화면에 출력
}

// 윈도우 리사이즈 콜백 함수
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}
// 색상 초기화 함수
void init_color(int shape) {
    float r = static_cast<float>(rand()) / RAND_MAX;
    float g = static_cast<float>(rand()) / RAND_MAX;
    float b = static_cast<float>(rand()) / RAND_MAX;

}

//타이머 함수
void Timer(int value) {


    // VBO 데이터를 업데이트하여 GPU에 다시 전송
    UpdateVBO();

    // 화면을 다시 그리도록 요청
    glutPostRedisplay();
    glutTimerFunc(100, Timer, value);
}

//마우스 클릭 이벤트 콜백 함수
void Mouse(int button, int state, int x, int y) {
    // OpenGL 좌표로 변환 (화면의 크기를 고려)
    float xpos = (static_cast<float>(x) / (ROW / 2.0f)) - 1.0f;
    float ypos = 1.0f - (static_cast<float>(y) / (COL / 2.0f));

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
       
    }
    // VBO 데이터를 업데이트하여 GPU에 다시 전송
    UpdateVBO();

    // 화면을 다시 그리도록 요청
    glutPostRedisplay();
}

//마우스 모션검사(클릭중Active)
void Motion(int x, int y) {

        // VBO 데이터를 업데이트하여 GPU에 다시 전송
        UpdateVBO();

        // 화면을 다시 그리도록 요청
        glutPostRedisplay();
    
}

// 키보드 입력 이벤트 콜백 함수
GLvoid Keyboard(unsigned char key, int x, int y) {
    switch (key) {

        break;
    }


    // VBO 데이터를 업데이트하여 GPU에 다시 전송
    UpdateVBO();

    // 화면을 다시 그리도록 요청
    glutPostRedisplay();
}



//정육면체 생성
void Make_cube(float x, float y, float z, float size) {
    cube.vertex.emplace_back(Vertex{ x,y,z,0.7f,0.7f,0.7f });
    cube.vertex.emplace_back(Vertex{ x,y + size,z,0.7f,0.7f,0.7f });
    cube.vertex.emplace_back(Vertex{ x + size,y + size,z,0.7f,0.7f,0.7f });
    cube.vertex.emplace_back(Vertex{ x + size,y,z,0.7f,0.7f,0.7f });

    cube.vertex.emplace_back(Vertex{ x,y,z + size,0.7f,0.7f,0.7f });
    cube.vertex.emplace_back(Vertex{ x,y + size,z + size,0.7f,0.7f,0.7f });
    cube.vertex.emplace_back(Vertex{ x,y,z + size,0.7f,0.7f,0.7f });
    cube.vertex.emplace_back(Vertex{ x,y,z + size,0.7f,0.7f,0.7f });
    cube.indexlist.emplace_back(Index{ 0,1,2 });
    cube.indexlist.emplace_back(Index{ 0,2,3 });
    cube.indexlist.emplace_back(Index{ 0,4,5 });
    cube.indexlist.emplace_back(Index{ 0,5,1 });
    cube.indexlist.emplace_back(Index{ 1,5,6 });
    cube.indexlist.emplace_back(Index{ 1,6,2 });

    cube.indexlist.emplace_back(Index{ 2,6,7 });
    cube.indexlist.emplace_back(Index{ 2,7,3 });
    cube.indexlist.emplace_back(Index{ 5,4,7 });
    cube.indexlist.emplace_back(Index{ 5,7,6 });
    cube.indexlist.emplace_back(Index{ 4,0,3 });
    cube.indexlist.emplace_back(Index{ 4,3,7 });
    // VBO 데이터를 업데이트하여 GPU에 다시 전송
    UpdateVBO(cube);
}


//삼각형 생성
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
    vertex.emplace_back(Vertex{ rnd_x, rnd_y + radius, 0.0f });
    vertex.emplace_back(Vertex{ rnd_x - radius * 0.85f, rnd_y - radius / 2.0f, 0.0f });
    vertex.emplace_back(Vertex{ rnd_x + radius * 0.85f, rnd_y - radius / 2.0f, 0.0f });
    init_color(3);
}

//사각형 만들기
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
    vertex.emplace_back(Vertex{ rnd_x + radius, rnd_y - radius, 0.0f });
    vertex.emplace_back(Vertex{ rnd_x - radius, rnd_y - radius, 0.0f });
    vertex.emplace_back(Vertex{ rnd_x + radius, rnd_y + radius, 0.0f });
    vertex.emplace_back(Vertex{ rnd_x - radius, rnd_y + radius, 0.0f });
    init_color(4);
}

// VAO 및 VBO 초기화
void InitBuffer() {
    glGenVertexArrays(1, &vao); // VAO 생성
    glGenBuffers(2, vbo); // 2개의 VBO 생성
    glGenBuffers(1, &EBO); //EBO 생성
    glBindVertexArray(vao); // VAO 바인딩
    // 정점 좌표 데이터가 존재하는지 확인
    if (!vertex.empty()) {
        // 정점 좌표 데이터를 VBO에 복사
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(Vertex), &vertex[0], GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x)); // x, y, z를 지정
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(Vertex), &vertex[0], GL_DYNAMIC_DRAW); // colors 데이터는 여전히 별도로 관리된다고 가정
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r)); // r, g, b를 지정
        glEnableVertexAttribArray(1);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Index), &index[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Index), (void*)offsetof(Index, v1));
    glEnableVertexAttribArray(0);
}

// VAO 및 VBO 초기화
void UpdateVBO() {
    glBindVertexArray(vao);  // VAO 바인딩 추가

    // 정점 데이터를 업데이트
    if (!vertex.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(Vertex), &vertex[0], GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x)); // x, y, z를 지정
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(Vertex), &vertex[0], GL_DYNAMIC_DRAW); // colors 데이터는 여전히 별도로 관리된다고 가정
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r)); // r, g, b를 지정
        glEnableVertexAttribArray(1);
    }

    glBindVertexArray(0);  // VAO 바인딩 해제
}
void UpdateVBO(Box box) {
    glBindVertexArray(box.VAO);  // VAO 바인딩 추가

    // 정점 데이터를 업데이트
    if (!box.vertex.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, box.VBO[0]);
        glBufferData(GL_ARRAY_BUFFER, box.vertex.size() * sizeof(Vertex), &box.vertex[0], GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x)); // x, y, z를 지정
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, box.VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, box.vertex.size() * sizeof(Vertex), &box.vertex[0], GL_DYNAMIC_DRAW); // colors 데이터는 여전히 별도로 관리된다고 가정
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r)); // r, g, b를 지정
        glEnableVertexAttribArray(1);
    }
    glBindVertexArray(0);  // VAO 바인딩 해제
}

// 셰이더 프로그램 생성 함수
GLuint make_shaderProgram() {
    GLuint shaderID = glCreateProgram();
    glAttachShader(shaderID, vertexShader);
    glAttachShader(shaderID, fragmentShader);
    glLinkProgram(shaderID);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderID;
}

// 버텍스 셰이더 생성 함수
void make_vertexShaders() {
    vertexSource = filetobuf("vertex.vs");
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
    glCompileShader(vertexShader);
}

// 프래그먼트 셰이더 생성 함수
void make_fragmentShaders() {
    fragmentSource = filetobuf("fragment.fs");
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
    glCompileShader(fragmentShader);
}

// 파일을 버퍼로 읽어오는 함수
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