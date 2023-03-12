
// Copyright (c) 23, Haonan
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
//     * Neither the name of Haonan nor the names of
//       its contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Author: Haonan Dong

#include <iostream>
#include <vector>
#include <functional>

#include "base/log.h"
#include "calibration/calibration_base.h"
#include "calibration/intrinsic_calibration.h"

typedef std::function<int(int, char **)> command_func_t;

int ShowHelp(const std::vector<std::pair<std::string, command_func_t>> &commands)
{

    std::cout << "fishcat -- Fisheye Calibration and Application Toolbox"
              << std::endl
              << std::endl;

    std::cout << "Usage:" << std::endl;
    std::cout << "  fishcat [command] [options]" << std::endl
              << std::endl;

    std::cout << "Documentation:" << std::endl;
    std::cout << "https://haonan-dong.github.io/" << std::endl
              << std::endl;

    std::cout
        << "Example usage:" << std::endl;
    std::cout << "  fishcat help [ -h, --help ]" << std::endl;
    std::cout << "  fishcat intrinsic_calibration" << std::endl;

    std::cout << "Available commands:" << std::endl;
    std::cout << "  help" << std::endl;
    for (const auto &command : commands)
    {
        std::cout << "  " << command.first << std::endl;
    }
    std::cout << std::endl;

    return EXIT_SUCCESS;
}

int RunIntrinsicCalibration(int argc, char **argv)
{
    fishcat::IntrinsicCalibrationHelp();
    fishcat::CalibrationSettings s;
    const std::string input_settings_file = argc > 1 ? argv[1] : "test.xml";
    cv::FileStorage fs(input_settings_file, cv::FileStorage::READ); // Read the CalibrationSettings

    if (!fs.isOpened())
    {
        LOG(ERROR) << "Could not open the configuration file: \""
                   << input_settings_file
                   << "\""
                   << std::endl;
        return -1;
    }

    // from the setting in the xml file.
    fs["Settings"] >> s;
    fs.release(); // close CalibrationSettings file

    if (!s.good_input_)
    {
        LOG(ERROR) << "Invalid input detected. Application stopping. "
                   << std::endl;
        return -1;
    }

    return true;
}

int RunPanoramicStitching(int argc, char **argv)
{
    return true;
}

int main(int argc, char **argv)
{
    InitialGoogleLog(argv);

    std::vector<std::pair<std::string, command_func_t>> commands;

    commands.emplace_back("intrinsic_calibration", &RunIntrinsicCalibration);
    commands.emplace_back("panoramic_stitching", &RunPanoramicStitching);

    if (argc == 1)
    {
        return ShowHelp(commands);
    }

    const std::string command = argv[1];
    if (command == "help" || command == "-h" || command == "--help")
    {
        return ShowHelp(commands);
    }
    else
    {
        command_func_t matched_command_func = nullptr;
        for (const auto &command_func : commands)
        {
            if (command == command_func.first)
            {
                matched_command_func = command_func.second;
                break;
            }
        }
        if (matched_command_func == nullptr)
        {
            std::cerr << "ERROR: Command not recognized. To list the "
                         "available commands, run `spreads help`."
                      << std::endl;
            return EXIT_FAILURE;
        }
        else
        {
            int command_argc = argc - 1;
            char **command_argv = &argv[1];
            command_argv[0] = argv[0];
            return matched_command_func(command_argc, command_argv);
        }
    }

    return ShowHelp(commands);
}