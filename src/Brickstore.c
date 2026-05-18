/*
 ============================================================================
 Name        : Brickstore.c
 Author      : Luken de Esquibel, Danel Sanz and Josu
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

// Usuario (Admin) = admin
// Contraseña = 1234
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>

#include "sqlite3.h"
#include "usuario.h"
#include "producto.h"
#include "valoracion.h"
#include "pedido.h"

#define PORT 5000
#define BUFFER_SIZE 4096

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

    char *sql4 =
        "CREATE TABLE IF NOT EXISTS pedido ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "id_usuario INTEGER,"
        "fecha TEXT,"
        "FOREIGN KEY(id_usuario) REFERENCES usuario(id));";

    char *sql5 =
        "CREATE TABLE IF NOT EXISTS linea_pedido ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "id_pedido INTEGER,"
        "id_producto INTEGER,"
        "cantidad INTEGER,"
        "FOREIGN KEY(id_pedido) REFERENCES pedido(id),"
        "FOREIGN KEY(id_producto) REFERENCES producto(id));";

    sqlite3_exec(db, sql1, 0, 0, 0);
    sqlite3_exec(db, sql2, 0, 0, 0);
    sqlite3_exec(db, sql3, 0, 0, 0);
    sqlite3_exec(db, sql4, 0, 0, 0);
    sqlite3_exec(db, sql5, 0, 0, 0);

    sqlite3_exec(db,
        "INSERT OR IGNORE INTO usuario "
        "(id, nombre, email, password, admin) "
        "VALUES (1, 'Admin', 'admin', '1234', 1);",
        0, 0, 0);
}

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

        if (loginUsuario(db, email, password, &u)) {
            if (u.admin == 1) {
                sprintf(respuesta, "OK;ADMIN;%d;Login administrador correcto", u.id);
                escribirLog("LOGIN OK - Administrador conectado");
            } else {
                sprintf(respuesta, "OK;USER;%d;Login usuario correcto", u.id);
                escribirLog("LOGIN OK - Usuario corriente conectado");
            }
        } else {
            strcpy(respuesta, "ERROR;Login incorrecto o credenciales invalidas");
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

        modificarProductoServidor(db, atoi(id), atoi(stock), atof(precio), respuesta);
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

    else if (strcmp(accion, "REGISTER") == 0) {
        char *nombre = strtok(NULL, ";");
        char *email = strtok(NULL, ";");
        char *password = strtok(NULL, ";");

        if (nombre == NULL || email == NULL || password == NULL) {
            strcpy(respuesta, "ERROR;Formato REGISTER incorrecto");
            return;
        }

        char sql[512];
        sprintf(sql, "INSERT INTO usuario (nombre, email, password, admin) VALUES ('%s', '%s', '%s', 0);", nombre, email, password);

        char *err_msg = 0;
        int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

        if (rc != SQLITE_OK) {
            sprintf(respuesta, "ERROR;No se pudo registrar: %s", err_msg);
            sqlite3_free(err_msg);
            escribirLog("REGISTER ERROR - Email duplicado o fallo en BD");
        } else {
            strcpy(respuesta, "OK;Usuario registrado correctamente");
            escribirLog("REGISTER OK - Nuevo usuario corriente registrado");
        }
    }

    else if (strcmp(accion, "GET_PRODUCT_DETAIL") == 0) {
        char *id_str = strtok(NULL, ";");
        if (id_str == NULL) {
            strcpy(respuesta, "ERROR;Formato detalle incorrecto");
            return;
        }
        int id_producto = atoi(id_str);

        sqlite3_stmt *stmt;
        char *sql = "SELECT nombre, stock, precio FROM producto WHERE id = ?;";

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, id_producto);
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                const unsigned char *nombre = sqlite3_column_text(stmt, 0);
                int stock = sqlite3_column_int(stmt, 1);
                double precio = sqlite3_column_double(stmt, 2);

                sprintf(respuesta, "DATA;ID: %d | %s | Stock: %d | Precio: %.2f EUR", id_producto, nombre, stock, precio);
                escribirLog("GET_PRODUCT_DETAIL - Detalle consultado con exito");
            } else {
                strcpy(respuesta, "ERROR;Producto no encontrado");
            }
            sqlite3_finalize(stmt);
        } else {
            strcpy(respuesta, "ERROR;Error de preparacion SQL");
        }
    }

    else if (strcmp(accion, "CREATE_ORDER") == 0) {
            char *id_usr_str = strtok(NULL, ";");
            char *lineas_str = strtok(NULL, ";");

            if (id_usr_str == NULL || lineas_str == NULL) {
                strcpy(respuesta, "ERROR;Formato de pedido incompleto");
                return;
            }

            Pedido *nuevoPedido = (Pedido *)malloc(sizeof(Pedido));
            nuevoPedido->id_usuario = atoi(id_usr_str);

            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            sprintf(nuevoPedido->fecha, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

            nuevoPedido->lineas = NULL;

            char *item = strtok(lineas_str, ",");
            while (item != NULL) {
                int id_prod = 0, cant = 0;
                if (sscanf(item, "%d:%d", &id_prod, &cant) == 2) {

                    LineaPedido *nuevaLinea = (LineaPedido *)malloc(sizeof(LineaPedido));
                    nuevaLinea->id_producto = id_prod;
                    nuevaLinea->cantidad = cant;

                    nuevaLinea->siguiente = nuevoPedido->lineas;
                    nuevoPedido->lineas = nuevaLinea;
                }
                item = strtok(NULL, ",");
            }

            registrarPedidoBD(db, nuevoPedido, respuesta);

            liberarPedido(nuevoPedido);

            if (strncmp(respuesta, "OK", 2) == 0) {
                escribirLog("CREATE_ORDER - Compra realizada con exito");
            } else {
                escribirLog("CREATE_ORDER - Error al registrar la compra en BD");
            }
        }

    else if (strcmp(accion, "GET_ORDERS") == 0) {
        char *id_usr_str = strtok(NULL, ";");
        if (id_usr_str == NULL) {
            strcpy(respuesta, "ERROR;Formato de consulta de pedidos incorrecto");
            return;
        }

        int id_usuario = atoi(id_usr_str);

        consultarPedidosBD(db, id_usuario, respuesta);

        escribirLog("GET_ORDERS - Historial de usuario consultado");
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

    if (sqlite3_open("brickstore.db", &db) != SQLITE_OK) {
        printf("Error al abrir BD: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    inicializarDB(db);
    inicializarFichero("lego.txt", db);
    inicializarFicheroUsuarios("usuario.txt", db);
    cargarValoracionesFichero("valoracion.txt", db);

    escribirLog("Servidor iniciado");

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

    closesocket(cliente);
    closesocket(servidor);

    WSACleanup();

    sqlite3_close(db);

    escribirLog("Servidor cerrado");

    printf("Servidor cerrado.\n");

    return 0;
}
