#include "cxxopts.hpp"
#include "pbrt_parser/parser.h"
#include "scene.h"
#include "render.h"

#include <embree4/rtcore.h>
#include <stdio.h>
#include <math.h>
#include <limits>
#include <stdio.h>

#if defined(_WIN32)
#  include <conio.h>
#  include <windows.h>
#endif


int main(int argc, char* argv[])
{
	cxxopts::Options opts("Alpha7XRender", "Tiny Offline Renderer");
	opts.allow_unrecognised_options();
	opts.add_options()
		("i,input_pbrt_scene", "Input pbrt scene path", cxxopts::value<std::string>())
		("o,output_image", "output image path", cxxopts::value<std::string>())
		("h,help", "Print help message.");

	auto opt_result = opts.parse(argc, argv);
	if (opt_result.count("h") || argc < 2 || opt_result.count("i") == 0)
	{
		printf(opts.help().c_str());
		printf("\n");
		exit(-1);
	}

	std::string input_pbrt_scene_path = opt_result["i"].as<std::string>();
	
	CAlpa7XScene scene;
	Alpha7XSceneBuilder builder(&scene);
	pbrt::ParseFile(&builder, input_pbrt_scene_path);

	{
		renderScene(scene);
	}

	return 0;
}