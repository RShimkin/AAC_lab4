// Шимкин Р.Р. 62 группа ПММ 2 курс Лаба4 (поиск в ширину, "ход коня")
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <queue>

using namespace std;

// возможные состояния поля (старт, пуста, вырезана, посещена, финиш)
enum State {
	START,
	DOT,
	CUT,
	USED,
	FINISH
};

// структура узла (клетка)
struct Node {
	char sign;          // знак клетки
	pair<int, int> pos; // координаты клетки
	Node* prev;         // указатель на предыдущую клетку (откуда пришли)
	State state;        // состояние клетки
	vector<Node*> neighbors;    // массив соседей (куда можно пойти, исключая выход за границы и вырезанные
	int next; // номер следующего соседа
	// изначально задаем знак (точка), коорд-ты, сост-е (пусто) и предка (null)
	Node(int a, int b, char s = '.', State st = DOT, Node* pr = NULL) {
		sign = s;
		pos.first = a;
		pos.second = b;
		state = st;
		prev = pr;
		next = 0;
	}
	Node() {
		sign = '.';
		pos.first = -1;
		pos.second = -1;
		state = DOT;
		prev = NULL;
		next = 0;
	}
	// если массив соседей не кончился, возвращаем следующего
	Node* getNextNeighbor() {
		Node* res = NULL;
		if (next < neighbors.size())
			res = neighbors[next++];
		return res;
	}
};

// основная структура поля
struct Field {
	int size;
	Node* start;
	Node* finish;
	Node** square;   // матрица клеток
	Field(ifstream& in) {
		in >> size;
		start = NULL;
		square = new Node* [size];
		string str;
		for (int i = 0; i < size; i++) {
			square[i] = new Node[size];
			in >> str;
			cout << str << endl;
			for (int k = 0; k < size; k++) {
				// переопределяем поле
				char x = str[k];
				square[i][k].pos.first = i;
				square[i][k].pos.second = k;
				square[i][k].sign = x;
				// встретив @, определяем либо старт, либо финиш
				if (x == '@') {
					if (!start) {
						start = &square[i][k];
						square[i][k].state = START;
					}
					else {
						square[i][k].state = FINISH;
						finish = &square[i][k];
					}
				}
				// встретив #, определяем вырезанное состояние
				else if (x == '#') square[i][k].state = CUT;
			}
		}
		// устанавливаем соседей для невырезанных клеток
		for (int i = 0; i < size; i++) {
			for (int k = 0; k < size; k++) {
				if (square[i][k].state != CUT) {
					Node* cur = &square[i][k];
					setNeighbors(cur);
				}
			}
		}
	}
	// функция заполнения массива соседей
	void setNeighbors(Node* sq) {
		// координаты данной клетки
		int x = sq->pos.first;
		int y = sq->pos.second;
		// проверяем 8 возможных ходов на выход за границы и CUT 
		if (x + 2 < size) {
			if ((y + 1 < size) && (square[x + 2][y + 1].state != CUT))
				sq->neighbors.push_back(&square[x + 2][y + 1]);
			if ((y - 1 >= 0) && (square[x + 2][y - 1].state != CUT))
				sq->neighbors.push_back(&square[x + 2][y - 1]);
		} 
		if (x + 1 < size) {
			if ((y + 2 < size) && (square[x + 1][y + 2].state != CUT))
				sq->neighbors.push_back(&square[x + 1][y + 2]);
			if ((y - 2 >= 0) && (square[x + 1][y - 2].state != CUT))
				sq->neighbors.push_back(&square[x + 1][y - 2]);
		}
		if (x - 1 >= 0) {
			if ((y + 2 < size) && (square[x - 1][y + 2].state != CUT))
				sq->neighbors.push_back(&square[x - 1][y + 2]);
			if ((y - 2 >= 0) && (square[x - 1][y - 2].state != CUT))
				sq->neighbors.push_back(&square[x - 1][y - 2]);
		}
		if (x - 2 >= 0) {
			if ((y + 1 < size) && (square[x - 2][y + 1].state != CUT))
				sq->neighbors.push_back(&square[x - 2][y + 1]);
			if ((y - 1 >= 0) && (square[x - 2][y - 1].state != CUT))
				sq->neighbors.push_back(&square[x - 2][y - 1]);
		}
	}
	// функция отображения поля
	void show() {
		// отрисовываем путь в обратном порядке (если путь был найден)
		Node* p = finish->prev;
		while (p) {
			p->sign = '@';
			p = p->prev;
		}
		// вывод
		for (int i = 0; i < size; i++) {
			for (int k = 0; k < size; k++)
				cout << square[i][k].sign;
			cout << endl;
		}
	}
	// основная функция алгоритма
	void bfs() {
		queue<Node*> qu;                        // очередь непройденных вершин
		qu.push(start);                         // помещаем в очередь стартовую клетку
		bool found = false;                     // флаг успеха поиска
		// пока очередь не пуста и не нашли
		while (!qu.empty() && !found) {
			// достаем из очереди текущий элемент
			Node* cur = qu.front();
			qu.pop();
			// если это финиш, то поиск окончен
			if (cur->state == FINISH) 
				found = true;
			else {
				// иначе помечаем вершины посещенной
				cur->state = USED;
				// помещаем в очередь соседей текущей вершины (если не вырезаны)
				Node* next = cur->getNextNeighbor();
				while (next) {
					if (next->state != USED) {
						qu.push(next);
						next->prev = cur;
					}
					next = cur->getNextNeighbor();
				}
			}
		}
		// вывод результата
		cout << "result: " << endl;
		if (!found) cout << "impossible" << endl;
		else show();
	}
};

int main() {
	cout << "Enter the name of the file: ";
	string name;
	cin >> name;
	ifstream file(name.c_str());
	if (!file.is_open()) cout << "File is not opened!";
	Field field(file);
	file.close();
	field.bfs();
}