/* This sketch verifies that the math macros still compile successfully
 * Many of them are sufficiently rarely used - at least by users who are 
 * confident enough in their code that when they find a bug they don't  
 * assume they were doing it wrong and will report it - a lot of them  
 * are Arduino-land "easy mode" functions that are mostly used by those
 * who are new to embedded development - but they are supposed to work. 
 * It also provides a way to detect changes in flash usage of these. 
 * 
 * This does NOT verify that they get right results, just that they compile.
 */


volatile uint32_t GARBAGE = 0;
void discard(uint32_t g) {
  GARBAGE ^= g;
};
void discard(uint8_t g) {
  discard((uint32_t) g);
};
void discard(uint16_t g) {
  discard((uint32_t) g);
};
void discard(int8_t g) {
  discard((uint32_t) g);
};
void discard(int16_t g) {
  discard((uint32_t) g);
};
void discard(int32_t g) {
  discard((uint32_t) g);
};

volatile uint8_t NOT_A_CONST_BYTE = 0;
volatile uint16_t NOT_A_CONST_WORD = 0;
volatile uint32_t NOT_A_CONST_DWORD = 0;
volatile int8_t NOT_A_CONST_CHAR = 0;
volatile int16_t NOT_A_CONST_INT = 0;
volatile int32_t NOT_A_CONST_LONG = 0;
volatile float NOT_A_CONST_FLOAT = 0;
void loop(){
}
void setup() {
  compile_math_uint8();
  compile_math_uint16();
  compile_math_uint32();
  compile_math_int8();
  compile_math_int16();
  compile_math_int32();
  compile_math_float();
  compile_test_etc();
};


void compile_math_float() {
  float arg1 = NOT_A_CONST_FLOAT;
  float arg2 = NOT_A_CONST_FLOAT;
  float arg3 = NOT_A_CONST_FLOAT;
  float retval_round_float = round(arg1);
  discard((uint32_t)retval_round_float);
  float retval_min_float = min(arg1, arg2);
  discard((uint32_t)retval_min_float);
  float retval_max_float = max(arg1, arg2);
  discard((uint32_t)retval_max_float);
  float retval_constrain_float = constrain(arg1, arg2, arg3);
  discard((uint32_t)retval_constrain_float);
  float retval_radians_float = radians(arg1);
  discard((uint32_t)retval_radians_float);
  float retval_degrees_float = degrees(arg1);
  discard((uint32_t)retval_degrees_float);
  float retval_sq_float = sq(arg1);
  discard((uint32_t)retval_sq_float);
}

void compile_math_uint8() {
  uint8_t arg1 = NOT_A_CONST_BYTE;
  uint8_t arg2 = NOT_A_CONST_BYTE;
  uint8_t arg3 = NOT_A_CONST_BYTE;
  uint8_t bitindex = NOT_A_CONST_BYTE;
  uint8_t retval_round_uint8 = round(arg1);
  discard(retval_round_uint8);
  uint8_t retval_min_uint8 = min(arg1, arg2);
  discard(retval_min_uint8);
  uint8_t retval_max_uint8 = max(arg1, arg2);
  discard(retval_max_uint8);
  uint8_t retval_constrain_uint8 = constrain(arg1, arg2, arg3);
  discard(retval_constrain_uint8);
  uint8_t retval_radians_uint8 = radians(arg1);
  discard(retval_radians_uint8);
  uint8_t retval_degrees_uint8 = degrees(arg1);
  discard(retval_degrees_uint8);
  uint8_t retval_sq_uint8 = sq(arg1);
  discard(retval_sq_uint8);
  uint8_t retval_bitRead_uint8 = bitRead(arg1, bitindex);
  discard(retval_bitRead_uint8);
  uint8_t retval_bitSet_uint8 = bitSet(arg1, bitindex);
  discard(retval_bitSet_uint8);
  uint8_t retval_bitClear_uint8 = bitClear(arg1, bitindex);
  discard(retval_bitClear_uint8);
  uint8_t retval_bitWrite_uint8 = bitWrite(arg1, bitindex, arg3);
  discard(retval_bitWrite_uint8);
  uint8_t retval_bit_uint8 = bit(bitindex);
  discard(retval_bit_uint8);
};


void compile_math_uint16() {
  uint16_t arg1 = NOT_A_CONST_WORD;
  uint16_t arg2 = NOT_A_CONST_WORD;
  uint16_t arg3 = NOT_A_CONST_WORD;
  uint8_t bitindex = NOT_A_CONST_BYTE;
  uint16_t retval_round_uint16 = round(arg1);
  discard(retval_round_uint16);
  uint16_t retval_min_uint16 = min(arg1, arg2);
  discard(retval_min_uint16);
  uint16_t retval_max_uint16 = max(arg1, arg2);
  discard(retval_max_uint16);
  uint16_t retval_constrain_uint16 = constrain(arg1, arg2, arg3);
  discard(retval_constrain_uint16);
  uint16_t retval_radians_uint16 = radians(arg1);
  discard(retval_radians_uint16);
  uint16_t retval_degrees_uint16 = degrees(arg1);
  discard(retval_degrees_uint16);
  uint16_t retval_sq_uint16 = sq(arg1);
  discard(retval_sq_uint16);
  uint16_t retval_bitRead_uint16 = bitRead(arg1, bitindex);
  discard(retval_bitRead_uint16);
  uint16_t retval_bitSet_uint16 = bitSet(arg1, bitindex);
  discard(retval_bitSet_uint16);
  uint16_t retval_bitClear_uint16 = bitClear(arg1, bitindex);
  discard(retval_bitClear_uint16);
  uint16_t retval_bitWrite_uint16 = bitWrite(arg1, bitindex, arg3);
  discard(retval_bitWrite_uint16);
  uint16_t retval_bit_uint16 = bit(bitindex);
  discard(retval_bit_uint16);
};
void compile_math_uint32() {
  uint32_t arg1 = NOT_A_CONST_DWORD;
  uint32_t arg2 = NOT_A_CONST_DWORD;
  uint32_t arg3 = NOT_A_CONST_DWORD;
  uint8_t bitindex = NOT_A_CONST_BYTE;
  uint32_t retval_round_uint32 = round(arg1);
  discard(retval_round_uint32);
  uint32_t retval_min_uint32 = min(arg1, arg2);
  discard(retval_min_uint32);
  uint32_t retval_max_uint32 = max(arg1, arg2);
  discard(retval_max_uint32);
  uint32_t retval_constrain_uint32 = constrain(arg1, arg2, arg3);
  discard(retval_constrain_uint32);
  uint32_t retval_radians_uint32 = radians(arg1);
  discard(retval_radians_uint32);
  uint32_t retval_degrees_uint32 = degrees(arg1);
  discard(retval_degrees_uint32);
  uint32_t retval_sq_uint32 = sq(arg1);
  discard(retval_sq_uint32);
  uint32_t retval_bitRead_uint32 = bitRead(arg1, bitindex);
  discard(retval_bitRead_uint32);
  uint32_t retval_bitSet_uint32 = bitSet(arg1, bitindex);
  discard(retval_bitSet_uint32);
  uint32_t retval_bitClear_uint32 = bitClear(arg1, bitindex);
  discard(retval_bitClear_uint32);
  uint32_t retval_bitWrite_uint32 = bitWrite(arg1, bitindex, arg3);
  discard(retval_bitWrite_uint32);
  uint32_t retval_bit_uint32 = bit(bitindex);
  discard(retval_bit_uint32);
};

void compile_math_int8() {
  int8_t arg1 = NOT_A_CONST_CHAR;
  int8_t arg2 = NOT_A_CONST_CHAR;
  int8_t arg3 = NOT_A_CONST_CHAR;
  uint8_t bitindex = NOT_A_CONST_BYTE;
  int8_t retval_round_int8 = round(arg1);
  discard(retval_round_int8);
  int8_t retval_min_int8 = min(arg1, arg2);
  discard(retval_min_int8);
  int8_t retval_max_int8 = max(arg1, arg2);
  discard(retval_max_int8);
  int8_t retval_constrain_int8 = constrain(arg1, arg2, arg3);
  discard(retval_constrain_int8);
  int8_t retval_radians_int8 = radians(arg1);
  discard(retval_radians_int8);
  int8_t retval_degrees_int8 = degrees(arg1);
  discard(retval_degrees_int8);
  int8_t retval_sq_int8 = sq(arg1);
  discard(retval_sq_int8);
  int8_t retval_bitRead_int8 = bitRead(arg1, bitindex);
  discard(retval_bitRead_int8);
  int8_t retval_bitSet_int8 = bitSet(arg1, bitindex);
  discard(retval_bitSet_int8);
  int8_t retval_bitClear_int8 = bitClear(arg1, bitindex);
  discard(retval_bitClear_int8);
  int8_t retval_bitWrite_int8 = bitWrite(arg1, bitindex, arg3);
  discard(retval_bitWrite_int8);
  int8_t retval_bit_int8 = bit(bitindex);
  discard(retval_bit_int8);
};

void compile_math_int16() {
  int16_t arg1 = NOT_A_CONST_INT;
  int16_t arg2 = NOT_A_CONST_INT;
  int16_t arg3 = NOT_A_CONST_INT;
  uint8_t bitindex = NOT_A_CONST_BYTE;
  int16_t retval_round_int16 = round(arg1);
  discard(retval_round_int16);
  int16_t retval_min_int16 = min(arg1, arg2);
  discard(retval_min_int16);
  int16_t retval_max_int16 = max(arg1, arg2);
  discard(retval_max_int16);
  int16_t retval_constrain_int16 = constrain(arg1, arg2, arg3);
  discard(retval_constrain_int16);
  int16_t retval_radians_int16 = radians(arg1);
  discard(retval_radians_int16);
  int16_t retval_degrees_int16 = degrees(arg1);
  discard(retval_degrees_int16);
  int16_t retval_sq_int16 = sq(arg1);
  discard(retval_sq_int16);
  int16_t retval_bitRead_int16 = bitRead(arg1, bitindex);
  discard(retval_bitRead_int16);
  int16_t retval_bitSet_int16 = bitSet(arg1, bitindex);
  discard(retval_bitSet_int16);
  int16_t retval_bitClear_int16 = bitClear(arg1, bitindex);
  discard(retval_bitClear_int16);
  int16_t retval_bitWrite_int16 = bitWrite(arg1, bitindex, arg3);
  discard(retval_bitWrite_int16);
  int16_t retval_bit_int16 = bit(bitindex);
  discard(retval_bit_int16);
};
void compile_math_int32() {
  int32_t arg1 = NOT_A_CONST_LONG;
  int32_t arg2 = NOT_A_CONST_LONG;
  int32_t arg3 = NOT_A_CONST_LONG;
  uint8_t bitindex = NOT_A_CONST_BYTE;
  int32_t retval_round_int32 = round(arg1);
  discard(retval_round_int32);
  int32_t retval_min_int32 = min(arg1, arg2);
  discard(retval_min_int32);
  int32_t retval_max_int32 = max(arg1, arg2);
  discard(retval_max_int32);
  int32_t retval_constrain_int32 = constrain(arg1, arg2, arg3);
  discard(retval_constrain_int32);
  int32_t retval_radians_int32 = radians(arg1);
  discard(retval_radians_int32);
  int32_t retval_degrees_int32 = degrees(arg1);
  discard(retval_degrees_int32);
  int32_t retval_sq_int32 = sq(arg1);
  discard(retval_sq_int32);
  int32_t retval_bitRead_int32 = bitRead(arg1, bitindex);
  discard(retval_bitRead_int32);
  int32_t retval_bitSet_int32 = bitSet(arg1, bitindex);
  discard(retval_bitSet_int32);
  int32_t retval_bitClear_int32 = bitClear(arg1, bitindex);
  discard(retval_bitClear_int32);
  int32_t retval_bitWrite_int32 = bitWrite(arg1, bitindex, arg3);
  discard(retval_bitWrite_int32);
  int32_t retval_bit_int32 = bit(bitindex);
  discard(retval_bit_int32);
};


void compile_test_etc() {
  /* Tests random stuff, starting with NOPs */
  _NOP();    // 1 clock
  _NOP2();   // 2 clock
  _NOPNOP(); // 2 clock
  _NOP8();   // 8 clocks in 3 words
  _NOP14();  // 14 clocks in 4 words
  // random 
  randomSeed(NOT_A_CONST_DWORD);
  randomSeed(12345);
  int32_t retval_random = random(12345);
  discard(retval_random);
  retval_random = random(NOT_A_CONST_LONG);
  discard(retval_random);
  retval_random = random(12322, 0);
  discard(retval_random);
  retval_random = random(0, 1234567);
  discard(retval_random);
  retval_random = random(-1234, 0);
  discard(retval_random);
  retval_random = random(0, -123456);
  discard(retval_random);
  retval_random = random(12345, NOT_A_CONST_LONG);
  discard(retval_random);
  retval_random = random(NOT_A_CONST_LONG, 12345);
  discard(retval_random);
  retval_random = random(NOT_A_CONST_LONG, NOT_A_CONST_LONG);
  discard(retval_random);
  // and (ugh) map.
  int32_t retval_map = map(NOT_A_CONST_LONG, 0, 800, -100000, 500000);
  discard(retval_map);
  retval_map = map(NOT_A_CONST_LONG, -999, 999, NOT_A_CONST_LONG, NOT_A_CONST_LONG);
  discard(retval_map);
  retval_map = map(NOT_A_CONST_LONG, NOT_A_CONST_LONG, NOT_A_CONST_LONG, 0, 10000);
  discard(retval_map);
  retval_map = map(NOT_A_CONST_LONG, NOT_A_CONST_LONG, NOT_A_CONST_LONG, NOT_A_CONST_LONG, NOT_A_CONST_LONG);
  discard(retval_map);
  retval_map = map(1234, -2340, 0, -100000, 500000);
  discard(retval_map);
  retval_map = map(2345, -777, 777, NOT_A_CONST_LONG, NOT_A_CONST_LONG);
  discard(retval_map);
  retval_map = map(3456, NOT_A_CONST_LONG, NOT_A_CONST_LONG, 0, 757);
  discard(retval_map);
  retval_map = map(4567, NOT_A_CONST_LONG, NOT_A_CONST_LONG, NOT_A_CONST_LONG, NOT_A_CONST_LONG);
  discard(retval_map);
}
