#define _CRT_SECURE_NO_WARNINGS
#include "3DGraphics.h"

#define WIDTH 1200
#define HEIGHT 900
#define PI 3.1415926535897932384620588419716939
#define MAX_LINE_LENGTH 1024
GLchar* vertexSource, * fragmentSource; // 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //세이더 객체
GLuint shaderProgramID; // 세이더 프로그램

Object cube{}, tetra{};
Object objfile{};
Coordinate Coordinate_system{};
unsigned int shape{ 0 };
GLfloat rotate_angle_x;
GLfloat rotate_angle_y;
bool mode_w{ false };
unsigned int mode_animate_x{ 0 };
unsigned int mode_animate_y{ 0 };
void mainLoop() {
	glutPostRedisplay(); // 다시 그리기 요청
}
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
	// 은면제거
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	
	InitBuffer();
	CreateShaderProgram();
	//도형 정보 초기화
	init_figure();
	read_obj_file("Daven.obj", objfile);
	//리콜 함수
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);
	glutDisplayFunc(Render);
	glutIdleFunc(mainLoop); // 메인 루프
	glutMainLoop(); //이벤트 루프 진입
}
//그리는 함수
GLvoid Render()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	init_Matrix();
	// 좌표계 그리기 (회전이나 변환 적용되지 않음)
	Draw_Coordinate(Coordinate_system);

	// 변환행렬 생성 및 적용
	Make_Matrix();
	std::cout << objfile.vertex.size() << std::endl;
	std::cout << objfile.indexlist.size() << std::endl;
	UpdateVBO(objfile);
	glDrawElements(GL_TRIANGLES, 3 * objfile.indexlist.size(), GL_UNSIGNED_INT, 0);
	// 도형 그리기
	if (shape == 1)
	{
		UpdateVBO(cube);
		if (!mode_w)
		{
			glDrawElements(GL_TRIANGLES, 3 * cube.indexlist.size(), GL_UNSIGNED_INT, 0);
		}
		else
		{
			glDrawElements(GL_LINE_LOOP, 3 * cube.indexlist.size(), GL_UNSIGNED_INT, 0);
		}
	}
	else if (shape == 2)
	{
		UpdateVBO(tetra);
		if (mode_w)
		{
			glDrawElements(GL_LINE_LOOP, 3 * tetra.indexlist.size(), GL_UNSIGNED_INT, 0);
		}
		else
		{
			glDrawElements(GL_TRIANGLES, 3 * tetra.indexlist.size(), GL_UNSIGNED_INT, 0);
		}
	}

	glutSwapBuffers();
}


GLvoid InitBuffer()
{
	glGenVertexArrays(1, &cube.vao);
	glGenBuffers(2, cube.vbo);
	glGenBuffers(1, &cube.EBO);

	glGenVertexArrays(1, &objfile.vao);
	glGenBuffers(2, objfile.vbo);
	glGenBuffers(1, &objfile.EBO);

	glGenVertexArrays(1, &tetra.vao);
	glGenBuffers(2, tetra.vbo);
	glGenBuffers(1, &tetra.EBO);

	glGenVertexArrays(1, &Coordinate_system.vao);
	glGenBuffers(2, Coordinate_system.vbo);
	glGenBuffers(1, &Coordinate_system.EBO);
}
GLvoid init_Matrix() {
	// 좌표계 그리기 전에 변환 초기화
	glm::mat4 identityMatrix = glm::mat4(1.0f);
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "trans");
	identityMatrix = glm::rotate(identityMatrix, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	identityMatrix = glm::rotate(identityMatrix, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(identityMatrix));
}
GLvoid Make_Matrix() {
	float rotate_gap = PI / 90;
	switch (mode_animate_x)
	{
	case 1://x축 양의 방향
		rotate_angle_x += rotate_gap;
		break;
	case 2://x축 음의 방향
		rotate_angle_x -= rotate_gap;
		break;
	default:
		break;
	}
	switch (mode_animate_y)
	{
	case 3://y축 양의 방향
		rotate_angle_y += rotate_gap;
		break;
	case 4://y축 음의 방향
		rotate_angle_y -= rotate_gap;
		break;
	default:
		break;
	}
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
	rot = glm::rotate(rot, glm::radians(rotate_angle_x), glm::vec3(1.0f, 0.0f, 0.0f));
	rot = glm::rotate(rot, glm::radians(rotate_angle_y), glm::vec3(0.0f, 1.0f, 0.0f));


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
	obj.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 1.0f });  // Y축: 파랑
	obj.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 1.0f });  // Y축: 파랑
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 0.0f });  // Z축: 초록
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 0.0f });  // Z축: 초록


	// 인덱스 추가
	obj.indexlist.emplace_back(0);  // X축
	obj.indexlist.emplace_back(1);

	obj.indexlist.emplace_back(2);  // Y축
	obj.indexlist.emplace_back(3);

	obj.indexlist.emplace_back(4);  // Z축
	obj.indexlist.emplace_back(5);

	// VBO 업데이트
	UpdateVBO(obj);

	// 인덱스의 개수는 좌표계의 라인이 3개이므로 총 6개
	glDrawElements(GL_LINES, obj.indexlist.size(), GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}
void Timer(int value) {
	
}

void Mouse(int button, int state, int x, int y) {

}
void Motion(int x, int y) {

}
void SpecialKeyboard(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		if (shape == 1)
		{
			for (int i = 0; i < cube.vertex.size(); i++)
			{
				cube.vertex.at(i).x -= 0.05f;
			}
		}
		else
		{
			for (int i = 0; i < tetra.vertex.size(); i++)
			{
				tetra.vertex.at(i).x -= 0.05f;
			}
		}
		break;
	case GLUT_KEY_RIGHT:
		if (shape == 1)
		{
			for (int i = 0; i < cube.vertex.size(); i++)
			{
				cube.vertex.at(i).x += 0.05f;
			}
		}
		else
		{
			for (int i = 0; i < tetra.vertex.size(); i++)
			{
				tetra.vertex.at(i).x += 0.05f;
			}
		}
		break;
	case GLUT_KEY_UP:
		if (shape == 1)
		{
			for (int i = 0; i < cube.vertex.size(); i++)
			{
				cube.vertex.at(i).y += 0.05f;
			}
		}
		else
		{
			for (int i = 0; i < tetra.vertex.size(); i++)
			{
				tetra.vertex.at(i).y += 0.05f;
			}
		}
		break;
	case GLUT_KEY_DOWN:
		if (shape == 1)
		{
			for (int i = 0; i < cube.vertex.size(); i++)
			{
				cube.vertex.at(i).y -= 0.05f;
			}
		}
		else
		{
			for (int i = 0; i < tetra.vertex.size(); i++)
			{
				tetra.vertex.at(i).y -= 0.05f;
			}
		}
		break;
	default:
		break;
	}
}
GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'c':
		shape = 1;
		break;
	case 't':
		shape = 2;
		break;
	case 'h':
		glEnable(GL_CULL_FACE);
		break;
	case 'H':
		glDisable(GL_CULL_FACE);
		break;
	case 'w':
		mode_w = true;
		break;
	case 'W':
		mode_w = false;
	case 'x':
		mode_animate_x = 1;
		break;
	case 'X':
		mode_animate_x = 2;
		break;
	case 'y':
		mode_animate_y = 3;
		break;
	case 'Y':
		mode_animate_y = 4;
		break;
	case 's':
		init_figure();
		mode_w = { false };
		rotate_angle_x = 0.0f;
		rotate_angle_y = 0.0f;
		mode_animate_x = { 0 };
		mode_animate_y = { 0 };
		break;
	default:
		break;
	}
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
void read_obj_file(const char* filename, Object& model) {
	FILE* file;
	fopen_s(&file, filename, "r");
	if (!file) {
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}

	char line[MAX_LINE_LENGTH];
	std::vector<glm::vec3> temp_vertices;
	std::vector<Index> temp_indices;

	while (fgets(line, sizeof(line), file)) {
		read_newline(line); // 개행문자 제거

		// 정점(Vertex) 데이터 처리
		if (line[0] == 'v' && line[1] == ' ') {
			glm::vec3 vertex;
			sscanf_s(line + 2, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}

		// 면(Face) 데이터 처리
		else if (line[0] == 'f' && line[1] == ' ') {
			unsigned int v1, v2, v3;
			sscanf_s(line + 2, "%u %u %u", &v1, &v2, &v3);

			// OBJ 인덱스는 1부터 시작하기 때문에 0부터 시작하도록 조정
			Index face = { v1 - 1, v2 - 1, v3 - 1 };
			temp_indices.push_back(face);
		}
	}

	fclose(file);

	// Object 구조체에 정점 및 인덱스 데이터를 전달
	model.vertex = temp_vertices; // 정점 데이터를 그대로 저장
	model.indexlist = temp_indices; // 인덱스 데이터를 그대로 저장

	// 색상은 임의로 설정 (추후 수정 가능)
	for (size_t i = 0; i < model.vertex.size(); i++) {
		model.color.push_back(glm::vec3(1.0f, 0.0f, 0.0f)); // 빨강색으로 설정
	}
}


void init_figure() {

	//---------------------------정사면체-----------------------------
	tetra.vertex.clear();
	tetra.color.clear();
	tetra.indexlist.clear();
	float size = 0.5f, x = 0.0f, y = 0.0f, z = 0.0f;
	float a = 0.85 * size;
	// 정점 추가
	tetra.vertex.emplace_back(glm::vec3{ x, y + size, z });
	tetra.vertex.emplace_back(glm::vec3{ x, y - size / 2, z + a });
	tetra.vertex.emplace_back(glm::vec3{ x + 0.85 * a, y - size / 2, z - a / 2 });
	tetra.vertex.emplace_back(glm::vec3{ x - 0.85 * a, y - size / 2, z - a / 2 });

	// 색상 추가
	tetra.color.emplace_back(glm::vec3{ 1.0f, 1.0f, 0.0f });
	tetra.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 1.0f });
	tetra.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 1.0f });
	tetra.color.emplace_back(glm::vec3{ 0.5f, 0.5f, 0.5f });

	//인덱스 리스트 추가
	tetra.indexlist.emplace_back(Index{ 0, 1, 2 });
	tetra.indexlist.emplace_back(Index{ 0, 2, 3 });
	tetra.indexlist.emplace_back(Index{ 0, 3, 1 });
	tetra.indexlist.emplace_back(Index{ 1, 2, 3 });

	//---------------------------정육면체-----------------------------
	cube.vertex.clear();
	cube.color.clear();
	cube.indexlist.clear();

	// 정점 추가
	cube.vertex.emplace_back(glm::vec3{ x - size, y + size, z - size });
	cube.vertex.emplace_back(glm::vec3{ x - size, y + size, z + size });
	cube.vertex.emplace_back(glm::vec3{ x + size, y + size, z + size });
	cube.vertex.emplace_back(glm::vec3{ x + size, y + size, z - size });

	cube.vertex.emplace_back(glm::vec3{ x - size, y - size, z - size });
	cube.vertex.emplace_back(glm::vec3{ x - size, y - size, z + size });
	cube.vertex.emplace_back(glm::vec3{ x + size, y - size, z + size });
	cube.vertex.emplace_back(glm::vec3{ x + size, y - size, z - size });

	// 색상 추가
	cube.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 0.0f });
	cube.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 1.0f });
	cube.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 1.0f });
	cube.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 1.0f });

	cube.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 0.0f });
	cube.color.emplace_back(glm::vec3{ 1.0f, 1.0f, 0.0f });
	cube.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 0.0f });
	cube.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 1.0f });

	// 인덱스 추가
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

	// VBO 업데이트
	UpdateVBO(tetra);
	UpdateVBO(cube);
}


