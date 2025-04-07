   # Automatic sliding door for garage
   # Control de puerta corredera de garage
   - La apertura se realiza con mando RF es una placa aparte y utilizo la salida de contacto seco del relé.
   - El cierre se realiza automaticamente según el tiempo indicado por defecto 15 segundos.
   - Por seguridad tiene una célula fotoeléctrica que se activa en caso de interponerse en la barrera y evitar atrapamiento.
   - Dispone de un rele seco manejado por el controlador Zigbee, entonces la apertura del garage a través del tablero del vehículo con android auto y home assistant.
   - Se puede obviar el montaje de pantalla, no es necesario para el funcionamiento.
   - Puede elegir entre dos tipos de pantalla, la SSD1306 de 4 pines (https://s.click.aliexpress.com/e/_oB2gdBL) y la sh1106 de 7 pines (https://s.click.aliexpress.com/e/_oofWsrf)
   - La placa optoacoplada de tres reles que he utilizado es esta https://s.click.aliexpress.com/e/_oFawcp3
   - La fuente de alimentación 220v AC a 5v DC es esta https://s.click.aliexpress.com/e/_oFyz5GN
   - He utilizado un conector de 3 pines y el resto son de 2 pines, los conectores son estos https://s.click.aliexpress.com/e/_oph4j1X


     
## Imagen genérica del panel de control.
![Fotografía genérica del panel de control](https://github.com/ABarrosoG/sliding-door-garage/blob/main/fot-panel.jpg)
## Imagen detallada del panel de control.
![Fotografía detallada del panel de control](https://github.com/ABarrosoG/sliding-door-garage/blob/main/fot-pcb.jpg)
## Imagen de la pantalla de android auto con un botón para abrir la puerta de garaje, integrado con Homeassistant.
![Fotografía de la pantalla de android auto en el vehículo.](https://github.com/ABarrosoG/sliding-door-garage/blob/main/fot-pcb.jpg)




Tareas por desarrollar:
- [x] Incluir barrera de infrarojos para prevenir accidentes.
- [x] Incluir destellos led para avisar a los viandantes u otros vehículos de salidas de vehículos.
- [x] Integrable con dos tipos de pantalla OLED, la SSD1306 de 4 pines y la sh1106 de 7 pines.
- [ ] - [ ] Incluir entrada para que la puerta abra parcialmente para acceso de personas.



     > [!WARNING]
     > UTILICE EL CÓDIGO Y SUS RECURSOS CON SUMO CIUDADO, EXISTE RIESGO DE ELECTROCUCIÓN O DESCARGAS ELECTRICAS EN LA MANIPULACIÓN DE COMPONENTES ELÉCTRICOS, EXISTEN RIESGOS DE ATRAPAMIENTO O ESTRANGULAMIENTO EN EL ACCIONAMIENTO DEL MOTOR. TODO LO QUE USTED REALICE SERÁ BAJO SU PROPIA RESPONSABILIDAD, INCLUIDO EL MANEJO DE ESTE CÓDIGO FUENTE.
