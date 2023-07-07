#include "common.h"

int main() {
  PRINT_INT("CHAR", sizeof(char));
  PRINT_INT("INT8", sizeof(int8_t));
  PRINT_INT("INT16", sizeof(int16_t));
  PRINT_INT("SHORT", sizeof(short));
  PRINT_INT("INT32", sizeof(int32_t));
  PRINT_INT("INT64", sizeof(int64_t));
  PRINT_INT("LONG", sizeof(long));
  PRINT_INT("UNSIGNED", sizeof(unsigned));
  PRINT_INT("LONG LONG", sizeof(long long));
  PRINT_INT("FLOAT", sizeof(float));
  PRINT_INT("DOUBLE", sizeof(double));
  PRINT_INT("LONG DOUBLE", sizeof(long double));
  PRINT_INT("WCHAR_T", sizeof(wchar_t));
  if (sizeof (mode_t)) return 0;
  return 0;
}
