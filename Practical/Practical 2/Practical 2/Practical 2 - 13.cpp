#define _CRT_SECURE_NO_WARNINGS
#include "3DGraphics.h"

//--------------------------- 전역 변수 ---------------------------------
float xpos, ypos{}; // 마우스 클릭 위치 좌표
vector<Vertex>vertex{};
vector<Index>indexlist{};
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
    Make_cube(0.0f, 0.0f, 0.0f, 0.2f);
    // VBO 데이터를 업데이트하여 GPU에 다시 전송
    
    // 화면을 다시 그리도록 요청
    glutPostRedisplay();
}

// 화면 그리기 콜백 함수
GLvoid drawScene() {
    // 배경색 설정
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramID); // 셰이더 프로그램 사용
    glm::mat4 transformMatrix = glm::mat4(1.0f);
    //--- 적용할 모델링 변환 행렬 만들기
    transformMatrix = glm::translate(transformMatrix, glm::vec3(0.0f, 0.0f, 0.0f)); //--- model 행렬에 이동 변환 적용
    //--- 세이더 프로그램에서 modelTransform 변수 위치 가져오기
    unsigned int transformLocation = glGetUniformLocation(shaderProgramID, "transform");
    //--- modelTransform 변수에 변환 값 적용하기
    glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));

    glBindVertexArray(vao); // VAO 바인딩
    glBindVertexArray(cube.VAO); // VAO 바인딩
    // 도형 그리기: 각 도형의 정점 개수에 따라 다르게 그려야 함
    int offset = 0;
    std::cout << cube.indexlist.size() << std::endl;
    glDrawElements(GL_TRIANGLES, cube.indexlist.size(), GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glutSwapBuffers(); // 화면에 출력
}

// 윈도우 리사이즈 콜백 함수
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}
// 색상 초기화 함수
void random_color(int shape) {
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

    // 버텍스 정보 전달
    cube.vertex.emplace_back(Vertex{ x, y, z, 0.7f, 0.0f, 0.7f });
    cube.vertex.emplace_back(Vertex{ x, y + size, z, 0.7f, 0.7f, 0.0f });
    cube.vertex.emplace_back(Vertex{ x + size, y + size, z, 0.0f, 0.0f, 0.7f });
    cube.vertex.emplace_back(Vertex{ x + size, y, z, 0.0f, 0.7f, 0.0f });

    cube.vertex.emplace_back(Vertex{ x, y, z + size, 0.7f, 0.0f, 0.0f });
    cube.vertex.emplace_back(Vertex{ x, y + size, z + size, 0.0f, 0.7f, 0.7f });
    cube.vertex.emplace_back(Vertex{ x + size, y + size, z, 0.7f, 0.0f, 0.7f });
    cube.vertex.emplace_back(Vertex{ x + size, y, z + size, 0.7f, 0.7f, 0.0f });

    // 인덱스 리스트 전달
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

    // 콘솔 로그로 정점 및 인덱스 리스트 출력
    std::cout << "Vertices:\n";
    for (const auto& v : cube.vertex) {
        std::cout << "Position: (" << v.x << ", " << v.y << ", " << v.z << "), Color: (" << v.r << ", " << v.g << ", " << v.b << ")\n";
    }

    std::cout << "Indices:\n";
    for (const auto& i : cube.indexlist) {
        std::cout << "Index: (" << i.v1 << ", " << i.v2 << ", " << i.v3 << ")\n";
    }

    // VBO 데이터를 업데이트하여 GPU에 다시 전송
    UpdateVBO(cube);
}






// VAO 및 VBO 초기화
void InitBuffer() {
    glGenVertexArrays(1, &vao); // VAO 생성
    glGenBuffers(2, vbo); // 2개의 VBO 생성
    glBindVertexArray(vao); // VAO 바인딩

    // 정점 좌표 데이터를 VBO에 복사
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

    // 인덱스 데이터를 EBO에 복사
    if (!indexlist.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexlist.size() * sizeof(Index), &indexlist[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Index), (void*)offsetof(Index, v1)); // 삭제
        glEnableVertexAttribArray(0); // 삭제
    }

    glBindVertexArray(0); // VAO 바인딩 해제
}

// VBO 업데이트
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

    // 인덱스 데이터를 EBO에 복사
    if (!indexlist.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexlist.size() * sizeof(Index), &indexlist[0], GL_STATIC_DRAW);
        // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Index), (void*)offsetof(Index, v1)); // 삭제
        // glEnableVertexAttribArray(0); // 삭제
    }

    glBindVertexArray(0); // VAO 바인딩 해제
}



void UpdateVBO(Box box) {
    glGenVertexArrays(1, &box.VAO); // VAO 생성
    glGenBuffers(2, box.VBO); // 2개의 VBO 생성
    glGenBuffers(1, &box.EBO); //EBO 생성
    glBindVertexArray(box.VAO);  // VAO 바인딩 추가

    // 정점 데이터를 업데이트
    if (!box.vertex.empty()) {
        std::cout << "큐브 버텍스정보 초기화\n";
        glBindBuffer(GL_ARRAY_BUFFER, box.VBO[0]);
        glBufferData(GL_ARRAY_BUFFER, box.vertex.size() * sizeof(Vertex), &box.vertex[0], GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x)); // x, y, z를 지정
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, box.VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, box.vertex.size() * sizeof(Vertex), &box.vertex[0], GL_DYNAMIC_DRAW); // colors 데이터는 여전히 별도로 관리된다고 가정
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r)); // r, g, b를 지정
        glEnableVertexAttribArray(1);
    }
    if (!box.indexlist.empty()) {
        std::cout << "큐브 인덱스리스트 초기화\n";
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, box.EBO);
       // glBufferData(GL_ELEMENT_ARRAY_BUFFER, box.indexlist.size() * sizeof(Index), &box.indexlist[0], GL_STATIC_DRAW);
       // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Index), (void*)offsetof(Index, v1));
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