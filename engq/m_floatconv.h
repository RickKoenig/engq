void float_test();
string float2str(float f);
float str2float(const C8* s); // if exp is h or H then use hex
string float2strhex(float f); // exponent is 2^ not 16^, +-1.nnnnnnh+=ee
float str2floathex(const C8* s); // special 13 char string to float s1.nnnnnnhsee
