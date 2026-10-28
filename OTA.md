# Actualizaciones OTA del D1 mini

La versión 1.0.9 permite actualizar el firmware por HTTP autenticado. El editor y
la API están en `http://IP_DEL_ESP/` (puerto 80). No requiere una conexión de vuelta
desde el ESP al ordenador, por lo que funciona entre subredes si puedes acceder
al editor y la red permite enviar el archivo por HTTP.

## Credenciales locales

La primera compilación genera `ota.local.json` con host y contraseña aleatoria.
Se excluye de Git y no se imprime en las cargas normales. Conserva una copia
privada: las siguientes compilaciones y cargas reutilizan esa contraseña.
`include/LocalOtaConfig.h` contiene los hashes para autenticar las cargas y también
se excluye de Git. Cambiar el host no requiere reinstalar el firmware. Para
cambiar la contraseña del dispositivo de forma sencilla, recompila y carga por
USB. El usuario del actualizador HTTP es `admin`.

## Primera carga por USB

```sh
pio run -e d1_mini -t upload
```

## Siguientes cargas por WiFi

En PlatformIO: **Project Tasks → d1_mini_ota → General → Upload**.

```sh
pio run -e d1_mini_ota -t upload
```

Host y contraseña se leen automáticamente desde `ota.local.json`. El entorno
USB sigue siendo el predeterminado para recuperación. No uses `-v` con OTA
clásica: las herramientas pueden mostrar argumentos con la contraseña.

También puedes abrir `http://IP_DEL_ESP/update`, identificarte con `admin` y la
contraseña del archivo local, y subir el archivo generado
`.pio/build/d1_mini/firmware.bin`. Esta pantalla acepta únicamente firmware,
no imágenes del sistema de archivos.

La actualización apaga temporalmente la matriz y reinicia al terminar. El
firmware normal conserva LittleFS: WiFi, configuración, rotación y animaciones.
El entorno OTA bloquea `uploadfs` y `uploadfsota`, que reemplazarían esos datos.
`/api/status` muestra el estado OTA, sin revelar la contraseña.

## OTA clásica opcional

`pio run -e d1_mini_ota_native -t upload` usa ArduinoOTA (UDP 8266) y requiere
conexión TCP desde el ESP al ordenador en el puerto 8266. Entre las subredes 10.205.12.x y 10.205.17.x, la contraseña fue aceptada pero
la conexión de vuelta no se completó. En la misma subred 10.205.17.x se ha
verificado una carga nativa completa. También se ha verificado una carga HTTP
completa con `d1_mini_ota`.

OTA clásica solo se inicia al conectar a la WiFi configurada. Usa cualquier
actualización OTA en una red de confianza y sin publicar los puertos en Internet.
HTTP Digest autentica la carga, pero no cifra el firmware ni el tráfico.
