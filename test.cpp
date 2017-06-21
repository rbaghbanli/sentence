#include "sentence.h"
#include <stdio.h>
#include <iostream>

struct { const char * expr; size_t idx; } err_tests[] =
{
	{ "", 0 },
	{ "(", 0 },
	{ ")", 0 },
	{ "()", 1 },
	{ "(a", 1 },
	{ "a)", 1 },
	{ "a()", 1 },
	{ "a(20)", 1 },
	{ "a//b", 2 },
	{ "\"a\"#", 3 },
	{ "\"aa\"*", 4 },
	{ "\"aa\"&", 4 },
	{ "sqrt(4", 5 },
	{ "-1*(2+3", 6 },
};

struct { const char * expr; double num; } num_tests[] =
{
	{ "-1", -1.0 },
	{ "0", 0.0 },
	{ "1", 1.0 },
	{ "2 ", 2.0 },
	{ "--3", 3.0 },
	{ "3+4", 3.0 + 4 },
	{ "((5)+(6))", 5.0 + 6 },
	{ "1+2+3+4", 1.0 + 2 + 3 + 4 },
	{ "1*2*3/4", 1.0 * 2 * 3 / 4 },
	{ "1+(2+3)+4", 1.0 + 2 + 3 + 4 },
	{ "1+(2+3)*4", 1.0 + (2 + 3) * 4 },
	{ "+1-(2-3)/4", 1.0 - (2.0 - 3.0) / 4.0 },
	{ "1*2*3>-456", 1.0 },
	{ "1*2*3<-456", 0.0 },
	{ "sqrt(4)", 2.0 },
	{ "#\"abcd\"", 4.0 },
	{ "pow(1*2,2)", 4.0 },
	{ "\"ab\"~\"abcd\"", 0.0 },
	{ "\"abcd\"~\"abcd\"", 1.0 },
	{ "\"abcd\"**\"abc----\"", 0.0 },
	{ "\"abcd\"*~\"_abcdef__abcd\"", 1.0 },
	{ "\"abcd\"~*\"abcd----abcd\"", 1.0 },
	{ "\"abcd\"**\"___abcd----abc+++abcd\"", 1.0 },
};

struct { const char * expr; const char * str; } str_tests[] =
{
	{ "\"abc\"", "abc" },
	{ "\"abcd\"@2", "cd" },
	{ "\"abcdef\"$\"bcd\"", "bcdef" },
};

struct { const char * expr; double num; double ret; } num_n_tests[] =
{
	{ "n", 1, 1 },
	{ "n = n", 2, 1 },
	{ "n > n", 3, 0 },
	{ "n+sqrt(n)/2", 4, 5 },
	{ "n >= 1,2,3", 6, 1 },
};

struct { const char * expr; const char * str; double ret; } num_s_tests[] =
{
	{ "#s", "a", 1 },
	{ "#(s@2)", "abcd", 2 },
	{ "#\"abcd\" < #s", "abcde", 1 },
	{ "#(s$\"abcdef\") / 6 > 10", "+-abcdef", 0 },
};

struct { const char * expr; const char * str; const char * ret; } str_s_tests[] =
{
	{ "s", "a", "a" },
	{ "s@2", "abc", "c" },
	{ "\"aaaabc\"$s$\"abc\"", "abc", "abc" },
};

struct { const char * expr; double n1; double n2; const char * str; double ret; } num_nns_tests[] =
{
	{ "-sqrt(n1) + (n1+n2)/2 * #s", 4, 2, "a", 1 },
	{ "-n1 - n2*#s + pow(n1, 2) / 2 + 2 = 0", 4, 2, "abc", 1 },
};

int main(int argc, char *argv[])
{
	sentence s;
	std::string str;

	for (auto t : err_tests)
		printf("\nparse error test %s - %s\n", t.expr, (s.parse(t.expr) - t.expr == t.idx) ? "SUCCESS" : "FAILURE");

	for (auto t : num_tests)
		printf("\nnumber test %s - %s\n", t.expr, (s.parse(t.expr) == NULL && s.evaluate() == t.num) ? "SUCCESS" : "FAILURE");

	for (auto t : str_tests)
		printf("\nstring test %s - %s\n", t.expr, (s.parse(t.expr) == NULL && strcmp(s.evaluate(str), t.str) == 0) ? "SUCCESS" : "FAILURE");

	for (auto t : num_n_tests)
		printf("\nnumber from number variable test %s - %s\n", t.expr, (s.parse(t.expr) == NULL && s.set("n", t.num).evaluate() == t.ret) ? "SUCCESS" : "FAILURE");

	for (auto t : str_s_tests)
		printf("\nstring from string variable test %s - %s\n", t.expr, (s.parse(t.expr) == NULL && strcmp(s.set("s", t.str).evaluate(str), t.ret) == 0) ? "SUCCESS" : "FAILURE");

	s.reset();

	for (auto t : num_nns_tests)
		printf("\nnumber from number+number+string variables test %s - %s\n", t.expr, (s.parse(t.expr) == NULL && s.set("n1", t.n1).set("n2", t.n2).set("s", t.str).evaluate() == t.ret) ? "SUCCESS" : "FAILURE");

	printf("\npress enter to exit...");
	std::cin.get();
	return 0;
}
