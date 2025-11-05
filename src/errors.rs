#[derive(Debug)]
pub(crate) enum Errors {
  SyntaxError,
  ValueError,
}

pub(crate) fn log_error(data_tuple: (Errors, String)) {
  let (err_type, message) = data_tuple;
  let esc = "\x1b[";

  match err_type {
    Errors::SyntaxError => {
      println!("{esc}31m{esc}1mSyntaxError:{esc}0m {}", message)
    },
    Errors::ValueError => {
      println!("{esc}31m{esc}1mValueError:{esc}0m {}", message)
    }
  }
}