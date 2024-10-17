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
int selectedBlock{-1};
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
    std::cout << "도형의 갯수 : "<< info.size() << std::endl;
    // 도형 그리기: 각 도형의 정점 개수에 따라 다르게 그려야 함
    int offset = 0;
    for (int i = 0; i < info.size(); i++)
    {
        switch (info.at(i).shape)
        {
        case 1://점 그리기(작은 삼각형)
            if (info.at(i).is_active) {
                glDrawArrays(GL_TRIANGLES, offset, 3); // 삼각형: 3개의 정점
            }
            offset += 3;
            break;
        case 2: //선 그리기
            if (info.at(i).is_active) {
                glDrawArrays(GL_LINES, offset, 2); // 삼각형: 3개의 정점
            }
            offset += 2;
            break;
        case 3:// 삼각형 그리기
            if (info.at(i).is_active) {
                glDrawArrays(GL_TRIANGLES, offset, 3); // 삼각형: 3개의 정점
            }
            offset += 3;
            break;
        case 4: // 사각형 그리기
            if (info.at(i).is_active) {
                for (int j = 0; j < 2; j++)
                {
                    glDrawArrays(GL_TRIANGLES, offset + j, 3); // 삼각형: 3개의 정점 
                }
            }
            offset += 4;
            break;
        case 5: // 오각형 그리기
            if (info.at(i).is_active) {
                for (int j = 0; j < 3; j++)
                {
                    glDrawArrays(GL_TRIANGLES, offset + j, 3); // 삼각형: 3개의 정점
                }
            }
            offset += 5;
            break;
        default:
            break;
        }
         
    }

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
    for (int i = 0; i < shape; i++)
    {
        colors.emplace_back(Color{ r + static_cast<float>(rand()) / RAND_MAX / 5,g + static_cast<float>(rand()) / RAND_MAX / 5,b + static_cast<float>(rand()) / RAND_MAX / 5 });
    }
}

//타이머 함수
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
                        case 0: // 좌
                            vertex.at(j + start_index).x -= moving_gap;
                            if (vertex.at(j + start_index).x < -1.0f)
                            {
                                change_dir[0] = true;
                            }
                            break;
                        case 1: // 우
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
                        case 0: // 하
                            vertex.at(j + start_index).y -= moving_gap;
                            if (vertex.at(j + start_index).y < -1.0f)
                            {
                                change_dir[1] = true;
                            }
                            break;
                        case 1: // 상
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
                        case 0: // 좌
                            vertex.at(j + start_index).x -= moving_gap;
                            if (vertex.at(j + start_index).x < -1.0f)
                            {
                                change_dir[0] = true;
                            }
                            break;
                        case 1: // 우
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
                        case 0: // 하
                            vertex.at(j + start_index).y -= moving_gap;
                            if (vertex.at(j + start_index).y < -1.0f)
                            {
                                change_dir[1] = true;
                            }
                            break;
                        case 1: // 상
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
            case 0: // 좌
                info.at(i).checkbox.left -= moving_gap;
                info.at(i).checkbox.right -= moving_gap;
                break;
            case 1: // 우
                info.at(i).checkbox.left += moving_gap;
                info.at(i).checkbox.right += moving_gap;
                break;
            }
            switch (info.at(i).dir2)
            {
            case 0: // 하
                info.at(i).checkbox.top -= moving_gap;
                info.at(i).checkbox.bottom -= moving_gap;
                break;
            case 1: // 상
                info.at(i).checkbox.top += moving_gap;
                info.at(i).checkbox.bottom += moving_gap;
                break;
            }
            
        }
    }


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
        std::cout << "Info의 갯수" << info.size() << std::endl;
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
    // VBO 데이터를 업데이트하여 GPU에 다시 전송
    UpdateVBO();

    // 화면을 다시 그리도록 요청
    glutPostRedisplay();
}

//마우스 모션검사(클릭중Active)
void Motion(int x, int y) {
    if (selectedBlock != -1) {  // 선택된 블록이 있을 때만 동작
        // OpenGL 좌표로 변환
        float xpos = static_cast<float>(x) / (ROW / 2.0f) - 1.0f;
        float ypos = 1.0f - static_cast<float>(y) / (COL / 2.0f);

        // 마우스 이동량 계산
        float deltaX = xpos - previousMouseX;
        float deltaY = ypos - previousMouseY;

        // 선택된 도형의 checkbox 좌표를 업데이트
        info.at(selectedBlock).checkbox.left += deltaX;
        info.at(selectedBlock).checkbox.right += deltaX;
        info.at(selectedBlock).checkbox.top += deltaY;
        info.at(selectedBlock).checkbox.bottom += deltaY;

        // 선택된 도형의 vertex 좌표를 업데이트
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

        // VBO 데이터를 업데이트하여 GPU에 다시 전송
        UpdateVBO();

        // 화면을 다시 그리도록 요청
        glutPostRedisplay();
    }
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

//사각형 충돌검사
bool IsRectOverlap(BoundingBox rect1, BoundingBox rect2) {
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
    info.emplace_back(Figure{ 4, true, bbox,rand() % 2 ,rand() % 2 });
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