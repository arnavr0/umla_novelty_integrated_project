# Novelty integration notes

This package adds a defensive layer to the original artifact. The key idea is to go beyond plain normalization and produce an explainable **risk score** for command-line triage.

## Integration points in the original code

### 1. `src/libumlec/libumlec.cpp`
Add the following responsibilities:
- map modifier letters to base Latin characters,
- count Unicode substitutions,
- drop bidi controls,
- expose `NormalizeCommand(...)` and `ScoreCommand(...)`.

This is the shared engine used by both tools.

### 2. `src/checker/checker.cpp`
Replace the old single-action conversion flow with a triage flow:
- read a command string,
- normalize it,
- compute the score,
- print a structured result.

In the original GUI, this corresponds to updating the Convert button handler.

### 3. `src/analyzer/analyzer.cpp`
Keep the original demo and scanning menu, then add:
- `53. Risk-score one command`
- `54. Batch-score commands from a file`

That keeps the existing artifact usable while adding the novelty mode.

## Why this counts as novelty

The original artifact already performs normalization and detection.  
The novelty here is the extra step of:
- ranking evasion strength,
- explaining why a command is suspicious,
- separating low-risk obfuscation from likely malicious evasion.

That makes the tool more useful for SOC triage and report writing.
