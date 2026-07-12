import os
import re
from pathlib import Path

root = Path(r'd:\Intern_FTEL\Fly_Hunter_game\Game_example\Workspace\Sources\fly-hunter-game\application')

skip_exts = {'.bin', '.o', '.a', '.elf', '.hex', '.map', '.lst', '.obj', '.out', '.pdf', '.png', '.jpg', '.jpeg', '.gif', '.bmp', '.ico', '.zip'}


def repl_token(match):
    token = match.group(0)
    if token.lower().endswith('flys'):
        base = 'FLY' if token.isupper() else ('Fly' if token[:1].isupper() else 'fly')
        return base + 's'
    if token.lower() == 'fly':
        return 'FLY' if token.isupper() else ('Fly' if token[:1].isupper() else 'fly')
    return token

for path in list(root.rglob('*')):
    if not path.is_file():
        continue
    if path.suffix.lower() in skip_exts:
        continue
    try:
        text = path.read_text(encoding='utf-8', errors='ignore')
    except Exception:
        continue
    if 'fly' not in text.lower():
        continue
    new_text = re.sub(r'flys|fly', repl_token, text, flags=re.IGNORECASE)
    if new_text != text:
        path.write_text(new_text, encoding='utf-8')

for path in list(root.rglob('*')):
    if not path.is_file():
        continue
    if 'fly' not in path.name.lower():
        continue
    new_name = re.sub(r'flys|fly', repl_token, path.name, flags=re.IGNORECASE)
    new_path = path.with_name(new_name)
    if new_path != path and not new_path.exists():
        os.replace(path, new_path)
