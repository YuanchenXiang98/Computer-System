#include "iobuffer.h"
#include "tokeniser-extras.h"

// to shorten the code
using namespace std ;
using namespace CS_IO_Buffers ;

namespace Assignment_Tokeniser
{

    // check if a char matches another char or is a member of a character group
    // eg char_isa('3',cg_digit) returns true
    bool char_isa(int ch,int cg)
    {

        // single character groups use their character as their value
        // do not add them to the switch statement

        if ( ch == cg ) return true ;

        switch(cg)
        {
        case cg_wspace:              // characters that start rule wspace
            switch(ch)
            {
            case '\t':
            case '\n':
            case '\r':
            case ' ':
                return true ;
            default:
                return false ;
            }

        // complete a switch statement for each of the following
        case cg_identifier:         // characters that start rule identifier
            switch(ch)
            {
            case 'a' ... 'z':
            case 'A' ... 'Z':
            case '$':
                return true;
            default:
                return false;
            }
        case cg_letter:             // characters that start rule letter
            switch(ch)
            {
            case 'a' ... 'z':
            case 'A' ... 'Z':
            case '0' ... '9':
            case '-':
            case '$':
            case '.':
                return true;
            default:
                return false;
            }
        case cg_number:             // characters that start rule number
            switch(ch)
            {
            case '0' ... '9':
                return true;
            default:
                return false;
            }
        case cg_integer:            // characters that start rule integer
            switch(ch)
            {
            case '0' ... '9':
                return true;
            default:
                return false;
            }
        case cg_scientific:         // characters that start rule scientific
            switch(ch)
            {
            case '0' ... '9':
                return true;
            default:
                return false;
            }
        case cg_digit:              // characters that start rule digit
            switch(ch)
            {
            case '0' ... '9':
                return true;
            default:
                return false;
            }
        case cg_digit19:            // characters that start rule digit19
            switch(ch)
            {
            case '1' ... '9':
                return true;
            default:
                return false;
            }
        case cg_exponent:           // characters that start rule exponent
            switch(ch)
            {
            case 'e':
            case 'E':
                return true;
            default:
                return false;
            }
        case cg_eee:                // characters that start rule eee
            switch(ch)
            {
            case 'e':
            case 'E':
                return true;
            default:
                return false;
            }
        case cg_sign:               // characters that start rule eee
            switch(ch)
            {
            case '+':
            case '-':
                return true;
            default:
                return false;
            }
        case cg_instring:           // characters that start rule instring
            switch(ch)
            {
            case ' ':
            case '!':
            case '#' ... '~':
            case 0X0C00 ... 0X0C7F:
                return true;
            default:
                return false;
            }
        case cg_telegu:             // characters that start rule telegu
            switch(ch)
            {
            case 0x0C00 ... 0x0C7F:
                return true;
            default:
                return false;
            }
        case cg_symbol:             // characters that start rule symbol
            switch(ch)
            {
            case '@':
            case '~':
            case '=':
            case '-':
            case '+':
            case '/':
            case '*':
            case '<':
            case '>':
            case '{':
            case '}':
            case '(':
            case ')':
            case '[':
            case ']':
            case '.':
                return true;
            default:
                return false;
            }
        case cg_comment_char:       // characters that start rule comment_char
            switch(ch)
            {
            case '\t':
            case '\r':
            case ' ' ... '~':
            case 0X0530 ... 0X058F:
            case 0XFB13 ... 0XFB17:
                return true;
            default:
                return false;
            }
        case cg_armenian:           // characters that start rule armenian
            switch(ch)
            {
            case 0X0530 ... 0X058F:
            case 0XFB13 ... 0XFB17:
                return true;
            default:
                return false;
            }

        default:
            return false ;
        }
    }

    // work out the kind of a parsed token, this may call keyword_or_identifier()
    // the spelling is a valid token or "" if at end of input
    TokenKind classify_spelling(string spelling)
    {
        if ( spelling == "" ) return tk_eoi ;
        switch(spelling[0])
        {
        case '\t':
            return tk_tab;
        case '\n':
            return tk_newline;
        case '\r':
            return tk_carriage_return;
        case ' ':
            return tk_space;
        case 'a' ... 'z':
        case 'A' ... 'Z':
        case '$':
            return keyword_or_identifier(spelling);
        case '0' ... '9':
        {
            int len = spelling.length();
            for(int i = 0; i < len; i++)
            {
                if(spelling[i] == '.')
                {
                    return tk_scientific;
                }
            }
            return tk_integer;
        }
        case '@':
            return tk_at;
        case '~':
            if(spelling.length() == 1)
            {
                return tk_not;
            }
            return tk_ne;
        case '=':
            if(spelling.length() == 1)
            {
                return tk_assign;
            }
            return tk_eq;
        case '-':
            return tk_sub;
        case '+':
            return tk_add;
        case '*':
            return tk_mult;
        case '/':
        {
            if(spelling.length() == 1)
            {
                return tk_div;
            }
            return tk_eol_comment;
        }
        case '{':
            return tk_lcb;
        case '}':
            return tk_rcb;
        case '(':
            return tk_lrb;
        case ')':
            return tk_rrb;
        case '[':
            return tk_lsb;
        case ']':
            return tk_rsb;
        case '.':
            return tk_dot;
        case '#':
            return tk_hash_comment;
        case '"':
            return tk_string;
        case '<':
            if(spelling.length() == 3)
            {
                return tk_ll_shift;
            }
            return tk_la_shift;
        case '>':
            if(spelling.length() == 3)
            {
                return tk_rl_shift;
            }
            return tk_ra_shift;
        default:
            return tk_oops;
        }
    }

    // work out the correct spelling to use in the Token object being created by new_token()
    // the spelling is a valid token and kind is its kind
    string correct_spelling(TokenKind kind,string spelling)
    {
        if ( spelling == "" ) return "" ;
        if ( kind == tk_string )
        {
            int len = spelling.length();
            string new_spelling = "";
            for(int i = 1; i < len - 1; i++)
            {
                new_spelling = new_spelling + spelling[i];
            }
            return new_spelling;
        }
        if ( kind == tk_eol_comment )
        {
            int len = spelling.length();
            string new_spelling = "";
            for(int i = 2; i < len - 1; i++)
            {
                new_spelling = new_spelling + spelling[i];
            }
            return new_spelling;
        }
        if ( kind == tk_hash_comment )
        {
            int len = spelling.length();
            string new_spelling = "";
            for(int i = 1; i < len - 1; i++)
            {
                new_spelling = new_spelling + spelling[i];
            }
            return new_spelling;
        }
        if ( kind == tk_scientific )
        {
            int len = spelling.length();
            int eposition = -1;
            string new_spelling = "";
            for(int i = 0; i < len; i++)
            {
                if (spelling[i] == 'e' || spelling[i] == 'E')
                {
                    eposition = i;
                }
            }
            string leftDigit = "";
            string rightDigit = "";
            if(eposition != -1)
            {
                leftDigit = spelling.substr(0, eposition);
                rightDigit = spelling.substr(eposition + 1, len - 1 - eposition);
            }
            else
            {
                leftDigit = spelling;
                rightDigit = "+0";
            }
            int shift_times = 0;
            double leftDigitToD = stod(leftDigit);
            if (leftDigitToD == 0)
            {
                return "0";
            }
            else if (leftDigitToD > 10)
            {
                while (leftDigitToD > 10)
                {
                    leftDigitToD /= 10;
                    shift_times++;
                }
            }
            else if (leftDigitToD < 1)
            {
                while(leftDigitToD < 1)
                {
                    leftDigitToD *= 10;
                    shift_times--;
                }
            }
            leftDigit = std::to_string(leftDigitToD);
            int length_left = leftDigit.length();
            int count = 0;
            for(int i = length_left - 1; i >= 0; i--)
            {
                if (leftDigit[i] == '0')
                {
                    count++;
                }
                else
                {
                    break;
                }
            }
            if(leftDigit[length_left - count - 1] == '.')
            {
                count++;
            }
            leftDigit = leftDigit.substr(0, length_left - count);
            int rightDigitToI = 0;
            if (rightDigit.length() != 0)
            {
                if (rightDigit.length() == 1)
                {
                    if (rightDigit[0] != '+' && rightDigit[0] != '-')
                    {
                        rightDigitToI = std::stoi(rightDigit, nullptr, 10);
                    }
                }
                else
                {
                    rightDigitToI = std::stoi(rightDigit, nullptr, 10);
                }
            }
            rightDigit = std::to_string(rightDigitToI + shift_times);
            if (rightDigitToI + shift_times >= 0)
            {
                rightDigit = "+" + rightDigit;
            }
            return leftDigit + "e" + rightDigit;
        }
        return spelling ;
    }
}