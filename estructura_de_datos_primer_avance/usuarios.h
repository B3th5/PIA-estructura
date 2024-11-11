#pragma once
extern NodoUsuario* inicioListaUsuarios;
extern NodoUsuario* finalListaUsuarios;
extern NodoUsuario* auxiliarListaUsuarios;

NodoUsuario* generarNodoUsuario(const string& usuario_temporal_,
    const string& correo_temporal_,
    const string& contrasena_temporal_,
    const string& nombre_temporal_,
    const string& apellido_p_temporal_,
    const string& apellido_m_temporal,
    const SYSTEMTIME& fecha_de_nacimiento_temporal_,
    const string& genero_temporal_,
    const string& ruta_temporal_,
    const string& administrador_temporal_);

void agregarNodoALaListaUsuarios(NodoUsuario* newNodo_);

void eliminarUsuario();

void guardarBinarioUsuarios(NodoUsuario* cabezaOriginal, const string& archivoDestino);

void leerBinarioUsuarios(const string& archivoFuente, NodoUsuario*& cabezaOriginal, NodoUsuario*& colaOriginal);

//.....................................................................................
//QUICKSORT:
// Función para copiar los nodos de la lista a un vector
std::vector<NodoUsuario*> copiarAVector(NodoUsuario* inicio);

// Función para realizar la partición en QuickSort
int particion(std::vector<NodoUsuario*>& vec, int bajo, int alto);

// Función recursiva de QuickSort
void quickSort(std::vector<NodoUsuario*>& vec, int bajo, int alto);

// Función para reconstruir la lista desde un vector ordenado
NodoUsuario* reconstruirLista(std::vector<NodoUsuario*>& vec, NodoUsuario** cola);

// Función principal para ordenar la lista ligada de usuarios
NodoUsuario* ordenarUsuariosPorNombre(NodoUsuario* inicio, NodoUsuario** cola);
//.....................................................................................

//BUSQUEDA BINARIA:
vector<NodoUsuario*> copiarNodosAVector(NodoUsuario* inicio);

NodoUsuario* busquedaBinariaEnVector(const vector<NodoUsuario*>& vec, const string& nombreUsuario);

void buscarYMostrarUsuario(HWND hEdit, HWND hListBox);
