#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include "resource.h"

#include <fstream>
#include <iostream>
#include <cstring>
#include <ctime>
#include <string>
#include <vector>
#include <cctype>

using namespace std;

int ultimoID = 0;
int ultimoIDTienda = 0;

int usuarioLogueado = 0;

int idClienteEditar = 0;
int idTiendaEditar = 0;

// Función para obtener la fecha actual
string obtenerFechaActual();
bool esNumero(const char* cadena);
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

//Estructura de teindas (listas doblemente ligadas)
struct Tienda {
    int id;                       // ID único de la tienda
    string nombre;                // Nombre de la tienda
    string direccion;             // Dirección de la tienda
    string estatus;               // Estatus de la tienda ("Activa", "Inactiva", etc.)
    string fechaCambioEstatus;    // Fecha de cambio de estatus
    Tienda* anterior;             // Puntero al nodo anterior en la lista doblemente ligada
    Tienda* siguiente;            // Puntero al siguiente nodo en la lista doblemente ligada

    // Constructor de Tienda
    Tienda(int id, const string& nombre, const string& direccion)
        : id(id), nombre(nombre), direccion(direccion), estatus("Activa"),
        fechaCambioEstatus(obtenerFechaActual()), anterior(nullptr), siguiente(nullptr) {}
};

// Estructura de Producto (listas doblemente ligadas)
struct Producto {
    string codigo;                   // Código único del producto
    string nombre;                // Nombre del producto
    double precio;                // Precio del producto
    double costo;                 // Costo del producto
    int existencias;              // Cantidad en existencia
    int tienda;                     // id a la tienda que ofrece el producto
    string estatus;               // Estatus del producto ("Disponible", "No disponible", etc.)
    string fechaCambioEstatus;    // Fecha de cambio de estatus
    Producto* anterior;           // Puntero al nodo anterior en la lista doblemente ligada
    Producto* siguiente;          // Puntero al siguiente nodo en la lista doblemente ligada

    // Constructor de Producto
    Producto(const string& codigo, const string& nombre, double precio, double costo, int existencias, int tienda)
        : codigo(codigo), nombre(nombre), precio(precio), costo(costo), existencias(existencias),
        tienda(tienda), estatus("Disponible"), fechaCambioEstatus(obtenerFechaActual()),
        anterior(nullptr), siguiente(nullptr) {
        // Validaciones
        if (precio < 0 || costo < 0 || existencias < 0) {
            MessageBox(nullptr, "El precio, costo y existencias no pueden ser negativos", "Error", MB_OK | MB_ICONERROR);
            throw std::invalid_argument("Los valores no pueden ser negativos");
        }
        if (nombre.empty()) {
            MessageBox(nullptr, "El nombre del producto no puede estar vacío", "Error", MB_OK | MB_ICONERROR);
            throw std::invalid_argument("El nombre no puede estar vacío");
        }
    }
};

// Función para agregar un cliente al árbol
Cliente* agregarCliente(Cliente* raiz, int id, const string& nombre, const string& apellidoPaterno, const string& apellidoMaterno, const string& usuario, const string& contrasenia, const string& estatus, const string& fechaActualizo, int idUsuarioActualizo);

// Función para buscar un cliente en el árbol
Cliente* buscarCliente(Cliente* raiz, int id);

// Función para mostrar los clientes en orden (ID en orden ascendente)
void mostrarClientesEnOrden(Cliente* raiz, HWND hwndListBox);

// Función para cambiar el estatus de un cliente
void editarCliente(Cliente* raiz, int id, const string& nuevoNombre, const string& nuevoApellidoPaterno, const string& nuevoApellidoMaterno, const string& nuevoUsuario, const string& nuevaContrasenia, const string& nuevoEstatus);

// Función para liberar la memoria del árbol
void eliminarArbolClientes(Cliente* raiz);

// Función para guardar los datos en un archivo binario
void guardarDatos(Cliente* raiz, ofstream& archivo);

Cliente* cargarDatos(ifstream& archivo, Cliente* raiz);

Cliente* buscarClientePorID(Cliente* raiz, int clienteID);

Cliente* buscarClientePorUsuario(Cliente* raiz, const string& usuario);

Cliente* raiz = nullptr;



// Función para guardar tiendas en un archivo binario
void guardarTiendas(Tienda* cabeza, const string& nombreArchivo);

// Función para cargar tiendas desde un archivo binario
Tienda* cargarTiendas(const string& nombreArchivo);

void agregarTienda(Tienda*& cabeza, int id, const string& nombre, const string& direccion);

void eliminarTiendas(Tienda*& cabeza);

void mostrarTiendasEnListBox(Tienda* cabeza, HWND hwndListBox);

void actualizarTienda(Tienda*& cabeza, int idTiendaEditar, HWND hwnd);

void cargarTiendasEnComboBox(HWND hwndComboBox, Tienda* cabezaTiendas);

Tienda* cabezaTienda = cargarTiendas("tiendas.bin");

void agregarProducto(Producto*& cabeza, const string& codigo, const string& nombre, double precio, double costo, int existencias, int tienda);
void liberarProductos(Producto*& cabeza);
void guardarProductos(Producto* cabeza, const string& nombreArchivo);
Producto* cargarProductos(const string& nombreArchivo);
void mostrarProductosEnListBox(Producto* cabeza, HWND hwndListBox);

Producto* cabezaProductos = cargarProductos("productos.bin");

INT_PTR CALLBACK fVentanaLogin(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK fVentanaDashboard(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK fVentanaRClientes(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK fVentanaEClientes(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK fVentanaRTienda(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK fVentanaETienda(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK fVentanaRProductos(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

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

    guardarTiendas(cabezaTienda, "tiendas.bin");

    guardarProductos(cabezaProductos, "productos.bin");

    //Liberar memoria de productos
    liberarProductos(cabezaProductos);

    // Liberar memoria de las tiendas al cerrar el programa
    eliminarTiendas(cabezaTienda);

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
        HWND hwndListBox = GetDlgItem(hwnd, LIS_DASHBOARD_CLIENTES);  
        mostrarClientesEnOrden(raiz, hwndListBox);

        HWND hwndListBoxTiendas = GetDlgItem(hwnd, LIST_DASHBOARD_TIENDAS);
        mostrarTiendasEnListBox(cabezaTienda, hwndListBoxTiendas);

        HWND hwndListBoxProductos = GetDlgItem(hwnd, LIST_DASHBOARD_PROD);
        mostrarProductosEnListBox(cabezaProductos, hwndListBoxProductos);

        return TRUE; // Devuelve TRUE para indicar que se ha inicializado correctamente
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case BTNMENU_REGISTRAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_PRODUCTOS), hwnd, fVentanaRProductos);
        }break;
        case BTNMENU_EDITAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_TIENDA), hwnd, fVentanaETienda);
        }break;
        case BTNMENU_REGISTRAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_TIENDA), hwnd, fVentanaRTienda);
        }break;
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
        case BTNMENU_DASHBOARD: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHBOARD), hwnd, fVentanaDashboard);
            break;
        }
        case BTNMENU_EDITAR_CLIENTES: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_CLIENTE), hwnd, fVentanaEClientes);
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
        case BTNMENU_REGISTRAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_PRODUCTOS), hwnd, fVentanaRProductos);
        }break;
        case BTNMENU_EDITAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_TIENDA), hwnd, fVentanaETienda);
        }break;
        case BTNMENU_REGISTRAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_TIENDA), hwnd, fVentanaRTienda);
        }break;
        case BTNMENU_DASHBOARD: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHBOARD), hwnd, fVentanaDashboard);
            break;
        }
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
        case BTNMENU_REGISTRAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_PRODUCTOS), hwnd, fVentanaRProductos);
        }break;
        case BTNMENU_EDITAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_TIENDA), hwnd, fVentanaETienda);
        }break;
        case BTNMENU_REGISTRAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_TIENDA), hwnd, fVentanaRTienda);
        }break;
        case BTNMENU_DASHBOARD: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHBOARD), hwnd, fVentanaDashboard);
            break;
        }
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

            idClienteEditar = 0;
            Cliente* clienteEditar = buscarClientePorUsuario(raiz, usuario);

            if (clienteEditar != nullptr) {
                idClienteEditar = clienteEditar->id;
                // Colocar la información del cliente en los cuadros de texto
                SetDlgItemText(hwnd, TXT_EDITAR_NOMBRE, clienteEditar->nombre.c_str());
                SetDlgItemText(hwnd, TXT_EDITAR_APELLIDOP, clienteEditar->apellidoPaterno.c_str());
                SetDlgItemText(hwnd, TXT_EDITAR_APELLIDOM, clienteEditar->apellidoMaterno.c_str());
                SetDlgItemText(hwnd, TXT_EDITAR_USUARIO, clienteEditar->usuario.c_str());
                SetDlgItemText(hwnd, TXT_EDITAR_CONTRA, clienteEditar->contrasenia.c_str());

                // Comprobar el estatus y marcar el botón de radio si es "Activo"
                if (clienteEditar->estatus == "Activo") {
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

        case BTN_EDITAR_MODIFICAR: {
            // Obtener el texto de los cuadros de texto
            int idUsuarioActualizo = usuarioLogueado;  // ID del usuario que realiza la actualización
            char nombre[30];
            char apellidoPaterno[30];
            char apellidoMaterno[30];
            char usuario[30];
            char contrasenia[30];

            // Obteniendo los textos desde los cuadros de texto de la interfaz
            GetDlgItemText(hwnd, TXT_EDITAR_NOMBRE, nombre, sizeof(nombre));
            GetDlgItemText(hwnd, TXT_EDITAR_APELLIDOP, apellidoPaterno, sizeof(apellidoPaterno));
            GetDlgItemText(hwnd, TXT_EDITAR_APELLIDOM, apellidoMaterno, sizeof(apellidoMaterno));
            GetDlgItemText(hwnd, TXT_EDITAR_USUARIO, usuario, sizeof(usuario));
            GetDlgItemText(hwnd, TXT_EDITAR_CONTRA, contrasenia, sizeof(contrasenia));

            // Validar que los campos no estén vacíos
            if (strlen(nombre) == 0 || strlen(apellidoPaterno) == 0 || strlen(usuario) == 0 || strlen(contrasenia) == 0) {
                MessageBox(hwnd, "Todos los campos son obligatorios", "Error", MB_OK);
                break;
            }

            // Obtener el estatus seleccionado de los Radio Buttons
            string nuevoEstatus;
            if (IsDlgButtonChecked(hwnd, RD_EDITAR_ACTIVO) == BST_CHECKED) {
                nuevoEstatus = "Activo";
            }
            else if (IsDlgButtonChecked(hwnd, RD_EDITAR_SUSPENDIDO) == BST_CHECKED) {
                nuevoEstatus = "Inactivo";
            }
            else {
                MessageBox(hwnd, "Seleccione un estatus.", "Error", MB_OK);
                break;
            }

            // Convertir los char* a string para llamar a la función editarCliente
            editarCliente(raiz, idClienteEditar, string(nombre), string(apellidoPaterno), string(apellidoMaterno), string(usuario), string(contrasenia), nuevoEstatus);

            MessageBox(hwnd, "Cliente editado exitosamente.", "Éxito", MB_OK | MB_ICONINFORMATION);
        } break;



        }

        break;

    case WM_DESTROY:
        PostQuitMessage(9);
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK fVentanaRTienda(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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
        case BTNMENU_REGISTRAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_PRODUCTOS), hwnd, fVentanaRProductos);
        }break;
        case BTNMENU_EDITAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_TIENDA), hwnd, fVentanaETienda);
        }break;
        case BTNMENU_DASHBOARD: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHBOARD), hwnd, fVentanaDashboard);
            break;
        }
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

        case BTNMENU_REGISTRAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_TIENDA), hwnd, fVentanaRTienda);
        }break;

        case BTN_RT_REGISTRAR: {
            
            char nombre[30];
            char direccion[30];

            GetDlgItemText(hwnd, TXT_RT_NOMBRE, nombre, sizeof(nombre));
            GetDlgItemText(hwnd, TXT_RT_DIREC, direccion, sizeof(direccion));

            if (strlen(nombre) == 0 || strlen(direccion) == 0) {
                MessageBox(hwnd, "Todos los campos son obligatorios", "Error", MB_OK);
                break;
            }
            ultimoIDTienda++;

            agregarTienda(cabezaTienda, ultimoIDTienda, nombre, direccion);

        }break;

        }

        break;

    case WM_DESTROY:
        PostQuitMessage(9);
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK fVentanaETienda(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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

        HWND hwndListBoxTiendas = GetDlgItem(hwnd, LIST_EDITART_TIENDA);
        mostrarTiendasEnListBox(cabezaTienda, hwndListBoxTiendas);
        return TRUE; // Devuelve TRUE para indicar que se ha inicializado correctamente
    }

    case WM_COMMAND:
        if (HIWORD(wParam) == LBN_SELCHANGE) { // Detectar cambio de selección en el ListBox
            HWND hwndListBox = GetDlgItem(hwnd, LIST_EDITART_TIENDA);
            int index = (int)SendMessage(hwndListBox, LB_GETCURSEL, 0, 0); // Obtener el índice seleccionado

            if (index != LB_ERR) {
                // Recorre la lista de tiendas para encontrar la tienda correspondiente
                Tienda* actual = cabezaTienda;
                int contador = 0;
                while (actual != nullptr && contador < index) {
                    actual = actual->siguiente;
                    contador++;
                }

                if (actual != nullptr) {
                    idTiendaEditar = actual->id;
                    // Rellenar los cuadros de texto con la información de la tienda
                    SetDlgItemText(hwnd, TXT_EDITART_NOMBRE, actual->nombre.c_str());
                    SetDlgItemText(hwnd, TXT_EDITART_DIRECCION, actual->direccion.c_str());
                    SetDlgItemText(hwnd, TXT_EDITART_FECHACT, actual->fechaCambioEstatus.c_str());
                    // Configurar el estado del radio button basado en el estatus
                    if (actual->estatus == "Activa") {
                        CheckRadioButton(hwnd, RB_EDITART_ACTIVO, RB_EDITART_SUSP, RB_EDITART_ACTIVO); // Activa el botón "Activo"
                    }
                    else if (actual->estatus == "Suspendido") {
                        CheckRadioButton(hwnd, RB_EDITART_ACTIVO, RB_EDITART_SUSP, RB_EDITART_SUSP); // Activa el botón "Suspendido"
                    }
                    else {
                        // Opcional: Manejar otros estados si es necesario
                        CheckRadioButton(hwnd, RB_EDITART_ACTIVO, RB_EDITART_SUSP, 0); // Desactiva todos los radio buttons
                    }
                }
            }
        }
        switch (LOWORD(wParam)) {
        case BTNMENU_REGISTRAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_PRODUCTOS), hwnd, fVentanaRProductos);
        }break;
        case BTNMENU_DASHBOARD: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHBOARD), hwnd, fVentanaDashboard);
            break;
        }
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

        case BTNMENU_REGISTRAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_TIENDA), hwnd, fVentanaRTienda);
        }break;

        case BTNMENU_EDITAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_TIENDA), hwnd, fVentanaETienda);
        }break;
        case BTN_EDITART_MODIFICAR: {
            actualizarTienda(cabezaTienda, idTiendaEditar, hwnd);
        }break;

        }

        break;

    case WM_DESTROY:
        PostQuitMessage(9);
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK fVentanaRProductos(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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
        HWND hwndComboBox = GetDlgItem(hwnd, CMB_RP_TIENDAS); // Reemplaza IDC_COMBO_TIENDAS con el ID de tu comboBox

        cargarTiendasEnComboBox(hwndComboBox, cabezaTienda);

        return TRUE; // Devuelve TRUE para indicar que se ha inicializado correctamente
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case BTNMENU_DASHBOARD: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHBOARD), hwnd, fVentanaDashboard);
            break;
        }
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

        case BTNMENU_REGISTRAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_TIENDA), hwnd, fVentanaRTienda);
        }break;

        case BTNMENU_EDITAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_TIENDA), hwnd, fVentanaETienda);
        }break;

        case BTNMENU_REGISTRAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_PRODUCTOS), hwnd, fVentanaRProductos);
        }break;

        case BTN_RP_REGISTRAR: {
            char nombre[30];
            char codigo[30];
            char precio[30];
            char costo[30];
            char existencias[30];
            int tiendaID = -1; // Inicializamos con un valor inválido

            GetDlgItemText(hwnd, TXT_RP_NOMBRE, nombre, sizeof(nombre));
            GetDlgItemText(hwnd, TXT_RP_CODIGO, codigo, sizeof(codigo));
            GetDlgItemText(hwnd, TXT_RP_PRECIO, precio, sizeof(precio));
            GetDlgItemText(hwnd, TXT_RP_COSTO, costo, sizeof(costo));
            GetDlgItemText(hwnd, TXT_RP_EXIS, existencias, sizeof(existencias));

            // Obtener el índice seleccionado en el comboBox
            HWND hwndComboBox = GetDlgItem(hwnd, CMB_RP_TIENDAS); // Reemplaza IDC_COMBO_TIENDAS con el ID de tu comboBox
            int indiceSeleccionado = SendMessage(hwndComboBox, CB_GETCURSEL, 0, 0);

            if (indiceSeleccionado != CB_ERR) {
                // Obtener el ID de la tienda asociado al índice seleccionado
                tiendaID = SendMessage(hwndComboBox, CB_GETITEMDATA, indiceSeleccionado, 0);
            }
            else {
                MessageBox(hwnd, "Por favor selecciona una tienda.", "Error", MB_OK);
                break;
            }
            // Validar que todos los campos obligatorios estén llenos
            if (strlen(nombre) == 0 || strlen(codigo) == 0 || strlen(precio) == 0 || strlen(costo) == 0 || strlen(existencias) == 0) {
                MessageBox(hwnd, "Todos los campos son obligatorios", "Error", MB_OK);
                break;
            }

            // Validar que los campos "precio", "costo" y "existencias" sean números
            if (!esNumero(precio) || !esNumero(costo) || !esNumero(existencias)) {
                MessageBox(hwnd, "Los campos 'precio', 'costo' y 'existencias' deben contener solo números.", "Error", MB_OK);
                break;
            }


            double precioDouble = std::stod(precio);
            double costoDouble = std::stod(costo);
            int existenciasInt = std::stoi(existencias);
            agregarProducto(cabezaProductos, codigo, nombre, precioDouble, costoDouble, existenciasInt, tiendaID);
            MessageBox(nullptr, "Se ha agregado la tienda correctamente", "Info", MB_ICONINFORMATION);
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
// Función para buscar un cliente en el árbol por ID
Cliente* buscarCliente(Cliente* raiz, int id) {
    // Si el árbol está vacío o hemos encontrado el cliente
    if (raiz == nullptr || raiz->id == id) {
        return raiz;  // Retorna el cliente encontrado o nullptr si no se encontró
    }

    // Si el ID a buscar es menor que el ID del nodo actual, busca en el subárbol izquierdo
    if (id < raiz->id) {
        return buscarCliente(raiz->izquierda, id);
    }

    // Si el ID a buscar es mayor que el ID del nodo actual, busca en el subárbol derecho
    return buscarCliente(raiz->derecha, id);
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

// Función para cambiar el estatus de un // Función para editar los detalles de un cliente
void editarCliente(Cliente* raiz, int id, const string& nuevoNombre, const string& nuevoApellidoPaterno, const string& nuevoApellidoMaterno, const string& nuevoUsuario, const string& nuevaContrasenia, const string& nuevoEstatus) {
    Cliente* cliente = buscarCliente(raiz, id);
    if (cliente == nullptr) {
        MessageBox(nullptr, "No se encontro el cliente", "Error", MB_ICONERROR);
        return;
    }

    // Actualizar los detalles del cliente
    cliente->nombre = nuevoNombre;
    cliente->apellidoPaterno = nuevoApellidoPaterno;
    cliente->apellidoMaterno = nuevoApellidoMaterno;
    cliente->usuario = nuevoUsuario;
    cliente->contrasenia = nuevaContrasenia;
    cliente->estatus = nuevoEstatus;
    cliente->fechaCambioEstatus = obtenerFechaActual();
    cliente->idUsuarioActualizo = usuarioLogueado;

    MessageBox(nullptr, "Cliente editado exitosamente", "Info", MB_ICONINFORMATION);
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


//------------------------------------Gestion de TIENDA------------------------------------------------------------------

// 1. Función para agregar una nueva tienda a la lista doblemente ligada
void agregarTienda(Tienda*& cabeza, int id, const string& nombre, const string& direccion) {
    // Validar que no exista un ID duplicado
    Tienda* actual = cabeza;
    while (actual != nullptr) {
        if (actual->id == id) {
            MessageBox(nullptr, "El ID esta repetido", "Error", MB_ICONERROR);
            return; // Salir si se encuentra un ID duplicado
        }
        actual = actual->siguiente;
    }

    // Crear una nueva tienda
    Tienda* nuevaTienda = new Tienda(id, nombre, direccion);

    if (cabeza == nullptr) {
        // Si la lista está vacía, la nueva tienda será la cabeza
        cabeza = nuevaTienda;
    }
    else {
        // Insertar al final de la lista
        actual = cabeza;
        while (actual->siguiente != nullptr) {
            actual = actual->siguiente;
        }
        actual->siguiente = nuevaTienda;
        nuevaTienda->anterior = actual;
    }
    MessageBox(nullptr, "Se ha agregado la tienda correctamente", "Info", MB_ICONINFORMATION);
}

void guardarTiendas(Tienda* cabeza, const string& nombreArchivo) {
    ofstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        MessageBox(nullptr, "No se pudo abrir el archivo de TIENDA", "Error", MB_ICONERROR);
        return;
    }

    Tienda* actual = cabeza;
    while (actual != nullptr) {
        archivo.write(reinterpret_cast<char*>(&actual->id), sizeof(actual->id));
        size_t nombreSize = actual->nombre.size();
        archivo.write(reinterpret_cast<char*>(&nombreSize), sizeof(nombreSize));
        archivo.write(actual->nombre.c_str(), nombreSize);

        size_t direccionSize = actual->direccion.size();
        archivo.write(reinterpret_cast<char*>(&direccionSize), sizeof(direccionSize));
        archivo.write(actual->direccion.c_str(), direccionSize);

        size_t estatusSize = actual->estatus.size();
        archivo.write(reinterpret_cast<char*>(&estatusSize), sizeof(estatusSize));
        archivo.write(actual->estatus.c_str(), estatusSize);

        // Aquí puedes guardar la fecha de cambio de estatus
        size_t fechaCambioSize = actual->fechaCambioEstatus.size();
        archivo.write(reinterpret_cast<char*>(&fechaCambioSize), sizeof(fechaCambioSize));
        archivo.write(actual->fechaCambioEstatus.c_str(), fechaCambioSize);

        actual = actual->siguiente; // Mover al siguiente nodo
    }

    archivo.close();
}

// Función para cargar tiendas desde un archivo binario
Tienda* cargarTiendas(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        MessageBox(nullptr, "No se pudo abrir el archivo TIENDAS", "Error", MB_ICONERROR);
        return nullptr;
    }

    Tienda* cabeza = nullptr;
    Tienda* cola = nullptr; // Para mantener el último nodo
    while (true) {
        Tienda* nuevaTienda = new Tienda(0, "", ""); // Crear un nuevo nodo temporal

        archivo.read(reinterpret_cast<char*>(&nuevaTienda->id), sizeof(nuevaTienda->id));
        if (archivo.eof()) {
            delete nuevaTienda; // Eliminar nodo temporal si se llegó al final
            break;
        }

        // Actualizar el último ID si el actual es mayor
        if (nuevaTienda->id > ultimoIDTienda) {
            ultimoIDTienda = nuevaTienda->id;
        }

        // Cargar nombre
        size_t nombreSize;
        archivo.read(reinterpret_cast<char*>(&nombreSize), sizeof(nombreSize));
        nuevaTienda->nombre.resize(nombreSize);
        archivo.read(&nuevaTienda->nombre[0], nombreSize);

        // Cargar direccion
        size_t direccionSize;
        archivo.read(reinterpret_cast<char*>(&direccionSize), sizeof(direccionSize));
        nuevaTienda->direccion.resize(direccionSize);
        archivo.read(&nuevaTienda->direccion[0], direccionSize);

        // Cargar estatus
        size_t estatusSize;
        archivo.read(reinterpret_cast<char*>(&estatusSize), sizeof(estatusSize));
        nuevaTienda->estatus.resize(estatusSize);
        archivo.read(&nuevaTienda->estatus[0], estatusSize);

        // Cargar fecha de cambio de estatus
        size_t fechaCambioSize;
        archivo.read(reinterpret_cast<char*>(&fechaCambioSize), sizeof(fechaCambioSize));
        nuevaTienda->fechaCambioEstatus.resize(fechaCambioSize);
        archivo.read(&nuevaTienda->fechaCambioEstatus[0], fechaCambioSize);

        // Enlazar en la lista
        nuevaTienda->siguiente = nullptr;
        nuevaTienda->anterior = cola; // Establecer puntero anterior
        if (cola != nullptr) {
            cola->siguiente = nuevaTienda; // Conectar el nodo anterior
        }
        else {
            cabeza = nuevaTienda; // Si es el primer nodo, establecer cabeza
        }
        cola = nuevaTienda; // Actualizar cola
    }

    archivo.close();
    return cabeza; // Retornar la cabeza de la lista
}

void eliminarTiendas(Tienda*& cabeza) {
    while (cabeza != nullptr) {
        Tienda* temp = cabeza;
        cabeza = cabeza->siguiente; // Mover la cabeza al siguiente nodo
        delete temp; // Liberar el nodo
    }
}

// Función para mostrar las tiendas en un ListBox (solo el nombre)
void mostrarTiendasEnListBox(Tienda* cabeza, HWND hwndListBox) {
    if (cabeza == nullptr) {
        SendMessage(hwndListBox, LB_ADDSTRING, 0, (LPARAM)"No hay tiendas");
        MessageBox(nullptr, "No hay tiendas para mostrar.", "Información", MB_ICONINFORMATION);
        return;
    }

    // Limpiar el ListBox antes de agregar nuevos elementos
    SendMessage(hwndListBox, LB_RESETCONTENT, 0, 0);

    Tienda* actual = cabeza;
    while (actual != nullptr) {
        // Formatear la cadena para mostrar el ID y el nombre de la tienda
        std::string tiendaInfo = "ID: " + std::to_string(actual->id) + " - " + actual->nombre;
        SendMessage(hwndListBox, LB_ADDSTRING, 0, (LPARAM)tiendaInfo.c_str());
        actual = actual->siguiente;
    }
}

void actualizarTienda(Tienda*& cabeza, int idTiendaEditar, HWND hwnd) {
    Tienda* tienda = cabeza;

    // Buscar la tienda con el ID proporcionado
    while (tienda != nullptr) {
        if (tienda->id == idTiendaEditar) {
            // Obtener los valores de los cuadros de texto
            char nombre[30], direccion[30];
            GetDlgItemText(hwnd, TXT_EDITART_NOMBRE, nombre, sizeof(nombre));
            GetDlgItemText(hwnd, TXT_EDITART_DIRECCION, direccion, sizeof(direccion));

            // Obtener el estatus del radio button
            bool estatusActivo = IsDlgButtonChecked(hwnd, RB_EDITART_ACTIVO);
            bool estatusSuspendido = IsDlgButtonChecked(hwnd, RB_EDITART_SUSP);

            // Verificar que los campos obligatorios no estén vacíos
            if (strlen(nombre) == 0 || strlen(direccion) == 0) {
                MessageBox(hwnd, "El nombre y la dirección son obligatorios", "Error", MB_OK | MB_ICONERROR);
                return;
            }

            // Actualizar los datos de la tienda
            tienda->nombre = nombre;
            tienda->direccion = direccion;

            // Actualizar el estatus según el radio button seleccionado
            if (estatusActivo) {
                tienda->estatus = "Activa";
            }
            else if (estatusSuspendido) {
                tienda->estatus = "Suspendido";
            }
            else {
                MessageBox(hwnd, "Debe seleccionar un estatus", "Error", MB_OK | MB_ICONERROR);
                return;
            }

            // Fecha de cambio de estatus (opcional, si quieres registrar cuándo se hizo la actualización)
            tienda->fechaCambioEstatus = obtenerFechaActual(); // Asumiendo que tienes una función para obtener la fecha

            // Mostrar mensaje de éxito
            MessageBox(hwnd, "La tienda se ha actualizado correctamente", "Éxito", MB_OK | MB_ICONINFORMATION);
            return;
        }

        tienda = tienda->siguiente; // Continuar con la siguiente tienda
    }

    // Si no se encuentra la tienda
    MessageBox(hwnd, "Tienda no encontrada", "Error", MB_OK | MB_ICONERROR);
}

// Función para cargar las tiendas en el comboBox
void cargarTiendasEnComboBox(HWND hwndComboBox, Tienda* cabezaTiendas) {
    // Limpiar el ComboBox antes de agregar nuevos elementos
    SendMessage(hwndComboBox, CB_RESETCONTENT, 0, 0);

    Tienda* actual = cabezaTiendas;
    while (actual != nullptr) {
        // Agregar el nombre de la tienda al ComboBox
        int indice = SendMessage(hwndComboBox, CB_ADDSTRING, 0, (LPARAM)actual->nombre.c_str());

        // Asociar el ID de la tienda con el elemento recién añadido
        SendMessage(hwndComboBox, CB_SETITEMDATA, indice, (LPARAM)actual->id);

        actual = actual->siguiente;
    }
}


int obtenerIdTiendaPorIndice(HWND hwndComboBox, Tienda* cabezaTiendas) {
    int indiceSeleccionado = SendMessage(hwndComboBox, CB_GETCURSEL, 0, 0);  // Obtener el índice seleccionado

    if (indiceSeleccionado == CB_ERR) {
        // Si no hay selección válida, retornar un valor inválido
        return -1;
    }

    // Asumimos que el índice corresponde al orden de las tiendas
    Tienda* actual = cabezaTiendas;
    int indiceActual = 0;

    while (actual != nullptr) {
        if (indiceActual == indiceSeleccionado) {
            // Devolver el ID de la tienda directamente
            return actual->id;
        }
        actual = actual->siguiente;
        indiceActual++;
    }

    return -1;
}


//------------------------------------Gestion de PRODUCTOS------------------------------------------------------------------

void agregarProducto(Producto*& cabeza, const string& codigo, const string& nombre, double precio, double costo, int existencias, int tienda) {
    if (existencias < 0) {
        MessageBox(nullptr, "No se puede agregar el producto. Las existencias no pueden ser negativas.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Verificar si el código ya existe
    Producto* actual = cabeza;
    while (actual != nullptr) {
        if (actual->codigo == codigo) {
            MessageBox(nullptr, "No se puede agregar el producto. Ya existe un producto con el mismo código.", "Error", MB_OK | MB_ICONERROR);
            return;
        }
        actual = actual->siguiente;
    }

    // Crear el nuevo producto
    Producto* nuevoProducto = new Producto(codigo, nombre, precio, costo, existencias, tienda);

    if (cabeza == nullptr) {
        // Si la lista está vacía, la nueva tienda será la cabeza
        cabeza = nuevoProducto;
    }
    else {
        // Insertar al final de la lista
        actual = cabeza;
        while (actual->siguiente != nullptr) {
            actual = actual->siguiente;
        }
        actual->siguiente = nuevoProducto;
        nuevoProducto->anterior = actual;
    }
}

void liberarProductos(Producto*& cabeza) {
    Producto* actual = cabeza;
    while (actual != nullptr) {
        Producto* siguiente = actual->siguiente; // Guardamos el siguiente nodo
        delete actual; // Liberamos la memoria del nodo actual
        actual = siguiente; // Avanzamos al siguiente nodo
    }
    cabeza = nullptr; // Aseguramos que la cabeza apunte a nullptr después de liberar
}

void guardarProductos(Producto* cabeza, const string& nombreArchivo) {
    ofstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        MessageBox(nullptr, "No se pudo abrir el archivo para guardar productos.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    Producto* actual = cabeza;
    while (actual != nullptr) {
        // Guardamos el código del producto
        size_t codigoSize = actual->codigo.size();
        archivo.write(reinterpret_cast<char*>(&codigoSize), sizeof(codigoSize));
        archivo.write(actual->codigo.c_str(), codigoSize);

        // Guardamos el nombre del producto
        size_t nombreSize = actual->nombre.size();
        archivo.write(reinterpret_cast<char*>(&nombreSize), sizeof(nombreSize));
        archivo.write(actual->nombre.c_str(), nombreSize);

        // Guardamos el precio y costo
        archivo.write(reinterpret_cast<char*>(&actual->precio), sizeof(actual->precio));
        archivo.write(reinterpret_cast<char*>(&actual->costo), sizeof(actual->costo));

        // Guardamos las existencias
        archivo.write(reinterpret_cast<char*>(&actual->existencias), sizeof(actual->existencias));

        // Guardamos el id de la tienda
        archivo.write(reinterpret_cast<char*>(&actual->tienda), sizeof(actual->tienda));

        // Guardamos el estatus
        size_t estatusSize = actual->estatus.size();
        archivo.write(reinterpret_cast<char*>(&estatusSize), sizeof(estatusSize));
        archivo.write(actual->estatus.c_str(), estatusSize);

        // Guardamos la fecha de cambio de estatus
        size_t fechaCambioEstatusSize = actual->fechaCambioEstatus.size();
        archivo.write(reinterpret_cast<char*>(&fechaCambioEstatusSize), sizeof(fechaCambioEstatusSize));
        archivo.write(actual->fechaCambioEstatus.c_str(), fechaCambioEstatusSize);

        // Avanzamos al siguiente producto
        actual = actual->siguiente;
    }

    archivo.close();
   
}


Producto* cargarProductos(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        MessageBox(nullptr, "No se pudo abrir el archivo de productos para cargar.", "Error", MB_OK | MB_ICONERROR);
        return nullptr;
    }

    Producto* cabeza = nullptr;
    Producto* cola = nullptr; // Para mantener el último nodo

    while (true) {
        // Creamos un nodo temporal para el producto
        Producto* nuevoProducto = nullptr;

        // Leemos el tamaño y código del producto
        size_t codigoSize;
        archivo.read(reinterpret_cast<char*>(&codigoSize), sizeof(codigoSize));
        if (archivo.eof()) break; // Si alcanzamos el final, terminamos la carga

        string codigo(codigoSize, '\0');
        archivo.read(&codigo[0], codigoSize);

        // Leemos el nombre del producto
        size_t nombreSize;
        archivo.read(reinterpret_cast<char*>(&nombreSize), sizeof(nombreSize));
        string nombre(nombreSize, '\0');
        archivo.read(&nombre[0], nombreSize);

        // Leemos precio, costo y existencias
        double precio, costo;
        int existencias, tienda;
        archivo.read(reinterpret_cast<char*>(&precio), sizeof(precio));
        archivo.read(reinterpret_cast<char*>(&costo), sizeof(costo));
        archivo.read(reinterpret_cast<char*>(&existencias), sizeof(existencias));
        archivo.read(reinterpret_cast<char*>(&tienda), sizeof(tienda));

        // Leemos el estatus
        size_t estatusSize;
        archivo.read(reinterpret_cast<char*>(&estatusSize), sizeof(estatusSize));
        string estatus(estatusSize, '\0');
        archivo.read(&estatus[0], estatusSize);

        // Leemos la fecha de cambio de estatus
        size_t fechaCambioEstatusSize;
        archivo.read(reinterpret_cast<char*>(&fechaCambioEstatusSize), sizeof(fechaCambioEstatusSize));
        string fechaCambioEstatus(fechaCambioEstatusSize, '\0');
        archivo.read(&fechaCambioEstatus[0], fechaCambioEstatusSize);

        // Creamos el nuevo producto
        try {
            nuevoProducto = new Producto(codigo, nombre, precio, costo, existencias, tienda);
            nuevoProducto->estatus = estatus;
            nuevoProducto->fechaCambioEstatus = fechaCambioEstatus;
        }
        catch (const std::invalid_argument&) {
            // Si el producto no es válido, continuamos con el siguiente
            continue;
        }

        // Añadimos el nuevo producto a la lista
        if (cabeza == nullptr) {
            cabeza = nuevoProducto;
            cola = nuevoProducto;
        }
        else {
            cola->siguiente = nuevoProducto;
            nuevoProducto->anterior = cola;
            cola = nuevoProducto;
        }
    }

    archivo.close();
    return cabeza;
}


void mostrarProductosEnListBox(Producto* cabeza, HWND hwndListBox) {
    if (cabeza == nullptr) {
        SendMessage(hwndListBox, LB_ADDSTRING, 0, (LPARAM)"No hay productos");
        MessageBox(nullptr, "No hay productos para mostrar.", "Información", MB_ICONINFORMATION);
        return;
    }

    // Limpiar el ListBox antes de agregar nuevos elementos
    SendMessage(hwndListBox, LB_RESETCONTENT, 0, 0);

    Producto* actual = cabeza;
    while (actual != nullptr) {
        // Formatear la cadena para mostrar la información del producto
        std::string productoInfo = (actual->codigo) + " " + actual->nombre;

        SendMessage(hwndListBox, LB_ADDSTRING, 0, (LPARAM)productoInfo.c_str());
        actual = actual->siguiente;
    }
}


bool esNumero(const char* cadena) {
    for (int i = 0; cadena[i] != '\0'; i++) {
        if (!isdigit(cadena[i]) && cadena[i] != '.') {
            return false; // Si el carácter no es un dígito o un punto decimal, no es un número
        }
    }
    return true;
}