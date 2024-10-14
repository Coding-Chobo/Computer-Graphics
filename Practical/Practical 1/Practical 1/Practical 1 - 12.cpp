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
int selectedBlock{};
vector<Vertex>vertex{};
vector<Color>colors{};
vector<Figure> info{};
float previousMouseX{}, previousMouseY{};
GLchar* vertexSource{}, * fragmentSource{}; // 소스코드 저장 변수
GLuint vertexShader{}, fragmentShader{}; // 세이더 객체
GLuint shaderProgramID{}; // 셰이더 프로그램
GLuint vao{}, vbo[2]{}; // VAO와 VBO



// 프로그램의 메인 함수
void main(int argc, char** argv) {
    srand(time(NULL));
    // 윈도우 생성
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
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
    Make_triangle(1);
    Make_triangle(1);
    Make_triangle(1);
    Make_Line(0);
    Make_Line(0);
    Make_Line(0);
    Make_triangle(0);
    Make_triangle(0);
    Make_triangle(0);
    Make_Rect(0);
    Make_Rect(0);
    Make_Rect(0);
    Make_Penta(0);
    Make_Penta(0);
    Make_Penta(0);

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
    std::cout << "도형을 그리고 있습니다." << std::endl;
    // 도형 그리기: 각 도형의 정점 개수에 따라 다르게 그려야 함
    int offset = 0;
    for (int i = 0; i < Max; i++)
    {
        if (info.at(i).is_active) {
            switch (info.at(i).shape-1)
            {
            case 0://점 그리기(작은 삼각형)
                glDrawArrays(GL_TRIANGLES, offset, 3); // 삼각형: 3개의 정점
                offset += 3;
                break;
            case 1: //선 그리기
                glDrawArrays(GL_LINES, offset, 2); // 삼각형: 3개의 정점
                offset += 2;
                break;
            case 2:// 삼각형 그리기
                glDrawArrays(GL_TRIANGLES, offset, 3); // 삼각형: 3개의 정점
                offset += 3;
                break;
            case 3: // 사각형 그리기
                for (int j = 0; j < 2; j++)
                {
                    glDrawArrays(GL_TRIANGLES, offset + j, 3); // 삼각형: 3개의 정점   
                }
                offset += 4;
                break;
            case 4: // 오각형 그리기
                for (int j = 0; j < 3; j++)
                {
                    glDrawArrays(GL_TRIANGLES, offset + j, 3); // 삼각형: 3개의 정점
                }
                offset += 5;
                break;
            default:
                break;
            }
         }
    }

    std::cout << "도형의 버텍스의 갯수" << colors.size() << std::endl;
    glutSwapBuffers(); // 화면에 출력
}

// 윈도우 리사이즈 콜백 함수
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

//타이머 함수
void Timer(int value) {
    float moving_gap = 0.05f;
    float spiral_gap = 0.01f;

    // VBO 데이터를 업데이트하여 GPU에 다시 전송
    UpdateVBO();

    // 화면을 다시 그리도록 요청
    glutPostRedisplay();
    glutTimerFunc(100, Timer, value);
}

// 색상 초기화 함수
void init_color(int shape) {
    float r = static_cast<float>(rand()) / RAND_MAX;
    float g = static_cast<float>(rand()) / RAND_MAX;
    float b = static_cast<float>(rand()) / RAND_MAX;
    for (int i = 0; i < shape; i++)
    {
        colors.emplace_back(Color{ r + static_cast<float>(rand()) / RAND_MAX / 5,g + static_cast<float>(rand()) / RAND_MAX / 5,b + static_cast<float>(rand()) / RAND_MAX / 5 });
    }
}

//마우스 클릭 이벤트 콜백 함수
void Mouse(int button, int state, int x, int y) {
    // OpenGL 좌표로 변환 (화면의 크기를 고려)
    float xpos = (static_cast<float>(x) / (ROW / 2.0f)) - 1.0f;
    float ypos = 1.0f - (static_cast<float>(y) / (COL / 2.0f));

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        for (int i = Max - 1; i >= 0; i--) {
            if ((xpos > info.at(i).checkbox.left) && (xpos < info.at(i).checkbox.right) && (ypos > info.at(i).checkbox.bottom) && (ypos < info.at(i).checkbox.top)) {
                selectedBlock = i;
                previousMouseX = xpos;
                previousMouseY = ypos;
            }
        }
    }
    // VBO 데이터를 업데이트하여 GPU에 다시 전송
    UpdateVBO();

    // 화면을 다시 그리도록 요청
    glutPostRedisplay();
}
void Motion(int x, int y) {
    std::cout << selectedBlock << std::endl;
    if (selectedBlock < Max) { // 선택된 블록이 있을 때만 동작
        // OpenGL 좌표로 변환
        float xpos = static_cast<float>(x) / (ROW / 2.0f) - 1.0f;
        float ypos = 1.0f - static_cast<float>(y) / (COL / 2.0f);

        // 마우스 이동량 계산
        float deltaX = xpos - previousMouseX;
        float deltaY = ypos - previousMouseY;
        info.at(selectedBlock).checkbox.left += deltaX;
        info.at(selectedBlock).checkbox.right += deltaX;
        info.at(selectedBlock).checkbox.top += deltaY;
        info.at(selectedBlock).checkbox.bottom += deltaY;
        int offset{};
        for (int i = 0; i < Max; i++)
        {
            if (info.at(i).is_active)
            {
                if (info.at(i).shape == 1)
                {
                    offset += 3;
                }
                else
                {
                    offset += info.at(i).shape;
                }
            }
        }
        
        for (int i = 0; i < Max; i++)
        {
            if (info.at(i).shape == 1)
            {
                vertex.at(offset).x += deltaX;
                vertex.at(offset).y += deltaY;
                vertex.at(offset+1).x += deltaX;
                vertex.at(offset+1).y += deltaY;
                vertex.at(offset+2).x += deltaX;
                vertex.at(offset+2).y += deltaY;
            }
            else
            {
                for (int j = 0; j < info.at(i).shape; j++)
                {
                    vertex.at(offset + j).x += deltaX;
                    vertex.at(offset + j).y += deltaY;
                }
            }
        }
        vertex.at(offset).x += deltaX;
        vertex.at(offset).y += deltaY;
    }
    previousMouseX = xpos;
    previousMouseY = ypos;
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
bool IsRectOverlap(RECT rect1, RECT rect2) {
    if (rect1.right <= rect2.left || rect1.left >= rect2.right) {
        return false;
    }
    if (rect1.top <= rect2.bottom || rect1.bottom >= rect2.top) {
        return false;
    }
    // 두 축 모두에서 겹친다면 충돌이 발생한 것
    return true;
}
//선분 생성
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
    info.emplace_back(Figure{ 2, true,{ static_cast<LONG>(rnd_x - radius / 3), static_cast<LONG>(rnd_y + radius / 4), static_cast<LONG>(rnd_x + radius / 3), static_cast<LONG>(rnd_y - radius / 3) } });
    vertex.emplace_back(Vertex{ rnd_x - radius,rnd_y + radius,0.0f });
    vertex.emplace_back(Vertex{ rnd_x + radius,rnd_y - radius,0.0f });
    init_color(2);
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
        info.emplace_back(Figure{ 3, true,{ static_cast<LONG>(rnd_x - radius / 4), static_cast<LONG>(rnd_y + radius / 4), static_cast<LONG>(rnd_x + radius / 4), static_cast<LONG>(rnd_y - radius / 4) } });
    }
    else if (size == 1) // point size
    {
        radius = 0.03f;
        info.emplace_back(Figure{ 1, true,{ static_cast<LONG>(rnd_x - radius / 2), static_cast<LONG>(rnd_y + radius / 2), static_cast<LONG>(rnd_x + radius / 2), static_cast<LONG>(rnd_y - radius / 4) } });
    }
    else if (size == 2) // Middle size
    {
        radius = 0.15f;
    }
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
    info.emplace_back(Figure{ 4, true,{ static_cast<LONG>(rnd_x - radius / 4), static_cast<LONG>(rnd_y + radius / 4), static_cast<LONG>(rnd_x + radius / 4), static_cast<LONG>(rnd_y - radius / 4) } });
    vertex.emplace_back(Vertex{ rnd_x + radius, rnd_y - radius, 0.0f });
    vertex.emplace_back(Vertex{ rnd_x - radius, rnd_y - radius, 0.0f });
    vertex.emplace_back(Vertex{ rnd_x + radius, rnd_y + radius, 0.0f });
    vertex.emplace_back(Vertex{ rnd_x - radius, rnd_y + radius, 0.0f });
    init_color(4);
}
//오각형 만들기
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
    info.emplace_back(Figure{ 5, true,{ static_cast<LONG>(rnd_x - radius / 4), static_cast<LONG>(rnd_y + radius / 4), static_cast<LONG>(rnd_x + radius / 4), static_cast<LONG>(rnd_y - radius / 4) } });
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





// VAO 및 VBO 초기화
void InitBuffer() {
    glGenVertexArrays(1, &vao); // VAO 생성
    glBindVertexArray(vao); // VAO 바인딩

    glGenBuffers(2, vbo); // 2개의 VBO 생성

    // 정점 좌표 데이터가 존재하는지 확인
    if (!vertex.empty()) {
        // 정점 좌표 데이터를 VBO에 복사
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(Vertex), &vertex[0], GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x)); // x, y, z를 지정
        glEnableVertexAttribArray(0);
    }
    // 색상 데이터를 VBO에 복사 (colors 배열이 있다고 가정)
    if (!colors.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(Color), &colors[0], GL_DYNAMIC_DRAW); // colors 데이터는 여전히 별도로 관리된다고 가정
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);
    }
}

// VAO 및 VBO 초기화
void UpdateVBO() {
    glBindVertexArray(vao);  // VAO 바인딩 추가

    // 정점 데이터를 업데이트
    if (!vertex.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(Vertex), &vertex[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
        glEnableVertexAttribArray(0);
    }

    // 색상 데이터를 업데이트
    if (!colors.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(Color), &colors[0], GL_DYNAMIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Color), (void*)offsetof(Color, r));
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