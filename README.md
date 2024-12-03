# VISUALIZADOR DE CENAS 3D COM OPENGL MODERNA

Repositório do projeto desenvolvido utilizando OpenGL moderna (3.3+) criado para a Atividade Acadêmica de Processamento Gráfico: Computação Gráfica e Aplicações do curso Ciência da Computação (modalidade presencial) da Unisinos.

As dependências estão pré-compiladas para Windows 10 64-bits e compilador do Visual Studio 2019/2022 e MinGW-64
Se o seu ambiente de desenvolvimento for diferente, você deve baixar:

- GLFW: https://www.glfw.org/
- GLAD: https://glad.dav1d.de/ - para mais instruções, consulte: https://learnopengl.com/Getting-started/Creating-a-window

Para configurar os projetos em sua máquina utilizando o VSCode ou o Visual Studio (pode ser o Community), vá para os links:

- [Visual Studio Code](https://github.com/fellowsheep/CG2024-2/blob/main/CONFIG-VSCode.md)
- [Visual Studio](https://github.com/fellowsheep/CG2024-2/blob/main/CONFIG-VS2022%2B.md)

## INSTRUÇÕES DE CONTROLE
Abaixo estão as instruções de controle para interação com o visualizador de cenas 3D:

Seleção de Objetos:
- Pressione KEY_1 para selecionar o Objeto 1.
- Pressione KEY_2 para selecionar o Objeto 2.

Rotação do Objeto Selecionado:
- Pressione KEY_X para rotacionar o objeto selecionado no eixo X.
- Pressione KEY_Y para rotacionar o objeto selecionado no eixo Y.
- Pressione KEY_Z para rotacionar o objeto selecionado no eixo Z.

Translação do Objeto Selecionado:
- Pressione KEY_UP para mover o objeto selecionado para cima.
- Pressione KEY_DOWN para mover o objeto selecionado para baixo.
- Pressione KEY_RIGHT para mover o objeto selecionado para a direita.
- Pressione KEY_LEFT para mover o objeto selecionado para a esquerda.
- Pressione K para mover o objeto selecionado para frente (eixo Z positivo).
- Pressione L para mover o objeto selecionado para trás (eixo Z negativo).

Escala do Objeto Selecionado:
- Pressione KEY_+ para aumentar a escala do objeto selecionado.
- Pressione KEY_- para diminuir a escala do objeto selecionado.

Controle da Câmera:
- Mova o mouse para rotacionar a câmera ao redor da cena.
- Utilize o scroll do mouse para realizar o zoom na câmera, aproximando ou afastando a visualização da cena.
