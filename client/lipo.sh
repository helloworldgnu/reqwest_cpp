lipo -create target/aarch64-apple-darwin/debug/libcrab_http.dylib target/x86_64-apple-darwin/debug/libcrab_http.dylib -output libclient_debug.dylib
install_name_tool -id @rpath/libcrab_http.dylib libclient_debug.dylib

lipo -create target/aarch64-apple-darwin/release/libcrab_http.dylib target/x86_64-apple-darwin/release/libcrab_http.dylib -output libclient_release.dylib
install_name_tool -id @rpath/libcrab_http.dylib libclient_release.dylib