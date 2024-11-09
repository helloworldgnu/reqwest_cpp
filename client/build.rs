extern crate cbindgen;

use cbindgen::RenameRule::CamelCase;
use cbindgen::{ParseConfig, SortKey::Name, StructConfig};

use cbindgen::Config;
use std::env;
use std::path::PathBuf;
use std::vec;

fn main() {
    let crate_dir = env::var("CARGO_MANIFEST_DIR").unwrap();

    let package_name = env::var("CARGO_PKG_NAME").unwrap();
    let output_file = target_dir()
        .join(format!("{}.hpp", package_name))
        .display()
        .to_string();

    let structure = StructConfig {
        rename_fields: CamelCase,
        ..Default::default()
    };

    let parse = ParseConfig {
        parse_deps: true,
        include: Some(vec![String::from("reqwest::blocking")]),
        ..Default::default()
    };
    let mut config = Config::default();
    config.namespace = Some(String::from("base::ffi"));
    config.includes = vec![String::from("ffi.hpp")];
    config.pragma_once = true;
    config.cpp_compat = true;
    config.sort_by = Name;
    config.structure = structure;
    config.parse = parse;

    let _ = match cbindgen::generate_with_config(&crate_dir, config) {
        Ok(x) => x.write_to_file(&output_file),
        Err(e) => {
            println!("{:#?}", e);
            false
        }
    };
}

/// Find the location of the `target/` directory. Note that this may be
/// overridden by `cmake`, so we also need to check the `CARGO_TARGET_DIR`
/// variable.
fn target_dir() -> PathBuf {
    if let Ok(target) = env::var("CARGO_TARGET_DIR") {
        PathBuf::from(target)
    } else {
        PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap()).join("target")
    }
}
