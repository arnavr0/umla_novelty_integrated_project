# Novelty integration notes

This package adds an extra layer of security to the original project. The main goal is to go beyond just cleaning up the text; we now create a clear, easy-to-understand **risk score** to help evaluate and prioritize commands.

## Where the new features are added in the code

### 1. `src/libumlec/libumlec.cpp`
We updated this file to handle these tasks:
- change special, tricky letters back into normal alphabet letters,
- count how many hidden character replacements were made,
- remove tricks used to change the text direction (like right-to-left text),
- provide the main `NormalizeCommand(...)` and `ScoreCommand(...)` functions.

This acts as the main engine that both tools use.

### 2. `src/checker/checker.cpp`
We changed the old, simple conversion process into a full evaluation step. Now it will:
- read the text command,
- clean it up (normalize it),
- calculate its risk score,
- print out a clear, organized report.

If you are looking at the original user interface, this means we updated what the "Convert" button does.

### 3. `src/analyzer/analyzer.cpp`
We kept the original menus exactly as they were, but added two new options:
- `53. Check the risk score for one command`
- `54. Check the risk scores for a whole file of commands`

This way, the old tool still works perfectly while offering the new features.

## Why these additions matter

The original project already cleans up text and spots hidden text.  
The new value we bring here is adding these extra steps:
- measuring how hard an attack is trying to hide itself,
- explaining exactly why a command looks dangerous,
- telling the difference between harmless text quirks and actual malicious attacks.

This makes the tool much better for security teams who need to evaluate daily threats and write reports.
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
