/*
RPP是强类型语言，没有强制转换，
可以通过函数转换类型
*/

main
{
	a=2.touint
	putsl typeof(a)

	a=2.to<uint>
	putsl typeof(a)
	
	b=uint(2)
	putsl typeof(b)
	
	c=2.torstr
	putsl typeof(c)
	
	d="123".toint
	putsl typeof(d)
	
	char ch=`a
	putsl ch.toint

	char ch=-1
	putsl ch.toint
}