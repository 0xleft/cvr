#include "SysdiffViewer.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <format>
#include <sstream>


// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static nlohmann::json parseFile(std::string file) {
    std::ifstream t(file);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string str = buffer.str();
    
    fprintf(stderr, "File: %s\n", str.c_str());

    try {
        // why tf this causes lag?
        nlohmann::json j = nlohmann::json::parse(str);

        return j;
    }
    catch (...) {
        return NULL;
    }

    return NULL;
}

// Main code
int main(int, char**) {

    SysdiffViewer* viewerInstance = new SysdiffViewer();

    ImGuiFileDialog fdinstanceBase;
    ImGuiFileDialog fdinstanceSecond;

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    GLFWwindow* window = glfwCreateWindow(1280, 720, "SysdiffViewer", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsLight();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(0.77f, 0.77f, 0.77f, 1.00f);

    bool diffViewer = false;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            if (ImGui::BeginMainMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Open base")) {
                        fdinstanceBase.OpenDialog("ChooseFileDlgKey", "Choose base", ".bin,.*", "", "");
                    }
                    if (ImGui::MenuItem("Open second")) {
                        fdinstanceSecond.OpenDialog("ChooseFileDlgKey", "Choose second", ".bin,.*", "", "");
                    }
                    if (ImGui::MenuItem("Clear")) {
                        viewerInstance->setBase(NULL);
                        viewerInstance->setSecond(NULL);
                        viewerInstance->clearFileMap();
                    }

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("View")) {
                    if (ImGui::MenuItem("Diff viewer")) {
                        diffViewer = !diffViewer;
					}

					ImGui::EndMenu();
				}

                ImGui::EndMainMenuBar();
            }

            if (viewerInstance->getBase() != NULL) {
                ImGui::Begin("Base");
                ImGui::Text("Base");
                ImGui::Text(std::format("Path: {}", fdinstanceBase.GetFilePathName()).c_str());
                viewerInstance->displayBaseMap();
                ImGui::End();
			}
                
            if (viewerInstance->getSecond() != NULL) {
                ImGui::Begin("Second");
                ImGui::Text("Second");
                ImGui::Text(std::format("Path: {}", fdinstanceSecond.GetFilePathName()).c_str());
                viewerInstance->displaySecondMap();
                ImGui::End();
            }

            if (fdinstanceBase.Display("ChooseFileDlgKey")) {
                if (fdinstanceBase.IsOk()) {
                    std::string filePathName = fdinstanceBase.GetFilePathName();
                    std::string filePath = fdinstanceBase.GetCurrentPath();

                    viewerInstance->setBase(parseFile(filePathName));
                    viewerInstance->loadBaseMap();
                }

                fdinstanceBase.Close();
            }

            if (diffViewer) {
                ImGui::Begin("Diff viewer");
				ImGui::Text("Diff viewer");
                viewerInstance->displayDiff();
				ImGui::End();
            }

            if (fdinstanceSecond.Display("ChooseFileDlgKey")) {
                if (fdinstanceSecond.IsOk()) {
                    std::string filePathName = fdinstanceSecond.GetFilePathName();
                    std::string filePath = fdinstanceSecond.GetCurrentPath();

                    viewerInstance->setSecond(parseFile(filePathName));
                    viewerInstance->loadSecondMap();
                }

                fdinstanceSecond.Close();
            }
        }

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    delete viewerInstance;

    return 0;
}