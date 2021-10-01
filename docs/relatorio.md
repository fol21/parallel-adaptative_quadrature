<style type="text/css">
    .tg  {border-collapse:collapse;border-spacing:0;}
    .tg td{border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;
    overflow:hidden;padding:10px 5px;word-break:normal;}
    .tg th{border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;
    font-weight:normal;overflow:hidden;padding:10px 5px;word-break:normal;}
    .tg .tg-0pky{border-color:inherit;text-align:left;vertical-align:top}
</style>


# Broadcast entre threads usando um buffer compartilhado
Descrição dos testes realizados, problemas identificados e não resolvidos. 

*por **Fernando Lima** (**2020877**)*


Nesse trabalho você deve implementar duas variantes do algoritmo da quadratura adaptativa (ver seção 8.2 desse artigo de Greg Andrews e livro do Peter Pacheco), utilizando OpenMP. Na primeira variante, faça também uma implementação usando pthreads, para comparar os resultados de tempos de execução.

O programa utiliza X threads para computar a aproximação, pelo método de trapézios, da área abaixo da curva formada por uma função f. O número de trapézios a ser calculado para realizar a aproximação depende de uma tolerância. Para determinado intervalo [a,b], o algoritmo calcula a área do trapézio com extremidades (a, b, f(a), f(b)), e a área dos dois subtrapézios formados por (a, (b-a)/2, f(a), f((b-a)/2)) e por ((b-a)/2, b, f((b-a)/2), f(b)). Se a diferença entre a área do trapézio maior e a soma dos dois subtrapézios fica abaixo de uma tolerância, o cálculo da área entre a e b está completo. Caso contrário, repet-se o mesmo procedimento para os trapézios menores. A tolerância na diferença entre um trapézio e os dois subtrapézios "seguintes" deve ser uma porcentagem da soma de áreas dos dois subtrapézios. 

O programa deve aceitar facilmente a mudança para outras funções e intervalos, parametrizando o cálculo da integral pelos extremos e por um ponteiro de função.

Execute cada variante para diferentes números de threads (1, 2 e 4), com algumas medidas preliminares dos tempos obtidos para cada combinação. Experimente com diferentes tolerâncias e com diferentes funções. Procure funções (pode inventar loops e muitas operações com doubles) que criem uma carga de processamento relevante. Uma fonte de inspiração para as funções a serem usadas é o artigo de Bailey e Borwein. Elabore um relatório com seus resultados, explicitando também a arquitetura em que executou os testes. (Observe que só vale a pena realizar testes com 4 threads em uma arquitetura com quatro ou mais núcleos.)

## Primeira Variante

Na primeira implementação, cada thread calcula um subintervalo pelo qual será responsável e calcula o resultado para esse subintervalo inteiro. Quando todas as threads terminarem, a thread principal deve mostrar o resultado final. Essa variante deve ser implementada com pthreads e com OpenMP.

## Segunda Variante

Na segunda variante, a thread principal inicialmente cria uma lista de tarefas, contendo os extremos dos intervalos, com NUMINICIAL tarefas. Cada thread executa uma tarefa, e se ela gerar subtarefas, coloca uma delas na fila global e processa a outra, até que não encontre mais tarefas na fila (Escreva operaçoes InsereTarefa e RetiraTarefa para manipular essa fila.). A thread principal espera as demais terminarem e mostra o resultado final. Essa variante deve ser implementada apenas com OpenMP.

## Resultados


<table class="tg">
<thead>
  <tr>
    <th class="tg-0pky">Intro</th>
    <th class="tg-0pky"></th>
    <th class="tg-0pky"></th>
    <th class="tg-0pky"></th>
    <th class="tg-0pky"></th>
  </tr>
</thead>
