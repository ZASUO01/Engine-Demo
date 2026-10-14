# Flappy Bird (demo da Engine)

O jogo inteiro está em [`FlappyBird.cpp`](FlappyBird.cpp) (~150 linhas de código, sem contar includes e comentários).
A Engine fornece loop, carregamento assíncrono de recursos, render 3D com sombras, física
(gravidade + triggers de colisão), câmera e texto na tela. O jogo só descreve atores e regras.

## Como rodar

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target FlappyBird -j
cd build && ./FlappyBird      # rodar de dentro de build/ (os assets são copiados para lá)
```

## Controles

- `ESPAÇO`: começar / bater as asas / reiniciar após o game over
- Teclas da própria Engine continuam valendo: `1` modelos, `2` luz/sombras, `4` mostra os colisores
