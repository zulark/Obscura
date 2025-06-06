# Obscura

Obscura é um jogo top-down em C, usando Raylib, com inspiração em Darksouls e Vampire Survivors. Projeto inicialmente voltado para a matéria de Programação Avançada 

## Funcionalidades
- Mundo grande, limites de movimentação, câmera com deadzone
- Inimigos variados (NORMAL, FAST, STRONG) com spawn probabilístico
- Sistema de XP, level, skill points, barra de XP, floating text
- Partículas para efeitos visuais (level up, magias, etc)
- HUD: vida, XP, nível, skill points, FPS, hotkey bar de magias
- Menus: pausa, game over
- Áudio modular: sons para eventos, controle de volume/pitch
- Cursor customizado centralizado
- Modularização: áudio, magias, UI, partículas, cursor
- Makefile para build e resource.res para ícone do executável

## Requisitos
- [Raylib 4.x](https://www.raylib.com/) (incluso: `libraylib.dll`)
- Compilador C (MinGW recomendado no Windows)

## Compilação
No Windows, com MinGW:
```sh
mingw32-make
```
O executável será gerado em `bin/Obscura.exe`.

## Execução
Execute `Obscura.exe` a partir da raiz do projeto ou garanta que a pasta `assets/` esteja ao lado do executável.

## Estrutura de Pastas
- `assets/` — sprites, sons, ícones
- `src/` — código-fonte principal
- `include/` — headers
- `bin/` — executável
- `obj/` — objetos temporários

## Distribuição
- Inclua `Obscura.exe`, `libraylib.dll` e a pasta `assets/`.
- O ícone do executável é definido via `resource.res`.
- Falsos positivos de antivírus podem ocorrer; para evitar, assine digitalmente o executável.

## Controles
- WASD: mover
- Mouse: mirar
- Clique esquerdo: atacar
- 1, 2, 3: usar magias (hotkey bar)
- ESC: menu/pause

## Créditos
- Raylib (www.raylib.com)
- Ícones e sons: ver pasta `assets/`

## TODO / Roadmap
- Magias reais (defesa, ataque em área, dash)
- Drag & drop na hotkey bar
- Sistema de desbloqueio de magias
- Mais efeitos visuais/sonoros
- Sistema de drops e power-ups
- Bosses e dificuldade dinâmica

---

Para dúvidas ou sugestões, abra uma issue ou envie um pull request!
