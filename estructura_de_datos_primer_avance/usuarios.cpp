#include "todasLasBibliotecas.h"

NodoUsuario* generarNodoUsuario(const string& usuario_temporal_,
    const string& correo_temporal_,
    const string& contrasena_temporal_,
    const string& nombre_temporal_,
    const string& apellido_p_temporal_,
    const string& apellido_m_temporal,
    const SYSTEMTIME& fecha_de_nacimiento_temporal_,
    const string& genero_temporal_,
    const string& ruta_temporal_,
    const string& administrador_temporal_) {

    NodoUsuario* newNodo = new NodoUsuario;

    newNodo->nombreUsuario = usuario_temporal_;
    newNodo->correoElectronico = correo_temporal_;
    newNodo->contrasena = contrasena_temporal_;
    newNodo->nombreCompleto.primerNombre = nombre_temporal_;
    newNodo->nombreCompleto.apellidoPaterno = apellido_p_temporal_;
    newNodo->nombreCompleto.apellidoMaterno = apellido_m_temporal;
    newNodo->fechaNacimiento = fecha_de_nacimiento_temporal_;
    newNodo->genero = genero_temporal_;
    newNodo->fotografia = ruta_temporal_;
    newNodo->tipoUsuario = ruta_temporal_;
    newNodo->tipoUsuario = administrador_temporal_;
    //MetaDatos:
    ZeroMemory(&horaActual, sizeof(SYSTEMTIME));//Limpiar variable SYSTEMTIME (horaActual)
    GetSystemTime(&horaActual);
    newNodo->fechaHoraDeRegistro = horaActual;

    return newNodo;
}

void agregarNodoALaListaUsuarios(NodoUsuario* newNodo_) {

    if (newNodo_ == nullptr) return; // Verifica que el nodo no sea nulo.

    if (inicioListaUsuarios == nullptr) {
        inicioListaUsuarios = newNodo_;
        finalListaUsuarios = newNodo_;
    }
    else {
        newNodo_->anterior = finalListaUsuarios; // Establece el puntero anterior del nuevo nodo.
        finalListaUsuarios->siguiente = newNodo_;
        finalListaUsuarios = newNodo_;
    }
}

void eliminarUsuario() {
    if (auxiliarListaUsuarios == nullptr) {
        return; // No hay nodo para eliminar.
    }

    // Desconectar el nodo de la lista
    if (auxiliarListaUsuarios->anterior) {
        auxiliarListaUsuarios->anterior->siguiente = auxiliarListaUsuarios->siguiente;
    }
    else {
        // Es el primer nodo
        inicioListaUsuarios = auxiliarListaUsuarios->siguiente;
    }

    if (auxiliarListaUsuarios->siguiente) {
        auxiliarListaUsuarios->siguiente->anterior = auxiliarListaUsuarios->anterior;
    }
    else {
        // Es el último nodo
        finalListaUsuarios = auxiliarListaUsuarios->anterior;
    }

    // Eliminar el nodo
    delete auxiliarListaUsuarios;
    auxiliarListaUsuarios = nullptr; // Evitar el uso después de la eliminación
}

void guardarBinarioUsuarios(NodoUsuario* cabezaOriginal, const string& archivoDestino) {
    ofstream archivo(archivoDestino, ios::binary);
    if (!archivo) {
        cerr << "No se pudo abrir el archivo para escribir." << endl;
        return;
    }

    SimpleNodoUsuario nodoSimplificado;

    while (cabezaOriginal != nullptr) {
        // Limpiar el nodo simplificado usando ZeroMemory
        ZeroMemory(&nodoSimplificado, sizeof(nodoSimplificado));

        // Copiar datos al nodo simplificado
        strncpy_s(nodoSimplificado.nombreUsuario, cabezaOriginal->nombreUsuario.c_str(), _TRUNCATE);
        strncpy_s(nodoSimplificado.correoElectronico, cabezaOriginal->correoElectronico.c_str(), _TRUNCATE);
        strncpy_s(nodoSimplificado.contrasena, cabezaOriginal->contrasena.c_str(), _TRUNCATE);
        strncpy_s(nodoSimplificado.nombreCompleto.primerNombre, cabezaOriginal->nombreCompleto.primerNombre.c_str(), _TRUNCATE);
        strncpy_s(nodoSimplificado.nombreCompleto.apellidoPaterno, cabezaOriginal->nombreCompleto.apellidoPaterno.c_str(), _TRUNCATE);
        strncpy_s(nodoSimplificado.nombreCompleto.apellidoMaterno, cabezaOriginal->nombreCompleto.apellidoMaterno.c_str(), _TRUNCATE);
        nodoSimplificado.fechaNacimiento = cabezaOriginal->fechaNacimiento;
        strncpy_s(nodoSimplificado.genero, cabezaOriginal->genero.c_str(), _TRUNCATE);
        strncpy_s(nodoSimplificado.fotografia, cabezaOriginal->fotografia.c_str(), _TRUNCATE);
        strncpy_s(nodoSimplificado.tipoUsuario, cabezaOriginal->tipoUsuario.c_str(), _TRUNCATE);
        nodoSimplificado.fechaHoraDeRegistro = cabezaOriginal->fechaHoraDeRegistro;

        // Escribir nodo simplificado al archivo
        archivo.write((char*)&nodoSimplificado, sizeof(nodoSimplificado));

        // Mover al siguiente nodo en la lista original
        cabezaOriginal = cabezaOriginal->siguiente;
    }

    archivo.close();
}

void leerBinarioUsuarios(const string& archivoFuente, NodoUsuario*& cabezaOriginal, NodoUsuario*& colaOriginal) {
    ifstream archivo(archivoFuente, ios::binary);
    if (!archivo) {
        cerr << "No se pudo abrir el archivo para lectura." << endl;
        return;
    }

    SimpleNodoUsuario nodoSimplificado;
    NodoUsuario* ultimo = nullptr;
    cabezaOriginal = nullptr;

    while (archivo.read((char*)&nodoSimplificado, sizeof(SimpleNodoUsuario))) {
        NodoUsuario* nuevo = new NodoUsuario;

        // Convertir datos de SimpleNodoUsuario a NodoUsuario
        nuevo->nombreUsuario = nodoSimplificado.nombreUsuario;
        nuevo->correoElectronico = nodoSimplificado.correoElectronico;
        nuevo->contrasena = nodoSimplificado.contrasena;
        nuevo->nombreCompleto.primerNombre = nodoSimplificado.nombreCompleto.primerNombre;
        nuevo->nombreCompleto.apellidoPaterno = nodoSimplificado.nombreCompleto.apellidoPaterno;
        nuevo->nombreCompleto.apellidoMaterno = nodoSimplificado.nombreCompleto.apellidoMaterno;
        nuevo->fechaNacimiento = nodoSimplificado.fechaNacimiento;
        nuevo->genero = nodoSimplificado.genero;
        nuevo->fotografia = nodoSimplificado.fotografia;
        nuevo->tipoUsuario = nodoSimplificado.tipoUsuario;
        nuevo->fechaHoraDeRegistro = nodoSimplificado.fechaHoraDeRegistro; // Directamente SYSTEMTIME sin MetaDatos

        // Reconstruir la lista ligada
        nuevo->siguiente = nullptr;
        nuevo->anterior = ultimo;
        if (ultimo) {
            ultimo->siguiente = nuevo;
        }
        else {
            cabezaOriginal = nuevo; // Primer nodo
        }
        ultimo = nuevo;
    }

    colaOriginal = ultimo; // El último nodo procesado será la cola de la lista
    archivo.close();
}

//.....................................................................................
//QUICKSORT:
vector<NodoUsuario*> copiarAVector(NodoUsuario* inicio) {
    vector<NodoUsuario*> vec;
    while (inicio != nullptr) {
        vec.push_back(inicio);
        inicio = inicio->siguiente;
    }
    return vec;
}

int particion(vector<NodoUsuario*>& vec, int bajo, int alto) {
    string pivote = vec[alto]->nombreUsuario;
    int i = bajo - 1;

    for (int j = bajo; j < alto; j++) {
        if (vec[j]->nombreUsuario < pivote) {
            i++;
            std::swap(vec[i], vec[j]);
        }
    }
    std::swap(vec[i + 1], vec[alto]);
    return i + 1;
}

void quickSort(vector<NodoUsuario*>& vec, int bajo, int alto) {
    if (bajo < alto) {
        int pi = particion(vec, bajo, alto);
        quickSort(vec, bajo, pi - 1);
        quickSort(vec, pi + 1, alto);
    }
}

NodoUsuario* reconstruirLista(vector<NodoUsuario*>& vec, NodoUsuario** cola) {
    if (vec.empty()) {
        *cola = nullptr;
        return nullptr;
    }

    NodoUsuario* inicio = vec[0];
    NodoUsuario* actual = inicio;

    for (size_t i = 0; i < vec.size() - 1; ++i) {
        vec[i]->siguiente = vec[i + 1];
        vec[i + 1]->anterior = vec[i];
    }

    vec.back()->siguiente = nullptr;
    vec.front()->anterior = nullptr;
    *cola = vec.back(); // Set the tail pointer

    return inicio;
}

NodoUsuario* ordenarUsuariosPorNombre(NodoUsuario* inicio, NodoUsuario** cola) {
    vector<NodoUsuario*> vec = copiarAVector(inicio);
    if (!vec.empty()) {
        quickSort(vec, 0, vec.size() - 1);
    }
    return reconstruirLista(vec, cola);
}
//.....................................................................................

//BUSQUEDA BINARIA:
vector<NodoUsuario*> copiarNodosAVector(NodoUsuario* inicio) {
    vector<NodoUsuario*> vec;
    while (inicio != nullptr) {
        vec.push_back(inicio);
        inicio = inicio->siguiente;
    }
    return vec;
}

NodoUsuario* busquedaBinariaEnVector(const vector<NodoUsuario*>& vec, const string& nombreUsuario) {
    int bajo = 0, alto = vec.size() - 1;
    while (bajo <= alto) {
        int medio = bajo + (alto - bajo) / 2;
        if (vec[medio]->nombreUsuario == nombreUsuario) {
            return vec[medio];  // Usuario encontrado
        }
        else if (vec[medio]->nombreUsuario < nombreUsuario) {
            bajo = medio + 1;
        }
        else {
            alto = medio - 1;
        }
    }
    return nullptr;  // No se encontró el usuario
}

void buscarYMostrarUsuario(HWND hEdit, HWND hListBox) {
    char nombreUsuario[256];
    GetWindowText(hEdit, nombreUsuario, 256);  // Obtener el texto del EditControl

    vector<NodoUsuario*> usuarios = copiarNodosAVector(inicioListaUsuarios);  // Asume que inicioListaUsuarios es global o accesible
    NodoUsuario* resultado = busquedaBinariaEnVector(usuarios, nombreUsuario);

    if (resultado != nullptr) {
        // Insertar el nombre de usuario encontrado en el ListBox y guardar la referencia al NodoUsuario
        int indice = SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)resultado->nombreUsuario.c_str());
        SendMessage(hListBox, LB_SETITEMDATA, indice, (LPARAM)resultado);
    }
    else {
        // Mostrar un MessageBox indicando que no se encontró el usuario
        MessageBox(NULL, "Usuario no encontrado o no registrado", "Búsqueda de Usuario", MB_OK | MB_ICONINFORMATION);
    }
}