#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

enum State {
	START,
	DOT,
	CUT,
	USED,
	FINISH
};

struct Field {
	int size;
	char* start;
	int startX, startY;
	char** kv;
	State** state;
	Field(ifstream& in) {
		in >> size;
		start = NULL;
		kv = new char* [size];
		state = new State * [size];
		string str;
		for (int i = 0; i < size; i++) {
			kv[i] = new char[size];
			state[i] = new State[size];
			in >> str;
			cout << str << endl;
			for (int k = 0; k < size; k++) {
				char x = str[k];
				kv[i][k] = x;
				state[i][k] = DOT;
				if (x == '@') {
					if (!start) {
						start = &kv[i][k];
						state[i][k] = START;
						startX = i;
						startY = k;
					}
					else state[i][k] = FINISH;
				}
				else if (x == '#') state[i][k] = CUT;
			}
		}
	}
	int getStartY() {
		return startY;
	}
	int getStartX() {
		return startX;
	}
	bool check(int x, int y) {
		if ((x >= 0) && (y >= 0) && (x < size) && (y < size)) {
			if ((state[x][y] == DOT) || (state[x][y] == FINISH)) return true;
		}
		return false;
	}
	void showFull() {
		for (int i = 0; i < size; i++) {
			for (int k = 0; k < size; k++) 
				cout << kv[i][k];
			cout << endl;
		}
		for (int i = 0; i < size; i++) {
			for (int k = 0; k < size; k++)
				cout << state[i][k];
			cout << endl;
		}
	}
	void show(vector<pair<int, int>>& arr) {
		for (int i = 0; i < arr.size(); i++)
			state[arr[i].first][arr[i].second] = USED;
		for (int i = 0; i < size; i++) {
			for (int k = 0; k < size; k++) {
				if (state[i][k] == USED) cout << "@";
				else cout << kv[i][k];
			}
			cout << endl;
		}
	}
};

struct Result {
	vector<pair<int, int>> bufseq;
	vector<pair<int, int>> seq;
	int len;
	int buflen;
	bool success;
	Result() {
		len = 100000;
		buflen = 0;
		success = false;
	}
	Result operator=(Result other) {
		len = other.len;
		success = other.success;
		seq = other.seq;
		return other;
	}
	void update() {
		if (buflen < len) {
			success = true;
			seq.clear();
			for (int i = 0; i < buflen; i++)
				seq.push_back(bufseq[i]);
			len = buflen;
		}
	}
};

void dfs(Field& f, int x, int y, Result& outres) {
	//cout << "dfs " << x << " " << y << endl;
	static Result res;
	if (res.buflen >= res.len - 1) return;
	if (res.buflen < res.bufseq.size()) res.bufseq[res.buflen] = make_pair(x, y);
	else res.bufseq.push_back(make_pair(x, y));
	res.buflen++;
	if (f.state[x][y] == FINISH) {
		if (res.buflen < res.len) {
			cout << "dfs " << res.buflen << " " << res.len << endl;
			res.update();
			outres = res;
		}
	}
	else {
		bool isdot = (f.state[x][y] != START) && (f.state[x][y] != FINISH);
		if (isdot) f.state[x][y] = USED;
		vector<pair<int, int>> variant;
		if (f.check(x + 2, y + 1)) variant.push_back(make_pair(x + 2, y + 1));
		if (f.check(x + 2, y - 1)) variant.push_back(make_pair(x + 2, y - 1));
		if (f.check(x + 1, y + 2)) variant.push_back(make_pair(x + 1, y + 2));
		if (f.check(x - 1, y - 2)) variant.push_back(make_pair(x - 1, y - 2));
		if (f.check(x + 1, y - 2)) variant.push_back(make_pair(x + 1, y - 2));
		if (f.check(x - 1, y + 2)) variant.push_back(make_pair(x - 1, y + 2));
		if (f.check(x - 2, y - 1)) variant.push_back(make_pair(x - 2, y - 1));
		if (f.check(x - 2, y + 1)) variant.push_back(make_pair(x - 2, y + 1));
		for (int i = 0; i < variant.size(); i++) 
			dfs(f, variant[i].first, variant[i].second, outres);
		if (isdot) f.state[x][y] = DOT;
	}
	res.buflen--;
}

int main() {
	cout << "Enter the name of the file: ";
	string name;
	cin >> name;
	ifstream file(name.c_str());
	if (!file.is_open()) cout << "File is not opened!";
	Field field(file);
	file.close();
	int x = field.getStartX();
	int y = field.getStartY();
	Result res;
	dfs(field, x, y, res);
	cout << "dfs worked" << endl;
	if (res.success == true) {
		cout << "result:\n";
		field.show(res.seq);
		cout << "length: " << res.len - 1;
	}
	else cout << "\nimpossible";
	//field.showFull();
}
