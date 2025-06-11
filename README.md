<!-- filepath: c:\Obscura\README.md -->
# Obscura

Obscura é um jogo top-down em C, usando Raylib, com inspiração em Darksouls e Vampire Survivors. Projeto inicialmente voltado para a matéria de Programação Avançada.

## Funcionalidades

- **Mundo e Câmera:**
    - Mundo expansivo com limites de movimentação.
    - Câmera com deadzone para suavização de movimento, centralizada e limitada à área do jogo.
    - Grid de debug opcional com coordenadas.
- **Jogador:**
    - Movimentação (WASD) e mira controlada pelo mouse.
    - Animação de movimento com espelhamento dinâmico (personagem vira para esquerda/direita).
    - Sistema de desenho do jogador desacoplado de variáveis globais, utilizando texturas passadas como parâmetro.
    - Escala visual do sprite do jogador ajustável (via `visualScale`), permitindo que o visual seja maior que a hitbox para melhor estética, mantendo a hitbox precisa para colisões.
    - Ataque básico (clique esquerdo).
    - Hitbox do jogador visível para debug (opcional).
- **Inimigos:**
    - Tipos variados (NORMAL, FAST, STRONG) com comportamentos distintos.
    - Spawn probabilístico e estratégico (fora da área visível/andável).
    - Perseguição inteligente ao jogador.
- **Sistema de Progressão:**
    - Ganho de XP ao derrotar inimigos.
    - Sistema de Nível (Level Up) que aumenta atributos do jogador.
    - Skill points para customização (mecânica a ser expandida).
    - Floating text para feedback de XP ganho.
    - Efeito visual e sonoro de level up, com ataque circular e invencibilidade temporária.
- **Combate e Magias:**
    - Hotkey bar para acesso rápido a magias (slots 1,2,3) com feedback visual de cooldown.
    - Magias implementadas: Ataque em Área (clique direito), Barreira (E), Dash (Espaço).
- **Interface (HUD) e Menus:**
    - HUD completo: barra de vida, barra de XP, nível atual, skill points, contador de FPS.
    - Menus de jogo: pausa e game over.
    - Cursor customizado centralizado na mira.
- **Áudio:**
    - Sistema de áudio modularizado.
    - Efeitos sonoros para eventos importantes (tiro, dano, level up, morte de inimigo).
    - Controle de volume e pitch para sons.
    - Música de game over corrigida para tocar uma única vez.
- **Técnico:**
    - Código modularizado (áudio, magias, UI, partículas, jogador, inimigos, etc.).
    - Makefile para compilação simplificada (com suporte a `resource.res` para ícone do executável).
    - Sistema de partículas para efeitos visuais diversos (level up, magias).
    - Colisões ajustadas para melhor alinhamento visual entre jogador e inimigos.
- **Distribuição e Extras:**
    - Instruções para distribuição do jogo (executável, DLLs, assets).
    - Informações sobre possíveis falsos positivos de antivírus.

## Requisitos
- [Raylib 4.x](https://www.raylib.com/) (incluso: `libraylib.dll` na raiz do projeto ou `bin/`)
- Compilador C (MinGW recomendado no Windows)

## Compilação
Foi usado o MSYS para poder compilar o game, no painel do MSYS, acesse a pasta raíz e digite: 
```sh
make clean && make && make run
```
O executável será gerado em `bin/Obscura.exe`.

## Execução
Execute `Obscura.exe` a partir da raiz do projeto. A pasta `assets/` deve estar ao lado do executável para que os recursos sejam carregados corretamente.

## Estrutura de Pastas
- `assets/` — sprites, sons, fontes, ícones
- `src/` — código-fonte principal (.c)
- `include/` — arquivos de cabeçalho (.h)
- `bin/` — executável e DLLs após compilação
- `obj/` — arquivos objeto temporários da compilação
- `lib/` — bibliotecas (como raylib, se não for usada a versão global)

## Distribuição
- Inclua `Obscura.exe` (de `bin/`), `libraylib.dll` (se não estiver junto ao .exe, copie da pasta `lib` ou `raylib/release/mingw-w64` da sua instalação Raylib) e a pasta `assets/` completa.
- O ícone do executável é definido via `resource.res` durante a compilação pelo Makefile.
- Falsos positivos de antivírus podem ocorrer com executáveis não assinados; para distribuição mais ampla, considere assinar digitalmente o executável.

## Controles
- **WASD**: Mover o personagem
- **Mouse**: Mirar
- **Clique Esquerdo**: Ataque básico / Interagir
- **Clique Direito**: Magia de Ataque em Área
- **E**: Magia de Barreira
- **Espaço**: Magia de Dash
- **ESC**: Abrir/Fechar menu de pausa

## Créditos
- Raylib (www.raylib.com) por Ramon Santamaria e colaboradores.
- Ícones, sons e sprites: (detalhar fontes específicas se conhecidas, ou referenciar a pasta `assets/` para exploração).

## TODO / Roadmap
- Sistema de desbloqueio e progressão de magias mais elaborado.
- Drag & drop de magias na hotkey bar.
- Mais efeitos visuais e sonoros para feedback e ambientação.
- Sistema de drops de itens e power-ups de inimigos.
- Implementação de bosses com mecânicas únicas.
- Dificuldade dinâmica ou níveis de dificuldade selecionáveis.
- Melhorias na IA dos inimigos.
- Mais tipos de inimigos e desafios.
- Salvar/Carregar progresso do jogo.
- Otimizações de performance.

---
