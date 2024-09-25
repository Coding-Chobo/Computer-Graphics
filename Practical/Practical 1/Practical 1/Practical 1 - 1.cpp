#include <iostream>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <ctime>
#include <cstdlib> // rand() 함수를 사용하기 위해 추가

#define COL 600
#define ROW 800
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
void Timer(int value); // 타이머 콜백 함수

bool timerRunning = false; // 타이머 상태를 저장하는 변수

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 
{
    srand(time(NULL));
    //--- 윈도우 생성하기
    glutInit(&argc, argv); // glut 초기화
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
    glutInitWindowPosition(100, 100); // 윈도우의 위치 지정
    glutInitWindowSize(ROW, COL); // 윈도우의 크기 지정
    glutCreateWindow("Example1"); // 윈도우 생성

    //--- GLEW 초기화하기
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { // glew 초기화
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else {
        std::cout << "GLEW Initialized\n";
    }

    glutDisplayFunc(drawScene); // 출력 함수의 지정
    glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
    glutKeyboardFunc(Keyboard);
    glutMainLoop(); // 이벤트 처리 시작
}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수 
{
    glViewport(0, 0, w, h);
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수 
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 바탕색을 흰색으로 설정
    glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
    glutSwapBuffers(); // 화면에 출력하기
}

void Timer(int value) {
    if (timerRunning) { // 타이머가 실행 중인 경우에만 실행
        // 랜덤한 색상 생성
        float r = static_cast<float>(rand()) / RAND_MAX;
        float g = static_cast<float>(rand()) / RAND_MAX;
        float b = static_cast<float>(rand()) / RAND_MAX;
        glClearColor(r, g, b, 1.0f); // 랜덤 색 설정
        glClear(GL_COLOR_BUFFER_BIT);
        glutSwapBuffers(); // 화면 갱신

        // 500ms마다 타이머를 다시 호출 (0.5초)
        glutTimerFunc(500, Timer, 0);
    }
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'c':
        glClearColor(0.0f, 1.0f, 1.0f, 1.0f); // 청록색 (cyan)
        glClear(GL_COLOR_BUFFER_BIT); // 화면을 청록색으로 칠하기
        glutSwapBuffers(); // 버퍼 교체 (화면 갱신)
        break;
    case 'm':
        glClearColor(1.0f, 0.0f, 1.0f, 1.0f); // 자홍색 (magenta)
        glClear(GL_COLOR_BUFFER_BIT);
        glutSwapBuffers(); // 버퍼 교체 (화면 갱신)
        break;
    case 'y':
        glClearColor(1.0f, 1.0f, 0.0f, 1.0f); // 노랑색 (yellow)
        glClear(GL_COLOR_BUFFER_BIT);
        glutSwapBuffers(); // 버퍼 교체 (화면 갱신)
        break;
    case 'a':
    {
        // 랜덤한 색 생성
        float r = static_cast<float>(rand()) / RAND_MAX;
        float g = static_cast<float>(rand()) / RAND_MAX;
        float b = static_cast<float>(rand()) / RAND_MAX;
        glClearColor(r, g, b, 1.0f); // 랜덤 색 설정
        glClear(GL_COLOR_BUFFER_BIT);
        glutSwapBuffers(); // 버퍼 교체 (화면 갱신)
    }
    break;
    case 'w':
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 백색
        glClear(GL_COLOR_BUFFER_BIT);
        glutSwapBuffers(); // 버퍼 교체 (화면 갱신)
        break;
    case 'k':
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // 검정색
        glClear(GL_COLOR_BUFFER_BIT);
        glutSwapBuffers(); // 버퍼 교체 (화면 갱신)
        break;
    case 't':
        if (!timerRunning) {
            timerRunning = true; // 타이머 시작
            glutTimerFunc(500, Timer, 0); // 500ms마다 Timer 함수 호출
        }
        break;
    case 's':
        timerRunning = false; // 타이머 종료
        break;
    case 'q':
        exit(0); // 프로그램 종료
        break;
    }
}
