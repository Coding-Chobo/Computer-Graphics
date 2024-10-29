	#define _CRT_SECURE_NO_WARNINGS
	#include "CG_OPGL.h"

	#define WIDTH 1200
	#define HEIGHT 900
	#define PI 3.1415926535897932384620588419716939
	#define MAX_LINE_LENGTH 1024
	GLchar* vertexSource, * fragmentSource; // 소스코드 저장 변수
	GLuint vertexShader, fragmentShader; //세이더 객체
	GLuint shaderProgramID; // 세이더 프로그램

	//그려질 오브젝트 선언
	vector<Object>figure;
	Object objfile{};
	Coordinate Coordinate_system{};
	Camera_Info camera{};

	GLuint vao;
	GLuint vbo[2];
	GLuint EBO;

	//실습에 필요한 변수들
	float theta{};
	float speed{ 3.0f };
	GLfloat orbit_angle_y{ 0.0f };  // 공전 각도
	unsigned int time_for_craft { 0 };
	//-----------------------------------------------------------------------
	void main(int argc, char** argv)
	{
		srand(time(NULL));
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
		//glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		InitBuffer();
		CreateShaderProgram();
		//도형 정보 초기화
		init_figure();

		//리콜 함수
		glutReshapeFunc(Reshape);
		glutMouseFunc(Mouse);
		glutMotionFunc(Motion);
		glutKeyboardFunc(Keyboard);
		glutSpecialFunc(SpecialKeyboard);
		glutDisplayFunc(Render);
		glutTimerFunc(50, Timer, 0);
		glutMainLoop(); //이벤트 루프 진입
	}
	//그리는 함수
	GLvoid Render() {
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shaderProgramID);

		// 좌표계 그리기
		init_Matrix();
		UpdateVBO(objfile);
		glDrawElements(GL_TRIANGLES, 3 * objfile.indexlist.size(), GL_UNSIGNED_INT, 0);	
	
		for (int i = 0; i < figure.size(); i++)
		{
			Make_Matrix(orbit_angle_y, figure.at(i));
			UpdateVBO(figure.at(i));
			glDrawElements(GL_TRIANGLES, 3 * figure.at(i).indexlist.size(), GL_UNSIGNED_INT, 0);
			UpdateVBO(figure.at(i));
			for (int i = 0; i < figure.at(i).follow_vertex.size(); i++)
			{
				glDrawArrays(GL_LINES, i, 2);
			}

		}

		glutSwapBuffers();
	}

	GLvoid InitBuffer()
	{
		//glGenVertexArrays(1, &cube.vao);
		//glGenBuffers(2, cube.vbo);
		//glGenBuffers(1, &cube.EBO);

		//glGenVertexArrays(1, &objfile.vao);
		//glGenBuffers(2, objfile.vbo);
		//glGenBuffers(1, &objfile.EBO);

		//glGenVertexArrays(1, &tetra.vao);
		//glGenBuffers(2, tetra.vbo);
		//glGenBuffers(1, &tetra.EBO);

		//glGenVertexArrays(1, &corn.vao);
		//glGenBuffers(2, corn.vbo);
		//glGenBuffers(1, &corn.EBO);

		//glGenVertexArrays(1, &Coordinate_system.vao);
		//glGenBuffers(2, Coordinate_system.vbo);
		//glGenBuffers(1, &Coordinate_system.EBO);

		glGenVertexArrays(1, &vao);
		glGenBuffers(2, vbo);
		glGenBuffers(1, &EBO);
	}

	GLvoid init_Matrix() {
		// 모델 location 인덱스 저장
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "trans");
		// 변환 관련 변수를 정의합니다.
		glm::mat4 modelMatrix = glm::mat4(1.0f); // 모델 변환
		glm::mat4 viewMatrix = glm::mat4(1.0f);  // 뷰 변환 (카메라 변환)
		glm::mat4 projectionMatrix = glm::mat4(1.0f); // 투영 변환 (프로젝션 변환)

		// 모델 변환
		modelMatrix = glm::translate(modelMatrix, glm::vec3(objfile.transform.x, objfile.transform.y, objfile.transform.z));  // 

		// 카메라 설정: 카메라 위치, 카메라가 바라보는 위치, 월드 업 벡터
		viewMatrix = glm::lookAt(
			glm::vec3(camera.x, camera.y, camera.z), // 카메라 위치
			glm::vec3(camera.at_x, camera.at_y, camera.at_z), // 카메라가 바라보는 지점
			glm::vec3(0.0f, 1.0f, 0.0f)  // 월드 업 벡터
		);

		// 투영 설정: FOV, 종횡비, near, far 클립 거리
		projectionMatrix = glm::perspective(
			glm::radians(45.0f), // 시야각
			(float)WIDTH / (float)HEIGHT, // 종횡비
			0.1f, 50.0f // 클립 평면 (near, far)
		);


		glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix; // MVP 변환


		// 모델 변환 적용
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

	}
	GLvoid Make_Matrix(float& orbit_angle, Object obj) {
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "trans");
		// 변환 관련 변수를 정의합니다.
		glm::mat4 modelMatrix = glm::mat4(1.0f);  // 모델 변환
		glm::mat4 viewMatrix = glm::mat4(1.0f);   // 뷰 변환 (카메라 변환)
		glm::mat4 projectionMatrix = glm::mat4(1.0f); // 투영 변환 (프로젝션 변환)
		// 공전 변환: 객체가 원점을 기준으로 회전하게 합니다.
		//modelMatrix = glm::rotate(modelMatrix, glm::radians(orbit_angle), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축 공전
		modelMatrix = glm::translate(modelMatrix, glm::vec3(obj.transform.x, obj.transform.y, obj.transform.z));  // 공전 거리 적용
		modelMatrix = glm::rotate(modelMatrix, glm::radians(theta), glm::vec3(0.0f, 0.0f, 1.0f));  // Y축 자전
	

		// 제자리 회전: 공전된 위치에서 객체를 제자리에서 회전시킵니다.
		modelMatrix = glm::rotate(modelMatrix, glm::radians(obj.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));  // X축 회전
		modelMatrix = glm::rotate(modelMatrix, glm::radians(obj.rotation.x), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축 회전
		modelMatrix = glm::rotate(modelMatrix, glm::radians(obj.rotation.x), glm::vec3(0.0f, 0.0f, 1.0f));  // Z축 회전

		// 크기 변환
		modelMatrix = glm::scale(modelMatrix, glm::vec3(obj.scaling.x, obj.scaling.y, obj.scaling.z));


		// 카메라 설정
		viewMatrix = glm::lookAt(
			glm::vec3(camera.x, camera.y, camera.z), // 카메라 위치
			glm::vec3(camera.at_x, camera.at_y, camera.at_z), // 카메라가 바라보는 지점
			glm::vec3(0.0f, 1.0f, 0.0f)  // 월드 업 벡터
		);

		// 투영 설정
		projectionMatrix = glm::perspective(
			glm::radians(45.0f), // 시야각
			(float)WIDTH / (float)HEIGHT, // 종횡비
			0.1f, 50.0f // 클립 평면 (near, far)
		);

		// 최종 변환 행렬(MVP)을 계산하여 GPU로 전달
		glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
	}

	void Timer(int value) {
		float rotate_gap = PI / 20;
		time_for_craft++;
		theta++;
		if (time_for_craft > 60)
		{
			float size = (static_cast<float>(rand()) / RAND_MAX)/10.0f + 0.075f;
			if (rand() % 10 >= 5)
			{
				figure.emplace_back(Make_figure_R(size, 3));
			}
			else
			{		
				figure.emplace_back(Make_figure_L(size,3));
			}
			time_for_craft = 0;
		}
		else if (time_for_craft == 30)
		{
			for (int i = 0; i < figure.size(); i++)
			{
				figure.at(i).follow_vertex.emplace_back(glm::vec3{ figure.at(i).transform.x ,figure.at(i).transform.y ,figure.at(i).transform.z});
			}
		}
	
		for (int i = 0; i < figure.size(); i++)
		{
			figure.at(i).flight_time++;
			if (!figure.at(i).is_sliced) {
				std::cout << " 변환 " << figure.at(i).dx << figure.at(i).dy << std::endl;
				figure.at(i).transform.x += figure.at(i).dx * speed;
				figure.at(i).transform.y += figure.at(i).dy * speed;
			}
			figure.at(i).transform.y -= figure.at(i).flight_time * 0.00015f;
		}
		glutTimerFunc(50, Timer, 0);
		glutPostRedisplay(); // 다시 그리기 요청
	}
	void Mouse(int button, int state, int x, int y) {

	}
	void Motion(int x, int y) {

	}
	void SpecialKeyboard(int key, int x, int y)
	{
		switch (key)
		{

		default:
			break;
		}
	}

	GLvoid Keyboard(unsigned char key, int x, int y) {
		switch (key)
		{
		case 's':

			break;
		default:
			break;
		}
	}

	void mainLoop() {


	
	}

	void init_figure() {
		//---------------------------obj파일-----------------------------
		objfile.vertex.clear();
		objfile.color.clear();
		objfile.indexlist.clear();
		read_obj_file("cat_ldc.obj", objfile);
		objfile.transform.y = -1.0f;
		//-----------------월드 변환 좌표 입력--------------------------
		camera.x = 0.0f, camera.y = 0.0f, camera.z = 4.0f;
		camera.at_x = 0.0f, camera.at_y = 0.0f, camera.at_z = 0.0f;
	
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

	Object Make_figure_L(float size, unsigned int mode){
		Object obj;

		float x = -2.0f + 2 * size;
		float y = (static_cast<float>(rand()) / RAND_MAX) / 2 - 0.75f;
		float dy = static_cast<float>(rand()) / RAND_MAX + 0.5f;
		float normalsize = glm::sqrt(dy * dy + 1.0f);
		obj.dy = (dy / normalsize) / 100.0f;
		obj.dx = (1.0f / normalsize) / 100.0f;
		obj.transform = { x,y,0.0f };
		obj.scaling = { 1.0f,1.0f,1.0f };
		obj.rotation = { 0.0f,0.0f,0.0f };

		// 정점 추가
		switch (mode)
		{
		case 3:
			obj.vertex.emplace_back(glm::vec3{ 0.0f,size,0.0f });
			obj.vertex.emplace_back(glm::vec3{ -0.85f * size,-size / 2,0.0f });
			obj.vertex.emplace_back(glm::vec3{ 0.85f * size,-size / 2,0.0f });
			break;
		default:
			break;
		}
		obj.follow_vertex.emplace_back(obj.vertex.front());
		/*obj.vertex.emplace_back(glm::vec3{  });
		obj.vertex.emplace_back(glm::vec3{  });
		obj.vertex.emplace_back(glm::vec3{  });
		obj.vertex.emplace_back(glm::vec3{  });*/
		// 색상 추가
		for (int i = 0; i < obj.vertex.size(); i++)
		{
			float r = (static_cast<float>(rand()) / RAND_MAX) / 4;
			float g = (static_cast<float>(rand()) / RAND_MAX) / 4;
			float b = (static_cast<float>(rand()) / RAND_MAX) / 4;
			obj.color.emplace_back(glm::vec3{ 0.4f + r, 0.1f + g, 0.1f + b });
		}

		//인덱스리스트 추가
		for (unsigned int i = 1; i < obj.vertex.size() - 1; i++)
		{
			obj.indexlist.emplace_back(Index{ 0,i,i + 1 });
		}

		return obj;
	}

	Object Make_figure_R(float size, unsigned int mode) {
		Object obj;

		float x = 2.0f - 2 * size;
		float y = (static_cast<float>(rand()) / RAND_MAX) / 2 - 0.75f;
		float dy = static_cast<float>(rand()) / RAND_MAX + 0.5f;
		float normalsize = glm::sqrt(dy * dy + 1.0f);
		obj.dy = (dy / normalsize) / 100.0f;
		obj.dx = -(1.0f / normalsize) / 100.0f;
		obj.transform = { x,y,0.0f };
		obj.scaling = { 1.0f,1.0f,1.0f };
		obj.rotation = { 0.0f,0.0f,0.0f };

		// 정점 추가
		switch (mode)
		{
		case 3:
			obj.vertex.emplace_back(glm::vec3{ 0.0f,size,0.0f });
			obj.vertex.emplace_back(glm::vec3{ -0.85f * size,-size / 2,0.0f });
			obj.vertex.emplace_back(glm::vec3{ 0.85f * size,-size / 2,0.0f });
			break;
		default:
			break;
		}
		obj.follow_vertex.emplace_back(obj.vertex.front());
		/*obj.vertex.emplace_back(glm::vec3{  });
		obj.vertex.emplace_back(glm::vec3{  });
		obj.vertex.emplace_back(glm::vec3{  });
		obj.vertex.emplace_back(glm::vec3{  });*/
		// 색상 추가
		for (int i = 0; i < obj.vertex.size(); i++)
		{
			float r = (static_cast<float>(rand()) / RAND_MAX) / 4;
			float g = (static_cast<float>(rand()) / RAND_MAX) / 4;
			float b = (static_cast<float>(rand()) / RAND_MAX) / 4;
			obj.color.emplace_back(glm::vec3{ 0.4f + r, 0.1f + g, 0.1f + b });
		}

		//인덱스리스트 추가
		for (unsigned int i = 1; i < obj.vertex.size() - 1; i++)
		{
			obj.indexlist.emplace_back(Index{ 0,i,i + 1 });
		}

		return obj;
	}

	GLvoid UpdateVBO(Object object) {
		// VAO 바인드
		glBindVertexArray(vao);

		// 큐브 위치 버퍼 바인드
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, object.vertex.size() * sizeof(glm::vec3), object.vertex.data(), GL_STREAM_DRAW);

		// 큐브 인덱스 버퍼 바인드
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		// 전체 인덱스 배열의 크기 전달
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, object.indexlist.size() * sizeof(Index), object.indexlist.data(), GL_DYNAMIC_DRAW);

		// 위치 속성 설정
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
		glEnableVertexAttribArray(0);

		// 큐브 색상 버퍼 바인드
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, object.color.size() * sizeof(glm::vec3), object.color.data(), GL_DYNAMIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
		glEnableVertexAttribArray(1);
	}

	GLvoid UpdateVBO(Coordinate object) {
		// VAO 바인드
		glBindVertexArray(vao);

		// 좌표계 정점 버퍼 바인드
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, object.vertex.size() * sizeof(glm::vec3), object.vertex.data(), GL_DYNAMIC_DRAW);

		// 좌표계 인덱스 버퍼 바인드
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, object.indexlist.size() * sizeof(unsigned int), object.indexlist.data(), GL_DYNAMIC_DRAW);

		// 위치 속성 설정
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
		glEnableVertexAttribArray(0);

		// 좌표계 색상 버퍼 바인드
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
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
				unsigned int vt1, vt2, vt3; // 텍스처 좌표 인덱스
				unsigned int vn1, vn2, vn3; // 법선 벡터 인덱스
				sscanf_s(line + 2, "%u/%u/%u %u/%u/%u %u/%u/%u",
					&v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);

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
			model.color.push_back(glm::vec3(1.0f, 1.0f, 1.0f)); // 빨강색으로 설정
		}
		model.transform = { 0.0f,0.0f,0.0f };
		model.scaling = { 1.0f,1.0f,1.0f };
		model.rotation = { 0.0f,0.0f,0.0f };
		UpdateVBO(model);
	}