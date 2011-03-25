/*
 * Grammar:
 * json            =   ( object | array ) , EOF;
 * object          =   "{", [ pair { ",", pair } ], "}";
 * pair            =   string, ":", value;
 * array           =   "[", [ value, { ",", value } ], "]";
 * value           =   (string | number | object | array | true | false | null);
 * string          =   '"', { ( nodquoteslash, escaped ) }, '"';
 * number          =   [ "-" ], ( "0" | nonzero, { digit } ), [ ".", digit,
 *                         { digit } ], [ ( "e" | "E" ), [ ( "+" | "-" ) ], digit,
 *                         { digit } ];
 * nodquoteslash   =   [^\"];
 * escaped         =   "\", ( '"' | "\" | "/" | "b" | "f" | "n" | "r" | "t" | "u", hex * 4);
 * nonzero         =   ( "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" );
 * digit           =   ( nonzero | "0" );
 * true            =   "t", "r", "u", "e";
 * false           =   "f", "a", "l", "s", "e";
 * null            =   "n", "u", "l", "l";
 */

#ifndef __JSON_H
#define __JSON_H

#include "parser.h"

bool _false();
bool _null();
bool _true();
bool array();
bool digit();
bool escaped();
bool json();
bool nodquoteslash();
bool nonzero();
bool number();
bool object();
bool pair();
bool string();
bool value();

#endif /* __JSON_H */
