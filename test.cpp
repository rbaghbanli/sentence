#include "sentence.h"
#include <stdio.h>
#include <iostream>

struct err_test { const char * expr; size_t idx; } err_tests[] =
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

struct num_test { const char * expr; double num; } num_tests[] =
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
	{ "\"abcd\"*~\"___abcd\"", 1.0 },
	{ "\"abcd\"~*\"abcd----\"", 1.0 },
	{ "\"abcd\"**\"___abcd----\"", 1.0 },
};

struct str_test { const char * expr; const char * str; } str_tests[] =
{
	{ "\"abc\"", "abc" },
	{ "\"abcd\"@2", "cd" },
	{ "\"abcdef\"$\"bcd\"", "bcdef" },
};

struct num_var_test { const char * expr; double num; double ret; } num_var_tests[] =
{
	{ "n", 1, 1 },
	{ "n = n", 2, 1 },
	{ "n > n", 3, 0 },
	{ "n+sqrt(n)/2", 4, 5 },
	{ "n >= 1,2,3", 6, 1 },
};

struct str_var_test { const char * expr; const char * str; const char * ret; } str_var_tests[] =
{
	{ "s", "a", "a" },
	{ "s@2", "abc", "c" },
	{ "s@s", "abc", "abc" },
};

int main(int argc, char *argv[])
{
	sentence s;
	std::string str;
	for (auto t : err_tests)
		printf("\nerror test %s - %s\n", t.expr, (s.parse(t.expr) - t.expr == t.idx) ? "SUCCESS" : "FAILURE");
	for (auto t : num_tests)
		printf("\nnumber test %s - %s\n", t.expr, (s.parse(t.expr) == NULL && s.evaluate() == t.num) ? "SUCCESS" : "FAILURE");
	for (auto t : str_tests)
		printf("\nstring test %s - %s\n", t.expr, (s.parse(t.expr) == NULL && strcmp(s.evaluate(str), t.str) == 0) ? "SUCCESS" : "FAILURE");
	for (auto t : num_var_tests)
		printf("\nstring test %s - %s\n", t.expr, (s.parse(t.expr) == NULL && s.set("n", t.num).evaluate() == t.ret) ? "SUCCESS" : "FAILURE");
	for (auto t : str_var_tests)
		printf("\nstring test %s - %s\n", t.expr, (s.parse(t.expr) == NULL && strcmp(s.set("s", t.str).evaluate(str), t.ret) == 0) ? "SUCCESS" : "FAILURE");
	printf("\npress enter to exit...");
	std::cin.get();
	return 0;

	//const char * expr = "x + y/z - sqrt(z) + 200/10 + pow(x,2) > 0 & _a1_ ~* \"any string\",\"abcdef\"";
	//const char * ret = s
	//	.set("x", 5)
	//	.set("y", 0.5)
	//	.set("z", 3)
	//	.set("_a1_", "abc")
	//	.parse(expr);
	//if (ret)
	//	printf("\nfailed to parse expression at %s\n", ret);
	//else
	//	printf("\nexpression %s\n\tis evaluated to %f\n", expr, s.num());
}
