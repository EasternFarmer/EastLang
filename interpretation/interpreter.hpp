#include "../parsing/ast.hpp"
#include "Environment.hpp"

RuntimeVal* evaluate(Stmt* astNode, Environment* env);

RuntimeVal* eval_member_expr(MemberExpr* specialExpr, Environment* env);

RuntimeVal* eval_special_expr(SpecialExpr* specialExpr, Environment* env);

RuntimeVal* eval_comparison_expr(RuntimeVal* left, RuntimeVal* right, ComparisonOperatorType op);

bool eval_runtimeval_to_bool(RuntimeVal* var);

RuntimeVal* eval_logical_expr(RuntimeVal* left, RuntimeVal* right, LogicalOperatorType op);

RuntimeVal* eval_call_expr(CallExpr* callexpr, Environment* env);

RuntimeVal* eval_if_expr(IfStatement* ifExpr, Environment* env);

RuntimeVal* eval_while_expr(WhileStatement* whileExpr, Environment* env);

std::vector<RuntimeVal*> eval_args(std::vector<Expr*> args, Environment* env);

RuntimeVal* eval_program(Program* program, Environment* env);

RuntimeVal* eval_assignment(AssignmentExpr* assign, Environment* env);

NumberVal* eval_binary_math(NumberVal* a, NumberVal* b, OperatorType op);

RuntimeVal* eval_binary_expr(BinaryExpr* binary, Environment* env);