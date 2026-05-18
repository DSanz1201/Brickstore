#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pedido.h"

void registrarPedidoBD(sqlite3 *db, int id_usuario, char *lineas_str, char *respuesta) {
    char sql_pedido[256];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char fecha[20];
    sprintf(fecha, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

    sprintf(sql_pedido, "INSERT INTO pedido (id_usuario, fecha) VALUES (%d, '%s');", id_usuario, fecha);

    if (sqlite3_exec(db, sql_pedido, 0, 0, 0) != SQLITE_OK) {
        strcpy(respuesta, "ERROR;Fallo al procesar pedido en Base de Datos");
        return;
    }

    long long id_pedido = sqlite3_last_insert_rowid(db);

    char *item = strtok(lineas_str, ",");
    while (item != NULL) {
        int id_prod = 0;
        int cant = 0;
        if (sscanf(item, "%d:%d", &id_prod, &cant) == 2) {
            char sql_linea[256];
            sprintf(sql_linea, "INSERT INTO linea_pedido (id_pedido, id_producto, cantidad) VALUES (%lld, %d, %d);", id_pedido, id_prod, cant);
            sqlite3_exec(db, sql_linea, 0, 0, 0);

            char sql_stock[256];
            sprintf(sql_stock, "UPDATE producto SET stock = stock - %d WHERE id = %d;", cant, id_prod);
            sqlite3_exec(db, sql_stock, 0, 0, 0);
        }
        item = strtok(NULL, ",");
    }

    strcpy(respuesta, "OK;Pedido guardado y stock actualizado correctamente");
}

void consultarPedidosBD(sqlite3 *db, int id_usuario, char *respuesta) {
    sqlite3_stmt *stmt;
    char *sql = "SELECT p.id, p.fecha, lp.id_producto, prod.nombre, lp.cantidad "
                "FROM pedido p "
                "JOIN linea_pedido lp ON p.id = lp.id_pedido "
                "JOIN producto prod ON lp.id_producto = prod.id "
                "WHERE p.id_usuario = ? ORDER BY p.id DESC;";

    strcpy(respuesta, "DATA;\n--- HISTORIAL DE PEDIDOS ---\n");

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id_usuario);
        int encontrado = 0;
        int ultimo_pedido_id = -1;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            encontrado = 1;
            int id_pedido = sqlite3_column_int(stmt, 0);
            const unsigned char *fecha = sqlite3_column_text(stmt, 1);
            int id_prod = sqlite3_column_int(stmt, 2);
            const unsigned char *nombre_prod = sqlite3_column_text(stmt, 3);
            int cantidad = sqlite3_column_int(stmt, 4);

            char linea[256];
            if (id_pedido != ultimo_pedido_id) {
                sprintf(linea, "Pedido ID: %d | Fecha: %s\n", id_pedido, fecha);
                strcat(respuesta, linea);
                ultimo_pedido_id = id_pedido;
            }
            sprintf(linea, "   -> Producto: %s (ID: %d) x%d unidades\n", nombre_prod, id_prod, cantidad);
            strcat(respuesta, linea);
        }
        if (!encontrado) {
            strcpy(respuesta, "DATA;No constan pedidos para este usuario.");
        }
        sqlite3_finalize(stmt);
    } else {
        strcpy(respuesta, "ERROR;Error al leer historial de la base de datos");
    }
}
