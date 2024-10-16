#define _CRT_SECURE_NO_WARNINGS
#include "3DGraphics.h"

#define WIDTH 1200
#define HEIGHT 900

GLuint VAO, VBO[2], EBO;
GLchar* vertexSource, * fragmentSource; // 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //세이더 객체
GLuint shaderProgramID; // 세이더 프로그램

Object cube{};
Coordinate Coordinate_system{};




void main(int argc, char** argv)
{
	// 윈도우 생성
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("OpenGL Practical");

	// GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	InitBuffer();

	CreateShaderProgram();

	glutDisplayFunc(Render);

	glutMainLoop();
}
//그리는 함수
GLvoid Render()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	//변환행렬 생성
	Make_Matrix();
	//좌표계 그리기
	Draw_Coordinate(Coordinate_system);
	//큐브그리기
	DrawCube(0.0f, 0.0f, 0.0f, 0.5f, cube);

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
	glGenVertexArrays(1, &Coordinate_system.vao);
	glGenBuffers(2, Coordinate_system.vbo);
	glGenBuffers(1, &Coordinate_system.EBO);
}

GLvoid Make_Matrix() {
	// 모델 location 인덱스 저장
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "trans");

	// 모델 및 변환 변수 초기화
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 scale = glm::mat4(1.0f);
	glm::mat4 rot = glm::mat4(1.0f);
	glm::mat4 move = glm::mat4(1.0f);

	// 변환
	scale = glm::scale(scale, glm::vec3(0.5f, 0.6f, 0.5f));
	move = glm::translate(move, glm::vec3(0.0f, 0.0f, 0.0f));
	rot = glm::rotate(rot, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	rot = glm::rotate(rot, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// 변환 과정 처리
	model = model * scale * move * rot;

	// 모델 변환 적용
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
}

void Draw_Coordinate(Coordinate obj) {
	obj.vertex.clear();
	obj.color.clear();
	obj.indexlist.clear();

	// 정점 추가
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });  // 원점
	obj.vertex.emplace_back(glm::vec3{ 5.0f, 0.0f, 0.0f });  // X축
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });  // 원점
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 5.0f, 0.0f });  // Y축
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });  // 원점
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 0.0f, 5.0f });  // Z축

	// 색상 추가 (각 축의 색상 설정)
	obj.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 0.0f });  // X축: 빨강
	obj.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 0.0f });  // X축: 빨강
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 0.0f });  // Y축: 초록
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 0.0f });  // Y축: 초록
	obj.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 1.0f });  // Z축: 파랑
	obj.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 1.0f });  // Z축: 파랑

	// 인덱스 추가
	obj.indexlist.emplace_back(0);  // X축
	obj.indexlist.emplace_back(1);

	obj.indexlist.emplace_back(2);  // Y축
	obj.indexlist.emplace_back(3);

	obj.indexlist.emplace_back(4);  // Z축
	obj.indexlist.emplace_back();

	// VBO 업데이트
	UpdateVBO(obj);

	// 인덱스의 개수는 좌표계의 라인이 3개이므로 총 6개
	glDrawElements(GL_LINES, obj.indexlist.size(), GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void DrawCube(float x, float y, float z, float size, Object obj) {
	obj.vertex.clear();
	obj.color.clear();
	obj.indexlist.clear();

	// 정점 추가
	obj.vertex.emplace_back(glm::vec3{ x - size, y + size, z - size });
	obj.vertex.emplace_back(glm::vec3{ x - size, y + size, z + size });
	obj.vertex.emplace_back(glm::vec3{ x + size, y + size, z + size });
	obj.vertex.emplace_back(glm::vec3{ x + size, y + size, z - size });

	obj.vertex.emplace_back(glm::vec3{ x - size, y - size, z - size });
	obj.vertex.emplace_back(glm::vec3{ x - size, y - size, z + size });
	obj.vertex.emplace_back(glm::vec3{ x + size, y - size, z + size });
	obj.vertex.emplace_back(glm::vec3{ x + size, y - size, z - size });

	// 색상 추가
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 1.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 1.0f });
	obj.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 1.0f });

	obj.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 0.0f });
	obj.color.emplace_back(glm::vec3{ 1.0f, 1.0f, 0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 1.0f });

	// 인덱스 추가
	obj.indexlist.emplace_back(Index{ 0, 1, 2 });
	obj.indexlist.emplace_back(Index{ 0, 2, 3 });
	obj.indexlist.emplace_back(Index{ 1, 5, 6 });
	obj.indexlist.emplace_back(Index{ 1, 6, 2 });
	obj.indexlist.emplace_back(Index{ 2, 6, 7 });
	obj.indexlist.emplace_back(Index{ 2, 7, 3 });
	obj.indexlist.emplace_back(Index{ 0, 4, 5 });
	obj.indexlist.emplace_back(Index{ 0, 5, 1 });
	obj.indexlist.emplace_back(Index{ 5, 4, 6 });
	obj.indexlist.emplace_back(Index{ 4, 7, 6 });
	obj.indexlist.emplace_back(Index{ 0, 7, 4 });
	obj.indexlist.emplace_back(Index{ 0, 3, 7 });

	// VBO 업데이트
	UpdateVBO(obj);

	std::cout << "Index count: " << obj.indexlist.size() << std::endl;

	glDrawElements(GL_TRIANGLES, obj.indexlist.size() * 3, GL_UNSIGNED_INT, 0);

	// 버퍼 해제
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

GLvoid UpdateVBO(Object object) {
	// VAO 바인드
	glBindVertexArray(object.vao);

	// 큐브 위치 버퍼 바인드
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, object.vertex.size() * sizeof(glm::vec3), object.vertex.data(), GL_DYNAMIC_DRAW);

	// 큐브 인덱스 버퍼 바인드
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.EBO);
	// 전체 인덱스 배열의 크기 전달
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, object.indexlist.size() * sizeof(Index), object.indexlist.data(), GL_DYNAMIC_DRAW);

	// 위치 속성 설정
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(0);

	// 큐브 색상 버퍼 바인드
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, object.color.size() * sizeof(glm::vec3), object.color.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(1);
}

GLvoid UpdateVBO(Coordinate object) {
	// VAO 바인드
	glBindVertexArray(object.vao);

	// 좌표계 정점 버퍼 바인드
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, object.vertex.size() * sizeof(glm::vec3), object.vertex.data(), GL_DYNAMIC_DRAW);

	// 좌표계 인덱스 버퍼 바인드
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, object.indexlist.size() * sizeof(unsigned int), object.indexlist.data(), GL_DYNAMIC_DRAW);

	// 위치 속성 설정
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(0);

	// 좌표계 색상 버퍼 바인드
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, object.color.size() * sizeof(glm::vec3), object.color.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(1);
}

GLvoid Reshape(int w, int h)
{
	// 뷰포트 기본 WIDTH HEIGHT로 설정
	glViewport(0, 0, w, h);
}

char* GetBuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;

	// 파일 읽기 형식으로 열기
	fopen_s(&fptr, file, "rb");

	// 예외처리
	if (!fptr) return NULL;

	// 파일 buf로 변환
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);

	// 파일 종료
	fclose(fptr);

	buf[length] = 0;
	return buf;
}

GLint CreateShader(const char* file, int type)
{
	// glsl 파일 읽기
	GLchar* source = GetBuf(file);

	// 객체 생성
	GLint shader = glCreateShader(type);
	glShaderSource(shader, 1, (const GLchar**)&source, 0);
	glCompileShader(shader);

	// 컴파일 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

	if (!result)
	{
		glGetShaderInfoLog(shader, 512, NULL, errorLog);
		std::cerr << "ERROR: 컴파일 실패\n" << errorLog << std::endl;
		return 0;
	}

	return shader;
}

void CreateShaderProgram()
{
	vertexShader = CreateShader("vertex.vs", GL_VERTEX_SHADER);
	fragmentShader = CreateShader("fragment.fs", GL_FRAGMENT_SHADER);

	// 세이더 프로그램 생성
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);

	// 세이더 삭제
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// 세이더 프로그램 사용
	glUseProgram(shaderProgramID);
}
