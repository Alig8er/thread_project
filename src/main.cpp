#include <iostream>
#include <thread>
#include <mutex>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <sstream>
using namespace std;

const int N = 1000; // # of integers in file
const int SIZE = 100; // # of ints read at a time

void read_file(mutex &m, FILE *f) {
	vector<int> buff(SIZE);
	int cnt = 0;
	while (true) {
		//cout << this_thread::get_id() << endl;
		m.lock();
		size_t M = fread(&buff[0], sizeof(buff[0]), SIZE, f);
		m.unlock();

		if (M < SIZE) {
			buff.resize(M);
			break;
		}
		/*
		 printf("read back: ");
		 for (size_t i = 0; i < M; i++)
		 printf("%d ", buff[i]);
		 cout << endl;
		 */

		string new_file_name;
		stringstream ss, ss2;
		ss << this_thread::get_id();
		ss2 << cnt;
		new_file_name = "bin/"+ss.str() + "_" + ss2.str();
		++cnt;

		cout << "FILE: " << new_file_name << endl;

		FILE *f1 = fopen(new_file_name.c_str(), "wb");
		assert(f1);
		fwrite(&buff[0], sizeof(buff[0]), N, f1);
		fclose(f1);

	}
}

int main() {
	srand(time(NULL));

	int a[N];
	for (int i = 0; i < N; ++i) {
		//a[i] = i + 1;
		a[i] = rand() % 100;
	}

	FILE *f1 = fopen("file.txt", "wb");
	assert(f1);
	size_t r1 = fwrite(a, sizeof a[0], N, f1);
	fclose(f1);

	FILE *f2 = fopen("file.txt", "rb");

	std::mutex m;

	std::cout << "starting first helper...\n";
	std::thread t1(read_file, ref(m), f2);

	std::cout << "starting second helper...\n";
	std::thread t2(read_file, ref(m), f2);

	/*
	 std::cout << "starting third helper...\n";
	 std::thread t3(read_file, ref(m), f2);

	 std::cout << "starting third helper...\n";
	 std::thread t4(read_file, ref(m), f2);
	 */

	std::cout << "waiting for helpers to finish..." << std::endl;
	t1.join();
	t2.join();
	/*
	 t3.join();
	 t4.join();
	 */

	std::cout << "done!\n";
	fclose(f2);
	return 0;
}

