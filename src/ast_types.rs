use crate::lexer_types::*;

#[derive(Debug)]
pub(crate) enum Ast {
    Program(Vec<Box<Ast>>),
    Not(Box<Ast>),
    Comparison {
        operator: ComparisonExpr,
        left: Box<Ast>,
        right: Box<Ast>,
    },
    LogicalExpr {
        operator: LogicalExpr,
        left: Box<Ast>,
        right: Box<Ast>,
    },
    BitwiseShift {
        shift_direction: BitwiseShift,
        left: Box<Ast>,
        right: Box<Ast>,
    },
    BinaryOperator {
        operator: BinaryOperator,
        left: Box<Ast>,
        right: Box<Ast>,
    },
    VariableDeclaration {
        identifier: Box<str>,
        right: Box<Ast>,
        constant: bool,
    },
    Assignment {
        identifier: Box<Ast>,
        right: Box<Ast>,
        local: bool,
    },
    MemberExpression {
        left: Box<Ast>,
        right: Box<Ast>,
    },
    CallExpression {
        left: Box<Ast>,
        arguments: Vec<Ast>,
    },
    IfExpression {
        check: Box<Ast>,
        body: Vec<Ast>,
    },
    String(Box<str>),
    Int(i32),
    Float(f32),
    Identifier(Box<str>),
}
