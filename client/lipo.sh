lipo -create target/aarch64-apple-darwin/debug/libcrab_http.dylib target/x86_64-apple-darwin/debug/libcrab_http.dylib -output libcrab_http_debug.dylib
install_name_tool -id @rpath/libcrab_http.dylib libcrab_http_debug.dylib

lipo -create target/aarch64-apple-darwin/release/libcrab_http.dylib target/x86_64-apple-darwin/release/libcrab_http.dylib -output libcrab_http_release.dylib
install_name_tool -id @rpath/libcrab_http.dylib libcrab_http_release.dylib