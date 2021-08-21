#include <iostream>
#include <limits>
#include <glut.h>
#include <vector>
#include <fstream>
#include <time.h>
#include <string>

using namespace std;

int* generateIndexMas(int n);
void glut_start(int argc, char** argv);
void pointsDisplay();

const float accuracy = 0.0001;

float** generate_points(int n) {
	//параметры генератора случайных чисел
	const int divider = (int)1 / sqrt(accuracy);

	//генерация координат точек
	float** point_coord = new float* [n];
	for (int i = 0; i < n; i++) {
		point_coord[i] = new float[2];
		point_coord[i][0] = (float)(rand() % (divider + 1)) / divider;
		point_coord[i][1] = (float)(rand() % (divider + 1)) / divider;
	}

	return point_coord;
}

float distance(float* p1, float* p2) {
	return sqrt(pow(p2[0] - p1[0], 2) + pow(p2[1] - p1[1], 2));
}

int* simple_nearest_points_search(float** points, int n) {
	int* nearest_points_indexes = new int[2];
	float min_distance = numeric_limits<float>::max();
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (i != j) {
				float d = distance(points[i], points[j]);
				if (d < min_distance) {
					nearest_points_indexes[0] = i;
					nearest_points_indexes[1] = j;
					min_distance = d;
				}
			}
		}
	}
	return nearest_points_indexes;
}

template<class Type>
void print_mas(Type* mas, int size) {
	for (int i = 0; i < size; i++) {
		cout << mas[i];
		cout << " ";
	}
	cout << "\n";
}

template<class Type>
void print_matrix(Type** matrix, int size_x, int size_y) {
	for (int i = 0; i < size_x; i++) {
		for (int j = 0; j < size_y; j++) {
			cout << matrix[i][j];
			if (j != size_y - 1) {
				cout << "\t\t";
			}
		}
		cout << "\n";
	}
}

template<class Type>
Type** transp_matrix(Type** matrix, int size_x, int size_y) {
	Type** t_matrix = new float* [size_y];
	for (int i = 0; i < size_y; i++) {
		t_matrix[i] = new float[size_x];
		for (int j = 0; j < size_x; j++) {
			t_matrix[i][j] = matrix[j][i];
		}
	}
	return t_matrix;
}

template<class T>
T* merge_sort(T* mas, int n, int** indexes = NULL) {
	if (n > 1) {
		T* first_half = new T[n / 2];
		T* second_half = new T[n - n / 2];
		int* first_half_indexes = new int[n / 2];
		int* second_half_indexes = new int[n - n / 2];
		for (int i = 0; i < n / 2; i++) {
			first_half[i] = mas[i];
			if (indexes != NULL) {
				first_half_indexes[i] = (*indexes)[i];
			}
		}
		for (int i = n / 2, j = 0; i < n; i++, j++) {
			second_half[j] = mas[i];
			if (indexes != NULL) {
				second_half_indexes[j] = (*indexes)[i];
			}
		}
		first_half = merge_sort(first_half, n / 2, &first_half_indexes);
		second_half = merge_sort(second_half, n - n / 2, &second_half_indexes);
		T* sorted_mas = new T[n];
		int* sorted_indexes = new int[n];
		int i = 0, j = 0, k = 0;
		for (; i < n / 2 && j < n - n / 2; k++) {
			if (first_half[i] < second_half[j]) {
				sorted_mas[k] = first_half[i];
				sorted_indexes[k] = first_half_indexes[i];
				i++;
			}
			else {
				sorted_mas[k] = second_half[j];
				sorted_indexes[k] = second_half_indexes[j];
				j++;
			}
		}
		if (i == n / 2) {
			for (; j < n - n / 2; j++, k++) {
				sorted_mas[k] = second_half[j];
				sorted_indexes[k] = second_half_indexes[j];
			}
		}
		else if (j == n - n / 2) {
			for (; i < n / 2; i++, k++) {
				sorted_mas[k] = first_half[i];
				sorted_indexes[k] = first_half_indexes[i];
			}
		}
		if (indexes != NULL) {
			*indexes = sorted_indexes;
		}
		return sorted_mas;
	}
	else {
		return mas;
	}
}

int* generateIndexMas(int n) {
	int* mas = new int[n];
	for (int i = 0; i < n; i++) {
		mas[i] = i;
	}
	return mas;
}

int transformIndex(int source_index, int* index_list, int n) { //приведение индексов после удаления рёбер из матрицы смежности
	int new_index;
	new_index = -1;
	for (int i = 0; i < n; i++) {
		if (source_index == index_list[i]) {
			new_index = i;
		}
	}
	return new_index;
}

template<class T>
T* sub_mas(T* mas, int start, int end) { //start включается, end не включается
	T* s_mas = new T[end - start];
	for (int i = 0, j = start; j < end; i++, j++) {
		s_mas[i] = mas[j];
	}
	return s_mas;
}

int* decompose_nearest_points_search(float** points, int n, int** sort_indexes, float* d) {
	
	if (n > 2) {
		int Q_size = n / 2;
		//выделение 1 части
		int** Q_indexes = new int* [2];
		Q_indexes[0] = sub_mas(sort_indexes[0], 0, Q_size);
		Q_indexes[1] = sub_mas(sort_indexes[1], 0, Q_size);

		float Qd = numeric_limits<float>::max();
		int* Qq = decompose_nearest_points_search(points, Q_size, Q_indexes, &Qd);

		int R_size = n - Q_size;
		//выделение 2 части
		int** R_indexes = new int* [2];
		R_indexes[0] = sub_mas(sort_indexes[0], Q_size, n);
		R_indexes[1] = sub_mas(sort_indexes[1], Q_size, n);

		float Rd = numeric_limits<float>::max();
		int* Rq = decompose_nearest_points_search(points, R_size, R_indexes, &Rd);

		int* nearest = new int[2];
		if (Qd < Rd) {
			*d = Qd;
			nearest = Qq;
		}
		else {
			*d = Rd;
			nearest = Rq;
		}

		int s_min = 0, s_max = 0;
		for (int i = 0; i < n; i++) {
			if (points[sort_indexes[0][i]][0] > points[sort_indexes[0][Q_size - 1]][0] - *d) {
				s_min = i; 
				break;
			}
		}
		for (int i = n - 1; i > 0; i--) {
			if (points[sort_indexes[0][i]][0] < points[sort_indexes[0][Q_size - 1]][0] + *d) {
				s_max = i;
				break;
			}
		}
		int s_size = s_max - s_min;
		if (s_size > 0) { // <- на случай, если в исходном массиве точек попадутся две с одинаковыми координатами
			int* Sy_indexes = sub_mas(sort_indexes[1], s_min, s_max);
			Sy_indexes = merge_sort(Sy_indexes, s_size);
			for (int i = 0; i < s_size; i++) {
				for (int j = i + 1; j < i + 15 && j < s_size; j++) {
					float s_dist = distance(points[Sy_indexes[i]], points[Sy_indexes[j]]);

					if (s_dist < *d) {
						*d = s_dist;
						nearest[0] = Sy_indexes[i];
						nearest[1] = Sy_indexes[j];
					}
				}
			}
		}
		return nearest;
	}
	else if (n == 2) { 
		*d = distance(points[0], points[1]);
		return merge_sort(sort_indexes[0], n);
	}
	else {
		*d = numeric_limits<float>::max();
		return sort_indexes[0];
	}

}

int* decompose_nearest_points_search(float** points, int n) {
	int** indexes = new int*[2];
	indexes[0] = generateIndexMas(n);
	indexes[1] = generateIndexMas(n);
	float** transp_points = transp_matrix(points, n, 2);
	merge_sort(transp_points[0], n, &indexes[0]);
	merge_sort(transp_points[1], n, &indexes[1]);
	float d;
	return decompose_nearest_points_search(points, n, indexes, &d);
}

float** points = NULL;
bool* highlited = NULL;
int n = 0;

int program_menu() {
	cout << "1 Решение задачи для точек из файла input.txt" << endl;
	cout << "2 Решение задачи для случайно сгенерированных точек" << endl;
	cout << "3 Тестирование алгоритмов" << endl;
	cout << "4 Выход" << endl;
	cout << endl;

	int sel = 0;
	do {
		cout << "Введите номер пункта меню программы (1-4): ";
		cin >> sel;
		cout << endl;
	} while (!(1 <= sel <= 4));

	return sel;
}

template <class Type>
Type** matrix_from_file(string path, int* n) {
	ifstream in(path);
	string line;
	vector<vector<Type>> matrix_lines;
	if (in.is_open())
	{
		while (getline(in, line))
		{
			vector<Type> matrix_line;
			int last_space_pos = 0;
			int space_pos = line.find('\t', last_space_pos);
			do
			{
				string elem = line.substr(last_space_pos, space_pos != -1 ? space_pos - last_space_pos : line.length());
				matrix_line.push_back(elem != "-" ?
					is_same<Type, int>::value ? stoi(elem) :
					is_same<Type, float>::value ? stof(elem) :
					numeric_limits<Type>::max() - 100 :
					numeric_limits<Type>::max() - 100
				);
				last_space_pos = space_pos + 1;
				space_pos = line.find('\t', last_space_pos);
			} while (last_space_pos != 0);
			matrix_lines.push_back(matrix_line);
		}
		in.close();
		Type** matrix = new Type * [matrix_lines.size()];
		for (int i = 0; i < matrix_lines.size(); i++) {
			matrix[i] = new Type[matrix_lines.at(i).size()];
			for (int j = 0; j < matrix_lines.at(i).size(); j++) {
				matrix[i][j] = matrix_lines.at(i).at(j);
			}
		}
		*n = matrix_lines.size();
		return matrix;
	}
	return NULL;
}

void test_points_from_file(int argc, char** argv) {
	points = matrix_from_file<float>("input.txt", &n);
	print_matrix(points, n, 2);
	cout << endl;

	cout << "Метод грубой силы" << endl;

	float start = clock();
	int* nearest_points = simple_nearest_points_search(points, n);
	float end = clock();
	float seconds = (double)(end - start) / CLOCKS_PER_SEC;
	
	cout << "Близжайшие точки: (" << points[nearest_points[0]][0] << ", " << points[nearest_points[0]][1] << ") и ("
		<< points[nearest_points[1]][0] << ", " << points[nearest_points[1]][1] << ")" << endl;
	cout << "Расстояние: " << distance(points[nearest_points[0]], points[nearest_points[1]]) << endl ;
	cout << "Время: " << seconds << " секунд" << endl << endl;


	cout << "Метод декомпозиции" << endl;

	start = clock();
	nearest_points = decompose_nearest_points_search(points, n);
	end = clock();
	seconds = (double)(end - start) / CLOCKS_PER_SEC;

	cout << "Близжайшие точки: (" << points[nearest_points[0]][0] << ", " << points[nearest_points[0]][1] << ") и ("
		<< points[nearest_points[1]][0] << ", " << points[nearest_points[1]][1] << ")" << endl;
	cout << "Расстояние: " << distance(points[nearest_points[0]], points[nearest_points[1]]) << endl;
	cout << "Время: " << seconds << " секунд" << endl;

	highlited = new bool[2];
	for (int i = 0; i < n; i++) {
		highlited[i] = false;
	}
	highlited[nearest_points[0]] = true;
	highlited[nearest_points[1]] = true;
	glut_start(argc, argv);

}

void test_generated_points(int argc, char** argv) {
	points = generate_points(n);
	cout << "Метод грубой силы" << endl;

	float start = clock();
	int* nearest_points = simple_nearest_points_search(points, n);
	float end = clock();
	float seconds = (double)(end - start) / CLOCKS_PER_SEC;

	cout << "Близжайшие точки: (" << points[nearest_points[0]][0] << ", " << points[nearest_points[0]][1] << ") и ("
		<< points[nearest_points[1]][0] << ", " << points[nearest_points[1]][1] << ")" << endl;
	cout << "Расстояние: " << distance(points[nearest_points[0]], points[nearest_points[1]]) << endl;
	cout << "Время: " << seconds << " секунд" << endl << endl;
	
	cout << "Метод декомпозиции" << endl;

	start = clock();
	nearest_points = decompose_nearest_points_search(points, n);
	end = clock();
	seconds = (double)(end - start) / CLOCKS_PER_SEC;

	cout << "Близжайшие точки: (" << points[nearest_points[0]][0] << ", " << points[nearest_points[0]][1] << ") и ("
		<< points[nearest_points[1]][0] << ", " << points[nearest_points[1]][1] << ")" << endl;
	cout << "Расстояние: " << distance(points[nearest_points[0]], points[nearest_points[1]]) << endl;
	cout << "Время: " << seconds << " секунд" << endl;

	highlited = new bool[2];
	for (int i = 0; i < n; i++) {
		highlited[i] = false;
	}
	highlited[nearest_points[0]] = true;
	highlited[nearest_points[1]] = true;
	glut_start(argc, argv);

}

void nearest_points_search_algs_test(int test_number) {
	clock_t start, end, simple_method_time = 0, decompose_method_time = 0;

	
	for (int i = 0; i < test_number; i++) {
		srand(i);
		points = generate_points(n);

		start = clock();
		simple_nearest_points_search(points, n);
		end = clock();
		simple_method_time += end - start;

		start = clock();
		decompose_nearest_points_search(points, n);
		end = clock();
		decompose_method_time += end - start;
	}
	cout << "Средние показатели " << test_number << " тестов для " << n << " точек:" << endl;
	cout << "Время работы метода грубой силы: " << (double)simple_method_time / CLOCKS_PER_SEC << " seconds" << endl;
	cout << "Время работы метода декомпозиции: " << (double)decompose_method_time / CLOCKS_PER_SEC << " seconds" << endl;
}

int main(int argc, char** argv)
{

	setlocale(LC_ALL, "Russian");
	int sel = 4;
	while ((sel = program_menu()) != 4) {
		switch (sel) {
		case 1:
			test_points_from_file(argc, argv);
			break;

		case 2:
			cout << "Введите число точек: ";
			cin >> n;
			cout << endl;
			test_generated_points(argc, argv);

			break;

		case 3:
			cout << "Введите число точек: ";
			cin >> n;
			cout << endl;
			int test_num;
			cout << "Введите число тестов: ";
			cin >> test_num;
			cout << endl;
			nearest_points_search_algs_test(test_num);
		
			break;
		}
		cout << endl;
		system("pause");
		system("cls");
	}

	
}

void pointsDisplay() {
	glPointSize(4);

	glBegin(GL_POINTS);
	if (points != NULL && highlited != NULL) {
		for (int i = 0; i < n; i++) {
			if (!highlited[i]) {
				glColor3d(0, 0, 0);
			}
			else {
				glColor3d(1, 0, 0);
			}
			glVertex2f(points[i][0], points[i][1]);
		}
	}
	glEnd();

	glutPostRedisplay();
	glFlush();

}

void glut_start(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("Близжайшие точки - метод декомпозиции");
	//glutReshapeFunc(changeWindowSize);

	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0, 0, 0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0, 1, 1, 0);
	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
	glFlush();

	glutDisplayFunc(pointsDisplay);
	glutMainLoop();

	
}