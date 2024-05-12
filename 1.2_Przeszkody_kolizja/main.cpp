#include <SDL.h>
#include <iostream>
#include <vector>

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

//Dodanie przeszkody (pachołka)
struct Obstacle {
    int x, y, width, height;
    SDL_Texture* texture;
};

std::vector<Obstacle> obstacles;

//Ładowanie tekstur
SDL_Texture* loadTexture(const std::string &file, SDL_Renderer* renderer) {
    SDL_Surface* loadedSurface = SDL_LoadBMP(file.c_str()); //Ładowanie obrazka MBP do SDL_Surface
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
void updatePhysics() {
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

    // Renderowanie przeszkód
    for (const auto& obstacle : obstacles) {
        SDL_Rect pacholek = {obstacle.x, obstacle.y, obstacle.width, obstacle.height}; //Rozmiary pacholka
        SDL_RenderCopy(renderer, obstacle.texture, NULL, &pacholek); //Wyświetlenie pacholka na ekran
    }

    SDL_RenderPresent(renderer);
}

//Dodawanie przeszkody
void addObstacle(SDL_Renderer* renderer) {
    Obstacle obstacle;
    obstacle.x = obstacleX; //Start na końcu ekranu
    obstacle.y = obstacleY; //Na tej samej wysokości co dinozaur
    obstacle.width = OBSTACLE_WIDTH; //Szerokość przeszkody
    obstacle.height = OBSTACLE_HEIGHT; //Wysokość przeszkody
    obstacle.texture = loadTexture("C:\\Users\\Filip\\Desktop\\SGD_Projekt\\pacholek.bmp", renderer); //Załadowanie obrazka bmp pacholka (ścieżkę prawdopodbnie trzeba będzie zmienić na zajęciach)

    obstacles.push_back(obstacle); //Dodanie do wektora, który przechowuje wszyskie przeszkody w grze
}

//Przesuwanie przeszkody
void moveObstacle() {
    for (auto& obstacle : obstacles) {
        obstacle.x -= 5; //Przesunięcie przeszkodę w lewo
        if (obstacle.x + obstacle.width < 0) { //Sprawdzenie czy przezkoda opuściła ekran
            obstacles.erase(obstacles.begin()); //Usunięcie przeszkody po opuszczeniu ekranu
        }
    }
}

//Sprawdzenie kolizji
void checkCollisions() {
    SDL_Rect dino = {dinoX, dinoY, DINO_WIDTH, DINO_HEIGHT}; //dinozaur
    for (auto& obstacle : obstacles) {
        SDL_Rect obs = {obstacle.x, obstacle.y, obstacle.width, obstacle.height}; //pacholek
        if (SDL_HasIntersection(&dino, &obs)) { //Sprawdzenie kolizji
            std::cout << "Kolizja!" << std::endl; //Wykrycie kolizji
        }
    }
}

int main(int argc, char* args[]) {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Gra w dinozaura", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* dinoTexture = loadTexture("C:\\Users\\Filip\\Desktop\\SGD_Projekt\\dino.bmp", renderer); //Załadowanie obrazka bmp dinozaura (ścieżkę prawdopodbnie trzeba będzie zmienić na zajęciach)

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        handleEvents(e, quit);
        updatePhysics();

        if (rand() % 100 == 0){
            addObstacle(renderer);
        }

        moveObstacle();
        checkCollisions();
        render(renderer, dinoTexture);

        SDL_Delay(10);
    }

    //Zwalnianie zasobów
    SDL_DestroyTexture(dinoTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    //Quit SDL
    SDL_Quit();

    return 0;
}