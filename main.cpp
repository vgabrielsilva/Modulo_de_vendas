#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>

using namespace std;

struct Produto {
    int codigo;
    string descricao;
    float preco;
    int quantidade;
};

struct ItemVenda {
    int codigoProduto;
    string descricao;
    float precoUnitario;
    int quantidade;
    float subtotal;
};

struct Venda {
    int numeroVenda;
    string data;
    int codigoCliente;
    vector<ItemVenda> itens;
    float totalVenda;
};

string obterDataAtual() {
    time_t agora = time(0);
    tm* tempoLocal = localtime(&agora);

    string dia = to_string(tempoLocal->tm_mday);
    if (dia.length() == 1) dia = "0" + dia;

    string mes = to_string(1 + tempoLocal->tm_mon);
    if (mes.length() == 1) mes = "0" + mes;

    string ano = to_string(1900 + tempoLocal->tm_year);

    return dia + "/" + mes + "/" + ano;
}

bool buscarProduto(int codigo, Produto* produto) {
    ifstream arquivo("produtos.txt");

    if (!arquivo.is_open()) {
        cout << "Erro ao abrir arquivo de produtos!" << endl;
        return false;
    }

    bool encontrado = false;
    string linha;

    while (getline(arquivo, linha)) {
        if (linha.empty()) continue;

        int codigoProduto;
        try {
            codigoProduto = stoi(linha);
        } catch (...) {
            continue;
        }

        string descricaoProduto;
        if (!getline(arquivo, descricaoProduto)) break;

        string linhaPreco;
        if (!getline(arquivo, linhaPreco)) break;
        float precoProduto;
        try {
            precoProduto = stof(linhaPreco);
        } catch (...) {
            continue;
        }

        string linhaQtd;
        if (!getline(arquivo, linhaQtd)) break;
        int quantidadeProduto;
        try {
            quantidadeProduto = stoi(linhaQtd);
        } catch (...) {
            continue;
        }

        if (codigoProduto == codigo) {
            produto->codigo = codigoProduto;
            produto->descricao = descricaoProduto;
            produto->preco = precoProduto;
            produto->quantidade = quantidadeProduto;
            encontrado = true;
            break;
        }
    }

    arquivo.close();
    return encontrado;
}

bool verificarEstoque(int codigo, int quantidade) {
    Produto produto;
    Produto* produtoPtr = &produto;

    if (buscarProduto(codigo, produtoPtr)) {
        return (produtoPtr->quantidade >= quantidade);
    }

    return false;
}

bool verificarCliente(int codigo) {
    ifstream arquivo("clientes.txt");

    if (!arquivo.is_open()) {
        cout << "Erro ao abrir arquivo de clientes!" << endl;
        return false;
    }

    string linha;
    bool encontrado = false;

    while (getline(arquivo, linha)) {
        if (linha.empty()) continue;

        int codigoCliente;
        try {
            codigoCliente = stoi(linha);
        } catch (...) {
            continue;
        }

        for (int i = 0; i < 3; i++) {
            if (!getline(arquivo, linha)) break;
        }

        if (codigoCliente == codigo) {
            encontrado = true;
            break;
        }
    }

    arquivo.close();
    return encontrado;
}

void removerItem(vector<ItemVenda>* itens, float* totalVenda) {
    if (itens->empty()) {
        cout << "Nao ha itens para remover!" << endl;
        return;
    }

    cout << "\nItens da venda:" << endl;
    for (int i = 0; i < itens->size(); i++) {
        cout << (i + 1) << ". " << (*itens)[i].descricao
             << " - Qtd: " << (*itens)[i].quantidade
             << " - Subtotal: R$ " << fixed << setprecision(2) << (*itens)[i].subtotal << endl;
    }

    int indice;
    cout << "\nDigite o numero do item a remover (0 para cancelar): ";
    cin >> indice;

    if (indice > 0 && indice <= itens->size()) {
        *totalVenda -= (*itens)[indice - 1].subtotal;
        itens->erase(itens->begin() + (indice - 1));
        cout << "Item removido com sucesso!" << endl;
    } else if (indice != 0) {
        cout << "Indice invalido!" << endl;
    }
}

void salvarVenda(Venda* venda) {
    ofstream arquivoVendas("vendas.txt");

    if (!arquivoVendas.is_open()) {
        cout << "Erro ao abrir arquivo de vendas!" << endl;
        return;
    }

    arquivoVendas << venda->numeroVenda << endl;
    arquivoVendas << venda->data << endl;
    arquivoVendas << venda->codigoCliente << endl;
    arquivoVendas << venda->itens.size() << endl;
    arquivoVendas << fixed << setprecision(2) << venda->totalVenda << endl;

    for (int i = 0; i < venda->itens.size(); i++) {
        arquivoVendas << venda->itens[i].codigoProduto << endl;
        arquivoVendas << venda->itens[i].descricao << endl;
        arquivoVendas << fixed << setprecision(2) << venda->itens[i].precoUnitario << endl;
        arquivoVendas << venda->itens[i].quantidade << endl;
        arquivoVendas << fixed << setprecision(2) << venda->itens[i].subtotal << endl;
    }

    arquivoVendas.close();

    ofstream arquivoEstoque("estoque_venda.txt");

    if (arquivoEstoque.is_open()) {
        for (int i = 0; i < venda->itens.size(); i++) {
            arquivoEstoque << venda->itens[i].codigoProduto << endl;
            arquivoEstoque << venda->itens[i].quantidade << endl;
        }

        arquivoEstoque.close();
        cout << "Arquivo para equipe de estoque atualizado: estoque_venda.txt" << endl;
    }

    ofstream contadorVendas("contador_vendas.txt");
    contadorVendas << (venda->numeroVenda + 1);
    contadorVendas.close();
}

int obterProximoNumeroVenda() {
    ifstream arquivo("contador_vendas.txt");
    int contador = 1;

    if (arquivo.is_open()) {
        arquivo >> contador;
        arquivo.close();
    }

    return contador;
}

void realizarVenda() {
    Venda* venda = new Venda;

    venda->numeroVenda = obterProximoNumeroVenda();
    venda->data = obterDataAtual();
    venda->totalVenda = 0.0;

    cout << "\n==== NOVA VENDA ====" << endl;
    cout << "Numero da Venda: " << venda->numeroVenda << endl;
    cout << "Data: " << venda->data << endl;

    int opcao;
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
            case 1: {
                int codigo, quantidade;
                cout << "\nCodigo do Produto: ";
                cin >> codigo;

                Produto produto;
                Produto* produtoPtr = &produto;

                if (buscarProduto(codigo, produtoPtr)) {
                    cout << "Descricao: " << produtoPtr->descricao << endl;
                    cout << "Preco: R$ " << fixed << setprecision(2) << produtoPtr->preco << endl;
                    cout << "Quantidade disponivel: " << produtoPtr->quantidade << endl;

                    cout << "Quantidade a comprar: ";
                    cin >> quantidade;

                    if (quantidade <= 0) {
                        cout << "Quantidade invalida!" << endl;
                        break;
                    }

                    if (!verificarEstoque(codigo, quantidade)) {
                        cout << "Estoque insuficiente!" << endl;
                        break;
                    }

                    ItemVenda* item = new ItemVenda;
                    item->codigoProduto = produtoPtr->codigo;
                    item->descricao = produtoPtr->descricao;
                    item->precoUnitario = produtoPtr->preco;
                    item->quantidade = quantidade;
                    item->subtotal = item->precoUnitario * item->quantidade;

                    venda->itens.push_back(*item);
                    venda->totalVenda += item->subtotal;

                    delete item;

                    cout << "Produto adicionado a venda!" << endl;
                    cout << "Total atual: R$ " << fixed << setprecision(2) << venda->totalVenda << endl;
                } else {
                    cout << "Produto nao encontrado!" << endl;
                }
                break;
            }

            case 2:
                if (venda->itens.empty()) {
                    cout << "Nao ha itens na venda atual!" << endl;
                } else {
                    removerItem(&venda->itens, &venda->totalVenda);
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
