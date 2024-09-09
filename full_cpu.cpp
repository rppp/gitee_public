#include <iostream>
#include <thread>

using namespace std;

int fib(int n)
{ // fei
	if (n <= 1)
		return 1;
	return fib(n - 1) + fib(n - 2);
}

void f()
{
	for (int i = 0; i < 10000; i++)
		cout << fib(43) << endl;
}

int main()
{
	for (int i = 0; i < 10; i++)
	{
		thread t(f);
		t.detach();
	}

	while (1)
		;
	return 0;
}
