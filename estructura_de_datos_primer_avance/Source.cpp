#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <fstream>
#include <iostream>
#include <cstring>

using namespace std;

class GestionClientes {
private:
    struct NodoCliente {
        char nombre[20];
        int id;
        NodoCliente* izq;
        NodoCliente* der;
    };

    NodoCliente* raiz_cte;

    NodoCliente* nuevoCliente() {
        NodoCliente* nuevo = new NodoCliente;
        nuevo->izq = nullptr;
        nuevo->der = nullptr;
        return nuevo;
    }

    void archivoPreOrden(NodoCliente* arbol, fstream& archi) {
        if (arbol != nullptr) {
            archi.write((char*)arbol, sizeof(NodoCliente));
            archivoPreOrden(arbol->izq, archi);
            archivoPreOrden(arbol->der, archi);
        }
    }

    void agregarNodo(NodoCliente** arbol, NodoCliente dato) {
        NodoCliente* aux = nullptr;
        if (*arbol == nullptr) {
            aux = new NodoCliente;
            aux->der = nullptr;
            aux->izq = nullptr;
            aux->id = dato.id;
            strcpy(aux->nombre, dato.nombre);
            *arbol = aux;
        }
        else {
            if (dato.id > (*arbol)->id)
                agregarNodo(&(*arbol)->der, dato);
            else if (dato.id < (*arbol)->id)
                agregarNodo(&(*arbol)->izq, dato);
        }
    }

    void buscarNodo(NodoCliente** arbol, int dato) {
        if (*arbol == nullptr) {
            printf("Cliente no existe\n");
        }
        else {
            if (dato > (*arbol)->id)
                buscarNodo(&(*arbol)->der, dato);
            else if (dato < (*arbol)->id)
                buscarNodo(&(*arbol)->izq, dato);
            else {
                printf("ID CTE\t\tNOMBRE (pre)\n");
                printf("%d\t\t%s\n", (*arbol)->id, (*arbol)->nombre);
            }
        }
    }

    void listaPreOrden(NodoCliente* arbol) {
        if (arbol != nullptr) {
            printf("%d\t\t%s\n", arbol->id, arbol->nombre);
            listaPreOrden(arbol->izq);
            listaPreOrden(arbol->der);
        }
    }

    void listaEnOrden(NodoCliente* arbol) {
        if (arbol != nullptr) {
            listaEnOrden(arbol->izq);
            printf("%d\t\t%s\n", arbol->id, arbol->nombre);
            listaEnOrden(arbol->der);
        }
    }

    void listaPostOrden(NodoCliente* arbol) {
        if (arbol != nullptr) {
            listaPostOrden(arbol->izq);
            listaPostOrden(arbol->der);
            printf("%d\t\t%s\n", arbol->id, arbol->nombre);
        }
    }

    void leerClientes(const char nombrearchivo[]) {
        NodoCliente temp;
        fstream archivo;
        archivo.open(nombrearchivo, ios::in | ios::binary);
        if (archivo.is_open()) {
            archivo.read((char*)&temp, sizeof(NodoCliente));
            temp.izq = temp.der = nullptr;

            while (!archivo.eof()) {
                agregarNodo(&raiz_cte, temp);
                archivo.read((char*)&temp, sizeof(NodoCliente));
                temp.izq = temp.der = nullptr;
            }

            archivo.close();
        }
    }

    void escribirClientes(const char nombrearchivo[]) {
        fstream archivo;
        archivo.open(nombrearchivo, ios::out | ios::binary | ios::trunc);
        if (archivo.is_open()) {
            archivoPreOrden(raiz_cte, archivo);
            archivo.close();
        }
    }

public:
    GestionClientes() : raiz_cte(nullptr) {}

    void capturarCliente(NodoCliente* cliente) {
        printf("Código: ");
        cin >> cliente->id;
        printf("Nombre: ");
        cin.ignore();
        cin.getline(cliente->nombre, 20);
    }

    void agregarCliente() {
        NodoCliente cliente;
        capturarCliente(&cliente);
        agregarNodo(&raiz_cte, cliente);
    }

    void buscarCliente() {
        int id;
        printf("ID del cliente a buscar: ");
        cin >> id;
        buscarNodo(&raiz_cte, id);
    }

    void listarClientesPreOrden() {
        printf("ID CTE\t\tNOMBRE (pre)\n");
        listaPreOrden(raiz_cte);
    }

    void listarClientesEnOrden() {
        printf("ID CTE\t\tNOMBRE (in)\n");
        listaEnOrden(raiz_cte);
    }

    void listarClientesPostOrden() {
        printf("ID CTE\t\tNOMBRE (post)\n");
        listaPostOrden(raiz_cte);
    }

    void gestionarArchivo(const char* archivo) {
        leerClientes(archivo);
        escribirClientes(archivo);
    }
};

int main() {
    GestionClientes gestion;
    short opc1 = 0;
    char archivo[] = "clientes.dat";

    gestion.gestionarArchivo(archivo);

    do {
        system("cls");
        system("color A1");
        printf(" << GESTION DE CLIENTES (ABB) >>\n");
        printf("1. Agregar cliente\n");
        printf("2. Buscar cliente\n");
        printf("3. Lista de clientes (pre)\n");
        printf("4. Lista de clientes (entre)\n");
        printf("5. Lista de clientes (post)\n");
        printf("6. Eliminar\n");
        printf("7. Salir\n");
        cin >> opc1;

        switch (opc1) {
        case 1:
            gestion.agregarCliente();
            break;
        case 2:
            gestion.buscarCliente();
            break;
        case 3:
            gestion.listarClientesPreOrden();
            system("pause");
            break;
        case 4:
            gestion.listarClientesEnOrden();
            system("pause");
            break;
        case 5:
            gestion.listarClientesPostOrden();
            system("pause");
            break;
        case 6:

            break;
        case 7:
            break;
        default:
            break;
        }
    } while (opc1 < 6);

    gestion.gestionarArchivo(archivo);
    return 0;
}
