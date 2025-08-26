#ifndef __AST__
#define __AST__
#include "string-auxiliar.h"

/**
 * Se define los tipos de nodos para el AST
 */
typedef enum {
    /* Estos son los tipos de nodo raíz */
    AST_DEFF, AST_DEFL, AST_APPLY, AST_SEARCH, AST_QUIT,

    /* Estos son tipos de nodo internos */
    AST_FUNCS, AST_FUNC, AST_REP, AST_LISTAS, AST_LISTA, AST_ELEMENTOS,

    /* Esto son tipos de nodos hoja */
    AST_DEF, AST_PRIMITIVA, AST_DIGITO, AST_ERROR
    
} TipoDeNodo;

/**
 * Para el recorrido del árbol, se define los tres tipos de recorridos posibles
 */
typedef enum {
    PREORDER, INORDER, POSORDER
} TipoDeRecorrido;

typedef void (*FuncionVisitante)(String lexema, TipoDeNodo tipo);

/**
 * Se define la cant. de hijos posibles que puede tener cada nodo
 */
#define CANT_HIJOS 3

/**
 * Se define la estructura general de un nodo de un árbol. Tiene un máx. de
 * 3 hijos cada uno
 */
typedef struct _ASTNodo {
    TipoDeNodo tipo;
    String lexema;
    int num_hijos;
    struct _ASTNodo* hijos[CANT_HIJOS];
} ASTNodo;

/**
 * También, por conveniencia, se define un nombre para el nodo raíz del AST
 */
typedef ASTNodo* ASTTree;

/**
 * crear_arbol: void -> ASTTree
 * Crea un árbol vacío para poco a poco llenarlo
 */
ASTTree crear_arbol();

/**
 * crear_nodo_arbol: String TipoDeNodo -> ASTNodo*
 * Crea un nodo del árbol con un tipo de nodo y, si se requiere, el lexema del nodo
 */
ASTNodo* crear_nodo_arbol(String lexema, TipoDeNodo tipo);

/**
 * anadir_hijo: ASTNodo* ASTNodo* -> void
 * Añade al padre un hijo más. Si no se puede añadir más hijos. Si no hay más espacio, no hace nada
 */
void anadir_hijo(ASTNodo* padre, ASTNodo* hijo);

/**
 * destruir_arbol: ASTNodo* -> void
 * Destruye el árbol con la raíz dada
 */
void destruir_arbol(ASTTree arbol);

#endif /* __AST__ */