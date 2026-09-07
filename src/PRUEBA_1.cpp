#include <SFML/Graphics.hpp>
#include <iostream>
#include <map>
#include <string>

enum class MarioState {
    SONRIENDO,
    HAMBRIADO,
    COMIENDO,
    DURMIENDO,
    ENFERMANDOSE,
    CURANDOSE,
    BRINCANDO,
    MURIENDO
};

enum class DecisionType {
    NINGUNA,
    HAMBRE,
    ENFERMO,
    SUENO,
    JUGAR
};

// ==========================================
// CLASE 1: Button (Sin dependencia de sf::Font)
// ==========================================
class Button {
private:
    sf::RectangleShape shape;
    sf::Text text;
    bool visible;

public:
    Button(float x, float y, float width, float height, const std::string& btnText, const sf::Font* font, sf::Color bgColor) {
        shape.setPosition(x, y);
        shape.setSize(sf::Vector2f(width, height));
        shape.setFillColor(bgColor);
        shape.setOutlineThickness(2.f);
        shape.setOutlineColor(sf::Color::White);

        if (font) {
            text.setFont(*font);
            text.setString(btnText);
            text.setCharacterSize(14);
            text.setFillColor(sf::Color::White);

            sf::FloatRect textRect = text.getLocalBounds();
            text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
            text.setPosition(x + width / 2.0f, y + height / 2.0f);
        }

        visible = false;
    }

    void setVisible(bool v) { visible = v; }
    bool isVisible() const { return visible; }

    void setText(const std::string& newText, const sf::Font* font) {
        if (font) {
            text.setString(newText);
            sf::FloatRect textRect = text.getLocalBounds();
            text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        }
    }

    bool isClicked(sf::Vector2i mousePos) const {
        if (!visible) return false;
        return shape.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    }

    void draw(sf::RenderWindow& window, bool hasFont) {
        if (visible) {
            window.draw(shape);
            if (hasFont) window.draw(text);
        }
    }
};

// ==========================================
// CLASE 2: MarioTamagotchi
// ==========================================
class MarioTamagotchi {
private:
    int hunger;
    int health;
    int energy;

    bool isSleeping;
    bool isSick;
    bool isDead;

    sf::Clock statTimer;
    sf::Clock actionTimer;

    std::map<MarioState, sf::Texture> textures;
    sf::Sprite sprite;
    MarioState currentState;
    DecisionType activeDecision;

    void loadTextures() {
        textures[MarioState::SONRIENDO].loadFromFile("images/MARIO_SONRIENDO.jpg");
        textures[MarioState::HAMBRIADO].loadFromFile("images/MARIO_HAMBRIADO.jpg");
        textures[MarioState::COMIENDO].loadFromFile("images/MARIO_COMIENDO.jpg");
        textures[MarioState::DURMIENDO].loadFromFile("images/MARIO_DURMIENDO.jpg");
        textures[MarioState::ENFERMANDOSE].loadFromFile("images/MARIO_ENFERMANDOSE.jpg");
        textures[MarioState::CURANDOSE].loadFromFile("images/MARIO_CURANDOSE.jpg");
        textures[MarioState::BRINCANDO].loadFromFile("images/MARIO_BRINCANDO.jpg");
        textures[MarioState::MURIENDO].loadFromFile("images/MARIO_MURIENDO.jpg");

        setState(MarioState::SONRIENDO);
    }

    void setState(MarioState newState) {
        currentState = newState;
        sf::Texture& tex = textures[currentState];
        sprite.setTexture(tex, true);

        float targetWidth = 360.0f;
        float targetHeight = 360.0f;
        sf::Vector2u textureSize = tex.getSize();

        if (textureSize.x > 0 && textureSize.y > 0) {
            float scaleX = targetWidth / static_cast<float>(textureSize.x);
            float scaleY = targetHeight / static_cast<float>(textureSize.y);
            sprite.setScale(scaleX, scaleY);
        }

        sprite.setPosition(390.f, 100.f);
    }

public:
    MarioTamagotchi() 
        : hunger(100), health(100), energy(100), 
          isSleeping(false), isSick(false), isDead(false), activeDecision(DecisionType::NINGUNA) {
        loadTextures();
    }

    void feed() {
        hunger = std::min(100, hunger + 40);
        setState(MarioState::COMIENDO);
        activeDecision = DecisionType::NINGUNA;
        actionTimer.restart();
    }

    void cure() {
        isSick = false;
        health = 100;
        setState(MarioState::CURANDOSE);
        activeDecision = DecisionType::NINGUNA;
        actionTimer.restart();
    }

    void sleep() {
        isSleeping = true;
        setState(MarioState::DURMIENDO);
        activeDecision = DecisionType::NINGUNA;
        actionTimer.restart();
    }

    void jump() {
        setState(MarioState::BRINCANDO);
        activeDecision = DecisionType::NINGUNA;
        actionTimer.restart();
    }

    void rejectFood() {
        hunger = std::max(0, hunger - 25);
        if (hunger <= 0) {
            isDead = true;
            setState(MarioState::MURIENDO);
        }
        activeDecision = DecisionType::NINGUNA;
    }

    void rejectCure() {
        health = std::max(0, health - 40);
        if (health <= 0) {
            isDead = true;
            setState(MarioState::MURIENDO);
        }
        activeDecision = DecisionType::NINGUNA;
    }

    void rejectSleep() {
        energy = std::max(0, energy - 20);
        activeDecision = DecisionType::NINGUNA;
    }

    void rejectJump() {
        activeDecision = DecisionType::NINGUNA;
    }

    void update() {
        if (isDead) return;

        if (isSleeping) {
            if (statTimer.getElapsedTime().asSeconds() >= 2.0f) {
                statTimer.restart();
                energy = std::min(100, energy + 30);
                if (energy >= 100) {
                    isSleeping = false;
                    setState(MarioState::SONRIENDO);
                }
            }
            return;
        }

        if (activeDecision != DecisionType::NINGUNA) return;

        if (statTimer.getElapsedTime().asSeconds() >= 3.0f) {
            statTimer.restart();

            hunger = std::max(0, hunger - 15);
            energy = std::max(0, energy - 10);

            if (hunger < 50 && (rand() % 100 < 50)) {
                isSick = true;
            }

            if (hunger <= 0 || health <= 0) {
                isDead = true;
                setState(MarioState::MURIENDO);
                return;
            }

            if (isSick) {
                activeDecision = DecisionType::ENFERMO;
                setState(MarioState::ENFERMANDOSE);
            } else if (hunger < 50) {
                activeDecision = DecisionType::HAMBRE;
                setState(MarioState::HAMBRIADO);
            } else if (energy < 40) {
                activeDecision = DecisionType::SUENO;
            } else {
                if (rand() % 100 < 40) {
                    activeDecision = DecisionType::JUGAR;
                }
            }
        }

        if (actionTimer.getElapsedTime().asSeconds() > 1.5f && activeDecision == DecisionType::NINGUNA) {
            if (isSick) setState(MarioState::ENFERMANDOSE);
            else if (isSleeping) setState(MarioState::DURMIENDO);
            else if (hunger < 50) setState(MarioState::HAMBRIADO);
            else setState(MarioState::SONRIENDO);
        }
    }

    void draw(sf::RenderWindow& window) { window.draw(sprite); }

    int getHunger() const { return hunger; }
    int getHealth() const { return health; }
    int getEnergy() const { return energy; }
    bool getIsSick() const { return isSick; }
    bool getIsSleeping() const { return isSleeping; }
    bool getIsDead() const { return isDead; }
    DecisionType getActiveDecision() const { return activeDecision; }
};

// ==========================================
// CLASE 3: GameEngine
// ==========================================
class GameEngine {
private:
    sf::RenderWindow window;
    MarioTamagotchi mario;
    sf::Font font;
    sf::Text uiText;
    sf::Text questionText;
    bool hasFont;

    Button* btnOption1;
    Button* btnOption2;

    void initUI() {
        // Intenta cargar la fuente en diferentes rutas de Windows por seguridad
        hasFont = font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf") || 
                  font.loadFromFile("C:\\Windows\\Fonts\\segoeui.ttf") || 
                  font.loadFromFile("arial.ttf");

        sf::Font* fontPtr = hasFont ? &font : nullptr;

        if (hasFont) {
            uiText.setFont(font);
            uiText.setCharacterSize(16);
            uiText.setFillColor(sf::Color::White);
            uiText.setPosition(30.f, 30.f);

            questionText.setFont(font);
            questionText.setCharacterSize(17);
            questionText.setFillColor(sf::Color::Yellow);
            questionText.setPosition(30.f, 260.f);
        }

        // Se instancian los botones de forma segura
        btnOption1 = new Button(30.f, 320.f, 320.f, 45.f, "Opcion 1", fontPtr, sf::Color(40, 160, 60));
        btnOption2 = new Button(30.f, 380.f, 320.f, 45.f, "Opcion 2", fontPtr, sf::Color(200, 50, 50));
    }

    void processInput() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                DecisionType currentDec = mario.getActiveDecision();

                if (btnOption1->isClicked(mousePos)) {
                    if (currentDec == DecisionType::HAMBRE) mario.feed();
                    else if (currentDec == DecisionType::ENFERMO) mario.cure();
                    else if (currentDec == DecisionType::SUENO) mario.sleep();
                    else if (currentDec == DecisionType::JUGAR) mario.jump();
                }

                if (btnOption2->isClicked(mousePos)) {
                    if (currentDec == DecisionType::HAMBRE) mario.rejectFood();
                    else if (currentDec == DecisionType::ENFERMO) mario.rejectCure();
                    else if (currentDec == DecisionType::SUENO) mario.rejectSleep();
                    else if (currentDec == DecisionType::JUGAR) mario.rejectJump();
                }
            }
        }
    }

    void update() {
        mario.update();

        DecisionType dec = mario.getActiveDecision();
        sf::Font* fontPtr = hasFont ? &font : nullptr;

        if (dec == DecisionType::HAMBRE) {
            if (hasFont) questionText.setString("¡Mario tiene hambre!\n¿Que deseas hacer?");
            btnOption1->setText("Alimentar a Mario", fontPtr);
            btnOption2->setText("No alimentar a Mario", fontPtr);
            btnOption1->setVisible(true);
            btnOption2->setVisible(true);
        } else if (dec == DecisionType::ENFERMO) {
            if (hasFont) questionText.setString("¡Mario se esta enfermando!\n¿Que deseas hacer?");
            btnOption1->setText("Curar a Mario", fontPtr);
            btnOption2->setText("No curar a Mario", fontPtr);
            btnOption1->setVisible(true);
            btnOption2->setVisible(true);
        } else if (dec == DecisionType::SUENO) {
            if (hasFont) questionText.setString("¡Mario esta muy cansado!\n¿Que deseas hacer?");
            btnOption1->setText("Poner a dormir a Mario", fontPtr);
            btnOption2->setText("No dejarlo dormir", fontPtr);
            btnOption1->setVisible(true);
            btnOption2->setVisible(true);
        } else if (dec == DecisionType::JUGAR) {
            if (hasFont) questionText.setString("¡Mario quiere jugar!\n¿Que deseas hacer?");
            btnOption1->setText("Hacer brincar a Mario", fontPtr);
            btnOption2->setText("Ignorar a Mario", fontPtr);
            btnOption1->setVisible(true);
            btnOption2->setVisible(true);
        } else {
            if (hasFont) questionText.setString("");
            btnOption1->setVisible(false);
            btnOption2->setVisible(false);
        }
    }

    void render() {
        window.clear(sf::Color(30, 30, 30));

        mario.draw(window);

        if (hasFont) {
            std::string status = "ESTADO DE MARIO BROS:\n\n";
            status += " Hambre  : " + std::to_string(mario.getHunger()) + " / 100\n";
            status += " Salud   : " + std::to_string(mario.getHealth()) + " / 100\n";
            status += " Energia : " + std::to_string(mario.getEnergy()) + " / 100\n\n";
            status += "-----------------------------------\n";

            if (mario.getIsDead()) {
                status += " ¡MARIO HA MUERTO!\n (Tus decisiones acabaron con el)";
            } else if (mario.getIsSleeping()) {
                status += " Durmiendo... (Recuperando energia)";
            } else if (mario.getActiveDecision() == DecisionType::NINGUNA) {
                status += " Estado: Esperando evento...";
            }

            uiText.setString(status);
            window.draw(uiText);
            window.draw(questionText);
        }

        btnOption1->draw(window, hasFont);
        btnOption2->draw(window, hasFont);

        window.display();
    }

public:
    GameEngine() 
        : window(sf::VideoMode(800, 580), "Mario Tamagotchi - Decisiones") {
        window.setFramerateLimit(60);
        initUI();
    }

    ~GameEngine() {
        delete btnOption1;
        delete btnOption2;
    }

    void run() {
        while (window.isOpen()) {
            processInput();
            update();
            render();
        }
    }
};

int main() {
    GameEngine game;
    game.run();
    return 0;
}