#include "game.h"

#include "renderer.h"
#include "logmanager.h"
#include "sprite.h"
#include "iniparser.h"

#include "imgui/imgui_impl_sdl2.h"
#include "inputsystem.h"
#include "animatedsprite.h"
#include "soundsystem.h"

#include "gamestatemanager.h"


Game* Game::sm_pInstance = 0;

Game& Game::GetInstance()
{
    if (sm_pInstance == 0) {
        sm_pInstance = new Game();
    }

    return (*sm_pInstance);
}

void Game::DestroyInstance() {
    delete sm_pInstance;
    sm_pInstance = 0;
}

Game::Game() : m_pRenderer(0), m_bLooping(true), m_soundSystem(0), m_pInputSystem(nullptr) {
}

Game::~Game() {
    delete m_pRenderer;
    m_pRenderer = 0;
}

void Game::Quit() {
    m_bLooping = false;
}

bool Game::Initialise() {
    
    int bbWidth = 500;
    int bbHeight = 500;

    m_pRenderer = new Renderer();
    if (!m_pRenderer->Initialise(false, bbWidth, bbHeight)) {
        LogManager::GetInstance().Log("Renderer failed to initialise!");
        return false;
    }

    bbWidth = m_pRenderer->GetWidth();
    bbHeight = m_pRenderer->GetHeight();

    m_iLastTime = SDL_GetPerformanceCounter();

    m_pInputSystem = new InputSystem();
    m_pInputSystem->Initialise();

    m_stateManager = new GameStateManager(*m_pRenderer, *m_pInputSystem);
    return true;
}

bool Game::DoGameLoop() {
    const float stepSize = 1.0f / 60.0f;
    // TODO: Process input here!
    m_pInputSystem->ProcessInput();

    if (m_bLooping)
    {
        Uint64 current = SDL_GetPerformanceCounter();
        float deltaTime = (current - m_iLastTime) / static_cast<float>(SDL_GetPerformanceFrequency());
        m_iLastTime = current;
        m_fExecutionTime += deltaTime;
        Process(deltaTime);
#if USE_LAG
        m_fLag += deltaTime;
        int innerLag = 0;
        while (m_fLag >= stepSize)
        {
            Process(stepSize);
            m_fLag -= stepSize;
            ++m_iUpdateCount;
            ++innerLag;
        }
#endif //USE_LAG
        Draw(*m_pRenderer);
    }
    return m_bLooping;
}

void Game::Process(float deltaTime)
{
    ProcessFrameCounting(deltaTime);

    m_bShowDebugWindow = (m_stateManager->GetDebuggingStatus()) ? true : false;

#if DEBUG
    if (m_bPauseSimulation) {
        deltaTime = 0.0f;
}
#endif // !DEBUG
    
    
    m_stateManager->Update(deltaTime);
    m_bLooping = m_stateManager->GetGameStatus();
}

void Game::Draw(Renderer& renderer)
{
    ++m_iFrameCount;
    renderer.Clear();
    m_stateManager->Draw();
    DebugDraw();

    renderer.Present();
}
void
Game::ProcessFrameCounting(float deltaTime)
{
    // Count total simulation time elapsed:
    m_fElapsedSeconds += deltaTime;
    // Frame Counter:
    if (m_fElapsedSeconds > 1.0f)
    {
        m_fElapsedSeconds -= 1.0f;
        m_iFPS = m_iFrameCount;
        m_iFrameCount = 0;
    }
}


void Game::DebugDraw
()
{
#if DEBUG
    if (m_bShowDebugWindow)
    {
        bool open = true;
        ImGui::Begin("Debug Window", &open, ImGuiWindowFlags_MenuBar);
        ImGui::Text("COMP710 GP Framework (%s)", "2025, S1, Space Escape");
        if (ImGui::Button("Quit Game"))
        {
            Quit();
        }

        m_stateManager->DebugDraw();

        ImGui::Checkbox("Show Loaded Textures", &m_bShowTextures);
        if (m_bShowTextures)
        {
            m_pRenderer->DebugDraw();
        }

        if (ImGui::Button("Pause simulation"))
        {
            m_bPauseSimulation = !m_bPauseSimulation;
        }

        ImGui::End();
    }
#endif // !DEBUG
}

void Game::ToggleDebugWindow
()
{
#if DEBUG
    m_bShowDebugWindow = !m_bShowDebugWindow;
    m_pInputSystem->ShowMouseCursor(m_bShowDebugWindow);
#endif // !DEBUG    
}
