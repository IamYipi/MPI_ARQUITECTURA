# MPI_ARQUITECTURA
## 1er Trabajo de la asignatura Arquitectura de Computadores 3º Ingeniería Informática


## [Enunciado](EnunciadoPrácticaMP2122I.pdf)

## Comienzo
Primero de todo tener instalado **MPI**. Instalar con el comando:
```sh
sudo apt-get install openmpi-bin
```
## TRABAJO


### PROCESO 0
- [ ]  Creación Proceso 0 **E/S**, guardado de palabra y su respectiva longitud.
- [ ]  Notificación *Rol* otros procesos
- [ ]  Notificación *Palabra y Longitud* a los **Comprobadores**
- [ ]  Modo pistas o no pista
- Quedarse a la espera de:
- [ ]  Recibir cadena con caracteres ya encontrados
- [ ]  Recibimiento de la palabra ya encontrada
- Proceder a:
- [ ]  Envío de letras encontradas al resto de generadores
- [ ]  Envio **señal terminación** Comprobadores y Generadores, recibir estadísticas de cada grupo


### RESTO PROCESOS
#### COMPROBADORES
- [ ]  Recibir **Longitud** de la Palabra
- [ ]  Recibir que son *Comprobadores*
- [ ]  Recibir **Palabra** a Comprobar
- Quedarse a la espera de:
- [ ]  Proceso **0** le mande acabar
- [ ]  Proceso **Generador** les mande una palabra
- [ ]  Recibo señal terminación
- Tras recibir la palabra:
- [ ]  Comprobar palabra
- [ ]  **FORZAR ESPERA PARA DAR PESO AL CALCULO**
- [ ]  Devolver respuesta a los *Generadores* con **caracteres acertados en sus posiciones**
- Tras recibo señal terminación Proceso 0:
- [ ] Envío estadísticas proceso 0

### GENERADORES
- [ ]  Recibir **Longitud** de la Palabra
- [ ]  Recibir que son *Generadores*
- [ ]  Asignar Comprobador
- Tener en cuenta:
- [ ]  Recibo señal terminación
- [ ]  Recibo pistas y actualizar cadena caracteres detectados
- Función:
- [ ]  Generar palabra de longitud indicada (Cuidado semilla)
- [ ]  Tener en cuenta caracteres ya encontrados
- [ ]  **FORZAR ESPERA PARA DAR PESO AL CALCULO**
- [ ]  Mandar Consulta proceso Comprobador que se le asignó
- [ ]  Recibir respuesta y **actualizar** cadena de caracteres encontrados
- Tras recibir señal terminación:
- [ ]  Envío *estadísticas* a proceso 0


### CONCLUSIONES ESTADÍSTICAS
(Falta por completar...)
