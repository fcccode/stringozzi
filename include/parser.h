/*
MIT License

Copyright (c) 2018 Osama Salem

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, 
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT 
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <vector>
#include <stack>
#include <cstring>

//#define USE_WCHAR
#if defined USE_WCHAR
typedef wchar_t Char;
#define _C(__x) L##__x
#define STRCAT wcsncat
#define STRTOF wcstof
#define STRTOL wcstol
#define STRCHR wcschr
#define STDSTRING std::wstring
#else
typedef char Char;
#define _C(__x) __x
#define STRCAT strncat
#define STRTOF strtof
#define STRTOL strtol
#define STRCHR strchr
#define STDSTRING std::string
#endif

#if defined _MSC_VER
#define END_OF_LINE	_C("\r\n")
#pragma warning( disable : 4251 ) 
#ifdef DLLEXPORT
#define LIB_REFERENCE __declspec(dllexport)
#else
#define LIB_REFERENCE __declspec(dllimport)
#endif
#elif defined __GNUC__
#define END_OF_LINE	_C("\n")
#define LIB_REFERENCE
#endif


/*!
 * @namespace 	Parsing
 * @brief 		The topmost root namespace that contains the main major namespaces
 */
namespace Stringozzi
{
	/*!
	 * @namespace 	Rules
	 * @brief 		the namespace that contains the patterns parsing/recognition elements
	 */
	namespace Rules
	{
		/*!
		 * @interface TokenizerInterface
		 * @brief The base class for all parsing elemnts.  You can create any parsing elemnts
		 * 		  just be implementing this interface
		 */
		class TokenizerInterface
		{
		public:
			/*!
			* @fn 	bool Check(const Char** )
			* Check if the string pointed by cursor is complying to this parsing rule
			* @param[in/out] cursor  A cursor to the parsing string, after successful parsing,
			* 						 the cursor value should move to the last character as far as
			* 						 the proper parsing ends
			* @return 				 Rerturn @a true if the parsing succeeds.
			* 						 Otherwise it returns false
			*
			*/
			virtual bool Check(const Char** cursor) const = 0;

		};

		/*!
		 * @class RulesSet
		 * @brief a generic container for all types of parsing elements ..
		 * 		  it is recommended to use it as named rule template which can be
		 * 		  used several times
		 * @code{.cpp}
		 * 		RuleSet rs = Sequence(Is('A'),Is('B'));
		 * @endcode
		 */

		//class LIB_REFERENCE RulesSet : public TokenizerInterface
		//{
		//	const TokenizerInterface& tok;
		//public:
		//	/*!
		//	* @fn 	RulesSet(TokenizerInterface& tok)
		//	* Constructor to RuleSet
		//	* @param[in] tok  contained parsing rule element
		//	*/
		//	 RulesSet(const TokenizerInterface& tok) : tok(tok) {}
		//	virtual bool Check(const Char** ) const ;


		//};

		/*!
		 * @namespace 	Manipulators
		 * @brief 		the namespace that contains controlling lexical and relational
		 * 				patterns parsing/recognition elements
		 * @see 		Filters, Filters::Utils
		 */
		namespace Manipulators
		{
			/*!
			* @class Not
			* @brief it is a negating effect parsing element. it turns the parsing process
			* 		 result of the contained element to the opposite ...
			* @note  You can use (!) operator for the same functionality
			* @warning 	Do not use nested @a Not instancess as it will not adjust the
			* 			cursor position properly
			*
			* @code{.cpp}
			* 		Not(Is('A')); 	// A,C,D,E ..etc => TRUE
			* 		!Is('A'); 		// the same effect
			* @endcode
			* @see And, Or, Not
			*/
			class LIB_REFERENCE  Not : public TokenizerInterface
			{
			private:
				const TokenizerInterface& _Tokenizer;
			/*!
			* @fn 	Not(TokenizerInterface& tok)
			* Constructor to Not class
			* @param[in] tok  contained parsing rule element
			*/
			public:
				/*!
				* @fn 	Not(TokenizerInterface& tok)
				* Constructor to Not
				* @param[in] tok  contained parsing rule element
				*/
				Not(const TokenizerInterface& tok) : _Tokenizer(tok) {}
				virtual bool Check(const Char** cursor)const ;

			};

			/*!
			* @class Or
			* @brief it is accepts two parsing elements and performs @a OR boolean
			* 			operation between them
			* 	is accepts two parsing elements and performs @a OR boolean
			* 	operation between them.the conditions has short circut effect
			* 	it will return for the first applicable condition and ignores
			* 	the rest of them, So be carful to use the most general form
			* 	later as the last condition after the most specified one
			* 	consider the example
			* @code{.cpp}
			* 		Or(Exact("A"),Exact("ABC")); // Wrong: it always returns in "A"
			* 		Or(Exact("ABC"),Exact("A")); // Correct
			* @endcode
			*
			*
			* @note  You can use (|) operator for the same functionality
			*
			* @b Example:
			* @code{.cpp}
			* 		Or(Is('A'),Is('B')); // "A" and "B" are Ok , "C" fails
			* 		Is('A') |  Is('B'); // the same functionality
			* @endcode
			* @see And, Or, Not
			*/
			class LIB_REFERENCE  Or : public TokenizerInterface
			{
			private:
				const TokenizerInterface& _TokenizerA;
				const TokenizerInterface& _TokenizerB;

			public:
				/*!
				* @fn 	Or(TokenizerInterface& tok1, TokenizerInterface& tok2)
				* Constructor to Or class
				* @param[in] tok1  the first contained parsing rule element
				* @param[in] tok2  the second contained parsing rule element
				*/
				Or(const TokenizerInterface& tok1,const  TokenizerInterface& tok2)
					:_TokenizerA (tok1)
					,_TokenizerB (tok2)	{}
				virtual bool Check(const Char** )const ;

			};

			/*!
			* @class And
			* @brief it is accepts two parsing elements and performs @a AND boolean
			* 			operation between them,
			* 	it is accepts two parsing elements and performs @a AND boolean
			* 	operation between them, the conditions has short circut effect
			* 	it will return for upon the first unapplicable condition and ignores
			* 	the rest .. this pattern is not intended to be widely used.. and it has
			* 	a perticular case as excluding pattern ,ex:
			* @code{.cpp}
			* 		And(Alphanumeric(),Not(Number()));		// "A" ,"B" are Ok , "7" fails
			* 		And(Within(1,2,Digit()),Integer(55)) 	// "11" and "32" Ok , "57" and "122" Fails
			* @endcode
			*
			* @note  You can use (&) operator as a short hand for the same functionality
			*
			* @b Example:
			* @code{.cpp}
			* 		And(Alphanumeric(),Number()); // "1" and "2" are Ok , "A" fails
			* 		Alphanumeric() & Number(); // the same functionality
			* @endcode
			* @see And, Or, Not
			*/
			class LIB_REFERENCE And : public TokenizerInterface
			{
			private:
				const TokenizerInterface& _TokenizerA;
				const TokenizerInterface& _TokenizerB;

			public:
				/*!
				* @fn 	And(TokenizerInterface& tok1, TokenizerInterface& tok2)
				* Constructor to And class
				* @param[in] tok1  the first contained parsing rule element
				* @param[in] tok2  the second contained parsing rule element
				*/
				And(const TokenizerInterface& tok1, const TokenizerInterface& tok2)
					:_TokenizerA(tok1)
					, _TokenizerB(tok2) {}

				virtual bool Check(const Char** )const ;

			};
			/*!
			* @class Sequence
			* @brief it accepts two parsing elements and execute them sequentialy in order
			* 			(ex. pattern1 then pattern2), if the first one fails the whole sequence fails
			* 			immediately .. this is common pattern to use
			*
			* @note  You can use (+) addition operator as a short hand for the same functionality
			*
			* @b Example:
			* @code{.cpp}
			* 		Sequence(Is('A'),Is('B')); // "AB" is OK , "A" or "B" fails
			* 		Is('A') + Is('B'); // the same functionality
			* @endcode
			* @see Exact, Within
			*/

			class LIB_REFERENCE  Sequence : public TokenizerInterface
			{
			private:
				const TokenizerInterface& _TokenizerA;
				const TokenizerInterface& _TokenizerB;


			public:
				/*!
				* @fn 	Sequence(TokenizerInterface& tok1, TokenizerInterface& tok2)
				* Constructor to Sequence class
				* @param[in] tok1  the first contained parsing rule element
				* @param[in] tok2  the next contained parsing rule element
				*/
				Sequence(const TokenizerInterface& tok1, const TokenizerInterface& tok2) :_TokenizerA(tok1), _TokenizerB(tok2) {}
				virtual bool Check(const Char** )const ;

			};

			/*!
			* @class Within
			* @brief it checks if the contained pattern is repeated number of times between
			* 		minimum and maximum limits , If maximum value equals -1 so the wont be
			* 		any upper limit (theoritically infinite repititions)
			*
			* @note  In repitition funtions, there is not cheking on end of text
			* 		 make sure that the rules is properly arranged to not overflow the
			* 		 string limits
			*
			* @b Example:
			* @code{.cpp}
			* 		Within(2,4,Is('A')); // "AA","AAA","AAAA" are OK , "","A" or "AAAAA" fail
			* @endcode
			* @see 	Times, While, Until, OneOrMore, ZeroOrOne, Filters::Exact
			* 		, Sequence
			*/
			class LIB_REFERENCE Within : public TokenizerInterface
			{
			private:
				unsigned long _Min;
				unsigned long _Max;
				const TokenizerInterface& _Tokenizer;
			public:
				/*!
				* @fn 	Within(unsigned long min, unsigned long max, TokenizerInterface& tok)
				* Constructor to Sequence class
				* @param[in] min  Minimum number of repitition
				* @param[in] max  Maximum number of repitition
				* @param[in] tok  the next contained parsing rule element
				*/
				Within(unsigned long min, unsigned long max, const TokenizerInterface& tok) : _Min(min), _Max(max), _Tokenizer(tok) {}
				/*!
				* @fn 	Within( unsigned long max, TokenizerInterface& tok)
				* Constructor to Sequence class
				* @param[in] max  Maximum number of repitition
				* @param[in] tok  the next contained parsing rule element
				*/
				Within(unsigned long max,const  TokenizerInterface& tok) : _Min(0), _Max(max), _Tokenizer(tok) {}
				virtual bool Check(const Char** )const ;

			};

			/*!
			* @class Times
			* @brief it checks if the contained pattern is repeated in exactly specified number of times
			* 		If the number equals -1 so there will not be
			* 		any upper limit (theoritically infinite repititions).. It is identical
			* 		to using "Within(number,number,...)"
			*
			* @note  In repitition funtions, there is not cheking on end of text
			* 		 make sure that the rules is properly arranged to not overflow the
			* 		 string limits
			*
			* @b Example:
			* @code{.cpp}
			* 		Within(2,4,Is('A')); // "AA","AAA","AAAA" are OK , "","A" or "AAAAA" fail
			* @endcode
			* @see 	 While, Until, OneOrMore, ZeroOrOne, Filters::Exact
			* 		, Sequence
			*/
			class LIB_REFERENCE Times : public TokenizerInterface
			{
			private:
				int  _Max;
				const TokenizerInterface& _Tokenizer;
			public:
				/*!
				* @fn 	Times( int max, TokenizerInterface& tok)
				* Constructor to Sequence class
				* @param[in] _Max  number of repitition
				* @param[in] tok  the next contained parsing rule element
				*/
				Times(int max, const TokenizerInterface& tok) : _Max(max), _Tokenizer(tok) {}
				virtual bool Check(const Char** )const ;

			};

			/*!
			* @class OneOrMore
			* @brief	it checks if the contained pattern is repeated at least once ...
			*			It is identical to using "Within(1,-1,...)"
			*
			* @note  In repitition funtions, there is not cheking on end of text
			* 		 make sure that the rules is properly arranged to not overflow the
			* 		 string limits
			*
			* @b Example:
			* @code{.cpp}
			* 		OneOrMore(Is('A')); // "A","AA","AAA"  are OK , "" or "B" fail
			* @endcode
			* @see 	Times, While, Until, ZeroOrOne, Filters::Exact
			* 		, Sequence
			*/
			class LIB_REFERENCE OneOrMore : public TokenizerInterface
			{
			private:
				const TokenizerInterface& _Tokenizer;

			public:
				/*!
				* @fn 	OneOrMore(TokenizerInterface& tok) 
				* Constructor to OneOrMore class
				* @param[in] tok  the contained parsing rule element
				*/
				 OneOrMore(const TokenizerInterface& tok) : _Tokenizer(tok) {}
				virtual bool Check(const Char** )const ;

			};

			/*!
			* @class ZeroOrOne
			* @brief	it checks if the contained pattern is exist or not .. both cases are acceptable.. it 
			*			is like checking for optional item ..but if it is existing it must compley the rule specified 
			*			, It is identical to using "Within(0,1,...)"
			*
			* @b Example:
			* @code{.cpp}
			* 		Is('A') + ZeroOrOne(Is('B')) + Is('C') ; // "AC","ABC" are OK , "ADC" fails
			* @endcode
			* @see 	Times, While, Until, ZeroOrOne, Filters::Exact
			* 		, Sequence
			*/
			class LIB_REFERENCE ZeroOrOne : public TokenizerInterface
			{
			private:
				const TokenizerInterface& _Tokenizer;

			public:
				/*!
				* @fn 	ZeroOrOne(TokenizerInterface& tok)
				* Constructor to ZeroOrOne class
				* @param[in] tok  the contained parsing rule element
				*/
				 ZeroOrOne(const TokenizerInterface& tok) : _Tokenizer(tok) {}
				virtual bool Check(const Char** )const ;

			};

			/*!
			* @class Optional
			* @brief	see ZeroOrOne
			*/
			typedef ZeroOrOne Optional;
			/*!
			* @class _0Or1
			* @brief	see ZeroOrOne
			*/
			typedef ZeroOrOne _0Or1;

			/*!
			* @class While
			* @brief	it checks if the contained pattern is exist or not .. both cases are acceptable.. it
			*			is like checking for optional item multiple times in repititive pattern in contrast to ZeroOrOne
			*			 ..but if it is existing it must compley the rule specified
			*			, It is identical to using "Within(0,-1,...)"
			*
			* @b Example:
			* @code{.cpp}
			* 		Is('A') + While(Is('B')) + Is('C') ; // "AC","ABC"and "ABBC" are OK , "ADC" fails
			* @endcode
			* @see 	Times, While, Until, ZeroOrOne, Filters::Exact
			* 		, Sequence
			*/
			class LIB_REFERENCE While : public TokenizerInterface
			{
			private:
				/*!
				* @fn 	While(TokenizerInterface& tok)
				* Constructor to While class
				* @param[in] tok  the contained parsing rule element
				*/
				const TokenizerInterface& _Tokenizer;
			public:
				 While(const TokenizerInterface& tok) : _Tokenizer(tok) {}
				virtual bool Check(const Char** )const ;

			};
	
			/*!
			* @class Until
			* @brief	it is the main searching utility ..it skips the string until
			* 			the contained pattern found , It is identical
			* 			to using "While(Not(...))"  or "Within(0,-1,...)"
			*
			* @b Example:
			* @code{.cpp}
			* 		Until(Is('C')) + Is('C') ; // "AC","AAC"and "ABBC" are OK , "" "D" fails
			* @endcode
			* @see 	Times, While, Until, ZeroOrOne, Filters::Exact
			* 		, Sequence
			*/
			class LIB_REFERENCE Until : public TokenizerInterface
			{
			private:
				const TokenizerInterface& _Tokenizer;
			public:
				/*!
				* @fn 	Until(TokenizerInterface& tok)
				* Constructor to Ultil class
				* @param[in] tok  the contained parsing rule element
				*/
				Until(const TokenizerInterface& tok) : _Tokenizer(tok) {}
				virtual bool Check(const Char** )const ;

			};

			/*!
			* @class Extract
			* @brief	It is the extracting utility .. it returns the matched pattern to
			* 			the supplied buffer ...
			*
			* @b Example:
			* @code{.cpp}
			* 		char buffer[1024];
			* 		Extract(buffer,Is('C')) ; // if "C" .. buffer = "C"
			* @endcode
			*/
			class LIB_REFERENCE Extract : public TokenizerInterface
			{
			private:
				const TokenizerInterface& _Tokenizer;
				Char*			_Input;

			public:
				/*!
				* @fn 	Extract(TokenizerInterface& tok)
				* Constructor to Extract class
				* @param[out] str  the returned string
				* @param[in] tok  the contained parsing rule element
				*/
				Extract(Char* str, const TokenizerInterface& tok) : _Input(str), _Tokenizer(tok) {}
				virtual bool Check(const Char** )const ;

			};

			/*!
			* @class ReturnToCallback
			* @brief	It is similar to Exctract.. but it has more delicate usage than Extract
			* 			like extract to list or arrays for repititive tokens
			* 			supplied callback instead of using buffers
			*
			*
			* @b Example:
			* @code{.cpp}
			* 		void callback(char* _Stream)
			* 		{
			* 			// Process the token
			* 		}
			* 		ReturnToCallback(callback,Is('C')) ; //
			* @endcode
			*/
			class LIB_REFERENCE ReturnToCallback  : public TokenizerInterface
			{
			public: typedef void(*CallBack)(const Char*);
			private:
				const TokenizerInterface& _Tokenizer;
				CallBack cb;

			public:
				/*!
				* @fn 	ReturnToCallback(CallBack cb, const TokenizerInterface& _Tokenizer)
				* Constructor to Extract class
				* @param[in] cb  the called callback for returning
				* @param[in] _Tokenizer  the contained parsing rule element
				*/
				ReturnToCallback(CallBack cb, const TokenizerInterface& tok) : cb(cb), _Tokenizer(tok) {}
				virtual bool Check(const Char** )const ;

			};


			typedef  While ZeroOrMore;

			/*!
			* @class Enclosed
			* @brief	It is pattern which matches the specified parsing element between
			* 			defined type of brackets
			*
			* @b Example:
			* @code{.cpp}
			* 		Enclosed("<",">",Is('C')) ; // "<C>" OK ... "C" "<C" "C>" "[C]" fails
			* @endcode
			*/
			class LIB_REFERENCE Enclosed : public TokenizerInterface
			{
				const TokenizerInterface& _Tokenizer;
				Char* open;
				Char* close;

			public:
				/*!
				* @fn 	Enclosed(Char* open, Char* close, const TokenizerInterface& tok)
				* Constructor to Extract class
				* @param[in] open the opening bracket
				* @param[in] close  the closing bracket
				* @param[in] tok  the contained parsing rule element
				*/
				Enclosed(Char* open, Char* close, const TokenizerInterface& tok) :_Tokenizer(tok), open(open), close(close) {}
				virtual bool Check(const Char** )const ;

			};



		}

		namespace Filters
		{
			/*!
			* @class Any
			* @brief	matching element that matches any character (even null terminator)
			* 			it is used in skipping characters and go forward ... it simply
			* 			increment the pointer and return true always
			*
			* @b Example:
			* @code{.cpp}
			* 		Any(); // "C" " " "#" OK ... never fails
			* @endcode
			*/
			class LIB_REFERENCE Any : public TokenizerInterface
			{
			public:
				Any() {}
				virtual bool Check(const Char** )const ;

			};

			extern LIB_REFERENCE Any ANY;

			/*!
			* @class In
			* @brief	Checks if the character unnder processing is existing in
			* 			a defined set ... it is identical to use
			* 			Is('...')|Is('...')|Is('...')
			*
			* @b Example:
			* @code{.cpp}
			* 		In("ABC"); // "A" , "B" or "C" OK ... "D" fails
			* @endcode
			*/
			class LIB_REFERENCE In : public TokenizerInterface
			{
			private:
				const Char* _Input;
			public:
				 In(const Char* str) : _Input(str) {}
				virtual bool Check(const Char**)const ;
			};

			/*!
			* @class Between
			* @brief	Checks if the character unnder processing is existing between
			* 			a defined range.. it can be represented by relation
			* 			min <= a <= max (<= : lesser than or equal)..minimum must be lesser
			* 			than maximum otherwise it will always return false...
			* 			if min and max are equal it is better to user Is()
			*
			* @b Example:
			* @code{.cpp}
			* 		Between('A','C'); // "A" , "B" or "C" OK ... "D" fails
			* @endcode
			*/
			class LIB_REFERENCE Between : public TokenizerInterface
			{
				Char _Lower, _Upper;
			public:
				Between(Char a, Char b) : _Lower(a), _Upper(b) {}
				virtual bool Check(const Char** )const ;

			};
			//TODO:class SmallLetter;
			//TODO:class CapitalLetter;
			//TODO:Class Letter;
			//TODO:Class Punctuation
			/*!
			* @class Alphabet
			* @brief	Checks if the character unnder processing is an alphabet letter
			* 			(small or capital) .. it can be represented by relation
			* 			A <= a <= Z (<= : lesser than or equal)..
			*
			* @b Example:
			* @code{.cpp}
			* 		Alphabet() ; // "A" , "B" or "C" are OK ... "9" and "#" fail
			* @endcode
			*/
			class LIB_REFERENCE Alphabet : public TokenizerInterface
			{

			public:
				Alphabet() {}
				virtual bool Check(const Char** )const ;

			};
			
			extern LIB_REFERENCE Alphabet ALPHABET;


			/*!
			* @class Digit
			* @brief	Checks if the character unnder processing is an numeric digit
			* 			 .. it can be represented by relation
			* 			0 <= a <= 9 (<= : lesser than or equal)..
			*
			* @b Example:
			* @code{.cpp}
			* 		Digit() ; // "1" , "2" or "7" are OK ... "A" and "#" fail
			* @endcode
			*/
			class LIB_REFERENCE Digit : public TokenizerInterface
			{

			public:
				Digit() {}
				virtual bool Check(const Char** )const ;

			};
			
			extern LIB_REFERENCE Digit DIGIT;
			//Class Octal
			/*!
			* @class Hex
			* @brief	Checks if the character unnder processing is an hex digit
			* 			from 0 to f .. it can be represented by relation
			*
			* @b Example:
			* @code{.cpp}
			* 		Hex() ; // "1" , "A" or "7" are OK ... "G" and "#" fail
			* @endcode
			*/
			class LIB_REFERENCE Hex : public TokenizerInterface
			{

			public:
				Hex() {}
				virtual bool Check(const Char** )const ;

			};
			
			extern LIB_REFERENCE Hex HEX;
			/*!
			* @class AlphaNumeric
			* @brief	Checks if the character unnder processing is an alphabet letter
			* 			or numeric digit  .. it is identical to using
			* 			Alphabet() | Digit()
			*
			* @b Example:
			* @code{.cpp}
			* 		AlphaNumeric() ; // "A" , "2" or "0" are OK ... " " and "#" fail
			* @endcode
			*/
			class LIB_REFERENCE AlphaNumeric : public TokenizerInterface
			{
			public:
				AlphaNumeric() {}
				virtual bool Check(const Char** )const ;

			};
			
			extern LIB_REFERENCE AlphaNumeric ALPHANUMERIC;
			

			/*!
			* @class Exact
			* @brief	Checks if the character unnder processing is following
			* 			the specified charcters sequence in case sensitive profile
			* 			.. it is identical to using
			* 			Is('...') + Is('...') + Is('...') + ...
			*
			* @b Example:
			* @code{.cpp}
			* 		Exact("ABC") ; // "ABC" is OK ... "abc" or anything else fails
			* @endcode
			*/
			class LIB_REFERENCE Exact : public TokenizerInterface
			{
				const Char *  _Input;
			public:
				Exact(const Char * str) : _Input(str) {}
				virtual bool Check(const Char** )const ;

			};

			/*!
			* @class Like
			* @brief	Checks if the character unnder processing is following
			* 			the specified charcters sequence in case insensitive profile
			* 			..
			*
			* @b Example:
			* @code{.cpp}
			* 		Like("ABC") ; // "ABC" and "abc" are OK ... otherwise fails
			* @endcode
			*/
			class LIB_REFERENCE Like : public TokenizerInterface
			{
				Char* _Input;
			public:
				 Like(Char* str) : _Input(str) {}
				virtual bool Check(const Char** )const ;

			};
			/*!
			* @class WhiteSpace
			* @brief	Checks if the character unnder processing is a whitespace,tab
			* 			or newline.. it is identical to using
			* 			In(" \t\n\r");
			*
			* @b Example:
			* @code{.cpp}
			* 		WhiteSpace () ; // " " , "\n" or "\t" are OK ... "A" and "#" fail
			* @endcode
			*/
			class LIB_REFERENCE WhiteSpace : public TokenizerInterface
			{
			public:
				WhiteSpace() {}
				virtual bool Check(const Char** )const ;

			};
			extern LIB_REFERENCE WhiteSpace WHITESPACE;
			
			/*!
			* @class Is
			* @brief	Checks if the character under processing is equal to the specified
			* 			character.. it is the most basic for of matching pattern
			*
			* @b Example:
			* @code{.cpp}
			* 		Is('A') ; // "A" are OK ... "a" or anything else fails
			* @endcode
			*/
			class LIB_REFERENCE Is : public TokenizerInterface
			{

				Char _Letter;
			public:
				 Is(const Char c) :_Letter(c) {}
				// Is(const Char* s) :letter(s[0]) {}
				virtual bool Check(const Char** )const ;

			};

			/*!
			* @class Decimal
			* @brief	Checks if the token unnder processing is following
			* 			the numeric pattern forming like a positive integer number..
			* 			it is identical to using
			* 			OneOrMore(Digit())
			* 			..
			*
			* @b Example:
			* @code{.cpp}
			* 		Decimal() ; // "1" and "12" are OK ... "A" "-1" fails
			* @endcode
			*/
			class LIB_REFERENCE Decimal : public TokenizerInterface
			{

			public:
				Decimal() {}
				virtual bool Check(const Char** )const ;

			};
			extern LIB_REFERENCE Decimal 	DECIMAL;

			/*!
			* @class Word
			* @brief	Checks if the token unnder processing is following
			* 			the aphabetic sequence.. it is identical to using
			* 			OneOrMore(Alphabet())
			* 			..
			*
			* @b Example:
			* @code{.cpp}
			* 		Word() ; // "1" and "12" are OK ... "A" "-1" fails
			* @endcode
			*/
			class LIB_REFERENCE Word : public TokenizerInterface
			{

			public:
				Word() {}
				virtual bool Check(const Char** )const ;

			};
			extern LIB_REFERENCE  Word	WORD;

			/*!
			* @class EndOfText
			* @brief	Checks if the character under processing terminates the text
			* 			in contrast to other patterns ...this matching pattern does not increment
			* 			string pointer.. it is not identical to using Is('\0')
			*
			* @b Example:
			* @code{.cpp}
			* 		EndOfText() ; // "" are OK ... otherwise fails
			* 		EndOfText() + EndOfText(); // "" are OK ... does not pass the end
			* @endcode
			*/
			class LIB_REFERENCE EndOfText : public TokenizerInterface
			{
			public:
				EndOfText() {}
				virtual bool Check(const Char** )const ;

			};
			extern LIB_REFERENCE EndOfText EOT;


			/*!
			* @class EndOfLine
			* @brief	Checks if the character under processing terminates the line
			* 			...it is identical to using Exact("\\r\\n")for MS Windows
			* 			and Is('\\n') Linux
			*
			*
			*/
			class LIB_REFERENCE EndOfLine : public TokenizerInterface
			{
			public:
				EndOfLine() {}
				virtual bool Check(const Char** )const ;

			};
			extern LIB_REFERENCE EndOfLine EOL;

			/*!
			* @class Integer
			* @brief	this is more intellegent matching pattern ...
			* 			Checks if the token under processing is integer number
			* 			within the specified number range
			* 			...it is not like using Decimal()
			*
			* @b Example:
			* @code{.cpp}
			* 		Integer(-1,15) ; // "0" "-1" "12" are OK ... "-2" or "16" fail
			* @endcode
			*/

			class LIB_REFERENCE Integer : public TokenizerInterface
			{
				int _Min, _Max;
			public:
				Integer(int min, int max) :_Min(min), _Max(max) {}
				 Integer(int max) :_Min(0), _Max(max) {}
				Integer() :_Min(0), _Max(0x7fffffff) {}
				virtual bool Check(const Char** )const ;

			};

			/*!
			* @class Float
			* @brief	this is more intellegent matching pattern ...
			* 			Checks if the token under processing is a floating point number
			* 			within the specified number range
			* 			...it is not like using Decimal()
			*
			* @b Example:
			* @code{.cpp}
			* 		Integer(-1.2f,7.24f) ; // "0" "-1.0" "5.447567" are OK ... "-2.0" or "16.3" fail
			* @endcode
			*/
			class LIB_REFERENCE Float : public TokenizerInterface
			{
				float _Min, _Max;
			public:
				Float(float min, float max) :_Min(min), _Max(max) {}
				 Float(float max) :_Min(0.0f), _Max(max) {}
				Float() : _Min(0.0f), _Max(3.402823e+38f) {}
				virtual bool Check(const Char** )const ;

			};
			//namespace Date

		}
		/*!
		 * @namespace 	Utils
		 * @brief 		more comprehensive matching pattern elements that are used
		 * 				usually in several development parsing/searching tasks
		 */
		namespace Utils
		{
			/*!
			* @struct Range
			* @brief	Range 
			*
			*/
			typedef struct Range
			{
				int Minimum;
				int Maximum;
				Range(int min, int max) :Minimum(min), Maximum(max) {}
			} R;

			/*!
			* @class IPv4
			* @brief	check if the token under processing is in IP address V4 format
			* 			(aaa.bbb.ccc.ddd)
			*
			*/
			class LIB_REFERENCE IPv4 : public TokenizerInterface
			{
			public:
				virtual bool Check(const Char** )const ;

			};
			extern LIB_REFERENCE IPv4 IPV4;

			/*!
			* @class Host
			* @brief	check if the token under processing is a host descriptive
			* 			(server.domain.com) (127.0.0.1)
			*
			*/
			class LIB_REFERENCE Host : public TokenizerInterface
			{
			public:
				virtual bool Check(const Char** )const ;

			};
			
			extern LIB_REFERENCE Host  HOST;

			/*!
			* @class Email
			* @brief	check if the token under processing is a email address
			* 			(abc@host.com)
			*
			*/
			class LIB_REFERENCE Email : public TokenizerInterface
			{
			public:	virtual bool Check(const Char** )const ;
			};
			extern LIB_REFERENCE Email EMAIL;

			/*!
			* @class Phone
			* @brief	check if the token under processing is a email address
			* 			(+(123)555 443 2 22)
			*
			*/
			class LIB_REFERENCE Phone : public TokenizerInterface
			{
			public:	virtual bool Check(const Char** )const ;
			};
			extern LIB_REFERENCE Phone PHONE;

			/*!
			* @class ServerAddress
			* @brief	check if the token under processing is server plus port
			* 			(host@domain.com:12345)
			*
			*/
			class LIB_REFERENCE ServerAddress : public TokenizerInterface
			{
			public:	virtual bool Check(const Char** )const ;
			};
			extern LIB_REFERENCE  ServerAddress SERVERADDRESS;

			/*!
			* @class URI
			* @brief	check if the token under processing is URI address
			* 			(proto:abc@host.domain.com:4444)
			*
			*/
			class LIB_REFERENCE Uri : public TokenizerInterface
			{
			public:	virtual bool Check(const Char** )const ;
			};
			extern LIB_REFERENCE Uri URI;

		}

	}

	class LIB_REFERENCE StringProcessor
	{
	private:
		const Char* _String;
		const Char* _Cursor;
		std::string _LastTokenized;
		std::stack<const Char*> _SavedPositions;

	public:
		StringProcessor(const Char* str); 
		bool Parse(const Rules::TokenizerInterface& tok);
		bool Validate(const Rules::TokenizerInterface& tok);
		const Char* Search(const Rules::TokenizerInterface& tok);
		int GetLastParserPosition();
		void Push();
		void Pop();
		bool IsEOT();
		void Reset();

	};

}


inline Stringozzi::Rules::Manipulators::Times operator *(const Stringozzi::Rules::TokenizerInterface& tok, int times)
{
	return Stringozzi::Rules::Manipulators::Times(times, tok);
}

inline Stringozzi::Rules::Manipulators::Times operator *(int times, const Stringozzi::Rules::TokenizerInterface& tok)
{
	return Stringozzi::Rules::Manipulators::Times(times, tok);
}

inline Stringozzi::Rules::Manipulators::Within operator *(const Stringozzi::Rules::TokenizerInterface& tok, const Stringozzi::Rules::Utils::Range& range)
{
	return Stringozzi::Rules::Manipulators::Within(range.Minimum, range.Maximum, tok);
}

inline Stringozzi::Rules::Manipulators::Within operator *(const Stringozzi::Rules::Utils::Range& range, const Stringozzi::Rules::TokenizerInterface& tok)
{
	return Stringozzi::Rules::Manipulators::Within(range.Minimum, range.Maximum, tok);
}

/* inline Stringozzi::Rules::Manipulators::Within operator *(const Stringozzi::Rules::TokenizerInterface& tok, std::initializer_list<int> times)
{
	return Stringozzi::Rules::Manipulators::Within(*times.begin() , *times.end(), tok);
}
 */
 
inline Stringozzi::Rules::Manipulators::Or operator | (const Stringozzi::Rules::TokenizerInterface& t1, const Stringozzi::Rules::TokenizerInterface& t2)
{
	return Stringozzi::Rules::Manipulators::Or(t1, t2);
}

inline Stringozzi::Rules::Manipulators::And operator & (const Stringozzi::Rules::TokenizerInterface& t1, const Stringozzi::Rules::TokenizerInterface& t2)
{
	return Stringozzi::Rules::Manipulators::And(t1, t2);
}

inline Stringozzi::Rules::Manipulators::Sequence operator + (const Stringozzi::Rules::TokenizerInterface& t1, const Stringozzi::Rules::TokenizerInterface& t2)
{
	return Stringozzi::Rules::Manipulators::Sequence(t1, t2);
}

inline Stringozzi::Rules::Manipulators::Not operator !(const Stringozzi::Rules::TokenizerInterface& tok)
{
	return Stringozzi::Rules::Manipulators::Not(tok);
}

//inline const Stringozzi::Rules::Manipulators::OneOrMore operator ++(const Stringozzi::Rules::TokenizerInterface& tok)
//{
//	return Stringozzi::Rules::Manipulators::OneOrMore(tok);
//}


//inline Parsing::Rules::Arithmatics::Sequence operator >> (Parsing::Rules::TokenizerInterface& t1, const char* t2)
//{
//	return Parsing::Rules::Arithmatics::Sequence(t1, Parsing::Rules::Tokens::Exact(t2));
//}

//inline Parsing::Rules::Arithmatics::Sequence operator >> (Parsing::Rules::TokenizerInterface& t1, Parsing::Rules::TokenizerInterface& t2)
//{
//	return Parsing::Rules::Arithmatics::Sequence(t1, t2);
//}
