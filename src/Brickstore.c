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
}

// Leer fichero y cargar datos
void inicializarFichero(char* nombreFichero, sqlite3 *db){
    FILE *f;
    f = fopen(nombreFichero, "r");

    if(f != NULL){
        Producto p;
        while(fscanf(f, "%d;%49[^;];%d;%f\n", &p.id, p.nombre, &p.stock,&p.precio) == 4){
            insertarProducto(db, p);
        }

        fclose(f);
    } else {
        printf("Error abriendo fichero\n");
    }
}

void crearFicheroProductos(char *nombreFichero){
    FILE *f;

    f = fopen(nombreFichero, "w");

    if(f != NULL){
    	// LOST FPRINTF HAS SIDO GENERADOS CON IA GENERATIVA (CHAT GPT)
        fprintf(f, "1;LEGO Star Wars Millennium Falcon;5;159.99\n");
        fprintf(f, "2;LEGO Harry Potter Hogwarts Castle;3;129.99\n");
        fprintf(f, "3;LEGO Technic Bugatti Chiron;2;349.99\n");
        fprintf(f, "4;LEGO City Police Station;10;89.99\n");
        fprintf(f, "5;LEGO Marvel Avengers Tower;4;199.99\n");
        fprintf(f, "6;LEGO Star Wars The Skywalker Saga PS5;8;59.99\n");
        fprintf(f, "7;LEGO Harry Potter Collection Switch;6;39.99\n");
        fprintf(f, "8;LEGO Marvel Super Heroes 2 PC;7;29.99\n");
        fprintf(f, "9;LEGO Jurassic World Xbox;5;24.99\n");
        fprintf(f, "10;LEGO Batman 3 Beyond Gotham PS4;9;19.99\n");

        fclose(f);

        printf("Fichero creado correctamente\n");

    } else {
        printf("Error creando fichero\n");
    }
}


int main(void) {
    sqlite3 *db;
    int cont;

    cont = sqlite3_open("tienda.db", &db);

    if(cont != SQLITE_OK){
        printf("Error al abrir BD\n");
        fflush(stdout);
        return 1;
    }
// ya han sido creados
    inicializarDB(db);
    crearFicheroProductos("lego.txt");
    inicializarFichero("lego.txt", db);

    sqlite3_close(db);
    return 0;
}
