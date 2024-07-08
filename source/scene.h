#pragma once
#include "pbrt_parser/parser.h"
class Alpha7XSceneBuilder : public pbrt::ParserTarget
{
public:
	friend void parse(pbrt::ParserTarget* scene, std::unique_ptr<pbrt::Tokenizer> t);

	void EndOfFiles();
private:
};