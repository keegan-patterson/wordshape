#include <SFML/Graphics.hpp>

void write_letter(sf::RenderWindow &window, sf::Text &text, const std::string &letter, const sf::Vector2f &position)
{
    text.setString(letter);
    text.setCharacterSize(100);
    text.setFillColor(sf::Color::White);
    text.setPosition(position);
}

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "CMake SFML Project");
    window.setFramerateLimit(144);

    sf::Font font("C:/Windows/Fonts/arial.ttf");
    sf::Text text = sf::Text(font);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            
            if(event->is<sf::Event::TextEntered>())
            {
                const sf::Event::TextEntered* t_event = event->getIf<sf::Event::TextEntered>();
                write_letter(window, text, std::string(1, static_cast<char>(t_event->unicode)), {100, 100});
            }
        }

        window.clear();
        window.draw(text);
        window.display();
    }
}