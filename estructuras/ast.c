#include "ast.h"
#include "string-auxiliar.h"
#include <stdlib.h>
#include <assert.h>

/**
 * crear_arbol: void -> ASTTree
 * Crea un árbol vacío para poco a poco llenarlo
 */
ASTTree crear_arbol() {
    return NULL;
}

/**
 * crear_nodo_arbol: String TipoDeNodo -> ASTNodo*
 * Crea un nodo del árbol sin hijos con un tipo de nodo y, si se requiere, el lexema del nodo
 */
ASTNodo* crear_nodo_arbol(String lexema, TipoDeNodo tipo) {
    ASTNodo* nodo = malloc(sizeof(ASTNodo));
    assert(nodo != NULL);
    for (int i = 0; i < CANT_HIJOS; i++) {
        nodo->hijos[i] = NULL;
    }
    if (lexema != NULL) nodo->lexema = str_dup(lexema);
    else nodo->lexema = NULL;
    nodo->tipo = tipo;
    nodo->num_hijos = 0;
    return nodo;
}

/**
 * anadir_hijo: ASTNodo* ASTNodo* -> void
 * Añade al padre un hijo más. Si no se puede añadir más hijos. Si no hay más espacio, no hace nada
 */
void anadir_hijo(ASTNodo* padre, ASTNodo* hijo) {
    if (padre->num_hijos < CANT_HIJOS) {
        padre->hijos[padre->num_hijos] = hijo;
        padre->num_hijos++;
    }
    return;
}

/**
 * destruir_arbol: ASTNodo* -> void
 * Destruye el árbol con la raíz dada
 */
void destruir_arbol(ASTTree arbol) {
    if (arbol == NULL) return;
    for (int i = 0; i < arbol->num_hijos; i++) {
        destruir_arbol(arbol->hijos[i]);
    }
    if (arbol->lexema != NULL) liberar_string(arbol->lexema);
    free(arbol);
    return;
}

/**
 * recorrer_arbol: ASTTree OrdenDeRecorrido FuncionVisitante -> void*
 * Recorre un AST y aplica una función en cada nodo que consigue
 */
