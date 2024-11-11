#pragma once
#include "bibliotecasC++.h"

struct Nombre;
struct NodoUsuario;

// Generales:
struct Nombre {
    string primerNombre;
    string apellidoPaterno;
    string apellidoMaterno;
};

// Usuarios:
struct NodoUsuario {
    string nombreUsuario;
    string correoElectronico;
    string contrasena;
    Nombre nombreCompleto;
    SYSTEMTIME fechaNacimiento{ 0 };
    string genero;
    string fotografia;
    string tipoUsuario;
    SYSTEMTIME fechaHoraDeRegistro{ 0 };
    NodoUsuario* siguiente = nullptr;
    NodoUsuario* anterior = nullptr;
};
