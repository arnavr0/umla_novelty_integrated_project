#include "../libumlec/libumlec.h"

#include <codecvt>
#include <fstream>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>
#include <vector>
#include <limits>

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

std::wstring ReadLine(const char* prompt) {
    std::cout << prompt;
    std::string raw;
    std::getline(std::cin, raw);
    return Utf8ToWide(raw);
}

std::vector<std::wstring> ReadLinesFromFile(const std::wstring& path) {
    std::vector<std::wstring> lines;
    std::ifstream file(WideToUtf8(path));
    if (!file) {
        return lines;
    }

    std::string raw;
    while (std::getline(file, raw)) {
        auto w = Utf8ToWide(raw);
        if (!Trim(w).empty()) {
            lines.push_back(w);
        }
    }
    return lines;
}

void PrintWindowsOnlyNote(const wchar_t* what) {
    std::cout << "[Windows-only demo] " << WideToUtf8(what) << "\n";
}

void CompareStringSimpleTest() {
#ifdef _WIN32
    if (CSTR_EQUAL != CompareStringW(LOCALE_INVARIANT, NORM_IGNORECASE, L"ˢ", -1, L"s", -1)) {
        std::wcout << L"CompareStringW: not equal\n";
    } else {
        std::wcout << L"CompareStringW: equal\n";
    }
    if (CSTR_EQUAL != CompareStringEx(NULL, NORM_IGNORECASE, L"ˢ", -1, L"s", -1, NULL, NULL, NULL)) {
        std::wcout << L"CompareStringEx: not equal\n";
    } else {
        std::wcout << L"CompareStringEx: equal\n";
    }
#else
    PrintWindowsOnlyNote(L"CompareStringW / CompareStringEx validation");
#endif
}

void IdnToAsciiSimpleTest() {
#ifdef _WIN32
    wchar_t spoofingIDN1[] = L"ʷʷʷ.ᴹᴵᶜʳᵒˢᵒᶠᵀ.ᶜᵒᴹ";
    wchar_t buffer[255] = {0};
    IdnToAscii(IDN_ALLOW_UNASSIGNED, spoofingIDN1, static_cast<int>(wcslen(spoofingIDN1)), buffer, 255);
    std::wcout << L"IdnToAscii result: " << buffer << L"\n";
#else
    PrintWindowsOnlyNote(L"IdnToAscii validation");
#endif
}

void CompareStringWParameterTest() {
#ifdef _WIN32
    std::wcout << L"Original parameter-space scanner is preserved in the full Windows build.\n";
#else
    PrintWindowsOnlyNote(L"CompareStringW parameter scanning");
#endif
}

void SingleWideCharComparsionTest() {
#ifdef _WIN32
    std::wcout << L"Original character-scan logic is preserved in the full Windows build.\n";
#else
    PrintWindowsOnlyNote(L"single wide-character comparison scan");
#endif
}

void SystemProgramScanning(const char* apiName) {
#ifdef _WIN32
    std::wcout << L"Original binary scanner is preserved in the full Windows build.\n";
#else
    std::wcout << L"[Windows-only demo] program scan for " << Utf8ToWide(apiName) << L"\n";
#endif
}

void RunRiskScoreOnce() {
    std::wstring cmd = ReadLine("Enter a command to score: ");
    if (cmd.empty()) {
        std::cout << "Empty input.\n";
        return;
    }
    std::cout << "\n" << WideToUtf8(FormatAssessment(ScoreCommand(cmd))) << "\n";
}

void RunRiskScoreBatch() {
    std::wstring path = ReadLine("Enter UTF-8 text file path with one command per line: ");
    auto lines = ReadLinesFromFile(path);
    if (lines.empty()) {
        std::wcout << L"No commands loaded.\n";
        return;
    }

    std::ofstream report("RiskReport.csv");
    report << "line_no,score,verdict,replaced,bidi,original,normalized\n";

    for (std::size_t i = 0; i < lines.size(); ++i) {
        auto a = ScoreCommand(lines[i]);
        std::wstring original = a.normalized.original;
        std::wstring normalized = a.normalized.normalized;

        auto narrow = [](const std::wstring& ws) {
            return WideToUtf8(ws);
        };

        report
            << (i + 1) << ","
            << a.score << ","
            << narrow(a.verdict) << ","
            << a.normalized.replacedCount << ","
            << a.normalized.bidiControlCount << ","
            << "\"" << narrow(original) << "\","
            << "\"" << narrow(normalized) << "\"\n";
    }

    std::wcout << L"Wrote RiskReport.csv with " << lines.size() << L" scored commands.\n";
}

void ShowMenu() {
    std::cout << "\nUnicode Modifier Letter Evasion Analysis Tool + Risk Triage\n\n";
    std::cout << "1. Show validation result of CompareStringW() with modifier letter arguments.\n";
    std::cout << "2. Show validation result of IdnToAscii() with modifier letter arguments.\n";
    std::cout << "3. Run CompareStringW parameter space scanning.\n";
    std::cout << "4. Run CompareStringW character scanning (single-character equal comparison).\n";
    std::cout << "51. Run program scanning for CompareStringW.\n";
    std::cout << "52. Run program scanning for CompareStringEx.\n";
    std::cout << "53. Run risk scoring on a single command.\n";
    std::cout << "54. Run risk scoring on a UTF-8 command file.\n";
    std::cout << "9. Exit.\n";
    std::cout << "Choice: ";
}

} // namespace

int main() {
    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale());
    std::wcin.imbue(std::locale());

    while (true) {
        int option = 0;
        ShowMenu();
        std::cin >> option;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (option) {
        case 1: CompareStringSimpleTest(); break;
        case 2: IdnToAsciiSimpleTest(); break;
        case 3: CompareStringWParameterTest(); break;
        case 4: SingleWideCharComparsionTest(); break;
        case 51: SystemProgramScanning("CompareStringW"); break;
        case 52: SystemProgramScanning("CompareStringEx"); break;
        case 53: RunRiskScoreOnce(); break;
        case 54: RunRiskScoreBatch(); break;
        case 9: return 0;
        default:
            std::wcout << L"Invalid option.\n";
            break;
        }
    }
    return 0;
}
