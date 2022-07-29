/**
 * @file main.cpp
 * @author hasan karaman (hk@hasankaraman.dev)
 * @version 0.1
 * @date 2022-07-27
 *
 * @copyright Copyright (c) 2022
 */

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <functional>
#include <nlohmann/json.hpp>

#define UNUSED(x) ((void)(x))

using csv_t = std::vector<std::vector<std::string>>;
using callback_t = std::function<void(const std::string &)>;

namespace options
{
	// if true, first line of file will be treated as header.
	bool header = true;
	// if true, output json will be beautified.
	bool beautify = false;
	// name (key) of the entire json will be set as 'json_name'.
	std::string json_name = "data";
	// default delimiter to separate the columns
	char delimiter = ',';
} // namespace options

int main(int argc, char *argv[])
{
	std::map<std::string, std::shared_ptr<callback_t>> flags;

	flags["-h"] = flags["--help"] = std::make_shared<callback_t>([](const std::string &next_arg) {
		UNUSED(next_arg);
		std::cout << "csv2json 0.1 - compiled: " << __DATE__ " " __TIME__ "\n"
					"options:\n\t-h, --help\tprints this helpful message\n"
					"\t-b, --beautify\tjson output will be beautified\n"
					"\t-d, --delimiter\tmust be followed by a single character such as ';' or the word `tab` if the input file is tab separated. default is comma (,)\n"
					"\t-n, --noheader\tfirst line will not be treated as header, field names will be automatically generated\n"
					"example use:\n\tcsv2json meta/sample/points.csv --beautify\n"
					"\tcsv2json meta/sample/points-noheader.csv --beautify --noheader\n"
					"\tcsv2json meta/sample/points-tab-separated.csv --delimiter tab --beautify\n"
					"\nhk@hasankaraman.dev\n";
		std::exit(EXIT_SUCCESS); });

	flags["-n"] = flags["--noheader"] = std::make_shared<callback_t>([&](const std::string &next_arg) {
		UNUSED(next_arg);
		options::header = false; });

	flags["-b"] = flags["--beautify"] = std::make_shared<callback_t>([&](const std::string &next_arg) {
		UNUSED(next_arg);
		options::beautify = true; });

	flags["-d"] = flags["--delimiter"] = std::make_shared<callback_t>([&](const std::string &next_arg) {
		if (next_arg.size() == 1)
			options::delimiter = next_arg.at(0);
		else if (next_arg == "tab")
		{
			options::delimiter = '\t';
		}
		else
		{
			std::cerr << "invalid delimiter! try something like ','." << std::endl;
		} });

	std::vector<std::string> args;
	{
		/** we skip the index 0 of argv */
		if (argc > 1)
			args = std::vector<std::string>(argv + 1, argv + argc);
		else
		{
			(*flags["--help"])("");
			return 1;
		}
	}

	std::string file_path;
	for (auto arg = args.begin(); arg != args.end(); ++arg)
	{
		if (flags.count(*arg))
		{
			std::string next_arg;
			if (std::next(arg) != args.end())
			{
				next_arg = *std::next(arg);
			}
			(*flags[*arg])(next_arg);
		}
		if (arg == args.begin())
			file_path = *arg;
	}

	if (file_path.empty())
	{
		std::cerr << "at least provide a filename :(\n";
		return 1;
	}

	csv_t csv_data;

	std::ifstream ifs(file_path);
	if (ifs.is_open())
	{
		std::string line;
		while (std::getline(ifs, line))
		{
			std::string token;
			std::vector<std::string> tokens;
			std::istringstream iss(line);
			while (std::getline(iss, token, options::delimiter))
			{
				tokens.push_back(token);
			}
			csv_data.push_back(tokens);
		}
		ifs.close();
	}
	else
	{
		std::cerr << "file '" << file_path << "' not found\n";
		return EXIT_FAILURE;
	}

	std::vector<std::string> headers;
	{
		if (options::header)
			headers = csv_data[0];
		else
			for (std::size_t i = 0; i < csv_data[0].size(); i++)
				headers.push_back(std::string("d_") + std::to_string(i));
	}

	nlohmann::json j;
	j["data"] = nlohmann::json::array();

	auto first = options::header ? ++csv_data.begin() : csv_data.begin();
	for (auto row = first; row != csv_data.end(); ++row)
	{
		nlohmann::json j1;
		for (std::size_t i = 0; i < row->size(); i++)
		{
			/** if column of a row is an empty string, we call it null */
			if (row->at(i).empty())
			{
				j1[headers[i]] = nullptr;
			}
			else
			{
				/** check if string represents an integer. */
				try
				{
					size_t index = -1;
					int val = std::stoi(row->at(i), &index);
					if (index == row->at(i).size())
					{
						j1[headers[i]] = val;
						continue;
					}
				}
				catch (...) {}

				/** check if string represents a float. */
				try
				{
					size_t index = -1;
					double val = std::stod(row->at(i), &index);
					if (index == row->at(i).size())
					{
						j1[headers[i]] = val;
						continue;
					}
				}
				catch (...) {}

				/** if string is not empty (null), integer or double
				 * we pass the string itself. */
				j1[headers[i]] = row->at(i);
			}
		}
		j["data"].push_back(j1);
	}

	std::cout << j.dump((options::beautify ? 1 : -1), '\t') << std::endl;

	return 0;
}
