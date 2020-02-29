#include <iostream>
#include <memory.h>



// print a varible in binary format
template <typename T>
void print(T a) {
  int size = sizeof(a);
  void * men = malloc(size);
  memcpy(men, &a, size);
  char* mem = (char*)men;
  mem = mem + size - 1;
  for (int i=0; i<size; ++i) {
    unsigned char flag = 0x80;
    for (int j=0; j<8; ++j) {
      if(flag & *mem) {std::cout << 1 ;}
      else {std::cout << 0 ;}
      flag = flag >> 1;
    }
    // std::cout << " " ;
    mem --;
  }
  std::cout << std::endl;
  free(men);
}
// convert a float32 to float16 store in a unsigned short varible
unsigned short fp32Tfp16(float a) {
  unsigned short fp16 = 0;  
  if (a < 0) {fp16 = 0x8000;}

  unsigned int b = 0;
  unsigned int mask = 0x000000ff;
  memcpy(&b, &a, 4);
  b = b >> 23;
  int low_8 = b & mask;
  low_8 = low_8 - 127 + 15;
  unsigned short short_e = low_8 >= 0 ? low_8 : 0;
  short_e = short_e > 31 ? 31 : short_e;
  short_e = short_e << 10;
  fp16 = fp16 | short_e;

  unsigned int f = 0;
  unsigned int f_mask = 0x3ff;
  memcpy(&f, &a, 4);
  f = f >> 13;
  f = f & f_mask;

  fp16 = fp16 | f;

  return fp16;
}


// print a float16 store in a unsigned short varible
void print_fp16(unsigned short a) {
  int sign = 0;
  unsigned short sign_mask = 0x8000;
  sign = a & sign_mask ? -1 : 1;

  unsigned short f_mask = 0x03ff;
  unsigned short f_value = a & f_mask;

  a = a >> 10;
  unsigned short e_mask = 0x001f;
  unsigned short e_value = a & e_mask;
  short e = e_value - 15;

  float result = 1;
  if (e > 0) {
    for (int i=0; i<e; ++i) {
      result *= 2;
    }
  } else {
    e = 0 - e;
    for (int i=0; i<e; ++i) {
      result /= 2;
    }
  }

  result = sign * result;
  result = result * (1 + (float)f_value/ (1024.0));

  std::cout << result << std::endl;

}



int main() {
  float a = 11.13;
  print(a);
  unsigned short b = fp32Tfp16(a);
  print_fp16(b);
  return 0;
}
