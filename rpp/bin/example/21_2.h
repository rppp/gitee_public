/*
连续创建100个线程
本程序在解释运行时至少需要400MB内存
*/

main
{
	rbuf<void*> arr
	for i=1 to 50
		arr.push(rf.create_thr(&thread))
	A a
	for i=1 to 50
		arr.push(rf.create_thr(&A.thread,&a))
	for i in arr
		rf.wait_thr(arr[i])
		rf.CloseHandle(arr[i])
}

thread(void* param)
{
	puts 2
}

class A
{
	int m_a=2

	thread
	{
		puts m_a
	}
}