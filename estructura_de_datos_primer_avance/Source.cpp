#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include "resource.h"

#include <fstream>
#include <iostream>
#include <cstring>
#include <ctime>
#include <string>
#include <vector>

using namespace std;

int ultimoID = 0;
int usuarioLogueado = 0;
// Función para obtener la fecha actual
string obtenerFechaActual();

//----------------------------------------------------------------CLIENTES-----------------------------------------------------------------

// Estructura del Cliente (nodo del árbol)
struct Cliente {
    int id;
    string nombre;
    string apellidoPaterno;
    string apellidoMaterno;
    string usuario;
    string contrasenia;
    string fechaRegistro;
    string estatus;
    string fechaCambioEstatus;
    int idUsuarioActualizo; // Nuevo campo para el ID del usuario que actualizó
    Cliente* izquierda;
    Cliente* derecha;

    Cliente(int id, const string& nombre, const string& apellidoPaterno, const string& apellidoMaterno, const string& usuario, const string& contrasenia, const string& estatus, const string& fechaActualizo, int idUsuarioActualizo)
        : id(id), nombre(nombre), apellidoPaterno(apellidoPaterno), apellidoMaterno(apellidoMaterno), usuario(usuario), contrasenia(contrasenia), fechaRegistro(obtenerFechaActual()), estatus(estatus),
        fechaCambioEstatus(fechaActualizo), idUsuarioActualizo(idUsuarioActualizo), // Inicializado a 0
        izquierda(nullptr), derecha(nullptr) {}

    Cliente() : id(0), // Inicializado a 0
        izquierda(nullptr), derecha(nullptr) {}
};


// Función para agregar un cliente al árbol
Cliente* agregarCliente(Cliente* raiz, int id, const string& nombre, const string& apellidoPaterno, const string& apellidoMaterno, const string& usuario, const string& contrasenia, const string& estatus, const string& fechaActualizo, int idUsuarioActualizo);

// Función para buscar un cliente en el árbol
Cliente* buscarCliente(Cliente* raiz, int id);

// Función para mostrar los clientes en orden (ID en orden ascendente)
void mostrarClientesEnOrden(Cliente* raiz, HWND hwndListBox);

// Función para cambiar el estatus de un cliente
void cambiarEstatus(Cliente* raiz, int id, const string& nuevoEstatus);

// Función para liberar la memoria del árbol
void eliminarArbolClientes(Cliente* raiz);

// Función para guardar los datos en un archivo binario
void guardarDatos(Cliente* raiz, ofstream& archivo);

Cliente* cargarDatos(ifstream& archivo, Cliente* raiz);

Cliente* buscarClientePorID(Cliente* raiz, int clienteID);

Cliente* buscarClientePorUsuario(Cliente* raiz, const string& usuario);

Cliente* raiz = nullptr;

INT_PTR CALLBACK fVentanaLogin(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK fVentanaDashboard(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK fVentanaRClientes(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK fVentanaEClientes(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, PSTR cmdline, int nCmdShow) {

    
    // Cargar datos al iniciar el programa
    ifstream archivoEntrada("clientes.bin", ios::binary);
    if (archivoEntrada) {
        raiz = cargarDatos(archivoEntrada, raiz);
        archivoEntrada.close();
    }
    else {
        raiz = agregarCliente(raiz, 1, "ADMIN", "admin", "null", "admin", "Adminpass", "Activo", obtenerFechaActual(), 1);
    }

    // Crear la ventana de login
    HWND hVentanaLogin = CreateDialog(hInstance, MAKEINTRESOURCE(DLG_LOGIN), NULL, fVentanaLogin);

    ShowWindow(hVentanaLogin, SW_SHOW);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Guardar datos al cerrar el programa
    ofstream archivoSalida("clientes.bin", ios::binary);
    if (archivoSalida) {
        guardarDatos(raiz, archivoSalida);
        archivoSalida.close();
    }
    // Liberar memoria del árbol
    eliminarArbolClientes(raiz);
    
    return 0;
}
//================================================================== CALLBACKS =====================================================================

INT_PTR CALLBACK fVentanaLogin(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case BTN_LOGIN_LOGIN: // Manejar el clic en el botón de inicio de sesión
        {
            // Obtener el texto de los cuadros de texto
            char usuario[20];
            char contrasenia[20];
            GetDlgItemText(hwnd, TXT_LOGIN_USER, usuario, 256);
            GetDlgItemText(hwnd, TXT_LOGIN_PASS, contrasenia, 256);

            Cliente* cliente = buscarClientePorUsuario(raiz, usuario);

            // Verificar si el cliente existe y las credenciales son correctas
            if (cliente != nullptr && strcmp(cliente->usuario.c_str(), usuario) == 0 && strcmp(cliente->contrasenia.c_str(), contrasenia) == 0) {
                usuarioLogueado = cliente->id;
                MessageBox(hwnd, "Inicio de sesión exitoso", "Éxito", MB_OK);
                // Proceder con el flujo de la aplicación o cerrar la ventana de login
                // Cerrar la ventana de login
                EndDialog(hwnd, IDOK);

                // Abrir la ventana de dashboard (suponiendo que es un diálogo)
                DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHBOARD), hwnd, fVentanaDashboard);
            }
            else {
                MessageBox(hwnd, "Usuario o contraseña incorrectos", "Error", MB_OK);
            }
        }
        break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(9);
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK fVentanaDashboard(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_INITDIALOG: {
        // Cargar y asignar el menú (si es necesario y aplicable para un diálogo)
        HMENU hMenu = LoadMenu((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDR_MENU3));
        if (hMenu) {
            SetMenu(hwnd, hMenu);
        }
        // Obtener el control ListBox desde la ventana
        HWND hwndListBox = GetDlgItem(hwnd, LIS_DASHBOARD_CLIENTES);  // Reemplaza IDC_LISTBOX con el ID de tu ListBox

        // Llamar a la función para mostrar los clientes en el ListBox
        mostrarClientesEnOrden(raiz, hwndListBox);

        return TRUE; // Devuelve TRUE para indicar que se ha inicializado correctamente
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case BTNMENU_SALIR: // Si tienes un botón de salir o algo similar
            EndDialog(hwnd, 0);  // Cierra la ventana de dashboard
            break;

        case BTNMENU_SALIR_CERRARSESI: {
            usuarioLogueado = 0;
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_LOGIN), hwnd, fVentanaLogin);
            break;
        }
        case BTNMENU_REGISTRAR_CLIENTES: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_RC), hwnd, fVentanaRClientes);
            break;
        }
        case BTNMENU_EDITAR_CLIENTES: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_RC), hwnd, fVentanaEClientes);
        }

        }
        
        break;

    case WM_DESTROY:
        PostQuitMessage(9);
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK fVentanaRClientes(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_INITDIALOG: {
        // Cargar y asignar el menú (si es necesario y aplicable para un diálogo)
        HMENU hMenu = LoadMenu((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDR_MENU3));
        if (hMenu) {
            SetMenu(hwnd, hMenu);
        }
        return TRUE; // Devuelve TRUE para indicar que se ha inicializado correctamente
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case BTNMENU_SALIR: // Si tienes un botón de salir o algo similar
            EndDialog(hwnd, 0);  // Cierra la ventana de dashboard
            break;

        case BTNMENU_SALIR_CERRARSESI: {
            usuarioLogueado = 0;
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_LOGIN), hwnd, fVentanaLogin);
        }break;

        case BTNMENU_EDITAR_CLIENTES: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_CLIENTE), hwnd, fVentanaEClientes);
            break;
        }

        case BTN_RC_REGISTRAR: {
            // Obtener el texto de los cuadros de texto

            int idUsuarioActualizo = usuarioLogueado;
            char nombre[30];
            char apellidoPaterno[30];
            char apellidoMaterno[30];
            char direccion[30];
            char usuario[30];
            char contrasenia[30];

            GetDlgItemText(hwnd, TXT_RC_NOMBRE, nombre, sizeof(nombre));
            GetDlgItemText(hwnd, TXT_RC_APELLIDOP, apellidoPaterno, sizeof(apellidoPaterno));
            GetDlgItemText(hwnd, TXT_RC_APELLIDOM, apellidoMaterno, sizeof(apellidoMaterno));
            GetDlgItemText(hwnd, TXT_RC_DIRECCION, direccion, sizeof(direccion));
            GetDlgItemText(hwnd, TXT_RC_USUARIO, usuario, sizeof(usuario));
            GetDlgItemText(hwnd, TXT_RC_CONTRA, contrasenia, sizeof(contrasenia));

            if (strlen(nombre) == 0 || strlen(apellidoPaterno) == 0 || strlen(usuario) == 0 || strlen(contrasenia) == 0) {
                MessageBox(hwnd, "Todos los campos son obligatorios", "Error", MB_OK);
                break;
            }
            ultimoID++;
            raiz = agregarCliente(raiz, ultimoID, nombre, apellidoPaterno, apellidoMaterno, usuario, contrasenia, "Activo", obtenerFechaActual(), idUsuarioActualizo);

            // Verificar si el cliente fue agregado correctamente
            if (raiz != nullptr) 
                MessageBox(hwnd, "Cliente agregado exitosamente.", "Éxito", MB_OK | MB_ICONINFORMATION);
            else 
                MessageBox(hwnd, "Hubo un error al agregar el cliente.", "Error", MB_OK | MB_ICONERROR);


        }break;
        }

        break;

    case WM_DESTROY:
        PostQuitMessage(9);
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK fVentanaEClientes(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_INITDIALOG: {
        // Cargar y asignar el menú (si es necesario y aplicable para un diálogo)
        HMENU hMenu = LoadMenu((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDR_MENU3));
        if (hMenu) {
            SetMenu(hwnd, hMenu);
        }
        return TRUE; // Devuelve TRUE para indicar que se ha inicializado correctamente
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case BTNMENU_SALIR: // Si tienes un botón de salir o algo similar
            EndDialog(hwnd, 0);  // Cierra la ventana de dashboard
            break;

        case BTNMENU_SALIR_CERRARSESI: {
            usuarioLogueado = 0;
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_LOGIN), hwnd, fVentanaLogin);
        }break;

        case BTNMENU_EDITAR_CLIENTES: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_CLIENTE), hwnd, fVentanaEClientes);
        }break;
        
        case BTN_EDITAR_BUSCAR: {
            char usuario[20];
            GetDlgItemText(hwnd, TXT_EDITAR_BUSCAR, usuario, sizeof(usuario));

            Cliente* cliente = buscarClientePorUsuario(raiz, usuario);

            if (cliente != nullptr) {
                // Colocar la información del cliente en los cuadros de texto
                SetDlgItemText(hwnd, TXT_EDITAR_NOMBRE, cliente->nombre.c_str());
                SetDlgItemText(hwnd, TXT_EDITAR_APELLIDOP, cliente->apellidoPaterno.c_str());
                SetDlgItemText(hwnd, TXT_EDITAR_APELLIDOM, cliente->apellidoMaterno.c_str());
                SetDlgItemText(hwnd, TXT_EDITAR_USUARIO, cliente->usuario.c_str());
                SetDlgItemText(hwnd, TXT_EDITAR_CONTRA, cliente->contrasenia.c_str());

                // Comprobar el estatus y marcar el botón de radio si es "Activo"
                if (cliente->estatus == "Activo") {
                    CheckRadioButton(hwnd, RD_EDITAR_ACTIVO, RD_EDITAR_SUSPENDIDO, RD_EDITAR_ACTIVO);
                }
                else {
                    CheckRadioButton(hwnd, RD_EDITAR_ACTIVO, RD_EDITAR_SUSPENDIDO, RD_EDITAR_SUSPENDIDO);
                }
            }
            else {
                MessageBox(hwnd, "Cliente no encontrado", "Error", MB_OK | MB_ICONERROR);
            }
        } break;


        }

        break;

    case WM_DESTROY:
        PostQuitMessage(9);
        break;
    }
    return FALSE;
}

//================================================================== FUNCIONES =====================================================================


//------------------------------------Gestion de Usuarios------------------------------------------------------------------
// Función para obtener la fecha actual
string obtenerFechaActual() {
    time_t t = time(0);
    struct tm* now = localtime(&t);
    char buffer[80];
    strftime(buffer, 80, "%Y-%m-%d", now);
    return buffer;
}

// Función para agregar un cliente al árbol
Cliente* agregarCliente(Cliente* raiz, int id, const string& nombre, const string& apellidoPaterno, const string& apellidoMaterno, const string& usuario, const string& contrasenia, const string& estatus, const string& fechaActualizo, int idUsuarioActualizo) {
    if (raiz == nullptr) {
        return new Cliente(id, nombre, apellidoPaterno, apellidoMaterno, usuario, contrasenia, estatus, fechaActualizo, idUsuarioActualizo);
    }

    if (id < raiz->id) {
        raiz->izquierda = agregarCliente(raiz->izquierda, id, nombre, apellidoPaterno, apellidoMaterno, usuario, contrasenia, estatus, fechaActualizo, idUsuarioActualizo);
    }

    else if (id > raiz->id) {
        raiz->derecha = agregarCliente(raiz->derecha, id, nombre, apellidoPaterno, apellidoMaterno, usuario, contrasenia, estatus, fechaActualizo, idUsuarioActualizo);
    }

    else {
        return raiz; // Puedes retornar aquí para evitar sobreescribir nada más
    }

    return raiz;
}

// Función para buscar un cliente en el árbol
Cliente* buscarCliente(Cliente* raiz, int id) {
    if (raiz == nullptr || raiz->id == id) {
        return raiz;
    }
    if (id < raiz->id) {
        return buscarCliente(raiz->izquierda, id);
    }
    else {
        return buscarCliente(raiz->derecha, id);
    }
}

// Función para mostrar los clientes en orden en un ListBox (solo nombre, apellido paterno y apellido materno)
void mostrarClientesEnOrden(Cliente* raiz, HWND hwndListBox) {
    if (raiz == nullptr) return;  // Si el árbol está vacío, no hace nada

    // Llamada recursiva para recorrer el subárbol izquierdo
    mostrarClientesEnOrden(raiz->izquierda, hwndListBox);

    // Crear el texto que se va a mostrar en el ListBox
    string clienteInfo = to_string(raiz->id) + " - " + raiz->nombre + " " + raiz->apellidoPaterno;

    // Convertir `clienteInfo` a un `const char*` para usarlo en WinAPI
    SendMessage(hwndListBox, LB_ADDSTRING, 0, (LPARAM)clienteInfo.c_str());

    // Llamada recursiva para recorrer el subárbol derecho
    mostrarClientesEnOrden(raiz->derecha, hwndListBox);
}


// Función para cambiar el estatus de un cliente
void cambiarEstatus(Cliente* raiz, int id, const string& nuevoEstatus) {
    Cliente* cliente = buscarCliente(raiz, id);
    if (cliente == nullptr) {
        cout << "Cliente no encontrado.\n";
        return;
    }
    cliente->estatus = nuevoEstatus;
    cliente->fechaCambioEstatus = obtenerFechaActual();
    cliente->idUsuarioActualizo = usuarioLogueado;
    cout << "Estatus cambiado exitosamente.\n";
}

// Función para liberar la memoria del árbol
void eliminarArbolClientes(Cliente* raiz) {
    if (raiz == nullptr) return;
    eliminarArbolClientes(raiz->izquierda);  // Liberar subárbol izquierdo
    eliminarArbolClientes(raiz->derecha);    // Liberar subárbol derecho
    delete raiz;                     // Eliminar el nodo actual
}

// Función para guardar los datos en un archivo binario
void guardarDatos(Cliente* raiz, ofstream& archivo) {
    if (raiz == nullptr) return;

    // Guardar el ID del cliente
    archivo.write(reinterpret_cast<char*>(&raiz->id), sizeof(raiz->id));

    // Guardar el nombre del cliente
    size_t nombreSize = raiz->nombre.size();
    archivo.write(reinterpret_cast<char*>(&nombreSize), sizeof(nombreSize));
    archivo.write(raiz->nombre.c_str(), nombreSize);

    // Guardar el nombre del cliente
    size_t apellidoPaternoSize = raiz->apellidoPaterno.size();
    archivo.write(reinterpret_cast<char*>(&apellidoPaternoSize), sizeof(apellidoPaternoSize));
    archivo.write(raiz->apellidoPaterno.c_str(), apellidoPaternoSize);

    // Guardar el nombre del cliente
    size_t apellidoMaternoSize = raiz->apellidoMaterno.size();
    archivo.write(reinterpret_cast<char*>(&apellidoMaternoSize), sizeof(apellidoMaternoSize));
    archivo.write(raiz->apellidoMaterno.c_str(), apellidoMaternoSize);

    // Guardar el nombre del cliente
    size_t usuarioSize = raiz->usuario.size();
    archivo.write(reinterpret_cast<char*>(&usuarioSize), sizeof(usuarioSize));
    archivo.write(raiz->usuario.c_str(), usuarioSize);

    // Guardar el nombre del cliente
    size_t contraSize = raiz->contrasenia.size();
    archivo.write(reinterpret_cast<char*>(&contraSize), sizeof(contraSize));
    archivo.write(raiz->contrasenia.c_str(), contraSize);

    // Guardar el estatus del cliente
    size_t estatusSize = raiz->estatus.size();
    archivo.write(reinterpret_cast<char*>(&estatusSize), sizeof(estatusSize));
    archivo.write(raiz->estatus.c_str(), estatusSize);

    // Guardar la fecha de registro
    archivo.write(raiz->fechaRegistro.c_str(), raiz->fechaRegistro.size());
    archivo.put('\0');  // Añadir el terminador nulo

    // Guardar la fecha de cambio de estatus
    archivo.write(raiz->fechaCambioEstatus.c_str(), raiz->fechaCambioEstatus.size());
    archivo.put('\0');  // Añadir el terminador nulo

    archivo.write(reinterpret_cast<char*>(&raiz->idUsuarioActualizo), sizeof(raiz->idUsuarioActualizo));

    // Recursión para guardar los subárboles izquierdo y derecho
    guardarDatos(raiz->izquierda, archivo);
    guardarDatos(raiz->derecha, archivo);
}

Cliente* cargarDatos(ifstream& archivo, Cliente* raiz) {
    int id;

    // Intenta leer el ID del cliente
    if (!archivo.read(reinterpret_cast<char*>(&id), sizeof(id))) {
        return raiz;  // Retorna si no se puede leer más (final del archivo)
    }
    if (id > ultimoID) {
        ultimoID = id;
    }
    // Crea un nuevo cliente y asigna su ID
    Cliente* nuevoCliente = new Cliente();
    nuevoCliente->id = id;

    // Leer el nombre del cliente
    size_t nombreSize;
    if (!archivo.read(reinterpret_cast<char*>(&nombreSize), sizeof(nombreSize))) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }
    nuevoCliente->nombre.resize(nombreSize);
    if (!archivo.read(&nuevoCliente->nombre[0], nombreSize)) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }

    // Leer el apellido paterno del cliente
    size_t apellidoPaternoSize;
    if (!archivo.read(reinterpret_cast<char*>(&apellidoPaternoSize), sizeof(apellidoPaternoSize))) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }
    nuevoCliente->apellidoPaterno.resize(apellidoPaternoSize);
    if (!archivo.read(&nuevoCliente->apellidoPaterno[0], apellidoPaternoSize)) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }

    // Leer el apellido materno del cliente
    size_t apellidoMaternoSize;
    if (!archivo.read(reinterpret_cast<char*>(&apellidoMaternoSize), sizeof(apellidoMaternoSize))) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }
    nuevoCliente->apellidoMaterno.resize(apellidoMaternoSize);
    if (!archivo.read(&nuevoCliente->apellidoMaterno[0], apellidoMaternoSize)) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }

    // Leer el usuario del cliente
    size_t usuarioSize;
    if (!archivo.read(reinterpret_cast<char*>(&usuarioSize), sizeof(usuarioSize))) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }
    nuevoCliente->usuario.resize(usuarioSize);
    if (!archivo.read(&nuevoCliente->usuario[0], usuarioSize)) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }

    // Leer el contrasenia del cliente
    size_t contraSize;
    if (!archivo.read(reinterpret_cast<char*>(&contraSize), sizeof(contraSize))) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }
    nuevoCliente->contrasenia.resize(contraSize);
    if (!archivo.read(&nuevoCliente->contrasenia[0], contraSize)) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }

    // Leer el estatus del cliente
    size_t estatusSize;
    if (!archivo.read(reinterpret_cast<char*>(&estatusSize), sizeof(estatusSize))) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }
    nuevoCliente->estatus.resize(estatusSize);
    if (!archivo.read(&nuevoCliente->estatus[0], estatusSize)) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }

    // Leer fechas
    getline(archivo, nuevoCliente->fechaRegistro, '\0');
    getline(archivo, nuevoCliente->fechaCambioEstatus, '\0');

    // Leer el idUsuarioActualizo
    if (!archivo.read(reinterpret_cast<char*>(&nuevoCliente->idUsuarioActualizo), sizeof(nuevoCliente->idUsuarioActualizo))) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }

    // Agregar cliente al árbol
    raiz = agregarCliente(raiz, nuevoCliente->id, nuevoCliente->nombre, nuevoCliente->apellidoPaterno, nuevoCliente->apellidoMaterno, nuevoCliente->usuario, nuevoCliente->contrasenia, nuevoCliente->estatus, nuevoCliente->fechaCambioEstatus, nuevoCliente->idUsuarioActualizo);

    // Leer los subárboles izquierdo y derecho
    raiz = cargarDatos(archivo, raiz); // Asegúrate de que esto mantenga la referencia correcta a la raíz

    return raiz;  // Retorna la raíz del árbol actualizado
}

Cliente* buscarClientePorID(Cliente* raiz, int clienteID) {
    if (raiz == nullptr) {
        return nullptr;  // Si el árbol está vacío, no hay cliente
    }

    if (raiz->id == clienteID) {
        return raiz;  // Si el ID coincide, se encontró el cliente
    }
    else if (clienteID < raiz->id) {
        return buscarClientePorID(raiz->izquierda, clienteID);  // Búsqueda en el subárbol izquierdo
    }
    else {
        return buscarClientePorID(raiz->derecha, clienteID);  // Búsqueda en el subárbol derecho
    }
}

Cliente* buscarClientePorUsuario(Cliente* raiz, const string& usuario) {
    // Caso base: Si el árbol está vacío, no hay cliente que buscar
    if (raiz == nullptr) {
        return nullptr;
    }

    // Si encontramos el cliente con el usuario solicitado, lo regresamos
    if (raiz->usuario == usuario) {
        return raiz;
    }

    // Si no lo encontramos, seguimos buscando en los subárboles
    // Primero en el subárbol izquierdo
    Cliente* resultado = buscarClientePorUsuario(raiz->izquierda, usuario);

    // Si no lo encontramos en el subárbol izquierdo, buscamos en el subárbol derecho
    if (resultado == nullptr) {
        resultado = buscarClientePorUsuario(raiz->derecha, usuario);
    }

    return resultado;
}

