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
- Clique direito: magia de área (Ataque em Área)
- E: Barreira
- Espaço: Dash
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


#  OBSCURA - V0.4.0
Funcionalidades:
- Mundo expandido, limites de movimentação, câmera centralizada e limitada
- Deadzone (zona morta) para suavizar o movimento da câmera
- Grid de debug com coordenadas
- Tipos de inimigos (NORMAL, FAST, STRONG) com spawn probabilístico e spawn fora da área andável
- Inimigos perseguem o centro do player
- Sistema de XP, level, skill points
- XP ao matar inimigos, barra de XP no HUD, level up aumenta atributos
- Ataque circular e invencibilidade temporária ao subir de nível
- Floating text: valor de XP ganho aparece acima do personagem
- Sistema de partículas (particle.h/.c), efeito visual ao upar, pronto para magias
- HUD mostra vida, XP, nível, skill points, FPS
- Menus de jogo, pausa, game over
- Hotkey bar de magias (slots 1,2,3), efeito visual de cooldown
- Modularização do áudio: criação de audio.h/.c, centralização dos efeitos sonoros
- Sons para tiro, dano no player, level up, morte de inimigo integrados e tocando nos eventos corretos
- Ajuste de volume e pitch via funções do módulo de áudio
- Correção da música de game over para tocar apenas uma vez
- Modularização inicial: headers e arquivos fonte para áudio, magias, UI/HUD
- Limpeza de código, remoção de warnings, atualização de comentários
- Ajuste de colisão para alinhar visualmente player e inimigos
- Makefile ajustado para incluir resource.res (ícone do .exe)
- Orientações para exportar o executável junto com a pasta assets
- Explicação sobre falsos positivos de antivírus e assinatura digital
- Implementação de cursor customizado com Texture2D, escala ajustável e offset para centralização
- Sistema simplificado para usar apenas um sprite de cursor
- Orientação para alinhar o centro do cursor à posição do mouse
ara dúvidas ou sugestões, abra uma issue ou envie um pull request!
