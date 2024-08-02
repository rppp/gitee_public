extern "C" int printf(const char* f,...);

int fib(int n) {
	if (n <= 1) {
		return 1;
	}
	return fib(n - 1) + fib(n - 2);
}

int main() {
	printf("%d\n", (int)sizeof(void*));
	printf("%d\n", fib(43));
	return 0;
}
