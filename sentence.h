#ifndef __SENTENCE_H__
#define __SENTENCE_H__

#include <cstdlib>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <vector>

struct variant
{
	virtual double num() const = 0;
	virtual const char * str() const = 0;
};

typedef double (num_function)(const std::vector<variant *> &);
typedef const char * (str_function)(const std::vector<variant *> &);

struct sentence
{
	sentence();
	virtual ~sentence();

	sentence & add(const char * fn, num_function * fp, int a); // add custom function
	sentence & add(const char * fn, str_function * fp, int a); // add custom function
	sentence & define(const char * cn, double n); // define custom constant
	sentence & define(const char * cn, const char * s); // define custom constant
	sentence & set(const char * vn, double n); // set variable
	sentence & set(const char * vn, const char * s); // set variable
	sentence & reset(); // reset parser and variables
	const char* parse(const char * expr); // parse expression string, return pointer to error on failure, NULL on success
	double evaluate() const; // evaluate result as a number
	const char * evaluate(std::string & ret) const; // evaluate result as a string

protected:

	enum { T_ERROR = -1, T_END = 0, T_SEP, T_OPEN, T_CLOSE, T_NUM, T_STR, T_NUM_OPER, T_STR_OPER, T_NUM_FUNC, T_STR_FUNC, T_VAR };

	struct variant_value : public variant
	{
		variant_value();
		variant_value(double n);
		variant_value(const char * s);
		double num() const override;
		const char * str() const override;
		int _type;
		union { double _num; const char * _str; };
	};

	struct variant_function
	{
		variant_function(int t, int a, num_function * f);
		variant_function(int t, int a, str_function * f);
		int _type;
		int _arity;
		union { num_function * _num_func; str_function * _str_func; };
	};

	struct node : public variant
	{
		node();
		node(double n);
		node(const char * s);
		node(num_function * f, node * n1 = nullptr, node * n2 = nullptr);
		node(str_function * f, node * n1 = nullptr, node * n2 = nullptr);
		node(variant_value * v);
		virtual ~node();
		double num() const override;
		const char * str() const override;
		int arity() const;
		void add(node * n);
		void trim();

	private:

		int _type;
		union { double _num; const char * _str; num_function * _num_func; str_function * _str_func; variant_value * _var; };
		std::vector<variant *> _vec;
	};

	struct state
	{
		const char *_token;
		const char *_next;
		int _type;
		int _arity;
		union { double _num; const char * _str; num_function * _num_func; str_function * _str_func; variant_value * _var; };
	};

	struct identifier_comparator : public std::binary_function<const char*, const char*, bool> { bool operator() (const char* s1, const char* s2) const; };

	std::shared_ptr<node> _root;
	std::map<const char *, variant_value, identifier_comparator> _const_map;
	std::map<const char *, variant_function, identifier_comparator> _func_map;
	std::map<const char *, std::shared_ptr<variant_value>, identifier_comparator> _var_map;
	std::vector<std::shared_ptr<std::vector<char>>> _str_vec;

	const char * intern(const char * begin, const char * end);
	variant_value * variable(const char* name);
	state & next(state & s);
	node * disj(state & s);
	node * conj(state & s);
	node * word(state & s);
	node * comp(state & s);
	node * term(state & s);
	node * fact(state & s);
	node * text(state & s);
	node * atom(state & s);
};

#endif // __SENTENCE_H__
