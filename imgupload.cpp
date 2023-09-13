// everything is in the _buffer variable.

#include "Includes/TestServer.hpp"

void splitString(const std::string &input, std::vector<std::string> &output, const char *delimiter) {
	char *token = strtok(const_cast<char *>(input.c_str()), delimiter);
	while (token != nullptr) {
		output.push_back(token);
		token = strtok(nullptr, delimiter);
	}
}

// std::string findBoundary(const std::vector<std::string> &tab)
// {
// 	for (std::vector<std::string>::iterator i = tab.begin(); i != tab.end(); ++i)
// 	{
// 		//char * strstr (       char * str1, const char * str2 );
// 		if (strstr(i, "boundary="))
// 		{
// 			std::cout << i << std::endl;
// 		}
// 	}
// 	return NULL;
// }

int main(void)
{
	std::string buffer = "POST /upload_images HTTP/1.1\r\nHost: example.com\r\nContent-Type: multipart/form-data; boundary=---------------------------1234567890\r\n\r\n-----------------------------1234567890\r\nContent-Disposition: form-data: name=\"image1\"; filename=\"image1.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n[Binary data for image1.jpg goes here]\r\n-----------------------------1234567890\r\nContent-Disposition: form-data: name=\"image2\"; filename=\"image2.png\"\r\nContent-Type: image/png\r\n\r\n[Binary data for image2.png goes here]\r\n-----------------------------1234567890--\r\n";
	std::cout << buffer << std::endl;
	std::vector<std::string> tab;
	splitString(buffer, tab, "\r\n");
	std::cout << "===================== Entering Parsing =====================\n\n\n" << std::endl; 

	for (std::vector<std::string>::iterator i = tab.begin(); i != tab.end(); ++i) {
		std::cout << "===================== NEW BLOCK =====================\n" << std::endl;
		std::cout << *i << "\n\n\n" << std::endl;
	}

	for (std::vector<std::string>::iterator i = tab.begin(); i != tab.end(); ++i)
	{
		if (strstr(*i, "boundary="))
		{
			std::cout << *i << std::endl;
		}
	}
	//std::string delimiter = findBoundary(tab);
	//splitBlock(tab, delimiter);
	return 0;
}

/*
void splitString(const std::string& input, std::vector<std::string>& output, const char* delimiter) {
    char* token = strtok(const_cast<char*>(input.c_str()), delimiter);
    while (token != nullptr) {
        output.push_back(token);
        token = strtok(nullptr, delimiter);
    }
}

int main() {
    std::string buffer = "POST /upload_images HTTP/1.1\r\nHost: example.com\r\nContent-Type: multipart/form-data; boundary=---------------------------1234567890\r\n\r\n-----------------------------1234567890\r\nContent-Disposition: form-data: name=\"image1\"; filename=\"image1.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n[Binary data for image1.jpg goes here]\r\n-----------------------------1234567890\r\nContent-Disposition: form-data: name=\"image2\"; filename=\"image2.png\"\r\nContent-Type: image/png\r\n\r\n[Binary data for image2.png goes here]\r\n-----------------------------1234567890--\r\n";

    std::vector<std::string> lines;
    splitString(buffer, lines, "\r\n");

    std::string boundary = "---------------------------1234567890";

    // Find and process body blocks
    for (size_t i = 0; i < lines.size(); ++i) {
        if (lines[i] == boundary) {
            // Start of a new block
            size_t blockStart = i + 1;

            // Find the end of the block
            size_t blockEnd = blockStart;
            while (blockEnd < lines.size() && lines[blockEnd] != boundary) {
                ++blockEnd;
            }

            // Process the block (from blockStart to blockEnd)
            for (size_t j = blockStart; j < blockEnd; ++j) {
                // Here, you can process each line of the block
                std::cout << "Block Line: " << lines[j] << std::endl;
            }

            // Set the current position to the end of this block
            i = blockEnd - 1;
        }
    }

    return 0;
}





#include <iostream>
#include <cstring>
#include <vector>

void splitString(const std::string& input, std::vector<std::string>& output, const char* delimiter) {
    char* token = strtok(const_cast<char*>(input.c_str()), delimiter);
    while (token != nullptr) {
        output.push_back(token);
        token = strtok(nullptr, delimiter);
    }
}

int main() {
    std::string buffer = "POST /upload_images HTTP/1.1\r\nHost: example.com\r\nContent-Type: multipart/form-data; boundary=---------------------------1234567890\r\n\r\n-----------------------------1234567890\r\nContent-Disposition: form-data: name=\"image1\"; filename=\"image1.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n[Binary data for image1.jpg goes here]\r\n-----------------------------1234567890\r\nContent-Disposition: form-data: name=\"image2\"; filename=\"image2.png\"\r\nContent-Type: image/png\r\n\r\n[Binary data for image2.png goes here]\r\n-----------------------------1234567890--\r\n";

    std::vector<std::string> tab;
    splitString(buffer, tab, "\r\n");

    for (std::vector<std::string>::iterator i = tab.begin(); i != tab.end(); ++i) {
        std::cout << *i << std::endl;
    }
	delimiter = findBoundary(tab);
	splitBlock(tab, delimiter);

    return 0;
}
*/

//add to obsidian : 
//https://www.tutorialspoint.com/http/http_requests.htm
//https://cplusplus.com/reference/cstring/
