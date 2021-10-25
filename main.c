#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#define animalesArch "animales.dat"

/// estructuras

typedef struct
{
    char nombreAnimal[30];
    int cant;
    int habitat;
    int idEspecie;
    char especie[20];
} registroArchivo;

typedef struct
{
    char nombreAnimal[30];
    int cantidad;
    int habitat; // 1 - selva, 2- savana, 3-bosque, 4-mar
} animal;

typedef struct nodito
{
    animal dato;
    struct nodito* sig;
} nodito;

typedef struct nodo
{
    int idEspecie;
    char especie [20]; //"Mamiferos","Aves","Reptiles","Peces"
    nodito* animales;
    struct nodo* sig;
} nodo;

/// funciones 1)

// sublista animales

nodito* crearNodito(animal dato)
{
    nodito* nuevo = (nodito*)malloc(sizeof(nodito));
    nuevo->dato = dato;
    nuevo->sig = NULL;
    return nuevo;
}

void insertarOrdenadoAnimal(nodito** listaAnimales, animal dato)
{
    if (*listaAnimales)
    {
        if (strcmpi((*listaAnimales)->dato.nombreAnimal, dato.nombreAnimal) > 0)
        {
            nodito* nuevo = crearNodito(dato);
            nuevo->sig = *listaAnimales;
            *listaAnimales = nuevo;
        }
        else
            insertarOrdenadoAnimal(&(*listaAnimales)->sig, dato);
    }
    else
        *listaAnimales = crearNodito(dato);
}

// lista especies

nodo* crearNodo(int idEspecie, char* nombreEspecie)
{
    nodo* nuevo = (nodo*)malloc(sizeof(nodo));
    nuevo->idEspecie = idEspecie;
    strcpy(nuevo->especie, nombreEspecie);
    nuevo->animales = NULL;
    nuevo->sig = NULL;
    return nuevo;
}

void insertarOrdenadoEspecies(nodo** listaEspecies, int idEspecie, char* nombreEspecie, animal dato)
{
    if(*listaEspecies)
    {
        if ((*listaEspecies)->idEspecie > idEspecie)
        {
            nodo* nuevo = crearNodo(idEspecie, nombreEspecie);
            insertarOrdenadoAnimal(&nuevo->animales, dato);
            nuevo->sig = *listaEspecies;
            *listaEspecies = nuevo;
        }
        else if ((*listaEspecies)->idEspecie < idEspecie)
            insertarOrdenadoEspecies(&(*listaEspecies)->sig, idEspecie, nombreEspecie, dato);
        else // si ya existe un nodo especie con su idEspecie
            insertarOrdenadoAnimal(&(*listaEspecies)->animales, dato);
    }
    else
    {
        *listaEspecies = crearNodo(idEspecie, nombreEspecie);
        insertarOrdenadoAnimal(&(*listaEspecies)->animales, dato);
    }
}

// global

void despersistirYAgregar (nodo** listaEspecies)
{
    FILE* fp = fopen(animalesArch, "rb");

    registroArchivo regisBuff;
    animal animBuff;

    if (fp)
    {
        while (fread(&regisBuff, sizeof(registroArchivo), 1, fp) > 0)
        {
            animBuff.cantidad = regisBuff.cant;
            animBuff.habitat = regisBuff.habitat;
            strcpy(animBuff.nombreAnimal, regisBuff.nombreAnimal);

            insertarOrdenadoEspecies(listaEspecies, regisBuff.idEspecie, regisBuff.especie, animBuff);
        }

        fclose(fp);
    }
    else
        printf("Error de datos.\n");
}

/// funciones 2)

void persistirArchivoAnimales(char* especie, nodito* animales)
{
   char nombreArchEspecie[23];
   strcpy(nombreArchEspecie, especie);
   FILE* fp = fopen(strcat(nombreArchEspecie, ".bin"), "wb");

   if (fp)
   {
       while(animales)
       {
           fwrite(&animales->dato, sizeof(animal), 1, fp);
           animales = animales->sig;
       }

       fclose(fp);
   }
   else
       printf("Error de datos.\n");
}

// global

void persistirArchivoEspecies(nodo* listaEspecies)
{
    while(listaEspecies)
    {
        persistirArchivoAnimales(listaEspecies->especie, listaEspecies->animales);
        listaEspecies = listaEspecies->sig;
    }
}

/// funciones 3)

void mostrarAnimal(animal dato)
{
    printf("ANIMAL: %s\n", dato.nombreAnimal);
    printf("CANTIDAD: %i\n", dato.cantidad);
    printf("HABITAT: %i\n", dato.habitat);
}

void mostrarAnimalesEspecie (nodo* listaEspecies, char* especie)
{
    if(listaEspecies)
    {
        if (strcmpi(listaEspecies->especie, especie) == 0)
        {
            printf("ESPECIE: %s\n", listaEspecies->especie);
            while(listaEspecies->animales)
            {
                mostrarAnimal(listaEspecies->animales->dato);
                listaEspecies->animales = listaEspecies->animales->sig;
            }
        }
        else
            mostrarAnimalesEspecie(listaEspecies->sig, especie);
    }
    else
        printf("NO SE ENCONTRO LA ESPECIE %s\n", especie);
}

/// funciones 4)

void mostrarAnimalesHabitat_2(nodito* listaAnimales, int hab)
{
    if(listaAnimales)
    {
        if(listaAnimales->dato.habitat == hab)
            mostrarAnimal(listaAnimales->dato);

        mostrarAnimalesHabitat_2(listaAnimales->sig, hab);
    }
}

void mostrarAnimalesHabitat(nodo* listaEspecies, int hab)
{
    if (listaEspecies)
    {
        mostrarAnimalesHabitat_2(listaEspecies->animales, hab);

        mostrarAnimalesHabitat(listaEspecies->sig, hab);
    }
}

/// funciones 5)

int retCantAnimales (nodito* listaAnimales)
{
    if (listaAnimales)
        return listaAnimales->dato.cantidad + retCantAnimales(listaAnimales->sig);
    else
        return 0;
}

void informarCantAnimalesEspecie (nodo* listaEspecies)
{
    nodo* mayor = listaEspecies;
    listaEspecies = listaEspecies->sig;

    while (listaEspecies)
    {
        if (retCantAnimales(mayor->animales) < retCantAnimales(listaEspecies->animales))
            mayor = listaEspecies;
        listaEspecies = listaEspecies->sig;
    }

    printf("LA ESPECIE CON MAS ANIMALES ES %s (%i)\n", mayor->especie, retCantAnimales(mayor->animales));
}

/// funciones 6)

void borrarAnimalesEspecie(nodito** listaAnimales)
{
    if (*listaAnimales)
    {
        nodito* aux = *listaAnimales;
        *listaAnimales = (*listaAnimales)->sig;
        free(aux);
        borrarAnimalesEspecie(listaAnimales);
    }
}

void eliminarEspecie (nodo** listaEspecies, char* especie)
{
    if (*listaEspecies)
    {
        if (strcmpi((*listaEspecies)->especie, especie) == 0)
        {
            nodo* aux = *listaEspecies;
            borrarAnimalesEspecie(&(*listaEspecies)->animales);
            *listaEspecies = (*listaEspecies)->sig;
            free(aux);
        }
        else
            eliminarEspecie(&(*listaEspecies)->sig, especie);
    }
}

/// main

int main()
{
    nodo* especies = NULL;

    despersistirYAgregar(&especies);

    persistirArchivoEspecies(especies);

    //mostrarAnimalesEspecie(especies, "peces");

/*    printf("MAR:\n");
    mostrarAnimalesHabitat(especies, 4); */

    eliminarEspecie(&especies, "peces");

    informarCantAnimalesEspecie(especies);

    //printf("\n\nID %i", especies->sig->sig->idEspecie);

    return 0;
}
