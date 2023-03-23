#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <iostream>

#define THREAD_NUMS 50
#define LOOP_NUMS 1000000

struct Aue_demoGameModeBase
{
	volatile int counter; 
	std::mutex mtx;  
	std::atomic<int> atomic_counter;

	static void attempt_10k_increases(Aue_demoGameModeBase* p) {
		for (int i=0; i<LOOP_NUMS; ++i) {
			p->atomic_counter++;
			p->mtx.lock();
			++p->counter;
			p->mtx.unlock();
		}
	}

	void test_mutex() {
		counter = 0;
		atomic_counter = 0;
		std::thread threads[THREAD_NUMS];
		for (int i=0; i<THREAD_NUMS; ++i){
			 threads[i] = std::thread(attempt_10k_increases, this);
		}
		for (auto& th : threads){
			th.join();
		} 
		std::cout << counter << " successful increases of the counter.\n";
		std::cout << atomic_counter << std::endl;
	}
	
	void test_f_point(void (*p)(int), int n){
		p(n);
	}
	
	static void print_int(int n){
		printf("%d\n", n);
	}
	
	Aue_demoGameModeBase(){
		test_mutex();
		
		test_f_point(print_int, 2);
		test_f_point(print_int, 3);

		//FScopeLock Lock(&m_mutex);
		//printf("in lock\n");
	}
};

int main(){
	Aue_demoGameModeBase a;
	
	return 0;
}
