#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>

using namespace std;

#define NOME_TAM 48
#define NOME_PRODUTO_TAM 32
#define TIPO_TAM 16

struct Produto {
    int id;
    int Quantidade;
    char dia[3];
    char mes[3];
    char ano[5];
    float precoTotal;
    float preco;
    bool disponivel;
    char categoria[TIPO_TAM+1];
    char TipoUnidade[TIPO_TAM+1];
    char nome[NOME_PRODUTO_TAM+1];
};

struct TabelaProduto {
    int qtd;
    Produto *dados;
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

void carregarTabelaCadsProduto(TabelaProduto &tabela) {
    ifstream arquivo("Produtos_DB.dat", ios::binary);
    if (!arquivo) {
        cerr << "Erro ao abrir o arquivo para leitura! Criando nova tabela vazia..." << endl;
        tabela.qtd = 0;
        tabela.dados = nullptr;
        return;
    }

    arquivo.read(reinterpret_cast<char *>(&tabela.qtd), sizeof(int));

    if (tabela.qtd <= 0 || tabela.qtd > 1000) {
        cerr << "Quantidade de produtos invalida: " << tabela.qtd << ". Criando tabela vazia..." << endl;
        tabela.qtd = 0;
        tabela.dados = nullptr;
        arquivo.close();
        return;
    }

    delete[] tabela.dados;

    tabela.dados = new Produto[tabela.qtd];

    for (int i = 0; i < tabela.qtd; i++) {
        arquivo.read(reinterpret_cast<char *>(&tabela.dados[i]), sizeof(Produto));
        if (arquivo.fail()) {
            cerr << "Erro ao ler o produto " << i+1 << ". Leitura parcial realizada." << endl;
            tabela.qtd = i;
            break;
        }
    }

    arquivo.close();
    cout << "Produtos carregados com sucesso! Total: " << tabela.qtd << endl;

    cout << "IDs dos produtos carregados: ";
    for (int i = 0; i < tabela.qtd; i++) {
        cout << tabela.dados[i].id << " ";
    }
    cout << endl;
}

void listarProdutosDisponiveis(const TabelaProduto &tabela) {
    if (!tabela.dados || tabela.qtd <= 0) {
        cout << "Nenhum produto disponivel para exibicao!" << endl;
        return;
    }

    cout << "\n===== PRODUTOS DISPONIVEIS =====" << endl;
    cout << setw(6) << "ID" << setw(32) << "Nome" << setw(16) << "Categoria"
         << setw(12) << "Unidade" << setw(10) << "Preco" << setw(10) << "Estoque"
         << setw(10) << "Disponivel" << endl;
    cout << string(96, '-') << endl;

    int produtosDisponiveis = 0;
    int produtosIndisponiveis = 0;
    int produtosZeroEstoque = 0;

    for (int i = 0; i < tabela.qtd; i++) {
        cout << setw(6) << tabela.dados[i].id
             << setw(32) << tabela.dados[i].nome
             << setw(16) << tabela.dados[i].categoria
             << setw(12) << tabela.dados[i].TipoUnidade
             << setw(10) << fixed << setprecision(2) << tabela.dados[i].preco
             << setw(10) << tabela.dados[i].Quantidade
             << setw(10) << (tabela.dados[i].disponivel ? "Sim" : "Nao") << endl;

        if (!tabela.dados[i].disponivel) {
            produtosIndisponiveis++;
        } else if (tabela.dados[i].Quantidade <= 0) {
            produtosZeroEstoque++;
        } else {
            produtosDisponiveis++;
        }
    }

    cout << "\nTotal de produtos carregados: " << tabela.qtd << endl;
    cout << "Produtos disponiveis (ativos e com estoque): " << produtosDisponiveis << endl;
    cout << "Produtos indisponiveis (nao ativos): " << produtosIndisponiveis << endl;
    cout << "Produtos sem estoque (ativos mas estoque zero): " << produtosZeroEstoque << endl;
}

void buscarProdutoPorNome(const TabelaProduto &tabela) {
    if (!tabela.dados || tabela.qtd <= 0) {
        cout << "Nenhum produto disponivel para busca!" << endl;
        return;
    }

    string termoBusca;
    cout << "Digite o nome ou parte do nome do produto: ";
    cin.ignore();
    getline(cin, termoBusca);

    for (char &c : termoBusca) {
        c = tolower(c);
    }

    cout << "\n===== RESULTADOS DA BUSCA =====" << endl;
    cout << setw(6) << "ID" << setw(32) << "Nome" << setw(16) << "Categoria"
         << setw(12) << "Unidade" << setw(10) << "Preco" << setw(10) << "Estoque" << endl;
    cout << string(86, '-') << endl;

    int produtosEncontrados = 0;

    for (int i = 0; i < tabela.qtd; i++) {
        if (tabela.dados[i].disponivel) {
            string nomeProduto = tabela.dados[i].nome;
            for (char &c : nomeProduto) {
                c = tolower(c);
            }

            if (nomeProduto.find(termoBusca) != string::npos) {
                cout << setw(6) << tabela.dados[i].id
                     << setw(32) << tabela.dados[i].nome
                     << setw(16) << tabela.dados[i].categoria
                     << setw(12) << tabela.dados[i].TipoUnidade
                     << setw(10) << fixed << setprecision(2) << tabela.dados[i].preco
                     << setw(10) << tabela.dados[i].Quantidade << endl;
                produtosEncontrados++;
            }
        }
    }

    if (produtosEncontrados == 0) {
        cout << "Nenhum produto encontrado com o termo '" << termoBusca << "'." << endl;
    } else {
        cout << "\nTotal de produtos encontrados: " << produtosEncontrados << endl;
    }
}

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

bool buscarProduto(int codigo, Produto* produto, TabelaProduto &tabela) {
    if (!tabela.dados || tabela.qtd <= 0) {
        cout << "Tabela de produtos vazia ou nao carregada!" << endl;
        return false;
    }

    for (int i = 0; i < tabela.qtd; i++) {
        if (tabela.dados[i].id == codigo && tabela.dados[i].disponivel) {
            *produto = tabela.dados[i];
            return true;
        }
    }

    return false;
}

bool verificarEstoque(int codigo, int quantidade, TabelaProduto &tabela) {
    Produto produto;

    if (buscarProduto(codigo, &produto, tabela)) {
        return (produto.Quantidade >= quantidade);
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

void atualizarEstoqueProdutos(vector<ItemVenda>& itens, TabelaProduto &tabela) {
    bool alteracoes = false;

    for (const ItemVenda& item : itens) {
        for (int i = 0; i < tabela.qtd; i++) {
            if (tabela.dados[i].id == item.codigoProduto) {
                tabela.dados[i].Quantidade -= item.quantidade;
                alteracoes = true;
                break;
            }
        }
    }

    if (alteracoes) {
        ofstream arquivo("Produtos_DB.dat", ios::binary | ios::trunc);
        if (arquivo.is_open()) {
            arquivo.write(reinterpret_cast<char*>(&tabela.qtd), sizeof(int));
            arquivo.write(reinterpret_cast<char*>(tabela.dados), tabela.qtd * sizeof(Produto));
            arquivo.close();
            cout << "Estoque atualizado com sucesso!" << endl;
        } else {
            cout << "ERRO: Falha ao atualizar estoque no arquivo binario!" << endl;
        }
    }
}

void salvarVenda(Venda* venda, TabelaProduto &tabela) {
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

    atualizarEstoqueProdutos(venda->itens, tabela);

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

void realizarVenda(TabelaProduto &tabela) {
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
        cout << "4. Listar produtos disponiveis" << endl;
        cout << "5. Buscar produto por nome" << endl;
        cout << "6. Finalizar venda" << endl;
        cout << "7. Cancelar venda" << endl;
        cout << "Escolha uma opcao: ";
        cin >> opcao;

        switch (opcao) {
            case 1: {
                int codigo, quantidade;
                cout << "\nCodigo do Produto: ";
                cin >> codigo;

                Produto produto;

                if (buscarProduto(codigo, &produto, tabela)) {
                    cout << "Descricao: " << produto.nome << endl;
                    cout << "Preco: R$ " << fixed << setprecision(2) << produto.preco << endl;
                    cout << "Quantidade disponivel: " << produto.Quantidade << endl;

                    cout << "Quantidade a comprar: ";
                    cin >> quantidade;

                    if (quantidade <= 0) {
                        cout << "Quantidade invalida!" << endl;
                        break;
                    }

                    if (!verificarEstoque(codigo, quantidade, tabela)) {
                        cout << "Estoque insuficiente!" << endl;
                        break;
                    }

                    ItemVenda* item = new ItemVenda;
                    item->codigoProduto = produto.id;
                    item->descricao = produto.nome;
                    item->precoUnitario = produto.preco;
                    item->quantidade = quantidade;
                    item->subtotal = item->precoUnitario * item->quantidade;

                    venda->itens.push_back(*item);
                    venda->totalVenda += item->subtotal;

                    delete item;

                    cout << "Produto adicionado a venda!" << endl;
                    cout << "Total atual: R$ " << fixed << setprecision(2) << venda->totalVenda << endl;
                } else {
                    cout << "Produto nao encontrado ou indisponivel!" << endl;
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
                listarProdutosDisponiveis(tabela);
                break;

            case 5:
                buscarProdutoPorNome(tabela);
                break;

            case 6:
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
                        salvarVenda(venda, tabela);
                        cout << "Venda finalizada com sucesso!" << endl;
                        vendaAtiva = false;
                    }
                }
                break;

            case 7:
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
    TabelaProduto tabela = {0, nullptr};

    carregarTabelaCadsProduto(tabela);

    if (tabela.qtd <= 0) {
        cout << "AVISO: Nenhum produto carregado. Verifique o arquivo 'Produtos_DB.dat'!" << endl;
    }

    do {
        cout << "\n===== MODULO DE VENDAS =====" << endl;
        cout << "1. Nova Venda" << endl;
        cout << "2. Listar Produtos Disponiveis" << endl;
        cout << "3. Buscar Produto por Nome" << endl;
        cout << "4. Recarregar Produtos" << endl;
        cout << "0. Sair" << endl;
        cout << "Escolha uma opcao: ";
        cin >> opcao;

        switch (opcao) {
            case 1:
                realizarVenda(tabela);
                break;
            case 2:
                listarProdutosDisponiveis(tabela);
                break;
            case 3:
                buscarProdutoPorNome(tabela);
                break;
            case 4:
                delete[] tabela.dados;
                tabela.dados = nullptr;
                tabela.qtd = 0;
                carregarTabelaCadsProduto(tabela);
                break;
            case 0:
                cout << "Saindo do modulo de vendas..." << endl;
                break;
            default:
                cout << "Opcao invalida!" << endl;
        }
    } while (opcao != 0);

    delete[] tabela.dados;

    return 0;
}
