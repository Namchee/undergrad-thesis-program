#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>

int main() {
    std::string temp, linee, mem;

    for (const auto& entry: std::filesystem::directory_iterator(std::filesystem::current_path())) {
        std::string name_string = entry.path().filename().string();

        if (
            name_string.find("simp") != std::string::npos ||
            entry.path().extension() != ".txt" ||
            entry.is_directory()
        ) {
            continue;
        }

        std::ifstream in_stream(entry.path());

        std::string file_name = (std::filesystem::current_path() / (entry.path().stem().string() + "-simp"))
            .make_preferred()
            .replace_extension(".txt");
        std::vector<std::vector<unsigned int> > romb;

        if (in_stream.good() && in_stream.is_open()) {
            std::getline(in_stream, temp);
            std::getline(in_stream, temp);
            std::getline(in_stream, temp);
            std::getline(in_stream, temp);
            std::getline(in_stream, temp);
            std::getline(in_stream, temp);
            std::getline(in_stream, temp);
    
            while (std::getline(in_stream, temp)) {
                std::stringstream liner(temp);
                std::getline(liner, linee, ',');

                std::vector<unsigned int> rombongan;
        
                std::stringstream ss(linee);

                while (ss >> mem) {
                    rombongan.push_back(std::stoul(mem));
                }

                romb.push_back(rombongan);

                continue;
            }

            std::ofstream out_stream;

            out_stream.open(
                file_name,
                std::ofstream::out | std::ofstream::trunc
            );

            if (out_stream.is_open()) {
                for (const auto& x: romb) {
                    for (const auto& y: x) {
                        out_stream << y << " ";
                    }

                    out_stream << std::endl;
                }

                out_stream.close();
            }
        }
    }

    return 0;
}