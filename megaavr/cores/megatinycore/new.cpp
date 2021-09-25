/*
  Copyright (c) 2014 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Updated 2021 Spence Konde to support C++17 (well, give a more useful error message for the new forms at least), and 2020 to support C++14
*/

#include <stdlib.h>

// For C++11, only need the following:
void *operator new  (size_t size) {
  return malloc(size);
}

void *operator new[](size_t size) {
  return malloc(size);
}

void  operator delete  (void * ptr) {
  free(ptr);
}

void  operator delete[](void * ptr) {
  free(ptr);
}

void * operator new  (size_t size, void * ptr) noexcept {
  (void)size;
  return ptr;
}

void * operator new[](size_t size, void * ptr) noexcept {
  (void)size;
  return ptr;
}

// Since C++14, also need two more delete variants...
// Note thnat this CAN break code that compiled amd worked in C++11.
#if (__cpp_sized_deallocation >= 201309L)
  void  operator delete  ( void* ptr, size_t size ) noexcept {
    (void) size;
    free(ptr);
  }
  void  operator delete[]( void* ptr, size_t size ) noexcept {
    (void) size;
    free(ptr);
  }
#endif

// Since C++17, there's four more each for new / delete, to support allocation
// of objects with alignment greater than __STDCPP_DEFAULT_NEW_ALIGNMENT__.
// Almost the entirety of AVR is using C++ 11. Any code that works elsewhere, but NOT on these cores and give these error messages either
// is trying to get alignment that it doesn't actually need, or is - by sheet luck - ending up with the addresses which worked. E
#if (__cpp_aligned_new >= 201606L)

  static void badAalloc(const char*) __attribute__((error("")));
  void* operator new  ( size_t size, std::align_val_t al ) {
    badAalloc("Alignment aware new/delete operators, a C++ 17 feaure, are not supported by this core or any other AVR cores at this point in time");
    (void) al;
    return malloc(size);
  }
  void* operator new[]( size_t size, std::align_val_t al ) {
    badAalloc("Alignment aware new/delete operators, a C++ 17 feaure, are not supported by this core or any other AVR cores at this point in time");
    (void) al;
    return malloc(size);
  }
  void  operator delete  ( void* ptr, std::align_val_t al ) noexcept {
    badAalloc("Alignment aware new/delete operators, a C++ 17 feaure, are not supported by this core or any other AVR cores at this point in time");
    (void) al;
    free(ptr);
  }
  void  operator delete[]( void* ptr, std::align_val_t al ) noexcept {
    badAalloc("Alignment aware new/delete operators, a C++ 17 feaure, are not supported by this core or any other AVR cores at this point in time");
    (void) al;
    free(ptr);
  }
  void  operator delete  ( void* ptr, size_t size, std::align_val_t al ) noexcept{
    badAalloc("Alignment aware new/delete operators, a C++ 17 feaure, are not supported by this core or any other AVR cores at this point in time");
    (void) al;
    (void) size;
    free(ptr);
  }
  void  operator delete[]( void* ptr, size_t size, std::align_val_t al ) noexcept {
    badAalloc("Alignment aware new/delete operators, a C++ 17 feaure, are not supported by this core or any other AVR cores at this point in time");
    (void) al;
    (void) size;
    free(ptr);
  }
#endif
