Este es un minero de CPU multiproceso para Litecoin y Bitcoin,
fork del cpuminer de referencia de Jeff Garzik.

Licencia: GPLv2. Consulte COPIA para obtener más detalles.

Descargas:   https://sourceforge.net/projects/cpuminer/files/ 
Árbol de Git:    https://github.com/pooler/cpuminer

Dependencias:
	libcurl			 http://curl.haxx.se/libcurl/ 
	jansson			 http://www.digip.org/jansson/
		(jansson está incluido en el árbol)

Instrucciones básicas de compilación * nix:
	./autogen.sh # solo es necesario si se construye desde el repositorio de git
	./nomacro.pl # en caso de que el ensamblador no admita macros
	./configure CFLAGS = "- O3" # ¡asegúrese de que -O3 sea una O y no un cero!
	hacer

Notas para usuarios de AIX:
	* Para construir un binario de 64 bits, exporte OBJECT_MODE = 64
	* Las opciones largas de estilo GNU no son compatibles, pero son accesibles
	  vía archivo de configuración

Instrucciones básicas de compilación de Windows, usando MinGW:
	Instale MinGW y el kit de herramientas para desarrolladores de MSYS ( http://www.mingw.org/ )
		* Asegúrese de tener mstcpip.h en MinGW \ include
	Si usa MinGW-w64, instale pthreads-w64
	Instale libcurl devel ( http://curl.haxx.se/download.html )
		* Asegúrese de tener libcurl.m4 en MinGW \ share \ aclocal
		* Asegúrese de tener curl-config en MinGW \ bin
	En el shell de MSYS, ejecute:
		./autogen.sh # solo es necesario si se construye desde el repositorio de git
		LIBCURL = "- lcurldll" ./configure CFLAGS = "- O3"
		hacer

Notas específicas de la arquitectura:
	ARM: Sin detección de CPU en tiempo de ejecución. El minero puede aprovechar
		de algunas instrucciones específicas para ARMv5E y procesadores posteriores,
		pero la decisión de usarlos se toma en tiempo de compilación,
		basado en macros definidas por el compilador.
		Para usar las instrucciones de NEON, agregue "-mfpu = neon" a CFLAGS.
	PowerPC: Sin detección de CPU en tiempo de ejecución.
		Para usar las instrucciones de AltiVec, agregue "-maltivec" a CFLAGS.
	x86: el minero comprueba el soporte de instrucciones SSE2 en tiempo de ejecución,
		y los usa si están disponibles.
	x86-64: el minero puede aprovechar las instrucciones AVX, AVX2 y XOP,
		pero solo si tanto la CPU como el sistema operativo los admiten.
		    * Linux admite AVX a partir de la versión 2.6.30 del kernel.
		    * FreeBSD admite AVX a partir de 9.1-RELEASE.
		    * Mac OS X agregó compatibilidad con AVX en la actualización 10.6.8.
		    * Windows admite AVX a partir de Windows 7 SP1 y
		      Windows Server 2008 R2 SP1.
		El script de configuración genera una advertencia si el ensamblador
		no admite algunos conjuntos de instrucciones. En ese caso, el minero
		todavía se pueden construir, pero las optimizaciones no disponibles se dejan de lado.
		El minero usa el motor VIA Padlock Hash cuando está disponible.

Instrucciones de uso: Ejecute "minerd --help" para ver las opciones.

Conexión a través de un proxy: use la opción --proxy.
Para utilizar un proxy SOCKS, agregue un prefijo socks4: // o socks5: // al host del proxy.
Los protocolos socks4a y socks5h, que permiten la resolución remota de nombres, también son
disponible desde libcurl 7.18.0.
Si no se especifica ningún protocolo, se asume que el proxy es un proxy HTTP.
Cuando no se usa la opción --proxy, el programa acepta http_proxy
y variables de entorno all_proxy.

También se tratan muchos problemas y preguntas frecuentes en el hilo del foro.
dedicado a este programa,
	https://bitcointalk.org/index.php?topic=55038.0
