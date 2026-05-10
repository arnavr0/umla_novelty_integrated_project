#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace umla_novelty {

struct NormalizationResult {
    std::wstring original;
    std::wstring normalized;
    std::size_t replacedCount = 0;
    std::size_t bidiControlCount = 0;
    std::vector<std::wstring> hits;
};

struct RiskAssessment {
    NormalizationResult normalized;
    int score = 0;
    std::wstring verdict;
    std::vector<std::wstring> reasons;
};

bool IsModifierLetter(wchar_t ch);
bool IsBidiControl(wchar_t ch);

std::wstring Trim(const std::wstring& text);
std::wstring NormalizeModifierLetters(const std::wstring& text, NormalizationResult* meta = nullptr);
NormalizationResult NormalizeCommand(const std::wstring& text);

int CountModifierLetters(const std::wstring& text);
int CountBidiControls(const std::wstring& text);
bool HasMixedScript(const std::wstring& text);

RiskAssessment ScoreCommand(const std::wstring& command);
std::wstring FormatAssessment(const RiskAssessment& assessment);

} // namespace umla_novelty
