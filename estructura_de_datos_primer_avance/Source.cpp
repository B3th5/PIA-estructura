#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include "resource.h"
#include <fstream>
#include <iostream>
#include <cstring>

NodoUsuario* usuarioActual = nullptr;


INT_PTR CALLBACK fVentanaLogin(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, PSTR cmdline, int nCmdShow) {

    // Crear la ventana de login
    HWND hVentanaLogin = CreateDialog(hInstance, MAKEINTRESOURCE(DLG_LOGIN), NULL, fVentanaLogin);

    ShowWindow(hVentanaLogin, SW_SHOW);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}


INT_PTR CALLBACK fVentanaLogin(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(9);
        break;
    }
    return FALSE;
}

