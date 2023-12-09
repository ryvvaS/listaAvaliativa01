//para rodar o codigo tem que colocar no terminal antes 
//chcp 65001

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>

#define MAX_LINHA 500

typedef struct Artistas
{
    char nomeArtista[100];
    char generoMusical[200];
    char localNascimento[800];
    char albuns[800];
} Artistas;

typedef struct ListaArtistas
{
    Artistas *artistas;
    int numElem;
} ListaArtistas;

int comparaArtistas(const void *a, const void *b)
{
    return strcmp(((Artistas *)a)->nomeArtista, ((Artistas *)b)->nomeArtista);
}

void inicializaLista(ListaArtistas *lista)
{
    lista->artistas = NULL;
    lista->numElem = 0;
}

void adicionaArtista(ListaArtistas *lista, const Artistas *artista)
{
    lista->numElem++;
    lista->artistas = (Artistas *)realloc(lista->artistas, lista->numElem * sizeof(Artistas));
    if (lista->artistas == NULL)
    {
        printf("Erro ao alocar memória para a lista de artistas.\n");
        exit(1);
    }
    lista->artistas[lista->numElem - 1] = *artista;
}

void imprimeLista(ListaArtistas *lista)
{
    for (int i = 0; i < lista->numElem; i++)
    {
        printf("Nome do Artista: %s\n", lista->artistas[i].nomeArtista);
        printf("Gênero Musical: %s\n", lista->artistas[i].generoMusical);
        printf("Local de Nascimento: %s\n", lista->artistas[i].localNascimento);
        printf("Álbuns:\n%s\n", lista->artistas[i].albuns);
        printf("------------------\n");
    }
}

void liberaLista(ListaArtistas *lista)
{
    free(lista->artistas);
    lista->artistas = NULL;
    lista->numElem = 0;
}

int buscaBinariaPorNome(ListaArtistas *listaArtistas, char *nome)
{
    int esq = 0;
    int dir = listaArtistas->numElem - 1;

    while (esq <= dir)
    {
        int meio = esq + (dir - esq) / 2;
        int cmp = strcmp(listaArtistas->artistas[meio].nomeArtista, nome);

        if (cmp == 0)
        {
            return meio; // Encontrou o artista
        }
        else if (cmp < 0)
        {
            esq = meio + 1; // Busca à direita do meio
        }
        else
        {
            dir = meio - 1; // Busca à esquerda do meio
        }
    }

    return -1; // Artista não encontrado
}

void insereArtistaOrdenado(ListaArtistas *lista, const Artistas *artista, const char *arquivo)
{
    int posicaoInsercao = 0;

    while (posicaoInsercao < lista->numElem &&
           strcmp(artista->nomeArtista, lista->artistas[posicaoInsercao].nomeArtista) > 0)
    {
        posicaoInsercao++;
    }

    lista->numElem++;
    lista->artistas = (Artistas *)realloc(lista->artistas, lista->numElem * sizeof(Artistas));

    if (lista->artistas == NULL)
    {
        printf("Erro ao alocar memória para a lista de artistas.\n");
        exit(1);
    }

    // Movendo os elementos para abrir espaço para o novo artista
    for (int i = lista->numElem - 1; i > posicaoInsercao; i--)
    {
        lista->artistas[i] = lista->artistas[i - 1];
    }

    // Inserindo o novo artista na posição correta
    lista->artistas[posicaoInsercao] = *artista;

    // Abrindo o arquivo para escrita
    FILE *file = fopen(arquivo, "w");
    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo para escrita.\n");
        exit(1);
    }

    // Escrevendo todos os artistas no arquivo na ordem correta
    for (int i = 0; i < lista->numElem; i++)
    {
        fprintf(file, "%s\n%s\n%s\n%s\n==========\n",
                lista->artistas[i].nomeArtista, lista->artistas[i].generoMusical,
                lista->artistas[i].localNascimento, lista->artistas[i].albuns);
    }

    // Fechando o arquivo
    fclose(file);
}

void removeArtista(ListaArtistas *lista, int posicao)
{
    if (posicao < 0 || posicao >= lista->numElem)
    {
        printf("Posição inválida.\n");
        return;
    }

    // Cria um arquivo temporário para armazenar os dados atualizados
    FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL)
    {
        printf("Erro ao criar o arquivo temporário.\n");
        exit(1);
    }

    // Abre o arquivo original para leitura
    FILE *file = fopen("artistas.txt", "r");
    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo para edição do artista.\n");
        fclose(tempFile);
        exit(1);
    }

    int linha = 0;
    char textoLinha[MAX_LINHA];
    Artistas artista;

    // Percorre o arquivo original e copia todos os artistas, exceto o que está sendo removido
    while (fgets(textoLinha, MAX_LINHA, file))
    {
        textoLinha[strcspn(textoLinha, "\n")] = '\0';

        if (strcmp(textoLinha, "==========") == 0)
        {
            if (linha >= 4)
            {
                if (linha / 4 != posicao)
                {
                    fprintf(tempFile, "%s\n%s\n%s\n%s\n==========\n",
                            artista.nomeArtista, artista.generoMusical, artista.localNascimento, artista.albuns);
                }
            }
            linha = 0;
        }
        else
        {
            switch (linha)
            {
            case 0:
                strcpy(artista.nomeArtista, textoLinha);
                break;
            case 1:
                strcpy(artista.generoMusical, textoLinha);
                break;
            case 2:
                strcpy(artista.localNascimento, textoLinha);
                break;
            case 3:
                strcpy(artista.albuns, textoLinha);
                break;
            }
            linha++;
        }
    }

    // Fecha ambos os arquivos
    fclose(file);
    fclose(tempFile);

    // Substitue o arquivo original pelo arquivo temporário
    remove("artistas.txt");
    rename("temp.txt", "artistas.txt");

    // Remove o artista da lista em memória
    for (int i = posicao; i < lista->numElem - 1; i++)
    {
        lista->artistas[i] = lista->artistas[i + 1];
    }

    lista->numElem--;
    lista->artistas = (Artistas *)realloc(lista->artistas, lista->numElem * sizeof(Artistas));
    if (lista->numElem == 0)
    {
        free(lista->artistas);
        lista->artistas = NULL;
    }
}

void editaArtista(ListaArtistas *lista, int posicao, const Artistas *novoArtista)
{
    if (posicao < 0 || posicao >= lista->numElem)
    {
        printf("Posição inválida.\n");
        return;
    }

    // Cria um arquivo temporário para armazenar os dados atualizados
    FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL)
    {
        printf("Erro ao criar o arquivo temporário.\n");
        exit(1);
    }

    // Abre o arquivo original para leitura
    FILE *file = fopen("artistas.txt", "r");
    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo para edição do artista.\n");
        fclose(tempFile);
        exit(1);
    }

    int linha = 0;
    char textoLinha[MAX_LINHA];
    Artistas artista;

    // Percorre o arquivo original e copia todos os artistas, exceto o que está sendo editado
    while (fgets(textoLinha, MAX_LINHA, file))
    {
        textoLinha[strcspn(textoLinha, "\n")] = '\0';

        if (strcmp(textoLinha, "==========") == 0)
        {
            if (linha >= 4)
            {
                if (linha / 4 != posicao)
                {
                    fprintf(tempFile, "%s\n%s\n%s\n%s\n==========\n",
                            artista.nomeArtista, artista.generoMusical, artista.localNascimento, artista.albuns);
                }
                else
                {
                    fprintf(tempFile, "%s\n%s\n%s\n%s\n==========\n",
                            novoArtista->nomeArtista, novoArtista->generoMusical,
                            novoArtista->localNascimento, novoArtista->albuns);
                }
            }
            linha = 0;
        }
        else
        {
            switch (linha)
            {
            case 0:
                strcpy(artista.nomeArtista, textoLinha);
                break;
            case 1:
                strcpy(artista.generoMusical, textoLinha);
                break;
            case 2:
                strcpy(artista.localNascimento, textoLinha);
                break;
            case 3:
                strcpy(artista.albuns, textoLinha);
                break;
            }
            linha++;
        }
    }

    // Fecha ambos os arquivos
    fclose(file);
    fclose(tempFile);

    // Substitue o arquivo original pelo arquivo temporário
    remove("artistas.txt");
    rename("temp.txt", "artistas.txt");

    // Atualiza o artista na lista em memória
    lista->artistas[posicao] = *novoArtista;
}

int buscaSequencialPorAlbum(ListaArtistas *lista, char *album)
{
    for (int i = 0; i < lista->numElem; i++)
    {
        if (strstr(lista->artistas[i].albuns, album) != NULL)
        {
            return i; // Encontrou o álbum
        }
    }
    return -1; // Álbum não encontrado
}

int main()
{

    setlocale(LC_ALL, "pt_BR.UTF-8");
    // Substitua o caminho abaixo pelo caminho desejado no seu sistema
char arquivo[100] = "C:\\Users\\ryann\\lista avaliativa\\artistas.txt";


    printf("Texto lido do arquivo:\n");

    ListaArtistas lista;
    inicializaLista(&lista);

    FILE *file = fopen(arquivo, "r");

    if (file == NULL)
    {
        printf("---Erro na abertura do arquivo!!!---\n");
        exit(1);
    }

    char textoLinha[MAX_LINHA];
    Artistas artista;
    int linha = 0;

    while (fgets(textoLinha, MAX_LINHA, file))
    {
        textoLinha[strcspn(textoLinha, "\n")] = '\0';

        if (strcmp(textoLinha, "==========") == 0)
        {
            if (linha >= 4)
            {
                adicionaArtista(&lista, &artista);
            }
            linha = 0;
        }
        else
        {
            switch (linha)
            {
            case 0:
                strcpy(artista.nomeArtista, textoLinha);
                break;
            case 1:
                strcpy(artista.generoMusical, textoLinha);
                break;
            case 2:
                strcpy(artista.localNascimento, textoLinha);
                break;
            case 3:
                strcpy(artista.albuns, textoLinha);
                break;
            default:
                strcat(artista.albuns, "\n");
                strcat(artista.albuns, textoLinha);
                break;
            }
            linha++;
        }
    }
    fclose(file);

    printf("Lista de Artistas:\n");

    qsort(lista.artistas, lista.numElem, sizeof(Artistas), comparaArtistas);

    imprimeLista(&lista);

    int opcao;
    char buscaNome[100];
    char buscaAlbum[100];
    int resultadoBusca;
    char novoNomeArtista[100];
    char novoGeneroMusical[200];
    char novoLocalNasc[800];
    // Artistas attArtista;
    int posicaoBusca;
    // int remArt;
    // int buscaAlb;

    do
    {
        printf("\nMenu:\n");
        printf("1 - Buscar artista por nome\n");
        printf("2 - Buscar artista por álbum\n");
        printf("3 - Adicionar artista\n");
        printf("4 - Editar artista\n");
        printf("5 - Remover artista\n");
        printf("6 - Sair\n");
        printf("\nEscolha uma opção: ");
        scanf("%d", &opcao);

        switch (opcao)
        {
        case 1:
            printf("\n\nDigite o nome do artista a ser buscado: ");
            getchar();
            fgets(buscaNome, sizeof(buscaNome), stdin);
            buscaNome[strcspn(buscaNome, "\n")] = '\0';

            posicaoBusca = buscaBinariaPorNome(&lista, buscaNome);

            if (posicaoBusca != -1)
            {
                printf("\n---Artista encontrado:---\n\n");
                printf("Nome do Artista: %s\n", lista.artistas[posicaoBusca].nomeArtista);
                printf("Gênero Musical: %s\n", lista.artistas[posicaoBusca].generoMusical);
                printf("Local de Nascimento: %s\n", lista.artistas[posicaoBusca].localNascimento);
                printf("Álbuns:\n%s\n", lista.artistas[posicaoBusca].albuns);
            }
            else
            {
                printf("---Artista não encontrado.---\n");
            }
            break;

        case 2:
            printf("\n\nDigite o nome do álbum a ser buscado: ");
            getchar();
            fgets(buscaAlbum, sizeof(buscaAlbum), stdin);
            buscaAlbum[strcspn(buscaAlbum, "\n")] = '\0';

            resultadoBusca = buscaSequencialPorAlbum(&lista, buscaAlbum);

            if (resultadoBusca != -1)
            {
                printf("\nÁlbum encontrado no artista:\n");
                printf("Nome do Artista: %s\n", lista.artistas[resultadoBusca].nomeArtista);
                printf("Álbuns:\n%s\n", lista.artistas[resultadoBusca].albuns);
            }
            else
            {
                printf("Álbum não encontrado em nenhum artista.\n");
            }
            break;

        case 3:
            printf("Digite o nome do artista: ");
            getchar();
            fgets(artista.nomeArtista, sizeof(artista.nomeArtista), stdin);
            artista.nomeArtista[strcspn(artista.nomeArtista, "\n")] = '\0';

            printf("Digite o gênero musical: ");
            fgets(artista.generoMusical, sizeof(artista.generoMusical), stdin);
            artista.generoMusical[strcspn(artista.generoMusical, "\n")] = '\0';

            printf("Digite o local de nascimento: ");
            fgets(artista.localNascimento, sizeof(artista.localNascimento), stdin);
            artista.localNascimento[strcspn(artista.localNascimento, "\n")] = '\0';

            printf("Digite os álbuns (um por linha, digite 'FIM' para encerrar):\n");
            strcpy(artista.albuns, "");

            char album[200];
            while (1)
            {
                fgets(album, sizeof(album), stdin);
                album[strcspn(album, "\n")] = '\0';

                if (strcmp(album, "FIM") == 0)
                {
                    system("cls");
                    printf("Artista adicionado com sucesso!");
                    sleep(2);
                    system("cls");

                    break;
                }

                strcat(artista.albuns, album);
                strcat(artista.albuns, "\n");
            }

            insereArtistaOrdenado(&lista, &artista, arquivo);
            imprimeLista(&lista);

            // Abre o arquivo original para escrita
            file = fopen(arquivo, "a");

            if (file == NULL)
            {
                printf("---Erro na abertura do arquivo!!!---\n");
                exit(1);
            }

            // Escreve o novo artista no arquivo
            fprintf(file, "%s\n%s\n%s\n%s\n==========\n",
                    artista.nomeArtista, artista.generoMusical, artista.localNascimento, artista.albuns);

            fclose(file);

            break;

        case 4:
            printf("Digite o nome do artista a ser editado: ");
            scanf(" %[^\n]s", buscaNome);

            // Busca o artista
            resultadoBusca = buscaBinariaPorNome(&lista, buscaNome);

            if (resultadoBusca != -1)
            {
                printf("Artista encontrado:\n");
                printf("Nome do Artista: %s\n", lista.artistas[resultadoBusca].nomeArtista);
                printf("Gênero Musical: %s\n", lista.artistas[resultadoBusca].generoMusical);
                printf("Local de Nascimento: %s\n", lista.artistas[resultadoBusca].localNascimento);
                printf("Álbuns:\n%s\n", lista.artistas[resultadoBusca].albuns);
                printf("------------------\n");

                printf("Digite o novo nome do artista: ");
                scanf(" %[^\n]s", novoNomeArtista);
                printf("Digite o novo gênero musical do artista: ");
                scanf(" %[^\n]s", novoGeneroMusical);
                printf("Digite o novo local de nascimento do artista: ");
                scanf(" %[^\n]s", novoLocalNasc);

                printf("Digite os novos álbuns do artista (separados por linha, digite 'FIM' para encerrar):\n");
                artista.albuns[0] = '\0';

                while (1)
                {
                    scanf(" %[^\n]s", album);
                    if (strcmp(album, "FIM") == 0)
                    {
                        system("cls");
                        printf("Artista editado com sucesso!");
                        sleep(1);
                        system("cls");
                        imprimeLista(&lista);
                        break;
                    }
                    strcat(artista.albuns, album);
                    strcat(artista.albuns, "\n");
                }

                if (strlen(novoNomeArtista) > 0)
                {
                    strcpy(artista.nomeArtista, novoNomeArtista);
                }
                if (strlen(novoGeneroMusical) > 0)
                {
                    strcpy(artista.generoMusical, novoGeneroMusical);
                }
                if (strlen(novoLocalNasc) > 0)
                {
                    strcpy(artista.localNascimento, novoLocalNasc);
                }

                // Edita o artista na lista em memória
                editaArtista(&lista, resultadoBusca, &artista);

                // Atualiza o arquivo "artistas.txt" com os dados atualizados
                file = fopen(arquivo, "w");
                if (file == NULL)
                {
                    printf("Erro ao abrir o arquivo para edição do artista.\n");
                    exit(1);
                }

                for (int i = 0; i < lista.numElem; i++)
                {
                    fprintf(file, "%s\n%s\n%s\n%s\n==========\n",
                            lista.artistas[i].nomeArtista, lista.artistas[i].generoMusical,
                            lista.artistas[i].localNascimento, lista.artistas[i].albuns);
                }

                fclose(file);

                system("cls");

                printf("Artista editado com sucesso!\n");
                printf("Lista de Artistas Atualizada:\n");
                imprimeLista(&lista);
            }
            else
            {
                printf("Artista não encontrado.\n");
            }
            break;

        case 5:
    printf("\nDigite o nome do artista a ser removido: ");
    getchar();
    fgets(buscaNome, sizeof(buscaNome), stdin);
    buscaNome[strcspn(buscaNome, "\n")] = '\0';

    posicaoBusca = buscaBinariaPorNome(&lista, buscaNome);

    if (posicaoBusca != -1)
    {
        // Remove o artista da lista em memória
        removeArtista(&lista, posicaoBusca);

        // Cria um arquivo temporário para armazenar os dados atualizados
        FILE *tempFile = fopen("temp.txt", "w");
        if (tempFile == NULL)
        {
            perror("Erro ao criar o arquivo temporário");
            exit(1);
        }

        // Abre o arquivo original para leitura
        FILE *file = fopen(arquivo, "r");
        if (file == NULL)
        {
            perror("Erro ao abrir o arquivo original para leitura");
            fclose(tempFile);
            exit(1);
        }

        int linha = 0;
        char textoLinha[MAX_LINHA];
        Artistas artista;

        // Percorre o arquivo original e copia todos os artistas, exceto o que está sendo removido
        while (fgets(textoLinha, MAX_LINHA, file))
        {
            textoLinha[strcspn(textoLinha, "\n")] = '\0';

            if (strcmp(textoLinha, "==========") == 0)
            {
                if (linha >= 4)
                {
                    if (linha / 4 != posicaoBusca)
                    {
                        fprintf(tempFile, "%s\n%s\n%s\n%s\n==========\n",
                                artista.nomeArtista, artista.generoMusical,
                                artista.localNascimento, artista.albuns);
                    }
                }
                linha = 0;
            }
            else
            {
                switch (linha)
                {
                case 0:
                    strcpy(artista.nomeArtista, textoLinha);
                    break;
                case 1:
                    strcpy(artista.generoMusical, textoLinha);
                    break;
                case 2:
                    strcpy(artista.localNascimento, textoLinha);
                    break;
                case 3:
                    strcpy(artista.albuns, textoLinha);
                    break;
                }
                linha++;
            }
        }

        // Fecha ambos os arquivos
        fclose(file);
        fclose(tempFile);

        // Substitue o arquivo original pelo arquivo temporário
        if (remove(arquivo) != 0)
        {
            perror("Erro ao remover o arquivo original");
            exit(1);
        }
        if (rename("temp.txt", arquivo) != 0)
        {
            perror("Erro ao renomear o arquivo temporário");
            exit(1);
        }

        system("cls");
        printf("\n---Artista removido com sucesso!---\n");
        sleep(2);
        system("cls");
        imprimeLista(&lista);
    }
    else
    {
        printf("\n---Artista não encontrado.---\n");
    }
    break;

        case 6:
            printf("Saindo...\n");
            break;

        default:
            printf("---Opção inválida. Tente novamente.---\n");
            break;
        }
    } while (opcao != 6);

    liberaLista(&lista);

    return 0;
}