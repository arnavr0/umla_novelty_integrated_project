#include "libumlec.h"
#include "risk_rules.h"

#include <algorithm>
#include <cwctype>
#include <locale>
#include <regex>
#include <sstream>
#include <unordered_map>

namespace umla_novelty {
namespace {

using Map = std::unordered_map<wchar_t, wchar_t>;

const Map& ModifierMap() {
    static const Map table = {
        {L'ˢ', L's'}, {L'ʷ', L'w'}, {L'ˣ', L'x'},
        {L'ᵃ', L'a'}, {L'ᵇ', L'b'}, {L'ᶜ', L'c'}, {L'ᵈ', L'd'},
        {L'ᵉ', L'e'}, {L'ᶠ', L'f'}, {L'ᵍ', L'g'}, {L'ʰ', L'h'},
        {L'ᶦ', L'i'}, {L'ʲ', L'j'}, {L'ᵏ', L'k'}, {L'ˡ', L'l'},
        {L'ᵐ', L'm'}, {L'ᶰ', L'n'}, {L'ᵒ', L'o'}, {L'ᵖ', L'p'},
        {L'ʳ', L'r'}, {L'ᵗ', L't'}, {L'ᵘ', L'u'}, {L'ᵛ', L'v'},
        {L'ʸ', L'y'}, {L'ᶻ', L'z'},

        {L'ᴬ', L'A'}, {L'ᴮ', L'B'}, {L'ᴰ', L'D'}, {L'ᴱ', L'E'},
        {L'ᴳ', L'G'}, {L'ᴴ', L'H'}, {L'ᴵ', L'I'}, {L'ᴶ', L'J'},
        {L'ᴷ', L'K'}, {L'ᴸ', L'L'}, {L'ᴹ', L'M'}, {L'ᴺ', L'N'},
        {L'ᴼ', L'O'}, {L'ᴾ', L'P'}, {L'ᴿ', L'R'}, {L'ᵀ', L'T'},
        {L'ᵁ', L'U'}, {L'ⱽ', L'V'}, {L'ᵂ', L'W'},

        {L'ᴀ', L'A'}, {L'ʙ', L'B'}, {L'ᴄ', L'C'}, {L'ᴅ', L'D'},
        {L'ᴇ', L'E'}, {L'ғ', L'F'}, {L'ɢ', L'G'}, {L'ʜ', L'H'},
        {L'ɪ', L'I'}, {L'ᴊ', L'J'}, {L'ᴋ', L'K'}, {L'ʟ', L'L'},
        {L'ᴍ', L'M'}, {L'ɴ', L'N'}, {L'ᴏ', L'O'}, {L'ᴘ', L'P'},
        {L'ʀ', L'R'}, {L'ᴛ', L'T'}, {L'ᴜ', L'U'}, {L'ᴠ', L'V'},
        {L'ᴡ', L'W'}, {L'ʏ', L'Y'}, {L'ᴢ', L'Z'},
    };
    return table;
}

bool IsAsciiLetter(wchar_t ch) {
    return (ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z');
}

bool IsNonAsciiLetter(wchar_t ch) {
    return std::iswalpha(ch) && !IsAsciiLetter(ch);
}

std::wstring LowerAsciiOnly(std::wstring text) {
    std::transform(text.begin(), text.end(), text.begin(), [](wchar_t ch) {
        if (ch >= L'A' && ch <= L'Z') {
            return static_cast<wchar_t>(ch - L'A' + L'a');
        }
        return ch;
    });
    return text;
}

std::wstring CollapseWhitespace(const std::wstring& text) {
    std::wstring out;
    out.reserve(text.size());
    bool inSpace = false;
    for (wchar_t ch : text) {
        const bool space = std::iswspace(ch) != 0;
        if (space) {
            inSpace = true;
            continue;
        }
        if (inSpace && !out.empty()) {
            out.push_back(L' ');
        }
        inSpace = false;
        out.push_back(ch);
    }
    return Trim(out);
}

template <typename T>
std::wstring ToWString(const T& value) {
    std::wstringstream ss;
    ss << value;
    return ss.str();
}

bool RegexMatch(const std::wstring& text, const std::wstring& pattern) {
    try {
        std::wregex rx(pattern, std::regex_constants::ECMAScript | std::regex_constants::icase);
        return std::regex_search(text, rx);
    } catch (...) {
        return false;
    }
}

std::wstring ScoreReason(const std::wstring& label, int points) {
    return label + L" (+" + ToWString(points) + L")";
}

} // namespace

bool IsModifierLetter(wchar_t ch) {
    const auto& map = ModifierMap();
    return map.find(ch) != map.end();
}

bool IsBidiControl(wchar_t ch) {
    switch (ch) {
    case 0x202A: // LRE
    case 0x202B: // RLE
    case 0x202C: // PDF
    case 0x202D: // LRO
    case 0x202E: // RLO
    case 0x2066: // LRI
    case 0x2067: // RLI
    case 0x2068: // FSI
    case 0x2069: // PDI
    case 0x200E: // LRM
    case 0x200F: // RLM
        return true;
    default:
        return false;
    }
}

std::wstring Trim(const std::wstring& text) {
    const auto begin = std::find_if_not(text.begin(), text.end(), [](wchar_t ch) {
        return std::iswspace(ch) != 0;
    });
    const auto end = std::find_if_not(text.rbegin(), text.rend(), [](wchar_t ch) {
        return std::iswspace(ch) != 0;
    }).base();

    if (begin >= end) {
        return L"";
    }
    return std::wstring(begin, end);
}

std::wstring NormalizeModifierLetters(const std::wstring& text, NormalizationResult* meta) {
    std::wstring out;
    out.reserve(text.size());

    std::size_t replaced = 0;
    std::size_t bidi = 0;
    std::vector<std::wstring> hits;

    const auto& map = ModifierMap();

    for (wchar_t ch : text) {
        if (IsBidiControl(ch)) {
            ++bidi;
            hits.emplace_back(L"bidi-control");
            continue;
        }

        auto it = map.find(ch);
        if (it != map.end()) {
            ++replaced;
            hits.push_back(std::wstring(1, ch) + L"->" + std::wstring(1, it->second));
            out.push_back(it->second);
            continue;
        }

        out.push_back(ch);
    }

    out = CollapseWhitespace(out);

    if (meta) {
        meta->original = text;
        meta->normalized = out;
        meta->replacedCount = replaced;
        meta->bidiControlCount = bidi;
        meta->hits = std::move(hits);
    }

    return out;
}

NormalizationResult NormalizeCommand(const std::wstring& text) {
    NormalizationResult result;
    result.normalized = NormalizeModifierLetters(text, &result);
    return result;
}

int CountModifierLetters(const std::wstring& text) {
    return static_cast<int>(std::count_if(text.begin(), text.end(), [](wchar_t ch) {
        return IsModifierLetter(ch);
    }));
}

int CountBidiControls(const std::wstring& text) {
    return static_cast<int>(std::count_if(text.begin(), text.end(), [](wchar_t ch) {
        return IsBidiControl(ch);
    }));
}

bool HasMixedScript(const std::wstring& text) {
    bool sawAscii = false;
    bool sawNonAscii = false;

    for (wchar_t ch : text) {
        if (IsAsciiLetter(ch)) {
            sawAscii = true;
        } else if (IsNonAsciiLetter(ch)) {
            sawNonAscii = true;
        }
        if (sawAscii && sawNonAscii) {
            return true;
        }
    }
    return false;
}

RiskAssessment ScoreCommand(const std::wstring& command) {
    RiskAssessment assessment;
    assessment.normalized = NormalizeCommand(command);

    int score = 0;

    if (assessment.normalized.replacedCount > 0) {
        const int add = std::min<int>(20, static_cast<int>(assessment.normalized.replacedCount) * 4);
        score += add;
        assessment.reasons.push_back(L"Unicode substitutions detected (" + ToWString(assessment.normalized.replacedCount) + L") " + ScoreReason(L"obfuscation", add));
    }

    if (assessment.normalized.bidiControlCount > 0) {
        const int add = std::min<int>(20, static_cast<int>(assessment.normalized.bidiControlCount) * 8);
        score += add;
        assessment.reasons.push_back(L"Bidi controls detected " + ScoreReason(L"directional obfuscation", add));
    }

    if (!assessment.normalized.hits.empty()) {
        score += 6;
        assessment.reasons.push_back(L"modifier-letter mapping was required " + ScoreReason(L"confusable mapping", 6));
    }

    if (HasMixedScript(command)) {
        score += 6;
        assessment.reasons.push_back(L"mixed ASCII / non-ASCII script pattern " + ScoreReason(L"mixed script", 6));
    }

    const std::wstring normalizedLower = LowerAsciiOnly(assessment.normalized.normalized);

    for (const auto& rule : BuiltInRiskRules()) {
        if (RegexMatch(normalizedLower, rule.pattern)) {
            score += rule.points;
            std::wstring reason = L"risky command pattern matched: ";
            reason += rule.label;
            reason += L" ";
            reason += ScoreReason(rule.label, rule.points);
            assessment.reasons.push_back(reason);
        }
    }

    if (assessment.normalized.original.size() > 128) {
        score += 3;
        assessment.reasons.push_back(L"long command line " + ScoreReason(L"length bias", 3));
    }

    if (score >= 40) {
        assessment.verdict = L"high";
    } else if (score >= 18) {
        assessment.verdict = L"medium";
    } else {
        assessment.verdict = L"low";
    }

    assessment.score = score;
    return assessment;
}

std::wstring FormatAssessment(const RiskAssessment& assessment) {
    std::wstringstream out;
    out << L"Original  : " << assessment.normalized.original << L"\n";
    out << L"Normalized: " << assessment.normalized.normalized << L"\n";
    out << L"Replaced  : " << assessment.normalized.replacedCount << L"\n";
    out << L"Bidi ctrls: " << assessment.normalized.bidiControlCount << L"\n";
    out << L"Risk score: " << assessment.score << L"\n";
    out << L"Verdict   : " << assessment.verdict << L"\n";

    if (!assessment.normalized.hits.empty()) {
        out << L"Hits      : ";
        for (std::size_t i = 0; i < assessment.normalized.hits.size(); ++i) {
            if (i) out << L", ";
            out << assessment.normalized.hits[i];
        }
        out << L"\n";
    }

    if (!assessment.reasons.empty()) {
        out << L"Reasons   :\n";
        for (const auto& reason : assessment.reasons) {
            out << L"  - " << reason << L"\n";
        }
    }
    return out.str();
}

} // namespace umla_novelty
