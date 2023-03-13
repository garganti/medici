/*
 * main.cpp
 *
 *  Created on: Mar 8, 2023
 *      Author: bombarda
 */
#ifndef CONSTRAINTSPARSER_H_
#define CONSTRAINTSPARSER_H_

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include "header.h"

using namespace std;

namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;

extern parameter_bimap valuesMap;

struct op_or {
};
struct op_and {
};
struct op_not {
};
struct op_eq {
};
struct op_neq {
};
struct op_impl {
};
struct op_dblimpl {
};

typedef std::string var;
template<typename tag> struct binop;
template<typename tag> struct unop;

typedef boost::variant<var, boost::recursive_wrapper<unop<op_not> >,
		boost::recursive_wrapper<binop<op_and> >,
		boost::recursive_wrapper<binop<op_or> >,
		boost::recursive_wrapper<binop<op_eq> >,
		boost::recursive_wrapper<binop<op_impl> >,
		boost::recursive_wrapper<binop<op_neq> >,
		boost::recursive_wrapper<binop<op_dblimpl> > > expr;

template<typename tag> struct binop {
	explicit binop(const expr &l, const expr &r) :
			oper1(l), oper2(r) {
	}
	expr oper1, oper2;
};

template<typename tag> struct unop {
	explicit unop(const expr &o) :
			oper1(o) {
	}
	expr oper1;
};

struct printer: boost::static_visitor<std::string> {
	printer() {
	}

	//
	std::string operator()(const var &v) const {
		return v;
	}

	std::string operator()(const binop<op_and> &b) const {
		return print(" *", b.oper1, b.oper2);
	}
	std::string operator()(const binop<op_or> &b) const {
		return print(" +", b.oper1, b.oper2);
	}
	std::string operator()(const binop<op_eq> &b) const {
		std::string parName = boost::apply_visitor(*this, b.oper1);
		std::string parValue = boost::apply_visitor(*this, b.oper2);
		std::pair<std::string, std::string> key = make_pair(parName, parValue);
		return std::to_string(valuesMap.left.find(key)->second);
	}
	std::string operator()(const binop<op_neq> &b) const {
		std::string parName = boost::apply_visitor(*this, b.oper1);
		std::string parValue = boost::apply_visitor(*this, b.oper2);
		std::pair<std::string, std::string> key = make_pair(parName, parValue);
		return std::to_string(valuesMap.left.find(key)->second) + " -";
	}
	std::string operator()(const binop<op_impl> &b) const {
		struct unop<op_not> notA(b.oper1);
		struct binop<op_or> orB(notA, b.oper2);
		return boost::apply_visitor(*this, (expr) orB);
	}
	std::string operator()(const binop<op_dblimpl> &b) const {
		struct unop<op_not> notA(b.oper1);
		struct unop<op_not> notB(b.oper2);
		struct binop<op_and> aAndB(b.oper1, b.oper2);
		struct binop<op_and> notAAndNotB(notA, notB);
		struct binop<op_or> res(aAndB, notAAndNotB);
		return boost::apply_visitor(*this, (expr) res);
	}

	std::string print(const std::string &op, const expr &l,
			const expr &r) const {
		std::string res = boost::apply_visitor(*this, l);
		res = res + " ";
		res = res + boost::apply_visitor(*this, r);
		res = res + op;
		return res;
	}

	std::string operator()(const unop<op_not> &u) const {
		std::string res = boost::apply_visitor(*this, u.oper1);
		res = res + " ";
		res = res + "-";
		return res;
	}
};

std::ostream& operator<<(std::ostream &os, const expr &e) {
	std::string res = boost::apply_visitor(printer(), e);
	os << res;
	return os;
}

template<typename It, typename Skipper = qi::space_type>
struct parser: qi::grammar<It, expr(), Skipper> {
	parser() :
			parser::base_type(expr_) {
		using namespace qi;

		expr_ = impl_.alias();

				impl_ = dblimpl_[_val = _1]
						>> *("=>"
								>> dblimpl_[_val = phx::construct<binop<op_impl>>(_val,
										_1)]);
				dblimpl_ = or_[_val = _1]
						>> *("<=>"
								>> or_[_val = phx::construct<binop<op_dblimpl>>(_val,
										_1)]);

				or_ = and_[_val = _1] >> *((no_case["or"] | "||") >> and_[_val =
						phx::construct<binop<op_or>>(_val, _1)]);

				and_ = neq_[_val = _1] >> *((no_case["and"] | "&&") >> neq_[_val =
						phx::construct<binop<op_and>>(_val, _1)]);

				neq_ =
						eq_[_val = _1]
								>> *("!="
										>> eq_[_val = phx::construct<binop<op_neq>>(
												_val, _1)]);

				eq_ = not_[_val = _1] >> *((*lit("=")) >> not_[_val =
						phx::construct<binop<op_eq>>(_val, _1)]);

				not_ = ("not" > simple)[_val = phx::construct<unop<op_not>>(_1)]
						| simple[_val = _1];

				simple = (('(' > expr_ > ')') | var_);

				var_ = (qi::lexeme[+qi::char_("a-zA-Z0-9_") - no_case["and"] - no_case["or"]]);

				BOOST_SPIRIT_DEBUG_NODE(expr_);
				BOOST_SPIRIT_DEBUG_NODE(impl_);
				BOOST_SPIRIT_DEBUG_NODE(dblimpl_);
				BOOST_SPIRIT_DEBUG_NODE(or_);
				BOOST_SPIRIT_DEBUG_NODE(and_);
				BOOST_SPIRIT_DEBUG_NODE(not_);
				BOOST_SPIRIT_DEBUG_NODE(eq_);
				BOOST_SPIRIT_DEBUG_NODE(neq_);
				BOOST_SPIRIT_DEBUG_NODE(simple);
				BOOST_SPIRIT_DEBUG_NODE(var_);
			}

		private:
			qi::rule<It, var(), Skipper> var_;
			qi::rule<It, expr(), Skipper> not_, and_, or_, simple, expr_, eq_,
					impl_, neq_, dblimpl_;
};

#endif // CONSTRAINTSPARSER_H_
