#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>

int main() {
    std::string temp, linee, mem, dur, a;

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
        std::vector<std::pair<unsigned int, unsigned int> > dura;

        if (in_stream.good() && in_stream.is_open()) {
            std::getline(in_stream, temp);
            std::getline(in_stream, temp);
            std::getline(in_stream, temp);
            std::getline(in_stream, temp);
            std::getline(in_stream, temp);
            std::getline(in_stream, temp);
            std::getline(in_stream, temp);

            int count = 0;
    
            while (std::getline(in_stream, temp)) {
                std::stringstream liner(temp);
                std::getline(liner, linee, ',');

                count++;

                std::vector<unsigned int> rombongan;
        
                std::stringstream ss(linee);

                while (ss >> mem) {
                    rombongan.push_back(std::stoul(mem));
                }

                std::getline(liner, linee);

                std::stringstream dura_ss(linee);

                while (dura_ss >> dur) {
                    auto pos = dur.find('-');

                    auto first = dur.substr(0, pos);
                    auto second = dur.substr(pos + 1);

                    romb.push_back(rombongan);
                    dura.push_back({ std::stoul(first), std::stoul(second) });
                }
            }

            std::ofstream out_stream;

            out_stream.open(
                file_name,
                std::ofstream::out | std::ofstream::trunc
            );

            if (out_stream.is_open()) {
                out_stream << count << std::endl;

                for (size_t itr = 0; itr < romb.size(); itr++) {
                    for (size_t mem_itr = 0; mem_itr < romb[itr].size(); mem_itr++) {
                        if (mem_itr > 0) {
                            out_stream << " ";
                        }

                        out_stream << romb[itr][mem_itr];
                    }

                    out_stream << std::endl;
                    out_stream << dura[itr].first << " " << dura[itr].second << std::endl;
                }

                out_stream.close();
            }
        }
    }

    return 0;
}