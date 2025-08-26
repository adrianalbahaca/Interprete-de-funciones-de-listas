#include "parser.h"
#include "estructuras/ast.h"
#include "estructuras/string-auxiliar.h"
#include "estructuras/tokens.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

/**
 * Para ver cada token, se declara de forma global la lista de tokens a analizar
 */
static TokenNodo *siguiente;

/* ------ Funciones Auxiliares ------ */

/**
 * next: TokenNodo* -> void
 * Si se le pasa un nodo de la lista de tokens, lo asigna en la variable
 * universal Sino, se mueve al siguiente nodo
 */
void next(TokenNodo *lista) {
  if (lista != NULL) {
    siguiente = lista;
  } 
  else if (siguiente == NULL) {
    return;
  }
  else {
    siguiente = siguiente->next;
  }
  return;
}

/**
 * match: TipoDeToken -> bool
 * Si el tipo de nodo coincide con el tipo de token, se mueve al siguiente y
 * retorna true Sino, no se hace nada y se retorna false
 */
bool match(TipoDeToken tipo) {
  if (siguiente == NULL) return false;
  if (siguiente->tipo == tipo)
    return true;
  else
    return false;
}

/**
 * error: String ASTTree -> void
 * Si ocurre algo, recibe el árbol y borra todo el árbol, si se estaba formando.
 * De lo contrario, no añade nada
 */
void error(String mensaje, ASTTree arbol) {
  printf("%s\n", (String)mensaje);
  if (arbol != NULL)
    destruir_arbol(arbol);
  return;
}

/* -------------------------------------------- */

/* Reglas gramaticales */

/**
 * Deff ::= 'deff' DEF '=' Funcs | 'deff' DEF '=' Funcs "<" Funcs ">" Funcs
 */
ASTTree deff();

/**
 * Funcs ::= DEF Funcs | PRIMITIVA Funcs | DEF | PRIMITIVA
 */
ASTTree funcs();

/**
 * Defl ::= "defl" DEF IGUAL COR_ABRE Elementos COR_CIERRA
 */
ASTTree defl();

/**
 * Elementos ::= DIGITO COMA Elementos | DIGITO | e
 */
ASTTree elementos();

/**
 * Apply ::= "apply" DEF DEF | "apply" PRIMITIVA DEF | "apply" DEF LISTA | "apply" PRIMITIVA LISTA
 */
ASTTree apply();

/**
 * Search ::= "search" LLAVE_ABRE Listas LLAVE_CIERRA 
 */
ASTTree search();

/**
 * Listas ::= Lista COMA Lista PUNTO_COMA Listas | Lista COMA Lista #
 */
ASTTree listas();

/**
 * Lista ::= DEF | COR_ABRE Elementos COR_CIERRA #
 */
ASTTree lista();

/**
 * Def ::= String que se guardará en un Hash después
 */
ASTNodo* def(String definicion);

/**
 * Primitiva ::= 0i | 0d | Si | Sd | Di | Dd
 */
ASTNodo* primitiva(String funcion);

/**
 * Quit: Comando para cerrar todo
 */
ASTTree quit();

/*---------------------------------------------------------------*/

/**
 * parse: TokenList -> ASTTree
 * Toma una lista de tokens y la convierte en un árbol de sintáxis abstracto
 * para poder procesarlo Sentencia ::= Definicion ';' | Aplicacion  ';' |
 * Busqueda ';'
 */
ASTTree parse(TokenList tokens) {

  // Asignar lista de tokens a procesar y a inicializar el arbol
  next(tokens->head);
  ASTTree ast = crear_arbol();

  // Luego, empezamos a determinar el caso que tenemos
  switch (siguiente->tipo) {
  case TOKEN_DEFF:
    ast = deff();
    if (!match(TOKEN_PUNTO_COMA)) {
      if (siguiente == NULL) error("Falta el punto y coma para finalizar el comando", ast);
      else error("Error en la definición de la función. Intente de nuevo", ast);
      ast = NULL;
    }
    next(NULL);
    break;

  case TOKEN_DEFL:
    ast = defl();
    if (!match(TOKEN_PUNTO_COMA)) {
      if (siguiente == NULL) error("Falta el punto y coma para finalizar el comando", ast);
      else error("Error en la definición de la lista. Intente de nuevo", ast); 
      ast = NULL;
    }
    next(NULL);
    break;

  case TOKEN_APPLY:
    ast = apply();
    if (!match(TOKEN_PUNTO_COMA)) {
      if (siguiente == NULL) error("Falta el punto y coma para finalizar el comando", ast);
      else error("Error en la definición de la aplicación. Intente de nuevo", ast);
      ast = NULL;
    }
    next(NULL);
    break;

  case TOKEN_SEARCH:
    ast = search();
    if (!match(TOKEN_PUNTO_COMA)) {
      if (siguiente == NULL) error("Falta el punto y coma para finalizar el comando", ast);
      else error("Error en la definición de la búsqueda. Intente de nuevo", ast);
      ast = NULL;
    }
    next(NULL);
    break;

  case TOKEN_QUIT:
    ast = quit();
    if (!match(TOKEN_PUNTO_COMA)) {
      if (siguiente == NULL) error("Falta el punto y coma para finalizar el comando", ast);
      else error("Error en el comando 'quit'. Intente de nuevo", ast);
      ast = NULL;
    }
    next(NULL);
    break;
  default:
    error("ERROR: Comando no válido. Intente de nuevo", ast);
    return NULL;
    break;
  }

  if (ast == NULL) {
    printf("Parece que hubo un error con el parsing. Intente de nuevo\n");
  } else {
    printf("Parsing con éxito\n");
    siguiente = NULL;
  }

  destruir_lista(tokens);
  return ast;
}

/**
 * deff: void -> ASTTree
 * Arma poco a poco el AST que representa el comando 'deff'
 * Se representa con la siguiente gramática:
 * Deff ::= 'deff' DEF '=' Funcs | 'deff' DEF '=' Repeticion
 */
ASTTree deff() {
  // Se chequea por el tipo de token deff por si las moscas
  if (!match(TOKEN_DEFF))
    return NULL;
  ASTTree arbolDeff = crear_arbol();
  arbolDeff = crear_nodo_arbol(NULL, AST_DEFF);
  next(NULL);

  // Luego, se chequea por una definicion apropiada
  if (!match(TOKEN_DEF)) {
    error("Definición no válida", arbolDeff);
    return NULL;
  }

  ASTNodo *nodoDef = def(siguiente->token);
  if (nodoDef == NULL) {
    error("Definición incorrecta", arbolDeff);
    return NULL;
  }
  anadir_hijo(arbolDeff, nodoDef);
  next(NULL);

  // Sigue el chequeo del igual
  if (!match(TOKEN_IGUAL)) {
    error("Falta el símbolo '='", arbolDeff);
    return NULL;
  }
  next(NULL);

  // Finalmente, se chequea si sigue una definicion de funciones adecuada
  ASTTree arbolFunciones = funcs();

  // En caso de detectar un '<', empezar a considerar una función repetición
  if (match(TOKEN_ANG_ABRE)) {
    ASTTree arbolRep = crear_arbol();
    arbolRep = crear_nodo_arbol(NULL, AST_REP);

    anadir_hijo(arbolRep, arbolFunciones);
    anadir_hijo(arbolDeff, arbolRep);

    next(NULL);

    ASTTree arbolFunciones2 = funcs();
    anadir_hijo(arbolRep, arbolFunciones2);

    if (!match(TOKEN_ANG_CIERRA)) {
      error("Falta cerrar con '>'. Intente de nuevo", arbolDeff);
      return NULL;
    }
    next(NULL);

    ASTTree arbolFunciones3 = funcs();
    anadir_hijo(arbolRep, arbolFunciones3);

  } else {
    anadir_hijo(arbolDeff, arbolFunciones);
  }

  return arbolDeff;
}

/**
 * defl: void -> ASTTree
 * Arma un AST que representa el comando 'defl'
 * Se representa con la siguiente gramática:
 * Defl ::= "defl" DEF IGUAL COR_ABRE Elementos COR_CIERRA
 */
ASTTree defl() {
  // Se chequea que el tipo sea el adecuado
  if (!match(TOKEN_DEFL))
    return NULL;
  ASTTree arbolDefl = crear_arbol();
  arbolDefl = crear_nodo_arbol(NULL, AST_DEFL);
  next(NULL);

  // Luego se revisa si se tiene una definición correcta
  if(!match(TOKEN_DEF)) {
    error("Definición incorrecta", arbolDefl);
    return NULL;
  }
  ASTNodo *nodoDef = def(siguiente->token);

  anadir_hijo(arbolDefl, nodoDef);
  next(NULL);

  // Luego, se verifica que haya un símbolo '='
  if (!match(TOKEN_IGUAL)) {
    error("Falta el símbolo '='", arbolDefl);
    return NULL;
  }
  next(NULL);

  // Se verifica que se abra con un '['
  if (!match(TOKEN_COR_ABRE)) {
    error("No se ha abierto la lista con '['", arbolDefl);
    return NULL;
  }
  next(NULL);

  ASTNodo* arbolElementos = elementos();
  anadir_hijo(arbolDefl, arbolElementos);

  if(!match(TOKEN_COR_CIERRA)) {
    return NULL;
  }
  next(NULL);

  return arbolDefl;
}

/**
 * apply: void -> ASTTree
 * Arma un AST que representa el comando 'apply'
 * Se representa con la siguiente gramática:
 * Apply ::= "apply" DEF DEF | "apply" PRIMITIVA DEF | "apply" DEF LISTA | "apply" PRIMITIVA LISTA
 */
ASTTree apply() {
  if(!match(TOKEN_APPLY)) 
    return NULL;
  ASTTree arbolApply = crear_arbol();
  arbolApply = crear_nodo_arbol(NULL, AST_APPLY);
  next(NULL);
  
  // Primero, verifica que use una Definición o una Primitiva
  if(match(TOKEN_DEF)) {
    ASTNodo* nodoDef = def(siguiente->token);
    anadir_hijo(arbolApply, nodoDef);
    next(NULL);
  }
  else if(match(TOKEN_PRIMITIVA)) {
    ASTNodo* nodoPrim = primitiva(siguiente->token);
    anadir_hijo(arbolApply, nodoPrim);
    next(NULL);
  }
  else {
    error("Definición de función no válida", arbolApply);
    return NULL;
  }

  // Luego, verifica si es un tipo de lista válida
  if(match(TOKEN_DEF)) {
    ASTNodo* nodoDef = def(siguiente->token);
    anadir_hijo(arbolApply, nodoDef);
    next(NULL);
  }
  else if(match(TOKEN_COR_ABRE)) {
    next(NULL);

    ASTTree arbolElementos = elementos();
    anadir_hijo(arbolApply, arbolElementos);

    if (!match(TOKEN_COR_CIERRA)) {
      error("No se ha cerrado con ']'", arbolApply);
      return NULL;
    }
    next(NULL);
  }
  else {
    error("No se definió de forma adecuada una lista a aplicar la función", arbolApply);
    return NULL;
  }

  return arbolApply;
}

/**
 * search: void -> ASTTree
 * Crea un arbol AST que representa el comando 'search'
 * La gramática que sigue es la siguiente:
 * Search ::= "search" LLAVE_ABRE Listas LLAVE_CIERRA 
 */
ASTTree search() {
  if(!match(TOKEN_SEARCH))
    return NULL;
  ASTTree arbolSearch = crear_arbol();
  arbolSearch = crear_nodo_arbol(NULL, AST_SEARCH);
  next(NULL);

  if(!match(TOKEN_LLAVE_ABRE)) {
    error("No se colocó las funciones en '{}'", arbolSearch);
    return NULL;
  }
  next(NULL);

  ASTTree arbolListas = listas();
  if (arbolListas == NULL) {
    error("Error en definición de listas", arbolSearch);
    return NULL;
  }
  anadir_hijo(arbolSearch, arbolListas);
  
  if(!match(TOKEN_LLAVE_CIERRA)) {
    error("No se cerró con '}'", arbolSearch);
    return NULL;
  }

  next(NULL);
  return arbolSearch;
}

/**
 * funcs: void -> ASTTree
 * Crea un arbol con el listado de funciones
 * Sigue la siguiente regla gramática:
 * Funcs ::= DEF Funcs | PRIMITIVA Funcs | DEF | PRIMITIVA
 */
ASTTree funcs() {
  ASTTree nodoFuncion = crear_arbol();

  if(match(TOKEN_DEF)) {
    nodoFuncion = crear_nodo_arbol(siguiente->token, AST_DEF);
    next(NULL);
  }
  else if(match(TOKEN_PRIMITIVA)) {
    nodoFuncion = crear_nodo_arbol(siguiente->token, AST_PRIMITIVA);
    next(NULL);
  }
  else return nodoFuncion;

  ASTTree nodoFunciones = funcs();
  if (nodoFunciones != NULL) {
    anadir_hijo(nodoFuncion, nodoFunciones);
  }
  return nodoFuncion;
}

/**
 * Elementos: void -> ASTTree
 * Retorna un árbol de elementos de una lista
 * Representa la siguiente regla gramatical:
 * Elementos ::= DIGITO COMA Elementos | DIGITO | e
 */
ASTTree elementos() {
  ASTNodo* nodoDigito = crear_arbol();
  if (match(TOKEN_DIGITO)) {
    nodoDigito = crear_nodo_arbol(siguiente->token, AST_DIGITO);
    next(NULL);
  }

  if (match(TOKEN_COMA)) {
    next(NULL);
    ASTTree arbolDigitos = elementos();
    if (arbolDigitos != NULL)
      anadir_hijo(arbolDigitos, nodoDigito);

    return arbolDigitos;
  }
  else {
    return nodoDigito;
  }
}

/**
 * listas: void -> ASTTree
 * Crea un arbol que contiene cada par de listas
 * Sigue la siguiente regla gramatical:
 * Listas ::= Lista COMA Lista PUNTO_COMA Listas | Lista COMA Lista
 */
ASTTree listas() {
  ASTTree arbolListas = crear_arbol();
  arbolListas = crear_nodo_arbol(NULL, AST_LISTAS);

  ASTNodo* nodoLista1 = lista();
  anadir_hijo(arbolListas, nodoLista1);
  next(NULL);

  if (nodoLista1 != NULL && nodoLista1->tipo == AST_ERROR) {
    error("Hubo un error en la definición de lista", arbolListas);
    return NULL;
  }
  else if (!match(TOKEN_COMA)) {
    error("Cada lista se debe separar con una coma", arbolListas);
    return NULL;
  }
  next(NULL);

  ASTNodo* nodoLista2 = lista();
  anadir_hijo(arbolListas, nodoLista2);
  if (nodoLista2->tipo == AST_ERROR) {
    error("Hubo un error en la definición de lista", arbolListas);
    return NULL;
  }
  next(NULL);

  if (match(TOKEN_PUNTO_COMA)) {
    next(NULL);
    ASTTree arbolListasSig = listas();
    if (arbolListasSig == NULL) {
      destruir_arbol(arbolListas);
      return NULL;
    }
    anadir_hijo(arbolListas, arbolListasSig);
  }

  return arbolListas;
}

/**
 * lista: void -> ASTTree
 * Crea o un nodo de tipo definición o una lista nueva
 * Sigue la siguiente regla gramatical:
 * Lista ::= DEF | COR_ABRE Elementos COR_CIERRA
 */
ASTTree lista() {
  ASTTree arbolLista = crear_arbol();
  
  if(match(TOKEN_DEF)) {
    arbolLista = def(siguiente->token);
  }
  else if (match(TOKEN_COR_ABRE)) {
    next(NULL);
    arbolLista = elementos();
    if (!match(TOKEN_COR_CIERRA)) {
      error("No se cerró con ']'", arbolLista);
      arbolLista = crear_arbol();
      arbolLista = crear_nodo_arbol(NULL, AST_ERROR);
      return NULL;
    }
  }
  else {
    error("No se ha definido una lista adecuadamente", arbolLista);
    return NULL;
  }

  return arbolLista;
}

/**
 * def: String -> ASTNodo*
 * Crea un nodo con una definición que incluye un String
 */
ASTNodo* def(String definicion) {
  ASTNodo* nodoDef = crear_nodo_arbol(definicion, AST_DEF);
  return nodoDef;
}

/**
 * primitiva: String -> ASTNodo*
 * Crea un nodo con un tipo de función primitiva
 */
ASTNodo* primitiva(String funcion) {
  ASTNodo* nodoPrim = crear_nodo_arbol(funcion, AST_PRIMITIVA);
  return nodoPrim;
}

/**
 * quit: void -> ASTTree
 * Crea un árbol con un único nodo de tipo QUIT
 */
ASTTree quit() {
  if(!match(TOKEN_QUIT)) return NULL;
  ASTTree arbolQuit = crear_arbol();
  arbolQuit = crear_nodo_arbol(NULL, AST_QUIT);
  next(NULL);
  return arbolQuit;
}