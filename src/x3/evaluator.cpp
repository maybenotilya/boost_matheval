#define MATHEVAL_IMPLEMENTATION

#include "evaluator.hpp"
#include "ast.hpp"

namespace matheval {

namespace ast {

// Optimizer

template <typename T>
struct holds_alternative_impl {
    using result_type = bool;

    template <typename U>
    bool operator()(U const &) const {
        return std::is_same<U, T>::value;
    }
};

template <typename T, typename... Ts>
bool holds_alternative(x3::variant<Ts...> const &v) {
    return boost::apply_visitor(holds_alternative_impl<T>(), v);
}

ConstantFolder::result_type ConstantFolder::operator()(nil) const {
    return result_type{0};
}

ConstantFolder::result_type ConstantFolder::operator()(double n) const {
    return result_type{n};
}

ConstantFolder::result_type ConstantFolder::
operator()(std::string const &c) const {
    return result_type{c};
}

ConstantFolder::result_type ConstantFolder::
operator()(operation const &x, operand const &lhs) const {
    auto rhs = boost::apply_visitor(*this, x.rhs);

    if (holds_alternative<double>(lhs) && holds_alternative<double>(rhs)) {
        return result_type{
            x.op(boost::get<double>(lhs), boost::get<double>(rhs))};
    }
    return result_type{binary_op{x.op, lhs, rhs}};
}

ConstantFolder::result_type ConstantFolder::
operator()(unary_op const &x) const {
    auto rhs = boost::apply_visitor(*this, x.rhs);

    /// If the operand is known, we can directly evaluate the function.
    if (holds_alternative<double>(rhs)) {
        return result_type{x.op(boost::get<double>(rhs))};
    }
    return result_type{unary_op{x.op, rhs}};
}

ConstantFolder::result_type ConstantFolder::
operator()(binary_op const &x) const {
    auto lhs = boost::apply_visitor(*this, x.lhs);
    auto rhs = boost::apply_visitor(*this, x.rhs);

    /// If both operands are known, we can directly evaluate the function,
    /// else we just update the children with the new expressions.
    if (holds_alternative<double>(lhs) && holds_alternative<double>(rhs)) {
        return result_type{
            x.op(boost::get<double>(lhs), boost::get<double>(rhs))};
    }
    return result_type{binary_op{x.op, lhs, rhs}};
}

ConstantFolder::result_type ConstantFolder::
operator()(ternary_op const &x) const {
    auto cond = boost::apply_visitor(*this, x.cond);
    auto t = boost::apply_visitor(*this, x.t);
    auto f = boost::apply_visitor(*this, x.f);

    /// If both operands are known, we can directly evaluate the function,
    /// else we just update the children with the new expressions.
    if (holds_alternative<double>(cond) && holds_alternative<double>(t) && holds_alternative<double>(f)) {
        return result_type{
            x.op(boost::get<double>(cond), boost::get<double>(t), boost::get<double>(f))};
    }
    return result_type{ternary_op{x.op, cond, t, f}};
}

ConstantFolder::result_type ConstantFolder::
operator()(expression const &x) const {
    auto state = boost::apply_visitor(*this, x.lhs);
    for (operation const &oper : x.rhs) {
        state = (*this)(oper, state);
    }
    return result_type{state};
}

// Evaluator

double eval::operator()(nil) const {
    BOOST_ASSERT(0);
    return 0;
}

double eval::operator()(double n) const { return n; }

double eval::operator()(std::string const &c) const {
    auto it = st.find(c);
    if (it == st.end()) {
        throw std::invalid_argument("Unknown variable " + c); // NOLINT
    }
    return it->second;
}

double eval::operator()(operation const &x, double lhs) const {
    double rhs = boost::apply_visitor(*this, x.rhs);
    return x.op(lhs, rhs);
}

double eval::operator()(unary_op const &x) const {
    double rhs = boost::apply_visitor(*this, x.rhs);
    return x.op(rhs);
}

double eval::operator()(binary_op const &x) const {
    double lhs = boost::apply_visitor(*this, x.lhs);
    double rhs = boost::apply_visitor(*this, x.rhs);
    return x.op(lhs, rhs);
}

double eval::operator()(ternary_op const &x) const {
    double cond = boost::apply_visitor(*this, x.cond);
    double t = boost::apply_visitor(*this, x.t);
    double f = boost::apply_visitor(*this, x.f);
    return x.op(cond, t, f);
}

double eval::operator()(expression const &x) const {
    double state = boost::apply_visitor(*this, x.lhs);
    for (operation const &oper : x.rhs) {
        state = (*this)(oper, state);
    }
    return state;
}

// Variables finder

std::set<std::string> variables_finder::operator()(nil) const {
    BOOST_ASSERT(0);
    return {};
}

std::set<std::string> variables_finder::operator()(double n) const { return {}; }

std::set<std::string> variables_finder::operator()(std::string const &c) const {
    return {c};
}

std::set<std::string> variables_finder::operator()(operation const &x, std::set<std::string> lhs) const {
    auto vars = boost::apply_visitor(*this, x.rhs);
    vars.insert(lhs.begin(), lhs.end());
    return vars;
}

std::set<std::string> variables_finder::operator()(unary_op const &x) const {
    return boost::apply_visitor(*this, x.rhs);
}

std::set<std::string> variables_finder::operator()(binary_op const &x) const {
    auto lhs = boost::apply_visitor(*this, x.lhs);
    auto rhs = boost::apply_visitor(*this, x.rhs);
    lhs.insert(rhs.begin(), rhs.end());
    return lhs;
}

std::set<std::string> variables_finder::operator()(ternary_op const &x) const {
    auto cond = boost::apply_visitor(*this, x.cond);
    auto t = boost::apply_visitor(*this, x.t);
    auto f = boost::apply_visitor(*this, x.f);
    cond.insert(t.begin(), t.end());
    cond.insert(f.begin(), f.end());
    return cond;
}

std::set<std::string> variables_finder::operator()(expression const &x) const {
    auto state = boost::apply_visitor(*this, x.lhs);
    for (operation const &oper : x.rhs) {
        state = (*this)(oper, state);
    }
    return state;
}

} // namespace ast

} // namespace matheval
