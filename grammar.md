# Esta es la gramática que se va a usar en el parser:
## Sentencia principal:
```
Sentencia ::= Deff PUNTO_COMA | Defl PUNTO_COMA | Apply PUNTO_COMA | Search PUNTO_COMA
```
## Definición de funciones
```
Deff ::= "deff" DEF IGUAL Funcs | "deff" DEF IGUAL Funcs "<" Funcs ">" Funcs
Funcs ::= DEF Funcs | PRIMITIVA Funcs | DEF | PRIMITIVA
```
## Definición de listas
```
Defl ::= "defl" DEF IGUAL COR_ABRE Elementos COR_CIERRA
Elementos ::= DIGITO COMA Elementos | DIGITO | e
```
## Definición de aplicación
```
Apply ::= "apply" DEF DEF | "apply" PRIMITIVA DEF | "apply" DEF LISTA | "apply" PRIMITIVA LISTA
```
## Definición de búsqueda
```
Search ::= "search" LLAVE_ABRE Listas LLAVE_CIERRA
Listas ::= Lista COMA Lista PUNTO_COMA Listas | Lista COMA Lista
Lista ::= DEF | COR_ABRE Elementos COR_CIERRA
```
## Terminales
```
COR_ABRE ::= "["
COR_CIERRA ::= "]"
LLAVE_ABRE ::= "{"
LLAVE_CIERRA ::= "}"
COMA ::= ","
PUNTO_COMA ::= ";"
DEF ::= String que se guardará en un Hash después
DIGITO ::= Entero dentro de una lista
PRIMITIVA ::= 0i | 0d | Si | Sd | Di | Dd
```