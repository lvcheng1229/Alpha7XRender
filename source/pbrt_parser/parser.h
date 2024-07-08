// pbrt is Copyright(c) 1998-2020 Matt Pharr, Wenzel Jakob, and Greg Humphreys.
// The pbrt source code is licensed under the Apache License, Version 2.0.
// SPDX: Apache-2.0

#ifndef PBRT_PARSER_H
#define PBRT_PARSER_H

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <optional>


namespace pbrt {

	// ParserTarget Definition
	class ParserTarget {
	public:

		virtual void EndOfFiles() = 0;
	protected:
	};

	// Scene Parsing Declarations
	void ParseFile(ParserTarget* target, const std::string& filename);

	// Token Definition
	struct Token {
		Token() = default;
		Token(std::string_view token) : token(token){}
		std::string ToString() const;
		std::string_view token;
	};

	// Tokenizer Definition
	class Tokenizer {
	public:
		Tokenizer(void* ptr, size_t len,
			std::function<void(const char*)> errorCallback);

		~Tokenizer();

		static std::unique_ptr<Tokenizer> CreateFromFile(
			const std::string& filename,
			std::function<void(const char*)> errorCallback);
		
		std::optional<Token> Next();
	private:
		// Tokenizer Private Methods
		void CheckUTF(const void* ptr, int len) const;

		int getChar() {
			if (pos == end)
				return EOF;
			int ch = *pos++;
			return ch;
		}
		void ungetChar() {
			--pos;
		}

		// Tokenizer Private Members
		// This function is called if there is an error during lexing.
		std::function<void(const char*)> errorCallback;

		char* srcDataPtr;

		// Pointers to the current position in the file and one past the end of
		// the file.
		const char* pos, * end;

		// If there are escaped characters in the string, we can't just return
		// a std::string_view into the mapped file. In that case, we handle the
		// escaped characters and return a std::string_view to sEscaped.  (And
		// thence, std::string_views from previous calls to Next() must be invalid
		// after a subsequent call, since we may reuse sEscaped.)
		std::string sEscaped;
	};
}
#endif