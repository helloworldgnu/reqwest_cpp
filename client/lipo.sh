lipo -create target/aarch64-apple-darwin/debug/libclient.dylib target/x86_64-apple-darwin/debug/libclient.dylib -output libclient_debug.dylib

lipo -create target/aarch64-apple-darwin/release/libclient.dylib target/x86_64-apple-darwin/release/libclient.dylib -output libclient_release.dylib