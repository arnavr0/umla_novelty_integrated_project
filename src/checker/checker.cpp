#include "../libumlec/libumlec.h"

#include <codecvt>
#include <fstream>
#include <iostream>
#include <locale>
#include <string>
#include <vector>

using namespace umla_novelty;

namespace {

std::wstring Utf8ToWide(const std::string& input) {
    try {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
        return conv.from_bytes(input);
    } catch (...) {
        return std::wstring(input.begin(), input.end());
    }
}

std::string WideToUtf8(const std::wstring& input) {
    try {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
        return conv.to_bytes(input);
    } catch (...) {
        return std::string(input.begin(), input.end());
    }
}

std::wstring JoinArgs(int argc, char* argv[]) {
    std::wstring result;
    for (int i = 1; i < argc; ++i) {
        if (i > 1) result.push_back(L' ');
        result += Utf8ToWide(argv[i]);
    }
    return result;
}

} // namespace

int main(int argc, char* argv[]) {
    std::wstring input = JoinArgs(argc, argv);

    if (input.empty()) {
        std::cout << "Unicode Evasion Risk Checker\n";
        std::cout << "Enter a command line to analyze:\n> ";
        std::string raw;
        std::getline(std::cin, raw);
        input = Utf8ToWide(raw);
    }

    if (input.empty()) {
        std::cout << "No input provided.\n";
        return 0;
    }

    const auto assessment = ScoreCommand(input);
    std::cout << "\n" << WideToUtf8(FormatAssessment(assessment)) << "\n";
    return 0;
}
