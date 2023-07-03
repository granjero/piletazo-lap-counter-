# piletazo-lap-counter-
Contador de vueltas para la pileta

## Resumen
No soy capaz de llevar la cuenta de largos cuando nado. Entonces hice este aparatito.

### Funcionamiento
- El display tiene 3 dígitos: Uno para las series. Dos para la cantidad de largos.
- El microcontrolaador es un Nodemcu Wifi Esp8266 sólo porque era lo más barato que había en el mercado con la catidad suficiente de pines.
- El switch para incrementar los contadores es un switch magnético, que se puede activar con cualquier iman pequeño.

#### Largos
Para incrementar el contador de largos se acerca y se aleja el imán.

#### Series
Para incrementar el contador de series se acerca el imán y se espera más de 1.5 segundos para alejarlo. Esto incrementa el contador de series y pone en 0 el de largos.

#### Reset
Para volver a 0 ambos contadores se acerca el imán y se espera más de 3.5 segundos para alejarlo.



