extern void putchar_via_gadget(output_gadget_t* gadget, char c);

// Possibly-write the string-terminating '\0' character
extern void append_termination_with_gadget(output_gadget_t* gadget);

// We can't use putchar_ as is, since our output gadget
// only takes pointers to functions with an extra argument
extern void putchar_wrapper(char c, void* unused);

extern output_gadget_t discarding_gadget();

extern output_gadget_t buffer_gadget(char* buffer, size_t buffer_size);

extern output_gadget_t function_gadget(void (*function)(char, void*), void* extra_arg);

extern output_gadget_t extern_putchar_gadget();

// internal secure strlen
// @return The length of the string (excluding the terminating 0) limited by 'maxsize'
// @note strlen uses size_t, but wes only use this function with printf_size_t
// variables - hence the signature.
extern printf_size_t strnlen_s_(const char* str, printf_size_t maxsize);

// internal test if char is a digit (0-9)
// @return true if char is a digit
extern bool is_digit_(char ch);


// internal ASCII string to printf_size_t conversion
extern printf_size_t atou_(const char** str);


// output the specified string in reverse, taking care of any zero-padding
extern void out_rev_(output_gadget_t* output, const char* buf, printf_size_t len, printf_size_t width, printf_flags_t flags);


// Invoked by print_integer after the actual number has been printed, performing necessary
// work on the number's prefix (as the number is initially printed in reverse order)
extern void print_integer_finalization(output_gadget_t* output, char* buf, printf_size_t len, bool negative, numeric_base_t base, printf_size_t precision, printf_size_t width, printf_flags_t flags);

// An internal itoa-like function
extern void print_integer(output_gadget_t* output, printf_unsigned_value_t value, bool negative, numeric_base_t base, printf_size_t precision, printf_size_t width, printf_flags_t flags);

#if (PRINTF_SUPPORT_DECIMAL_SPECIFIERS || PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS)

// Stores a fixed-precision representation of a double relative
// to a fixed precision (which cannot be determined by examining this structure)
struct double_components {
  int_fast64_t integral;
  int_fast64_t fractional;
    // ... truncation of the actual fractional part of the double value, scaled
    // by the precision value
  bool is_negative;
};

#define NUM_DECIMAL_DIGITS_IN_INT64_T 18
#define PRINTF_MAX_PRECOMPUTED_POWER_OF_10  NUM_DECIMAL_DIGITS_IN_INT64_T
static const double powers_of_10[NUM_DECIMAL_DIGITS_IN_INT64_T] = {
  1e00, 1e01, 1e02, 1e03, 1e04, 1e05, 1e06, 1e07, 1e08,
  1e09, 1e10, 1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17
};

#define PRINTF_MAX_SUPPORTED_PRECISION NUM_DECIMAL_DIGITS_IN_INT64_T - 1


// Break up a double number - which is known to be a finite non-negative number -
// into its base-10 parts: integral - before the decimal point, and fractional - after it.
// Taken the precision into account, but does not change it even internally.
extern struct double_components get_components(double number, printf_size_t precision);

#if PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS
struct scaling_factor {
  double raw_factor;
  bool multiply; // if true, need to multiply by raw_factor; otherwise need to divide by it
};

extern double apply_scaling(double num, struct scaling_factor normalization);

extern double unapply_scaling(double normalized, struct scaling_factor normalization);

extern struct scaling_factor update_normalization(struct scaling_factor sf, double extra_multiplicative_factor);

extern struct double_components get_normalized_components(bool negative, printf_size_t precision, double non_normalized, struct scaling_factor normalization, int floored_exp10);
#endif // PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS

extern void print_broken_up_decimal(
  struct double_components number_, output_gadget_t* output, printf_size_t precision,
  printf_size_t width, printf_flags_t flags, char *buf, printf_size_t len);

      // internal ftoa for fixed decimal floating point
extern void print_decimal_number(output_gadget_t* output, double number, printf_size_t precision, printf_size_t width, printf_flags_t flags, char* buf, printf_size_t len);

#if PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS

// A floor function - but one which only works for numbers whose
// floor value is representable by an int.
extern int bastardized_floor(double x);

// Computes the base-10 logarithm of the input number - which must be an actual
// positive number (not infinity or NaN, nor a sub-normal)
extern double log10_of_positive(double positive_number);


extern double pow10_of_int(int floored_exp10);

extern void print_exponential_number(output_gadget_t* output, double number, printf_size_t precision, printf_size_t width, printf_flags_t flags, char* buf, printf_size_t len);
#endif  // PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS

extern void print_floating_point(output_gadget_t* output, double value, printf_size_t precision, printf_size_t width, printf_flags_t flags, bool prefer_exponential);

#endif  // (PRINTF_SUPPORT_DECIMAL_SPECIFIERS || PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS)

// Advances the format pointer past the flags, and returns the parsed flags
// due to the characters passed
extern printf_flags_t parse_flags(const char** format);
