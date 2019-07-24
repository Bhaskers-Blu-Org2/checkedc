// Feature tests of parsing new Checked C pointer types.
//
// * Test that checked pointer types can be used as parameter types, return
//   types, local variable types, structure member types, in typedefs, and as
//   arguments to operators that take types.
// * Test parsing of array types and function types combined with checked
//   ponter types.
// * Test the case of nested checked types such as ptr<ptr<T>>, where >>
//   could be mistaken for the right-shift operator.
//
// The following lines are for the LLVM test harness:
//
// RUN: %clang_cc1 -verify %s
// expected-no-diagnostics

#include <stdchecked.h>


typedef struct node {
  int val;
  mmsafe_ptr<struct node> next;
  unsigned long ID;
} Node;


//
// parameters have new pointer types
//

extern void f1(ptr<int> p, int y) {
   *p = y;
}

extern void f2(const ptr<int> p, int y) {
   *p = y;
}

extern void f3(ptr<const int> p, int y) {
   y = *p;
}

extern void f4(ptr<ptr<int>> p, int y) {
   **p = y;
}

extern void f5(ptr<ptr<ptr<int>>> p, int y) {
   ***p = y;
}

extern void f6(array_ptr<int> p : count(1), int y) {
    *p = y;
}

extern void f7(array_ptr<int> p : count(1), int y) {
   *p = y;
   f6(p, y);
}

extern void f8(nt_array_ptr<int> p : count(1), int y) {
  *p = y;
}

extern void f9(nt_array_ptr<int> p : count(1), int y) {
  *p = y;
  f8(p, y);
}

extern void f10(nt_array_ptr<nt_array_ptr<int>> p : count(1),
                nt_array_ptr<int> y) {
  *p = y;
}

extern void f11(mmsafe_ptr<Node> p) {
  p->val = 0;
}

extern void f12(const mmsafe_ptr<Node> p) {
  p->ID = 1;
}

extern void f13(mmsafe_ptr<const Node> p, int y) {
  y = p->val;
}

extern void f14(mmsafe_ptr<Node> p, mmsafe_ptr<Node> next) {
    p->next = next;
}

//
// Second parameter is a new pointer type
//

extern void g1(int y, ptr<int> p) {
   *p = y;
}

extern void g2(int y, const ptr<int> p) {
   *p = y;
}

extern void g3(int y, ptr<const int> p) {
   y = *p;
}

extern void g4(int y, ptr<ptr<int>> p) {
   **p = y;
}

extern void g5(int y, ptr<ptr<ptr<int>>> p) {
   ***p = y;
}


extern void g6(int y, array_ptr<int> p : count(1)) {
   *p = y;
   f7(p, y);
}

extern void g7(int y, nt_array_ptr<int> p : count(1)) {
  *p = y;
  f9(p, y);
}


//
// returns a new pointer type
//

extern ptr<int> h1(int y, ptr<int> p) {
   *p = y;
   return p;
}

extern const ptr<int> h2(int y, const ptr<int> p) {
   *p = y;
   return p;
}

extern ptr<const int> h3(int y, array_ptr<ptr<const int>> p : count(1)) {
   y = **p;
   return *p;
}

extern ptr<ptr<int>> h4(int y, ptr<ptr<int>> p) {
   **p = y;
   return p;
}

extern ptr<ptr<ptr<int>>> h5(int y, ptr<ptr<ptr<int>>> p) {
   ***p = y;
   return p;
}

extern array_ptr<int> h6(int y, array_ptr<int> p) {
  return p;
}


extern array_ptr<ptr<int>> h7(int y, array_ptr<ptr<int>> p : count(1)) {
  **p = y;
  return p;
}

extern nt_array_ptr<int> h8(int y, nt_array_ptr<int> p) {
  return p;
}


extern nt_array_ptr<ptr<int>> h9(int y, nt_array_ptr<ptr<int>> p : count(1)) {
  **p = y;
  return p;
}

extern mmsafe_ptr<Node> h10(int y, mmsafe_ptr<Node> p) {
  p->val = y;
  return p;
}

extern const mmsafe_ptr<Node> h11(int y, const mmsafe_ptr<Node> p) {
  p->val = y;
  return p;
}

extern mmsafe_ptr<const Node> h12(int y, mmsafe_ptr<const Node> p) {
    y = p->val;
    return p;
}

//
// Local variables with pointer types
//

extern void k1(int y)
{
   int v = y;
   ptr<int> t1 = &v;
   array_ptr<int> t2 : count(1) = &v;
   array_ptr<ptr<int>> t3  : count(1) = &t1;
   nt_array_ptr<int> t4 = 0;
   *t1 = 0;
   *t2 = 0;
   *t3 = 0;

   Node n;
   mmsafe_ptr<Node> node = &n;
   node->val = v;
}

//
// Struct member with pointer types;
//

struct Vector {
    array_ptr<float> data : count(len);
    int len;
};

extern int Multiply(struct Vector vec1, struct Vector vec2) {
    if (vec1.len != vec2.len) {
       return 1;
    }
    for (int i = 0; i < vec1.len; i++) {
        vec1.data[i] *= vec2.data[i];
    }
    return 0;
}

extern int Multiply2(ptr<struct Vector> vec1p, ptr<struct Vector> vec2p) {
    if (vec1p->len != vec2p->len) {
       return 1;
    }
    for (int i = 0; i < vec1p->len; i++) {
        vec1p->data[i] *= vec2p->data[i];
    }
    return 0;
}

struct StringWrapper {
  nt_array_ptr<char> str : count(0);
};

extern void setValOfNextNode(mmsafe_ptr<Node> p, int y) {
    p->next->val = y;
}

//
// Declaring pointers to functions
//

// An unchecked pointer to a function that takes two integer parameters and returns an integer.
int (*unchecked_ptr_to_func)(int x, int y);
// A ptr to a function that takes two integer parameters and returns an integer
ptr<int (int x, int y)> ptrfunc;
// Not allowed: an array_ptr of a function type:
// array_ptr<int (int x, int y)> array_ptrfunc;
// Function types have no size, so bounds checking does not make sense.
//
// Allowed: An array_ptr to an array of function pointers.
array_ptr<ptr<int(int x, int  y)>> array_ptr_of_ptrfunc;
nt_array_ptr<ptr<int(int x, int  y)>> nullterm_array_ptr_of_ptrfunc;

//
// Declaring pointers to arrays and arrays of pointers
//
int (*unchecked_ptr_to_array)[5];
ptr<int[5]> ptr_to_array;
array_ptr<int[5]> array_ptr_to_array;
// not allowed: null terminated array_ptr to array
// nt_array_ptr<int[5]> nullterm_array_ptr_to_array;

int(*unchecked_ptr_to_incomplete_array)[];
ptr<int[]> ptr_to_incomplete_array;
array_ptr<int[]> array_ptr_to_incomplete_array;
// not allowed: null terminated array_ptr to incomplete array
// nt_array_ptr<int[]> nullterm_array_ptr_to_incomplete_array;

// Declaring arrays of pointers
int *array_of_unchecked_ptrs[5];
ptr<int> array_of_ptrs[5];
array_ptr<int> array_of_array_ptrs[5];
nt_array_ptr<int> array_of_nullterm_pointers[5];
mmsafe_ptr<Node> array_of_mmsafe_ptrs[5];
array_ptr<mmsafe_ptr<Node>> array_of_array_mmsafe_ptrs[5];

// Declaring null-terminated arrays of pointers
int *nullterm_array_of_unchecked_ptrs nt_checked[5];
ptr<int> nullterm_array_of_ptrs nt_checked[5];
array_ptr<int> nullterm_array_of_array_ptrs nt_checked[5];
nt_array_ptr<int> nullterm_array_of_nullterm_pointers nt_checked[5];
mmsafe_ptr<Node> nullterm_array_of_mmsafe_ptrs nt_checked[5];

// Declare an unchecked pointer to arrays of pointers
int *(*uncheckedptr_to_array_of_unchecked_ptrs)[5];
ptr<int>(*unchecked_ptr_to_array_of_ptrs)[5];
array_ptr<int>(*unchecked_ptr_to_array_of_array_ptrs)[5];
nt_array_ptr<int>(*unchecked_ptr_to_array_of_null_term_array_ptrs)[5];
mmsafe_ptr<Node>(*uncheckedptr_to_array_of_mmsafe_ptrs)[5];

int *(*uncheckedptr_to_nullterm_array_of_unchecked_ptrs) nt_checked[5];
ptr<int>(*unchecked_ptr_to_nullterm_array_of_ptrs) nt_checked[5];
array_ptr<int>(*unchecked_ptr_to_null_termarray_of_array_ptrs) nt_checked[5];
nt_array_ptr<int>(*unchecked_ptr_to_null_term_array_of_null_term_array_ptrs)nt_checked[5];
mmsafe_ptr<Node>(*unchecked_ptr_to_nullterm_array_of_mmsafe_ptrs)nt_checked[5];

// Declare ptr to arrays of pointers
ptr<int *[5]> ptr_to_array_of_unchecked_ptrs;
ptr<ptr<int>[5]> ptr_to_array_of_ptrs;
ptr<array_ptr<int>[5]> ptr_to_array_of_array_ptrs;
ptr<nt_array_ptr<int>[5]> ptr_to_array_of_nullterm_array_ptrs;
ptr<mmsafe_ptr<Node>[5]> ptr_to_array_of_mmsafe_ptrs;

// Declare ptr to nullterm arrays of pointers
ptr<int *nt_checked[5]> ptr_to_nullterm_array_of_unchecked_ptrs;
ptr<ptr<int>nt_checked[5]> ptr_to_nullterm_array_of_ptrs;
ptr<array_ptr<int>nt_checked[5]> ptr_to_nullterm_array_of_array_ptrs;
ptr<nt_array_ptr<int>nt_checked[5]> ptr_to_nullterm_array_of_nullterm_array_ptrs;
ptr<mmsafe_ptr<Node>nt_checked[5]> ptr_to_nullterm_array_of_mmsafe_ptrs;


// Declare ptr to an array of function pointers
ptr<int (*[5])(int x, int y)> ptr_to_array_of_unchecked_func_ptrs;
ptr<ptr<int (int x, int y)>[5]> ptr_to_array_of_checked_func_ptrs;
// Make parameter and return types be ptrs too.
ptr<ptr<ptr<int> (ptr<int> x, ptr<int> y)>[5]> ptr_to_array_of_checked_func_ptrs_with_ptr_parameters;

//
// Typedefs using checked pointer types
//

typedef ptr<int> t_ptr_int;
typedef ptr<int (int x, int y)> t_ptr_func;
typedef array_ptr<int> t_array_ptr_int;
typedef array_ptr<ptr<int>> t_array_ptr_ptr_int;
typedef nt_array_ptr<int> t_nullterm_array_ptr_int;
typedef nt_array_ptr<ptr<int>> t_nullterm_array_ptr_ptr_int;
typedef mmsafe_ptr<Node> t_mmsafe_ptr_node;

//
// Operators that take types
//

void parse_operators_with_types(void) {
    int s1 = sizeof(ptr<int>);
    int s2 = sizeof(array_ptr<int>);
    int s3 = sizeof(nt_array_ptr<int>);
    int s4 = sizeof(ptr<int[5]>);
    int s5 = sizeof(array_ptr<int[5]>);
    // not allowed: sizeof(nt_array_ptr<int[5]>);
    int s6 = sizeof(ptr<int>[5]);
    int s7 = sizeof(array_ptr<int>[5]);
    int s8 = sizeof(nt_array_ptr<int>[5]);
    // C11 spec says sizeof function types is illegal, but clang accepts it.
    int s9 = sizeof(int(int x, int y));
    int s10 = sizeof(ptr<int>(int x, int y));
    // These are OK
    int s11 = sizeof(ptr<int(int x, int y)>);
    int s12 = sizeof(int(*)(int x, int y));

    int s20 = _Alignof(ptr<int>);
    int s21 = _Alignof(array_ptr<int>);
    int s22 = _Alignof(nt_array_ptr<int>);
    int s23 = _Alignof(ptr<int[5]>);
    int s24 = _Alignof(array_ptr<int[5]>);
    // not allowed: _Alignof(nt_array_ptr<int[5]>);
    int s25 = _Alignof(ptr<int>[5]);
    int s26 = _Alignof(array_ptr<int>[5]);
    int s27 = _Alignof(nt_array_ptr<int>[5]);
    // C11 spec says _Alignof function types is illegal, but clang accepts it.
    int s28 = _Alignof(int(int x, int y));
    int s29 = _Alignof(ptr<int>(int x, int y));
    // These are OK
    int s30 = _Alignof(ptr<int(int x, int y)>);
    int s31 = _Alignof(int(*)(int x, int y));

    int s32 = sizeof(mmsafe_ptr<Node>);
    int s33 = sizeof(mmsafe_ptr<Node>[5]);
    int s34 = sizeof(mmsafe_ptr<Node>(int x, int y));
    int s35 = sizeof(ptr<mmsafe_ptr<Node>(int x, int y)>);

    int s36 = _Alignof(mmsafe_ptr<Node>);
    int s37 = _Alignof(mmsafe_ptr<Node>[5]);
    int s38 = _Alignof(mmsafe_ptr<Node>(int x, int y));
    int s39 = _Alignof(ptr<mmsafe_ptr<Node>(int x, int y)>);

    // Test parsing of some cast operations that should pass checking
    // of bounds declarations.
    int x = 0;
    int arr[5];
    ptr<int> px = (ptr<int>) &x;
    array_ptr<int> pax = (array_ptr<int>) &x;
    int nt_arr nt_checked[5];
    nt_array_ptr<int> nt_pax = (nt_array_ptr<int>) nt_arr;
    // ptr to array type
    ptr<int[5]> parr = 0;
    parr = &arr;
    // ptr to function type
    ptr<int (int x, int y)> pfunc = (ptr<int (int x, int y)>) 0;
    ptr<ptr<int (int x, int y)>[5]> ptr_to_pfunc_arr = (ptr<ptr<int (int x, int y)>[5]>) 0;
}
