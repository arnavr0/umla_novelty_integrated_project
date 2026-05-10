# UMLA Novelty Integration Package

This source tree extends the original **Unicode Modifier Letter Evasion Attack** artifact with a defensive novelty layer.

## What changed

The existing idea in the paper artifact is kept intact:
- Unicode modifier-letter normalization
- detection of obfuscated command lines
- analyzer-style reporting

The novelty added here is a **risk-scoring triage layer** that:
- normalizes modifier letters,
- measures obfuscation strength,
- flags mixed-script or bidi-controlled input,
- assigns a readable risk score,
- ranks likely evasion attempts before rule matching.

## Main source files

- `src/libumlec/libumlec.h`
- `src/libumlec/libumlec.cpp`
- `src/libumlec/risk_rules.h`
- `src/analyzer/analyzer.cpp`
- `src/checker/checker.cpp`

## How to use

The project is organized so the same scoring core can be called from:
- the analyzer tool, and
- the checker tool.

The `checker.cpp` file is written as a self-contained console triage demo.  
In the original Win32 GUI, the same `ScoreCommand(...)` call can be placed inside the Convert button handler so the UI shows:
- original text
- normalized text
- number of Unicode substitutions
- risk score
- verdict and reasons

## Extra files

- `examples/sample_commands.txt` — sample obfuscated commands
- `tests/expected_output_snippets.txt` — simple sanity expectations
- `NOVELTY_NOTES.md` — exact integration points

## Notes

This package is source-only. It is designed as a clean overlay for the original project structure and as a readable implementation of the novelty idea.
