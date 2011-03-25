/*
 * Copyright (c) 2011 Stefan 'psYchotic' Zwanenburg <stefanhetzwaantje@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/*
 * This program is a recursive descent parser for JSON.
 * All it does now is perform parsing. It does not in any way translate the
 * JSON data to some other representation, in memory or otherwise.
 * Potential use for this is for validating JSON input.
 * To compile: make sure you enable support for C99. Example:
 *      gcc -o json json.c -Wall -std=c99
 * To get extra output during parsing, define DEBUG. Example:
 *      gcc -o json json.c -Wall -std=c99 -DDEBUG
 * For information about JSON, go to http://www.json.org.
 */

#include <wctype.h>
#include "json.h"
#include "parser.h"

wchar_t next = L'\0';

/*
 * Grammar:
 * json            =   ( object | array ), EOF;
 */
bool json() {
    bool success = false;
    enter("json");

    if (object() || array());

    if (!expect(WEOF))
        error("json() expected EOF");

    success = true;
    leave("json");

    return success;
}

/*
 * object          =   "{", [ pair { ",", pair } ], "}";
 */
bool object() {
    bool success = false;
    enter("object");
    if (expect('{')) {
        if (pair()) {
            while (expect(',')) {
                if (!pair()) {
                    error("object() expected another pair after ','");
                }
            }
        }
        if (expect('}')) {
            success = true;
        } else {
            error("object() expected a pair or '}' to end object definition");
        }
    }
    leave("object");

    return success;
}

/*
 * pair            =   string, ":", value;
 */
bool pair() {
    int success = false;
    enter("pair");
    if (string()) {
        if (expect(':')) {
            if (value()) {
                    success = true;
            } else {
                error("pair() expected a value after ':'");
            }
        } else {
            error("pair() expected a ':' after object name");
        }
    }
    leave("pair");

    return success;
}

/*
 * array           =   "[", [ value, { ",", value } ], "]";
 */
bool array() {
    bool success = false;
    enter("array");
    if (expect('[')) {
        if (value()) {
            while (expect(',')) {
                if (!value()) {
                    error("array() expected another value after ','");
                }
            }
        }
        if (expect(']')) {
            success = true;
        } else {
            error("array() expected a ']' to end array");
        }
    }
    leave("array");

    return success;
}

/*
 * value           =   (string | number | object | array | true | false | null);
 */
bool value() {
    bool success = false;
    enter("value");
    if (
            string()    ||
            number()    ||
            object()    ||
            array()     ||
            _true()     ||
            _false()    ||
            _null()
       ) {
        success = true;
    }
    leave("value");

    return success;
}

/*
 * string          =   '"', { ( nodquoteslash | escaped ) }, '"';
 */
bool string() {
    bool success = false;
    enter("string");
    if (expect('"')) {
        while (nodquoteslash() || escaped());
        if (expect('\"')) {
            success = true;
        } else {
            error("string() expected a '\"' to end a string");
        }
    }
    leave("string");

    return success;
}

/*
 * number          =   [ "-" ], ( "0" | nonzero, { digit } ), [ ".", digit,
 *                         { digit } ], [ ( "e" | "E" ), [ ( "+" | "-" ) ], digit,
 *                         { digit } ];
 */
bool number() {
    bool success = false;
    enter("number");
    expect('-');
    if (expect('0')) {
        success = true;
    } else if (nonzero()) {
        while (digit());
        success = true;
    } else {
        success = false;
    }
    if (success) {
        if (expect('.')) {
            if (digit()) {
                while(digit());
            } else {
                error("number() expected an integer after '.'");
            }
        }
        if (expect('e') || expect('E')) {
            if (expect('+') || expect('-'));
            if (digit()) {
                while (digit());
            } else {
                error("number() expected a number after exponent character");
            }
        }
    }

    leave("number");

    return success;
}

/*
 * nodquoteslash   =   [^\"];
 */
bool nodquoteslash() {
    bool success = false;
    enter("nodquoteslash");
    if (noexpect(L"\\\"")) {
        success = true;
    }
    leave("nodquoteslash");

    return success;
}

/*
 * escaped         =   "\", ( '"' | "\" | "/" | "b" | "f" | "n" | "r" | "t" | "u", hex * 4);
 */
bool escaped() {
    bool success = false;
    enter("escaped");
    if (expect('\\')) {
        if (
                expect('"')     ||
                expect('\\')    ||
                expect('/')     ||
                expect('b')     ||
                expect('f')     ||
                expect('n')     ||
                expect('r')     ||
                expect('t')
           ) {
            success = true;
        } else if (expect('u')) {
            int count = 4;
            while (iswxdigit(next) && --count) {
                scan();
            }

            if (count) {
                error("escaped() expected exactly 4 hex digits after \"\\u\"");
            } else {
                success = true;
            }
        }
    }
    leave("escaped");

    return success;
}

/*
 * nonzero         =   ( "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" );
 */
bool nonzero() {
    bool success = false;
    enter("nonzero");
    if (iswdigit(next) && next != '0') {
        scan();
        success = true;
    }
    leave("nonzero");

    return success;
}

/*
 * digit           =   ( nonzero | "0" );
 */
bool digit() {
    bool success = false;
    enter("digit");
    if (iswdigit(next)) {
        success = true;
        scan();
    }
    leave("digit");

    return success;
}

/*
 * true            =   "t", "r", "u", "e";
 */
bool _true() {
    int success = false;
    enter("true");
    if (expect('t')) {
        if (expect('r') && expect('u') && expect('e')) {
            success = true;
        } else {
            error("true() unexpected token");
        }
    }
    leave("true");

    return success;
}

/*
 * false           =   "f", "a", "l", "s", "e";
 */
bool _false() {
    int success = false;
    enter("false");
    if (expect('f')) {
        if (expect('a') && expect('l') && expect('s') && expect('e')) {
            success = true;
        } else {
            error("false() unexpected token");
        }
    }
    leave("false");

    return success;
}

/*
 * null            =   "n", "u", "l", "l";
 */
bool _null() {
    int success = false;
    enter("null");
    if (expect('n')) {
        if (expect('u') && expect('l') && expect('l')) {
            success = true;
        } else {
            error("null() unexpected token");
        }
    }
    leave("null");

    return success;
}
