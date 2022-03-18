# MPI_ARQUITECTURA
## 1er Trabajo de la asignatura Arquitectura de Computadores 3º Ingeniería Informática


## [Enunciado](EnunciadoPrácticaMP2122I.pdf)

## Comienzo
Primero de todo tener instalado **MPI**. Instalar con el comando:
```sh
sudo apt-get install openmpi-bin
```
## TRABAJO
- [ X ] Salida por pantalla como [Enunciado](EnunciadoPrácticaMP2122I.pdf)

### PROCESO 0
- [ X ]  Creación Proceso 0 **E/S**, guardado de palabra y su respectiva longitud.
- [ X ]  Notificación *Rol* otros procesos
- [ X ]  Notificación *Palabra y Longitud* a los **Comprobadores**
- [ X ]  Modo pista
- [ X ]  Modo No Pista
- Quedarse a la espera de:
- [ X ]  Recibir cadena con caracteres ya encontrados
- [ X ]  Recibimiento de la palabra ya encontrada
- Proceder a:
- [ X ]  Envío de letras encontradas al resto de generadores
- [ X ]  Envio **señal terminación** Comprobadores y Generadores, recibir estadísticas de cada grupo


### RESTO PROCESOS
#### COMPROBADORES
- [ X ]  Recibir **Longitud** de la Palabra
- [ X ]  Recibir que son *Comprobadores*
- [ X ]  Recibir **Palabra** a Comprobar
- Quedarse a la espera de:
- [ X ]  Proceso **0** le mande acabar
- [ X ]  Proceso **Generador** les mande una palabra
- [ X ]  Recibo señal terminación
- Tras recibir la palabra:
- [ X ]  Comprobar palabra
- [ X ]  **FORZAR ESPERA PARA DAR PESO AL CALCULO**
- [ X ]  Devolver respuesta a los *Generadores* con **caracteres acertados en sus posiciones**
- Tras recibo señal terminación Proceso 0:
- [ X ] Envío estadísticas proceso 0

### GENERADORES
- [ X ]  Recibir **Longitud** de la Palabra
- [ X ]  Recibir que son *Generadores*
- [ X ]  Asignar Comprobador
- Tener en cuenta:
- [ X ]  Recibo señal terminación
- [ x ]  Recibo pistas y actualizar cadena caracteres detectados
- Función:
- [ X ]  Generar palabra de longitud indicada (Cuidado semilla)
- [ X ]  Tener en cuenta caracteres ya encontrados
- [ X ]  **FORZAR ESPERA PARA DAR PESO AL CALCULO**
- [ X ]  Mandar Consulta proceso Comprobador que se le asignó
- [ X ]  Recibir respuesta y **actualizar** cadena de caracteres encontrados
- Tras recibir señal terminación:
- [ X ]  Envío *estadísticas* a proceso 0


### CONCLUSIONES ESTADÍSTICAS
(Falta por completar...)
