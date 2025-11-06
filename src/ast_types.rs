use crate::lexer_types::*;

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
    String(Box<str>),
    Int(i32),
    Float(f32),
    Identifier(Box<str>),
}
