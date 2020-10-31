#if defined(__cplusplus)
extern "C" {
#endif

#if defined(WIN32) && !defined(strnlen)
size_t strnlen(const char* s, size_t maxlen);
#endif

// OS indepedent API to generate a cryptographic random number.
int platformRandomDataFunction(unsigned char* buffer, unsigned long size);

#if defined(__cplusplus)
}
#endif
