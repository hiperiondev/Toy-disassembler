#ifndef TOY_LITERALS_H_
#define TOY_LITERALS_H_

typedef enum TOY_LITERAL_TYPE {
    TOY_LITERAL_NULL,       //
    TOY_LITERAL_BOOLEAN,    //
    TOY_LITERAL_INTEGER,    //
    TOY_LITERAL_FLOAT,      //
    TOY_LITERAL_STRING,     //
    TOY_LITERAL_ARRAY,      //
    TOY_LITERAL_DICTIONARY, //
    TOY_LITERAL_FUNCTION,   //
    TOY_LITERAL_IDENTIFIER, //
    TOY_LITERAL_TYPE,       //
    TOY_LITERAL_OPAQUE,     //
    TOY_LITERAL_ANY,        //

    // these are meta-level types - not for general use
    TOY_LITERAL_TYPE_INTERMEDIATE,       // used to process types in the compiler only
    TOY_LITERAL_ARRAY_INTERMEDIATE,      // used to process arrays in the compiler only
    TOY_LITERAL_DICTIONARY_INTERMEDIATE, // used to process dictionaries in the compiler only
    TOY_LITERAL_FUNCTION_INTERMEDIATE,   // used to process functions in the compiler only
    TOY_LITERAL_FUNCTION_ARG_REST,       // used to process function rest parameters only
    TOY_LITERAL_FUNCTION_NATIVE,         // for handling native functions only
    TOY_LITERAL_FUNCTION_HOOK,           // for handling hook functions within literals only
    TOY_LITERAL_INDEX_BLANK,             // for blank indexing i.e. arr[:]
} toy_literal_type_t;

#endif /* TOY_LITERALS_H_ */
