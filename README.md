# Projeto_Computação_Visual

**Autores:** [gebra04](http://github.com/gebra04), [HenriUz](http://github.com/HenriUz), [Jooaomarcelo](http://github.com/Jooaomarcelo), [ValimD](http://github.com/ValimD).

---

## Resumo
Este trabalho apresenta uma aplicação em C++ utilizando a biblioteca OpenGL para renderização de objetos 3D. A figura principal é uma ampulheta construída a partir de 12 triângulos e 9 pontos fixos. O projeto implementa movimentação da câmera em todos os eixos (X, Y, Z), além de sombreamento e coloração para realçar a visualização.

## Introdução
A computação visual em 3D é essencial em diversas áreas como jogos, simulações e visualização científica. Utilizando OpenGL, é possível controlar o pipeline gráfico de forma eficiente, manipulando vértices, cores e shaders. Neste projeto, exploramos conceitos básicos de transformação de coordenadas e iluminação simples para demonstrar a capacidade de renderizar um objeto geométrico complexos de forma interativa.

## Estrutura do Projeto
```

projeto/
├── images/
├── lib/
│   ├── utils.cpp
│   └── utils.h
├── main.cpp
└── Makefile

````

## Dependências e Ambiente
- **Compilador:** g++ (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0  
- **Padrão C++:** C++17 (`__cplusplus` = 201703L)  
- **Bibliotecas:** OpenGL 4.6, GLUT, GLEW  

## Build
No terminal, dentro da pasta do projeto, execute:

```bash
make
````

Para remover o executável gerado:

```bash
make clean
```

## Execução

Inicie o programa com:

```bash
./main
```

### Controle de câmera

* **W / S:** mover para cima/baixo no eixo Y
* **A / D:** mover para esquerda/direita no eixo X
* **Z / X:** mover para frente/trás no eixo Z
* **- / +:** zoom in/out
* **4 / 6:** mover a câmera esqueda/direita
* **8 / 2:** mover a câmera frente/trás
* **Mouse:** mover para cima/baixo no eixo Y e mover para a esquerda/direita no eixo X

## Ilustração

![Imagem Ampulheta](https://github.com/gebra04/Projeto_Computacao_Visual/blob/main/images/Screenshot%20from%202025-05-11%2012-48-41.png)
