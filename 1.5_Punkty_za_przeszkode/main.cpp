#include <SDL.h>
#include <iostream>
#include <vector>
#include <string>

//Zmienne globalne
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int DINO_WIDTH = 50;
const int DINO_HEIGHT = 50;
const int OBSTACLE_WIDTH = 20;
const int OBSTACLE_HEIGHT = 50;

int dinoX = 50;  // Startowa pozycja X dinozaura
int dinoY = SCREEN_HEIGHT - DINO_HEIGHT;  // Startowa pozycja Y (na ziemi)

int obstacleX = SCREEN_WIDTH;  // Startowa pozycja przeszkody (na koncu ekranu)
int obstacleY = SCREEN_HEIGHT - 50;  // Wysokośc przeszkody (wysokość dinozaura)

bool isJumping = false;
int jumpSpeed = 0;

int successfullJump = 0;

//Dodanie przeszkody (pachołka) (przy pomocy AI)
struct Obstacle {
    int x, y, width, height;
    SDL_Texture* texture;
};

std::vector<Obstacle> obstacles; //Przechowywanie wszystkich przeszkód w grze

//Ładowanie tekstur
SDL_Texture* loadTexture(const std::string &file, SDL_Renderer* renderer) {
    SDL_Surface* loadedSurface = SDL_LoadBMP(file.c_str()); //Ładowanie obrazka BMP do SDL_Surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, loadedSurface); //Zamiana powierzchni na tekstury
    SDL_FreeSurface(loadedSurface);  // Zwolnienie pamięci zajmowanej przez SDL_Surface (niepotrzebna po zamianie na tekstury)
    return texture;
}

// Obsługa eventów (reakcja na naciśnięcie przycisku na klawiaturze)
void handleEvents(SDL_Event& event, bool& quit) {
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            quit = true;
        }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_SPACE:
                    if (!isJumping) {
                        isJumping = true;
                        jumpSpeed = -15; // Prędkość skoku pionowo
                    }
                    break;
            }
        }
    }
}

//Fizyka gry
void gamePhysics() {
    if (isJumping) {
        dinoY += jumpSpeed; // Pozycja po skoku
        jumpSpeed += 1; // Szybkość opadu dinozaura

        if (dinoY >= SCREEN_HEIGHT - DINO_HEIGHT) {
            dinoY = SCREEN_HEIGHT - DINO_HEIGHT; // Blokada przed wpadnięciem dinozaura pod ziemie
            isJumping = false;
        }
    }
}


//Renderowanie
void render(SDL_Renderer* renderer, SDL_Texture* texture) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Kolor tła
    SDL_RenderClear(renderer); //Usuwanie wszystkiego co było na ekranie przed renderowaniem

    // Renderowanie dinozaura
    SDL_Rect dino = {dinoX, dinoY, DINO_WIDTH, DINO_HEIGHT}; //Rozmiary dinozaura
    SDL_RenderCopy(renderer, texture, NULL, &dino); //Wyświetlenie dinozaura na ekran

    // Renderowanie przeszkód (przy pomocy AI)
    for (const auto& obstacle : obstacles) {
        SDL_Rect pacholek = {obstacle.x, obstacle.y, obstacle.width, obstacle.height}; //Rozmiary pacholka
        SDL_RenderCopy(renderer, obstacle.texture, NULL, &pacholek); //Wyświetlenie pacholka na ekran
    }

    SDL_RenderPresent(renderer);
}

// Renderowanie końca gry
void renderGameOver(SDL_Renderer* renderer){
    SDL_Texture* gameOverTexture = loadTexture("C:\\Users\\Filip\\Desktop\\SGD_Projekt\\assets\\game_over.bmp", renderer); //Załadowanie obrazka bmp końca gry (ścieżkę prawdopodbnie trzeba będzie zmienić na zajęciach)
    SDL_Rect gameover = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}; //Obrazek na środku ekranu
    SDL_RenderCopy(renderer, gameOverTexture, NULL, &gameover); //Wyświetlenie końca gry na ekran
}

//Dodawanie przeszkody (przy pomocy AI)
void addObstacle(SDL_Renderer* renderer) {
    Obstacle obstacle;
    obstacle.x = obstacleX; //Start na końcu ekranu
    obstacle.y = obstacleY; //Na tej samej wysokości co dinozaur
    obstacle.width = OBSTACLE_WIDTH; //Szerokość przeszkody
    obstacle.height = OBSTACLE_HEIGHT; //Wysokość przeszkody
    obstacle.texture = loadTexture("C:\\Users\\Filip\\Desktop\\SGD_Projekt\\assets\\pacholek.bmp", renderer); //Załadowanie obrazka bmp pacholka (ścieżkę prawdopodbnie trzeba będzie zmienić na zajęciach)

    obstacles.push_back(obstacle); //Dodanie do wektora, który przechowuje wszyskie przeszkody w grze
}

//Przesuwanie przeszkody (przy pomocy AI)
void moveObstacle() {
    for (auto& obstacle : obstacles) {
        obstacle.x -= 5; //Przesunięcie przeszkodę w lewo
        if (obstacle.x + obstacle.width < 0) { //Sprawdzenie czy przezkoda opuściła ekran
            obstacles.erase(obstacles.begin()); //Usunięcie przeszkody po opuszczeniu ekranu
        }
    }
}

//Czas między pojawianiem się przeszkody
void obstacleSpawnTime(SDL_Renderer* renderer, int& lastObstacleTime, int obstacleDelay){
    int currentTime = SDL_GetTicks(); //Mierzenie czasu przeszkód
    if (currentTime - lastObstacleTime > obstacleDelay) { //Czas od pojawienia się poprzedniej przeszkody
        if (rand() % 2 == 0) { //50% szansy na dodanie przeszkody
            addObstacle(renderer);
            lastObstacleTime = currentTime; //Reset czasu po zniknięciu przeszkody
        }
    }
}

//Sprawdzenie kolizji
bool checkCollisionsAndGivePoints() {
    bool collision = false;
    SDL_Rect dino = {dinoX, dinoY, DINO_WIDTH, DINO_HEIGHT}; //dinozaur
    for (auto &obstacle: obstacles) {
        SDL_Rect obs = {obstacle.x, obstacle.y, obstacle.width, obstacle.height}; //pacholek
        if (SDL_HasIntersection(&dino, &obs)) { //Sprawdzenie kolizji
            std::cout << "Kolizja!" << std::endl; //Wykrycie kolizji
            collision = true;
            break;
        }
    }

        for (auto it = obstacles.begin(); it != obstacles.end();) {
            if (dinoX > it->x + it->width && !collision) { // Sprawdzamy czy przeskoczył i czy nie było kolizji
                successfullJump += 10; // Plus 10 puntków za przeszkoczenie pachoła
                it = obstacles.erase(it); // Usuwamy przeszkodę z listy
            } else {
                ++it;
            }
        }

        return collision;
    }

//Ładowanie tekstur liczb do wektora (cyfry od 0 do 9 w formacie bmp)
std::vector<SDL_Texture*> loadNumberTextures(SDL_Renderer* renderer) {
    std::vector<SDL_Texture*> digitTextures;
    for (int i = 0; i < 10; i++) {
        std::string filePath = "C:\\Users\\Filip\\Desktop\\SGD_Projekt\\assets\\" + std::to_string(i) + ".bmp";
        SDL_Surface* loadedSurface = SDL_LoadBMP(filePath.c_str()); //to samo co w loadTextures
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        SDL_FreeSurface(loadedSurface);
        digitTextures.push_back(texture);
    }
    return digitTextures;
}

//Renderowanie cyfry na ekran
void renderNumber(SDL_Renderer* renderer, const std::vector<SDL_Texture*>& digitTextures, int number, int x, int y) {
    std::string string = std::to_string(number); //Zmiana liczb na string
    int space = 0;
    for (char number : string) {
        int index = number - '0';
        SDL_Rect dstRect = {x + space, y, 20, 30}; //Wyświetlenie cyfry na ekranie
        SDL_RenderCopy(renderer, digitTextures[index], NULL, &dstRect); //Wyświetlenie cyfry na ekranie
        space += 20;  //Odległość między cyframi
    }
}

int main(int argc, char* args[]) {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Gra w dinozaura", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    std::vector<SDL_Texture*> numberTextures = loadNumberTextures(renderer);
    SDL_Texture* dinoTexture = loadTexture("C:\\Users\\Filip\\Desktop\\SGD_Projekt\\assets\\dino.bmp", renderer); //Załadowanie obrazka bmp dinozaura (ścieżkę prawdopodbnie trzeba będzie zmienić na zajęciach)

    int points = 0, startTime = SDL_GetTicks();
    int lastTimeUpdate = startTime, gameScore = 0;
    bool quit = false;
    SDL_Event e;

    int lastObstacleTime = SDL_GetTicks(); //Mierzenie czasu od ostatniej przeszkody
    while (!quit) {

        int currentTime = SDL_GetTicks(); //Mierzenie czasu od rozpoczęcia programu
        if (currentTime - lastTimeUpdate >= 1000) { //Odstęp 1s po zmianie
            points++;
            gameScore++;
            lastTimeUpdate = currentTime;
            std::cout << "Liczba punktow: " << gameScore << ", Czas gry: " << (currentTime - startTime) / 1000 << std::endl;
        }

        SDL_RenderClear(renderer);

        obstacleSpawnTime(renderer, lastObstacleTime, 1000);

        handleEvents(e, quit);
        gamePhysics();
        moveObstacle();
        if(checkCollisionsAndGivePoints()){
            renderGameOver(renderer); //Wyświetlenie obrazka końca gry na ekranie
            SDL_RenderPresent(renderer); //Wyświetlenie na ekranie z zachowaniem poprzedniego stanu gry
            std::cout << "Koniec gry!" << std::endl;
            SDL_Delay(3000); //Automatyczne zamknięcie gry po jej zakończeniu
            break;
        }

        render(renderer, dinoTexture);

        gameScore = points + successfullJump; //Dodanie aktualnych punktów plus punkty za prawidłowy skok

        // Renderowanie punktacji i czasu
        renderNumber(renderer, numberTextures, gameScore, 10, 10); //Licznik czasu w lewym górnym rogu
        renderNumber(renderer, numberTextures, (currentTime - startTime) / 1000, 640-50, 10); //Licznik czasu w prawym górym rogu

        SDL_RenderPresent(renderer); //Aktualizacja ekranu

        SDL_Delay(10);
    }

    //Zwalnianie zasobów
    for (SDL_Texture* textures : numberTextures) {
        if (textures) SDL_DestroyTexture(textures);
    }
    SDL_DestroyTexture(dinoTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    //Quit SDL
    SDL_Quit();

    return 0;
}