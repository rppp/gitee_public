/*
函数指针不需要声明即可使用，
参数可以由编译器自动推断出类型
*/

main
{
	putsl int[&fadd,1,2]
	
	p=&fadd
	putsl int[p,1,2]

	putsl int[&fadd(int,int),1,2]

	putsl int[&main.fadd(int,int),1,2]
}

int fadd(int a,int b)
{
	return a+b
}