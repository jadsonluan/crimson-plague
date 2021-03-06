#include <iostream>
#include "batalha.h"
#include "../util.h"
#include <string>
using namespace std;

int hpMonstro;
int danoMonstro;
int defesaMonstro;
bool fugiu;
Habilidade* habilidades;

void carregaHabilidades() {
    habilidades = carregarHabilidades();
}

void defineAtributosMonstro(Monstro &monstro) {
    hpMonstro = calcularHPMonstro(monstro);
    danoMonstro = monstro.danoAtaque;
    defesaMonstro = monstro.ca;
}

bool temManaParaAlgumaHabilidade(Ficha ficha, int qtdHabs) {
    for (int i = 0; i <= qtdHabs; i++) {
        if (temManaSuficiente(habilidades[i],ficha)) {
            return true;
        }
    }
    return false;
}

int dropMonstro(Monstro monstro) {
    if (monstro.id != 2 && monstro.id != 4) {
        if (rolarDado(20) < 10) {
            return -1;
        }
    }
    return monstro.drop;
}

int getDanoHabilidade(WINDOW* janelaMenu, Ficha &ficha) {
    int qtdHabs = qtdHabilidadesDisponiveis(ficha);
    int dano = -2;

    if (temManaParaAlgumaHabilidade(ficha, qtdHabs)) {
        string opcoesHabilidade[qtdHabs];

        for(int i = 0; i < qtdHabs; i++) {
            opcoesHabilidade[i] = habilidades[i].nome + " ["+to_string(habilidades[i].mp)+" mp]";
        }

        int opcao = realizaPergunta(janelaMenu, "Magias", opcoesHabilidade, qtdHabs);
        dano = usarHabilidade(habilidades[opcao], ficha);
    }

    return dano;
}

void recuperaManaPassiva(Ficha &ficha) {
    if (getMP(ficha) < getMaxMP(ficha))
        ficha.personagem.mp++;
}

string ataquePersonagem(WINDOW* janelaMenu,Ficha &ficha) {
    int danoInfligido = 0;

    if (ficha.personagem.classe == Classe::MAGO) {
        danoInfligido = getDanoHabilidade(janelaMenu, ficha);

        if (rolarDado(D20) < RESULTADO_DEFESA_MAGIA) {
            danoInfligido /= 2;
        }

        recuperaManaPassiva(ficha);
    } else {
        danoInfligido = getDano(ficha);

        if (getDano(ficha) + rolarDado(D20) < defesaMonstro) {
            danoInfligido = 0;
        }
    }

    string resultadoDoAtaque = "";

    if (danoInfligido > 0) {
        hpMonstro = max(hpMonstro - danoInfligido, 0);
        resultadoDoAtaque = "Você infligiu um total de " + to_string(danoInfligido) + " danos no monstro.";
    } else {
        if (danoInfligido == 0) {
            resultadoDoAtaque = "Você errou o ataque...";
        } else {
            resultadoDoAtaque = "Você não tem mana para habilidade alguma...";
        }
        resultadoDoAtaque += " O monstro ri de você...";
    }

    return resultadoDoAtaque;
}

string ataqueMonstro(Ficha &ficha) {
    int defesaPersonagem = 10 + getArmadura(ficha.inventario) + getModificadorDES(ficha);
    defesaPersonagem = min(defesaPersonagem, 17);
    int danoInfligido = (danoMonstro + rolarDado(20) > defesaPersonagem) ? danoMonstro : 3;
    int novoHP = getHP(ficha);
    novoHP -= danoInfligido;
    ficha.personagem.hp = max(novoHP, 0);

    return "O monstro infligiu um total de " + to_string(danoInfligido) + " danos a você.";
}

void abrirMochila(Ficha &ficha, WINDOW *janelaMenu) {
    int numElementos = qtdItens(ficha.inventario);
    string *opcoes = new string[numElementos + 1];

    string nome;
    int quantidade;
    int j = 0;
    for(int i = 0; i < ficha.inventario.tamInvent; i++) {
        if (ficha.inventario.mochila[i].id != 34) {
            nome = ficha.inventario.mochila[i].nome;
            quantidade = ficha.inventario.quantidade[i];
            string label(nome + " (" + to_string(quantidade) + "x)");
            opcoes[j++] = label;
        }
    }

    opcoes[numElementos] = "Voltar";

    int opcao = realizaPergunta(janelaMenu, "Selecione um item", opcoes, numElementos + 1);

    if (opcao < numElementos) {
        Item itemSelecionado = ficha.inventario.mochila[opcao];
        bool confirmou = confirmacao(janelaMenu);
        if (confirmou) {
            if (itemSelecionado.consumivel)
                usarItemConsumivel(opcao, ficha);
            else
                equiparItem(opcao, ficha.inventario);
        }
    }
}

int menuCombate(WINDOW* janelaMenu, Ficha &ficha) {
    string opcaoATK = "";
    if (ficha.personagem.classe == Classe::MAGO) {
        opcaoATK = "Lançar magia?";
    } else {
        opcaoATK = "Atacar? ";
    }
    string opcoesMenu[3] = {opcaoATK, "Abrir mochila?", "Tentar fugir?"};
    int op = realizaPergunta(janelaMenu, "O que você faz?", opcoesMenu, 3);

    return op + 1;
}

bool venceu() {
    return hpMonstro == ZERO_HP;
}

bool fugiuDaBatalha() {
    return fugiu;
}

bool personagemFugiu() {
    int rolagem = rolarDado(20);
    return rolagem >= 15;
}

string tentaFugir(WINDOW* janelaMenu, bool fugiu) {
    string str =  "Você tenta fugir e... ";

    (fugiu) ? str += "Escapou..." : str+= "Não consegue... O monstro está rindo de você...";

    return str;
}

void iniciaBatalha(WINDOW* janelaMenu, WINDOW* janelaDialogo, Ficha &ficha, Monstro monstro) {
    carregaHabilidades();
    defineAtributosMonstro(monstro);

    string falaIntroducaoBatalha[4] = {
        "Um desafio se aproxima, um " + monstro.nome + " te ataca...",
        "Seu HP: " + to_string(getHP(ficha)),
        "Seu MP: " + to_string(getMP(ficha)),
        "HP monstro: " + to_string(hpMonstro)
    };
    proximoDialogo(janelaDialogo, "BATALHA", falaIntroducaoBatalha, 4);

    bool batalhaFinalizada = false;
    fugiu = false;

    while (!batalhaFinalizada and !fugiu) {
        if (ficha.personagem.classe == Classe::MAGO) {
            int mp = getMP(ficha);
            mp += 1;
            ficha.personagem.mp = min(mp, getMaxMP(ficha));
        }
        int op = menuCombate(janelaMenu, ficha);

        string resultadoDaOpcao = "";
        if (op == OpcoesBatalha::ATACAR) {
            resultadoDaOpcao = ataquePersonagem(janelaMenu, ficha);
        } else if (op == OpcoesBatalha::ABRIR) {
            abrirMochila(ficha, janelaMenu);
        } else if (op == OpcoesBatalha::FUGIR) {
            fugiu = personagemFugiu();
            resultadoDaOpcao = tentaFugir(janelaMenu, fugiu);
        }

        if (!fugiu) {
            string atkMonstro = ataqueMonstro(ficha);
            string strHP = "Seu HP: " + to_string(getHP(ficha));
            string strMP =  "Seu MP: " + to_string(getMP(ficha));
            string strHPMonstro = "HP monstro: " + to_string(hpMonstro);
            string resultadoDoTurno[5] = {resultadoDaOpcao, atkMonstro, strHP, strMP, strHPMonstro};

            mostraDialogo(janelaDialogo, "Resultado do turno", resultadoDoTurno, 5);
            batalhaFinalizada = (hpMonstro == ZERO_HP or getHP(ficha) == ZERO_HP);
        } else {
            string arrayConseguiuFugir[1] = {resultadoDaOpcao};
            mostraDialogo(janelaDialogo, "Resultado do turno", arrayConseguiuFugir, 1);
        }
    }

    if (venceu()) {
        string parabens = "Parabéns pela vitória!";
        int drop = dropMonstro(monstro);
        if (drop != -1) {
            adicionarItem(drop, ficha.inventario);
            Item item = getItemPeloId(drop, ficha.inventario);
            string dropMonster = "O monstro deixou cair " + item.nome;
            string drops[] = {parabens, dropMonster};

            proximoDialogo(janelaDialogo, "VITORIA", drops, 2);

            string opcao[2] = {"Sim", "Nao"};
            int infoDrop = realizaPergunta(janelaMenu, "Obter informações sobre o drop?",
                opcao, 2);

            if (infoDrop == 0) {
                string inforDrop[2] = {
                    "Descrição do item " + item.nome + ", dropado pelo " + monstro.nome + ":",
                    item.descricao
                };
                proximoDialogo(janelaDialogo, "VITORIA - INFORMACAO DO DROP", inforDrop, 2);
            }
        } else {
            string arrayParabens[1] = {parabens};
            proximoDialogo(janelaDialogo, "VITORIA", arrayParabens, 1);
        }
    } else if (!fugiu){
        string msgDerrota = "";
        if (monstro.id == 7) {
            msgDerrota = "VOCE MORREU...";
            string derrota[1] = {msgDerrota};
            ficha.personagem.hp = 1;
            proximoDialogo(janelaDialogo, "DERROTA", derrota, 1);
        } else {
            msgDerrota = "Você perde a batalha contra o " + monstro.nome + ", desmaia e fica com 1 de HP...";
            string derrota[2] = {msgDerrota, "Tenha mais cuidado!"};
            ficha.personagem.hp = 1;
            proximoDialogo(janelaDialogo, "DERROTA", derrota, 2);
        }

        
    }
}
