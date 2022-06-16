#define UNICODE

#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <cctype>
#include <random>
#include <conio.h>
#include <Windows.h>
#include <WinUser.h>
#include <mmsystem.h>
#include <filesystem>
#include <typeinfo>

using namespace cv;
using namespace std;

namespace fs = std::filesystem;

// char ASCII_CHARS[] = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
// char ASCII_CHARS[] = {'@', '#', 'S', '%', '?', '*', '+', ';', ':', ',', '.', ' '};
char ASCII_CHARS[] = " .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";

string author = "minhcrafters";
string version = "v1.2.0.170622";

string title = "ASCII Player by " + author + " | " + version;

random_device dev;
mt19937 rng(dev());
uniform_int_distribution<mt19937::result_type> generate_random_number(1, 3);

typedef chrono::steady_clock::period period;
typedef chrono::duration<float, period> duration;

std::string elapsedTime(std::chrono::seconds secs)
{
    bool neg = secs < 0s;
    if (neg)
        secs = -secs;
    auto h = chrono::duration_cast<chrono::hours>(secs);
    secs -= h;
    auto m = chrono::duration_cast<chrono::minutes>(secs);
    secs -= m;
    std::string result;
    if (neg)
        result.push_back('-');
    if (h < 10h)
        result.push_back('0');
    result += to_string(h / 1h);
    result += ':';
    if (m < 10min)
        result.push_back('0');
    result += to_string(m / 1min);
    result += ':';
    if (secs < 10s)
        result.push_back('0');
    result += to_string(secs / 1s);
    return result;
}

void get_int(int &d, std::string prompt, std::string fail)
{
    while (1)
    {

        std::cout << prompt;
        std::string str;
        std::cin >> str;

        std::istringstream ss(str);
        int val1;
        ss >> val1;

        if (!ss.eof())
        {
            std::cout << fail;
            continue;
        }
        else
        {
            d = val1;
            break;
        }
    }
}

void clearScreen()
{
    //// COORD tl = {0, 0};
    //// CONSOLE_SCREEN_BUFFER_INFO s;
    //// HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    //// GetConsoleScreenBufferInfo(console, &s);
    //// DWORD written, cells = s.dwSize.X * s.dwSize.Y;
    //// FillConsoleOutputCharacter(console, fill, cells, tl, &written);
    //// FillConsoleOutputAttribute(console, s.wAttributes, cells, tl, &written);
    //// SetConsoleCursorPosition(console, tl);
    COORD cursorPosition;
    cursorPosition.X = 0;
    cursorPosition.Y = 0;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
}

void showCursor(bool showFlag)
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag;
    SetConsoleCursorInfo(out, &cursorInfo);
}

void maximizeWindow()
{
    HWND hWnd;
    SetConsoleTitleA(title.c_str());
    hWnd = FindWindowA(NULL, title.c_str());
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD NewSBSize = GetLargestConsoleWindowSize(hOut);
    SMALL_RECT DisplayArea = {0, 0, 0, 0};

    SetConsoleScreenBufferSize(hOut, NewSBSize);

    DisplayArea.Right = NewSBSize.X - 1;
    DisplayArea.Bottom = NewSBSize.Y - 1;

    SetConsoleWindowInfo(hOut, TRUE, &DisplayArea);

    ShowWindow(hWnd, SW_MAXIMIZE);
}

void setConsoleSize(int size)
{
    CONSOLE_FONT_INFOEX cfi{};
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 0;
    cfi.dwFontSize.Y = size;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, L"Consolas");
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
}

void exit_program()
{
    SetConsoleTitleA(title.c_str());

    system("cls");
    setConsoleSize(16);

    cout << "\nProgram executed successfully. Press any key to exit..." << endl;

    getch();

    exit(0);
}

void importFrames(Mat image, int frameNumber, string parentDirName)
{
    if (frameNumber == 1)
    {
        fs::path pathToGo = fs::current_path();

        pathToGo += "\\";
        pathToGo += parentDirName;
        pathToGo += "\\";

        if (!fs::exists(parentDirName))
        {
            fs::create_directory(parentDirName);
            fs::current_path(pathToGo);
        }
        else
        {
            fs::remove_all(pathToGo);
            fs::create_directory(parentDirName);
            fs::current_path(pathToGo);
        }

        pathToGo += "frames\\";

        if (!fs::exists("frames"))
        {
            fs::create_directory("frames");
            fs::current_path(pathToGo);
        }
        else
        {
            fs::remove_all(pathToGo);
            fs::create_directory("frames");
            fs::current_path(pathToGo);
        }
    }

    Mat imageGray;

    cvtColor(image, imageGray, COLOR_RGB2GRAY);

    string frameName = "frame" + to_string(frameNumber) + ".png";

    imwrite(frameName, imageGray);
}

void loadFrames(Mat image, int frameNumber, int size)
{
    if (frameNumber == 1)
    {
        fs::path pathToGo = fs::current_path();
        pathToGo += "\\text\\";

        if (!fs::exists("text"))
        {
            fs::create_directory("text");
            fs::current_path(pathToGo);
        }
        else
        {
            fs::remove_all(pathToGo);
            fs::create_directory("text");
            fs::current_path(pathToGo);
        }
    }

    string finalDisplay = "";

    Mat imageResized;

    maximizeWindow();

    setConsoleSize(size);

    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    int w_width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int w_height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    double height_ratio, width_ratio;

    height_ratio = (double)w_height / image.size().height;

    width_ratio = (double)w_width / image.size().width;

    resize(image, imageResized, Size(), width_ratio, height_ratio);

    for (int y = 0; y < imageResized.rows; y++)
    {
        for (int x = 0; x < imageResized.cols; x++)
        {
            Vec3b color = imageResized.at<Vec3b>(Point(x, y));

            float colorFloat = color[0];

            if (colorFloat >= 200)
            {
                colorFloat += 35;
                if (colorFloat >= 255)
                {
                    colorFloat = 255;
                }
            }
            else if (colorFloat <= 120)
            {
                colorFloat -= 40;

                if (colorFloat <= 0)
                {
                    colorFloat = 0;
                }
            }

            finalDisplay += ASCII_CHARS[(255 - int(floor(colorFloat))) * (strlen(ASCII_CHARS) - 1) / 256];
        }

        finalDisplay += "\n";
    }

    finalDisplay.resize(finalDisplay.size() - 1);

    string fileName = "txt" + to_string(frameNumber) + ".txt";

    ofstream outfile(fileName);
    outfile << finalDisplay;
    outfile.close();

    finalDisplay.clear();
}

void playAudio(string videoName, fs::path pathToGo)
{
    pathToGo += "\\videos\\audio\\";

    fs::current_path(pathToGo);

    string pathToAudio = fs::current_path().string();

    videoName += ".wav";

    if (fs::exists(pathToAudio))
    {
        fs::current_path(pathToAudio);
        // string lmao = "open \"" + videoName + "\" type MPEGVideo alias mp3";
        // mciSendStringA(lmao.c_str(), NULL, 0, NULL);
        // mciSendStringA("play mp3", NULL, 0, NULL);
        PlaySoundA(videoName.c_str(), NULL, SND_ASYNC);
    }
}

void displayFrame(string videoName, int numberOfFrames, string textPath, const int frameRate, int size, bool exists)
{
    string framePath;
    string veryFinalDisplay;
    string musicName;

    fs::path pathToGo;

    if (!exists)
    {
        pathToGo = fs::current_path().parent_path().parent_path();
    }
    else
    {
        pathToGo = fs::current_path();
    }

    system("color F0");

    showCursor(false);

    cout << "Launching animation..." << endl;
    this_thread::sleep_for(chrono::seconds(generate_random_number(rng)) + 100ms);

    setConsoleSize(size);
    maximizeWindow();

    system("cls");

    this_thread::sleep_for(chrono::milliseconds(generate_random_number(rng) * 200));

    std::thread play_audio(playAudio, videoName, pathToGo);
    // playAudio(videoName, pathToGo);
    play_audio.join();

    int FPS = frameRate;

    using dsec = chrono::duration<double>;
    auto invFpsLimit = round<chrono::system_clock::duration>(dsec{1. / FPS});
    auto m_BeginFrame = chrono::system_clock::now();
    auto m_EndFrame = m_BeginFrame + invFpsLimit;
    unsigned frame_count_per_second = 0;
    auto prev_time_in_seconds = chrono::time_point_cast<chrono::seconds>(m_BeginFrame);

    string fpsTitle = " - " + to_string(frame_count_per_second) + " FPS";
    SetConsoleTitleA((title + fpsTitle).c_str());

    for (int t = 1; t <= numberOfFrames; t++)
    {
        framePath = textPath + to_string(t) + ".txt";

        ifstream stream(framePath);

        stringstream strStream;

        strStream << stream.rdbuf();

        stream.close();

        veryFinalDisplay = strStream.str();

        printf("\r");
        printf("%s", veryFinalDisplay.c_str());
        fflush(stdout);

        auto time_in_seconds = chrono::time_point_cast<chrono::seconds>(chrono::system_clock::now());

        frame_count_per_second++;

        if (time_in_seconds > prev_time_in_seconds)
        {
            // std::cerr << frame_count_per_second << " frames per second\n";
            fpsTitle = " | " + to_string(frame_count_per_second) + " FPS";
            SetConsoleTitleA((title + fpsTitle).c_str());
            frame_count_per_second = 0;
            prev_time_in_seconds = time_in_seconds;
        }

        std::this_thread::sleep_until(m_EndFrame);
        m_BeginFrame = m_EndFrame;
        m_EndFrame = m_BeginFrame + invFpsLimit;

        clearScreen();
    }
}

void processVideo(VideoCapture cap, string videoName, int frameRate, int totalFrames)
{
    char importFrames_1;
    int size = 8;
    char lmao;

    string frameRateInput;

    //// fs::current_path(fs::current_path().parent_path());

    fs::path cur_path = fs::current_path();

    cur_path += "\\data\\";
    cur_path += videoName;
    cur_path += "\\frames\\";

    fs::path curpath = fs::current_path();

    curpath += "\\data\\";
    curpath += videoName;
    curpath += "\\text\\";

    fs::path cur_path_2 = fs::current_path();

    cur_path_2 += "\\data\\";
    cur_path_2 += videoName;
    cur_path_2 += "\\size.txt";

    if (fs::exists(curpath))
    {
        showCursor(true);
        // std::cout << "Seems like you already have the text files to display. Do you want to re-import them? [Y/N] ";
        do
        {
            cout << "Seems like you already have the text files to display. Do you want to re-import them? [Y/N] ";
            cin >> lmao;
        } while (!cin.fail() && toupper(lmao) != 'Y' && toupper(lmao) != 'N');

        if (toupper(lmao) == 'N')
        {
            string size_1;
            ifstream the_size(cur_path_2.string());
            getline(the_size, size_1);
            size = std::stoi(size_1);
            curpath += "\\txt";
            string curpath_str = curpath.string();
            std::replace(curpath_str.begin(), curpath_str.end(), '\\', '/');
            displayFrame(videoName, totalFrames, curpath_str, frameRate, size, true);
            exit_program();
        }
    }

    if (fs::exists(cur_path))
    {
        showCursor(true);
        // std::cout << "\nSeems like you already have enough frames for converting. Do you want to re-import them? [Y/N] ";
        do
        {
            cout << "Seems like you already have enough frames for converting. Do you want to re-import them? [Y/N] ";
            cin >> importFrames_1;
        } while (!cin.fail() && toupper(importFrames_1) != 'Y' && toupper(importFrames_1) != 'N');
    }
    else
    {
        importFrames_1 = 'y';
    }

    bool framesImported = false;
    int frameNumber = 1;

    Mat frame;
    string progressBar;

    cur_path = fs::current_path();

    cur_path += "\\data\\";

    fs::current_path(cur_path);

    if (toupper(importFrames_1) == 'Y')
    {
        showCursor(false);

        cout << "Importing frames..." << endl;

        float progress = 0.0;

        auto _begin = chrono::steady_clock().now();

        while (progress < 1.0)
        {
            int barWidth = 50;

            cap >> frame;

            if (frame.empty())
            {
                break;
            }

            importFrames(frame, frameNumber, videoName);

            // if (frameNumber == 1)
            // {
            //     system("cls");
            // }

            // printf("[");
            // printf("\rProgress:\n[");

            chrono::seconds time_taken = chrono::duration_cast<chrono::seconds>(chrono::steady_clock().now() - _begin);

            cout << "Progress: [";
            int pos = barWidth * progress;
            for (int i = 0; i < barWidth; ++i)
            {
                if (i < pos)
                    // printf("=");
                    cout << "=";
                else if (i == pos)
                    // printf(">");
                    cout << ">";
                else
                    // printf(" ");
                    cout << " ";
            }
            // std::cout << "] " << int(progress * 100.0) << "%\r";
            // printf("] %d%% - Frame %d of %d\r", int(progress * 100.0), frameNumber, totalFrames);
            cout << "] " << int(progress * 100.0) + 1 << "% - Frame " << frameNumber << " of " << totalFrames << " - " << elapsedTime(time_taken) << " elapsed\r";
            cout.flush();

            // this_thread::sleep_for(60ns);

            progress += (1.0 / totalFrames);

            frameNumber++;
        }

        // printf("\rProgress: Frame %d of %d\n%s", frameNumber, totalFrames, progressBar.c_str());

        std::cout << endl
                  << "Done" << endl;

        cap.release();

        framesImported = true;
    }

    char createTextFiles;

    frameNumber = 1;
    progressBar = "";

    Mat currentFrame;
    string imagePathStr;
    fs::path videoPath;

    if (framesImported)
    {
        videoPath = fs::current_path().parent_path();
        videoPath += "\\";
    }
    else
    {
        videoPath = fs::current_path();
        videoPath += "\\";
        videoPath += videoName;
        videoPath += "\\";
    }

    fs::current_path(videoPath);

    // // if (fs::exists("text"))
    // // {
    // //     showCursor(true);
    // //     // std::cout << "Seems like you already have the text files to display. Do you want to re-import them? [Y/N] ";
    // //     printf("Seems like you already have the text files to display. Do you want to re-import them? [Y/N] ");
    // //     cin >> createTextFiles;
    // //     do {
    // //         printf("Seems like you already have the text files to display. Do you want to re-import them? [Y/N] ");
    // //         cin >> createTextFiles;
    // //     } while (!cin.fail() && toupper(lmao) != 'Y' && toupper(lmao) != 'N');
    // // }
    // // else
    // // {
    // //     createTextFiles = 'y';
    // // }

    bool wroteCurrentFile = FALSE;

    char changeSize;

    showCursor(true);

    do
    {
        cout << "Do you want to change the font size? (The default font size is 8) [Y/N] ";
        cin >> changeSize;
    } while (!cin.fail() && toupper(changeSize) != 'Y' && toupper(changeSize) != 'N');

    if (toupper(changeSize) == 'Y')
    {
        get_int(size, "Please enter the size (The lower it is, the better the result will be. I recommend not going lower than 8, as this could cause lag or desynchronization): ", "Sorry, that's not an integer.\n");
    }

    showCursor(false);

    cout << "Creating text files..." << endl;

    this_thread::sleep_for(500ms);

    float text_progress = 0.0;

    auto _begin = chrono::steady_clock().now();

    int barWidth = 50;

    for (int n = 1; n <= totalFrames; n++)
    {
        fs::path imagePath = fs::current_path();

        imagePathStr = imagePath.string();

        if (n != 1)
        {
            imagePathStr.resize(imagePathStr.size() - 5);
        }

        imagePathStr += "\\frames\\frame";
        imagePathStr += to_string(n);
        imagePathStr += ".png";

        std::replace(imagePathStr.begin(), imagePathStr.end(), '\\', '/');

        currentFrame = imread(imagePathStr);

        if (currentFrame.empty())
        {
            std::cout << imagePathStr << " - Could not load image"
                      << "\n";
        }
        else
        {
            loadFrames(currentFrame, frameNumber, size);
            // std::thread load_frame(loadFrames, currentFrame, frameNumber, size);
        }

        chrono::seconds time_taken = chrono::duration_cast<chrono::seconds>(chrono::steady_clock().now() - _begin);

        cout << "Progress: [";
        int pos = barWidth * text_progress;
        for (int i = 0; i < barWidth; ++i)
        {
            if (i < pos)
                // printf("=");
                cout << "=";
            else if (i == pos)
                // printf(">");
                cout << ">";
            else
                // printf(" ");
                cout << " ";
        }
        // std::cout << "] " << int(progress * 100.0) << "%\r";
        // printf("] %d%% - Frame %d of %d\r", int(progress * 100.0), frameNumber, totalFrames);
        cout << "] " << int(text_progress * 100.0) + 1 << "% - Frame " << frameNumber + 1 << " of " << totalFrames << " - " << elapsedTime(time_taken) << " elapsed\r";
        cout.flush();

        // this_thread::sleep_for(chrono::nanoseconds(generate_random_number(rng)));

        if (frameNumber == totalFrames)
        {
            break;
        }

        text_progress += (1.0 / totalFrames);
        frameNumber++;
    }

    fs::current_path(fs::current_path().parent_path());
    wroteCurrentFile = TRUE;

    cout << endl
         << "Done" << endl;

    fs::path sizePath = fs::current_path();
    sizePath += "\\size.txt";
    int sizeInFile;

    if (fs::exists(sizePath))
    {
        ifstream streamSize(sizePath);
        stringstream sizeStream;
        sizeStream << streamSize.rdbuf();
        sizeInFile = stoi(sizeStream.str());
        streamSize.close();

        if (wroteCurrentFile)
        {
            fs::remove_all(sizePath);
            ofstream outfileText("size.txt");
            outfileText << size;
            outfileText.close();
        }
        else
        {
            size = sizeInFile;
        }
    }
    else
    {
        ofstream outfileText("size.txt");
        outfileText << size;
        outfileText.close();
    }

    fs::path textPath = fs::current_path();
    string textPathStr = textPath.string();
    textPathStr += "\\text\\txt";
    std::replace(textPathStr.begin(), textPathStr.end(), '\\', '/');

    cout << "size.txt created." << endl;

    setConsoleSize(16);

    char yes_or_no;

    showCursor(true);

    do
    {
        cout << "Do you want to keep the frame images? [Y/N] ";
        cin >> yes_or_no;
    } while (!cin.fail() && toupper(yes_or_no) != 'Y' && toupper(yes_or_no) != 'N');

    if (toupper(yes_or_no) == 'N')
    {
        fs::path daPath = fs::current_path();

        daPath += "\\frames\\";

        fs::remove_all(daPath);

        cout << "Frame images removed." << endl;
    }

    cout << "All files imported." << endl;

    // displayFrame(videoName, totalFrames, textPathStr, frameRate, size, false);
    std::thread display_frame(displayFrame, videoName, totalFrames, textPathStr, frameRate, size, false);
    display_frame.join();
}

int main()
{
    std::ios_base::sync_with_stdio(false);

    SetConsoleTitleA(title.c_str());

    if (!fs::exists("data"))
    {
        fs::create_directory("data");
    }

    if (!fs::exists("videos"))
    {
        fs::create_directory("videos");
    }

    char buffer[8192];

    setvbuf(stdout, buffer, _IOFBF, sizeof(buffer));

    string videoName;

    system("cls");

    std::cout << "\nEnter the video's name.\n> ";

    std::getline(std::cin, videoName);

    videoName += ".mp4";

    fs::path cur_path = fs::current_path();

    cur_path += "\\videos\\";
    cur_path += videoName;

    string cleanVideoName = videoName;
    cleanVideoName.resize(cleanVideoName.size() - 4);

    if (cleanVideoName.find("http") != std::string::npos)
    {
        showCursor(false);
        cout << "\nURL found. Trying to get it from YouTube..." << endl;
        std::ostringstream ss;
        ss << "youtube-dl --get-title \"" << cleanVideoName << "\""
           << " > temp.txt";
        std::string str = ss.str();
        system(str.c_str());
        ifstream get_url("temp.txt");
        string title = "";
        getline(get_url, videoName);
        get_url.close();
        fs::remove("temp.txt");
        cout << "Downloading video..." << endl;
        //// printf(finalURL.c_str());
        fs::path videoPath = fs::current_path();
        videoPath += "\\videos\\";
        string temp = "";
        //// printf(videoPath.string().c_str());
        for (int i = 0; i < videoName.size(); ++i)
        {
            if ((videoName[i] >= 'a' && videoName[i] <= 'z') || (videoName[i] >= 'A' && videoName[i] <= 'Z'))
            {
                temp = temp + videoName[i];
            }
        }
        videoName = temp;
        if (system(("youtube-dl -f best -o \"" + videoPath.string() + videoName + ".mp4\" " + cleanVideoName).c_str()) == 1)
        {
            cout << "Could not download the video. Press any key to exit..." << endl;
            getch();
            return 1;
        }
        cout << "Video downloaded." << endl;
        // printf("\nEnter new name for the video: ");
        videoName += ".mp4";
    }
    else
    {
        if (!fs::exists(cur_path))
        {
            showCursor(false);
            cout << "\nCould not find the video in the videos folder. Trying to get it from YouTube..." << endl;
            // // std::ostringstream ss;
            // // ss << "youtube-dl --get-id \"ytsearch:" << cleanVideoName << "\""
            // //    << " > \"" << cleanVideoName << ".txt\"";
            // // std::string str = ss.str();
            // // system(str.c_str());
            // // string finalURL;
            // // string name = cleanVideoName + ".txt";
            // // ifstream get_url(name);
            // // getline(get_url, finalURL);
            // // get_url.close();
            // // fs::remove(name);
            cout << "Downloading video..." << endl;
            //// printf(finalURL.c_str());
            fs::path videoPath = fs::current_path();
            videoPath += "\\videos\\";
            //// printf(videoPath.string().c_str());
            if (system(("youtube-dl -f best -o \"" + videoPath.string() + cleanVideoName + ".mp4\" " + "\"ytsearch:" + cleanVideoName + "\"").c_str()) == 1)
            {
                cout << "\nCould not download the video. Press any key to exit..." << endl;
                getch();
                return 1;
            }
            cout << "Video downloaded." << endl;
        }
    }

    fs::path cur_path_1 = fs::current_path();

    cur_path_1 += "\\videos\\";

    fs::current_path(cur_path_1);

    VideoCapture cap(videoName);

    system("cls");

    if (cap.isOpened())
    {
        cout << "\nVideo Properties:\n"
             << endl;
    }

    int frameRate = cap.get(CAP_PROP_FPS);
    int totalFrames = cap.get(CAP_PROP_FRAME_COUNT);

    cout << "Name: " << videoName << endl;
    cout << "FPS: " << frameRate << endl;
    cout << "Total frames: " << totalFrames << endl;

    cout << "\nExtracting audio..." << endl;

    string input = "ffmpeg";

    input += " -loglevel quiet -y -i \"";
    input += videoName;
    input += "\" -vn";
    input += " -acodec copy \"";
    input += cleanVideoName;
    input += ".aac\"";

    if (system(input.c_str()) == 1)
    {
        cout << "Could not convert the video to aac. Please make sure you have ffmpeg installed and added to PATH." << endl;
        getch();
        return 1;
    }

    input = "ffmpeg";
    input += " -loglevel quiet -y -i \"";
    input += cleanVideoName;
    input += ".aac\"";
    input += " -vn -ar 44100 -ac 2 -b:a 192k \"";
    input += cleanVideoName;
    input += ".wav\"";

    if (system(input.c_str()) == 1)
    {
        cout << "Could not convert the audio file to wav. Please make sure you have ffmpeg installed and added to PATH." << endl;
        getch();
        return 1;
    }

    fs::rename(cleanVideoName + ".wav", "audio\\" + cleanVideoName + ".wav");
    fs::remove(cleanVideoName + ".aac");

    fs::current_path(fs::current_path().parent_path());

    cout << "Audio extracted." << endl;

    processVideo(cap, cleanVideoName, frameRate, totalFrames);

    exit_program();
}