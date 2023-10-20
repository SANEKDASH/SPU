#ifndef debug
#define debug

#ifdef DEBUG
#define CHECK(expression) CheckIt(expression, __LINE__, __func__, __FILE__)
#else
#define CHECK(expression) ;
#endif




//! Checks expr expression for falsity (used when debugging)    // I want my bag back!
//!
//! @param    expr expression being checked
//! @param    line line number
//! @param    func_name name of the function in which the check is performed
//! @param    file_name name of the file in which the check is performed

void CheckIt(bool expr,
             int line,
             const char* func_name,
             const char* file_name);

void* save_calloc(size_t data_len,
                  size_t data_size,
                  int line,
                  const char *func,
                  const char *file);

void save_free(void *ptr,
              int line,
              const char *func,
              const char *file);

#endif
