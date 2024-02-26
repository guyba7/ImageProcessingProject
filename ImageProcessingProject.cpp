#include <iostream>
#include <vector>
#include <string>
#include <windows.h>  // Required for Windows console functions
#include <filesystem>

#include "Effect.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using std::string;  // Make string available as 'string'
using std::vector;  // Make vector available as 'vector'
using std::filesystem::path;
namespace fs = std::filesystem;

#define INPUT_IMAGES_FOLDER_PATH "../Input Images";
#define OUTPUT_IMAGES_FOLDER_PATH "../Output Images";

#define WELCOME_MESSAGE "Welcome to the PNG Processing Application!\n"\
                        "This application allows you to apply effects to images.\n"\
                        "Press ENTER to continue.\n"

#define SELECT_IMAGE_MESSAGE "Select a PNG image to apply effect on.\n"\
                             "Use the UP and DOWN arrow keys to navigate options.\n"\
                             "Press ENTER to select an option.\n\n"

#define SELECT_EFFECT_MESSAGE "Select an effect to apply to the image.\n"\
                             "Use the UP and DOWN arrow keys to navigate options.\n"\
                             "Press ENTER to select an option.\n\n"

#define ENDING_MESSAGE "Image with effect created successfully!\n"\
                        "Press ENTER to create a new image or press ESC to close application.\n"

#define ENDING_MESSAGE_ERORR "Image processing failed...\n"\
                             "Press ENTER to create a new image or press ESC to close application.\n"

ShaderManager* m_shaderManager = new ShaderManager();

static void InitializeLists(vector<path>& images, vector<BaseEffect*>& effects) {

    string inputImagesPath = INPUT_IMAGES_FOLDER_PATH;

    // Clear any existing entries in images
    images.clear();

    fs::directory_iterator it(inputImagesPath);

    while (it != fs::directory_iterator()) {
        fs::directory_entry entry = *it;
        if (entry.is_regular_file() && entry.path().extension().string() == ".png") {

            images.push_back(entry.path());
        }
        it++;
    }

    // Initialize effects (remains unchanged)
    effects = {
        new BlurEffect(),
        new ColorInversionEffect(),
        new MirrorEffect(),
        new ShrinkEffect(),
        new EdgeDetectionEffect(),
        new EqualizationEffect(),
        new FishEyeEffect()
    };

}


static void DisplayMenu(const vector<string>& options, int highlight, const string& instruction) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Clear the screen
   system("cls");
    
    // Display instruction
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
    std::cout << instruction;

    for (size_t i = 0; i < options.size(); ++i) {
        if (i == highlight) {
            // Attempt to make the text "brighter" as a substitute for bold, since true bold is not supported
            SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);  // Green and intense (brighter)
            std::cout << "> " << options[i] << std::endl;
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);  // Reset to default color
        }
        else {
            std::cout << "  " << options[i] << std::endl;
        }
    }

    // Reset text attributes to default after displaying the menu
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}


static int PromptSelectionOptions(const vector<string>& options, const char* prompt) {
    int selection = 0;  // Currently selected option index
    const int optionsCount = options.size();

    // Initial display
    std::cout << prompt << std::endl;
    DisplayMenu(options, selection, prompt);

    // Setup input handling
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));  // Disable echo and line input

    INPUT_RECORD inputRecord;
    DWORD eventsRead;

    while (true) {
        ReadConsoleInput(hStdin, &inputRecord, 1, &eventsRead);

        if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown) {
            switch (inputRecord.Event.KeyEvent.wVirtualKeyCode) {
            case VK_UP:
                selection = (selection - 1 + optionsCount) % optionsCount;
                break;
            case VK_DOWN:
                selection = (selection + 1) % optionsCount;
                break;
            case VK_RETURN:  // Enter key
                SetConsoleMode(hStdin, mode);  // Restore console mode
                return selection;
            }

            DisplayMenu(options, selection, prompt);  // Update the display with the new selection
        }
    }
}

static void PromptWelcomeScreen() {

    // Clear the screen
    system("cls");

    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD inputRecord;
    DWORD eventsRead;

    std::cout << WELCOME_MESSAGE;

    while (true) {
        ReadConsoleInput(hStdin, &inputRecord, 1, &eventsRead);

        if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown && inputRecord.Event.KeyEvent.wVirtualKeyCode == VK_RETURN) {
            return;
        }
    }
}

static bool PromptEndingScreen(bool isSuccess) {

    if (!isSuccess)
        Sleep(5000);

    // Clear the screen
   system("cls");

    if (isSuccess)
    {
        std::cout << ENDING_MESSAGE;
    }
    else
    {
        std::cout << ENDING_MESSAGE_ERORR;
    }

    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD inputRecord;
    DWORD eventsRead;



    while (true) {
        ReadConsoleInput(hStdin, &inputRecord, 1, &eventsRead);

        if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown) {
            switch (inputRecord.Event.KeyEvent.wVirtualKeyCode) {
            case VK_RETURN:
                return TRUE;
            case VK_ESCAPE:
                return FALSE;
            }
        }
    }
}

static bool ApplyEffectToImage(path imagePath, BaseEffect* effect) {
    int width, height, channels;
    

    if (!fs::exists(imagePath)){
        std::cout << "Invalid image path: " << imagePath << std::endl;
        return false;
    }

    // Load the image
    unsigned char* imageData = stbi_load(imagePath.string().c_str(), &width, &height, &channels, 0);
  
    if (!imageData) {
        std::cout << "Error loading image: " << imagePath << std::endl;
        return false;
    }

    string* effectError = new string("OK");

    // Apply Effect on imageData
    if (!effect->ApplyEffectFromRawImageData(imageData, width, height, channels, m_shaderManager, effectError))
    {
        std::cout << "Error applying effect to image data: /n" << *effectError << std::endl;
        return false;
    }
    
    // Construct the output file name/path
    path outputDir = OUTPUT_IMAGES_FOLDER_PATH; 
    path fileName = imagePath.stem();
    string newFileName = fileName.string() + "_" + effect->GetEffectFileSuffix() + imagePath.extension().string();
    path outputPath = outputDir / newFileName;

    // Ensure the output directory exists
    create_directories(outputDir);

    // Save the image back to disk
    int success = stbi_write_png(outputPath.string().c_str(), width, height, channels, imageData, width * channels);

    if (!success) {
        std::cout << "Error saving image: " << outputPath << std::endl;
        stbi_image_free(imageData);
        return false;
    }

    // Free the image memory
    stbi_image_free(imageData);

    return true;
}

static vector<string> convertImagePathsToStrings(vector<path> imagePaths) {
    vector<string> imageNames;

    for (path currPath : imagePaths){
        imageNames.push_back(currPath.filename().string());
    }

    return imageNames;
}

static vector<string> convertEffectsToStrings(vector<BaseEffect*> Effects) {
    vector<string> effectNames;

    for (BaseEffect* currEffect : Effects) {
        effectNames.push_back(currEffect->GetEffectDisplayName());
    }

    return effectNames;
}


int main() {

    string* errorString = new string("OK");

    // init shader manager
    m_shaderManager = new ShaderManager();
    if (!m_shaderManager->initalizeShaderManager(errorString))
    {
        std::cout << "ERROR: "  << errorString;
        return -1;
    }

    vector<path> imagePaths;
    vector<BaseEffect*> effects;
    InitializeLists(imagePaths, effects);

    PromptWelcomeScreen();

    bool resumeAppFlag = 1;

    while (resumeAppFlag == 1) {

        vector<string> imageOptions = convertImagePathsToStrings(imagePaths);
        int chosenImageIndex = PromptSelectionOptions(imageOptions, SELECT_IMAGE_MESSAGE);
        //std::cout << "\nYou selected: " << imageOptions[chosenImageIndex] << std::endl;

        vector<string> effectsOptions = convertEffectsToStrings(effects);
        int chosenEffect = PromptSelectionOptions(effectsOptions, SELECT_EFFECT_MESSAGE);
     //   std::cout << "You selected: " << effects[chosenEffect] << std::endl;

        // apply the chosen effect on the chosen image
        bool isSuccess = ApplyEffectToImage(imagePaths[chosenImageIndex], effects[chosenEffect]);

        resumeAppFlag = PromptEndingScreen(isSuccess);
    }

    return 1;
}
