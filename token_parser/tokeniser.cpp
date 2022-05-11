// tokeniser implementation for the workshop example language

#include "iobuffer.h"
#include "tokeniser-extras.h"

// to shorten the code
using namespace std ;
using namespace CS_IO_Buffers ;

// we are extending the Assignment_Tokeniser namespace

namespace Assignment_Tokeniser
{
    static void parse_identifier();
    static void parse_integer();
    static void parse_number();
    static void parse_scientific();
    static void parse_fraction();
    static void parse_exponent();
    static void parse_string();
    static void parse_symbol();
    static void parse_mult();
    static void parse_ne();
    static void parse_eq();
    static void parse_left_shift();
    static void parse_right_shift();
    static void parse_eol_comment();
    static void parse_eol_suffix();
    static void parse_hash_comment();

    // identifier ::= ('a'-'z'|'A'-'Z'|'$') letter*
    static void parse_identifier(){
        next_char_mustbe(cg_identifier);
        while (next_char_isa(cg_letter)){
            read_next_char();
        }
    }

    // number ::= integer | scientific
    static void parse_fraction()
    {
        next_char_mustbe('.');
        while(next_char_isa(cg_digit))
        {
            read_next_char();
        }
    }

    // this is the parser for exponent
    static void parse_exponent()
    {
        next_char_mustbe(cg_eee);
        if(next_char_isa(cg_sign))
        {
            read_next_char();
        }
        if(next_char_isa(cg_integer))
        {
            parse_integer();
        }
    }

    // this is the parser_integer
    static void parse_integer()
    {
        if(next_char_isa('0'))
        {
            read_next_char();
        }else
        {
            next_char_mustbe(cg_digit19);
            while(next_char_isa(cg_digit))
            {
                read_next_char();
            }
        }
    }

    // start with '.'
    static void parse_scientific()
    {
        parse_fraction();
        if(next_char_isa(cg_exponent))
        {
            parse_exponent();
        }
    }

    // this is parser for number
    static void parse_number()
    {
        parse_integer();
        if(next_char_isa('.'))
        {
            parse_scientific();
        }
    }

    // string ::= '"' instring* '"' ?????????????
    static void parse_string()
    {
        next_char_mustbe('"');
        while(next_char_isa(cg_instring))
        {
            read_next_char();
        }
        next_char_mustbe('"');
    }

    // parse symbol
    static void parse_symbol()
    {
        if (next_char_isa('*')) parse_mult(); else
        if (next_char_isa('~')) parse_ne(); else
        if (next_char_isa('=')) parse_eq(); else
        if (next_char_isa('<')) parse_left_shift(); else
        if (next_char_isa('>')) parse_right_shift(); else
        read_next_char();
    }

    // *=
    static void parse_mult()
    {
        read_next_char();
        next_char_mustbe('=');
    }

    // ~ or ~=
    static void parse_ne()
    {
        read_next_char();
        if(next_char_isa('='))
        {
            read_next_char();
        }
    }

    // = or ==
    static void parse_eq()
    {
        read_next_char();
        if(next_char_isa('='))
        {
            read_next_char();
        }
    }

    // << or <<<
    static void parse_left_shift()
    {
        read_next_char();
        next_char_mustbe('<');
        if(next_char_isa('<'))
        {
            read_next_char();
        }
    }

    // >> or >>>
    static void parse_right_shift()
    {
        read_next_char();
        next_char_mustbe('>');
        if(next_char_isa('>'))
        {
            read_next_char();
        }
    }

    // parse eol comment
    static void parse_eol_comment()
    {
        next_char_mustbe('/');
        if(next_char_isa('/'))
        {
            parse_eol_suffix();
        }
    }

    // this is the parser for eol suffix
    static void parse_eol_suffix()
    {
        next_char_mustbe('/');
        while(next_char_isa(cg_comment_char))
        {
            read_next_char();
        }
        next_char_mustbe('\n');
    }

    // this is the parser hash comment
    static void parse_hash_comment()
    {
        next_char_mustbe('#');
        while(next_char_isa(cg_comment_char))
        {
            read_next_char();
        }
        next_char_mustbe('\n');
    }

    // token ::= ...
    static void parse_token()
    {
        if ( next_char_isa(cg_wspace) ) read_next_char() ; else
        if ( next_char_isa(cg_identifier) ) parse_identifier() ; else
        if ( next_char_isa(cg_number) ) parse_number() ; else
        if ( next_char_isa('"') ) parse_string() ; else
        // if ( next_char_isa(cg_keyword) ) parse_keyword() ; else
        if ( next_char_isa('/') ) parse_eol_comment(); else
        if ( next_char_isa(cg_symbol) ) parse_symbol() ; else
        if ( next_char_isa('#') ) parse_hash_comment() ; else
        if ( next_char_isa(EOF) ) ; else
        did_not_find_start_of_token() ;
    }

    // parse the next token in the input and return a new
    // Token object that describes its kind and spelling
    // Note: you must not call new_token() anywhere else in your program
    // Note: you should not modify this function
    //
    Token read_next_token()
    {
        parse_token() ;

        return new_token() ;
    }
}