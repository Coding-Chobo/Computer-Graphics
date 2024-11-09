	#define _CRT_SECURE_NO_WARNINGS
	#include "CG_OPGL.h"

	#define WIDTH 1000
	#define HEIGHT 1000
	#define PI 3.1415926535897932384620588419716939
	#define MAX_LINE_LENGTH 1024
	GLchar* vertexSource, * fragmentSource; // �ҽ��ڵ� ���� ����
	GLuint vertexShader, fragmentShader; //���̴� ��ü
	GLuint shaderProgramID; // ���̴� ���α׷�

	//�׷��� ������Ʈ ����
	vector<Object>figure;
	Object objfile{};
	Coordinate Coordinate_system{}, eraser_m{};
	Camera_Info camera{};
	Translate cp1, cp2;
	//���� ����
	GLuint vao;
	GLuint vbo[2];
	GLuint EBO;

	//�ǽ��� �ʿ��� ������
	float theta{};
	float speed{ 3.0f };
	unsigned int time_for_craft { 0 };
	bool mode_line{false}, mode_path{false};
	unsigned int re_time = 50;


	//-----------------------------------------------------------------------
	void main(int argc, char** argv)
	{
		srand(time(NULL));
		// ������ ����
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
		glutInitWindowPosition(0, 0);
		glutInitWindowSize(WIDTH, HEIGHT);
		glutCreateWindow("Slice Polygon");

		// GLEW �ʱ�ȭ�ϱ�
		glewExperimental = GL_TRUE;
		glewInit();

		// ���� ����

		// ��������
		glEnable(GL_CULL_FACE);
		//glEnable(GL_DEPTH_TEST);

		InitBuffer();
		CreateShaderProgram();
		//���� ���� �ʱ�ȭ
		init_figure();

		//���� �Լ�
		glutReshapeFunc(Reshape);
		glutMouseFunc(Mouse);
		glutMotionFunc(Motion);
		glutKeyboardFunc(Keyboard);
		glutSpecialFunc(SpecialKeyboard);
		glutDisplayFunc(Render);
		glutTimerFunc(50, Timer, 0);

		glutIdleFunc(mainLoop);
		glutMainLoop(); //�̺�Ʈ ���� ����
	}
	//�׸��� �Լ�
	GLvoid Render() {
		glClearColor(0.8f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT );
		glUseProgram(shaderProgramID);
		
		//�ڸ��� ���� �׸���
		if (eraser_m.vertex.size() == 2)
		{
			Make_Matrix(eraser_m);
			UpdateVBO(eraser_m);
			glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
		}
		init_Matrix();

		//����� �׸���
		UpdateVBO(objfile);
		glDrawElements(GL_TRIANGLES, 3 * objfile.indexlist.size(), GL_UNSIGNED_INT, 0);	

		//��� �׸���
		if (mode_path)
		{
			for (int i = 0; i < figure.size(); i++)
			{
				Make_Matrix(figure.at(i).path);
				UpdateVBO(figure.at(i).path);
				glDrawElements(GL_LINES, figure.at(i).path.indexlist.size(), GL_UNSIGNED_INT, 0);
			}
		}

		if (mode_line)
		{
			//�����׸���
			for (int i = 0; i < figure.size(); i++)
			{
				Make_Matrix(figure.at(i));
				UpdateVBO(figure.at(i));
				glDrawElements(GL_LINE_LOOP, 3 * figure.at(i).indexlist.size(), GL_UNSIGNED_INT, 0);
			}
		}
		else
		{
			//�����׸���
			for (int i = 0; i < figure.size(); i++)
			{
				Make_Matrix(figure.at(i));
				UpdateVBO(figure.at(i));
				glDrawElements(GL_TRIANGLES, 3 * figure.at(i).indexlist.size(), GL_UNSIGNED_INT, 0);
			}
		}



		glutSwapBuffers();
	}

	GLvoid InitBuffer()
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(2, vbo);
		glGenBuffers(1, &EBO);
	}

	GLvoid init_Matrix() {
		// �� location �ε��� ����
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "trans");
		// ��ȯ ���� ������ �����մϴ�.
		glm::mat4 modelMatrix = glm::mat4(1.0f); // �� ��ȯ
		glm::mat4 viewMatrix = glm::mat4(1.0f);  // �� ��ȯ (ī�޶� ��ȯ)
		glm::mat4 projectionMatrix = glm::mat4(1.0f); // ���� ��ȯ (�������� ��ȯ)

		// �� ��ȯ
		modelMatrix = glm::translate(modelMatrix, glm::vec3(objfile.transform.x, objfile.transform.y, objfile.transform.z));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(objfile.scaling.x, objfile.scaling.y, objfile.scaling.z));
		glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix; // MVP ��ȯ
		// �� ��ȯ ����
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

	}
	GLvoid Make_Matrix(Coordinate obj) {
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "trans");
		// ��ȯ ���� ������ �����մϴ�.
		glm::mat4 modelMatrix = glm::mat4(1.0f);  // �� ��ȯ
		glm::mat4 viewMatrix = glm::mat4(1.0f);   // �� ��ȯ (ī�޶� ��ȯ)
		glm::mat4 projectionMatrix = glm::mat4(1.0f); // ���� ��ȯ (�������� ��ȯ)


		// ���� ��ȯ ���(MVP)�� ����Ͽ� GPU�� ����
		glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
	}
	GLvoid Make_Matrix(Object obj) {
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "trans");
		// ��ȯ ���� ������ �����մϴ�.
		glm::mat4 modelMatrix = glm::mat4(1.0f);  // �� ��ȯ
		glm::mat4 viewMatrix = glm::mat4(1.0f);   // �� ��ȯ (ī�޶� ��ȯ)
		glm::mat4 projectionMatrix = glm::mat4(1.0f); // ���� ��ȯ (�������� ��ȯ)
		// ���� ��ȯ: ��ü�� ������ �������� ȸ���ϰ� �մϴ�.
		//modelMatrix = glm::rotate(modelMatrix, glm::radians(orbit_angle), glm::vec3(0.0f, 1.0f, 0.0f));  // Y�� ����
		modelMatrix = glm::translate(modelMatrix, glm::vec3(obj.transform.x, obj.transform.y, obj.transform.z));  // ���� �Ÿ� ����
		modelMatrix = glm::rotate(modelMatrix, glm::radians(theta), glm::vec3(0.0f, 0.0f, 1.0f));  // Y�� ����
	
		// ���� ��ȯ ���(MVP)�� ����Ͽ� GPU�� ����
		glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	}

	void Timer(int value) {
		float rotate_gap = PI / 20;
		time_for_craft++;
		//theta+=2;
		
		// ��������
		if (time_for_craft > 40)
		{
			float size = (static_cast<float>(rand()) / RAND_MAX)/20.0f + 0.075f;
			float shape = rand() % 17 + 3;
			if (rand() % 10 >= 5)
			{
				figure.emplace_back(Make_figure_R(size, shape));
			}
			else
			{		
				figure.emplace_back(Make_figure_L(size, shape));
			}
			time_for_craft = 0;
		}

		//------------------------------��� �߰�-----------------------------------
		if (time_for_craft % 3 == 0)
		{
			for (int i = 0; i < figure.size(); i++)
			{
				figure.at(i).path.vertex.emplace_back(glm::vec3{ figure.at(i).transform.x,figure.at(i).transform.y,0.0f});
				figure.at(i).path.color.emplace_back(glm::vec3{ 1.0f,1.0f ,1.0f });
				figure.at(i).path.indexlist.emplace_back(figure.at(i).path.vertex.size() - 1);
				figure.at(i).path.indexlist.emplace_back(figure.at(i).path.vertex.size() - 1);
			}
		}	

		//�߷� ���� �� ������ ���⺤�ͷ� �̵�
		for (int i = 0; i < figure.size(); i++)
		{
			figure.at(i).flight_time++;
			if (!figure.at(i).is_sliced) {
				//std::cout << " ��ȯ " << figure.at(i).dx << figure.at(i).dy << std::endl;
				figure.at(i).transform.x += figure.at(i).dx * speed;
				figure.at(i).transform.y += figure.at(i).dy * speed;
				figure.at(i).transform.y -= figure.at(i).flight_time * 0.0004f;
			}
			else
			{
				figure.at(i).transform.x += figure.at(i).dx * speed;
				figure.at(i).transform.y += figure.at(i).dy * speed;
				figure.at(i).transform.y -= figure.at(i).flight_time * 0.003f;
			}
		}



		glutTimerFunc(re_time, Timer, 0);
		glutPostRedisplay(); // �ٽ� �׸��� ��û
	}

	void mainLoop() {
		// �� ��Ż �˻�
		for (auto it = figure.begin(); it != figure.end(); )  // �ݺ��ڸ� ����Ͽ� ��ȸ
		{
			if (it->transform.x < -1.1f || it->transform.x > 1.1f)
			{
				it = figure.erase(it);  // ���ǿ� �´� ��� ���� ��, ���ο� ��ġ�� �ݺ��� ��ȯ
			}
			if (it->transform.y < -1.1f || it->transform.y > 1.1f)
			{
				it = figure.erase(it);  // ���ǿ� �´� ��� ���� ��, ���ο� ��ġ�� �ݺ��� ��ȯ
			}
			else
			{
				++it;  // ���ǿ� ���� ������ ���� ��ҷ� �̵�
			}
		}

		BoundingBox hitbox = { objfile.transform.x - 0.08f,objfile.transform.y + 0.1f,objfile.transform.x + 0.08f,objfile.transform.y - 0.1f };
		// ������ obj���� �浹�˻�
		for (int i = 0; i < figure.size(); i++) {
			if (figure[i].is_sliced)
			{
				for (int j = 0; j < figure[i].vertex.size(); j++)
				{
					glm::vec3 v = { figure[i].transform.x + figure[i].vertex[j].x,figure[i].transform.y + figure[i].vertex[j].y,0 };
					if (InRectangle(v, hitbox))
					{
						float point = figure[i].color[0].r + figure[i].color[0].g + figure[i].color[0].b;
						point /= 15.0f;
						for (unsigned k = 0; k < objfile.color.size(); k++)
						{
							objfile.color[k].r += point;
							objfile.color[k].g += point;
							objfile.color[k].b += point;
						}
						figure.erase(figure.begin() + i);
						break;
					}

				}
			}
		}

		//������ ȸ�� 0.001���� ȸ��
		for (int i = 0; i < figure.size(); i++) {
			for (size_t j = 0; j < figure[i].vertex.size(); j++)
			{	
				float x = figure[i].vertex[j].x;
				float y = figure[i].vertex[j].y;
				figure[i].vertex[j].x = x * cos(glm::radians(0.001f)) - y * sin(glm::radians(0.001f));
				figure[i].vertex[j].y = x * sin(glm::radians(0.001f)) + y * cos(glm::radians(0.001f));
			}
		}
	}

	bool InRectangle(glm::vec3 p, BoundingBox hitbox) {
		if (hitbox.left <= p.x && hitbox.right >= p.x)
		{
			if (hitbox.bottom <= p.y && hitbox.top >= p.y)
			{
				return true;
			}
		}
		return false;
	}

	void Mouse(int button, int state, int x, int y) {
		float xpos = static_cast<float>(x) / (WIDTH / 2.0f) - 1.0f;
		float ypos = 1.0f - static_cast<float>(y) / (HEIGHT / 2.0f);

		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			eraser_m.vertex.emplace_back(glm::vec3{ xpos, ypos, 0.0f });
			eraser_m.vertex.emplace_back(glm::vec3{ xpos, ypos, 0.0f });
		}
		else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
			for (size_t i = 0; i < figure.size(); ++i) {
				Translate cp1, cp2;
				if (CheckCollisionWithPolygon(figure[i], eraser_m.vertex[0], eraser_m.vertex[1], cp1, cp2)) {					
					figure[i].is_sliced = true;
					figure[i].flight_time = 0;
					// �ٰ����� �ڸ� �� ���� ���ο� �ٰ��� ����
					Object newFigure1, newFigure2;
					SplitPolygonIntoTwo(figure[i], newFigure1, newFigure2, cp1, cp2);

					// figure�� �������� ���ο� �ٰ��� �߰�
					figure.push_back(newFigure1);
					figure.push_back(newFigure2);

					// ���� ���� ����
					figure.erase(figure.begin() + i);
					break;
				}
			}
			eraser_m.vertex.clear();
		}
	}

	void SplitPolygonIntoTwo(const Object& original, Object& fig1, Object& fig2, const Translate& cp1, const Translate& cp2) {
		fig1 = original;
		fig2 = original;
		fig1.vertex.clear();
		fig2.vertex.clear();
		// ������ �߰�
		fig1.vertex.emplace_back(glm::vec3(cp1.x, cp1.y, cp1.z));
		fig2.vertex.emplace_back(glm::vec3(cp2.x, cp2.y, cp2.z));

		// ������ ���� ���� ���
		glm::vec3 lineDirection = glm::vec3(cp2.x - cp1.x, cp2.y - cp1.y, cp2.z - cp1.z);

		// ���� ������: ������ �������� ������ �� ���� �׷����� ����
		for (const auto& vertex : original.vertex) {
			// cp1���� ���� ���������� ����
			glm::vec3 vertexDirection = glm::vec3(vertex.x - cp1.x, vertex.y - cp1.y, vertex.z - cp1.z);

			// ���� ����Ͽ� ������ ��ġ �Ǵ�
			float crossProduct = lineDirection.x * vertexDirection.y - lineDirection.y * vertexDirection.x;

			// ���� ����� ���� ���� �׷�ȭ
			if (crossProduct > 0) {
				fig1.vertex.push_back(vertex);  // ���� �ٰ����� �߰�
			}
			else {
				fig2.vertex.push_back(vertex);  // �ٸ� �� �ٰ����� �߰�
			}
		}
		
		// �� �ٰ����� ���� ������ �߰��Ͽ� �ݱ�
		fig1.vertex.push_back(glm::vec3(cp2.x, cp2.y, cp2.z));
		fig2.vertex.push_back(glm::vec3(cp1.x, cp1.y, cp1.z));

		//������ �ݽð�������� ����
		SortVerticesCounterClockwise(fig1);
		SortVerticesCounterClockwise(fig2);

		// ����� �ε��� �߰�
		AddColors_Indexlist(fig1);
		AddColors_Indexlist(fig2);

		fig1.dx = -(fig1.dx * 2 / 3);
		fig2.dx = (fig2.dx * 2 / 3);
	}

	void AddColors_Indexlist(Object& fig) {
		unsigned int j = rand() % 3;
		float c[3]{ 0.4f };
		c[0] = fig.color[0].r;
		c[1] = fig.color[0].g;
		c[2] = fig.color[0].b;
		fig.color.clear();
		fig.indexlist.clear();
		// ���� ���� ����

		for (int i = 0; i < fig.vertex.size(); i++) {
			float r = (static_cast<float>(rand()) / RAND_MAX) / 10;
			float g = (static_cast<float>(rand()) / RAND_MAX) / 10;
			float b = (static_cast<float>(rand()) / RAND_MAX) / 10;
			fig.color.emplace_back(glm::vec3{ c[0] + r,c[1] + g,c[2] + b });
		}
		// �ε��� �߰� (�ﰢ���� �̷�� ������� �߰�)
		for (unsigned int i = 1; i < fig.vertex.size() - 1; i++) {
			fig.indexlist.emplace_back(Index{ 0, i, i + 1 });
		}
	}

	void Motion(int x, int y) {
		// ��ǥ ��ȯ
		float xpos = static_cast<float>(x) / (WIDTH / 2.0f) - 1.0f;
		float ypos = 1.0f - static_cast<float>(y) / (HEIGHT / 2.0f);
		if (!eraser_m.vertex.empty())
		{
			eraser_m.vertex.at(1) = glm::vec3{ xpos ,ypos ,0.0f };
		}
	}

	void SpecialKeyboard(int key, int x, int y)
	{
		switch (key)
		{
		case GLUT_KEY_LEFT:
			objfile.transform.x -= 0.02f;
			break;
		case GLUT_KEY_RIGHT:
			objfile.transform.x += 0.02f * (150 - re_time) / 100;
			break;
		default:
			break;
		}
	}

	GLvoid Keyboard(unsigned char key, int x, int y) {
		switch (key)
		{
		case 'q':
			exit(0);
			break;
		case 'l':
			mode_line = !mode_line;
			break;
		case 'p':
			mode_path = !mode_path;
			break;
		case '+':
			if (re_time > 10)
			{
				re_time--;
			}
			break;
		case '-':
			if (re_time < 100)
			{
				re_time++;
			}
			break;
		default:
			break;
		}
	}

	void init_figure() {
		//---------------------------obj����-----------------------------
		objfile.vertex.clear();
		objfile.color.clear();
		objfile.indexlist.clear();
		read_obj_file("cat_ldc.obj", objfile);
		objfile.transform.y = -0.87f;
		objfile.scaling.x = 0.5f;
		objfile.scaling.y = 0.5f;
		//-----------------���� ��ȯ ��ǥ �Է�--------------------------
		camera.x = 0.0f, camera.y = 0.0f, camera.z = 4.0f;
		camera.at_x = 0.0f, camera.at_y = 0.0f, camera.at_z = 0.0f;
		//--------------------------���콺 ���찳-----------------------------
		eraser_m.color.emplace_back(glm::vec3{ 1.0f,1.0f ,1.0f });
		eraser_m.color.emplace_back(glm::vec3{ 1.0f,1.0f ,1.0f });
		eraser_m.indexlist.clear();
		eraser_m.indexlist.emplace_back(0);
		eraser_m.indexlist.emplace_back(1);
	}

	Object Make_figure_L(float size, unsigned int mode){
		Object obj;

		float x = size - 1.0f;
		float y = (static_cast<float>(rand()) / RAND_MAX) / 2 - 0.75f;
		float dy = static_cast<float>(rand()) / RAND_MAX + 0.5f;
		obj.flight_time = 0;
		float normalsize = glm::sqrt(dy * dy + 1.0f);
		obj.dy = (dy / normalsize) / 100.0f;
		obj.dx = (1.0f / normalsize) / 100.0f;
		obj.transform = { x,y,0.0f };
		obj.scaling = { 1.0f,1.0f,1.0f };
		obj.rotation = { 0.0f,0.0f,0.0f };

		// ���� �߰�
		float each_angle = 360 / mode;
		if (mode != 7)
		{
			for (int i = 0; i < mode; i++)
			{
				obj.vertex.emplace_back(glm::vec3{ size * cos(glm::radians(each_angle * i)), size * sin(glm::radians(each_angle * i)),0.0f });
			}
			SortVerticesCounterClockwise(obj);
		}
		else if (mode == 7)
		{

			obj.vertex.emplace_back(glm::vec3{ 0.0f,size * sin(glm::radians(15.0f)),0.0f });
			obj.vertex.emplace_back(glm::vec3{ size * cos(glm::radians(40.0f)) -size * cos(glm::radians(15.0f)),size * sin(glm::radians(40.0f)),0.0f });
			obj.vertex.emplace_back(glm::vec3{ -size * cos(glm::radians(40.0f)),size * sin(glm::radians(40.0f)),0.0f });
			obj.vertex.emplace_back(glm::vec3{ -size * cos(glm::radians(15.0f)),size * sin(glm::radians(15.0f)),0.0f });
			obj.vertex.emplace_back(glm::vec3{ size * cos(glm::radians(270.0f)),size * sin(glm::radians(270.0f)),0.0f });
			obj.vertex.emplace_back(glm::vec3{ size * cos(glm::radians(15.0f)),size * sin(glm::radians(15.0f)),0.0f });
			obj.vertex.emplace_back(glm::vec3{ size * cos(glm::radians(40.0f)),size * sin(glm::radians(40.0f)),0.0f });
			obj.vertex.emplace_back(glm::vec3{ size * cos(glm::radians(15.0f)) - size * cos(glm::radians(40.0f)),size * sin(glm::radians(40.0f)),0.0f });
		}

		//���� �ε�������Ʈ ����
		unsigned int j = rand() % 3;
		float c[3]{ 0.3f , 0.3f , 0.3f };
		c[j] += (static_cast<float>(rand()) / RAND_MAX) / 2;
		obj.color.emplace_back(glm::vec3{ c[0] ,c[1] ,c[2] });
		AddColors_Indexlist(obj);
		//------------------------------���-----------------------------------
		obj.path.vertex.emplace_back(glm::vec3{ obj.transform.x,obj.transform.y,obj.transform.z });
		obj.path.color.emplace_back(glm::vec3{ 1.0f,1.0f ,1.0f });
		obj.path.indexlist.emplace_back(0);

		return obj;
	}

	Object Make_figure_R(float size, unsigned int mode) {
		Object obj;

		float x = 1.0f - size;
		float y = (static_cast<float>(rand()) / RAND_MAX) / 2 - 0.75f;
		float dy = static_cast<float>(rand()) / RAND_MAX + 0.5f;
		float normalsize = glm::sqrt(dy * dy + 1.0f);
		obj.flight_time = 0;
		obj.dy = (dy / normalsize) / 100.0f;
		obj.dx = -(1.0f / normalsize) / 100.0f;
		obj.transform = { x,y,0.0f };
		obj.scaling = { 1.0f,1.0f,1.0f };
		obj.rotation = { 0.0f,0.0f,0.0f };

		// ���� �߰�
		float each_angle = 360 / mode;
		if (mode != 7)
		{
			for (int i = 0; i < mode; i++)
			{
				obj.vertex.emplace_back(glm::vec3{ size * cos(glm::radians(each_angle * i)), size * sin(glm::radians(each_angle * i)),0.0f });
			}
			SortVerticesCounterClockwise(obj);
		}
		else if (mode == 7)
		{

			obj.vertex.emplace_back(glm::vec3{ 0.0f,size * sin(glm::radians(15.0f)),0.0f });
			obj.vertex.emplace_back(glm::vec3{ size * cos(glm::radians(40.0f)) - size * cos(glm::radians(15.0f)),size * sin(glm::radians(40.0f)),0.0f });
			obj.vertex.emplace_back(glm::vec3{ -size * cos(glm::radians(40.0f)),size * sin(glm::radians(40.0f)),0.0f });
			obj.vertex.emplace_back(glm::vec3{ -size * cos(glm::radians(15.0f)),size * sin(glm::radians(15.0f)),0.0f });
			obj.vertex.emplace_back(glm::vec3{ size * cos(glm::radians(270.0f)),size * sin(glm::radians(270.0f)),0.0f });
			obj.vertex.emplace_back(glm::vec3{ size * cos(glm::radians(15.0f)),size * sin(glm::radians(15.0f)),0.0f });
			obj.vertex.emplace_back(glm::vec3{ size * cos(glm::radians(40.0f)),size * sin(glm::radians(40.0f)),0.0f });
			obj.vertex.emplace_back(glm::vec3{ size * cos(glm::radians(15.0f)) - size * cos(glm::radians(40.0f)),size * sin(glm::radians(40.0f)),0.0f });
		}

		//���� �ε�������Ʈ ����
		unsigned int j = rand() % 3;
		float c[3]{ 0.3f , 0.3f , 0.3f };
		c[j] += (static_cast<float>(rand()) / RAND_MAX) / 2;
		obj.color.emplace_back(glm::vec3{ c[0] ,c[1] ,c[2] });
		AddColors_Indexlist(obj);

		//------------------------------���-----------------------------------
		obj.path.vertex.emplace_back(glm::vec3{ obj.transform.x,obj.transform.y,obj.transform.z });
		obj.path.color.emplace_back(glm::vec3{ 1.0f,1.0f ,1.0f });
		obj.path.indexlist.emplace_back(0);

		return obj;
	}

	bool CheckCollisionWithPolygon(const Object& obj, glm::vec3 lineStart, glm::vec3 lineEnd, Translate& cp1, Translate& cp2) {
		size_t vertexCount = obj.vertex.size();
		int intersectionCount = 0;

		for (size_t i = 0; i < vertexCount; ++i) {
			glm::vec3 v0 = { obj.vertex[i].x + obj.transform.x, obj.vertex[i].y + obj.transform.y, 0.0f };
			glm::vec3 v1 = { obj.vertex[(i + 1) % vertexCount].x + obj.transform.x, obj.vertex[(i + 1) % vertexCount].y + obj.transform.y, 0.0f };

			Translate intersection;
			if (GetIntersectionPoint(lineStart, lineEnd, v0, v1, intersection)) {
				intersectionCount++;
				if (intersectionCount == 1) {
					cp1 = intersection;
				}
				else if (intersectionCount == 2) {
					cp2 = intersection;
					if (cp1.x > cp2.x) {
						Translate temp;
						temp = cp1;
						cp1 = cp2;
						cp2 = temp;
					}
					cp1.x -= obj.transform.x;
					cp1.y -= obj.transform.y;
					cp2.x -= obj.transform.x;
					cp2.y -= obj.transform.y;
					return true;  // 2�� �̻��� ���� �����ϸ� �߶��� ������ ����
				}
			}
		}
		return false;  // 2�� �̻��� ���� �������� ������ �ڸ��� ����
	}

	bool CheckLineIntersection(glm::vec3 p1, glm::vec3 p2, glm::vec3 q1, glm::vec3 q2) {
		// CCW ��� ���
		auto ccw = [](glm::vec3 A, glm::vec3 B, glm::vec3 C) {
			return (C.y - A.y) * (B.x - A.x) > (B.y - A.y) * (C.x - A.x);
			};

		// �� ���� (p1, p2)�� (q1, q2) ������ ���� ���� ����
		return (ccw(p1, q1, q2) != ccw(p2, q1, q2)) && (ccw(p1, p2, q1) != ccw(p1, p2, q2));
	}

	bool IsPointInTriangle(glm::vec3 pt, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2) {
		// ���� AB, BC, CA�� ����
		glm::vec3 AB = v1 - v0;
		glm::vec3 BC = v2 - v1;
		glm::vec3 CA = v0 - v2;

		// ���� �� ���� ���� ���
		glm::vec3 AP = pt - v0;
		glm::vec3 BP = pt - v1;
		glm::vec3 CP = pt - v2;

		// ������ ���� ���� �ﰢ���� ���ο� �ִ��� Ȯ��
		glm::vec3 cross1 = glm::cross(AB, AP);
		glm::vec3 cross2 = glm::cross(BC, BP);
		glm::vec3 cross3 = glm::cross(CA, CP);

		// ��� ���� ����� ������ �����ϸ� ���� �ﰢ�� ���ο� ����
		return (glm::dot(cross1, cross2) >= 0 && glm::dot(cross2, cross3) >= 0);
	}

	bool GetIntersectionPoint(const glm::vec3& p1, const glm::vec3& p2, 
		const glm::vec3& q1, const glm::vec3& q2,Translate& intersection) {
		// ���� �������� ��� ���
		float A1 = p2.y - p1.y;
		float B1 = p1.x - p2.x;
		float C1 = A1 * p1.x + B1 * p1.y;

		float A2 = q2.y - q1.y;
		float B2 = q1.x - q2.x;
		float C2 = A2 * q1.x + B2 * q1.y;

		float det = A1 * B2 - A2 * B1;

		// det�� 0�̸� �� ������ ������
		if (fabs(det) < 1e-6) {
			return false;  // ������ ����
		}

		// �� ������ �����ϹǷ� ������ ���
		intersection.x = (B2 * C1 - B1 * C2) / det;
		intersection.y = (A1 * C2 - A2 * C1) / det;
		intersection.z = 0.0f;  // 2D ����̹Ƿ� Z�� 0���� ����

		// �������� �� ������ ���� ���� �ִ��� Ȯ��
		if (intersection.x < std::min(p1.x, p2.x) || intersection.x > std::max(p1.x, p2.x) ||
			intersection.y < std::min(p1.y, p2.y) || intersection.y > std::max(p1.y, p2.y) ||
			intersection.x < std::min(q1.x, q2.x) || intersection.x > std::max(q1.x, q2.x) ||
			intersection.y < std::min(q1.y, q2.y) || intersection.y > std::max(q1.y, q2.y)) {
			return false;  // �������� ���� ���� �ۿ� ����
		}

		return true;  // �������� ������ ���� ���� ���� ��ġ
	}

	void SortVerticesCounterClockwise(Object& obj) {
		// ������: �������� �߽�(�����߽�)�� ����
		glm::vec3 center(0.0f);
		for (const auto& vertex : obj.vertex) {
			center += vertex;
		}
		center /= static_cast<float>(obj.vertex.size());

		// ���� ����� ���� ������ ������ �Բ� ����
		std::vector<std::pair<glm::vec3, float>> verticesWithAngles;
		for (const auto& vertex : obj.vertex) {
			glm::vec3 direction = vertex - center;
			float angle = atan2(direction.y, direction.x);  // Y��� X���� �̿��� ���� ���
			verticesWithAngles.push_back({ vertex, angle });
		}

		// ���� �������� ����
		std::sort(verticesWithAngles.begin(), verticesWithAngles.end(),
			[](const std::pair<glm::vec3, float>& a, const std::pair<glm::vec3, float>& b) {
				return a.second < b.second;
			});

		// ���ĵ� ������ obj.vertex�� �ٽ� ����
		obj.vertex.clear();
		for (const auto& vertexWithAngle : verticesWithAngles) {
			obj.vertex.push_back(vertexWithAngle.first);
		}
	}

	GLvoid UpdateVBO(Object object) {
		// VAO ���ε�
		glBindVertexArray(vao);

		// ť�� ��ġ ���� ���ε�
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, object.vertex.size() * sizeof(glm::vec3), object.vertex.data(), GL_STREAM_DRAW);

		// ť�� �ε��� ���� ���ε�
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		// ��ü �ε��� �迭�� ũ�� ����
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, object.indexlist.size() * sizeof(Index), object.indexlist.data(), GL_DYNAMIC_DRAW);

		// ��ġ �Ӽ� ����
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
		glEnableVertexAttribArray(0);

		// ť�� ���� ���� ���ε�
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, object.color.size() * sizeof(glm::vec3), object.color.data(), GL_DYNAMIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
		glEnableVertexAttribArray(1);
	}
	GLvoid UpdateVBO(Coordinate object) {
		// VAO ���ε�
		glBindVertexArray(vao);

		// ��ǥ�� ���� ���� ���ε�
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, object.vertex.size() * sizeof(glm::vec3), object.vertex.data(), GL_DYNAMIC_DRAW);

		// ��ǥ�� �ε��� ���� ���ε�
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, object.indexlist.size() * sizeof(unsigned int), object.indexlist.data(), GL_DYNAMIC_DRAW);

		// ��ġ �Ӽ� ����
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
		glEnableVertexAttribArray(0);

		// ��ǥ�� ���� ���� ���ε�
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, object.color.size() * sizeof(glm::vec3), object.color.data(), GL_DYNAMIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
		glEnableVertexAttribArray(1);
	}

	GLvoid Reshape(int w, int h)
	{
		// ����Ʈ �⺻ WIDTH HEIGHT�� ����
		glViewport(0, 0, w, h);
	}

	char* GetBuf(const char* file)
	{
		FILE* fptr;
		long length;
		char* buf;

		// ���� �б� �������� ����
		fopen_s(&fptr, file, "rb");

		// ����ó��
		if (!fptr) return NULL;

		// ���� buf�� ��ȯ
		fseek(fptr, 0, SEEK_END);
		length = ftell(fptr);
		buf = (char*)malloc(length + 1);
		fseek(fptr, 0, SEEK_SET);
		fread(buf, length, 1, fptr);

		// ���� ����
		fclose(fptr);

		buf[length] = 0;
		return buf;
	}

	GLint CreateShader(const char* file, int type)
	{
		GLchar* source = GetBuf(file);

		// ��ü ����
		GLint shader = glCreateShader(type);
		glShaderSource(shader, 1, (const GLchar**)&source, 0);
		glCompileShader(shader);

		// ������ ���� üũ
		GLint result;
		GLchar errorLog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

		if (!result)
		{
			glGetShaderInfoLog(shader, 512, NULL, errorLog);
			std::cerr << "ERROR: ������ ����\n" << errorLog << std::endl;
			return 0;
		}

		return shader;
	}

	void CreateShaderProgram()
	{
		vertexShader = CreateShader("vertex.vs", GL_VERTEX_SHADER);
		fragmentShader = CreateShader("fragment.fs", GL_FRAGMENT_SHADER);

		// ���̴� ���α׷� ����
		shaderProgramID = glCreateProgram();
		glAttachShader(shaderProgramID, vertexShader);
		glAttachShader(shaderProgramID, fragmentShader);
		glLinkProgram(shaderProgramID);

		// ���̴� ����
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		// ���̴� ���α׷� ���
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
			read_newline(line); // ���๮�� ����

			// ����(Vertex) ������ ó��
			if (line[0] == 'v' && line[1] == ' ') {
				glm::vec3 vertex;
				sscanf_s(line + 2, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
				temp_vertices.push_back(vertex);
			}

			// ��(Face) ������ ó��
			else if (line[0] == 'f' && line[1] == ' ') {
				unsigned int v1, v2, v3;
				unsigned int vt1, vt2, vt3; // �ؽ�ó ��ǥ �ε���
				unsigned int vn1, vn2, vn3; // ���� ���� �ε���
				sscanf_s(line + 2, "%u/%u/%u %u/%u/%u %u/%u/%u",
					&v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);

				// OBJ �ε����� 1���� �����ϱ� ������ 0���� �����ϵ��� ����
				Index face = { v1 - 1, v2 - 1, v3 - 1 };
				temp_indices.push_back(face);
			}
		}

		fclose(file);

		// Object ����ü�� ���� �� �ε��� �����͸� ����
		model.vertex = temp_vertices; // ���� �����͸� �״�� ����
		model.indexlist = temp_indices; // �ε��� �����͸� �״�� ����

		// ������ ���Ƿ� ���� (���� ���� ����)
		for (size_t i = 0; i < model.vertex.size(); i++) {
			model.color.push_back(glm::vec3(0.05f, 0.05f, 0.05f)); // ���������� ����
		}
		model.transform = { 0.0f,0.0f,0.0f };
		model.scaling = { 1.0f,1.0f,1.0f };
		model.rotation = { 0.0f,0.0f,0.0f };
		UpdateVBO(model);
	}