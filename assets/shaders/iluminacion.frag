#version 120

// Texturas
uniform sampler2D textura;     // Textura difusa principal
uniform sampler2D normalMap;   // Mapa de normales

// Dimensiones de textura
uniform vec2 textureSize;      // Tamaño completo de la textura en pixels
uniform vec4 textureRect;      // Sub-rectángulo del frame actual (x, y, w, h) en pixels

// Parámetros de la luz
uniform vec2 lightPos;         // Posición de la luz en el mundo (pixels)
uniform vec3 lightColor;       // Color de la luz (R, G, B)
uniform vec3 ambientColor;     // Color de la luz ambiental (R, G, B)
uniform vec3 lightFalloff;     // Factores de atenuación: (constante, lineal, cuadrática)
uniform float lightHeight;     // Altura de la luz sobre el plano 2D (Z)

// Propiedades geométricas del Sprite
uniform vec2 spritePos;        // Posición central/world de la entidad
uniform vec2 spriteOrigin;     // Pivote de origen del sprite en pixels locales (por defecto centro)
uniform vec2 spriteScale;      // Escala aplicada al sprite (x, y)

void main()
{
    // 1. Muestrear el color difuso del sprite
    vec4 diffuseColor = texture2D(textura, gl_TexCoord[0].xy);
    
    // Descartar pixels completamente transparentes para evitar sobrecoste y artifacts de iluminación
    if (diffuseColor.a < 0.05) {
        discard;
    }

    // 2. Muestrear y decodificar el vector normal desde el mapa de normales
    // Nota: Muestreamos la misma coordenada porque el mapa de normales tiene idéntica estructura de frames.
    vec4 normalColor = texture2D(normalMap, gl_TexCoord[0].xy);
    
    // Decodificar el vector de normales de [0, 1] a rango bidireccional [-1, 1]
    vec3 normal = normalize(normalColor.rgb * 2.0 - 1.0);
    
    // Invertir el eje Y de la normal para alinear el mapa de normales (Y+ arriba)
    // con el sistema de coordenadas de pantalla de SFML (Y+ abajo)
    normal.y = -normal.y;
    
    // Si el mapa de normales está vacío o no se ha cargado correctamente (color plano/negro),
    // asumimos una normal por defecto apuntando directamente al jugador (0, 0, 1)
    if (length(normalColor.rgb) < 0.01) {
        normal = vec3(0.0, 0.0, 1.0);
    }
    
    // Ajustar/invertir componentes si el sprite está volteado horizontal o verticalmente
    if (spriteScale.x < 0.0) {
        normal.x = -normal.x;
    }
    if (spriteScale.y < 0.0) {
        normal.y = -normal.y;
    }

    // 3. Reconstruir la posición del pixel actual en el espacio del mundo
    // Convertir de coordenadas de textura normalizadas [0,1] de la hoja a pixels reales en la hoja
    vec2 sheetPixelPos = gl_TexCoord[0].xy * textureSize;
    
    // Calcular el desplazamiento respecto al inicio del frame actual (top-left)
    vec2 localPixelPos = sheetPixelPos - textureRect.xy;
    
    // Calcular la posición relativa al origen del pivote y aplicar escala
    vec2 localPos = (localPixelPos - spriteOrigin) * spriteScale;
    
    // Obtener posición del pixel en coordenadas del mundo
    vec2 pixelWorldPos = spritePos + localPos;

    // 4. Calcular dirección e intensidad de la luz
    // El vector va desde el pixel hasta la luz en 3D
    vec3 lightDir = vec3(lightPos - pixelWorldPos, lightHeight);
    float dist = length(lightDir);
    
    // Normalizar vector de dirección de luz
    vec3 L = normalize(lightDir);

    // 5. Calcular la atenuación de la luz por distancia (fórmula cuadrática clásica)
    float attenuation = 1.0 / (lightFalloff.x + (lightFalloff.y * dist) + (lightFalloff.z * dist * dist));

    // 6. Contribución difusa usando el producto punto (Lambertian 2D)
    float D = max(dot(normal, L), 0.0);
    vec3 diffuse = D * lightColor * attenuation;

    // 7. Mezclar luz ambiental con contribución de la luz puntual
    vec3 finalColor = (ambientColor + diffuse) * diffuseColor.rgb;

    // 8. Escribir el color final con la opacidad del sprite original
    gl_FragColor = vec4(finalColor, diffuseColor.a);
}
