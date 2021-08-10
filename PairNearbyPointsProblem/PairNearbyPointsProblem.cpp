#include <iostream>
#include <limits>
#include <glut.h>
#include <vector>

using namespace std;

void glutInit(int argc, char** argv);
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
		*indexes = sorted_indexes;
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

float** points = NULL;
bool* highlited = NULL;
int n = 0;

int main(int argc, char** argv)
{
	n = 10;
	points = generate_points(n);
	int* indexes = generateIndexMas(n);

	float* sorted_x = merge_sort(points[0], n, &indexes);
	print_mas(sorted_x, n);
	print_mas(indexes, n);

	highlited = new bool[n];
	for (int i = 0; i < n; i++) {
		highlited[i] = false;
	}
	int* nearest_indexes = simple_nearest_points_search(points, n);
	highlited[nearest_indexes[0]] = true;
	highlited[nearest_indexes[1]] = true;

	glutInit(argc, argv);

}

//const float aspect_ratio = 1;
//const int window_x = 640;
//const int window_y = 640;
//
//void changeWindowSize(int x, int y) {
//	float aspect = x / (float)y;
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	glViewport(0, 0, x, y);
//	if (aspect > aspect_ratio)  gluOrtho2D(0, window_x * aspect / aspect_ratio, window_y, 0);
//	if (aspect <= aspect_ratio)  gluOrtho2D(0, window_x, (window_y / aspect) * aspect_ratio, 0);
//
//	glMatrixMode(GL_MODELVIEW);
//
//}

void pointsDisplay() {
	glPointSize(5);

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

void glutInit(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("Nearest points");
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