#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<math.h>
#include<windows.h>
double modulo(double n) { //Fun��o que retorna o m�dulo de um double
    if(n >= 0)
        return n;
    else
        return (n * (-1));
}
//Cabe�alho
double modulo(double n);
void imprime_tableaux(int l, int c, double tableaux[][100], int * tipo_restricao);
void imprime_problema(int op, int l, int c, double * Z, double restricoes[][100], int * tipo_restricao);
void imprime_na_forma_padrao(int l, int c, double * Z, double padrao[][100], int * tipo_restricao, int q_var, int op);
int menu();
void salvar_valores(double * Z, double restricoes[][100], int l, int c, int * tipo_restricao);
int verificar_otimo(double tableaux[][100], int c, int l, int cPivo);
int variable_in(double tableaux[][100], int c);
int variable_out(double tableaux[][100], int ind, int l, int c);
void resultado_tableaux(double tableaux[][100], int l, int c, int * tipo_restricao);
void iteracao_tableaux(double tableaux[][100], int l, int c, int lPivo, int cPivo);
void primeira_fase(double tableaux[][100], double * W, int l, int c, int * tipo_restricao);
int win(double * W, int c);
int verificar_w(double * W, int c);
void iteracao_w(double tableaux[][100], double * W, int l, int c, int lPivo, int cPivo);
void imprime_w(int l, int c, double tableaux[][100], double * W);
int main() {
    int opcao, op, i, j, l, c, aux, tipo_restricao[100], q_var, cont = 0, tamZ;
    double restricoes[100][100], padrao[100][100], Z[100], W[100];
    FILE * arquivo;
    arquivo = fopen("in.txt", "r");
    do {
        do {
            system("cls");
            printf("----------SIMPLEX----------\n");
            printf("DESEJA UTILIZAR PROBLEMAS DO ARQUIVO OU INSERIR PROBLEMAS MANUALMENTE?\n");
            printf("1. ARQUIVO\n");
            printf("2. DIGITAR\n");
            printf("3. SAIR\n\n");
            printf("Opcao: ");
            scanf("%d%*c", & opcao);
        } while(opcao != 1 && opcao != 2 && opcao != 3);
        if(opcao ==  3)
            break;
        if(opcao == 2) {
            system("cls");
            printf("----------SIMPLEX----------\n");
            do {
                printf("\nQuantas restricoes tem o problema? ");  //A quantidade de restri��es representa a quantidade de linhas da matriz
                scanf("%d", & l);
                if(l <= 0)
                    printf("Quantidade invalida!\n");
            } while(l <= 0);
            do {
                printf("\nQuantas variaveis tem o problema? "); //A quantidade de vari�veis + 1 representa a quantidade de colunas da matriz
                scanf("%d", & c);
                if(c <= 0)
                    printf("Quantidade invalida!\n");
            } while(c <= 0);
            tamZ = c;
            c = c + 1;
            printf("\n");
            salvar_valores(Z, restricoes, l, c, tipo_restricao); // 1.2 - fun��o criada separadamente pra ler as entradas de dados
            op = menu(); //1.2 - fun��o menu criada s� pra diminuir um pouco o c�digo na main
        } else {
            if(fscanf(arquivo, "%d %d", & l, & c) == EOF) {
                printf("NAO EXISTE MAIS PROBLEMAS NO ARQUIVO!!!\n");
                getch();
                continue;
            }
            tamZ = c;
            c = c + 1;
            int cont = 0;
            for(i = 0; i < c - 1; i++)
                fscanf(arquivo, "%lf", & Z[i]);
            for(i = 0; i < l; i++)
                for(j = 0; j < c; j++) {
                    if(j == c - 1) { //Verifica se vai ler um coeficiente de X ou a igualdade da restri��o
                        fscanf(arquivo, "%d", & tipo_restricao[cont]);
                        cont++;
                    }
                    fscanf(arquivo, "%lf", & restricoes[i][j]);
                }
            fscanf(arquivo, "%d", & op);
        }
        imprime_problema(op, l, c, Z, restricoes, tipo_restricao);
        //Colocando a matriz na forma padr�o
        //aux=c-1;    //Guarda em aux a posi��o da matriz a partir da qual v�o se colocar as vari�veis de folga
        aux = 0;
        //c=c+l;  //O n�mero de colunas da matriz aumenta por causa das vari�veis de folga
        q_var = c - 1;
        for(i = 0; i < l; i++)
            c = c + tipo_restricao[i];
        for(i = 0; i < l; i++)
            for(j = 0; j < c - 1; j++)
                if(j < q_var) //Ate o tamanho da matriz restricoes apenas copia os coeficientes de uma pra outra
                    padrao[i][j] = restricoes[i][j];
                else {
                    if(j == q_var)  //Copia a igualdade da matriz restri��es para o fim da matriz padrao
                        padrao[i][c - 1] = restricoes[i][j];
                    padrao[i][j] = 0;
                }
        for(j = 0; j < c - 1; j++)
            if(j >= q_var) {
                if(tipo_restricao[cont] == 1)
                    padrao[aux][j] = 1;
                else {
                    if(padrao[aux][j - 1] == -1)
                        padrao[aux][j] = 1;
                    else {
                        padrao[aux][j] = -1;
                        cont--;
                        aux--;
                    }
                }
                cont++;
                aux = aux + 1;
            }
        //1.1 - Colocando Z na forma padr�o
        if(op == 1) //1.3 - s� muda o sinal se for de maximiza��o (equivale a multiplicar todo o Z por -1)
            for(j = 0; j < q_var; j++)
                Z[j] = Z[j] * (-1);
        Z[j] = 0;
        //restricoes = desaloca_matriz(l, restricoes);   //Matriz restri��es n�o vai mais ser usada, ent�o desaloquei ela
        imprime_na_forma_padrao(l, c, Z, padrao, tipo_restricao, q_var, op);
        double tableaux[100][100]; //1.1 - tableaux � a matriz que vai guardar o tableaux
        c = c + 1; //1.1 - precisa de mais uma coluna para o Z
        l = l + 1; //1.1 - precisa de mais uma linha para a base
        //1.1. - Definindo a primeira coluna do tableaux
        if(op == 1)
            tableaux[0][0] = 1;
        else
            tableaux[0][0] = -1; //1.3 - caso seja de minimiza��o multiplica-se a linha base por -1, inclusive o Z
        //Atribui��o da coluna do Z que tem 1 na primeira linha e 0 nas outras
        for(i = 1; i < l; i++)
            tableaux[i][0] = 0;
        //1.1 - Colocando Z no tableaux
        for(j = 1; j < tamZ + 1; j++) //1.1 - Atribuindo o vetor Z como a linha baase no tableaux
            tableaux[0][j] = Z[j - 1];
        for(j = tamZ + 1; j < c; j++)
            tableaux[0][j] = 0;
        //1.1 - Colocando as restri��es no tableaux
        for(i = 1; i < l; i++)
            for(j = 1; j < c; j++)
                tableaux[i][j] = padrao[i - 1][j - 1];
        aux = 0;
        for(i = 0; i < l - 1; i++)
            if(tipo_restricao[i] == 2) {
                aux = 1;
                break;
            }
        printf("\n\nTableaux:");
        if(aux == 1) {
            W[0] = -1;
            for(i = 0; i < l - 1; i++)
                if(tipo_restricao[i] == 2) {
                    for(j = 0; j < c - 1; j++)
                        W[j + 1] = W[j + 1] + (padrao[i][j] * (-1));
                    for(j = c - 1; j != 0; j--) {
                        if(W[j] == -1) {
                            W[j] = 0;
                            break;
                        }
                    }
                }
            imprime_w(l, c, tableaux, W);
            primeira_fase(tableaux, W, l, c, tipo_restricao);
        } else {
            imprime_tableaux(l, c, tableaux, tipo_restricao);
            resultado_tableaux(tableaux, l, c, tipo_restricao);
        }
        do {
            system("cls");
            printf("DESEJA CONTINUAR COM OUTRO PROBLEMA?\n");
            printf("1. SIM\n");
            printf("2. NAO\n\n");
            printf("Opcao: ");
            scanf("%d", & opcao);
        } while(opcao != 1 && opcao != 2);
    } while(opcao != 2);
    fclose(arquivo);
    return 0;
}
void imprime_w(int l, int c, double tableaux[][100], double * W) {
    int i, j, isBasic, k, * x, cont = 0;
    x = (int * )calloc(l - 1, sizeof(int));
    printf("\n\n");
    printf("\tW\t");
    for(j = 0; j < c - 2; j++)
        printf("X%d\t", j + 1);
    printf("b\n\n");
    printf("base\t");
    for(j = 0; j < c; j++)
        printf("%.1lf\t", W[j]);
    printf("\n\n");
    //1.1 - imprime as outras linhas
    for(i = 1; i < l; i++) {
        for(j = 1; j < c; j++)
            if(tableaux[i][j] == 1) {
                isBasic = 0;
                for(k = 1; k < l; k++)
                    if(tableaux[k][j] != 0)
                        isBasic = isBasic + 1;
                if(isBasic == 1) {
                    x[cont++] = j;
                    printf("X%d\t", j);
                    break;
                }
            }
        for(j = 0; j < c; j++)
            printf("%.1lf\t", tableaux[i][j]);
        printf("\n\n");
    }
    getch();
}
void iteracao_w(double tableaux[][100], double * W, int l, int c, int lPivo, int cPivo) {
    int i, j;
    double aux;
    aux = tableaux[lPivo][cPivo];
    for(i = 0; i < c; i++)
        tableaux[lPivo][i] = tableaux[lPivo][i] / aux;
    for(i = 0; i < l; i++)
        if(i != lPivo)
            for(j = 0; j < c; j++) {
                if(j == 0)
                    aux = tableaux[i][cPivo];
                tableaux[i][j] = tableaux[i][j] - (aux * tableaux[lPivo][j]);
            }
    for(j = 0; j < c; j++) {
        if(j == 0)
            aux = W[cPivo];
        W[j] = W[j] - (aux * tableaux[lPivo][j]);
    }
}
int verificar_w(double * W, int c) {
    int i, aux = 0;
    for(i = 1; i < c - 1; i++) {
        if(W[i] < 0) {
            aux = 1;
            break;
        }
    }
    if(aux == 0 && W[c - 1] != 0)
        aux = 2;
    return aux;
}
int win(double * W, int c) {
    int i, indice = 0;
    double menor = 0;
    for(i = 1; i < c - 1; i++) {
        if(W[i] < menor) {
            menor = W[i];
            indice = i;
        }
    }
    return indice;
}

void primeira_fase(double tableaux[][100], double * W, int l, int c, int * tipo_restricao) {
    int isOtimo, in, out, i, aux, j;
    int cPivo;
    cPivo = win(W, c);
    isOtimo = verificar_w(W, c);
    if(isOtimo == 1) {
        printf("\nW NAO E IGUAL A ZERO!!!\n");
        in = win(W, c);
        printf("\nENTRA A VARIAVEL X%d\n", in);
        out = variable_out(tableaux, in, l, c);
        aux = 0;
        for(j = 1; j < c - 1; j++) {
            if(tableaux[out][j] == 1) {
                for(i = 1; i < l; i++) {
                    if(tableaux[i][j] != 0)
                        aux++;
                }
            }
            if(aux == 1) {
                aux = j;
                break;
            } else
                aux = 0;
        }
        printf("SAI A VARIAVEL X%d\n", aux);
        getch();
        printf("\n\nNOVA SOLUCAO:\n");
        iteracao_w(tableaux, W, l, c, out, in);
        imprime_w(l, c, tableaux, W);
        primeira_fase(tableaux, W, l, c, tipo_restricao);
    } else {
        if(isOtimo == 2)
            printf("\nA FUNCAO NAO TEM SOLUCAO!!!\n");
        else {
            printf("\nW E IGUAL A ZER0!!!\n");
            printf("\nFIM DA PRIMEIRA FASE\n");
        }
        getch();
        if(isOtimo == 0) {
            printf("\n\nINICIO DA SEGUNDA FASE:\n");
            imprime_tableaux(l, c, tableaux, tipo_restricao);
            resultado_tableaux(tableaux, l, c, tipo_restricao);
        }
    }
}
//1.1 - fun��o que imprime o tableaux
void imprime_tableaux(int l, int c, double tableaux[][100], int * tipo_restricao) {
    int i, j, isBasic, k, * x, cont = 0, tam, k2;
    x = (int * )calloc(l - 1, sizeof(int)); //2.0 - vetor que guarda os �ndices dos x pra serem impressos embaixo do tableaux
    printf("\n\n");
    printf("\tZ\t");
    tam = c - 2;
    for(i = 0; i < l - 1; i++)
        tam = tam - tipo_restricao[i];
    for(j = 0; j < tam; j++)
        printf("X%d\t", j + 1);
    k = 0;
    for(j = tam; j < c - 2; j++) {
        printf("X%d\t", j + 1);
        if(tipo_restricao[k++] == 2)
            j++;
    }
    printf("b\n\n");
    printf("base\t");
    k = 0;
    for(j = 0; j < c; j++) {
        printf("%.1lf\t", tableaux[0][j]);   //1.1 - imprime a primeira linha do tableaux
        if(j >= tam + 1 && j < c - 1)
            if(tipo_restricao[k++] == 2)
                j++;
    }
    printf("\n\n");
    //1.1 - imprime as outras linhas
    for(i = 1; i < l; i++) {
        for(j = 1; j < c; j++) {
            if(tableaux[i][j] == 1) { //1.1 - procura por 1 na linha
                isBasic = 0;
                for(k = 1; k < l; k++) //1.1 - se encontrar percorre toda a coluna
                    if(tableaux[k][j] != 0)
                        isBasic = isBasic + 1;
                if(isBasic == 1) { //1.1 - se o 1 for o �nico elemento diferente de 0 na coluna, a vari�vel � b�sica
                    x[cont++] = j;  //2.0 - Nesse ponto x guarda os indices dos X que est�o na base
                    printf("X%d\t", j); //1.1 - imprima ela
                    break;
                }
            }
        }
        k2 = 0;
        for(j = 0; j < c; j++) {
            printf("%.1lf\t", tableaux[i][j]);   //1.1 - imprime os elementos de cada linha do tableaux
            if(j >= tam + 1 && j < c - 1)
                if(tipo_restricao[k2++] == 2)
                    j++;
        }
        printf("\n\n");
    }
    k2 = 0;
    for(i = 0; i < c - 2; i++) { //2.0 - c-2 pois a quantidade de X � a quantidade de colunas menos a linha Z e a linha b
        k = 0;  //2.0 - k indica se o x est� ou n�o na base
        printf("X%d = ", i + 1);
        for(j = 0; j < cont; j++)//2.0 - percorre o vetor e busca se o �ndice do x em quest�o est� na base
            if(i + 1 == x[j]) { //2.0 - se sim imprime o valor dele
                printf("%.1lf; ", tableaux[j + 1][c - 1]);
                k = 1;
                break;
            }
        if(k == 0)  //2.0 - se n�o imprime 0
            printf("0; ");
        if(i >= tam)
            if(tipo_restricao[k2++] == 2)
                i++;
    }
    if(tableaux[0][0] == -1 && tableaux[0][c - 1] != 0) //2.0 - Imprime o valor de Z (Max) ou -Z (Min)
        printf("\n\nZ = %.1lf\n\n", tableaux[0][c - 1] * (-1));
    else
        printf("\n\nZ = %.1lf\n\n", tableaux[0][c - 1]);
    getch();
}
//1.1 - fun��o que imprime o problema
void imprime_problema(int op, int l, int c, double * Z, double restricoes[][100], int * tipo_restricao) {
    int i, j, cont = 0;
    //Imprime o problema
    printf("O problema e:\n");
    //Dependendo da op��o op imprime Max ou Min
    if(op == 1)
        printf("\nMax Z = ");
    if(op == 2)
        printf("\nMin Z = ");
    //Imprime Z
    for(j = 0; j < c - 1; j++) {
        if(j == 0)  //Se for o primeiro elemento apenas imprime normalmente
            printf("%.0lfX%d ", Z[j], j + 1);
        else
            printf("%.0lfX%d ", modulo(Z[j]), j + 1); //Se for outro elemento imprime apenas o m�dulo
        if(j != c - 2) { //Aqui imprime o sinal para que fique alinhado
            if(Z[j + 1] >= 0)
                printf("+ ");
            else
                printf("- ");
        }
    }
    //Imprime as restri��es
    printf("\n\nSujeito a:\n\n");
    for(i = 0; i < l; i++) {
        for(j = 0; j < c - 1; j++) { //Vai at� o utimo X
            if(j == 0)
                printf("%.0lfX%d ", restricoes[i][j], j + 1);
            else
                printf("%.0lfX%d ", modulo(restricoes[i][j]), j + 1);
            if(j != c - 2) {
                if(restricoes[i][j + 1] >= 0)
                    printf("+ ");
                else
                    printf("- ");
            } else { //Se for o ultimo X, imprime a igualdade e o numero depois dela
                if(tipo_restricao[cont] == 1)
                    printf("<= ");
                else
                    printf(">= ");
                cont++;
                printf("%.0lf ", restricoes[i][c - 1]);
            }
        }
        printf("\n\n");
    }
    //Imprime as restri��es de >=0
    for(j = 0; j < c - 1; j++) {
        printf("X%d", j + 1);
        if(j != c - 2)
            printf(", ");
    }
    printf(" >= 0");
    getch();
}
//1.1 - fun��o que imprime o problema na forma padr�o
void imprime_na_forma_padrao(int l, int c, double * Z, double padrao[][100], int * tipo_restricao, int q_var, int op) {
    int i, j, cont = 0;
    //Imprimindo na forma padr�o
    printf("\n\nForma padrao:\n");
    //1.1 - impress�o do Z modificada para uma forma mais f�cil
    if(op == 1)
        printf("\nZ ");
    else
        printf("\n- Z ");
    for(j = 0; j < q_var; j++) {
        if(Z[j] >= 0)
            printf("+ ");
        else
            printf("- ");
        printf("%.0lfX%d ", modulo(Z[j]), j + 1);
    }
    for(i = q_var + 1; cont < l; i++) {
        if(tipo_restricao[cont] == 1)
            printf("+ 0X%d ", i);
        else {
            printf("+ 0X%d ", i);
            i++;
            printf("+ 0X%d ", i);
        }
        cont++;
    }
    printf("= 0");
    printf("\n\nSujeito a:\n\n");
    for(i = 0; i < l; i++) {
        for(j = 0; j < c - 1; j++) {
            if(j == 0)
                printf("%.0lfX%d ", padrao[i][j], j + 1);
            else
                printf("%.0lfX%d ", modulo(padrao[i][j]), j + 1);
            if(j != c - 2) {
                if(padrao[i][j + 1] >= 0)
                    printf("+ ");
                else
                    printf("- ");
            } else {
                printf("= ");
                printf("%.0lf ", padrao[i][c - 1]);
            }
        }
        printf("\n\n");
    }
    for(j = 0; j < c - 1; j++) {
        printf("X%d", j + 1);
        if(j != c - 2)
            printf(", ");
    }
    printf(" >= 0");
    getch();
}
//1.2 - fun��o p/ selecionar a op��o
int menu() {
    int op;
    do {
        printf("\nO problema e de:\n");
        printf("1. Maximizacao\n");
        printf("2. Minimizacao\n");
        printf("? ");
        scanf("%d", & op);
        if(op != 1 && op != 2)
            printf("Opcao invalida!\n");
    } while(op != 1 && op != 2);
    system("cls");
    return op;
}
//1.2 - fun��o p/ ler valores de Z e da matriz de Restri��es
void salvar_valores(double * Z, double restricoes[][100], int l, int c, int * tipo_restricao) {
    int i, j, cont = 0;
    for(i = 0; i < c - 1; i++) {
        printf("Digite o coeficiente de X%d de Z: ", i + 1);
        scanf("%lf", & Z[i]);
    }
    for(i = 0; i < l; i++)
        for(j = 0; j < c; j++) {
            if(j == c - 1) { //Verifica se vai ler um coeficiente de X ou a igualdade da restri��o
                do {
                    printf("A restricao e de:\n");
                    printf("1. <=\n");
                    printf("2. >=\n? ");
                    scanf("%d", & tipo_restricao[cont]);
                    if(tipo_restricao[cont] != 1 && tipo_restricao[cont] != 2)
                        printf("Opcao invalida!\n");
                } while(tipo_restricao[cont] != 1 && tipo_restricao[cont] != 2);
                cont++;
                printf("Digite o resultado da restricao %d: ", i + 1);
            } else
                printf("Digite o coeficiente de X%d da restricao %d: ", j + 1, i + 1);
            scanf("%lf", & restricoes[i][j]);
        }
}
//1.2 Verifica se a fun��o � �tima
int verificar_otimo(double tableaux[][100], int c, int l, int cPivo) { //1.6 - O cabe�alho da fun��o foi alterado, pois h� a necessidade de receber o numero de linhas do tableaux e a coluna Piv�.
    int i, aux = 0;
    if(cPivo != 0) { //1.8.1 - Precisei fazer isso pra corrigir um bug, atribui 0 como valor inicial do indice de variable_in, pois no ultimo tableaux, como n�o tem variaveis pra entrar a fun��o vai jogar um lixo em cPivo, consequentemente quando tu tentar ler ela aqui o programa vai parar de funcionar, pois n�o existe a coluna "lixo"
        for(i = 0; i < l; i++) //1.6 - para percorrer toda a coluna piv�
            if(tableaux[i][cPivo] > 0)
                aux++; //1.6 - Se pelo algum elemento da coluna piv� for positivo, aux � incrementado, indicando que a solu��o n�o � infinita
        if(aux == 0) //1.6 - Se nesse ponto do programa aux for igual 0, indica que na coluna piv� s� h� valores negativos
            return 2; //1.6 - Retorna 2 indicando que a solu��o � infinita
        aux = 0;
    }
    for(i = 1; i < c - 1; i++) //1.2 - percorre somente as culunas das vari�veis Xi.
        if(tableaux[0][i] < 0) {
            aux = 1; //1.2 - aux recebe 1, indicando que ainda h� vari�veis negativas, portanto a solu��o n�o � otima.
            break;
        }
    return aux; //1.2 - se aux aqui for 0, indica que a solu��o � �tima
}
//1.2 - fun��o que encontra quem deve entrar na base.
int variable_in(double tableaux[][100], int c) {
    int i, indice = 0; //1.2 - indice vai indicar a coluna do tlabeux que est� a vari�vel que deve entrar na base.
    double menor = 0.0; //1.2 - variavel auxiliar pra ir comparando os valores das vari�veis Xi
    for(i = 1; i < c - 1; i++) //1.5 - i inicia com 1, pq n�o precisa comparar a coluna do Z e vai at� c-1, pq n�o compara a umtima coluna de resultados.
        if(tableaux[0][i] < menor) {
            menor = tableaux[0][i];
            indice = i; //1.2 - Se por exemplo, vari�vel que for entrar na base for X1, o indice vai guardar 1
        }
    return indice;
}
//1.2 - fun��o que encontra quem deve sair da base.
int variable_out(double tableaux[][100], int ind, int l, int c) {
    int i, indice; // 1.2- indice guarda a linha que est� a vari�vel que vai sair.
    double menor = 100000.0, result; //1.2 - result guarda o resultado da divis�o dos elementos de B pelos valores da coluna piv�
    for(i = 1; i < l; i++) //1.2 - come�a a percorrer depois da linha do Z
        if(tableaux[i][ind] > 0) { //1.2 - S� ir� dividir se o denominador for maior que 0
            result = tableaux[i][c - 1] / tableaux[i][ind]; //1.2 - divis�o da �ltima coluna do tableaux pelos elementos da linha piv�.
            if(result < menor) { // 1.2 - vai comparando se o resultado da divis�o � menor que o valor da divis�o anterior
                menor = result; //1.2 - se for menor, armazena este resultado como o novo menor
                indice = i;
            }
        }
    return indice;
}
//1.5 - fun��o para testar se as fun��es variable_in e variable_out est�o funcionando
void resultado_tableaux(double tableaux[][100], int l, int c, int * tipo_restricao) {
    int isOtimo, in, out, i, aux, j;
    int cPivo; //1.6 - vari�vel auxiliar para receber a coluna piv� do tableaux
    cPivo = variable_in(tableaux, c); //1.6 - A fun��o variable_in s� � chamada pq a fun��o abaixo(verificar_otimo) precisa saber quem � a coluna piv�
    isOtimo = verificar_otimo(tableaux, c, l, cPivo);
    //1.8 - Verifica se a solu��o � multipla
    aux = 0;
    for(i = 1; i < c - 1; i++) //1.8 - Percorre a linha da base guardando a quantidade de zeros em aux
        if(tableaux[0][i] == 0)
            aux++;
    for(i = 0; i < l - 1; i++)
        if(tipo_restricao[i] == 2)
            aux--;
    if(aux > l - 1) //1.8 - Se houverem mais zeros do que variaveis na base a solu��o � multipla
        printf("\nO PROBLEMA POSSUI MULTIPLAS SOLUCOES!\n");
    //1.7 - Verifica se a solu��o � degenerada
    aux = 0;
    for(i = 1; i < l; i++)//1.7 - A linha da base n�o conta, por isso come�a do 1
        if(tableaux[i][c - 1] <= 1e-10 && tableaux[i][c - 1] >= -1e-10) //1.7 - Se houver algum 0 na ultima coluna (b) incrementa aux
            aux++;
    if(aux > 0) //1.7 - Se aux for maior que 0 significa que alguma variavel b�sica � 0 (Solu��o degenerada)
        printf("\nA SOLUCA0 E DEGENERADA!\n");
    if(isOtimo == 1) {
        printf("\nA SOLUCAO NAO E OTIMA!!!\n");
        in = variable_in(tableaux, c);
        printf("\nENTRA A VARIAVEL X%d\n", in);
        out = variable_out(tableaux, in, l, c);
        //2.0 - baseado no �ndice de out, encontra qual X vai sair da base (quase o msm algoritmo que esta em imprimir_tableaux)
        aux = 0;
        for(j = 1; j < c - 1; j++) {
            if(tableaux[out][j] == 1)
                for(i = 1; i < l; i++)
                    if(tableaux[i][j] != 0)
                        aux++;
            if(aux == 1) {
                aux = j;
                break;
            } else
                aux = 0;
        }
        printf("SAI A VARIAVEL X%d\n", aux);
        getch();
        printf("\n\nNOVA SOLUCAO:\n");
        iteracao_tableaux(tableaux, l, c, out, in);
        imprime_tableaux(l, c, tableaux, tipo_restricao);
        resultado_tableaux(tableaux, l, c, tipo_restricao); //1.5 - como n�o � otimo chama a fun��o recursivamente.
    } else {
        if(isOtimo == 2)
            printf("\nA FUNCAO TEM SOLUCAO INFINITA!!!\n");
        else
            printf("\nA SOLUCAO E OTIMA!!!\n");
        getch();
    }
}
//1.5 - fun��o que aplica os c�lculos no tableuax
void iteracao_tableaux(double tableaux[][100], int l, int c, int lPivo, int cPivo) {
    int i, j;
    double aux;
    aux = tableaux[lPivo][cPivo];
    for(i = 0; i < c; i++) //1.5 - para o c�lculo da nova linha piv�
        tableaux[lPivo][i] = tableaux[lPivo][i] / aux;   //A nova linha ser� ela mesma dividida pela intersecc��o entre a linha e a coluna piv�s
    for(i = 0; i < l; i++) { //1.5 - inicia o percurso pela primeira linha do tableaux
        if(i != lPivo) //1.5 - s� prossigo com os c�lculos se a linha atual n�o for a linha piv�, pois ela j� foi calculada antes
            for(j = 0; j < c; j++) { //1.5 - caso n�o seja a LP, percorre toda a linha
                if(j == 0)
                    aux = tableaux[i][cPivo];//1.5 - aux guarda a intersec��o entre a linha atual com a coluna piv�
                tableaux[i][j] = tableaux[i][j] - (aux * tableaux[lPivo][j]);
                // 1.5 - Os novos valores de cada linha, � cada valor subtraido da multiplica��o de aux com o valor correspondente na mesma coluna, s� que na linha Piv�
            }
    }
}
