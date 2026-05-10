#pragma once

#include <string>
#include <vector>

namespace umla_novelty {

struct RiskRule {
    const wchar_t* pattern;
    int points;
    const wchar_t* label;
};

inline const std::vector<RiskRule>& BuiltInRiskRules() {
    static const std::vector<RiskRule> rules = {
        {LR"(\breg\s+save\b)", 28, L"registry hive export"},
        {LR"(\breg\s+delete\b)", 24, L"registry modification"},
        {LR"(\bcertutil\b.*(-urlcache|-split))", 24, L"certutil download/cache usage"},
        {LR"(\btaskkill\b.*\blsass\.exe\b)", 30, L"critical process termination"},
        {LR"(\bschtasks?\b.*\b/create\b)", 18, L"task creation"},
        {LR"(\bpowershell\b.*(-enc|encodedcommand))", 18, L"encoded PowerShell"},
        {LR"(\b(?:curl|wget)\b.*https?://)", 10, L"network fetch"},
    };
    return rules;
}

} // namespace umla_novelty
