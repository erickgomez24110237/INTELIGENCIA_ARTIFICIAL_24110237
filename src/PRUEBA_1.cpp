#include <SFML/Graphics.hpp>

int main() {
    // 1. Crear ventana
    sf::RenderWindow ventana(sf::VideoMode(800, 600), "Mascota Brincando");
    ventana.setFramerateLimit(60);

    // 2. Crear el personaje (un círculo rosa)
    sf::CircleShape personaje(40.f);
    personaje.setFillColor(sf::Color(255, 105, 180)); // Color rosa
    personaje.setOrigin(40.f, 40.f);                 // Origen en el centro del círculo

    // Posiciones en pantalla
    const float posX = 400.f;
    const float posY_suelo = 450.f;
    const float posY_salto = 320.f;

    personaje.setPosition(posX, posY_suelo);

    // 3. Relojes para controlar la animación y el tiempo
    sf::Clock relojSalto;      // Controla el intervalo de 2 segundos
    sf::Clock relojAnimacion;  // Controla la suavidad del brinco
    
    bool enElAire = false;

    while (ventana.isOpen()) {
        sf::Event evento;
        while (ventana.pollEvent(evento)) {
            if (evento.type == sf::Event::Closed) {
                ventana.close();
            }
        }

        // --- LÓGICA DEL BRINCO CADA 2 SEGUNDOS ---
        // Si han pasado 2 segundos, inicia el salto
        if (relojSalto.getElapsedTime().asSeconds() >= 2.0f && !enElAire) {
            enElAire = true;
            relojAnimacion.restart();
            relojSalto.restart();
        }

        // Animación de subida y bajada durante el salto (dura 0.5 segundos)
        if (enElAire) {
            float tiempoSalto = relojAnimacion.getElapsedTime().asSeconds();
            
            if (tiempoSalto < 0.25f) {
                // Subiendo
                personaje.setPosition(posX, posY_suelo - (tiempoSalto / 0.25f) * (posY_suelo - posY_salto));
            } else if (tiempoSalto < 0.50f) {
                // Bajando
                personaje.setPosition(posX, posY_salto + ((tiempoSalto - 0.25f) / 0.25f) * (posY_suelo - posY_salto));
            } else {
                // Termina el salto
                personaje.setPosition(posX, posY_suelo);
                enElAire = false;
            }
        }

        // --- RENDERIZADO ---
        ventana.clear(sf::Color(240, 240, 240));

        // Dibujar suelo de referencia
        sf::RectangleShape suelo(sf::Vector2f(800.f, 10.f));
        suelo.setPosition(0.f, 490.f);
        suelo.setFillColor(sf::Color(100, 100, 100));
        ventana.draw(suelo);

        // Dibujar personaje
        ventana.draw(personaje);

        ventana.display();
    }

    return 0;
}