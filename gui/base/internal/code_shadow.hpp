#pragma once

#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <new>
#include <ostream>

namespace base {
namespace ffi {
const uint8_t* bytes_content(void* handle);
uint64_t bytes_len(void* handle);
void free_byte_buffer(void* handle);
void free_bytes(uint8_t* buf, uint64_t len);

void* md5_bytes(const uint8_t* input, uint32_t len);
void* md5_file(const char* path);
void* md5_file_wide(const wchar_t* file_path, uintptr_t length);

void* base64_decode(const uint8_t* input, uint32_t len);
void* base64_encode(const uint8_t* input, uint32_t len);
void* base64_url_decode(const uint8_t* input, uint32_t len);
void* base64_url_encode(const uint8_t* input, uint32_t len);

void* url_decode(const char* input);
void* url_encode(const char* input);
}  // namespace ffi

namespace fs {
enum class FileErrorKind;
enum class FileSeek;

namespace ffi {
#ifdef __cplusplus
extern "C" {
#endif
//===================file ===================
FileErrorKind file_err_kind(void* handle);
const uint8_t* file_err_msg(void* handle);
uint64_t file_err_msg_len(void* handle);
void file_err_clear(void* handle);

void close_file(void* handle);
void* open_file(const char* file_path, fs::FileErrorKind* err);
void* open_file_wide(const wchar_t* file_path, uint32_t length,
                     fs::FileErrorKind* err);
void* read_all(void* handle);
int64_t read_data(void* handle, uint8_t* buf, uint64_t len);
int64_t seek(void* handle, fs::FileSeek whence, int64_t offset);
int64_t write_data(void* handle, const uint8_t* buf, int64_t len);
void flush(void* handle);

FileErrorKind file_meta_err_kind(void* handle);
const uint8_t* file_meta_err_msg(void* handle);
uint64_t file_meta_err_msg_len(void* handle);
void file_meta_err_clear(void* handle);

void free_file_meta(void* handle);
void* file_meta(const char* file_path, FileErrorKind* err);
void* file_meta_wide(const wchar_t* file_path, uint32_t length,
                     FileErrorKind* err);
uint64_t last_modified_time(void* handle);
uint64_t last_access_time(void* handle);
uint64_t created_time(void* handle);
bool is_file(void* handle);
bool is_dir(void* handle);
bool is_symlink(void* handle);
uint64_t file_size(void* handle);
bool readonly(void* handle);
void set_readonly(void* handle, bool value);

//=================== format ===================
void* file_format_from_bytes(const unsigned char* bytes, unsigned int length);

void* file_format_parse(const char* file_path);

void* file_format_parse_wide(const wchar_t* file_path, uintptr_t length);

void mt_file_format_destroy(void* handle);

const uint8_t* mt_file_format_extension(void* handle);

int64_t mt_file_format_extension_len(void* handle);

const uint8_t* mt_file_format_format(void* handle);

int64_t mt_file_format_format_len(void* handle);

const uint8_t* mt_file_format_kind(void* handle);

int64_t mt_file_format_kind_len(void* handle);

const uint8_t* mt_file_format_media_type(void* handle);

int64_t mt_file_format_media_type_len(void* handle);

const uint8_t* mt_file_format_name(void* handle);

int64_t mt_file_format_name_len(void* handle);

const uint8_t* mt_file_format_short_name(void* handle);

int64_t mt_file_format_short_name_len(void* handle);
#ifdef __cplusplus
}
#endif
}  // namespace ffi
}  // namespace fs

namespace net::dns {
namespace ffi {
#ifdef __cplusplus
extern "C" {
#endif
void* dns_record(void* resp, uint32_t index);

void dns_record_free(void* handle);

int64_t dns_records_count(void* handle);

void dns_resp_free(void* handle);

const uint8_t* ipv4AddrBytes(void* handle);

int64_t ipv4AddrLen(void* handle);

void* query_domain_ipv4(const char* domain_name, const char* name_server);
#ifdef __cplusplus
}
#endif
}  // namespace ffi
}  // namespace net::dns
}  // namespace base
