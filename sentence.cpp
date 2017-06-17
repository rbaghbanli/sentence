#include <algorithm>
#include <utility>
#include "sentence.h"

const double TRUE_NUM = 1.0;
const double FALSE_NUM = 0.0;
const double VOID_NUM = std::numeric_limits<double>::quiet_NaN();
const char * VOID_STR = NULL;

static inline bool alpha(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
}

static inline bool numeric(char c)
{
	return (c >= '0' && c <= '9') || (c == '.');
}

static inline bool alphanumeric(char c)
{
	return alpha(c) || numeric(c);
}

static inline bool character(char c)
{
	return c && c != '\"';
}

#pragma warning(push)
#pragma warning(disable: 4706)

int compare_identifiers(const char * s1, const char * s2)
{
	int diff = 0;
	bool c1 = c1 = alpha(*s1), c2 = alpha(*s2);
	while (c1 && c2 && !(diff = *(unsigned char *)s1 - *(unsigned char *)s2))
	{
		c1 = alphanumeric(*++s1); c2 = alphanumeric(*++s2);
	}
	if (c1 && c2)
		return diff;
	if (c1 != c2)
		return c1 ? 1 : -1;
	return 0;
}

int compare_strings(const char * s1, const char * s2)
{
	int diff = 0;
	bool c1 = character(*s1), c2 = character(*s2);
	while (c1 && c2 && !(diff = *(unsigned char *)s1 - *(unsigned char *)s2))
	{
		c1 = character(*++s1); c2 = character(*++s2);
	}
	if (c1 && c2)
		return diff;
	if (c1 != c2)
		return c1 ? 1 : -1;
	return 0;
}

bool string_begins(const char * s1, const char * s2)
{
	const char * w1 = s1;
	const char * w2 = s2;
	while (character(*w1) && character(*w2) && *w1 == *w2)
		++w1, ++w2;
	if (!character(*w2))
		return true;
	return false;
}

bool string_ends(const char * s1, const char * s2)
{
	const char *str1 = s1;
	while (character(*str1))
	{
		const char * w1 = str1;
		const char * w2 = s2;
		while (character(*w1) && character(*w2) && *w1 == *w2)
			++w1, ++w2;
		if (!character(*w2))
			return !character(*w1);
		++str1;
	}
	return false;
}

size_t size_string(const char * s)
{
	const char * w = s;
	while (character(*w))
		++w;
	return (size_t)(w - s);
}

int find_string(const char * s1, const char * s2)
{
	const char * str1 = s1;
	while (character(*str1))
	{
		const char * w1 = str1;
		const char * w2 = s2;
		while (character(*w1) && character(*w2) && *w1 == *w2)
			++w1, ++w2;
		if (!character(*w2))
			return str1 - s1;
		++str1;
	}
	return -1;
}

#pragma warning(pop)

static double func_or(const std::vector<variant *> & vec)
{
	double n = vec[0]->num(), n1 = vec[1]->num();
	return n == VOID_NUM || n1 == VOID_NUM ? VOID_NUM : n != FALSE_NUM || n1 != FALSE_NUM ? TRUE_NUM : FALSE_NUM;
}

static double func_and(const std::vector<variant *> & vec)
{
	double n = vec[0]->num(), n1 = vec[1]->num();
	return n == VOID_NUM || n1 == VOID_NUM ? VOID_NUM : n != FALSE_NUM && n1 != FALSE_NUM ? TRUE_NUM : FALSE_NUM;
}

static double func_not(const std::vector<variant *> & vec)
{
	double n = vec[0]->num();
	return n == VOID_NUM ? VOID_NUM : n == FALSE_NUM ? TRUE_NUM : FALSE_NUM;
}

static double func_eq(const std::vector<variant *> & vec)
{
	double n = vec[0]->num();
	if (n == VOID_NUM)
		return VOID_NUM;
	for (size_t i = 1; i < vec.size(); ++i)
	{
		double ni = vec[i]->num();
		if (ni == VOID_NUM)
			return VOID_NUM;
		if (n == ni)
			return TRUE_NUM;
	}
	return FALSE_NUM;
}

static double func_ne(const std::vector<variant *> & vec)
{
	double n = vec[0]->num();
	if (n == VOID_NUM)
		return VOID_NUM;
	for (size_t i = 1; i < vec.size(); ++i)
	{
		double ni = vec[i]->num();
		if (ni == VOID_NUM)
			return VOID_NUM;
		if (n == ni)
			return FALSE_NUM;
	}
	return TRUE_NUM;
}

static double func_gt(const std::vector<variant *> & vec)
{
	double n = vec[0]->num(), n1 = vec[1]->num();
	return n == VOID_NUM ? VOID_NUM : n > n1 ? TRUE_NUM : FALSE_NUM;
}

static double func_lt(const std::vector<variant *> & vec)
{
	double n = vec[0]->num(), n1 = vec[1]->num();
	return n == VOID_NUM ? VOID_NUM : n < n1 ? TRUE_NUM : FALSE_NUM;
}

static double func_ge(const std::vector<variant *> & vec)
{
	double n = vec[0]->num(), n1 = vec[1]->num();
	return n == VOID_NUM ? VOID_NUM : n >= n1 ? TRUE_NUM : FALSE_NUM;
}

static double func_le(const std::vector<variant *> & vec)
{
	double n = vec[0]->num(), n1 = vec[1]->num();
	return n == VOID_NUM ? VOID_NUM : n <= n1 ? TRUE_NUM : FALSE_NUM;
}

static double func_match(const std::vector<variant *> & vec)
{
	const char * s = vec[0]->str();
	if (s == VOID_STR)
		return VOID_NUM;
	for (size_t i = 1; i < vec.size(); ++i)
	{
		const char * si = vec[i]->str();
		if (s == VOID_STR)
			return VOID_NUM;
		else
			if (!compare_strings(s, si))
				return TRUE_NUM;
	}
	return FALSE_NUM;
}

static double func_mismatch(const std::vector<variant *> & vec)
{
	const char * s = vec[0]->str();
	if (s == VOID_STR)
		return VOID_NUM;
	for (size_t i = 1; i < vec.size(); ++i)
	{
		const char * si = vec[i]->str();
		if (s == VOID_STR)
			return VOID_NUM;
		else
			if (!compare_strings(s, si))
				return FALSE_NUM;
	}
	return TRUE_NUM;
}

static double func_begin(const std::vector<variant *> & vec)
{
	const char * s = vec[0]->str();
	if (s == VOID_STR)
		return VOID_NUM;
	for (size_t i = 1; i < vec.size(); ++i)
	{
		const char * si = vec[i]->str();
		if (s == VOID_STR)
			return VOID_NUM;
		else
			if (string_begins(si, s))
				return TRUE_NUM;
	}
	return FALSE_NUM;
}

static double func_end(const std::vector<variant *> & vec)
{
	const char * s = vec[0]->str();
	if (s == VOID_STR)
		return VOID_NUM;
	for (size_t i = 1; i < vec.size(); ++i)
	{
		const char * si = vec[i]->str();
		if (s == VOID_STR)
			return VOID_NUM;
		else
			if (string_ends(si, s))
				return TRUE_NUM;
	}
	return FALSE_NUM;
}

static double func_part(const std::vector<variant *> & vec)
{
	const char * s = vec[0]->str();
	if (s == VOID_STR)
		return VOID_NUM;
	for (size_t i = 1; i < vec.size(); ++i)
	{
		const char * si = vec[i]->str();
		if (!si)
			return VOID_NUM;
		else
			if (find_string(si, s) >= 0)
				return TRUE_NUM;
	}
	return FALSE_NUM;
}

static double func_add(const std::vector<variant *> & vec) { return vec[0]->num() + vec[1]->num(); }

static double func_sub(const std::vector<variant *> & vec) { return vec[0]->num() - vec[1]->num(); }

static double func_neg(const std::vector<variant *> & vec) { return -vec[0]->num(); }

static double func_mul(const std::vector<variant *> & vec) { return vec[0]->num() * vec[1]->num(); }

static double func_div(const std::vector<variant *> & vec) { return vec[0]->num() / vec[1]->num(); }

static double func_mod(const std::vector<variant *> & vec) { return fmod(vec[0]->num(), vec[1]->num()); }

static const char * func_pos(const std::vector<variant *> & vec)
{
	const char * s = vec[0]->str();
	double n = vec[1]->num();
	return s == VOID_STR || n == VOID_NUM || n >= size_string(s) ? VOID_STR : (s + (size_t)n);
}

static const char * func_first(const std::vector<variant *> & vec)
{
	const char * s = vec[0]->str(), *s1 = vec[1]->str();
	if (s == VOID_STR || s1 == VOID_STR)
		return VOID_STR;
	int pos = find_string(s, s1);
	return pos < 0 ? VOID_STR : (s + (size_t)pos);
}

static double func_len(const std::vector<variant *> & vec)
{
	const char * s = vec[0]->str();
	return s == VOID_STR ? VOID_NUM : size_string(s);
}

static double func_abs(const std::vector<variant *> & vec) { return fabs(vec[0]->num()); }

static double func_exp(const std::vector<variant *> & vec) { return exp(vec[0]->num()); }

static double func_logd(const std::vector<variant *> & vec) { return log10(vec[0]->num()); }

static double func_logn(const std::vector<variant *> & vec) { return log(vec[0]->num()); }

static double func_fac(const std::vector<variant *> & vec)
{
	double n = vec[0]->num();
	if (n == VOID_NUM || n < 0.0 || n > 160)
		return VOID_NUM;
	unsigned int un = (unsigned int)(n);
	double result = 1;
	for (unsigned int i = 1; i <= un; i++)
		result *= (double)i;
	return result;
}

static double func_max(const std::vector<variant *> & vec)
{
	double n = vec[0]->num();
	if (n == VOID_NUM)
		return VOID_NUM;
	for (size_t i = 1; i < vec.size(); ++i)
	{
		double ni = vec[i]->num();
		if (ni == VOID_NUM)
			return VOID_NUM;
		if (n < ni)
			n = ni;
	}
	return n;
}

static double func_min(const std::vector<variant *> & vec)
{
	double n = vec[0]->num();
	if (n == VOID_NUM)
		return VOID_NUM;
	for (size_t i = 1; i < vec.size(); ++i)
	{
		double ni = vec[i]->num();
		if (ni == VOID_NUM)
			return VOID_NUM;
		if (n > ni)
			n = ni;
	}
	return n;
}

static double func_pow(const std::vector<variant *> & vec)
{
	double n = vec[0]->num(), n1 = vec[1]->num();
	return n == VOID_NUM || n1 == VOID_NUM ? VOID_NUM : pow(n, n1);
}

static double func_root(const std::vector<variant *> & vec)
{
	double n = vec[0]->num(), n1 = vec[1]->num();
	return n == VOID_NUM || n1 == VOID_NUM || n1 == 0.0 ? VOID_NUM : pow(n, 1 / n1);
}

static double func_sqrt(const std::vector<variant *> & vec)
{
	double n = vec[0]->num();
	return n == VOID_NUM ? VOID_NUM : sqrt(vec[0]->num());
}

static double func_pct(const std::vector<variant *> & vec)
{
	double n = vec[0]->num(), n1 = vec[1]->num();
	return n == VOID_NUM || n1 == VOID_NUM || n1 == 0.0 ? VOID_NUM : n * 100 / n1;
}

sentence::sentence()
{
	_const_map.insert(std::make_pair("true", variant_value(TRUE_NUM)));
	_const_map.insert(std::make_pair("false", variant_value(FALSE_NUM)));
	_const_map.insert(std::make_pair("pi", variant_value(3.14159265358979323846)));
	_const_map.insert(std::make_pair("e", variant_value(2.71828182845904523536)));
	_func_map.insert(std::make_pair("or", variant_function(T_NUM_OPER, 2, func_or)));
	_func_map.insert(std::make_pair("and", variant_function(T_NUM_OPER, 2, func_and)));
	_func_map.insert(std::make_pair("not", variant_function(T_NUM_OPER, 1, func_not)));
	_func_map.insert(std::make_pair("eq", variant_function(T_NUM_OPER, -1, func_eq)));
	_func_map.insert(std::make_pair("ne", variant_function(T_NUM_OPER, -1, func_ne)));
	_func_map.insert(std::make_pair("gt", variant_function(T_NUM_OPER, -1, func_gt)));
	_func_map.insert(std::make_pair("lt", variant_function(T_NUM_OPER, -1, func_lt)));
	_func_map.insert(std::make_pair("ge", variant_function(T_NUM_OPER, -1, func_ge)));
	_func_map.insert(std::make_pair("le", variant_function(T_NUM_OPER, -1, func_le)));
	_func_map.insert(std::make_pair("match", variant_function(T_NUM_OPER, -1, func_match)));
	_func_map.insert(std::make_pair("mismatch", variant_function(T_NUM_OPER, -1, func_mismatch)));
	_func_map.insert(std::make_pair("begin", variant_function(T_NUM_OPER, -1, func_begin)));
	_func_map.insert(std::make_pair("end", variant_function(T_NUM_OPER, -1, func_end)));
	_func_map.insert(std::make_pair("part", variant_function(T_NUM_OPER, -1, func_part)));
	_func_map.insert(std::make_pair("add", variant_function(T_NUM_OPER, 2, func_add)));
	_func_map.insert(std::make_pair("sub", variant_function(T_NUM_OPER, 2, func_sub)));
	_func_map.insert(std::make_pair("neg", variant_function(T_NUM_OPER, 1, func_neg)));
	_func_map.insert(std::make_pair("mul", variant_function(T_NUM_OPER, 2, func_mul)));
	_func_map.insert(std::make_pair("div", variant_function(T_NUM_OPER, 2, func_div)));
	_func_map.insert(std::make_pair("mod", variant_function(T_NUM_OPER, 2, func_mod)));
	_func_map.insert(std::make_pair("pos", variant_function(T_STR_OPER, 1, func_pos)));
	_func_map.insert(std::make_pair("first", variant_function(T_STR_OPER, 1, func_first)));
	_func_map.insert(std::make_pair("len", variant_function(T_NUM_FUNC, 1, func_len)));
	_func_map.insert(std::make_pair("abs", variant_function(T_NUM_FUNC, 1, func_abs)));
	_func_map.insert(std::make_pair("exp", variant_function(T_NUM_FUNC, 1, func_exp)));
	_func_map.insert(std::make_pair("fac", variant_function(T_NUM_FUNC, 1, func_fac)));
	_func_map.insert(std::make_pair("logd", variant_function(T_NUM_FUNC, 1, func_logd)));
	_func_map.insert(std::make_pair("logn", variant_function(T_NUM_FUNC, 1, func_logn)));
	_func_map.insert(std::make_pair("max", variant_function(T_NUM_FUNC, -1, func_max)));
	_func_map.insert(std::make_pair("min", variant_function(T_NUM_FUNC, -1, func_min)));
	_func_map.insert(std::make_pair("pct", variant_function(T_NUM_FUNC, 2, func_pct)));
	_func_map.insert(std::make_pair("pow", variant_function(T_NUM_FUNC, 2, func_pow)));
	_func_map.insert(std::make_pair("root", variant_function(T_NUM_FUNC, 2, func_root)));
	_func_map.insert(std::make_pair("sqrt", variant_function(T_NUM_FUNC, 1, func_sqrt)));
}

sentence::~sentence() {}

sentence::variant_value * sentence::var(const char * name)
{
	std::shared_ptr<variant_value>& ptr = _var_map[name];
	variant_value * var = ptr.get();
	if (nullptr != var)
		return var;
	std::shared_ptr<variant_value> nptr = std::make_shared<variant_value>();
	ptr.swap(nptr);
	return ptr.get();
}

sentence & sentence::add(const char * fn, num_function * fp, int a)
{
	_func_map.insert(std::make_pair(fn, variant_function(T_NUM_FUNC, a, fp)));
	return *this;
}

sentence & sentence::add(const char * fn, str_function * fp, int a)
{
	_func_map.insert(std::make_pair(fn, variant_function(T_STR_FUNC, a, fp)));
	return *this;
}

sentence & sentence::define(const char * cn, double n)
{
	_const_map.insert(std::make_pair(cn, variant_value(n)));
	return *this;
}

sentence & sentence::define(const char * cn, const char * s)
{
	_const_map.insert(std::make_pair(cn, variant_value(s)));
	return *this;
}

sentence & sentence::set(const char * name, double n)
{
	auto v = var(name);
	v->_type = T_NUM;
	v->_num = n;
	return *this;
}

sentence & sentence::set(const char * name, const char * s)
{
	auto v = var(name);
	v->_type = T_STR;
	v->_str = s;
	return *this;
}

const char * sentence::parse(const char * expr)
{
	state s;
	s._token = s._next = expr;
	_root.reset(disj(next(s)));
	if (s._type != T_END || !_root)
		return s._token;
	_root->trim();
	return NULL; // success
}

double sentence::evaluate() const
{
	return _root ? _root->num() : VOID_NUM;
}

const char * sentence::evaluate(std::string & ret) const
{
	const char * s = _root ? _root->str() : VOID_STR;
	if (s == VOID_STR)
		ret.assign("");
	else
		s = ret.assign(s, size_string(s)).c_str();
	return s;
}

sentence::state & sentence::next(sentence::state& s)
{
	for (s._type = T_END; s._type == T_END && *s._next; ++s._next)
	{
		s._token = s._next; // set parser to the next position
		s._arity = -1;
		if (numeric(*s._token))
		{ // if numeric advance next position, then return number
			s._type = T_NUM;
			s._num = strtod(s._token, (char **)&s._next); // advance next position
			return s; // with advanced next position
		}
		if (alpha(*s._token))
		{ // if alpha advance next position, then try to return variable, function, number or string
			while (alphanumeric(*s._next))
				++s._next;  // advance next position
			auto cp = _const_map.find(s._token);
			if (cp != _const_map.end())
			{
				s._type = cp->second._type;
				switch (s._type)
				{
				case T_NUM: s._num = cp->second.num(); break;
				case T_STR: s._str = cp->second.str(); break;
				}
				return s; // with advanced next position
			}
			auto fp = _func_map.find(s._token);
			if (fp != _func_map.end())
			{
				s._type = fp->second._type;
				s._arity = fp->second._arity;
				switch (s._type)
				{
				case T_NUM_OPER: case T_NUM_FUNC: s._num_func = fp->second._num_func; break;
				case T_STR_OPER: case T_STR_FUNC: s._str_func = fp->second._str_func; break;
				}
				return s; // with advanced next position
			}
			s._type = T_VAR;
			s._var = var(s._token);
			return s; // with advanced next position
		}
		switch (*s._token)
		{
		case '\"': s._type = T_STR; s._str = ++s._next; while (character(*s._next)) ++s._next; break;
		case ' ': case '\t': case '\n': case '\r': break;
		case '(': s._type = T_OPEN; break;
		case ')': s._type = T_CLOSE; break;
		case ',': s._type = T_SEP; break;
		case '|': s._type = T_NUM_OPER; s._num_func = func_or; break;
		case '&': s._type = T_NUM_OPER; s._num_func = func_and; break;
		case '!': s._type = T_NUM_OPER;
			switch (s._token[1])
			{
			case '=': ++s._next; s._num_func = func_ne; break;
			case '~': ++s._next; s._num_func = func_mismatch; break;
			default: s._num_func = func_not; break;
			}
			break;
		case '=': s._type = T_NUM_OPER;
			switch (s._token[1])
			{
			case '=': ++s._next;
			default: s._num_func = func_eq; break;
			}
			break;
		case '~': s._type = T_NUM_OPER;
			switch (s._token[1])
			{
			case '*': ++s._next; s._num_func = func_begin; break;
			case '~': ++s._next;
			default: s._num_func = func_match; break;
			}
			break;
		case '>': s._type = T_NUM_OPER;
			switch (s._token[1])
			{
			case '=': ++s._next; s._num_func = func_ge; break;
			default: s._num_func = func_gt; break;
			}
			break;
		case '<': s._type = T_NUM_OPER;
			switch (s._token[1])
			{
			case '=': ++s._next; s._num_func = func_le; break;
			default: s._num_func = func_lt; break;
			}
			break;
		case '+': s._type = T_NUM_OPER; s._num_func = func_add; break;
		case '-': s._type = T_NUM_OPER; s._num_func = func_sub; break;
		case '*': s._type = T_NUM_OPER;
			switch (s._token[1])
			{
			case '~': ++s._next; s._num_func = func_end; break;
			case '*': ++s._next; s._num_func = func_part; break;
			default: s._num_func = func_mul; break;
			}
			break;
		case '/': s._type = T_NUM_OPER; s._num_func = func_div; break;
		case '%': s._type = T_NUM_OPER; s._num_func = func_mod; break;
		case '#': s._type = T_NUM_OPER; s._num_func = func_len; break;
		case '@': s._type = T_STR_OPER; s._str_func = func_pos; break;
		case '$': s._type = T_STR_OPER; s._str_func = func_first; break;
		default: s._type = T_ERROR; break;
		}
	}
	return s;
}

sentence::node * sentence::disj(sentence::state& s)
{
	node* n = conj(s);
	while (s._type == T_NUM_OPER && s._num_func == func_or)
		n = new node(func_or, n, conj(next(s)));
	return n;
}

sentence::node * sentence::conj(sentence::state& s)
{
	node* n = word(s);
	while (s._type == T_NUM_OPER && s._num_func == func_and)
		n = new node(func_and, n, word(next(s)));
	return n;
}

sentence::node * sentence::word(sentence::state& s)
{
	bool neg = false; // if negation
	while (s._type == T_NUM_OPER && s._num_func == func_not)
	{
		neg = !neg;
		next(s);
	}
	node* n = comp(s);
	while (s._type == T_NUM_OPER && (s._num_func == func_eq || s._num_func == func_ne
		|| s._num_func == func_gt || s._num_func == func_lt
		|| s._num_func == func_ge || s._num_func == func_le))
	{
		n = new node(s._num_func, n);
		do n->add(comp(next(s)));
		while (s._type == T_SEP);
	}
	if (neg)
		n = new node(func_neg, n);
	return n;
}

sentence::node * sentence::comp(sentence::state& s)
{
	node* n = term(s);
	while (s._type == T_NUM_OPER && (s._num_func == func_add || s._num_func == func_sub))
	{
		auto func = s._num_func;
		n = new node(func, n, term(next(s)));
	}
	return n;
}

sentence::node * sentence::term(sentence::state& s)
{
	node* n = fact(s);
	while (s._type == T_NUM_OPER && (s._num_func == func_mul || s._num_func == func_div
		|| s._num_func == func_mod))
	{
		auto func = s._num_func;
		n = new node(func, n, fact(next(s)));
	}
	return n;
}

sentence::node * sentence::fact(sentence::state& s)
{
	bool neg = false; // if negation
	while (s._type == T_NUM_OPER && (s._num_func == func_add || s._num_func == func_sub))
	{
		if (s._num_func == func_sub)
			neg = !neg;
		next(s);
	}
	bool len = false; // if length
	while (s._type == T_NUM_OPER && s._num_func == func_len)
	{
		len = !len;
		next(s);
	}
	node* n = text(s);
	while (s._type == T_NUM_OPER && (s._num_func == func_match || s._num_func == func_mismatch
		|| s._num_func == func_begin || s._num_func == func_end || s._num_func == func_part))
	{
		n = new node(s._num_func, n);
		do n->add(text(next(s)));
		while (s._type == T_SEP);
	}
	if (len)
		n = new node(func_len, n);
	if (neg)
		n = new node(func_neg, n);
	return n;
}

sentence::node * sentence::text(sentence::state& s)
{
	node* n = atom(s);
	while (s._type == T_STR_OPER && (s._str_func == func_pos || s._str_func == func_first))
	{
		auto func = s._str_func;
		n = new node(func, n, atom(next(s)));
	}
	return n;
}

sentence::node * sentence::atom(sentence::state& s)
{
	node* n = NULL;
	switch (s._type)
	{
	case T_VAR:
		n = new node(s._var);
		next(s);
		break;
	case T_OPEN:
		n = disj(next(s));
		if (s._type == T_CLOSE)
			next(s);
		else
			s._type = T_ERROR;
		break;
	case T_NUM:
		n = new node(s._num);
		next(s);
		break;
	case T_STR:
		n = new node(s._str);
		next(s);
		break;
	case T_NUM_FUNC:
		{
			int arity = s._arity;
			n = new node(s._num_func);
			next(s);
			if (s._type == T_OPEN)
			{
				do n->add(disj(next(s)));
				while (s._type == T_SEP);
				if (s._type == T_CLOSE && (arity < 0 || n->arity() == arity))
					next(s);
				else
					s._type = T_ERROR;
			}
			else
				s._type = T_ERROR;
		}
		break;
	case T_STR_FUNC:
		{
			int arity = s._arity;
			n = new node(s._str_func);
			next(s);
			if (s._type == T_OPEN)
			{
				do n->add(atom(next(s)));
				while (s._type == T_SEP);
				if (s._type == T_CLOSE && (arity < 0 || n->arity() == arity))
					next(s);
				else
					s._type = T_ERROR;
			}
			else
				s._type = T_ERROR;
		}
		break;
	default:
		s._type = T_ERROR;
		break;
	}
	return n;
}

sentence::variant_value::variant_value() : _type(T_ERROR) {}

sentence::variant_value::variant_value(double n) : _type(T_NUM), _num(n) {}

sentence::variant_value::variant_value(const char * s) : _type(T_STR), _str(s) {}

double sentence::variant_value::num() const { return _type == T_NUM ? _num : VOID_NUM; }

const char * sentence::variant_value::str() const { return _type == T_STR ? _str : VOID_STR; }

sentence::variant_function::variant_function(int t, int a, num_function * f) : _type(t), _arity(a), _num_func(f) {}

sentence::variant_function::variant_function(int t, int a, str_function * f) : _type(t), _arity(a), _str_func(f) {}

sentence::node::node() : _type(T_ERROR) {}

sentence::node::node(double n) : _type(T_NUM), _num(n) {}

sentence::node::node(const char * s) : _type(T_STR), _str(s) {}

sentence::node::node(num_function * f, node* n1, node* n2) : _type(T_NUM_FUNC), _num_func(f) { add(n1); add(n2); }

sentence::node::node(str_function * f, node* n1, node* n2) : _type(T_STR_FUNC), _str_func(f) { add(n1); add(n2); }

sentence::node::node(variant_value * v) : _type(T_VAR), _var(v) {}

sentence::node::~node() { for (auto n: _vec) delete n; }

int sentence::node::arity() const { return _vec.size(); }

void sentence::node::add(node * n) { if (n) _vec.push_back(n); }

void sentence::node::trim()
{
	if (_type >= T_NUM_FUNC || _type == T_STR_FUNC)
	{
		bool c = true;
		for (auto v: _vec)
		{
			node * n = dynamic_cast<node *>(v);
			n->trim();
			if (n->_type != T_NUM && n->_type != T_STR)
				c = false;
		}
		if (c)
		{
			switch (_type)
			{
			case T_NUM_FUNC: { double n = num(); _type = T_NUM; _num = n;} break;
			case T_STR_FUNC: { const char * s = str(); _type = T_STR; _str = s;} break;
			}
			for (auto v: _vec)
				delete v;
			_vec.clear();
		}
	}
}

double sentence::node::num() const
{
	switch (_type)
	{
	case T_NUM: return _num;
	case T_NUM_FUNC: return _num_func(_vec);
	case T_VAR: return _var->num();
	}
	return VOID_NUM;
}

const char * sentence::node::str() const
{
	switch (_type)
	{
	case T_STR: return _str;
	case T_STR_FUNC: return _str_func(_vec);
	case T_VAR: return _var->str();
	}
	return VOID_STR;
}

bool sentence::identifier_comparator::operator() (const char * s1, const char * s2) const { return compare_identifiers(s1, s2) < 0; }
