
# parallel-adaptative_quadrature
Implements two variants of the adaptive quadrature algorithm, using OpenMP. In the first variant is also implemented using pthreads, to compare runtime results

## Pré-Requisitos
Certifique-se que os programas *make* e *gcc* estão instalados no ambiente linux, caso não instale com os comandos:

Ubuntu, Debian:
````bash
sudo apt-get install make gcc gdb
````

CentOS, Fedora:
````bash
yum install make gcc gdb
````

ou equivalentes.

## Executando o Programa

Basta executar um dos comandos
```bash
make tarefa2-1 && clear && ./bin/tarefa2-1 0 10 0.000000001 4
make tarefa2-2 && clear && ./bin/tarefa2-1 0 10 0.001 4
```

## Compilando o Programa

*** Antes de começar, verifique se make e gcc estão instalados ***

Execute o comando
```bash
make tarefa2-1
make tarefa2-2
```

## Testes

Descrição dos testes realizados, problemas identificados e não resolvidos. 

[Relatorio dos Testes](./docs/relatorio.md)

