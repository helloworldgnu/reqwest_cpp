// We'll put our errors in an `errors` module, and other modules in
// this crate will `use errors::*;` to get access to everything
// `error_chain!` creates.


//! Common error types used in this crate.
error_chain! {

    foreign_links {
        Reqwest(::reqwest::Error);
    }
    errors {
        Panic(inner: Box<::std::any::Any + Send + 'static>) {
            description("Thread Panicked")
                display("{}",
                        if let Some(s) = inner.downcast_ref::<String>() {
                            s.clone()
                        } else if let Some(s) = inner.downcast_ref::<&str>() {
                            s.to_string()
                        } else {
                            String::from("Thread Panicked")
                        })
        }
    }
}