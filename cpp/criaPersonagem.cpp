#include <iostream>
#include "criaPersonagem.h"

using namespace std;

void informacoesClasses() {
  cout << "---INFORMAÇÕES---" << endl;
  cout << "(1) - Guerreiro:" << endl;
  cout << "Guerreiros são exímios lutadores marciais, sempre prontos para lutar. ";
  cout << "Possuem extremas habilidades de combate corpo a corpo." << endl << endl;

  cout << "(2) - Mago:" << endl;
  cout << "Magos são pesquisadores das artes arcanas da Idade Média e utilizam-as ";
  cout << "em suas batalhas, porém são fisicamente fracos e não possuem muitas habilidades ";
  cout << "de combate com armas." << endl << endl;

  cout << "(3) - Ladino:" << endl;
  cout << "Bem furtivos e escorregadios, ladinos, se não estão roubando algo, ";
  cout << "estão trabalhando numa busca por um tesouro. Possuem altas habilidades ";
  cout << "em combates de longa distância e armas de longo alcance." << endl << endl;
}

void ajustaAtributos(Ficha &ficha) {
  Classe classePersonagem = ficha.personagem.classe;

  switch (classePersonagem) {
    case Guerreiro:
          ficha.atributos.forca += 2;
          ficha.atributos.vitalidade += 1;
          ficha.atributos.destreza -= 1;
          ficha.atributos.inteligencia -= 2;
          break;
    case Mago:
          ficha.atributos.inteligencia += 2;
          ficha.atributos.sorte += 1;
          ficha.atributos.destreza -= 1;
          ficha.atributos.forca -= 2;
          break;
    case Ladino:
          ficha.atributos.destreza += 2;
          ficha.atributos.inteligencia += 1;
          ficha.atributos.carisma -= 1;
          ficha.atributos.vitalidade -= 2;
          break;
  }
}

string defineNomePersonagem() {
  string nomeJogador;

  char opcao = 'n';

  while (tolower(opcao) != 's') {
    cout << "Digite um nome para seu personagem: ";
    cin >> nomeJogador;
    cout << "Tem certeza? (S/N): ";
    cin >> opcao;
  }

  return nomeJogador;
}

Classe defineClassePersonagem() {
  char opcao = 'n';
  int classe;

  while (tolower(opcao) != 's') {
    classe = -1;
    cout << "Existem três tipos de classe:" << endl;
    while (classe < 1 or classe > 3) {
      cout << "(1) Guerreiro - (2) Mago - (3) Ladino - (0) Informações" << endl;
      cout << "Escolha sua classe: ";
      cin >> classe;

      if (classe < 0 or classe > 3) {
        cout << "Opção inválida!" << endl;
      }

      if (classe == 0) {
        informacoesClasses();
      }
    }

    cout << "Tem certeza? (S/N): ";
    cin >> opcao;
  }

  return (Classe) classe;
}

void distribuicaoPontos(Ficha &ficha) {
  int pontos = 8;
  int opcao = 'n';

  cout << "Você pode distribuir " << pontos << " pontos pelos seus atributos:" << endl;

  // TODO distribuição de pontos

}

Ficha criarPersonagem() {
  Ficha ficha;
  Personagem personagem;

  personagem.nomePersonagem = defineNomePersonagem();
  personagem.classe = defineClassePersonagem();

  ficha.personagem = personagem;

  ajustaAtributos(ficha);
  distribuicaoPontos(ficha);

  return ficha;
}
