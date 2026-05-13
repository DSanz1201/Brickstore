/*
 ============================================================================
 Name        : Brickstore.c
 Author      : Luken de Esquibel and Danel Sanz
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>

#include "sqlite3.h"
#include "usuario.h"
#include "producto.h"
#include "valoracion.h"

#define PORT 5000
#define BUFFER_SIZE 4096

// ===============================
// LOG
// ===============================

void escribirLog(char *mensaje) {
    FILE *f = fopen("server.log", "a");

    if (f != NULL) {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);

        fprintf(f, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
                tm.tm_year + 1900,
                tm.tm_mon + 1,
                tm.tm_mday,
                tm.tm_hour,
                tm.tm_min,
                tm.tm_sec,
                mensaje);

        fclose(f);
    }
}

// ===============================
// CREAR TABLAS
// ===============================

void inicializarDB(sqlite3 *db) {
    char *sql1 =
        "CREATE TABLE IF NOT EXISTS producto ("
        "id INTEGER PRIMARY KEY,"
        "nombre TEXT,"
        "stock INTEGER,"
        "precio REAL);";

    char *sql2 =
        "CREATE TABLE IF NOT EXISTS usuario ("
        "id INTEGER PRIMARY KEY,"
        "nombre TEXT,"
        "email TEXT UNIQUE,"
        "password TEXT,"
        "admin INTEGER);";

    char *sql3 =
        "CREATE TABLE IF NOT EXISTS valoracion ("
        "id INTEGER PRIMARY KEY,"
        "id_usuario INTEGER,"
        "id_producto INTEGER,"
        "puntuacion INTEGER,"
        "comentario TEXT,"
        "FOREIGN KEY(id_usuario) REFERENCES usuario(id),"
        "FOREIGN KEY(id_producto) REFERENCES producto(id));";

    sqlite3_exec(db, sql1, 0, 0, 0);
    sqlite3_exec(db, sql2, 0, 0, 0);
    sqlite3_exec(db, sql3, 0, 0, 0);

    sqlite3_exec(db,
        "INSERT OR IGNORE INTO usuario "
        "(id, nombre, email, password, admin) "
        "VALUES (1, 'Admin', 'admin', '1234', 1);",
        0, 0, 0);
}

// ===============================
// CARGAR PRODUCTOS DESDE FICHERO
// ===============================

void inicializarFichero(char *nombreFichero, sqlite3 *db) {
    FILE *f = fopen(nombreFichero, "r");

    if (f != NULL) {
        Producto p;

        while (fscanf(f, " %d;%49[^;];%d;%f",
                      &p.id,
                      p.nombre,
                      &p.stock,
                      &p.precio) == 4) {
            insertarProducto(db, p);
        }

        fclose(f);
    } else {
        printf("Error abriendo fichero de productos\n");
    }
}

// ===============================
// CARGAR VALORACIONES
// ===============================

void cargarValoracionesFichero(char *nombreFichero, sqlite3 *db) {
    FILE *f = fopen(nombreFichero, "r");

    if (f != NULL) {
        Valoracion v;

        while (fscanf(f, " %d;%d;%d;%d;%199[^\n\r]",
                      &v.id,
                      &v.id_usuario,
                      &v.id_producto,
                      &v.puntuacion,
                      v.comentario) == 5) {
            InsertarValoracion(db, v);
        }

        fclose(f);
    } else {
        printf("Error abriendo fichero de valoraciones\n");
    }
}

// ===============================
// LISTAR PRODUCTOS PARA CLIENTE
// ===============================

void listarProductosServidor(sqlite3 *db, char *respuesta) {
    sqlite3_stmt *stmt;
    char *sql = "SELECT id, nombre, stock, precio FROM producto;";

    strcpy(respuesta, "\n--- CATALOGO DE PRODUCTOS ---\n");

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        strcpy(respuesta, "ERROR;No se pudo consultar productos");
        return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        char linea[300];

        sprintf(linea,
                "ID: %d | Nombre: %s | Stock: %d | Precio: %.2f\n",
                sqlite3_column_int(stmt, 0),
                sqlite3_column_text(stmt, 1),
                sqlite3_column_int(stmt, 2),
                sqlite3_column_double(stmt, 3));

        strcat(respuesta, linea);
    }

    sqlite3_finalize(stmt);
}

// ===============================
// LISTAR VALORACIONES PARA CLIENTE
// ===============================

void listarValoracionesServidor(sqlite3 *db, char *respuesta) {
    sqlite3_stmt *stmt;

    char *sql =
        "SELECT v.puntuacion, v.comentario, u.nombre, p.nombre "
        "FROM valoracion v "
        "LEFT JOIN usuario u ON v.id_usuario = u.id "
        "LEFT JOIN producto p ON v.id_producto = p.id;";

    strcpy(respuesta, "\n--- VALORACIONES DE CLIENTES ---\n");

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        strcpy(respuesta, "ERROR;No se pudo consultar valoraciones");
        return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        char linea[500];

        sprintf(linea,
                "Usuario: %s | Producto: %s | Nota: %d | Comentario: %s\n",
                sqlite3_column_text(stmt, 2),
                sqlite3_column_text(stmt, 3),
                sqlite3_column_int(stmt, 0),
                sqlite3_column_text(stmt, 1));

        strcat(respuesta, linea);
    }

    sqlite3_finalize(stmt);
}

// ===============================
// INSERTAR PRODUCTO
// ===============================

void insertarProductoServidor(sqlite3 *db, Producto p, char *respuesta) {
    insertarProducto(db, p);
    strcpy(respuesta, "OK;Producto insertado correctamente");
}

// ===============================
// MODIFICAR PRODUCTO
// ===============================

void modificarProductoServidor(sqlite3 *db, int id, int stock, float precio, char *respuesta) {
    sqlite3_stmt *stmt;

    char *sql =
        "UPDATE producto "
        "SET stock = ?, precio = ? "
        "WHERE id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        strcpy(respuesta, "ERROR;No se pudo preparar modificacion");
        return;
    }

    sqlite3_bind_int(stmt, 1, stock);
    sqlite3_bind_double(stmt, 2, precio);
    sqlite3_bind_int(stmt, 3, id);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        strcpy(respuesta, "OK;Producto modificado correctamente");
    } else {
        strcpy(respuesta, "ERROR;No se pudo modificar producto");
    }

    sqlite3_finalize(stmt);
}

// ===============================
// ELIMINAR PRODUCTO
// ===============================

void eliminarProductoServidor(sqlite3 *db, int id, char *respuesta) {
    sqlite3_stmt *stmt;

    char *sql = "DELETE FROM producto WHERE id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        strcpy(respuesta, "ERROR;No se pudo preparar eliminacion");
        return;
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        strcpy(respuesta, "OK;Producto eliminado correctamente");
    } else {
        strcpy(respuesta, "ERROR;No se pudo eliminar producto");
    }

    sqlite3_finalize(stmt);
}

// ===============================
// REGISTRAR ADMIN
// ===============================

void registrarAdminServidor(sqlite3 *db, Usuario u, char *respuesta) {
    u.admin = 1;
    insertarUsuario(db, u);
    strcpy(respuesta, "OK;Administrador registrado correctamente");
}

// ===============================
// PROCESAR COMANDOS DEL CLIENTE
// ===============================

void procesarComando(sqlite3 *db, char *comando, char *respuesta) {
    char copia[BUFFER_SIZE];
    strcpy(copia, comando);

    char *accion = strtok(copia, ";");

    if (accion == NULL) {
        strcpy(respuesta, "ERROR;Comando vacio");
        return;
    }

    if (strcmp(accion, "LOGIN") == 0) {
        char *email = strtok(NULL, ";");
        char *password = strtok(NULL, ";");

        if (email == NULL || password == NULL) {
            strcpy(respuesta, "ERROR;Formato LOGIN incorrecto");
            escribirLog("LOGIN ERROR - Formato incorrecto");
            return;
        }

        Usuario u;

        if (loginUsuario(db, email, password, &u) && u.admin == 1) {
            strcpy(respuesta, "OK;Login administrador correcto");
            escribirLog("LOGIN OK - Administrador conectado");
        } else {
            strcpy(respuesta, "ERROR;Login incorrecto o no administrador");
            escribirLog("LOGIN ERROR - Credenciales incorrectas");
        }
    }

    else if (strcmp(accion, "LISTAR") == 0) {
        listarProductosServidor(db, respuesta);
        escribirLog("LISTAR - Productos consultados");
    }

    else if (strcmp(accion, "ADD") == 0) {
        Producto p;

        char *id = strtok(NULL, ";");
        char *nombre = strtok(NULL, ";");
        char *stock = strtok(NULL, ";");
        char *precio = strtok(NULL, ";");

        if (id == NULL || nombre == NULL || stock == NULL || precio == NULL) {
            strcpy(respuesta, "ERROR;Formato ADD incorrecto");
            return;
        }

        p.id = atoi(id);
        strcpy(p.nombre, nombre);
        p.stock = atoi(stock);
        p.precio = atof(precio);

        insertarProductoServidor(db, p, respuesta);
        escribirLog("ADD - Producto insertado");
    }

    else if (strcmp(accion, "UPDATE") == 0) {
        char *id = strtok(NULL, ";");
        char *stock = strtok(NULL, ";");
        char *precio = strtok(NULL, ";");

        if (id == NULL || stock == NULL || precio == NULL) {
            strcpy(respuesta, "ERROR;Formato UPDATE incorrecto");
            return;
        }

        modificarProductoServidor(db,
                                  atoi(id),
                                  atoi(stock),
                                  atof(precio),
                                  respuesta);

        escribirLog("UPDATE - Producto modificado");
    }

    else if (strcmp(accion, "DELETE") == 0) {
        char *id = strtok(NULL, ";");

        if (id == NULL) {
            strcpy(respuesta, "ERROR;Formato DELETE incorrecto");
            return;
        }

        eliminarProductoServidor(db, atoi(id), respuesta);
        escribirLog("DELETE - Producto eliminado");
    }

    else if (strcmp(accion, "VALORACIONES") == 0) {
        listarValoracionesServidor(db, respuesta);
        escribirLog("VALORACIONES - Consultadas");
    }

    else if (strcmp(accion, "REG_ADMIN") == 0) {
        Usuario u;

        char *nombre = strtok(NULL, ";");
        char *email = strtok(NULL, ";");
        char *password = strtok(NULL, ";");

        if (nombre == NULL || email == NULL || password == NULL) {
            strcpy(respuesta, "ERROR;Formato REG_ADMIN incorrecto");
            return;
        }

        u.id = 0;
        strcpy(u.nombre, nombre);
        strcpy(u.email, email);
        strcpy(u.password, password);
        u.admin = 1;

        registrarAdminServidor(db, u, respuesta);
        escribirLog("REG_ADMIN - Nuevo administrador registrado");
    }

    else if (strcmp(accion, "SALIR") == 0) {
        strcpy(respuesta, "OK;Conexion cerrada");
        escribirLog("SALIR - Cliente desconectado");
    }

    else {
        strcpy(respuesta, "ERROR;Comando no reconocido");
        escribirLog("ERROR - Comando no reconocido");
    }
}

// ===============================
// MAIN SERVIDOR
// ===============================

int main() {
    setbuf(stdout, NULL);

    printf("Iniciando servidor\n");

    WSADATA wsa;
    SOCKET servidor;
    SOCKET cliente;

    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;

    int clientSize;
    int bytes;

    char buffer[BUFFER_SIZE];
    char respuesta[BUFFER_SIZE];

    sqlite3 *db;

    // ===============================
    // ABRIR BD
    // ===============================

    if (sqlite3_open("brickstore.db", &db) != SQLITE_OK) {
        printf("Error al abrir BD: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    inicializarDB(db);

    inicializarFichero("lego.txt", db);
    inicializarFicheroUsuarios("usuario.txt", db);
    cargarValoracionesFichero("valoracion.txt", db);

    escribirLog("Servidor iniciado");

    // ===============================
    // INICIAR WINSOCK
    // ===============================

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Error inicializando Winsock\n");
        escribirLog("ERROR - WSAStartup");
        sqlite3_close(db);
        return 1;
    }

    servidor = socket(AF_INET, SOCK_STREAM, 0);

    if (servidor == INVALID_SOCKET) {
        printf("Error creando socket\n");
        escribirLog("ERROR - socket");
        WSACleanup();
        sqlite3_close(db);
        return 1;
    }

    // ===============================
    // CONFIGURAR SERVIDOR
    // ===============================

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(servidor, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Error en bind. Codigo: %d\n", WSAGetLastError());
        escribirLog("ERROR - bind");
        closesocket(servidor);
        WSACleanup();
        sqlite3_close(db);
        return 1;
    }

    if (listen(servidor, 1) == SOCKET_ERROR) {
        printf("Error en listen. Codigo: %d\n", WSAGetLastError());
        escribirLog("ERROR - listen");
        closesocket(servidor);
        WSACleanup();
        sqlite3_close(db);
        return 1;
    }

    printf("Servidor BrickStore escuchando en puerto %d...\n", PORT);
    escribirLog("Servidor escuchando conexiones");

    // ===============================
    // ACEPTAR CLIENTE
    // ===============================

    clientSize = sizeof(clientAddr);

    cliente = accept(servidor, (struct sockaddr *)&clientAddr, &clientSize);

    if (cliente == INVALID_SOCKET) {
        printf("Error aceptando cliente. Codigo: %d\n", WSAGetLastError());
        escribirLog("ERROR - accept");
        closesocket(servidor);
        WSACleanup();
        sqlite3_close(db);
        return 1;
    }

    printf("Cliente conectado.\n");
    escribirLog("Cliente conectado");

    // ===============================
    // BUCLE PRINCIPAL
    // ===============================

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        memset(respuesta, 0, BUFFER_SIZE);

        bytes = recv(cliente, buffer, BUFFER_SIZE - 1, 0);

        if (bytes <= 0) {
            printf("Cliente desconectado.\n");
            escribirLog("Cliente desconectado inesperadamente");
            break;
        }

        buffer[bytes] = '\0';

        printf("Comando recibido: %s\n", buffer);

        procesarComando(db, buffer, respuesta);

        send(cliente, respuesta, strlen(respuesta), 0);

        printf("Respuesta enviada: %s\n", respuesta);

        if (strncmp(buffer, "SALIR", 5) == 0) {
            break;
        }
    }

    // ===============================
    // CERRAR TODO
    // ===============================

    closesocket(cliente);
    closesocket(servidor);

    WSACleanup();

    sqlite3_close(db);

    escribirLog("Servidor cerrado");

    printf("Servidor cerrado.\n");

    return 0;
}
