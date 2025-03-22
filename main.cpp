#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>
#include <cstring>

using namespace std;

struct Produto {
    int codigo;
    char descricao[100];
    float preco;
    int quantidade;
};

struct ItemVenda {
    int codigoProduto;
    char descricao[100];
    float precoUnitario;
    int quantidade;
    float subtotal;
};

struct Venda {
    int numeroVenda;
    char data[11];
    int codigoCliente;
    vector<ItemVenda> itens;
    float totalVenda;
};

void obterDataAtual(char* data) {
    time_t now = time(0);
    tm* ltm = localtime(&now);

    sprintf(data, "%02d/%02d/%04d", ltm->tm_mday, 1 + ltm->tm_mon, 1900 + ltm->tm_year);
}

bool buscarProduto(int codigo, Produto* produto) {
    ifstream arquivo("produtos.txt");

    if (!arquivo) {
        cout << "Erro ao abrir arquivo de produtos!" << endl;
        return false;
    }

    bool encontrado = false;
    string linha;

    while (arquivo.good()) {
        int cod;
        char desc[100];
        float pr;
        int qtd;

        getline(arquivo, linha);
        if (linha.empty() || arquivo.eof()) break;
        cod = stoi(linha);

        getline(arquivo, linha);
        if (arquivo.eof()) break;
        strcpy(desc, linha.c_str());

        getline(arquivo, linha);
        if (arquivo.eof()) break;
        pr = stof(linha);

        getline(arquivo, linha);
        if (arquivo.eof()) break;
        qtd = stoi(linha);

        if (cod == codigo) {
            produto->codigo = cod;
            strcpy(produto->descricao, desc);
            produto->preco = pr;
            produto->quantidade = qtd;
            encontrado = true;
            break;
        }
    }

    arquivo.close();
    return encontrado;
}

bool verificarEstoque(int codigo, int quantidade) {
    Produto* produto = new Produto;
    bool disponivel = false;

    if (buscarProduto(codigo, produto)) {
        disponivel = (produto->quantidade >= quantidade);
    }

    delete produto;
    return disponivel;
}

bool verificarCliente(int codigo) {
    ifstream arquivo("clientes.txt");

    if (!arquivo) {
        cout << "Erro ao abrir arquivo de clientes!" << endl;
        return false;
    }

    string linha;
    bool encontrado = false;

    while (arquivo.good()) {
        int cod;

        getline(arquivo, linha);
        if (linha.empty() || arquivo.eof()) break;
        cod = stoi(linha);

        getline(arquivo, linha);
        if (arquivo.eof()) break;

        getline(arquivo, linha);
        if (arquivo.eof()) break;

        getline(arquivo, linha);
        if (arquivo.eof()) break;

        if (cod == codigo) {
            encontrado = true;
            break;
        }
    }

    arquivo.close();
    return encontrado;
}

void removerItem(vector<ItemVenda>* itens, float* totalVenda) {
    int indice;

    cout << "\nItens da venda:" << endl;
    for (size_t i = 0; i < itens->size(); i++) {
        cout << i + 1 << ". " << (*itens)[i].descricao
             << " - Qtd: " << (*itens)[i].quantidade
             << " - Subtotal: R$ " << fixed << setprecision(2) << (*itens)[i].subtotal << endl;
    }

    cout << "\nDigite o numero do item a remover (0 para cancelar): ";
    cin >> indice;

    if (indice > 0 && indice <= static_cast<int>(itens->size())) {
        *totalVenda -= (*itens)[indice - 1].subtotal;

        itens->erase(itens->begin() + indice - 1);
        cout << "Item removido com sucesso!" << endl;
    } else if (indice != 0) {
        cout << "Indice invalido!" << endl;
    }
}

void salvarVenda(Venda* venda) {
    ofstream arquivoVendas("vendas.txt");

    if (!arquivoVendas) {
        cout << "Erro ao abrir arquivo de vendas!" << endl;
        return;
    }

    arquivoVendas << venda->numeroVenda << endl
                  << venda->data << endl
                  << venda->codigoCliente << endl
                  << venda->itens.size() << endl
                  << fixed << setprecision(2) << venda->totalVenda << endl;

    for (size_t i = 0; i < venda->itens.size(); i++) {
        arquivoVendas << venda->itens[i].codigoProduto << endl
                      << venda->itens[i].descricao << endl
                      << fixed << setprecision(2) << venda->itens[i].precoUnitario << endl
                      << venda->itens[i].quantidade << endl
                      << fixed << setprecision(2) << venda->itens[i].subtotal << endl;
    }

    arquivoVendas.close();

    ofstream arquivoEstoque("estoque_venda.txt");

    if (arquivoEstoque) {
        for (size_t i = 0; i < venda->itens.size(); i++) {
            arquivoEstoque << venda->itens[i].codigoProduto << endl
                          << venda->itens[i].quantidade << endl;
        }

        arquivoEstoque.close();
        cout << "Arquivo para equipe de estoque atualizado: estoque_venda.txt" << endl;
    }

    ofstream saida("contador_vendas.txt");
    saida << venda->numeroVenda + 1;
    saida.close();
}

int obterProximoNumeroVenda() {
    ifstream arquivo("contador_vendas.txt");
    int contador = 1;

    if (arquivo) {
        arquivo >> contador;
        arquivo.close();
    }

    return contador;
}

void realizarVenda() {
    Venda* venda = new Venda;
    ItemVenda item;
    Produto* produto = new Produto;
    int codigo, quantidade, opcao;

    venda->numeroVenda = obterProximoNumeroVenda();
    obterDataAtual(venda->data);
    venda->totalVenda = 0.0;

    cout << "\n==== NOVA VENDA ====" << endl;
    cout << "Numero da Venda: " << venda->numeroVenda << endl;
    cout << "Data: " << venda->data << endl;

    cout << "\nDeseja vincular esta venda a um cliente? (1-Sim / 0-Nao): ";
    cin >> opcao;

    if (opcao == 1) {
        cout << "Digite o codigo do cliente: ";
        cin >> venda->codigoCliente;

        if (!verificarCliente(venda->codigoCliente)) {
            cout << "Cliente nao encontrado! Venda sera realizada sem vinculacao." << endl;
            venda->codigoCliente = 0;
        }
    } else {
        venda->codigoCliente = 0;
    }

    bool vendaAtiva = true;
    bool vendaFinalizada = false;

    while (vendaAtiva) {
        cout << "\n--- Menu de Venda ---" << endl;
        cout << "1. Buscar e adicionar produto" << endl;
        cout << "2. Remover item" << endl;
        cout << "3. Exibir itens atuais" << endl;
        cout << "4. Finalizar venda" << endl;
        cout << "5. Cancelar venda" << endl;
        cout << "Escolha uma opcao: ";
        cin >> opcao;

        switch (opcao) {
            case 1:
                cout << "\nCodigo do Produto: ";
                cin >> codigo;

                if (buscarProduto(codigo, produto)) {
                    cout << "Descricao: " << produto->descricao << endl;
                    cout << "Preco: R$ " << fixed << setprecision(2) << produto->preco << endl;
                    cout << "Quantidade disponivel: " << produto->quantidade << endl;

                    cout << "Quantidade a comprar: ";
                    cin >> quantidade;

                    if (quantidade <= 0) {
                        cout << "Quantidade invalida!" << endl;
                        continue;
                    }

                    if (!verificarEstoque(codigo, quantidade)) {
                        cout << "Estoque insuficiente!" << endl;
                        continue;
                    }

                    item.codigoProduto = produto->codigo;
                    strcpy(item.descricao, produto->descricao);
                    item.precoUnitario = produto->preco;
                    item.quantidade = quantidade;
                    item.subtotal = item.precoUnitario * item.quantidade;

                    venda->itens.push_back(item);
                    venda->totalVenda += item.subtotal;

                    cout << "Produto adicionado a venda!" << endl;
                    cout << "Total atual: R$ " << fixed << setprecision(2) << venda->totalVenda << endl;
                } else {
                    cout << "Produto nao encontrado!" << endl;
                }
                break;

            case 2:
                if (venda->itens.empty()) {
                    cout << "Nao ha itens na venda atual!" << endl;
                } else {
                    removerItem(&(venda->itens), &(venda->totalVenda));
                }
                break;

            case 3:
                if (venda->itens.empty()) {
                    cout << "Nao ha itens na venda atual!" << endl;
                } else {
                    cout << "\n==== ITENS DA VENDA ====" << endl;
                    cout << setw(10) << "Codigo" << setw(30) << "Descricao" << setw(15) << "Preco Unit."
                         << setw(10) << "Qtd" << setw(15) << "Subtotal" << endl;

                    for (size_t i = 0; i < venda->itens.size(); i++) {
                        cout << setw(10) << venda->itens[i].codigoProduto
                             << setw(30) << venda->itens[i].descricao
                             << setw(15) << fixed << setprecision(2) << venda->itens[i].precoUnitario
                             << setw(10) << venda->itens[i].quantidade
                             << setw(15) << fixed << setprecision(2) << venda->itens[i].subtotal << endl;
                    }

                    cout << "\nTotal da Venda: R$ " << fixed << setprecision(2) << venda->totalVenda << endl;
                }
                break;

            case 4:
                if (venda->itens.empty()) {
                    cout << "Nao e possivel finalizar uma venda sem itens!" << endl;
                } else {
                    cout << "\n==== RESUMO DA VENDA ====" << endl;
                    cout << "Numero: " << venda->numeroVenda << " | Data: " << venda->data << endl;

                    if (venda->codigoCliente > 0) {
                        cout << "Codigo do Cliente: " << venda->codigoCliente << endl;
                    }

                    cout << "\nItens:" << endl;
                    cout << setw(10) << "Codigo" << setw(30) << "Descricao" << setw(15) << "Preco Unit."
                         << setw(10) << "Qtd" << setw(15) << "Subtotal" << endl;

                    for (size_t i = 0; i < venda->itens.size(); i++) {
                        cout << setw(10) << venda->itens[i].codigoProduto
                             << setw(30) << venda->itens[i].descricao
                             << setw(15) << fixed << setprecision(2) << venda->itens[i].precoUnitario
                             << setw(10) << venda->itens[i].quantidade
                             << setw(15) << fixed << setprecision(2) << venda->itens[i].subtotal << endl;
                    }

                    cout << "\nTotal da Venda: R$ " << fixed << setprecision(2) << venda->totalVenda << endl;

                    cout << "\nConfirmar venda? (1-Sim / 0-Nao): ";
                    cin >> opcao;

                    if (opcao == 1) {
                        salvarVenda(venda);
                        cout << "Venda finalizada com sucesso!" << endl;
                        vendaFinalizada = true;
                        vendaAtiva = false;
                    }
                }
                break;

            case 5:
                cout << "Tem certeza que deseja cancelar a venda? (1-Sim / 0-Nao): ";
                cin >> opcao;

                if (opcao == 1) {
                    cout << "Venda cancelada!" << endl;
                    vendaAtiva = false;
                }
                break;

            default:
                cout << "Opcao invalida!" << endl;
        }
    }

    delete produto;
    delete venda;
}

int main() {
    int opcao;

    do {
        cout << "\n===== MODULO DE VENDAS =====" << endl;
        cout << "1. Nova Venda" << endl;
        cout << "0. Sair" << endl;
        cout << "Escolha uma opcao: ";
        cin >> opcao;

        switch (opcao) {
            case 1:
                realizarVenda();
                break;
            case 0:
                cout << "Saindo do modulo de vendas..." << endl;
                break;
            default:
                cout << "Opcao invalida!" << endl;
        }
    } while (opcao != 0);

    return 0;
}
