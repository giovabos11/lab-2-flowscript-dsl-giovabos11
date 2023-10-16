#include "compilejob.h"
#include <iostream>
#include <array>

void CompileJob::Execute()
{
    std::array<char, 128> buffer;

    // Get project name
    projectName = command.substr(command.find_last_of(' ') + 1, command.length());

    // Redirect cerr to cout
    command.append(" 2>&1");

    // Open pile and run command
#ifdef __linux__
    FILE *pipe = popen(command.c_str(), "r");
#elif _WIN32
    FILE *pipe = _popen(command.c_str(), "r");
#else
    FILE *pipe = popen(command.c_str(), "r");
#endif

    if (!pipe)
    {
        std::cout << "popen Failed: Failed to open pipe" << std::endl;
        return;
    }

    // Read until the end of the process
    while (fgets(buffer.data(), 128, pipe) != NULL)
    {
        this->output.append(buffer.data());
    }

    // Close pipe and get the return code
#ifdef __linux__
    this->returnCode = pclose(pipe);
#elif _WIN32
    this->returnCode = _pclose(pipe);
#else
    this->returnCode = pclose(pipe);
#endif

    // Parse JSON output
    parseFile();

    std::cout << "Job " << this->GetUniqueID() << " Has Been Executed" << std::endl;
}

void CompileJob::parseFile()
{
    // Split the different errors in separete json objects to parse
    int startIndex = 0, endIndex = 0;
    std::string temp;
    for (int i = 0; i <= this->output.size(); i++)
    {
        if (this->output[i] == '\n' || i == this->output.size())
        {
            endIndex = i;
            temp = "";
            temp.append(this->output, startIndex, endIndex - startIndex);
            generateJson(temp);
            startIndex = endIndex + 1;
        }
    }

    std::cout << "Compile Job ID: " << this->GetUniqueID() << std::endl;
    std::cout << "Return Code: " << this->returnCode << std::endl;
    std::cout << "Output:" << std::endl
              << this->output << std::endl;

    // Write to file
    std::ofstream o("C:\\Users\\giova_pwwkjqa\\OneDrive\\Escritorio\\SMU\\lab-2-flowscript-dsl-giovabos11/Data/output_" + projectName + ".json"); // std::ofstream o("../Data/output_" + projectName + ".json");
    o << std::setw(4) << outputJson << std::endl;
    o.close();
}

void CompileJob::generateJson(std::string &str)
{
    // If no errors or the string is empty, exit
    if (str.size() == 0 || str == "[]")
        return;

    // Parse file
    json data;
    try
    {
        data = json::parse(str);
    }
    catch (const std::exception &e)
    {
        std::cout << "Error parsing the console output: Invalid json format" << std::endl;
        return;
    }

    std::string sourceFilename, kind, message;
    int lineNumber, columnNumber;

    // Go through every error/warning
    for (int i = 0; i < data.size(); i++)
    {
        // Get either error or warning
        kind = data[i]["kind"];
        // Ignore everithing that is not a warning or error
        if (kind != "error" && kind != "warning" && kind != "note")
            continue;
        // std::cout << data[i]["kind"] << std::endl;

        // Get message
        message = data[i]["message"];
        // std::cout << message << std::endl;

        // Get main error/warning source line number (first caret) in the file
        lineNumber = data[i]["locations"][0]["caret"]["line"];
        // std::cout << lineNumber << std::endl;

        // Get main error/warning source line number (first caret) in the file
        columnNumber = data[i]["locations"][0]["caret"]["display-column"];
        // std::cout << columnNumber << std::endl;

        // Get file name
        sourceFilename = data[i]["locations"][0]["caret"]["file"];
        // std::cout << sourceFilename << std::endl;

        // Get 2 lines above and bellow if possible (code snippet)
        std::ifstream sourceFile(sourceFilename);
        int currentLineNumber = 1;
        std::string currrentLine = "", snippet = ""; // MAKE ONE LINER
        int codeIndex = 0;
        while (!sourceFile.eof() && !sourceFile.fail())
        {
            getline(sourceFile, currrentLine);
            if (currentLineNumber == lineNumber - 2 ||
                currentLineNumber == lineNumber - 1 ||
                currentLineNumber == lineNumber ||
                currentLineNumber == lineNumber + 1 ||
                currentLineNumber == lineNumber + 2)
                snippet += currrentLine + "\n";

            currentLineNumber++;
        }
        sourceFile.close();

        // Build json object
        outputJson[sourceFilename] += {{"file", sourceFilename}, {kind, message}, {"line", lineNumber}, {"column", columnNumber}, {"code", snippet}};
    }
}

void CompileJob::JobCompleteCallback()
{
    std::cout << "Job " << this->GetUniqueID() << " retired" << std::endl;
}