#include "todasLasBibliotecas.h"
//---------------------------------------------------------------------Globales:
//LISTAS:
NodoUsuario* inicioListaUsuarios = nullptr;
NodoUsuario* finalListaUsuarios = nullptr;
NodoUsuario* auxiliarListaUsuarios = nullptr;
//---------------------------------------------------------------------

LRESULT CALLBACK cUSUARIOS(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {

	case WM_INITDIALOG: {
		// Llamando a la función y reasignando el inicio de la lista
		inicioListaUsuarios = ordenarUsuariosPorNombre(inicioListaUsuarios, &finalListaUsuarios);
		// Obtiene la referencia al componente Listbox en la interfaz de usuario una sola vez
		HWND hListbox = GetDlgItem(hwnd, USUARIOS_LISTBOX_LISTA_DE_USUARIOS);

		// Verifica si la lista de usuarios no está vacía y recorre cada nodo
		for (NodoUsuario* nodo = inicioListaUsuarios; nodo != nullptr; nodo = nodo->siguiente) {
			// Prepara el texto que se mostrará en el Listbox. Formato: "_<NombreUsuario>"
			string texto = "_" + nodo->nombreUsuario;

			// Agrega el texto al Listbox y asocia el nodo de usuario actual con el nuevo ítem
			int indice = SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)texto.c_str());
			SendMessage(hListbox, LB_SETITEMDATA, indice, (LPARAM)nodo);
		}

		// Limpieza y carga de datos de usuario actual si es necesario
		ZeroMemory(&horaActual, sizeof(SYSTEMTIME)); // Limpiar variable SYSTEMTIME (horaActual)
		auxiliarListaUsuarios = nullptr; //Limpiar puntero auxiliarListaUsuarios
		cargarUsuarioActual(hwnd, USUARIOS_STATICTEXT_USUARIO_ACTUAL, USUARIOS_PICTURECONTROL_USUARIO_ACTUAL);
	} break;

	case WM_COMMAND: {

		switch (LOWORD(wParam)) {

		case USUARIOS_LISTBOX_LISTA_DE_USUARIOS: {
			switch (HIWORD(wParam)) {
			case LBN_SELCHANGE: {
				// Obtiene el índice del elemento seleccionado en el Listbox
				int indice = SendDlgItemMessage(hwnd, USUARIOS_LISTBOX_LISTA_DE_USUARIOS, LB_GETCURSEL, 0, 0);

				// Recupera el puntero a la estructura de datos del usuario asociada al ítem seleccionado
				auxiliarListaUsuarios = (NodoUsuario*)SendDlgItemMessage(
					hwnd, USUARIOS_LISTBOX_LISTA_DE_USUARIOS, LB_GETITEMDATA, indice, 0);

				if (auxiliarListaUsuarios != NULL) {
					//Deshabilitar componentes de Alta de Usuarios, en el caso de que se seleccione un usuario del Listbox:
					EnableWindow(GetDlgItem(hwnd, USUARIOS_BUTTON_ALTA), FALSE);
					EnableWindow(GetDlgItem(hwnd, USUARIOS_EDITCONTROL_NOMBRE_DE_USUARIO), FALSE);
					EnableWindow(GetDlgItem(hwnd, USUARIOS_EDITCONTROL_CORREO_ELECTRONICO), FALSE);
					EnableWindow(GetDlgItem(hwnd, USUARIOS_EDITCONTROL_CONTRASENA), FALSE);
					EnableWindow(GetDlgItem(hwnd, USUARIOS_EDITCONTROL_NOMBRE), FALSE);
					EnableWindow(GetDlgItem(hwnd, USUARIOS_EDITCONTROL_APELLIDO_PATERNO), FALSE);
					EnableWindow(GetDlgItem(hwnd, USUARIOS_EDITCONTROL_APELLIDO_MATERNO), FALSE);
					EnableWindow(GetDlgItem(hwnd, USUARIOS_DATETIMEPICKER_FECHA_DE_NACIMIENTO), FALSE);
					EnableWindow(GetDlgItem(hwnd, USUARIOS_RADIOBUTTON_MASCULINO), FALSE);
					EnableWindow(GetDlgItem(hwnd, USUARIOS_RADIOBUTTON_FEMENINO), FALSE);
					EnableWindow(GetDlgItem(hwnd, USUARIOS_BUTTON_SUBIR), FALSE);
					EnableWindow(GetDlgItem(hwnd, USUARIOS_CHECKBOX_ADMINISTRADOR), FALSE);
					EnableWindow(GetDlgItem(hwnd, USUARIOS_EDITCONTROL_RUTA), FALSE);

					// Actualiza los campos de la interfaz con los datos del usuario seleccionado
					SetDlgItemText(hwnd, USUARIOS_EDITCONTROL_NOMBRE_DE_USUARIO, auxiliarListaUsuarios->nombreUsuario.c_str());
					SetDlgItemText(hwnd, USUARIOS_EDITCONTROL_CORREO_ELECTRONICO, auxiliarListaUsuarios->correoElectronico.c_str());
					SetDlgItemText(hwnd, USUARIOS_EDITCONTROL_CONTRASENA, auxiliarListaUsuarios->contrasena.c_str());
					SetDlgItemText(hwnd, USUARIOS_EDITCONTROL_NOMBRE, auxiliarListaUsuarios->nombreCompleto.primerNombre.c_str());
					SetDlgItemText(hwnd, USUARIOS_EDITCONTROL_APELLIDO_PATERNO, auxiliarListaUsuarios->nombreCompleto.apellidoPaterno.c_str());
					SetDlgItemText(hwnd, USUARIOS_EDITCONTROL_APELLIDO_MATERNO, auxiliarListaUsuarios->nombreCompleto.apellidoMaterno.c_str());

					// Configura el control DateTimePicker con la fecha de nacimiento del usuario
					SYSTEMTIME st;
					ZeroMemory(&st, sizeof(SYSTEMTIME));
					st = auxiliarListaUsuarios->fechaNacimiento;
					SendDlgItemMessage(hwnd, USUARIOS_DATETIMEPICKER_FECHA_DE_NACIMIENTO, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&st);

					// Configura el control de radio button según el género del usuario
					if (auxiliarListaUsuarios->genero == "Masculino") {
						SendDlgItemMessage(hwnd, USUARIOS_RADIOBUTTON_MASCULINO, BM_SETCHECK, BST_CHECKED, 0);
						SendDlgItemMessage(hwnd, USUARIOS_RADIOBUTTON_FEMENINO, BM_SETCHECK, BST_UNCHECKED, 0);
					}
					else {
						SendDlgItemMessage(hwnd, USUARIOS_RADIOBUTTON_FEMENINO, BM_SETCHECK, BST_CHECKED, 0);
						SendDlgItemMessage(hwnd, USUARIOS_RADIOBUTTON_MASCULINO, BM_SETCHECK, BST_UNCHECKED, 0);
					}

					// Selecciona el checkbox si el usuario seleccionado es administrador
					if (auxiliarListaUsuarios->tipoUsuario == "Administrador") {
						SendDlgItemMessage(hwnd, USUARIOS_CHECKBOX_ADMINISTRADOR, BM_SETCHECK, BST_CHECKED, 0);
					}
					else {
						SendDlgItemMessage(hwnd, USUARIOS_CHECKBOX_ADMINISTRADOR, BM_SETCHECK, BST_UNCHECKED, 0);
					}

					// Actualiza el campo de la ruta de la fotografía del usuario
					SetDlgItemText(hwnd, USUARIOS_EDITCONTROL_RUTA, auxiliarListaUsuarios->fotografia.c_str());
				}
				break; // Fin de LBN_SELCHANGE
			}
			}
			break; // Fin de USUARIOS_LISTBOX_LISTA_DE_USUARIOS
		}break;

		case USUARIOS_LISTBOX_BUSQUEDA: {
			switch (HIWORD(wParam)) {
			case LBN_SELCHANGE: {
				// Obtiene el índice del elemento seleccionado en el Listbox
				int indice = SendDlgItemMessage(hwnd, USUARIOS_LISTBOX_BUSQUEDA, LB_GETCURSEL, 0, 0);

				// Recupera el puntero a la estructura de datos del usuario asociada al ítem seleccionado
				auxiliarListaUsuarios = (NodoUsuario*)SendDlgItemMessage(
					hwnd, USUARIOS_LISTBOX_BUSQUEDA, LB_GETITEMDATA, indice, 0);

				if (auxiliarListaUsuarios != NULL) {
					//Deshabilitar componentes de Alta de Usuarios, en el caso de que se seleccione un usuario del Listbox:
					EnableWindow(GetDlgItem(hwnd, USUARIOS_BUTTON_ALTA), FALSE);
					EnableWindow(GetDlgItem(hwnd, USUARIOS_EDITCONTROL_NOMBRE_DE_USUARIO), FALSE);
					EnableWindow(GetDlgItem(hwnd, USUARIOS_EDITCONTROL_CORREO_ELECTRONICO), FALSE);
					EnableWindow(GetDlgItem(hwnd, USUARIOS_EDITCONTROL_CONTRASENA), FALSE);
					EnableWindow(GetDlgItem(hwnd, USUARIOS_EDITCONTROL_NOMBRE), FALSE);
					EnableWindow(GetDlgItem(hwnd, USUARIOS_EDITCONTROL_APELLIDO_PATERNO), FALSE);
					EnableWindow(GetDlgItem(hwnd, USUARIOS_EDITCONTROL_APELLIDO_MATERNO), FALSE);
					EnableWindow(GetDlgItem(hwnd, USUARIOS_DATETIMEPICKER_FECHA_DE_NACIMIENTO), FALSE);
					EnableWindow(GetDlgItem(hwnd, USUARIOS_RADIOBUTTON_MASCULINO), FALSE);
					EnableWindow(GetDlgItem(hwnd, USUARIOS_RADIOBUTTON_FEMENINO), FALSE);
					EnableWindow(GetDlgItem(hwnd, USUARIOS_BUTTON_SUBIR), FALSE);
					EnableWindow(GetDlgItem(hwnd, USUARIOS_CHECKBOX_ADMINISTRADOR), FALSE);
					EnableWindow(GetDlgItem(hwnd, USUARIOS_EDITCONTROL_RUTA), FALSE);

					// Actualiza los campos de la interfaz con los datos del usuario seleccionado
					SetDlgItemText(hwnd, USUARIOS_EDITCONTROL_NOMBRE_DE_USUARIO, auxiliarListaUsuarios->nombreUsuario.c_str());
					SetDlgItemText(hwnd, USUARIOS_EDITCONTROL_CORREO_ELECTRONICO, auxiliarListaUsuarios->correoElectronico.c_str());
					SetDlgItemText(hwnd, USUARIOS_EDITCONTROL_CONTRASENA, auxiliarListaUsuarios->contrasena.c_str());
					SetDlgItemText(hwnd, USUARIOS_EDITCONTROL_NOMBRE, auxiliarListaUsuarios->nombreCompleto.primerNombre.c_str());
					SetDlgItemText(hwnd, USUARIOS_EDITCONTROL_APELLIDO_PATERNO, auxiliarListaUsuarios->nombreCompleto.apellidoPaterno.c_str());
					SetDlgItemText(hwnd, USUARIOS_EDITCONTROL_APELLIDO_MATERNO, auxiliarListaUsuarios->nombreCompleto.apellidoMaterno.c_str());

					// Configura el control DateTimePicker con la fecha de nacimiento del usuario
					SYSTEMTIME st;
					ZeroMemory(&st, sizeof(SYSTEMTIME));
					st = auxiliarListaUsuarios->fechaNacimiento;
					SendDlgItemMessage(hwnd, USUARIOS_DATETIMEPICKER_FECHA_DE_NACIMIENTO, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&st);

					// Configura el control de radio button según el género del usuario
					if (auxiliarListaUsuarios->genero == "Masculino") {
						SendDlgItemMessage(hwnd, USUARIOS_RADIOBUTTON_MASCULINO, BM_SETCHECK, BST_CHECKED, 0);
						SendDlgItemMessage(hwnd, USUARIOS_RADIOBUTTON_FEMENINO, BM_SETCHECK, BST_UNCHECKED, 0);
					}
					else {
						SendDlgItemMessage(hwnd, USUARIOS_RADIOBUTTON_FEMENINO, BM_SETCHECK, BST_CHECKED, 0);
						SendDlgItemMessage(hwnd, USUARIOS_RADIOBUTTON_MASCULINO, BM_SETCHECK, BST_UNCHECKED, 0);
					}

					// Selecciona el checkbox si el usuario seleccionado es administrador
					if (auxiliarListaUsuarios->tipoUsuario == "Administrador") {
						SendDlgItemMessage(hwnd, USUARIOS_CHECKBOX_ADMINISTRADOR, BM_SETCHECK, BST_CHECKED, 0);
					}
					else {
						SendDlgItemMessage(hwnd, USUARIOS_CHECKBOX_ADMINISTRADOR, BM_SETCHECK, BST_UNCHECKED, 0);
					}

					// Actualiza el campo de la ruta de la fotografía del usuario
					SetDlgItemText(hwnd, USUARIOS_EDITCONTROL_RUTA, auxiliarListaUsuarios->fotografia.c_str());
				}
				break; // Fin de LBN_SELCHANGE
			}
			}
			break; // Fin de USUARIOS_LISTBOX_LISTA_DE_USUARIOS
		}break;

		case USUARIOS_BUTTON_BUSCAR: {
			HWND hEdit = GetDlgItem(hwnd, USUARIOS_EDITCONTROL_BUSQUEDA);  // IDC_EDIT_USUARIO es el ID del control de edición definido en los recursos de tu aplicación.
			HWND hListBox = GetDlgItem(hwnd, USUARIOS_LISTBOX_BUSQUEDA);  // IDC_LIST_USUARIOS es el ID del control List Box.
			buscarYMostrarUsuario(hEdit, hListBox);
		}break;

		case USUARIOS_BUTTON_ALTA: {
			// Obtener información del formulario
			char usuario_temp[50]{ 0 }, correo_temp[50]{ 0 }, contrasena_temp[50]{ 0 }, nombre_temp[50]{ 0 }, apellido_p_temp[50]{ 0 }, apellido_m_temp[50]{ 0 }, ruta_temp[200]{ 0 };
			GetDlgItemText(hwnd, USUARIOS_EDITCONTROL_NOMBRE_DE_USUARIO, usuario_temp, sizeof(usuario_temp));
			GetDlgItemText(hwnd, USUARIOS_EDITCONTROL_CORREO_ELECTRONICO, correo_temp, sizeof(correo_temp));
			GetDlgItemText(hwnd, USUARIOS_EDITCONTROL_CONTRASENA, contrasena_temp, sizeof(contrasena_temp));
			GetDlgItemText(hwnd, USUARIOS_EDITCONTROL_NOMBRE, nombre_temp, sizeof(nombre_temp));
			GetDlgItemText(hwnd, USUARIOS_EDITCONTROL_APELLIDO_PATERNO, apellido_p_temp, sizeof(apellido_p_temp));
			GetDlgItemText(hwnd, USUARIOS_EDITCONTROL_APELLIDO_MATERNO, apellido_m_temp, sizeof(apellido_m_temp));
			GetDlgItemText(hwnd, USUARIOS_EDITCONTROL_RUTA, ruta_temp, sizeof(ruta_temp));

			SYSTEMTIME fecha_de_nacimiento_temporal{ 0 };
			SendDlgItemMessage(hwnd, USUARIOS_DATETIMEPICKER_FECHA_DE_NACIMIENTO, DTM_GETSYSTEMTIME, 0, (LPARAM)&fecha_de_nacimiento_temporal);

			string genero_temporal = IsDlgButtonChecked(hwnd, USUARIOS_RADIOBUTTON_MASCULINO) ? "Masculino" :
				IsDlgButtonChecked(hwnd, USUARIOS_RADIOBUTTON_FEMENINO) ? "Femenino" : "";

			string administrador_temporal = IsDlgButtonChecked(hwnd, USUARIOS_CHECKBOX_ADMINISTRADOR) ? "Administrador" : "NOadministrador";

			// Validar información
			if (string(usuario_temp).empty() || string(correo_temp).empty() || string(contrasena_temp).empty() ||
				string(nombre_temp).empty() || string(apellido_p_temp).empty() || string(apellido_m_temp).empty()) {
				MessageBox(hwnd, "Por favor, completa todos los campos.", "Información.", MB_OK | MB_ICONINFORMATION);
				break;
			}

			// Validar edad mayor a 18 años
			SYSTEMTIME horaActual;
			GetSystemTime(&horaActual);
			int age = horaActual.wYear - fecha_de_nacimiento_temporal.wYear -
				((horaActual.wMonth < fecha_de_nacimiento_temporal.wMonth ||
					(horaActual.wMonth == fecha_de_nacimiento_temporal.wMonth && horaActual.wDay < fecha_de_nacimiento_temporal.wDay)) ? 1 : 0);

			if (age < 18) {
				MessageBox(hwnd, "No puedes registrarte si tienes menos de 18 años.", "Información.", MB_OK | MB_ICONINFORMATION);
				break;
			}

			// Validar selección de género
			if (genero_temporal.empty()) {
				MessageBox(hwnd, "Selecciona un género.", "Información.", MB_OK | MB_ICONINFORMATION);
				break;
			}

			// Crear y agregar nodo
			NodoUsuario* newNodo = generarNodoUsuario(string(usuario_temp), string(correo_temp), string(contrasena_temp),
				string(nombre_temp), string(apellido_p_temp), string(apellido_m_temp),
				fecha_de_nacimiento_temporal, genero_temporal, string(ruta_temp), administrador_temporal);
			agregarNodoALaListaUsuarios(newNodo);

			MessageBox(hwnd, "Usuario agregado exitosamente.", "Información.", MB_OK | MB_ICONINFORMATION);
			cambiarVentana(hwnd, DLG_USUARIOS, cUSUARIOS);
			break;

		}break; // Asegura la salida del case USUARIOS_BUTTON_ALTA

		case USUARIOS_BUTTON_SUBIR: {

			OPENFILENAME ofn;

			char zFile[MAX_PATH]; // 260 caracteres

			ZeroMemory(&ofn, sizeof(ofn));

			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hwnd;
			ofn.lpstrFilter = "ALL\0*.*\0BITMAP\0*.bmp\0";
			ofn.lpstrFile = zFile;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(zFile);
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			ofn.nFilterIndex = 2;

			if (GetOpenFileName(&ofn) == TRUE) {

				SetDlgItemText(hwnd, USUARIOS_EDITCONTROL_RUTA, ofn.lpstrFile);

			}
			else {

				MessageBox(hwnd, "No se cargó ninguna imagen.", "Información.", MB_OK | MB_ICONINFORMATION);

			}

		}break;

		case USUARIOS_BUTTON_BAJA: {

			if (auxiliarListaUsuarios == nullptr) {
				MessageBox(hwnd, "Seleccione el usuario que quiere dar de baja.", "Información.", MB_OK | MB_ICONINFORMATION);
			}
			else {
				int respuesta = MessageBox(hwnd, "¿Seguro que quiere dar este usuario de baja?.", "Información.", MB_ICONEXCLAMATION | MB_OKCANCEL);
				if (respuesta == IDOK) {
					eliminarUsuario();
					cambiarVentana(hwnd, DLG_USUARIOS, cUSUARIOS);
				}
			}

		}break;

		case USUARIOS_BUTTON_MODIFICACION: {

			if (auxiliarListaUsuarios != nullptr) {
				cambiarVentana(hwnd, DLG_MODIFICACION_DE_USUARIOS, cMODIFICACION_USUARIOS);
			}
			else {
				MessageBox(hwnd, "Seleccione el usuario que quiere modificar.", "Información.", MB_OK | MB_ICONINFORMATION);
			}

		}break;

		case USUARIOS_BUTTON_ACTUALIZAR: {

			cambiarVentana(hwnd, DLG_USUARIOS, cUSUARIOS);

		}break;

		case USUARIOS_BUTTON_CANCELAR: {

			messageBoxPantallaPrincipal(hwnd);

		}break;

		}

	}break;

	case WM_CLOSE: {

		messageBoxPantallaPrincipal(hwnd);

	}break;

	}

	return FALSE;
}