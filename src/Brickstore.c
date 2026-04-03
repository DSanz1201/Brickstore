/*
 ============================================================================
 Name        : Brickstore.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include"producto.h"
#include <stdio.h>
#include <stdlib.h>

#include "sqlite3.h"
#include "usuario.h"
#include "pedido.h"
#include "lineaPedido.h"
#include "valoracion.h"
#include <time.h>

void inicializarDB(sqlite3 *db){
    char *sql1 =
        "CREATE TABLE IF NOT EXISTS producto ("
        "id INTEGER PRIMARY KEY,"
        "nombre TEXT,"
        "stock INTEGER,"
        "precio REAL);";

    if(sqlite3_exec(db, sql1, 0, 0, 0) != SQLITE_OK){
        printf("Error creando tabla producto\n");

    }

    char *sql2 =
        "CREATE TABLE IF NOT EXISTS usuario ("
        "id INTEGER PRIMARY KEY,"
        "nombre TEXT,"
        "email TEXT UNIQUE,"
        "password TEXT,"
        "admin INTEGER);";

    if(sqlite3_exec(db, sql2, 0, 0, 0) != SQLITE_OK){
        printf("Error creando tabla usuario\n");
    }

    char *sql3 =
        "CREATE TABLE IF NOT EXISTS pedido ("
        "id INTEGER PRIMARY KEY,"
        "id_usuario INTEGER,"
        "total REAL,"
        "fecha TEXT,"
        "FOREIGN KEY (id_usuario) REFERENCES usuario(id));";

    if(sqlite3_exec(db, sql3, 0, 0, 0) != SQLITE_OK){
        printf("Error creando tabla pedido\n");
    }

    char *sql4 =
        "CREATE TABLE IF NOT EXISTS linea_pedido ("
        "id INTEGER PRIMARY KEY,"
        "id_pedido INTEGER,"
        "id_producto INTEGER,"
        "cantidad INTEGER,"
        "FOREIGN KEY (id_pedido) REFERENCES pedido(id),"
        "FOREIGN KEY (id_producto) REFERENCES producto(id));";

    if(sqlite3_exec(db, sql4, 0, 0, 0) != SQLITE_OK){
        printf("Error creando tabla linea_pedido\n");

    }
    char *sql5 =
        "CREATE TABLE IF NOT EXISTS valoracion ("
        "id INTEGER PRIMARY KEY,"
        "id_usuario INTEGER,"
        "id_producto INTEGER,"
        "puntuacion INTEGER,"
        "comentario TEXT,"
        "FOREIGN KEY (id_usuario) REFERENCES usuario(id),"
        "FOREIGN KEY (id_producto) REFERENCES producto(id));";

    if(sqlite3_exec(db, sql5, 0, 0, 0) != SQLITE_OK){
        printf("Error creando tabla valoracion\n");
    }

    sqlite3_exec(db, "INSERT OR IGNORE INTO usuario (nombre, email, password, admin) VALUES ('Admin', 'admin', '1234', 1);", 0, 0, 0);
}

// Leer fichero y cargar datos
void inicializarFichero(char* nombreFichero, sqlite3 *db){
    FILE *f;
    f = fopen(nombreFichero, "r");

    if(f != NULL){
        Producto p;
        while(fscanf(f, " %d;%49[^;];%d;%f", &p.id, p.nombre, &p.stock, &p.precio) == 4) {
            insertarProducto(db, p);
        }

        fclose(f);
    } else {
        printf("Error abriendo fichero\n");
    }
}
void obtenerConfiguracion(char* clave, char* valor) {
    FILE *f = fopen("config.txt", "r");
    char linea[100];
    if (f != NULL) {
        while (fgets(linea, sizeof(linea), f)) {
            if (strstr(linea, clave)) {
                strtok(linea, "=");
                char *v = strtok(NULL, "\n");
                strcpy(valor, v);
                break;
            }
        }
        fclose(f);
    }
}

void escribirLog(char* mensaje) {
    FILE *f = fopen("server.log", "a");
    if (f != NULL) {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        fprintf(f, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                tm.tm_hour, tm.tm_min, tm.tm_sec, mensaje);
        fclose(f);
    }
}

void cargarValoracionesFichero(char* nombreFichero, sqlite3 *db) {
    FILE *f = fopen(nombreFichero, "r");
    if (f != NULL) {
        Valoracion v;
        // El formato es: id;id_usuario;id_producto;puntuacion;comentario
        while (fscanf(f, " %d;%d;%d;%d;%199[^\n\r]", &v.id, &v.id_usuario, &v.id_producto, &v.puntuacion, v.comentario) == 5) {
            InsertarValoracion(db, v);
        }
        fclose(f);
    } else {
        printf("Error abriendo fichero de valoraciones\n");
    }
}


int main(void) {
	setbuf(stdout, NULL);
    sqlite3 *db;
    Usuario userLogueado;
    char email[80], pass[30];
    int logueado = 0;
    char db_path[50];

    obtenerConfiguracion("DB_PATH", db_path);

    if(sqlite3_open(db_path, &db) != SQLITE_OK){
        printf("Error al abrir BD\n");
        return 1;
    }

// ya han sido creados
//    inicializarDB(db);
//    inicializarFichero("lego.txt", db);
//    inicializarFicheroUsuarios("usuario.txt", db);
//    cargarValoracionesFichero("valoracion.txt", db);


    printf("=== BRICKSTORE: ACCESO ADMINISTRADOR ===\n");
    while(!logueado) {
        printf("Email: ");
        fflush(stdout);
        scanf(" %s", email);

        printf("Contraseña: ");
        fflush(stdout);
        scanf(" %s", pass);

        if(loginUsuario(db, email, pass, &userLogueado)) {
            if(userLogueado.admin == 1) {
                printf("Bienvenido Administrador: %s\n", userLogueado.nombre);
                escribirLog("LOGIN OK - Administrador conectado");
                logueado = 1;
            } else {
                printf("Error: Solo los administradores pueden acceder a esta herramienta.\n");
            }
        }
    }

    int opcion = 0;
        while(opcion != 7) {
            printf("\n--- MENU DE ADMINISTRACION ---\n");
            printf("1. Visualizar catalogo de productos\n");
            printf("2. Añadir nuevo producto\n");
            printf("3. Modificar producto (Stock/Precio)\n");
            printf("4. Eliminar producto\n");
            printf("5. Ver valoraciones de clientes\n");
            printf("6. Registrar nuevo administrador\n");
            printf("7. Salir\n");
            printf("Seleccione una opcion: ");
            fflush(stdout);
            scanf("%d", &opcion);

            switch(opcion) {
                case 1:
                	listarProductos(db);
                	escribirLog("LISTAR - El administrador ha consultado el catálogo completo de productos.");
                    break;
                case 2:
                    anyadirProducto(db);
                    escribirLog("INSERTAR - Se ha dado de alta un nuevo producto en la base de datos.");
                    break;
                case 3:
                    modificarProducto(db);
                    escribirLog("MODIFICAR - Se han actualizado los datos (stock/precio) de un producto existente.");
                    break;
                case 4:
                    eliminarProducto(db);
                    escribirLog("ELIMINAR - Se ha borrado un producto del catálogo de la tienda.");
                    break;
                case 5:
                    listarValoraciones(db);
                    escribirLog("VALORACIONES - El administrador ha consultado las reseñas.");
                    break;
                case 6:
                    registrarAdmin(db);
                    escribirLog("NUEVO_ADMIN - Se ha creado una nueva cuenta de administrador.");
                    break;
                case 7:
                    printf("Cerrando sesion y saliendo...\n");
                    escribirLog("LOGOUT - El administrador ha cerrado la sesión y la herramienta de gestión.");
                    break;
                default:
                    printf("Opcion no valida.\n");
            }
        }

    sqlite3_close(db);
    return 0;
}
