# Sentence

Compact recursive descent expression parser, and evaluation engine 
for closed-form analytic expressions.

Sentence supports boolean expressions, regular algebraic expressions, 
numeric and string functions and comparsions.
Expression parsing follows the established operator notation and precedence. 
Boolean values represented by numbers 0 (false) and 1 (true).

## Why

* Parse once, execute multiple times
* Efficient expression evaluation
* Fully encapsulated logic is easy to integrate into the project
* Boolean and arithmetic operators supported
* Numeric and string comparision operators supported
* Variadic functions and operators supported
* Standard math functions included
* Easy to add custom functions or constants
* All operators support literal equivalent

## What

#### Arithmetic operations
unary: negation (-, "sub");
binary: addition (+, "add"), subtraction (-, "sub"),
 multiplication (\*, "mul"), division (/, "div"), modulus (%, "mod")
#### String operations
unary: string length (\#, "len");
binary: substring at position (@, "pos"), substring at first occurence ($, "first")
#### Numeric comparisions
binary: greater than (>, "gt"), less than (<, "lt"),
 greater than or equal to (>=, "ge"), less than or equal to (<=, "le");
variadic: equal to (=, "eq"), not equal to (!=, "ne")
#### String comparisions
variadic: match (\~, "match"), mismatch (!\~, "mismatch"),
 begin of (\~\*, "begin"), end of (\*\~, "end"), part of (\*\*, "part")
#### Boolean operators
unary: negation (!, "not");
binary: disjunction (|, "or"), conjunction (&, "and")
#### Arithmetic functions
unary: abs, ceil, floor, fac, logn, logd;
binary: pow, exp;
variadic: min, max
#### Numeric constants
e, pi

The expression parsing is performed using the following grammar:

	<disj> = <conj>{"|"<conj>}
	<conj> = <word>{"&"<word>}
	<word> = {"!"}<comp>{("="|"!="|">"|">="|"<"|"<=")<comp>{","<comp>}}
	<comp> = <term>{("+"|"-")<term>}
	<term> = <fact>{("*"|"/"|"%")<fact>}
	<fact> = {("-"|"+")}{"#"}<text>{(|"~"|"~*"|"*~"|"**")<text>{","<text>}}
	<text> = <atom>{("@"|"$")<atom>
	<atom> = (<number>|<string>|<variable>|<function>"("<disj>{","<disj>}")")

Whitespace characters are ignored.

Valid variable or function names consist of a letter or underscore character followed by any combination
of letters, digits, and underscore character. For example *x*, *_a1*, *_25*

Constants can be strings, integers, or decimal numbers.
Scientific notation is supported, for example '1e2' for '100').

## How

Use method *parse* to parse expression string.
If successful method returns NULL,
otherwise it returns pointer to the position in the string where parser stopped.

Use method *set* to set variable value. It returns reference to the *sentence* object for easy chaining.

Use methods *num* or *str* to evaluate expression as a number or a string accordingly.

Method *set* can be used before or after the *parse*.
During the parsing any alphanumeric sequence not identified as
number value, string value, operator, or function name is assumed to be variable.
Any variable with value not set prior to evaluation is considered to have value NAN or NULL
depending on the context of the evaluation.

Sample expressions:

	x+y/z-sqrt(z)+25
	"abcdef"
	-pow(x+4,5-y) > 11-fac(6)/150 & x!=1,2,3
	a1 *~ "my string","her string","his string" & x > 9
	max(x,y) > z & (_str ~ "abc","def" | _str ** "ghijklmn")

Sample code:

```C++
#include "sentence.h"

sentence s; // create an object of the class sentence
s.parse("x+y*z-25);
s.set("x", 1.5); // set the variable value
s.set("y", 20);
s.set("z", 500);
double result = s.evaluate(); // get result of the evaluation
double another_result = s.set("x", 1000.05).set("y", 25.116).set("z", 10).evaluate(); // get another result
```

More examples:
	
```C++
#include "sentence.h"

sentence s; // create an object of the class sentence
s.add("my_func" /* function identifier */, my_function /* address */, 2 /* arity */) // to add function
 .define("my_const" /* constant identifier */, MY_CONST /* value */) // to define constant
 .set("a", 1.5) // .set the variable value
 .set("b", 20)
 .set("_c", 5)
 .set("_d1", 0.0001)
 .set("_d2", 12)
 .set("extra", 5) // unused variables are ignored
 .parse("my_const - a+b < sqrt(_c*_d1) or a=10,12,14 & b<-20+my_func(_d2+2)");
double result = s.evaluate(); // get result of the evaluation
double another_result = s.set("a", 1000.05).set("b", 25.116).evaluate(); // get another result
```
