CG-Proj 3

Autores:\
	Luisa Salles de Oliveira  RA 11201720136\
	Gustavo Duarte de Souza   RA 11012515\
  \
Objetivo:
\
&nbsp;&nbsp; Semelhante ao Proj 1, nosso objetivo era criar um jogo Endless Runner, onde o jogador deve desviar de obstáculos infinitamente.\
&nbsp;&nbsp;Algumas funções, como animação de "explosão", verificação de colisão e obstáculos-recompensa não foram implantadas.
  

Conteúdo:
\
&nbsp;&nbsp;&nbsp; O projeto foi criado tomando como base os exemplos [Starfield](http://professor.ufabc.edu.br/~harlen.batagelo/cg/starfield.html "Starfield") e [Viewer5](http://professor.ufabc.edu.br/~harlen.batagelo/cg/normalmapping.html "Viewer5") do livro da disciplina.

  
&nbsp;&nbsp;Viewer5:
\
&nbsp;&nbsp;&nbsp;&nbsp;Foram modificadas as funções de tratamento de eventos, adicionando comandos para troca de textura do avião (T = camuflagem, N = normal mapping), controle de velocidade do obstáculo (L-Shift = Acelerar, Espaço = Desacelerar) e controle do avião. \
&nbsp;&nbsp;&nbsp;&nbsp; Funcionalidades de UI e trackball foram removidas. \
&nbsp;&nbsp;&nbsp;&nbsp; Modificados InicializeGL, PaintGL e loadmodel para tratar todos os objetos. 
  
&nbsp;&nbsp;Stars:
\
&nbsp;&nbsp;&nbsp;&nbsp; Adaptação do conteúdo de Starfields: Programs, view Matrix e proj Matrix são compartilhadas com outros objetos, sendo gerados em OpenGLWindow e enviados como parâmetro. Tanto neste caso como nos shaders de cada objeto, a múltiplicação de matrizes apresentou o maior desafio, pois os shaders não permitem debug da mesma forma que o resto do código e a visualização do produto final é muito mais complexa.

&nbsp;&nbsp;Shaders:
\
&nbsp;&nbsp;&nbsp;&nbsp; Foram modificados afim de orientar todos os objetos da cena e movimentá-los corretamente. O avião possui dois shaders, um para cada textura selecionada pelo controle do jogador. Ambos possuem mesmos controles para movimentação, permitindo 4 direções e inclinando o objeto quando movimentado horizontalmente.
  
Apesar do projeto não ser um jogo completo, acreditamos que ele demonstre o conteúdo da parte final da disciplina, utilizando shaders, VBOs, EBOs, VAOs, Modelos .obj, texturização a partir de imagens e vetores normais, além de outras funções da biblioteca ABCg. 

  
