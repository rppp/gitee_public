main
{
	putsl '123'
	putsl '123'.cstr

	s=\\abc'123
	putsl s
	s='abc\'123'
	putsl s

	putsl `a
	putsl r_char("a")
	putsl r_char('a')
	
	putsl 1_0000_0000
	putsl 100000000

	putsl 0xa
	putsl 0b1010
	putsl 10

	putsl 0.5
}