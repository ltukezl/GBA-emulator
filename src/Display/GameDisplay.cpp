#include "Display/GameDisplay.hpp"

void GameDisplay::draw()
{
    m_display->clear(sf::Color::Black);
    m_game_texture.update(reinterpret_cast<uint8_t*>(&(*m_game_pixels)[0][0]));
    m_display->draw(m_game_sprite);
    m_display->display();
}

void GameDisplay::handleEvents()
{

    const auto onClose = [this](const sf::Event::Closed&) {
        m_display->close();
    };

    const auto onKeyEntered = [](const sf::Event::KeyPressed& event) {
        if (event.code == sf::Keyboard::Key::Down) {
            keyInput->btn_down = 0;
        }

        if (event.code == sf::Keyboard::Key::Up) {
            keyInput->btn_up = 0;
        }

        if (event.code == sf::Keyboard::Key::Left) {
            keyInput->btn_left = 0;
        }

        if (event.code == sf::Keyboard::Key::Right) {
            keyInput->btn_right = 0;
        }

        if (event.code == sf::Keyboard::Key::Z) {
            keyInput->btn_A = 0;
        }

        if (event.code == sf::Keyboard::Key::X) {
            keyInput->btn_B = 0;
        }

        if (event.code == sf::Keyboard::Key::A) {
            keyInput->btn_l = 0;
        }

        if (event.code == sf::Keyboard::Key::S) {
            keyInput->btn_r = 0;
        }

        if (event.code == sf::Keyboard::Key::Q) {
            keyInput->btn_start = 0;
        }

        if (event.code == sf::Keyboard::Key::W) {
            keyInput->btn_select = 0;
        }
    };

    const auto onKeyReleased = [](const sf::Event::KeyReleased& event) {
        if (event.code == sf::Keyboard::Key::Down) {
            keyInput->btn_down = 1;
        }

        if (event.code == sf::Keyboard::Key::Up) {
            keyInput->btn_up = 1;
        }

        if (event.code == sf::Keyboard::Key::Left) {
            keyInput->btn_left = 1;
        }

        if (event.code == sf::Keyboard::Key::Right) {
            keyInput->btn_right = 1;
        }

        if (event.code == sf::Keyboard::Key::Z) {
            keyInput->btn_A = 1;
        }

        if (event.code == sf::Keyboard::Key::X) {
            keyInput->btn_B = 1;
        }

        if (event.code == sf::Keyboard::Key::A) {
            keyInput->btn_l = 1;
        }

        if (event.code == sf::Keyboard::Key::S) {
            keyInput->btn_r = 1;
        }

        if (event.code == sf::Keyboard::Key::Q) {
            keyInput->btn_start = 1;
        }

        if (event.code == sf::Keyboard::Key::W) {
            keyInput->btn_select = 1;
        }
    };

    m_display->handleEvents(onClose, onKeyEntered, onKeyReleased);
    /*
    while (display->pollEvent(event)) {
        // "close requested" event: we close the window
        if (event.type == sf::Event::Closed)

        if (event.type == sf::Event::KeyPressed) {



            if (keypadInterruptCtrl->IRQ_EN && InterruptEnableRegister->keyPad){
                uint16_t tmp = ~((keyInput->addr) & 0x3FFF);
                if (keypadInterruptCtrl->IRQ_cond){
                    if (tmp & (keypadInterruptCtrl->addr & 0x3FFF)){
                        InterruptFlagRegister->keyPad = 1;
                    }
                }
                else{
                    if ((tmp & (keyInput->addr & 0x3FFF)) == tmp){
                        InterruptFlagRegister->keyPad = 1;
                    }
                }
            }

        }


        }
    }
        */
}
