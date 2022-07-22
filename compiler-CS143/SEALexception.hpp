#ifndef SEAL_EXCEPTION_HPP
#define SEAL_EXCEPTION_HPP

class Exception{};

class Out_Of_Boundary_ERROR : public Exception{};

class Syntactic_ERROR : public Exception{};

class Lvalue_ERROR : public Syntactic_ERROR{};

class Parentheses_Unmatched_ERROR : public Syntactic_ERROR{};

class Unrecognized_symbol_ERROR : public Syntactic_ERROR{};

class Semantic_ERROR : public Exception{};

class Redefinition_ERROR : public Semantic_ERROR{};

class Undefined_Identifier_ERROR : public Semantic_ERROR{};

class Unmathced_Type_ERROR : public Semantic_ERROR{};

class Invalid_Continue_ERROR : public Semantic_ERROR{};

class Invalid_Break_ERROR : public Semantic_ERROR{};

class Argument_ERROR : public Semantic_ERROR{};




#endif