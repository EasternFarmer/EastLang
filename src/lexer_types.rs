#[derive(Debug, PartialEq, Clone, Copy)]
pub(crate) enum LogicalExpr {
    And,
    Or,
    Xor,
}

#[derive(Debug, PartialEq, Clone, Copy)]
pub(crate) enum ComparisonExpr {
    Equals,
    NotEquals,
    Greater,
    GreaterEqual,
    Less,
    LessEqual,
}

#[derive(Debug, PartialEq, Clone, Copy)]
pub(crate) enum BinaryOperator {
    Add,
    Substract,
    Multiply,
    Divide,
    Modulo,
}

#[derive(Debug, PartialEq, Clone, Copy)]
pub(crate) enum BitwiseShift {
    Left,
    Right,
}

#[derive(Debug, PartialEq, Clone)]
pub(crate) enum TokenType {
    // types
    String(Box<str>),
    Int(i32),
    Float(f32),
    // List,
    Identifier(Box<str>),
    Callable,
    Const,
    Local,
    BinaryOperator(BinaryOperator),
    BitwiseShift(BitwiseShift),
    Equals,

    If,
    Else,
    ElseIf,
    While,

    LogicalExpr(LogicalExpr),
    Not,
    ComparisonExpr(ComparisonExpr),

    OpenParen,
    ClosedParen,

    OpenBrace,   // {
    ClosedBrace, // }

    OpenBracket,   // [
    ClosedBracket, // ]

    Comma,
    Dot,
    Colon,
    SemiColon,
    Monkey, // @

    EOF,
}
