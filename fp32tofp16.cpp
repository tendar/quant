#include <iostream>
#include <memory.h>
#include <random>

using namespace std;

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
float fp16Tfp32(unsigned short a) {
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

  return result;
}

// create a random float data between min and max
float myRander(float min,float max) {
  random_device rd;
  mt19937 eng(rd());
  uniform_real_distribution<float> dist(min,max);
  return dist(eng);
}

// test all the funtion
int main() {
  // float a = 11.13;
  // print(a);
  // unsigned short b = fp32Tfp16(a);
  // print_fp16(b);

  float max = 0;		// recore the max error in the test
  float max_input = 0;		// recore which input cause the max error
  float range_min = 65504.0;	//
  float range_max = 0.00005;
  for (int i=0; i<100000000; ++i) {			// test i times
    float input = 0; 
    if (i % 2) {					// input range from 0.00005 to 65504 and -65504 to -0.00005
      input =  myRander(0.00005, 65504.0);		// becuase it is the range of float16, out of the range will
    } else {						// cause higher error
      input =  myRander(-0.00005, -65504.0);
    }
    // float input =  myRander(0.00005, 65504.0);
    // std::cout << input << "	";
    // float input = 0.00004;
    unsigned short mediem = fp32Tfp16(input);		// conver float32 input to float16 output
    float output = fp16Tfp32(mediem);			// conver float16 to float32
    // std::cout << output << "	";

    // std::cout << output - input << std::endl;
    float tmp = 0;					// recore the error between float16 and float32
    if (input > output) {tmp = input - output;}
    else {tmp = output - input;}

    input = input > 0 ? input : 0 - input;		// caculate the abs of input
    if (input < range_min) { range_min = input;}	// recore the max value we have test
    if (input > range_max) { range_max = input;}	// recore the min value we have test

    tmp = tmp / input;					// caculate the error rate

    if (tmp > max) {					// recore the max error rate
      max = tmp;					// recore which input cause the max error rate
      max_input = input;
    }
  }
  // print out the test result
  std::cout << "when the input is : [" << max_input << "] , wu cha zui da wei : " << max << std::endl;
  std::cout << "range max : " << range_max << std::endl;
  std::cout << "range min : " << range_min << std::endl;
  return 0;
}
